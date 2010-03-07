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
#include "printerpreferences.h"
#include "constants.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

#include <QPrinterInfo>

using namespace Print::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PrinterPreferencesPage::PrinterPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{
    setObjectName("PrinterPreferencesPage");
    checkSettingsValidity();
}

PrinterPreferencesPage::~PrinterPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PrinterPreferencesPage::id() const { return objectName(); }
QString PrinterPreferencesPage::name() const { return tkTr(Trans::Constants::DEFAULTS); }
QString PrinterPreferencesPage::category() const { return tkTr(Trans::Constants::PRINTING); }

void PrinterPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void PrinterPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PrinterPreferencesPage::finish() { delete m_Widget; }

void PrinterPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Print::Constants::S_DEFAULT_PRINTER, QString("System"));
    defaultvalues.insert(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);
    defaultvalues.insert(Print::Constants::S_RESOLUTION, QPrinter::PrinterResolution);
    defaultvalues.insert(Print::Constants::S_KEEP_PDF, false);
    defaultvalues.insert(Print::Constants::S_PDF_FOLDER, QVariant());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *PrinterPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PrinterPreferencesWidget(parent);
    return m_Widget;
}



PrinterPreferencesWidget::PrinterPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
}

void PrinterPreferencesWidget::setDatasToUi()
{
    printerList->clear();
    if (!QPrinterInfo::availablePrinters().count()) {
        QListWidgetItem *item = new QListWidgetItem(printerList);
        QFont bold;
        bold.setBold(true);
        item->setBackgroundColor(QColor("yellow"));
        item->setForeground(QColor("red"));
        item->setFont(bold);
        item->setText(tr("WARNING !\n"
                         "No configurated printer found on your system.\n"
                         "Printing and print preview should not work at all.\n"
                         "Please configure a printer.\n"
                         "Refer to your operating system documentation."));
        return;
    }
    QString select = settings()->value(Print::Constants::S_DEFAULT_PRINTER).toString();
    foreach(const QPrinterInfo &info, QPrinterInfo::availablePrinters()) {
        QListWidgetItem *item = new QListWidgetItem(printerList);
        QString name = info.printerName();
        if (name == select) {
            QColor c = QColor("lightred");
            c.setAlpha(122);
            item->setData(Qt::BackgroundRole, c);
            name.append(" [Selected]");
            item->setSelected(true);
        }
        if (info.isDefault()) {
            QColor c = QColor("lightblue");
            c.setAlpha(122);
            item->setData(Qt::BackgroundRole, c);
            name.append(" [Default]");
        }
        item->setData(Qt::DisplayRole, name);
    }
    // Add settings name if printer is not connected !

    QListWidgetItem *system = new QListWidgetItem(tr("Always use system's default printer"), printerList);
    system->setData(Qt::UserRole, "System");
    if (select == "System")
        system->setSelected(true);
    QListWidgetItem *user = new QListWidgetItem(tr("Always ask user for the printer"), printerList);
    user->setData(Qt::UserRole, "User");
    if (select == "User")
        user->setSelected(true);

    // Color print
    if (settings()->value(Print::Constants::S_COLOR_PRINT).toInt() == QPrinter::Color) {
        colorBox->setChecked(true);
    } else {
        grayBox->setChecked(true);
    }

    // Resolution
    resolutionCombo->setCurrentIndex(settings()->value(Print::Constants::S_RESOLUTION).toInt());

    // Pdf
    keepPdfBox->setChecked(settings()->value(Print::Constants::S_KEEP_PDF).toBool());
//    S_PDF_FOLDER
}

void PrinterPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    // Default printer
    if (printerList->selectedItems().count()) {
        QListWidgetItem *sel = printerList->selectedItems().at(0);
        if (sel) {
            if (!sel->data(Qt::UserRole).toString().isEmpty()) {
                s->setValue(Print::Constants::S_DEFAULT_PRINTER, sel->data(Qt::UserRole));
            } else {
                s->setValue(Print::Constants::S_DEFAULT_PRINTER, sel->text());
            }
        }
    } else {
        s->setValue(Print::Constants::S_DEFAULT_PRINTER, "system");
    }
    // Color Print
    if (colorBox->isChecked())
        s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::Color);
    else
        s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);

    // Resolution
    s->setValue(Print::Constants::S_RESOLUTION, resolutionCombo->currentIndex());

    // Pdf
    s->setValue(Print::Constants::S_KEEP_PDF, keepPdfBox->isChecked());
    //    S_PDF_FOLDER
}

void PrinterPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Utils::Log::addMessage("PrinterPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("Printer"));
    s->setValue(Print::Constants::S_DEFAULT_PRINTER, QString("System"));
    s->setValue(Print::Constants::S_COLOR_PRINT, QPrinter::GrayScale);
    s->setValue(Print::Constants::S_RESOLUTION, QPrinter::PrinterResolution);
    s->setValue(Print::Constants::S_KEEP_PDF, false);
    s->setValue(Print::Constants::S_PDF_FOLDER, QVariant());
    s->sync();
}

void PrinterPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
