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

#include <stdlib.h>
#include <stdio.h>
#include "plaintext.h"
#include "engparser.h"

static char *get_line(FILE *);

/*TODO: distinguish more than one of parse errors*/
int process_plain_text(FILE *text)
{
 int parse_res = 0;
 char *phrase = NULL;

 while (!feof(text)) {
	phrase = get_line(text);
	parse_res = parseengphrase(phrase)||parse_res;
	free(phrase);
 }
 return parse_res;
}

static char *get_line(FILE *name)
{
 const size_t buflen = 100;
 char c = '\0';
 char *buffer = NULL;
 unsigned int bufcnt = 1, length = 0;

 do {
	buffer = realloc((char*)(buffer - (bufcnt - 1)*buflen), buflen*bufcnt);
	buffer = buffer + (bufcnt - 1)*buflen;
	length = 0;
	do {
		c = fgetc(name);
		*buffer = c;
		buffer++;
		length++;
		if (c == EOF)
			*(buffer-1) = '\0';
		if (c == '\n') {
			/*we don't know whether there is enough space in buffer for '\0'*/
			if (!(length < buflen)) {
				bufcnt++;
				buffer = realloc((char*)(buffer - (bufcnt - 1)*buflen), buflen*bufcnt);
				buffer = buffer + (bufcnt - 1)*buflen;
				length = 0;
			}
			*buffer = '\0';
			break;
		} else if (c == '\0')
			break;
	} while ((c != EOF)&&(length < buflen));
	bufcnt++;
 } while ((c != EOF)&&(c != '\n')&&(c != '\0'));
 buffer = buffer - (bufcnt - 2)*buflen - length;
 return buffer;
}

