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
#include "pmhcreatordialog.h"
#include "pmhcore.h"
#include "pmhbase.h"
#include "pmhdata.h"
#include "pmhcategorymodel.h"
#include "constants.h"

#include <coreplugin/dialogs/helpdialog.h>

#include "ui_pmhcreatordialog.h"

#include <QDebug>

using namespace PMH;

static inline PmhCore *pmhCore() {return PmhCore::instance();}


PmhCreatorDialog::PmhCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PmhCreatorDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Past Medical History Creator"));
    ui->pmhViewer->setEditMode(PmhViewer::ReadWriteMode);
    ui->pmhViewer->createNewPmh();
    ui->pmhViewer->setShowPatientInformations(true);
}

PmhCreatorDialog::~PmhCreatorDialog()
{
    delete ui;
}

void PmhCreatorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            Internal::PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Feed category model with this new PmhData
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
            accept();
            break;
        }
    case QDialogButtonBox::Cancel: reject(); break;
    case QDialogButtonBox::Help: helpRequested(); break;
    default: break;
    }
}

void PmhCreatorDialog::helpRequested()
{
    Core::HelpDialog::showPage(Constants::H_PMH_CREATOR_PAGE);
}

void PmhCreatorDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
