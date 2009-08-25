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

#define PATH_LENGTH 50
#define PROGNAME "WordXtract"
#define OPT_FOLDER "/.wordxtract"
#define CONF_FILE "/config"
#define OPTION_LENGTH 30

typedef enum {ENG = 0} Language;
typedef struct {
	unsigned int columns;
	unsigned int col_width;
} SaveOpt;

/*application settings*/
extern Language lang;
extern SaveOpt save_user_words;

extern char optpath[PATH_LENGTH];
extern char dictfile[PATH_LENGTH];

void read_config();
void write_config();

#endif /*MAIN_H*/
