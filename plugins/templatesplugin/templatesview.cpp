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
#include "templatesview.h"
#include "templatesview_p.h"
#include "ui_templatesview.h"
#include "templatesmodel.h"
#include "templatebase.h"
#include "itemplates.h"
#include "itemplateprinter.h"
#include "constants.h"
#include "templateseditdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QObject>
#include <QWidget>
#include <QToolBar>
#include <QSpacerItem>
#include <QTreeWidget>

#include <QDebug>

using namespace Templates;
using namespace Templates::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::UniqueIDManager *uid() {return Core::ICore::instance()->uniqueIDManager();}
static inline Core::ContextManager *contextManager() {return Core::ICore::instance()->contextManager();}

static inline QAction *registerAction(const QString &objName, Core::ActionContainer *menu, const QString &iconName,
                                      const QString &actionName, const QString &group, const QString &tr,
                                      const QList<int> &context, QObject *parent)
{
    QAction *toReturn = new QAction(parent);
    toReturn->setObjectName(objName);
    toReturn->setIcon(theme()->icon(iconName));
    Core::Command *cmd = actionManager()->registerAction(toReturn, actionName, context);
    cmd->setTranslations(tr);
    menu->addAction(cmd, group);
    return toReturn;
}


namespace TemplatesViewConstants
{
    static const char* const C_BASIC_EDIT         = "context.TemplatesView.Edit";
    static const char* const C_BASIC_ADD          = "context.TemplatesView.Add";
    static const char* const C_BASIC_REMOVE       = "context.TemplatesView.Remove";
    static const char* const C_BASIC_SAVE         = "context.TemplatesView.Save";
    static const char* const C_BASIC_PRINT        = "context.TemplatesView.Print";
    static const char* const C_BASIC_LOCK         = "context.TemplatesView.Lock";
}

/////////////////////////////////////////////////////////////////////////// List View Manager
TemplatesViewManager *TemplatesViewManager::m_Instance = 0;

TemplatesViewManager *TemplatesViewManager::instance(QObject *parent)
{
    if (!m_Instance) {
        if (!parent) {
            m_Instance = new TemplatesViewManager(qApp);
        } else {
            m_Instance = new TemplatesViewManager(parent);
        }
    }
    return m_Instance;
}

TemplatesViewManager::TemplatesViewManager(QObject *parent) : TemplatesViewActionHandler(parent)
{
    if (contextManager())
        connect(contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
}

void TemplatesViewManager::updateContext(Core::IContext *object)
{
//    qWarning() << "*** TemplatesViewManager::updateContext ***";
//    if (object)
//        qWarning() << object;
    Templates::TemplatesView *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }
        view = qobject_cast<Templates::TemplatesView *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        TemplatesViewActionHandler::setCurrentView(view);
    }
}



