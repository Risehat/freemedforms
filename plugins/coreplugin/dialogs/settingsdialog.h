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
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <coreplugin/core_exporter.h>

#include <QDialog>
#include <QList>

/**
 * \file settingsdialog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 08 Sept 2009
*/

namespace Core {
class IOptionsPage;

namespace Internal {
namespace Ui{
    class SettingsDialog;
} // Ui
} // Internal

class CORE_EXPORT SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent,
                   const QString &initialCategory = QString(),
                   const QString &initialPage = QString());
    ~SettingsDialog();

    // Run the dialog and return true if 'Ok' was choosen or 'Apply' was invoked
    // at least once
    bool execDialog();

public Q_SLOTS:
    void done(int);

private Q_SLOTS:
    void pageSelected();
    void accept();
    void reject();
    void apply();
    void restoreDefaults();
    void showHelp();

private:
    Internal::Ui::SettingsDialog *m_ui;
    QList<Core::IOptionsPage*> m_pages;
    bool m_applied;
    QString m_currentCategory;
    QString m_currentPage;
};

} // namespace Core

#endif // SETTINGSDIALOG_H
