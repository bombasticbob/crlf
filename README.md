# crlf
cross-platform line ending conversion utility

This is a simple 'CRLF' translation utility for FreeBSD
(c) 2003 by 'Big Bad Bob' Frazier - all rights reserved

This software is covered by the GNU GPL or a BSD-like license
See www.gnu.org/copyleft/gpl.html for details on the GPL.

For license and other terms, see COPYING.

This software comes with no warantees, either implied or explicit.



INSTALLATION
------------

This application has been tested and built on a FreeBSD system (5.0-RELEASE)
and as such may not function exactly the same on other systems.

The file 'crlf' is a FreeBSD 'ELF'  executable binary and should be copied
to a directory such as '/usr/local/bin'.  The source files should be copied
into a directory such as '/usr/local/shared/apps/crlf'.  The make file
'Makefile' specifies the output file as '/usr/local/bin/crlf' so if you
build it without changing the make file, the output file will be written
to '/usr/local/bin/crlf'.



As with any software package, you should back up any relevant information
or files prior to installing or running this application.



RE-DISTRIBUTION
---------------

Re-distribution of this application shall consist of one of the following:

1.  The original 'crlf.tar.gz' as distributed by the author

2.  An equivalent archive file containing the following
  a) crlf      (executable binary)
  b) crlf.c    (source)
  c) Makefile  ('make' info)
  d) README.md (this file)


Modifications to the source files should be noted where applicable.


FILE NAME RECURSION
-------------------

The 'crlf' application uses a method of file name recursion (via '-r')
that more closely resembles a windows application than a *nix one.
However, it is far more useful to use recursion in this manner than
via the 'globbed' method.  The traditional *nix shell uses 'globbing'
to translate a file specification "*.txt" into a space-delimited set
of file names on the command line.  If you use the '-r' option with
this list, only those directories that match the pattern would be
recursed, and to each of these a file specification of "*.*" would be
used.  Obviously this has very limited usefulness without the 'find'
and 'xargs' utilities to properly refine recursive file searches.

Therefore, 'crlf' allows you to pass the file name specification in
quotes, to avoid globbing, as follows

    crlf -r "*.txt"

This would recurse all directories, looking for files that match the
pattern "*.txt", and operate on them.  If the quotes were not present,
files matching "*.txt" would be 'globbed' by the shell, and any sub-
directories found would be searched with a file name specification of
"*.*".  Since it is unlikely you would have a directory name that
matches the pattern "*.txt" the 'globbed' option is meaningless, and
of very limited use.  The behavior of crlf is superior to the 'globbing
effect' caused by the shells.