/////////////////////////////////////////////////////////////////////////// Action Handler
TemplatesViewActionHandler::TemplatesViewActionHandler(QObject *parent) :
        QObject(parent),
        aAdd(0),
        aRemove(0),
        aEdit(0),
        aPrint(0),
        aSave(0),
        aLocker(0),
        aDatabaseInfos(0),
        m_CurrentView(0),
        m_IsLocked(settings()->value(Constants::S_LOCKCATEGORYVIEW).toBool())
{
    if (!actionManager())
        return;

    QList<int> editContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_EDIT);
    QList<int> lockContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_LOCK);
    QList<int> addContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_ADD);
    QList<int> removeContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_REMOVE);
    QList<int> saveContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_SAVE);
    QList<int> printContext = QList<int>() << uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_PRINT);

    // Edit Menu and Contextual Menu
    Core::ActionContainer *editMenu = actionManager()->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *cmenu = actionManager()->actionContainer(Core::Constants::M_EDIT_TEMPLATES);
    if (!cmenu) {
        cmenu = actionManager()->createMenu(Core::Constants::M_EDIT_TEMPLATES);
        cmenu->appendGroup(Core::Constants::G_EDIT_TEMPLATES);
        cmenu->appendGroup(Core::Constants::G_EDIT_CATEGORIES);
        cmenu->setTranslations(Trans::Constants::M_EDIT_TEMPLATES_TEXT);
        if (editMenu)
            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_TEMPLATES);
    }

    // Add
    aAdd = registerAction("TemplatesView.aAdd", cmenu, Core::Constants::ICONADD,
                          Core::Constants::A_TEMPLATE_ADD, Core::Constants::G_EDIT_TEMPLATES,
                          Trans::Constants::ADDCATEGORY_TEXT, addContext, this);
    connect(aAdd, SIGNAL(triggered()), this, SLOT(addCategory()));

    // Remove
    aRemove = registerAction("TemplatesView.aRemove", cmenu, Core::Constants::ICONREMOVE,
                             Core::Constants::A_TEMPLATE_REMOVE, Core::Constants::G_EDIT_TEMPLATES,
                             Trans::Constants::REMOVE_TEXT, removeContext, this);
    connect(aRemove, SIGNAL(triggered()), this, SLOT(removeItem()));

    // Edit
    aEdit = registerAction("TemplatesView.aEdit", cmenu, Core::Constants::ICONEDIT,
                           Core::Constants::A_TEMPLATE_EDIT, Core::Constants::G_EDIT_TEMPLATES,
                           Trans::Constants::M_EDIT_TEXT, editContext, this);
    connect(aEdit, SIGNAL(triggered()), this, SLOT(editCurrentItem()));

    // Edit
    aPrint = registerAction("TemplatesView.aPrint", cmenu, Core::Constants::ICONPRINT,
                            Core::Constants::A_TEMPLATE_PRINT, Core::Constants::G_EDIT_TEMPLATES,
                            Trans::Constants::FILEPRINT_TEXT, printContext, this);
    connect(aPrint, SIGNAL(triggered()), this, SLOT(print()));


    // Save
    aSave = registerAction("TemplatesView.aSave", cmenu, Core::Constants::ICONSAVE,
                           Core::Constants::A_TEMPLATE_SAVE, Core::Constants::G_EDIT_TEMPLATES,
                           Trans::Constants::FILESAVE_TEXT, saveContext, this);
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveModel()));

    // Locker
    aLocker = registerAction("TemplatesView.aLocker", cmenu, Core::Constants::ICONUNLOCK,
                           Core::Constants::A_TEMPLATE_LOCK, Core::Constants::G_EDIT_TEMPLATES,
                           Trans::Constants::UNLOCKED_TEXT, lockContext, this);
    connect(aLocker, SIGNAL(triggered()), this, SLOT(lock()));

    // Database informations
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    if (hmenu) {
        aDatabaseInfos = registerAction("TemplatesView.aDbInfos", hmenu, Core::Constants::ICONABOUT,
                                        Core::Constants::A_TEMPLATE_DATABASEINFORMATIONS,
                                        Core::Constants::G_HELP_DATABASES,
                                        Trans::Constants::TEMPLATES_DATABASE_INFORMATIONS_TEXT,
                                        QList<int>() << Core::Constants::C_GLOBAL_ID, this);
        connect(aDatabaseInfos, SIGNAL(triggered()), this, SLOT(databaseInformations()));
//        contextManager()->updateContext();
    }

    updateActions();
}

void TemplatesViewActionHandler::setCurrentView(TemplatesView *view)
{
//    if (view)
//        qWarning() << "TemplatesViewActionHandler" << view;
    // disconnect old view
    if (m_CurrentView) {
//        disconnect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//                   this, SLOT(templatesViewItemChanged()));
    }
    m_CurrentView = view;
    if (!view) { // this should never be the case
        return;
    }
    // reconnect some actions
//    connect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(templatesViewItemChanged()));
    m_IsLocked = m_CurrentView->isLocked();
    updateActions();
}

void TemplatesViewActionHandler::templatesViewItemChanged()
{
//    bool templateState = m_CurrentView->currentItemIsTemplate();
//    aAdd->setEnabled(!templateState);
//    aRemove->setEnabled(!templateState);
//    aEdit->setEnabled(!templateState);
}

void TemplatesViewActionHandler::updateActions()
{
    aAdd->setEnabled(!m_IsLocked);
    aEdit->setEnabled(!m_IsLocked);
    aRemove->setEnabled(!m_IsLocked);
    Core::Command *cmd = actionManager()->command(Core::Constants::A_TEMPLATE_LOCK);
    if (!m_IsLocked) {
        cmd->setTranslations(Trans::Constants::UNLOCKED_TEXT, Trans::Constants::UNLOCKED_TEXT);
        cmd->action()->setIcon(theme()->icon(Core::Constants::ICONUNLOCK));
    } else {
        cmd->setTranslations(Trans::Constants::LOCKED_TEXT, Trans::Constants::LOCKED_TEXT);
        cmd->action()->setIcon(theme()->icon(Core::Constants::ICONLOCK));
    }
    cmd->retranslate();
}

