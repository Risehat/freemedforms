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
 *   Main developers : %AuthorName%
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
@if "%Internal%" == "true"
#ifndef %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
#define %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
@else
#ifndef %PluginNamespace:u%_%ClassName:u%_H
#define %PluginNamespace:u%_%ClassName:u%_H
@endif

@if "%Exported%" == "true"
#include <missing/%PluginNamespace:l%_exporter.h>
@endif
#include <QObject>

/**
 * \file %ClassName:l%.%CppHeaderSuffix:l%
 * \author %AuthorName%
 * \version 0.8.0
 * \date %CurrentDate%
*/

namespace %PluginNamespace:c% {
namespace Internal {
@if  "%PIMPL%" == "true"
class %ClassName:c%Private;
@endif
@if "%Internal%" != "true"
} // namespace Internal
@endif

@if "%Exported%" == "true"
class %PluginNamespace:u%_EXPORT %ClassName:c% : public QObject
@else
class %ClassName:c% : public QObject
@endif
{
    Q_OBJECT

@if "%Singleton%" == "true"
protected:
    explicit %ClassName:c%(QObject *parent = 0);

public:
    static %ClassName:c% &instance();
@else
public:
    explicit %ClassName:c%(QObject *parent = 0);
@endif
    ~%ClassName:c%();

    bool initialize();

Q_SIGNALS:

public Q_SLOTS:

@if "%PIMPL%" == "true"
private:
@if "%Internal%" == "true"
    %ClassName:c%Private *d;
@else
    Internal::%ClassName:c%Private *d;
@endif
@endif
@if "%Singleton%" == "true"
    static %ClassName:c% *_instance;
@endif
};

@if "%Internal%" == "true"
} // namespace Internal
@endif
} // namespace %PluginNamespace:c%

@if "%Internal%" == "true"
#endif // %PluginNamespace:u%_INTERNAL_%ClassName:u%_H
@else
#endif  // %PluginNamespace:u%_%ClassName:u%_H
@endif
