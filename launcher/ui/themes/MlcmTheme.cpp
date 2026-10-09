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
#include "MlcmTheme.h"

#include <QObject>

// ---------------------------------------------------------------------------------------------
//  Colour tokens (kept here as a reference; the style sheet below uses the literal values)
//
//    window        #1e1f22     toolbar / status   #18191b     panel        #26272b
//    base (inputs) #17181b     card               #2c2d32     card hover   #35363c
//    border        #3a3b41     text               #ececec     muted text   #a0a1a8
//
//    Minecraft green   #3c8527  (hover #4a9e30, bevel light #62b343, bevel dark #1d4a10)   -> selection, PLAY, progress
//    Modrinth green    #1bd96a                                                             -> focus ring, links
//    CurseForge orange #f16436  (hover #ff7a4d, bevel dark #c24b22)                        -> default buttons, tabs, sidebar
// ---------------------------------------------------------------------------------------------

namespace {

// Global / window / menus / tooltips
QString sheetBase()
{
    return QStringLiteral(R"QSS(
QToolTip {
    color: #f2f2f2;
    background-color: #111214;
    border: 1px solid #1bd96a;
    padding: 4px 8px;
}

QMainWindow, QDialog {
    background-color: #1e1f22;
}

QMainWindow::separator {
    background: #2f3035;
    width: 1px;
    height: 1px;
}

QMenuBar {
    background-color: #18191b;
    color: #ececec;
    border-bottom: 1px solid #2f3035;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
}
QMenuBar::item:selected {
    background-color: #35363c;
}

QMenu {
    background-color: #26272b;
    color: #ececec;
    border: 1px solid #3a3b41;
    padding: 4px;
}

QStatusBar {
    background-color: #18191b;
    color: #a0a1a8;
    border-top: 1px solid #2f3035;
}
QStatusBar::item {
    border: none;
}
)QSS");
}

// Toolbars: main (top), instance (right) and news (bottom) + the big green PLAY and orange ADD buttons
QString sheetToolbars()
{
    return QStringLiteral(R"QSS(
QToolBar {
    background-color: #18191b;
    border: none;
    padding: 6px;
    spacing: 4px;
}
QToolBar#mainToolBar {
    border-bottom: 2px solid #2f3035;
}
QToolBar#newsToolBar {
    border-top: 1px solid #2f3035;
    padding: 2px 6px;
}
QToolBar#instanceToolBar {
    background-color: #222327;
    border-left: 2px solid #2f3035;
    padding: 10px;
    spacing: 6px;
}
QToolBar::separator {
    background-color: #33343a;
    width: 1px;
    height: 1px;
    margin: 6px 4px;
}

QToolButton {
    background-color: transparent;
    color: #ececec;
    border: 1px solid transparent;
    border-radius: 4px;
    padding: 6px 12px;
}
QToolButton:hover {
    background-color: #35363c;
    border-color: #45464d;
}
QToolButton:pressed,
QToolButton:checked {
    background-color: #2c2d32;
    border-color: #1bd96a;
}
QToolButton:disabled {
    color: #6d6e75;
    background-color: transparent;
    border-color: transparent;
}

/* Instance header (icon + name) in the right-hand panel */
QToolBar#instanceToolBar QToolButton#changeIconButton,
QToolBar#instanceToolBar QToolButton#renameButton {
    background-color: #2c2d32;
    border: 1px solid #3a3b41;
    border-radius: 8px;
    padding: 8px;
}
QToolBar#instanceToolBar QToolButton#changeIconButton:hover,
QToolBar#instanceToolBar QToolButton#renameButton:hover {
    background-color: #35363c;
    border-color: #1bd96a;
}
QToolBar#instanceToolBar QToolButton#renameButton {
    font-weight: bold;
}

/* Minecraft Launcher style PLAY button (bevelled grass green) */
QToolButton#launchButton {
    background-color: #3c8527;
    color: #ffffff;
    font-size: 15px;
    font-weight: bold;
    min-height: 30px;
    padding: 10px 18px;
    border: 2px solid #1d4a10;
    border-top-color: #62b343;
    border-left-color: #52a535;
    border-radius: 2px;
}
QToolButton#launchButton:hover {
    background-color: #4a9e30;
}
QToolButton#launchButton:pressed {
    background-color: #2f6b1e;
    border-color: #62b343;
    border-top-color: #1d4a10;
    border-left-color: #1d4a10;
}
QToolButton#launchButton:disabled {
    background-color: #2b3328;
    color: #77807a;
    border-color: #232a20;
}

