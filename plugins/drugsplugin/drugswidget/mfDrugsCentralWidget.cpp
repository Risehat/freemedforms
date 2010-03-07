/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDrugsCentralWidget.h"

// include drugs widgets headers
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <drugsplugin/drugswidget/mfDrugSelector.h>
#include <drugsplugin/drugswidget/mfPrescriptionViewer.h>
#include <drugsplugin/dosagedialog/mfDosageCreatorDialog.h>
#include <drugsplugin/dosagedialog/mfDosageDialog.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <templatesplugin/templatescreationdialog.h>

#ifdef FREEDIAMS
#  include <fdcoreplugin/patient.h>
#endif

#include "ui_mfDrugsCentralWidget.h"
#include <QTreeWidget>

using namespace DrugsWidget;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::UniqueIDManager *uid() {return Core::ICore::instance()->uniqueIDManager();}
static inline Core::ContextManager *contextManager() {return Core::ICore::instance()->contextManager();}


/** \brief Constructor */
DrugsCentralWidget::DrugsCentralWidget(QWidget *parent) :
    QWidget(parent), m_CurrentDrugModel(0),
    m_SelectionOnlyMode(false)
{
    // create instance of DrugsManager
    DrugsWidgetManager::instance();
}

/** \brief Initialize the widget after the ui was setted */
bool DrugsCentralWidget::initialize()
{
    m_ui = new DrugsWidget::Internal::Ui::DrugsCentralWidget();
    m_ui->setupUi(this);

    // create context
    m_Context = new Internal::DrugsContext(this);
    m_Context->setContext(QList<int>() << uid()->uniqueIdentifier(Constants::C_DRUGS_PLUGINS));
    contextManager()->addContextObject(m_Context);

    // create model view for selected drugs list
    m_CurrentDrugModel = new DrugsDB::DrugsModel(this);
    m_ui->m_PrescriptionView->initialize();
    m_ui->m_PrescriptionView->setModel(m_CurrentDrugModel);

    m_ui->m_DrugSelector->initialize();

    m_ui->m_DrugSelector->setFocus();

    DrugsWidgetManager::instance()->setCurrentView(this);

    changeFontTo(QFont(settings()->value(Constants::S_VIEWFONT).toString(), settings()->value(Constants::S_VIEWFONTSIZE).toInt()));

    return true;
}

QListView *DrugsCentralWidget::prescriptionListView()
{
    return m_ui->m_PrescriptionView->listview();
}

PrescriptionViewer *DrugsCentralWidget::prescriptionView()
{
    return m_ui->m_PrescriptionView;
}

Internal::DrugSelector *DrugsCentralWidget::drugSelector()
{
    return m_ui->m_DrugSelector;
}

DrugsDB::DrugsModel *DrugsCentralWidget::currentDrugsModel() const
{
    return m_CurrentDrugModel;
}

void DrugsCentralWidget::setCurrentSearchMethod(int method)
{
    m_ui->m_DrugSelector->setSearchMethod(method);
}

void DrugsCentralWidget::setMode(Modes mode)
{
    // no change
    if (mode==SelectOnly && m_SelectionOnlyMode)
        return;
    if (mode==Prescriber && !m_SelectionOnlyMode)
        return;
    // ask user
    if (m_CurrentDrugModel->rowCount() > 0) {
        bool yes;
        yes = Utils::yesNoMessageBox(tr("Prescription is not empty. Clear it ?"),
                                     tr("You select another editing mode than the actual one. "
                                        "Changing of mode during edition may cause prescription lose.\n"
                                        "Do you really want to change the editing mode ?"));
        if (yes) {
           m_CurrentDrugModel->clearDrugsList();
       } else {
           return;
       }
    }
    // change the mode
    if (mode == SelectOnly) {
        m_SelectionOnlyMode = true;
        m_CurrentDrugModel->setSelectionOnlyMode(true);
    } else {
        m_SelectionOnlyMode = false;
        m_CurrentDrugModel->setSelectionOnlyMode(false);
    }
}

