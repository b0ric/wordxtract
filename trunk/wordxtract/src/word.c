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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "word.h"
#include "dict.h"

Word *words;
SortedWords user_words;

static SortedWords sorted = {0, NULL};

static Word *find_word(Word *, const char *);
/*third argument of find_parent() should be NULL when func is to be called*/
static void find_parent(Word *, const Word *, Word **);
static void sort_words(const Word *);
static Word *get_the_rightest(Word *);
static inline char *wordcpy(char *);
static inline void free_word(Word *);

Word *add_word(Word *root, char *word)
{
 int cond;

 if (!root) {
	root = malloc(sizeof(Word));
	root->word = wordcpy(word);
	root->lsibl = root->rsibl = NULL;
 }
 else {
	cond = strcmp(word, root->word);
	if (cond > 0)
		root->rsibl = add_word(root->rsibl, word);
	else if (cond < 0)
		root->lsibl = add_word(root->lsibl, word);
 }
 return root;
}

void del_word(Word **root, char *word)
{
 Word *node, *parent = NULL;
 Word **parent_leaf = NULL;

 node = find_word(*root, word);
 find_parent(*root, node, &parent);
 if ((parent)&&(parent->lsibl == node))
	parent_leaf = &parent->lsibl;
 else if ((parent)&&(parent->rsibl == node))
	parent_leaf = &parent->rsibl;
 /*NOTE that (root == node) is a special case (without parent)*/
 if ((!node->lsibl)&&(!node->rsibl)) {
	if (parent)
		*parent_leaf = NULL;
	else
		*root = NULL;
	free_word(node);
	return ;
 }
 if ((node->lsibl)&&(!node->rsibl)) {
	if (parent)
		*parent_leaf = node->lsibl;
	else
		*root = node->lsibl;
	free_word(node);
 } else if ((!node->lsibl)&&(node->rsibl)) {
	if (parent)
		*parent_leaf = node->rsibl;
	else
		*root = node->rsibl;
	free_word(node);
 } else {
	Word *temp;
	temp = get_the_rightest(node->lsibl);
	temp->rsibl = node->rsibl;
	if (parent)
		*parent_leaf = node->lsibl;
	else
		*root = node->lsibl;
	free_word(node);
 }
}

void copy_words(Word *src, Word **dst)
{
 if (src) {
	*dst = malloc(sizeof(Word));
	(*dst)->word = wordcpy(src->word);
	(*dst)->lsibl = (*dst)->rsibl = NULL;
	if (src->lsibl)
		copy_words(src->lsibl, &((*dst)->lsibl));
	if (src->rsibl)
		copy_words(src->rsibl, &((*dst)->rsibl));
 }
}

void save_words(FILE *tofile, SortedWords words, SaveOpt options)
{
 int i, j;
 char format[10] = {0};
 
 sprintf(format, "%%-%us", options.col_width);
 i = 0;
 while (i < words.count) {
	for (j = 0; (j < options.columns)&&(i < words.count); j++) {
		if (j == options.columns-1)
			fprintf(tofile, "%s\n", *words.by_az);
		else
			fprintf(tofile, format, *words.by_az);
		words.by_az++;
		i++;
	}
 }
}

unsigned int get_words_count(const Word *root)
{
 unsigned int cnt = 0;

 if (!root)
	return 0;
 else {
	cnt += get_words_count(root->lsibl);
	cnt += get_words_count(root->rsibl);
	cnt++;
	return cnt;
 }
}

static Word *find_word(Word *root, const char *word)
{
 int cond;

 if (root) {
	cond = strcmp(word, root->word);
	if (cond > 0)
		return find_word(root->rsibl, word);
	else if (cond < 0)
		return find_word(root->lsibl, word);
	else
		return root;
 } else
	return NULL;
}

static void find_parent(Word *root, const Word *sibling, Word **parent)
{
 if (root) {
	if (*parent)
		return ;
	if (root->lsibl != sibling)
		find_parent(root->lsibl, sibling, parent);
	else {
		*parent = root;
		return ;	
	}
	if (*parent)
		return ;
	if (root->rsibl != sibling)
		find_parent(root->rsibl, sibling, parent);
	else{
		*parent = root;
		return ;	
	}
 }
}

SortedWords get_sorted(Word *root)
{
 unsigned int cnt;

 cnt = get_words_count(root);
 sorted.count = cnt;
 sorted.by_az = NULL;
 if (cnt) {
	sorted.by_az = malloc(cnt*sizeof(char*));
	sort_words(root);
 	sorted.by_az = sorted.by_az - cnt;
 }
 return sorted;
}

static void sort_words(const Word *root)
{
 if (root) {
	sort_words(root->lsibl);
	*sorted.by_az = root->word;
	sorted.by_az++;
	sort_words(root->rsibl);
 }
}

static Word *get_the_rightest(Word *root)
{
 if (root->rsibl)
	return get_the_rightest(root->rsibl);
 else
	return root;
 /*it's useless but GCC wants it to be*/
 return NULL;
}

void free_words(Word *root)
{
 if (root->lsibl)
	free_words(root->lsibl);
 if (root->rsibl)
	free_words(root->rsibl);
 free_word(root);
}

static inline void free_word(Word *record)
{
 free(record->word);
 free(record);
}

static inline char *wordcpy(char *word)
{
 char *p;
 p = malloc(strlen(word)+1);
 if (p != NULL)
	strcpy(p, word);
 return p;
}

