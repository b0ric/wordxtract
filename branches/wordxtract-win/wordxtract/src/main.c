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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WordXtract. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "word.h"
#include "engparser.h"
#include "mainwin.h"
#include "dict.h"

int exit_query = 1;
Language lang;
SaveOpt save_user_words = {2, 40};
char optpath[PATH_LENGTH] = {0};
char dictfile[PATH_LENGTH] = {0};
char conffile[PATH_LENGTH] = {0};

void file_error(char *, GtkWidget *, GtkMessageType, char *, char *);
static void fill_vars();

int main(int argc, char *argv[])
{
 FILE *fdict;

 gtk_init(&argc, &argv);
 bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
 bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
 textdomain(GETTEXT_PACKAGE);
 fill_vars();
 dict = load_dict();
 lang = ENG;
 create_main_window();
 gtk_widget_show(main_window);
 gtk_main();

 /*termination*/
 if (!(fdict = fopen(dictfile, "w")))
	perror(dictfile);
 else {
	save_dict(fdict, dict);
	fclose(fdict);
 }
 if (dict_words.by_az)
	free(dict_words.by_az);
 if (user_words.by_az)
	free(user_words.by_az);
 if (dict) {
	if (dict->lsibl)
		free_words(dict->lsibl);
	if (dict->rsibl)
		free_words(dict->rsibl);
	free(dict->word);
 }
 return 0;
}

static void fill_vars()
{
 strcpy(optpath, getenv("HOME"));
 strcat(optpath, OPT_FOLDER);
 strcat(dictfile, optpath);
 strcat(dictfile, DICT_FILE);
 strcat(conffile, optpath);
 strcat(conffile, CONF_FILE);
 read_config();
}

void create_config(char *path)
{
 mode_t mode_0755 = 0755; //S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
 struct stat st;
 FILE *conf;
 char file[PATH_LENGTH] = {0};

 if (stat(path, &st)) {
	fprintf(stderr, "%s: Creating directory\n", path);
	if (mkdir(path, mode_0755)) {
		perror(path);
		exit(1);
	}
 }
 strcat(file, path);
 strcat(file, CONF_FILE);
 fprintf(stderr, "%s: Creating configuration file\n", file);
	if (!(conf = fopen(file, "w"))) {
		perror(file);
		exit(1);
	}
 fclose(conf);
}

void read_config()
{
 FILE *conf;

 if (!(conf = fopen(conffile, "r"))) {
	perror(conffile);
	create_config(optpath);
	write_config();
	if (!(conf = fopen(conffile, "r"))) {
		perror(conffile);
		exit(1);
	}
 } else {
	char option[OPTION_LENGTH] = {0};
	char svalue[OPTION_LENGTH] = {0};
	while (fscanf(conf, "%s\t\t%s\n", option, svalue) != EOF) {
		if (!(strcmp(option, "exit_query"))) {
			exit_query = atoi(svalue);
		} else if (!strcmp(option, "columns")) {
			save_user_words.columns = atoi(svalue);
		} else if (!strcmp(option, "field")) {
			save_user_words.col_width = atoi(svalue);
		} else if (!strcmp(option, "language")) {
			lang = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_capital")) {
			excl_w_capital = atoi(svalue);
		} else if (!strcmp(option, "lower_first_capital")) {
			lower_first_capital = atoi(svalue);
		} else if (!strcmp(option, "exclude_quoted")) {
			quote.excl_symbolled = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_first_quote")) {
			quote.excl_w_starting = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_middle_quote")) {
			quote.excl_w_middle = atoi(svalue);
		} else if (!strcmp(option, "exclude_part_after_quote")) {
			quote.excl_word_after_symb = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_end_quote")) {
			quote.excl_w_ending = atoi(svalue);
		} else if (!strcmp(option, "exclude_hyphened")) {
			hyphen.excl_symbolled = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_first_hyphen")) {
			hyphen.excl_w_starting = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_middle_hyphen")) {
			hyphen.excl_w_middle = atoi(svalue);
		} else if (!strcmp(option, "exclude_part_after_hyphen")) {
			hyphen.excl_word_after_symb = atoi(svalue);
		} else if (!strcmp(option, "exclude_with_end_hyphen")) {
			hyphen.excl_w_ending = atoi(svalue);
		}
	}
	fclose(conf);
 }
}

void write_config()
{
 FILE *conf;

 if (!(conf = fopen(conffile, "w")))
	perror(conffile);
 else {
	fprintf(conf, "%s\t\t%d\n", "exit_query", exit_query);
	fprintf(conf, "%s\t\t%d\n", "columns", save_user_words.columns);
	fprintf(conf, "%s\t\t%d\n", "field", save_user_words.col_width);
	fprintf(conf, "%s\t\t%d\n", "language", lang);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_capital", excl_w_capital);
	fprintf(conf, "%s\t\t%d\n", "lower_first_capital", lower_first_capital);
	fprintf(conf, "%s\t\t%d\n", "exclude_quoted", quote.excl_symbolled);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_first_quote", quote.excl_w_starting);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_middle_quote", quote.excl_w_middle);
	fprintf(conf, "%s\t\t%d\n", "exclude_part_after_quote", quote.excl_word_after_symb);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_end_quote", quote.excl_w_ending);
	fprintf(conf, "%s\t\t%d\n", "exclude_hyphened", hyphen.excl_symbolled);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_first_hyphen", hyphen.excl_w_starting);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_middle_hyphen", hyphen.excl_w_middle);
	fprintf(conf, "%s\t\t%d\n", "exclude_part_after_hyphen", hyphen.excl_word_after_symb);
	fprintf(conf, "%s\t\t%d\n", "exclude_with_end_hyphen", hyphen.excl_w_ending);
	fclose(conf);
 }
}

void file_error(char *filename, GtkWidget *parent, GtkMessageType msg_type, char *msg_caption, char *msg_text)
{
 char *errstr = malloc(strlen(filename) + strlen(msg_text)+1);
 strcpy(errstr, msg_text);
 strcat(errstr, filename);
 GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(parent), 
										GTK_DIALOG_DESTROY_WITH_PARENT, msg_type,
										GTK_BUTTONS_OK, errstr);
 gtk_window_set_title(GTK_WINDOW(msg), msg_caption);
 gtk_dialog_run(GTK_DIALOG(msg));
 gtk_widget_destroy(msg);
 free(errstr);
}