void TemplatesViewActionHandler::editCurrentItem()
{
    if (m_CurrentView)
        m_CurrentView->editCurrentItem();
}
void TemplatesViewActionHandler::addCategory()
{
    if (m_CurrentView)
        m_CurrentView->addCategory();
}

void TemplatesViewActionHandler::removeItem()
{
    if (m_CurrentView)
        m_CurrentView->removeItem();
}

void TemplatesViewActionHandler::saveModel()
{
    if (m_CurrentView)
        m_CurrentView->saveModel();
}

void TemplatesViewActionHandler::print()
{
    if (m_CurrentView) {
        m_CurrentView->printTemplate();
    }
}

void TemplatesViewActionHandler::lock()
{
    if (m_CurrentView) {
        m_IsLocked = !m_IsLocked;
        m_CurrentView->lock(m_IsLocked);
        updateActions();
    }
}

void TemplatesViewActionHandler::databaseInformations()
{
    QDialog dlg(qApp->activeWindow(), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    Templates::TemplateBase::instance()->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}


namespace Templates {
namespace Internal {
class TemplatesViewPrivate : public QObject
{
    Q_OBJECT
public:
    TemplatesViewPrivate(TemplatesView *parent, Templates::TemplatesView::EditModes editModes) :
            QObject(parent), q(parent), m_Model(0), m_ui(0), m_Context(0),
            m_ToolBar(0)
    {
        // Verify that Manager is instanciated
        Templates::Internal::TemplatesViewManager::instance(this);

        // Create Actions context
        m_Context = new Templates::Internal::TemplatesViewContext(q);
        contextManager()->addContextObject(m_Context);

        // Create toolbar
        m_ToolBar = new QToolBar("Actions", q);
        m_ToolBar->setIconSize(QSize(22,22));
//        Core::ActionContainer *cmenu = actionManager()->actionContainer(Core::Constants::M_EDIT_TEMPLATES);
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_ADD)->action());
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_REMOVE)->action());
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_EDIT)->action());
        m_ToolBar->addSeparator();
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_PRINT)->action());
        m_ToolBar->addSeparator();
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_SAVE)->action());
        QWidget *w = new QWidget(m_ToolBar);
        QSpacerItem *spacer = new QSpacerItem(20,10,QSizePolicy::Expanding, QSizePolicy::Fixed);
        QHBoxLayout *layout = new QHBoxLayout(w);
        layout->addSpacerItem(spacer);
        m_ToolBar->addWidget(w);
        m_ToolBar->addAction(actionManager()->command(Core::Constants::A_TEMPLATE_LOCK)->action());
        m_ToolBar->setFocusPolicy(Qt::ClickFocus);

        // Create Ui
        m_ui = new Ui::TemplatesView;
        m_ui->setupUi(q);
        m_ui->categoryLayout->insertWidget(1, m_ToolBar);
        m_Model = new TemplatesModel(this);
        m_Model->setObjectName("TemplateViewModel");
        m_ui->categoryTreeView->setModel(m_Model);
        if (settings()->value(Templates::Constants::S_ALWAYSSHOWEXPANDED, true).toBool())
            m_ui->categoryTreeView->expandAll();
        m_ui->categoryTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
        int i;
        for(i=1; i < m_ui->categoryTreeView->model()->columnCount(); ++i) {
            m_ui->categoryTreeView->setColumnHidden(i, true);
        }

        // manage contexts and drag/drop
        manageContexts(editModes);
#if QT_VERSION >= 0x040600
        m_ui->categoryTreeView->setDefaultDropAction(Qt::CopyAction);
#endif
        connect(m_ui->categoryTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
//        connect(m_Model,SIGNAL(modelAboutToBeReset()), this, SLOT(on_ModelAboutToReset()));
        connect(m_Model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(model_rowInserted(const QModelIndex &, int, int)));
    }

    ~TemplatesViewPrivate()
    {
        contextManager()->removeContextObject(m_Context);
        delete m_Context;
        m_Context = 0;
    }

    void manageContexts(Templates::TemplatesView::EditModes editModes)
    {
        m_Context->clearContext();
        m_Context->addContext(Core::Constants::C_GLOBAL_ID);
        if (editModes==0) {
            this->m_ToolBar->hide();
            return;
        }
        if (editModes & Templates::TemplatesView::Save)
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_SAVE));
        if (editModes & Templates::TemplatesView::Add)
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_ADD));
        if (editModes & Templates::TemplatesView::Remove)
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_REMOVE));
        if (editModes & Templates::TemplatesView::Edit) {
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_EDIT));
            m_ui->categoryTreeView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        } else {
            m_ui->categoryTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        }
        if (editModes & Templates::TemplatesView::Print)
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_PRINT));
        if (editModes & Templates::TemplatesView::LockUnlock)
            m_Context->addContext(uid()->uniqueIdentifier(TemplatesViewConstants::C_BASIC_LOCK));
    }