/* Kill button: calm red, same size as the PLAY button */
QToolButton#killButton {
    background-color: #5a2a2a;
    color: #ffffff;
    font-weight: bold;
    min-height: 22px;
    border: 1px solid #7a3636;
    border-radius: 2px;
}
QToolButton#killButton:hover {
    background-color: #7a3636;
}
QToolButton#killButton:disabled {
    background-color: #2a2323;
    color: #6d6e75;
    border-color: #332a2a;
}

/* CurseForge style call-to-action */
QToolButton#addInstanceButton {
    background-color: #f16436;
    color: #ffffff;
    font-weight: bold;
    padding: 7px 16px;
    border: 1px solid #c24b22;
    border-bottom: 3px solid #c24b22;
    border-radius: 4px;
}
QToolButton#addInstanceButton:hover {
    background-color: #ff7a4d;
}
QToolButton#addInstanceButton:pressed {
    background-color: #d9552b;
    border-bottom-width: 1px;
}
)QSS");
}

// Push buttons, inputs, scrollbars
QString sheetControls()
{
    return QStringLiteral(R"QSS(
QPushButton {
    background-color: #3a3b41;
    color: #f2f2f2;
    border: 1px solid #4a4b52;
    border-radius: 4px;
    padding: 6px 16px;
    min-height: 20px;
}
QPushButton:hover {
    background-color: #45464d;
    border-color: #5a5b63;
}
QPushButton:pressed {
    background-color: #2c2d32;
}
QPushButton:disabled {
    background-color: #2a2b2f;
    color: #6d6e75;
    border-color: #33343a;
}
QPushButton:focus {
    border-color: #1bd96a;
}
QPushButton:default {
    background-color: #f16436;
    color: #ffffff;
    font-weight: bold;
    border: 1px solid #c24b22;
    border-bottom: 3px solid #c24b22;
}
QPushButton:default:hover {
    background-color: #ff7a4d;
}
QPushButton:default:pressed {
    background-color: #d9552b;
    border-bottom-width: 1px;
}
QPushButton:default:disabled {
    background-color: #4a3029;
    color: #8a7068;
    border-color: #3a2822;
}

QLineEdit,
QTextEdit,
QPlainTextEdit,
QTextBrowser {
    background-color: #17181b;
    color: #ececec;
    border: 1px solid #3a3b41;
    border-radius: 4px;
    padding: 5px 8px;
    selection-background-color: #3c8527;
    selection-color: #ffffff;
}
QLineEdit:focus,
QTextEdit:focus,
QPlainTextEdit:focus,
QTextBrowser:focus {
    border: 1px solid #1bd96a;
}
QLineEdit:disabled,
QTextEdit:disabled,
QPlainTextEdit:disabled {
    color: #6d6e75;
    background-color: #1c1d20;
}
QComboBox QLineEdit,
QAbstractSpinBox QLineEdit {
    border: none;
    background: transparent;
    padding: 0;
}

QScrollBar:vertical {
    background: transparent;
    width: 12px;
    margin: 2px;
}
QScrollBar:horizontal {
    background: transparent;
    height: 12px;
    margin: 2px;
}
QScrollBar::handle:vertical {
    background-color: #4a4b52;
    border-radius: 4px;
    min-height: 32px;
}
QScrollBar::handle:horizontal {
    background-color: #4a4b52;
    border-radius: 4px;
    min-width: 32px;
}
QScrollBar::handle:vertical:hover,
QScrollBar::handle:horizontal:hover {
    background-color: #5f6068;
}
QScrollBar::handle:vertical:pressed,
QScrollBar::handle:horizontal:pressed {
    background-color: #1bd96a;
}
QScrollBar::add-line,
QScrollBar::sub-line {
    width: 0px;
    height: 0px;
    background: none;
    border: none;
}
QScrollBar::add-page,
QScrollBar::sub-page {
    background: none;
}
)QSS");
}

