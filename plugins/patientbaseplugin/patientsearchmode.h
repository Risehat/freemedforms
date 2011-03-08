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
#ifndef PATIENTSEARCHMODE_H
#define PATIENTSEARCHMODE_H

#include <coreplugin/modemanager/imode.h>

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QIcon;
class QWidget;
QT_END_NAMESPACE

namespace Patients {
class PatientSelector;

namespace Internal {

class PatientSearchMode : public Core::IMode
{
    Q_OBJECT

public:
    PatientSearchMode(QObject *parent);
    ~PatientSearchMode();

    // IMode
    QString name() const;
    QIcon icon() const;
    int priority() const;
    QWidget* widget();
    const char* uniqueModeName() const;
    QList<int> context() const;

private:
    PatientSelector *m_Selector;
};

} // namespace Internal
} // namespace Patients


#endif // PATIENTSEARCHMODE_H
