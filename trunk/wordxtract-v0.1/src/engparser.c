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
#include "main.h"
#include "word.h"
#include "engparser.h"
#include "dict.h"

int lower_first_capital = 1;
int excl_w_capital = 1;
Parseoptions hyphen = {'-', 0, 1, 0, 1, 0};
Parseoptions quote = {'\'', 0, 1, 1, 0, 0};

/* It deletes first and last symbol in the word*/
static void del_first_n_last(char *);

int parseengphrase(char *phrase)
{
 extern Parseoptions hyphen;
 extern Parseoptions quote;
 int sentence_start = 1;
 int phrase_len;
 char *word;
 int i, k;

 if (!g_utf8_validate(phrase, -1, NULL))
	return PHRASE_NOT_PARSED;
 phrase_len = strlen(phrase) + 1;
 word = malloc(phrase_len);
 for (i = 0; i < phrase_len; i++)
	word[i] = 0;
 i = 0;
 add_sentence(phrase);
 for (i = 0, k = 0; phrase[k] != '\0'; k++) {
	if ((isalpha(phrase[k]))||(phrase[k] == '\'')||(phrase[k] == '-')) {
		word[i++] = phrase[k];
		if ((lower_first_capital)&&(sentence_start)&&(phrase[k] != '\'')&&(phrase[k] != '-')) {
			word[i-1] = tolower(word[i-1]);
			sentence_start = 0;
		}
	} else {
		if ((*word != '\0')&&!parse_eng_word(word, quote)&&
			!parse_eng_word(word, hyphen)&&!is_in_dict(word, dict)) 
			words = add_word(words, word);
		sentence_start = sentence_start||(phrase[k] == '.')||(phrase[k] == '!')||(phrase[k] == '?');
		for (i = 0; i < phrase_len; i++)
			word[i] = 0;
		i = 0;
	}
 }
 free(word);
 return PHRASE_PARSED;
}

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