void DrugsCentralWidget::createConnections()
{
    connect(m_ui->m_DrugSelector, SIGNAL(drugSelected(int)), this, SLOT( selector_drugSelected(const int) ) );
    connect(prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
            m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
}

void DrugsCentralWidget::disconnect()
{
    prescriptionListView()->disconnect( prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
             m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
    m_ui->m_DrugSelector->disconnect(m_ui->m_DrugSelector, SIGNAL(drugSelected(int)),
                                     this, SLOT( selector_drugSelected(const int) ) );
}

void DrugsCentralWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_ui->m_DrugSelector->setFocus();
}

/**
  \brief Slot called when is selected from the drugSelector.
  Verify that the drug isn't already prescribed (if it is warn user and stop). \n
  Add the drug to the mfDrugsModel and open the mfDosageCreatorDialog\n
*/
void DrugsCentralWidget::selector_drugSelected(const int uid)
{
    // if exists dosage for that drug show the dosageSelector widget
    // else show the dosage creator widget
    if (m_CurrentDrugModel->containsDrug(uid)) {
        Utils::warningMessageBox(tr("Can not add this drug to your prescription."),
                                    tr("Prescription can not contains twice the sample pharmaceutical drug.\n"
                                       "Drug %1 is already in your prescription")
                                    .arg(m_CurrentDrugModel->drugData(uid, DrugsDB::Constants::Drug::Denomination).toString()),
                                    tr("If you want to change the dosage of this drug please double-click on it in the prescription box."));
        return;
    }
//    int drugPrescriptionRow = m_CurrentDrugModel->addDrug(uid);
    m_CurrentDrugModel->addDrug(uid);
    if (!m_SelectionOnlyMode) {
        Internal::DosageCreatorDialog dlg(this, m_CurrentDrugModel->dosageModel(uid));
        if (dlg.exec()==QDialog::Rejected) {
            m_CurrentDrugModel->removeLastInsertedDrug();
        }
        m_ui->m_PrescriptionView->listview()->update();
    }
}

/** \brief Change the font of the viewing widget */
void DrugsCentralWidget::changeFontTo(const QFont &font)
{
    m_ui->m_DrugSelector->setFont(font);
    m_ui->m_PrescriptionView->listview()->setFont(font);
}

bool DrugsCentralWidget::printPrescription()
{
    return DrugsDB::DrugsIO::printPrescription(m_CurrentDrugModel);
}

void DrugsCentralWidget::printPreview()
{
    return DrugsDB::DrugsIO::prescriptionPreview(m_CurrentDrugModel);
}

bool DrugsCentralWidget::createTemplate()
{
    if (m_CurrentDrugModel->rowCount() == 0)
        return false;
    // get the template content
    QString content = DrugsDB::DrugsIO::prescriptionToXml(m_CurrentDrugModel);
    // create a new template with it
    Templates::TemplatesCreationDialog dlg(this);
    dlg.setTemplateContent(content);
    dlg.setTemplateSummary(DrugsDB::DrugsIO::prescriptionToHtml(m_CurrentDrugModel, DrugsDB::DrugsIO::SimpleVersion));
    dlg.setTemplateMimeTypes(DrugsDB::DrugsIO::prescriptionMimeTypes());
#ifdef FREEDIAMS
    dlg.setUserUuid(DrugsDB::Constants::FREEDIAMS_DEFAULT_USER_UUID);
#else
    /** \todo FMF : add user uuid */
#endif
    dlg.exec();
    return true;
}

void DrugsCentralWidget::showDatabaseInformations()
{
    const DrugsDB::DatabaseInfos *info = DrugsDB::Internal::DrugsBase::instance()->actualDatabaseInformations();
    if (!info)
        return;
    QDialog dlg(this, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    info->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
