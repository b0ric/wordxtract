#Makefile.am for WordExtract project
#Process this file with automake to produce Makefile.in

bin_PROGRAMS = wordxtract

DIST_SUBDIRS = pixmaps win32
SUBDIRS = pixmaps win32

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
AM_CFLAGS = -mwindows -DWIN32			\
	-DWORDXTRACT_DATA_DIR=\""../share/wordxtract"\"
else
AM_CFLAGS =                                     \
	-DWORDXTRACT_DATA_DIR=\""$(datadir)/wordxtract"\"       \
	-DGNOME_ICONDIR=\""$(datadir)/pixmaps"\"                \
	-DLOCALE_DIR=\""$(wordxtractlocaledir)"\"
endif
AM_CFLAGS += @GTK_CFLAGS@

wordxtract_LDADD = @GTK_LIBS@
if WIN32
wordxtract_LDADD += win32/wordxtract.res
endif

# Here is the price we pay for cross compilation support
#       pkg-config may return some non-standart dirs with -L options
#       which will be threated as additional targets and error will occur
if WIN32
wordxtract_DEPENDENCIES = win32/wordxtract.res
else
wordxtract_DEPENDENCIES = @GTK_LIBS@
endif
