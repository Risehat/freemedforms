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
#ifndef DOSAGECREATORDIALOG_H
#define DOSAGECREATORDIALOG_H

#include <QtGlobal>
QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

// include Ui
#include "ui_mfDosageCreatorDialog.h"

/**
 * \file mfDosageCreatorDialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 19 Sept 2010
*/

namespace DrugsDB {
namespace Internal {
class DosageModel;
}  // End Internal
}  // End DrugsDB

namespace DrugsWidget {
namespace Internal {
class DosageModel;
class DosageCreatorDialogPrivate;

class DosageCreatorDialog : public QDialog, public Ui::DosageCreatorDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(DosageCreatorDialog);

public:
    explicit DosageCreatorDialog(QWidget *parent, DrugsDB::Internal::DosageModel *dosageModel);
    ~DosageCreatorDialog();

private:
    void keyPressEvent(QKeyEvent *e);

private Q_SLOTS:
    void done(int r);
    void updateSettings();
    void protocolDatasChanged();
    void saveRequested();
    void prescribeRequested();
    void saveAndPrescribeRequested();
    void helpRequested();
    void drugsInformationsRequested();
    void addTestOnlyRequested();

private:
    DosageCreatorDialogPrivate *d;
};

}  // End Internal
}  // End DrugsWidget


#endif // DOSAGECREATORDIALOG_H
