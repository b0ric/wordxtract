if WIN32
wordxtract.res: wordxtract.rc
	$(WINDRES) -i wordxtract.rc --input-format=rc -o wordxtract.res -O coff

BUILT_SOURCES = wordxtract.res
endif

EXTRA_DIST = wordxtract.ico wordxtract.rc

DISTCLEANFILES = wordxtract.res
