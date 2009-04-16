#!/bin/sh

# Script to do refactoring of duma.. (put all files in extra subfolders to
# make it somewhat more organized ;-)

# planned folder structure:
#
# contrib/              <-- contribution: serveral bash/gdb scripts
# contrib/debian        <-- debian files? outdated!
# contrib/ebuild        <-- gentoo files?
# contrib/rpm           <-- rpm spec file?
#
# doc/                  <-- documentation goes here
# doc/comparisons/      <-- copy of JP Martin's site
#
# include/              <-- headers go in here
# include/duma/         <-- duma headers go in here
# include/dumadetours/  <-- dumadetours headers go in here
#
# lib/                  <-- compiled (shared) libs
# obj/                  <-- compiled objects
#
# src/                  <-- sources go in here
# src/libduma/          <-- duma sources go in here
#### src/dumadetours/      <-- dumadetours sources go in here. --- not necessary
# src/test/             <-- test programs ???
# examples/             <-- example code
# examples/cpp          <-- C++ examples
# examples/C            <-- C examples (TODO ?)

# test/                 <-- tests go in here

# win32/                <-- all those win32* folders go in here
# prj/                  <-- alternative - but more general to win32/:
#                           win32-*, codeblocks, later: eclipse?
#

mkdir contrib
mv gdbinit.rc    contrib/
mv duma.sh       contrib/
mv builddocs.bat contrib/
mv mkclean.bat   contrib/
mv debian        contrib/

mkdir doc
mv README.txt doc/
mv INSTALL    doc/
mv CHANGELOG  doc/
mv TODO       doc/
mv duma.3     doc/
mv WIN32_DETOURS_HOWTO.txt doc/
mv comparisons doc/

mkdir include
mkdir include/duma
mv noduma.h      include/duma/
mv duma.h        include/duma/
mv dumapp.h      include/duma/
mv duma_sem.h    include/duma/
mv duma_config.h include/duma/
# mkdir include/dumadetours

mkdir lib
mkdir obj

mkdir src
mkdir src/libduma
mv duma.c     src/libduma/
mv duma_hlp.h src/libduma/
mv paging.h   src/libduma/
mv print.h    src/libduma/
mv print.c    src/libduma/
mv sem_inc.c  src/libduma/
mv dumapp.cpp src/libduma/


mkdir src/conf
mv createconf.c src/conf/

mkdir examples
mkdir examples/cpp
mv example*cpp examples/cpp/

if /bin/false ; then
  # move all win32-* folders to win32/
  mkdir win32
  mv win32-* win32/
else
  mkdir prj
  mv win32-* prj/
  mv codeblocks prj/
fi