public Q_SLOTS:
    QMenu *getContextMenu()
    {
        QMenu *menu = new QMenu(tkTr(Trans::Constants::TEMPLATES), q);
        QList<QAction *> list;
        list    << actionManager()->command(Core::Constants::A_TEMPLATE_ADD)->action()
                << actionManager()->command(Core::Constants::A_TEMPLATE_REMOVE)->action()
                << actionManager()->command(Core::Constants::A_TEMPLATE_EDIT)->action()
                << actionManager()->command(Core::Constants::A_TEMPLATE_PRINT)->action();
        bool returnMenu = false;
        foreach(QAction *action, list) {
            if (action->isEnabled()) {
                returnMenu = true;
                break;
            }
        }
        if (returnMenu) {
            menu->addActions(list);
            return menu;
        } else
            return 0;
    }
    void contextMenu(const QPoint &p)
    {
        QMenu *pop = getContextMenu();
        if (pop) {
            pop->popup(m_ui->categoryTreeView->mapToGlobal(p));
            pop->exec();
        }
    }
    void on_ModelAboutToReset() {
        /** \todo Store the expanded indexes of the view and restore when view resets. */
    }
    void model_rowInserted(const QModelIndex &parent, int start, int count)
    {
        Q_UNUSED(start);
        Q_UNUSED(count);
        m_ui->categoryTreeView->setExpanded(parent, false);
        m_ui->categoryTreeView->expand(parent);
        m_ui->categoryTreeView->scrollTo(m_Model->index(m_Model->rowCount(parent), 0, parent), QAbstractItemView::EnsureVisible);
    }

public:
    TemplatesView *q;
    TemplatesModel *m_Model;
    Ui::TemplatesView *m_ui;
    TemplatesViewContext *m_Context;
    QToolBar *m_ToolBar;
};
}
}

TemplatesView::TemplatesView(QWidget *parent, int viewContent, EditModes editModes) :
    QWidget(parent),
    d(new TemplatesViewPrivate(this, editModes))
{
    lock(settings()->value(Constants::S_LOCKCATEGORYVIEW).toBool());
    if (viewContent == CategoriesOnly)
        d->m_Model->categoriesOnly();
    QFont font;
    font.fromString(settings()->value(Constants::S_FONT, QFont().toString()).toString());
    d->m_ui->categoryTreeView->setFont(font);
}

TemplatesView::~TemplatesView()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void TemplatesView::setViewContent(int viewContent)
{
    if (viewContent == CategoriesOnly)
        d->m_Model->categoriesOnly();
}

void TemplatesView::setEditMode(EditModes editModes)
{
    d->manageContexts(editModes);
}

TemplatesModel *TemplatesView::templatesModel() const
{
    return d->m_Model;
}

QModelIndex TemplatesView::currentItem() const
{
    return d->m_ui->categoryTreeView->currentIndex();
}

QItemSelectionModel *TemplatesView::selectionModel() const
{
    return d->m_ui->categoryTreeView->selectionModel();
}

void TemplatesView::expandAll() const
{
    d->m_ui->categoryTreeView->expandAll();
}

void TemplatesView::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    d->m_ui->categoryTreeView->setSelectionMode(mode);
}

bool TemplatesView::currentItemIsTemplate() const
{
    QModelIndex index = d->m_ui->categoryTreeView->selectionModel()->currentIndex();
    if (index.isValid()) {
        return d->m_Model->isTemplate(index);
    }
    return false;
}

