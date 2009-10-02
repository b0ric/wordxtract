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

if WIN32
AM_CFLAGS = 					\
	-mwindows				\
	-DWIN32					\
	-DWORDXTRACT_DATA_DIR=\"""\"		\
	-DGNOME_ICONDIR=\"""\"			\
	-DLOCALE_DIR=\"""\"
else
AM_CFLAGS = 					\
	-DWORDXTRACT_DATA_DIR=\""$(datadir)/wordxtract"\"	\
	-DGNOME_ICONDIR=\""$(datadir)/pixmaps"\"		\
	-DLOCALE_DIR=\""$(wordxtractlocaledir)"\"
endif
AM_CFLAGS += @GTK_CFLAGS@

wordxtract_LDADD = @GTK_LIBS@

# Here is the price we pay for cross compilation support
#	pkg-config may return some non-standart dirs with -L options
#	which will be threated as additional targets
if !WIN32
wordxtract_DEPENDENCIES = @GTK_LIBS@
endif
