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
#ifndef FORMFILESSELECTORWIDGET_H
#define FORMFILESSELECTORWIDGET_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE


/**
 * \file formfileselectorwidget.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 08 June 2010
*/

namespace Form {
namespace Internal {
class FormFilesSelectorWidgetPrivate;
}  // End namespace Internal

/** \todo add default management */
class FORM_EXPORT FormFilesSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    FormFilesSelectorWidget(QWidget *parent = 0);
    ~FormFilesSelectorWidget();

private Q_SLOTS:
    void on_useButton_clicked();
    void on_listView_activated(const QModelIndex &index);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::FormFilesSelectorWidgetPrivate *d;
};

}  // End namespace Form

#endif // FORMFILESSELECTORWIDGET_H