void TemplatesView::lock(bool toLock)
{
//    d->m_ui->categoryTreeView->setDragEnabled(!toLock);
    d->m_ui->categoryTreeView->setAcceptDrops(!toLock);
    d->m_ui->categoryTreeView->setDropIndicatorShown(!toLock);
    if (toLock) {
        d->m_ui->categoryTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else {
        d->m_ui->categoryTreeView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    }
}

bool TemplatesView::isLocked() const
{
    return (!d->m_ui->categoryTreeView->acceptDrops());
}

void TemplatesView::addCategory()
{
    /** \todo Manage USER and MIMETYPE */
    QModelIndex idx = d->m_ui->categoryTreeView->currentIndex();
    if (!d->m_ui->categoryTreeView->selectionModel()->hasSelection())
        idx = QModelIndex();
//    qWarning() << idx.isValid() << idx.data() << d->m_ui->categoryTreeView->currentIndex().data();
//    if (!idx.isValid())
//        return;
    while (!d->m_Model->isCategory(idx)) {
        idx = idx.parent();
    }

    d->m_Model->insertRow(d->m_Model->rowCount(idx), idx);
    d->m_Model->setData(d->m_Model->index(d->m_Model->rowCount(idx)-1, Constants::Data_IsNewlyCreated, idx), true);
    QModelIndex newItem = d->m_Model->index(d->m_Model->rowCount(idx)-1, 0, idx);
    d->m_ui->categoryTreeView->expand(idx);
    d->m_ui->categoryTreeView->scrollTo(newItem);
    d->m_ui->categoryTreeView->edit(newItem);
}

void TemplatesView::removeItem()
{
    if (!d->m_ui->categoryTreeView->selectionModel()->hasSelection())
    return;

    QModelIndexList list1 = d->m_ui->categoryTreeView->selectionModel()->selectedIndexes();
    if (!list1.count())
        return;
    QList<QPersistentModelIndex> list2;
    QString names;
    QModelIndex last;
    foreach(const QModelIndex &idx, list1) {
        if ((last.row() == idx.row()) && (last.parent()==idx.parent()))
            continue;
        last = idx;
        if (idx.isValid()) {
            list2 << QPersistentModelIndex(idx);
            names.append(d->m_Model->index(idx.row(), Constants::Data_Label, idx.parent()).data().toString() + ", ");
        }
    }
    names.chop(2);

    // Confirmation dialog
    bool yes = true;
    if (settings()->value(Constants::S_PROMPTFORDELETION).toBool()) {
        yes = Utils::yesNoMessageBox(tr("About to delete %1.").arg(names),
                                          tr("Do you really want to remove %1 from your templates' list ?").arg(names),
                                          tr("This action is definitive and all informations will be lost. Children of this item"
                                             " will be deleted as well."));
    }
    if (yes) {
        foreach(const QPersistentModelIndex &idx, list2) {
            if (idx.isValid())
                d->m_Model->removeRow(idx.row(), idx.parent());
        }
    }
}

void TemplatesView::editCurrentItem()
{
    if (!d->m_ui->categoryTreeView->selectionModel()->hasSelection())
        return;

    QModelIndex idx = d->m_ui->categoryTreeView->selectionModel()->currentIndex();
    if (!idx.isValid())
        return;

    TemplatesEditDialog dlg(this);
    dlg.setModel(d->m_Model);
    dlg.setModelIndex(idx);
    dlg.exec();
}

void TemplatesView::saveModel()
{
    d->m_Model->submit();
}

bool TemplatesView::printTemplate()
{
    // Get selected items
    if (!d->m_ui->categoryTreeView->selectionModel()->hasSelection())
        return true;

    QList<const ITemplate *> selection;
    foreach(const QModelIndex &idx, d->m_ui->categoryTreeView->selectionModel()->selectedRows(0)) {
        const ITemplate *t = d->m_Model->getTemplate(idx);
        if (!selection.contains(t))
            selection << t;
    }

    // Get all ITemplatePrinter object from pluginsManager
    QList<ITemplatePrinter *> printers = ExtensionSystem::PluginManager::instance()->getObjects<ITemplatePrinter>();

    // Ask mime of each
    QMultiHash<const ITemplatePrinter *, const ITemplate *> printer_templates;
    foreach(const ITemplate *t, selection) {
        foreach(const ITemplatePrinter *prt, printers) {
            if (t->contentMimeTypes().contains(prt->mimeType())) {
                printer_templates.insertMulti(prt, t);
                break;
            }
        }
    }

    // Print the selected templates
    foreach(const ITemplatePrinter *prt, printer_templates.uniqueKeys()) {
        if (!prt->printTemplates(printer_templates.values(prt)))
            return false;
    }
    return true;
}


void TemplatesView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

#include "templatesview.moc"