// Lists, tabs, side navigation, progress bars, group boxes
QString sheetViews()
{
    return QStringLiteral(R"QSS(
QTreeView,
QTableView,
QListView,
QListWidget {
    background-color: #17181b;
    alternate-background-color: #1c1d20;
    color: #ececec;
    border: 1px solid #2f3035;
    border-radius: 6px;
    selection-background-color: #3c8527;
    selection-color: #ffffff;
}
QTreeView:focus,
QTableView:focus,
QListView:focus {
    border-color: #1bd96a;
}

/* The instance grid sits directly on the window surface */
InstanceView {
    background-color: #1e1f22;
    border: none;
    border-radius: 0px;
}

/* CurseForge / Modrinth style side navigation (settings, instance edit, mod browser...) */
QListView[_kde_side_panel_view="true"] {
    background-color: #222327;
    border: none;
    border-right: 2px solid #2f3035;
    border-radius: 0px;
    padding: 6px;
}
QListView[_kde_side_panel_view="true"]::item {
    padding: 7px 10px;
    margin: 1px 0px;
    border-left: 3px solid transparent;
    border-radius: 4px;
    color: #c8c9ce;
}
QListView[_kde_side_panel_view="true"]::item:hover {
    background-color: #2c2d32;
}
QListView[_kde_side_panel_view="true"]::item:selected {
    background-color: #2f3035;
    border-left: 3px solid #f16436;
    color: #ffffff;
}

QTabWidget::pane {
    border: 1px solid #2f3035;
    border-radius: 6px;
    top: -1px;
}
QTabBar::tab {
    background: transparent;
    color: #a0a1a8;
    padding: 8px 16px;
    margin-right: 2px;
    border: none;
    border-bottom: 2px solid transparent;
}
QTabBar::tab:hover {
    color: #ececec;
}
QTabBar::tab:selected {
    color: #ffffff;
    border-bottom: 2px solid #f16436;
}
QTabBar::tab:disabled {
    color: #5a5b61;
}

/* Minecraft XP-bar style progress */
QProgressBar {
    background-color: #17181b;
    color: #ececec;
    border: 1px solid #3a3b41;
    border-radius: 4px;
    text-align: center;
    min-height: 16px;
}
QProgressBar::chunk {
    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #62b343, stop: 1 #3c8527);
    border-radius: 3px;
}

QSplitter::handle {
    background-color: #2f3035;
}
)QSS");
}

}  // namespace

QString MlcmTheme::id()
{
    return "mlcm";
}

QString MlcmTheme::name()
{
    return QObject::tr("MLCM");
}

QString MlcmTheme::tooltip()
{
    return QObject::tr("Minecraft Launcher look with CurseForge and Modrinth accents");
}

QPalette MlcmTheme::colorScheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0x1e, 0x1f, 0x22));
    palette.setColor(QPalette::WindowText, QColor(0xec, 0xec, 0xec));
    palette.setColor(QPalette::Base, QColor(0x17, 0x18, 0x1b));
    palette.setColor(QPalette::AlternateBase, QColor(0x1c, 0x1d, 0x20));
    palette.setColor(QPalette::ToolTipBase, QColor(0x11, 0x12, 0x14));
    palette.setColor(QPalette::ToolTipText, QColor(0xf2, 0xf2, 0xf2));
    palette.setColor(QPalette::Text, QColor(0xec, 0xec, 0xec));
    palette.setColor(QPalette::Button, QColor(0x2c, 0x2d, 0x32));
    palette.setColor(QPalette::ButtonText, QColor(0xec, 0xec, 0xec));
    palette.setColor(QPalette::BrightText, QColor(0xff, 0x5c, 0x5c));
    palette.setColor(QPalette::Link, QColor(0x1b, 0xd9, 0x6a));
    palette.setColor(QPalette::LinkVisited, QColor(0x8f, 0xe8, 0xb3));
    palette.setColor(QPalette::Highlight, QColor(0x3c, 0x85, 0x27));
    palette.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    palette.setColor(QPalette::PlaceholderText, QColor(0x77, 0x78, 0x7f));
    return fadeInactive(palette, fadeAmount(), fadeColor());
}

double MlcmTheme::fadeAmount()
{
    return 0.5;
}

QColor MlcmTheme::fadeColor()
{
    return QColor(0x1e, 0x1f, 0x22);
}

bool MlcmTheme::hasStyleSheet()
{
    return true;
}

QString MlcmTheme::appStyleSheet()
{
    return sheetBase() + sheetToolbars() + sheetControls() + sheetViews();
}
