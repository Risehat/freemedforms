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
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "staticalertwidgets.h"
#include "alertcore.h"
#include "alertitem.h"
#include "alertitemeditordialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QFileInfo>
#include <QEvent>
#include <QAction>
#include <QMenu>
#include <QInputDialog>

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

namespace {
static QIcon getIcon(const AlertItem &item)
{
    if (item.themedIcon().isEmpty() || !QFileInfo(theme()->iconFullPath(item.themedIcon())).exists()) {
        // get the level icon
        QString icon;
        switch (item.priority()) {
        case AlertItem::High: icon = Core::Constants::ICONCRITICAL; break;
        case AlertItem::Medium: icon = Core::Constants::ICONWARNING; break;
        case AlertItem::Low: icon = Core::Constants::ICONINFORMATION; break;
        }
        return theme()->icon(icon, Core::ITheme::SmallIcon);
    }
    return theme()->icon(item.themedIcon(), Core::ITheme::SmallIcon);
}

static QString getToolTip(const AlertItem &item)
{
    QString toolTip;
    if (item.category().isEmpty())
        toolTip = QString("<p>%1</p>").arg(item.label());
    else
        toolTip = QString("<p><b>%1</b>: %2</p>").arg(item.category()).arg(item.label());

    QStringList related;
    for(int i = 0; i < item.relations().count(); ++i) {
        const AlertRelation &rel = item.relationAt(i);
        related += QString("%1").arg(rel.relationTypeToString());
    }

    QString content;
    if (!related.isEmpty())
        content += QString("<span style=\"color:#303030\">%1</span><br />").arg(related.join("<br />"));

    if (!item.description().isEmpty())
        content += QString("<span style=\"color:#606060\">%1</span>").arg(item.description());

    if (!content.isEmpty()) {
        content = QString("<p style=\"margin-left:15px\">%1</p>").arg(content);
        toolTip += content;
    }
    return toolTip;
}

}  // namespace anonymous


/**
  \class Alert::StaticAlertLabel
  Create a QToolButton for any static view type Alert::AlertItem. The alert can be:
    - validated
    - edited
  using the menu of this button.
*/
StaticAlertToolButton::StaticAlertToolButton(QWidget *parent) :
    QToolButton(parent)
{
    setMinimumSize(QSize(16,16));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setPopupMode(QToolButton::InstantPopup);

    // create actions and menu
    aLabel = new QAction(this);
    aCategory = new QAction(this);
    aValidate = new QAction(this);
    aEdit = new QAction(this);
    aOverride = new QAction(this);
    QAction *sep = new QAction(this);
    sep->setSeparator(true);

    aValidate->setIcon(theme()->icon(Core::Constants::ICONOK));
    aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    aEdit->setIcon(theme()->icon(Core::Constants::ICONNEXT));

    addAction(aCategory);
    addAction(sep);
    addAction(aLabel);
    addAction(sep);
    addAction(aValidate);
    addAction(aEdit);
    addAction(aOverride);

    connect(aValidate, SIGNAL(triggered()), this, SLOT(validateAlert()));
    connect(aEdit, SIGNAL(triggered()), this, SLOT(editAlert()));
    connect(aOverride, SIGNAL(triggered()), this, SLOT(overrideAlert()));
    retranslateUi();
}

StaticAlertToolButton::~StaticAlertToolButton()
{
}

/** Define the Alert::AlertItem to use for this button. */
void StaticAlertToolButton::setAlertItem(const AlertItem &item)
{
    setIcon(getIcon(item));
    setToolTip(getToolTip(item));
    if (aLabel)
        aLabel->setText(item.label());
    if (aCategory) {
        if (item.category().isEmpty())
            aCategory->setText(tr("No category"));
        else
            aCategory->setText(item.category());
    }
    _item = item;
}

void StaticAlertToolButton::validateAlert()
{
    _item.validateAlertWithCurrentUserAndConfirmationDialog();
}

void StaticAlertToolButton::editAlert()
{
    AlertItemEditorDialog dlg(this);
    dlg.setAlertItem(_item);
    if (dlg.exec() == QDialog::Accepted) {
        dlg.submit(_item);
        AlertCore::instance()->updateAlert(_item);
        AlertCore::instance()->saveAlert(_item);
    }
}

void StaticAlertToolButton::overrideAlert()
{
    // TODO: improve the dialog by creating a specific AlertOverridingConfirmationDialog
    bool yes = Utils::yesNoMessageBox(tr("Override alert"),
                                      tr("Do you really want to override this alert ?"),
                                      tr("By overriding an alert, you report your disagreement "
                                         "with the alert. The alert will no longer be presented.\n"
                                         "It is sometimes necessary to clarify your arguments."));
    if (yes) {
        QString comment;
        if (_item.isOverrideRequiresUserComment()) {
            bool ok;
            comment = QInputDialog::getText(this, tr("Explain why you override this alert"),
                                                 tr("Override comment"), QLineEdit::Normal,
                                                 "", &ok);
            if (!ok || comment.isEmpty())
                return;
        }

        QString validator;
        user() ? validator = user()->uuid() : validator = "UnknownUser";
        if (!_item.validateAlert(validator, true, comment, QDateTime::currentDateTime())) {
            LOG_ERROR("Unable to validate the static alert");
        } else {
            AlertCore::instance()->saveAlert(_item);
        }
    }
}

void StaticAlertToolButton::retranslateUi()
{
    aValidate->setText(tkTr(Trans::Constants::VALIDATE));
    aEdit->setText(tkTr(Trans::Constants::EDIT_ALERT));
    aOverride->setText(tkTr(Trans::Constants::OVERRIDE));
    aLabel->setText(_item.label());
    if (_item.category().isEmpty())
        aCategory->setText(tr("No category"));
    else
        aCategory->setText(_item.category());
}

void StaticAlertToolButton::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
    QToolButton::changeEvent(event);
}

/**
  \class Alert::StaticAlertLabel
  Create a QLabel for any static view type Alert::AlertItem. The QLabel will only present the icon
  of the alert and its label/category as tooltip. It is a 16x16 sized QLabel.
*/
StaticAlertLabel::StaticAlertLabel(QWidget *parent) :
    QLabel(parent)
{
    setMinimumSize(QSize(16,16));
}

/** Define the Alert::AlertItem to use for this button. */
void StaticAlertLabel::setAlertItem(const AlertItem &item)
{
    setPixmap(getIcon(item).pixmap(16,16));
    setToolTip(getToolTip(item));
}