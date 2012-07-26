/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers:                                                     *
 *       ChristianAReiter <christian.a.reiter@gmail.com>                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "webcampreferences.h"
#include "ui_webcampreferences.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Webcam;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

/* ----------------------  Preferences Widget ---------------------- */


/*! Creates a new preferences widget with a given parent. */
WebcamPreferencesWidget::WebcamPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebcamPreferencesWidget)
{
    ui->setupUi(this);
}

WebcamPreferencesWidget::~WebcamPreferencesWidget()
{
    delete ui;
}

/*! Sets data of a changed data model to the ui's widgets. */
void WebcamPreferencesWidget::setDataToUi()
{
}

/*! Saves the settings in the ui to the settings data model. */
void WebcamPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    // if no sets given as param, take default interface
    Core::ISettings *s = sets? sets : settings();
}

/*! Writes the default settings to the data model. */
void WebcamPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    Q_UNUSED(s);
    //    LOG_FOR(tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("WebcamPreferencesWidget"));
}

/*! Retranslates the ui widgets to the changed language. */
void WebcamPreferencesWidget::retranslateUi()
{
}

void WebcamPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}

/* ----------------------  Preferences Page ---------------------- */
/*! 
 * \class WebcamPreferencesPage
 * \brief Generic FreeMedForms preferences page for %PluginName:c% plugin.
 *
 * A WebcamPreferencesPage creates a page in the FMF settings dialog, it is listed in
 * the category returned by \sa category().
 *
 * All you have to do is to load this preferences page in the constructor if the %PluginName:c% plugin.
 * Do this like this:
 * \code
 * WebcamPlugin::WebcamPlugin():
 *     ExtensionSystem::IPlugin()
 * {
 *     _prefPage = new WebcamPreferencesPage(this);
 *     addObject(_prefPage);
 * }
 * \endcode
 */

/*! Creates a new preferences page with a given parent. */
WebcamPreferencesPage::WebcamPreferencesPage(QObject *parent) :
    IOptionsPage(parent),
    m_Widget(0)
{
    setObjectName("WebcamPreferencesPage");
}

WebcamPreferencesPage::~WebcamPreferencesPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

/*! Returns the id if the preferences page. */
QString WebcamPreferencesPage::id() const
{
    return objectName();
}

/*! Returns the (translated) name of the preferences page. */
QString WebcamPreferencesPage::name() const
{
    return tr("General");
}

/*! Returns the (translated) category of the preferences page. */
QString WebcamPreferencesPage::category() const
{
    return tr("Webcam");
}

/*! Resets the whole preferences page to the default settings of the settings data model. */
void WebcamPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

/*! Overridden function that apllies pending changes to the data model without closing the dialog. */
void WebcamPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void WebcamPreferencesPage::finish()
{
    delete m_Widget;
}
#include <coreplugin/constants.h>
/*! \brief Checks if the entered settings are valid.
 * 
 * Overloads the interface method. For each empty value the default settings value is written.
 */
void WebcamPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert("Webcam/General","-");
    
    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

/*! Creates the settings page */
QWidget *WebcamPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new WebcamPreferencesWidget(parent);
    return m_Widget;
}


