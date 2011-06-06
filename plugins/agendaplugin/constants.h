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
#ifndef AGENDA_CONSTANTS_H
#define AGENDA_CONSTANTS_H


namespace Agenda {
namespace Constants {

    // User Database connection
    const char * const DB_NAME           = "agenda";
    const char * const DB_VERSION        = "0.1";

    // For database management
    /** \brief Enumerates all the tables of the database */
    enum Tables
    {
        Table_CALENDAR = 0, Table_USERCALENDARS,
        Table_COMMON, Table_EVENTS, Table_CYCLINGEVENTS,
        Table_EVENTTYPES, Table_EVENTSTATUS, Table_USER_WORKINGTIME,
        Table_VERSION,
        Table_MaxParam
    };

    enum fieldsUSERCALENDARS {
        USERCAL_ID = 0,
        USERCAL_USER_UUID,
        USERCAL_CAL_ID
    };

    enum FieldsCALENDAR {
        CAL_ID = 0,
        CAL_ISVALID,
        CAL_SITEUID,
        CAL_LABEL,
        CAL_XMLOPTIONS
    };

    enum fieldsCOMMON_FOR_EVENTS {
        COMMON_ID = 0,
        COMMON_CAT_ID,
        COMMON_TYPE_ID,
        COMMON_STATUS_ID, // (like draft, validated, rejected, need more info, need to send letter…)
        COMMON_SITE_UID, // (for a later use)
        COMMON_ISPRIVATE,
        COMMON_PASSWORD, // (private events can be protected by a password)
        COMMON_ISBUSY,
        COMMON_ISAGROUPEVENT, // (event can be defined by and for a group of users)
        COMMON_LABEL,
        COMMON_FULLCONTENT,
        COMMON_TEXTUAL_SITE
    };

    enum fieldsEVENTS {
        EVENT_ID = 0,
        EVENT_CAL_ID,
        EVENT_COMMON_ID,
        EVENT_PATIENT_UID,
        EVENT_ISNOTRELATEDTOPATIENT,
        EVENT_ISVALID,
        EVENT_DATESTART,
        EVENT_DATEEND,
        EVENT_XMLVIEWOPTIONS, // (color, margins, spacing…)
        EVENT_XMLCALOPTIONS // (can be changed…)
    };

    enum fieldsCYCLINGEVENTS {
        CYCLING_ID = 0,
        CYCLING_CALENDAR_ID,
        CYCLING_COMMON_ID,
        CYCLING_PATIENT_UID,
        CYCLING_ISNOTRELATEDTOPATIENT,
        CYCLING_ISVALID,
        CYCLING_DATESTART,
        CYCLING_DATEEND,
        CYCLING_REPEATINTERVAL,
        CYCLING_REPEATSCHEMA,
        CYCLING_REPEATTIMES // (eg repeat only seven times)
    };

    enum fieldsEVENTTYPES {  // ?? hard coded ??
        TYPE_ID = 0,
        TYPE_LABEL,
        TYPE_DEFAULT_DURING_TIME,
        TYPE_COLOR,
        TYPE_XML_OPTIONS
    };

    enum fieldsEVENTSTATUS {  // ?? hard coded ??
        STATUS_ID,
        STATUS_LABEL
    };

    enum fieldsUSER_WORKINGTIME {
//        Define here the user vacancies, working time, time for each types
    };

    enum VERSION_Fields {
        VERSION_ACTUAL = 0
    };

}  // End Constants
}  // End Agenda

#endif // AGENDA_CONSTANTS_H
