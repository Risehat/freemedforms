/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Agenda::Internal::UserCalendarPageForUserViewer
  Is an Internal class and an extension to the UserPlugin::UserViewer
*/

#include "usercalendarpageforuserviewer.h"
#include "agendabase.h"
#include "usercalendarmodel.h"
#include "usercalendarmodelfulleditorwidget.h"
#include "agendacore.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>

#include <usermanagerplugin/usermodel.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDataWidgetMapper>
#include <QHBoxLayout>

//#include "ui_agendauserviewerwidget.h"

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}
static inline Agenda::AgendaCore *agendaCore() {return Agenda::AgendaCore::instance();}

UserCalendarPageForUserViewerWidget::UserCalendarPageForUserViewerWidget(QWidget *parent) :
    UserPlugin::IUserViewerWidget(parent),
    m_Model(0),
    m_Widget(new UserCalendarModelFullEditorWidget(this))
{
    setObjectName("UserCalendarPageForUserViewerWidget");
    QHBoxLayout *l = new QHBoxLayout(this);
    setLayout(l);
    l->setMargin(0);
    l->addWidget(m_Widget);
}

UserCalendarPageForUserViewerWidget::~UserCalendarPageForUserViewerWidget()
{}

void UserCalendarPageForUserViewerWidget::setUserModel(UserPlugin::UserModel *model)
{
    m_Model = model;
}

void UserCalendarPageForUserViewerWidget::setUserIndex(const int index)
{
    Q_ASSERT(m_Model);
    if (m_Model) {
        UserCalendarModel *model = agendaCore()->userCalendarModel(m_Model->index(index, Core::IUser::Uuid).data().toString());
        m_Widget->setUserCalendarModel(model);
    }
}

void UserCalendarPageForUserViewerWidget::clear()
{
    if (m_Widget)
        m_Widget->clear();
}

bool UserCalendarPageForUserViewerWidget::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}


UserCalendarPageForUserViewer::UserCalendarPageForUserViewer(QObject *parent) :
    UserPlugin::IUserViewerPage(parent)
{
    setObjectName("UserCalendarPageForUserViewer");
}

UserCalendarPageForUserViewer::~UserCalendarPageForUserViewer()
{}

QString UserCalendarPageForUserViewer::id() const
{
    return objectName();
}

QString UserCalendarPageForUserViewer::name() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString UserCalendarPageForUserViewer::category() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString UserCalendarPageForUserViewer::title() const
{
    return tr("User agendas");
}

QWidget *UserCalendarPageForUserViewer::createPage(QWidget *parent)
{
    return new UserCalendarPageForUserViewerWidget(parent);
}

