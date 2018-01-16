#!/bin/bash

printf "Running tagest in \""`pwd`"\"... \n"
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.p01

if [ ! -f ../../../../../rtagest25b ]; then
printf "Checking tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing ../../../../../rtagest25b\n"
exit 0
fi

../../../../../rtagest25b &> tagestModified.log

if [ ! -f $FILENAME.p01 ]; then
printf "Checking tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.p01.$BENCHMARK ]; then
printf "Checking tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01.%s\n" $FILENAME $BENCHMARK
exit 0
fi

if [ ! `diff $FILENAME.p01 $FILENAME.p01.$BENCHMARK | wc -l` -eq 0 ]; then
printf "Checking tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: %s.p01 and %s.p01.%s differ\n" $FILENAME $FILENAME $BENCHMARK
exit 0
fi

#rm -f fmin.log
#rm -f gmon.out
#rm -f tagest.bar
#rm -f tagest.rep
#rm -f tagest.log
#rm -f variance
#rm -f eigv.rpt
#rm -f tagestModified.log
#rm -f $FILENAME.tmp
#rm -f $FILENAME.ppp

printf "Checking tagest in \""`pwd`"\"... "
printf "OK\n"
exit 0
