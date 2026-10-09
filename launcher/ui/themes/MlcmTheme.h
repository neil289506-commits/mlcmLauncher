// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  MLCM theme: a blend of the Minecraft Launcher (60%), CurseForge (20%) and Modrinth (20%) looks.
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

#include "FusionTheme.h"

/**
 * Design tokens
 *   Minecraft Launcher (dominant): charcoal surfaces, bevelled grass-green PLAY button, XP-bar progress, square-ish controls
 *   CurseForge (accent):           orange call-to-action buttons and tab / sidebar indicators
 *   Modrinth (accent):             bright green focus rings, rounded cards, slim rounded scrollbars
 */
class MlcmTheme : public FusionTheme {
   public:
    virtual ~MlcmTheme() {}

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
