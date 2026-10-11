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
#pragma once

#include <QString>

class QWidget;

/**
 * Brings instances (and their groups and custom icons) over from a Prism Launcher data folder.
 *
 * - one way: Prism Launcher -> this launcher. The Prism Launcher folder is only ever read.
 * - additive: instances that already exist here are never overwritten, nothing is ever deleted.
 *
 * The very first start of the launcher uses the migration prompt in Application::handleDataMigration()
 * (which also carries over accounts and settings); this class is what the sync button in the status bar runs.
 */
namespace PrismSync {

/// Where Prism Launcher keeps its data by default (%APPDATA%/PrismLauncher on Windows).
QString defaultSourceDir();

/// A folder counts as Prism Launcher data if it has a prismlauncher.cfg or an instances folder.
bool isValidSource(const QString& dir);

/// Interactive flow: locate the Prism Launcher folder, show what is new, ask, copy, refresh the instance list.
void run(QWidget* parent);

}  // namespace PrismSync
