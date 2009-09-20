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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "word.h"
#include "engparser.h"
#include "dict.h"

#define STACK_DEPTH 3

int print_sentences = 1;
int lower_first_capital = 1;
int excl_w_capital = 1;
Parseoptions hyphen = {'-', 0, 1, 0, 1, 0};
Parseoptions quote = {'\'', 0, 1, 1, 0, 0};

/* It deletes first and last symbol in the word*/
static void del_first_n_last(char *);
/* stack-related functions */
static void push_word(char *);
static char *get_stack_first();
static void clear_stack();
static int is_stack_full();
static char *get_complex(int);

int parseengphrase(char *phrase)
{
 extern Parseoptions hyphen;
 extern Parseoptions quote;
 int sentence_start = 1;
 int phrase_len;
 char *word, *complex_word = NULL;
 int i, k, j;

 if (!g_utf8_validate(phrase, -1, NULL))
	return PHRASE_NOT_PARSED;
 phrase_len = strlen(phrase) + 1;
 word = malloc(strlen(phrase) + 1);
 for (i = 0; i < phrase_len; i++)
	word[i] = 0;
 i = 0;
 if (print_sentences)
	add_sentence(phrase);
 for (i = 0, k = 0; phrase[k] != '\0'; k++) {
	if ((isalpha(phrase[k]))||(phrase[k] == '\'')||(phrase[k] == '-')) {
		word[i] = phrase[k];
		if ((lower_first_capital)&&(sentence_start)&&(phrase[k] != '\'')&&(phrase[k] != '-')) {
			word[i] = tolower(word[i]);
			sentence_start = 0;
		}
		i++;
	} else {
		if ((*word != '\0')&&!parse_eng_word(word, quote)&&!parse_eng_word(word, hyphen)) {
			if ((is_stack_full())&&(!find_word(dict, get_stack_first())))
				words = add_word(words, get_stack_first());
			push_word(word);
			for (j = 2; j <= STACK_DEPTH; j++) {
				complex_word = get_complex(j);
				if ((complex_word)&&(find_word(dict, complex_word)))
					clear_stack();
				free(complex_word);
			}
		}
		sentence_start = sentence_start||(phrase[k] == '.')||(phrase[k] == '!')||(phrase[k] == '?');
		for (i = 0; i < phrase_len; i++)
			word[i] = 0;
		i = 0;
	}
 }
 free(word);
 /* here we clean the rest of stack
  *
  * NOTE: this is also a disadvantage, cause parser can't handle phrasal verbs 
  * (and etc) which are splitted into two strings
  */
 for (i = 0; i < STACK_DEPTH; i++) {
	if (get_stack_first()&&(!find_word(dict, get_stack_first())))
		words = add_word(words, get_stack_first());
	push_word("");
 }
 clear_stack();
 return PHRASE_PARSED;
}

/* this function decide whether to add word to list or not */
int parse_eng_word(char *word, Parseoptions opt)
{
 extern int excl_w_capital;
 int ends = 0, sts = 0, mids = 0;
 int symbolled = 0;
 int capital = 0;
 int wlength = 0;
 int i;
 
 wlength = strlen(word);
 for (i = 0; word[i] != '\0'; i++) {
	capital = capital||isupper(word[i]);
	if (word[i] == opt.symbol) {
		if (i == 0)
			sts = 1;
		else if (i == wlength-1)
			ends = 1;
		else
			mids = 1;
	}
	if (mids&&opt.excl_word_after_symb&&!opt.excl_w_middle) {
		word[i] = '\0';
		break;
	}
 }
 if (excl_w_capital&&capital)
	return WORD_NOT_INCLUDED;
 symbolled = sts&&ends;
 if (opt.excl_symbolled&&symbolled)
	return WORD_NOT_INCLUDED;
 else if (symbolled) {
	if (wlength < 3)
		return WORD_NOT_INCLUDED;
	del_first_n_last(word);
 }
 if (opt.excl_w_starting&&sts&&!symbolled)
	return WORD_NOT_INCLUDED;
 if (opt.excl_w_middle&&mids)
	return WORD_NOT_INCLUDED;
 if (opt.excl_w_ending&&ends&&!symbolled)
	return WORD_NOT_INCLUDED;
 return WORD_INCLUDED;
}

static void del_first_n_last(char *word)
{
 int i, k;
 int wlength;

 wlength = strlen(word);
 if (wlength == 1) {
	word[1] = '\0';
	return ;
 } else if (wlength == 2) {
	word[1] = word[2] = '\0';
	return ;
 } else {
	for (k = 0, i = 1; i < wlength; i++, k++) {
		word[k] = word[i];
	}
	word[k] = '\0';
 }
}

/***** STACK RELATED DATA AND FUNCTIONS' SECTION *****/

/* this is a FIFO-like data structure to store words sequence */
static char *word_stack[STACK_DEPTH];

static void push_word(char *word)
{
 int i = 0;

 if (word_stack[i])
	free(word_stack[i]);
 for ( ; i < STACK_DEPTH-1; i++) {
	if (word_stack[i+1]) {
		word_stack[i] = malloc(strlen(word_stack[i+1]) + 1);
		strcpy(word_stack[i], word_stack[i+1]);
		free(word_stack[i+1]);
	}
 }
 word_stack[STACK_DEPTH-1] = malloc(strlen(word) + 1);
 strcpy(word_stack[STACK_DEPTH-1], word);
}

static char *get_stack_first()
{
 return word_stack[0];
}

static void clear_stack()
{
 int i;

 for (i = 0; i < STACK_DEPTH; i++) {
	free(word_stack[i]);
	word_stack[i] = NULL;
 }
}

static int is_stack_full()
{
 int i, result = 1;

 for (i = 0; i < STACK_DEPTH-1; i++)
	result = word_stack[0]&&result;
 return result;
}

static char *get_complex(int words_cnt)
{
 char *complex_word = NULL;
 size_t word_len = 0;
 int i, j;

 if (words_cnt > STACK_DEPTH)
	return NULL;
 for (i = STACK_DEPTH-1, j = 0; word_stack[i]&&(i >= 0); i--) 
	j++;
 if (j < words_cnt)
	return NULL;

 for (i = STACK_DEPTH - words_cnt; i < STACK_DEPTH; i++)
	word_len += strlen(word_stack[i]) + 1;
 complex_word = malloc(word_len);
 strcpy(complex_word, "\0");
 for (i = STACK_DEPTH - words_cnt; i < STACK_DEPTH; i++) {
	strcat(complex_word, word_stack[i]);
	if (i != STACK_DEPTH-1) 
		strcat(complex_word, " ");
 }
 return complex_word;
}


