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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "word.h"
#include "dict.h"

Word *dict;
SortedWords dict_words = {0, NULL};

static void create_dict_file(char *path);

void create_dict_file(char *path)
{
 mode_t mode_0755 = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
 struct stat st;
 FILE *fdict;
 char file[PATH_LENGTH] = {0};

 if (stat(path, &st)) {
	fprintf(stderr, "%s: Creating directory\n", path);
	if (mkdir(path, mode_0755)) {
		perror(path);
		exit(1);
	}
 }
 strcat(file, path);
 strcat(file, DICT_FILE);
 fprintf(stderr, "%s: Creating blank dictionary file\n", file);
	if (!(fdict = fopen(file, "w"))) {
		perror(file);
		exit(1);
	}
	fclose(fdict);
}

Word *load_dict()
{
 FILE *fdict;
 Word *root = NULL;
 char word[WORDLENGTH] = {0};
 int i;

 if (!(fdict = fopen(dictfile, "r"))) {
	perror(dictfile);
	create_dict_file(optpath);
	if (!(fdict = fopen(dictfile, "r"))) {
		perror(dictfile);
		exit(1);
	}
 }
 while (!feof(fdict)) {
	fgets(word, WORDLENGTH, fdict);
	if (strcmp(word, "\0") == 0)		/*to not process final blank line*/
		break;
	word[strlen(word)-1] = '\0';
	root = add_word(root, word);
	for (i = 0; i < WORDLENGTH; i++)
		word[i] = 0;
 }
 fclose(fdict);
 dict_words = get_sorted(root);
 return root;
}

void save_dict(FILE *file, Word *root)
{
 if (root != NULL) {
	fprintf(file, "%s\n", root->word);
	save_dict(file, root->lsibl);
	save_dict(file, root->rsibl);	
 }
}

