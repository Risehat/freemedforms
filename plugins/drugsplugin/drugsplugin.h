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
#ifndef DRUGSWIDGETPLUGIN_H
#define DRUGSWIDGETPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file drugswidgetmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 26 Oct 2009
 * \internal
*/


namespace DrugsWidget {
class DrugsViewOptionsPage;
class DrugsSelectorOptionsPage;
class DrugsPrintOptionsPage;
class DrugsUserOptionsPage;
class DrugsExtraOptionsPage;
class DrugsDatabaseSelectorPage;
class ProtocolPreferencesPage;
}


namespace DrugsWidget {
namespace Internal {

class DrugsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    DrugsPlugin();
    ~DrugsPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

private:
    DrugsWidget::DrugsViewOptionsPage *viewPage;
    DrugsWidget::DrugsSelectorOptionsPage *selectorPage;
    DrugsWidget::DrugsPrintOptionsPage *printPage;
    DrugsWidget::DrugsUserOptionsPage *userPage;
    DrugsWidget::DrugsExtraOptionsPage *extraPage;
    DrugsWidget::DrugsDatabaseSelectorPage *databaseSelectorPage;
    DrugsWidget::ProtocolPreferencesPage *protocolPage;
};

} // namespace Internal
} // namespace DrugsWidget

#endif // DRUGSWIDGETPLUGIN_H
