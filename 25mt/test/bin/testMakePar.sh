#!/bin/bash

printf "Checking makepar in \""`pwd`"\"... "
declare -r FILENAME=`head -n 1 file_nam.dat`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.p00
rm -f $FILENAME.tag
svn revert $FILENAME.pos &> /dev/null
if [ ! -f ../../../../makepar ]; then
printf "FAILED\n"
printf "Error: missing ../../../../makepar\n"
exit 0
fi
../../../../makepar &> makepar.log

if [ ! -f $FILENAME.p00 ]; then
printf "FAILED\n"
printf "Error: missing %s.p00\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.p00.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s.p00.%s\n" $FILENAME $BENCHMARK
exit 0
fi

if [ ! `diff $FILENAME.p00 $FILENAME.p00.$BENCHMARK | wc -l` -eq 0 ]; then
printf "FAILED\n"
printf "Error: %s.p00 and %s.p00.%s differ\n" $FILENAME $FILENAME $BENCHMARK
exit 0
fi

if [ ! -f $FILENAME.tag ]; then
printf "FAILED\n"
printf "Error: missing %s.tag\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.tag.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s.tag.%s\n" $FILENAME $BENCHMARK
exit 0
fi

grep "^ " $FILENAME.tag > $FILENAME.tag.2
grep "^ " $FILENAME.tag.$BENCHMARK > $FILENAME.tag.$BENCHMARK.2
if [ ! `diff $FILENAME.tag.2 $FILENAME.tag.$BENCHMARK.2 | wc -l` -eq 0 ]; then
printf "FAILED\n"
printf "Error: %s.tag and %s.tag.%s differ\n" $FILENAME $FILENAME $BENCHMARK
exit 0
fi

#clean for OK only
svn revert $FILENAME.pos &> /dev/null
rm -f $FILENAME.tag.2
rm -f $FILENAME.tag.$BENCHMARK.2
rm -f makepar.log
rm -f gmon.out

printf "OK\n"
exit 0
