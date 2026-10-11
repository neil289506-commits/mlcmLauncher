// SPDX-License-Identifier: GPL-3.0-only
/*
 *  mlcmLauncher
 *  One-way "sync" from an existing Prism Launcher data folder into this launcher.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "PrismSync.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>

#include "Application.h"
#include "BuildConfig.h"
#include "DataMigrationTask.h"
#include "FileSystem.h"
#include "Filter.h"
#include "InstanceList.h"
#include "ui/dialogs/ProgressDialog.h"

namespace {

struct NewInstance {
    QString id;
    QString name;
};

QString title()
{
    return QObject::tr("Sync with Prism Launcher");
}

// The instance folder configured in Prism Launcher (relative paths are relative to its data folder).
QString sourceInstanceDir(const QString& sourceData)
{
    QString dir = QStringLiteral("instances");

    const QString cfgPath = FS::PathCombine(sourceData, "prismlauncher.cfg");
    if (QFileInfo::exists(cfgPath)) {
        QSettings cfg(cfgPath, QSettings::IniFormat);
        const QString configured = cfg.value("InstanceDir").toString();
        if (!configured.isEmpty())
            dir = configured;
    }

    return QDir::isAbsolutePath(dir) ? QDir::cleanPath(dir) : QDir::cleanPath(FS::PathCombine(sourceData, dir));
}

// instance id -> group name, as stored by Prism Launcher in instgroups.json
QHash<QString, QString> sourceGroups(const QString& sourceData)
{
    QHash<QString, QString> result;

    QFile file(FS::PathCombine(sourceData, "instgroups.json"));
    if (!file.open(QIODevice::ReadOnly))
        return result;

    const QJsonObject groups = QJsonDocument::fromJson(file.readAll()).object().value("groups").toObject();
    for (auto iter = groups.begin(); iter != groups.end(); ++iter) {
        if (iter.key().isEmpty())
            continue;
        const QJsonArray ids = iter.value().toObject().value("instances").toArray();
        for (const QJsonValue& id : ids)
            result.insert(id.toString(), iter.key());
    }
    return result;
}

bool copyWithProgress(QWidget* parent, const QString& from, const QString& to, const Filter& filter)
{
    ProgressDialog dialog(parent);
    DataMigrationTask task(from, to, filter);
    if (dialog.execWithTask(&task) != 0)
        return true;

    QMessageBox::critical(parent, title(), QObject::tr("Sync failed! Reason: %1").arg(task.failReason()));
    return false;
}

}  // namespace

namespace PrismSync {

QString defaultSourceDir()
{
    // <appdata>/<org>/<app>/../.. is the folder that holds the data folders of all launchers
    return QDir::cleanPath(FS::PathCombine(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "../../PrismLauncher"));
}

bool isValidSource(const QString& dir)
{
    if (dir.isEmpty())
        return false;
    return QFileInfo::exists(FS::PathCombine(dir, "prismlauncher.cfg")) || QFileInfo(FS::PathCombine(dir, "instances")).isDir();
}

void run(QWidget* parent)
{
    auto* instances = APPLICATION->instances();
    if (!instances)
        return;

    // 1. find the Prism Launcher data folder
    QString source = defaultSourceDir();
    if (!isValidSource(source)) {
        QMessageBox::information(parent, title(),
                                 QObject::tr("The Prism Launcher data folder was not found automatically.\n\n"
                                             "Please select it (the folder that contains prismlauncher.cfg)."));
        source = QFileDialog::getExistingDirectory(parent, QObject::tr("Select the Prism Launcher data folder"), QDir::homePath());
        if (source.isEmpty())
            return;
        source = QDir::cleanPath(source);
        if (!isValidSource(source)) {
            QMessageBox::warning(parent, title(), QObject::tr("This folder does not look like Prism Launcher data."));
            return;
        }
    }

    if (QDir(source) == QDir(APPLICATION->dataRoot())) {
        QMessageBox::information(parent, title(), QObject::tr("The selected folder is this launcher's own data folder."));
        return;
    }

    // 2. see what is new
    const QString sourceInstances = sourceInstanceDir(source);
    const QString targetInstances = instances->primaryDir();

    QList<NewInstance> fresh;
    int alreadyHere = 0;
    const auto entries = QDir(sourceInstances).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo& entry : entries) {
        const QString id = entry.fileName();
        const QString cfgPath = FS::PathCombine(entry.absoluteFilePath(), "instance.cfg");
        if (id.startsWith('.') || id.startsWith('_') || !QFileInfo::exists(cfgPath))
            continue;  // hidden / temporary folders and anything that is not an instance

        if (instances->getInstanceById(id) || QFileInfo::exists(FS::PathCombine(targetInstances, id))) {
            ++alreadyHere;
            continue;
        }

        QSettings cfg(cfgPath, QSettings::IniFormat);
        const QString name = cfg.value("name").toString();
        fresh.append({ id, name.isEmpty() ? id : name });
    }

    if (fresh.isEmpty()) {
        QMessageBox::information(parent, title(),
                                 QObject::tr("Everything is already in sync.\n\n%1 instance(s) in Prism Launcher are already present here.")
                                     .arg(alreadyHere));
        return;
    }

    // 3. ask
    QStringList names;
    for (const auto& instance : fresh)
        names.append(instance.name);

    QMessageBox box(QMessageBox::Question, title(),
                    QObject::tr("%1 new instance(s) were found in Prism Launcher.\n\n"
                                "Copy them into %2?\n\n"
                                "Prism Launcher's data is never modified, and instances that already exist here are never overwritten.")
                        .arg(fresh.size())
                        .arg(BuildConfig.LAUNCHER_DISPLAYNAME),
                    QMessageBox::NoButton, parent);
    box.setDetailedText(names.join('\n'));
    auto* syncButton = box.addButton(QObject::tr("Sync"), QMessageBox::AcceptRole);
    box.addButton(QMessageBox::Cancel);
    box.exec();
    if (box.clickedButton() != syncButton)
        return;

    // 4. copy the new instances (only those folders, so existing instances are never touched)
    QList<Filter> instanceFilters;
    for (const auto& instance : fresh)
        instanceFilters.append(Filters::startsWith(instance.id + "/"));

    if (!copyWithProgress(parent, sourceInstances, targetInstances, Filters::any(std::move(instanceFilters))))
        return;

    // 5. custom icons: only the ones that are missing here
    if (QFileInfo(FS::PathCombine(source, "icons")).isDir()) {
        const QString targetData = APPLICATION->dataRoot();
        Filter missingIcons = [targetData](const QString& relative) {
            return relative.startsWith("icons/") && !QFileInfo::exists(FS::PathCombine(targetData, relative));
        };
        if (!copyWithProgress(parent, source, targetData, missingIcons))
            return;
    }

    // 6. show them, keep their groups
    instances->loadList();
    const auto groups = sourceGroups(source);
    for (const auto& instance : fresh) {
        const auto group = groups.constFind(instance.id);
        if (group != groups.constEnd())
            instances->setInstanceGroup(instance.id, group.value());
    }

    QMessageBox::information(parent, title(), QObject::tr("Synced %1 instance(s) from Prism Launcher.").arg(fresh.size()));
}

}  // namespace PrismSync
