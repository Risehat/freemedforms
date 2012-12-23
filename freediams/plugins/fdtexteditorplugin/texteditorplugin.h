/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TEXTEDITORTPLUGIN_H
#define TEXTEDITORTPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file texteditorplugin.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

namespace Editor {

class TextEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeDiams.TextEditorPlugin" FILE "TextEditor.json")

public:
    TextEditorPlugin();
    ~TextEditorPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
};


}  // End Editor

#endif // End TEXTEDITORTPLUGIN_H
