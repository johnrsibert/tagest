#!/bin/bash

printf "Checking makeifr in \""`pwd`"\"... "
declare -r FILENAME=`head -n 1 file_nam.dat`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.ifr
svn revert $FILENAME.pos &> /dev/null
if [ ! -f ../../../../makeifr ]; then
printf "FAILED\n"
printf "Error: missing ../../../../makeifr\n"
exit 0
fi
../../../../makeifr &> makeifr.log

if [ ! -f $FILENAME.ifr ]; then
printf "FAILED\n"
printf "Error: missing %s.ifr\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.ifr.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s.ifr.%s\n" $FILENAME $BENCHMARK
exit 0
fi

if [ ! `diff $FILENAME.ifr $FILENAME.ifr.$BENCHMARK | wc -l` -eq 0 ]; then
printf "FAILED\n"
printf "Error: %s.ifr and %s.ifr.%s differ\n" $FILENAME $FILENAME $BENCHMARK
exit 0
fi

#Do clean up for OK
svn revert $FILENAME.pos &> /dev/null
rm -f gmon.out
rm -f makeifr.log

printf "OK\n"
exit 0
