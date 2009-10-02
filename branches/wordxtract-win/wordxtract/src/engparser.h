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

#ifndef ENG_PARSER_H
#define ENG_PARSER_H

#define PHRASE_PARSED 0
#define PHRASE_NOT_PARSED 1
#define WORD_INCLUDED 0
#define WORD_NOT_INCLUDED 1

typedef struct {
	char symbol;
	int excl_symbolled;
	int excl_w_starting;
	int excl_w_middle;
	int excl_w_ending;
	int excl_word_after_symb;
} Parseoptions;

extern int print_sentences;
/* Parsing options */
extern int excl_w_capital;
extern int lower_first_capital;
extern Parseoptions hyphen;
extern Parseoptions quote;

/*
 * Recieves: line terminated by "\n\0"
 *
 * Words with "-" we'll be processed as a whole word
 *	And also we suppose that each sentence starts with capital letter 
 *	and other words in the middle of the sentence which starts with 
 *	capital letter is name (peoples, towns, I, etc...).
 * Single quoted sentences are not allowed. It's gramatically incorrect
 */
int parseengphrase(char *);
int parse_eng_word(char *word, Parseoptions opt);

#endif /*ENG_PARSER_H*/
