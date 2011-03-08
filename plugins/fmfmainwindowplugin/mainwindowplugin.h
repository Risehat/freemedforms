/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MAINWIN_PLUGIN_H
#define MAINWIN_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file mainwindowplugin.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.2
 * \date 23 Oct 2009
*/

namespace MainWin {
namespace Internal {
class MainWindowPreferencesPage;
class VirtualBasePage;
} // End Internal

class MainWindow;

class MainWinPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    MainWinPlugin();
    ~MainWinPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

private:
    MainWindow *m_MainWindow;
    Internal::MainWindowPreferencesPage *prefPage;
    Internal::VirtualBasePage *virtualBasePage;
};


}  // End MainWin

#endif  // End MAINWIN_PLUGIN_H
