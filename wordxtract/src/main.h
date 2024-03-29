/*
 * This file is part of WordXtract.
 *
 * Copyright (C) 2009 Borisov Alexandr
 *
 * WordXtract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WordXtract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WordXtract.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*internationalization support*/
#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext(String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) String
#endif
#else /*NLS is disabled*/
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(String) (String)
#define dcgettext(String) (String)
#define bindtextdomain(Domain, Directory) (Domain)
#define bind_textdomain_codeset(Domain, Codeset) (Codeset)
#endif /*ENABLE_NLS*/

/*program messages*/
#define ERR_MSG_CAPTION _("Error")
#define WARN_MSG_CAPTION _("Warning")
#define CONFIRM_MSG_CAPTION _("Confirmation")
#define ERR_READING_FILE _("Could not open file ")
#define ERR_SAVING_WORDS _("Could not save words to file ")
#define ERR_SAVING_TEXT _("Could not save text to file ")
#define WARN_NON_UTF8_CHARS _("File you've chosen contains non UTF-8 characters. Result can be unpredictable!")
#define WARN_ALREADY_EXISTS _(" already exists. Do you want to owerwrite it?")
#define CONFIRM_QUIT_MSG _("Are you sure to exit?")

/*other*/
#define PATH_LENGTH 50
#define PROGNAME "WordXtract"
#ifdef WIN32
#define OPT_FOLDER "../options"
#else
#define OPT_FOLDER "/.wordxtract"
#endif
#define CONF_FILE "/config"
#define OPTION_LENGTH 30

typedef enum {ENG = 0} Language;
typedef struct {
	unsigned int columns;
	unsigned int col_width;
} SaveOpt;

/*application settings*/
extern int exit_query;
extern Language lang;
extern SaveOpt save_user_words;

extern char optpath[PATH_LENGTH];
extern char dictfile[PATH_LENGTH];

void read_config();
void write_config();

#endif /*MAIN_H*/
