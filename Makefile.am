bin_PROGRAMS = crlf 
crlf_SOURCES = crlf.c
man1_MANS = @MAN_PAGES@
EXTRA_DIST = README.md crlf.1 crlf.interix $(man1_MANS) 

# additional cpp defines go here
AM_CPPFLAGS = -D_ALL_SOURCE=1


# use README.md rather than README
# tell 'automake' I'm a 'foreign' thing so it doesn't force README
AUTOMAKE_OPTIONS = foreign


