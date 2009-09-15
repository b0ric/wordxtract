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

#ifndef WORD_H
#define WORD_H

/* reffering to the Wikipedia, words, in common
 * language, aren't longer than 22 letters
 */
#define WORDLENGTH 25

struct words {
	char *word;
	struct words *lsibl;
	struct words *rsibl;
};
typedef struct words Word;
typedef struct {
	unsigned int count;
	char **by_az;
} SortedWords;

/*binary tree of words from parsed file*/
extern Word *words;
/*sorted array of words*/
extern SortedWords user_words;

Word *add_word(Word *, char *);
void del_word(Word **, char *);
void copy_words(Word *, Word **);
void save_words(FILE *, SortedWords, SaveOpt);
unsigned int get_words_count(const Word *);
void free_words(Word *);
SortedWords get_sorted(Word *);

#endif /*WORD_H*/
