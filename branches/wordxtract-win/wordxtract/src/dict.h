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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#define DICT_FILE "/dict"
#define IN_DICT 1
#define NOT_IN_DICT 0

extern Word *dict;
extern SortedWords dict_words;

Word *load_dict();
void save_dict(FILE *, Word *);

#endif /*DICT_H*/
