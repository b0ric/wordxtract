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

#ifndef SRT_H
#define SRT_H

/* maximum subtitle line and phrase length
 * it's not allocated dinamically 'cause it costs a lot of resources and
 * it can't be more than mean human reading speed in a half of a minute
 * it's about 100 letters + formatting
 */
#define MAXLINE 300
#define MAXPHRASE 100

int process_srt(FILE *);
int process_srt_line(char *);

#endif /*SRT_H*/
