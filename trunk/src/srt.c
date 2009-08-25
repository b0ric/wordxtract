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

#include <stdio.h>
#include <ctype.h>
#include "main.h"
#include "engparser.h"
#include "srt.h"

static int get_srt_tag_length(char *);

/*
 *	Reads text lines from *.srt file
 *	NOTE that "\r\n" and "\n" line ends are possible
 */
int process_srt(FILE *subtitle)
{
 char line[MAXLINE] = {0};
 int lines;

 while (!feof(subtitle)) {
	 for (lines = 0; lines < 2; lines++) {	 
		 fgets(line, MAXLINE, subtitle); 
		 if (feof(subtitle)) return 0;
	 }
	 fgets(line, MAXLINE, subtitle);
	 process_srt_line(line);
	 fgets(line, MAXLINE, subtitle);
	 if (!((line[0] == '\n')||((line[0] == '\r')&&(line[1] == '\n')))) {
		 process_srt_line(line);	  
	 	 fgets(line, MAXLINE, subtitle);
	 }
 }
 return 0;
}

/*
 *	Recieves: line from *.srt file with final '\n' or "\r\n"
 */
int process_srt_line(char *line)
{
 char phrase[MAXPHRASE] = {0};
 int i;
 int taglength;

 for (i = 0; !((line[0] == '\n')||((line[0] == '\r')&&(line[1] == '\n'))); i++, line++) {
	 while (*line == '<') {
		 if (!(taglength = get_srt_tag_length(line)))
			break;
		 line += taglength;
	 }
	 if ((line[0] == '\n')||((line[0] == '\r')&&(line[1] == '\n')))
			break;
	 phrase[i] = *line;
 }
 phrase[i] = '\n';
 switch (lang) {
	 case ENG: parseengphrase(phrase);
 }
 return 0;
}

/*
 * It processes tags and calculate it's length. Possible tags are:
 * <font color="#00FF00" size="6">The <font size="35">most</font> difficult tag</font>
 * <b><i><u>Some of formatted text</u></i></b>
 * It also misses tags with mistakes
 */
static int get_srt_tag_length(char *line)
{
 char c;
 int taglength;

 if (line[1] == '/') {
	c = line[2];
	switch (c) {
		case 'b': case 'i': case 'u': 
			taglength = 4;
			break;
		case 'f':
			taglength = 7;
			break;
		default: 
			taglength = 0;
			break;
	}
 }
 else {
	c = line[1];
	switch (c) {
		case 'b': case 'i': case 'u': 
			taglength = 3;
			break;
		case 'f':
			taglength = 5;
			c = line[5];
			while (c == ' ') {
				taglength++;
				c = line[taglength];
				switch (c) {
					case 'c':
						taglength += 15;
						break;
					case 's':
						taglength += 6;
						while isdigit(line[taglength])
							taglength++;
						taglength++;
						break;
					case '>':
						break;
					default:
						taglength = 0;
				}
				c = line[taglength];
			}
			if (c == '>')
				taglength++;
			else
				taglength = 0;
			break;
		default: 
			taglength = 0;
			break;
	}
 }
 return taglength;
}

