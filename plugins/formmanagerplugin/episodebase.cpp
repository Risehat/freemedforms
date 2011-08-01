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
#include "episodebase.h"
#include "constants_db.h"
#include "iformio.h"
#include "subforminsertionpoint.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include <QFont>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("EpisodeBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}

//namespace Patients {
//namespace Internal {
//class EpisodeBasePrivate
//{
//public:
//    EpisodeBasePrivate(EpisodeBase *parent = 0) : q(parent) {}
//    ~EpisodeBasePrivate () {}
//
//    void checkDatabaseVersion()
//    {}
//
//private:
//    EpisodeBase *q;
//};
//}
//}

EpisodeBase *EpisodeBase::m_Instance = 0;
bool EpisodeBase::m_initialized = false;

EpisodeBase *EpisodeBase::instance()
{
    if (!m_Instance) {
        m_Instance = new EpisodeBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

EpisodeBase::EpisodeBase(QObject *parent) :
        QObject(parent), Utils::Database()
//        d_prt(new EpisodeBasePrivate(this))
{
    setObjectName("EpisodeBase");

    using namespace Form::Constants;

    addTable(Table_EPISODES, "EPISODES");
    addField(Table_EPISODES, EPISODES_ID, "EPISODE_ID", FieldIsUniquePrimaryKey);
    addField(Table_EPISODES, EPISODES_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_EPISODES, EPISODES_LK_TOPRACT_LKID, "LK_TOPRACT_LKID", FieldIsInteger);
    addField(Table_EPISODES, EPISODES_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_EPISODES, EPISODES_FORM_PAGE_UID, "FORM_PAGE_UID", FieldIsUUID);
    addField(Table_EPISODES, EPISODES_LABEL, "LABEL", FieldIsShortText);
    addField(Table_EPISODES, EPISODES_DATE, "DATE", FieldIsDate);
    addField(Table_EPISODES, EPISODES_DATEOFCREATION, "DATECREATION", FieldIsDate);
    addField(Table_EPISODES, EPISODES_DATEOFMODIFICATION, "DATEMODIF", FieldIsDate);
    addField(Table_EPISODES, EPISODES_DATEOFVALIDATION, "DATEVALIDATION", FieldIsDate);
    addField(Table_EPISODES, EPISODES_VALIDATED, "VALIDATED", FieldIsBoolean);
    addIndex(Table_EPISODES, EPISODES_ID);
    addIndex(Table_EPISODES, EPISODES_PATIENT_UID);
    addIndex(Table_EPISODES, EPISODES_FORM_PAGE_UID);

    /** \todo code here : add a valid field for the deletion of episodes */

    addTable(Table_EPISODE_CONTENT, "EPISODES_CONTENT");
    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_ID, "CONTENT_ID", FieldIsUniquePrimaryKey);
    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_EPISODE_ID, "EPISODE_ID", FieldIsLongInteger);
    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_XML, "XML_CONTENT", FieldIsBlob);
    addIndex(Table_EPISODE_CONTENT, EPISODE_CONTENT_ID);
    addIndex(Table_EPISODE_CONTENT, EPISODE_CONTENT_EPISODE_ID);
    /** \todo code here : add a valid field for the deletion of episodes content */

    addTable(Table_FORM, "FORM_FILES");
    addField(Table_FORM, FORM_ID,      "ID", FieldIsUniquePrimaryKey);
    addField(Table_FORM, FORM_VALID,   "VALID", FieldIsBoolean);
    addField(Table_FORM, FORM_GENERIC, "GENERIC", FieldIsShortText);
    addField(Table_FORM, FORM_PATIENTUID, "PATIENT", FieldIsUUID);
    // Uuid of the form to insert
    addField(Table_FORM, FORM_SUBFORMUID, "SUBUID", FieldIsShortText);
    // Insertion point = formuuid where to insert the form
    addField(Table_FORM, FORM_INSERTIONPOINT, "IP", FieldIsShortText);
    addField(Table_FORM, FORM_INSERTASCHILD, "CHILD", FieldIsBoolean, "true");
    addField(Table_FORM, FORM_APPEND, "APPEND", FieldIsBoolean, "false");
    addIndex(Table_FORM, FORM_ID);
    addIndex(Table_FORM, FORM_PATIENTUID);
    addIndex(Table_FORM, FORM_SUBFORMUID);
    addIndex(Table_FORM, FORM_INSERTIONPOINT);

    /** \todo code here : manage user access restriction */
    /** \todo code here : add index */

    // Version
    addTable(Table_VERSION, "VERSION");
    addField(Table_VERSION, VERSION_TEXT, "VERSION", FieldIsShortText);

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

EpisodeBase::~EpisodeBase()
{
//    if (d) {
//        delete d;
//        d = 0;
//    }
}

bool EpisodeBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    createConnection(Constants::DB_NAME, Constants::DB_NAME,
                     settings()->databaseConnector(),
                     Utils::Database::CreateDatabase);

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    //    d->checkDatabaseVersion();

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    m_initialized = true;
    return true;
}

bool EpisodeBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__EPISODE_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__EPISODE_CREATOR"))
            QSqlDatabase::removeDatabase("__EPISODE_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tr("Please contact dev team."));
            return false;
        }
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Add version number
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    populateWithDefaultValues();

    return true;
}

/** \brief Populate the database with the default value after its creation. */
void EpisodeBase::populateWithDefaultValues()
{
    // set default patient FormFile
    setGenericPatientFormFile(QString("%1/%2").arg(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH).arg(Core::Constants::S_DEF_PATIENTFORMS_FILENAME));
}

void EpisodeBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    init();
}

/**
  \brief Store the central patient form file into the database.
  This Form File will be used for all patient as central form. Some sub-forms can then be added.
*/
bool EpisodeBase::setGenericPatientFormFile(const QString &absPathOrUid)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__)) {
        return false;
    }
    QHash<int, QString> where;
    where.insert(Constants::FORM_GENERIC, QString("IS NOT NULL"));
    if (count(Constants::Table_FORM, Constants::FORM_GENERIC, getWhereClause(Constants::Table_FORM, where))) {
        // update
        QSqlQuery query(DB);
        QString req = prepareUpdateQuery(Constants::Table_FORM, Constants::FORM_GENERIC, where);
        query.prepare(req);
        query.bindValue(0, absPathOrUid);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
    } else {
        // save
        QSqlQuery query(DB);
        QString req = prepareInsertQuery(Constants::Table_FORM);
        query.prepare(req);
        query.bindValue(Constants::FORM_ID, QVariant());
        query.bindValue(Constants::FORM_VALID, 1);
        query.bindValue(Constants::FORM_GENERIC, absPathOrUid);
        query.bindValue(Constants::FORM_PATIENTUID, QVariant());
        query.bindValue(Constants::FORM_SUBFORMUID, QVariant());
        query.bindValue(Constants::FORM_INSERTIONPOINT, QVariant());
        query.bindValue(Constants::FORM_INSERTASCHILD, QVariant());
        query.bindValue(Constants::FORM_APPEND, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
    }
    return true;
}

/**
  \brief Return the central patient form file into the database.
  This Form File will be used for all patient as central form. Some sub-forms can then be added.
*/
QString EpisodeBase::getGenericFormFile()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__)) {
        return QString();
    }
    QHash<int, QString> where;
    where.insert(Constants::FORM_GENERIC, QString("IS NOT NULL"));
    where.insert(Constants::FORM_VALID, QString("=1"));
    QSqlQuery query(DB);
    QString req = select(Constants::Table_FORM, Constants::FORM_GENERIC, where);
    QString path;
    if (query.exec(req)) {
        if (query.next()) {
            path = query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
        return QString();
    }
    return path;
}

/**
  Return all sub-form additions.
  \sa Form::SubFormInsertionPoint, Form::FormManager::insertSubForm()
*/
QVector<Form::SubFormInsertionPoint> EpisodeBase::getSubFormFiles()
{
    QVector<SubFormInsertionPoint> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__)) {
        return toReturn;
    }
    QHash<int, QString> where;
    where.insert(Constants::FORM_GENERIC, QString("IS NULL"));
    where.insert(Constants::FORM_VALID, QString("=1"));
    where.insert(Constants::FORM_PATIENTUID, QString("='%1'").arg(patient()->data(Core::IPatient::Uid).toString()));
    QSqlQuery query(DB);
    QString req = select(Constants::Table_FORM, QList<int>()
                         << Constants::FORM_SUBFORMUID
                         << Constants::FORM_INSERTIONPOINT
                         << Constants::FORM_INSERTASCHILD
                         << Constants::FORM_APPEND, where);
    if (query.exec(req)) {
        while (query.next()) {
            QString insertUid = query.value(1).toString();
            insertUid.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
            insertUid.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
            SubFormInsertionPoint point(insertUid, query.value(0).toString());
            point.setAddAsChild(query.value(2).toBool());
            point.setAppendToForm(query.value(3).toBool());
            toReturn << point;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

/**
  Save subForm insertions to the database.
  \sa Form::SubFormInsertionPoint, Form::FormManager::insertSubForm()
*/
bool EpisodeBase::addSubForms(const QVector<SubFormInsertionPoint> &insertions)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__)) {
        return false;
    }
    // save
    bool success = true;
    QSqlQuery query(DB);
    for(int i = 0; i < insertions.count(); ++i) {
        query.prepare(prepareInsertQuery(Constants::Table_FORM));
        query.bindValue(Constants::FORM_ID, QVariant());
        query.bindValue(Constants::FORM_VALID, 1);
        query.bindValue(Constants::FORM_GENERIC, QVariant());
        query.bindValue(Constants::FORM_PATIENTUID, patient()->data(Core::IPatient::Uid));
        query.bindValue(Constants::FORM_SUBFORMUID, insertions.at(i).subFormUid());
        query.bindValue(Constants::FORM_INSERTIONPOINT, insertions.at(i).receiverUid());
        query.bindValue(Constants::FORM_INSERTASCHILD, insertions.at(i).addAsChild());
        query.bindValue(Constants::FORM_APPEND, insertions.at(i).appendToForm());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            success = false;
        }
        query.finish();
    }
    return success;
}


void EpisodeBase::toTreeWidget(QTreeWidget *tree)
{
    Database::toTreeWidget(tree);
    QString uuid = user()->uuid();
    QHash<int, QString> where;
    where.clear();
    /** \todo here */
//    where.insert(Constants::LK_TOPRACT_PRACT_UUID, QString("='%1'").arg(uuid));
//    QString req = select(Constants::Table_LK_TOPRACT, Constants::LK_TOPRACT_LKID, where);
//    where.clear();
//    where.insert(Constants::IDENTITY_LK_TOPRACT_LKID, QString("IN (%1)").arg(req));
//    req = getWhereClause(Constants::Table_IDENT, where);
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "Episodes count");
    db->setFont(0, bold);
    new QTreeWidgetItem(db, QStringList() << "Total episodes" << QString::number(count(Constants::Table_EPISODES, Constants::EPISODES_ID)));
    tree->expandAll();
}
