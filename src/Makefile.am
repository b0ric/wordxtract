#Makefile.am for WordExtract project
#Process this file with automake to produce Makefile.in

DIST_SUBDIRS = pixmaps
SUBDIRS = pixmaps

bin_PROGRAMS = wordxtract

wordxtract_SOURCES = 				\
	main.c main.h				\
	word.c word.h				\
	dict.c dict.h				\
	mainwin.c mainwin.h			\
	dictwin.c dictwin.h			\
	confwin.c confwin.h			\
	engparser.c engparser.h			\
	srt.c srt.h				\
	plaintext.c plaintext.h

AM_CFLAGS = 					\
	@GTK_CFLAGS@				\
	-DWORDXTRACT_DATA_DIR=\""$(datadir)/wordxtract"\"	\
	-DGNOME_ICONDIR=\""$(datadir)/pixmaps"\"		\
	-DLOCALE_DIR=\""$(wordxtractlocaledir)"\"

wordxtract_LDADD = @GTK_LIBS@
wordxtract_DEPENDENCIES = @GTK_LIBS@