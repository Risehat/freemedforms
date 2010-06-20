/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
 ***************************************************************************/
#ifndef TEXTEDITORFACTORY_H
#define TEXTEDITORFACTORY_H

#include <texteditorplugin/texteditor.h>

#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/iformitemdata.h>

#include <QStringList>

namespace BaseWidgets {

class TextEditorFactory : public Form::IFormWidgetFactory
{
    Q_OBJECT
public:
    TextEditorFactory(QObject *parent = 0);
    ~TextEditorFactory();

    bool initialize(const QStringList &arguments, QString *errorString);
    bool extensionInitialized();
    bool isInitialized() const;

    bool isContainer(const int idInStringList) const;
    QStringList providedWidgets() const;
    Form::IFormWidget *createWidget(const QString &name, Form::FormItem *object, QWidget *parent = 0);
};

class TextEditorForm : public Form::IFormWidget
{
    Q_OBJECT
public:
    TextEditorForm(Form::FormItem *linkedObject, QWidget *parent = 0);
    ~TextEditorForm();

    void addWidgetToContainer(Form::IFormWidget *) {}
    bool isContainer() const {return false;}

public Q_SLOTS:
    void retranslate();

private:
    Editor::TextEditor *m_Text;
};

class TextEditorData : public Form::IFormItemData
{
public:
    TextEditorData(Form::FormItem *item);
    ~TextEditorData();

    void setEditor(Editor::TextEditor* editor) {m_Editor = editor; clear();}

    void clear();

    Form::FormItem *parentItem() const {return m_FormItem;}
    bool isModified() const;

    bool setData(const int ref, const QVariant &data, const int role);
    QVariant data(const int ref, const int role) const;

    void setStorableData(const QVariant &data);
    QVariant storableData() const;

private:
    Form::FormItem *m_FormItem;
    Editor::TextEditor* m_Editor;
    QString m_OriginalValue;
};

}  // End namespace BaseWidgets

#endif // TEXTEDITORFACTORY_H
