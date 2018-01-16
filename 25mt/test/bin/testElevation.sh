#!/bin/bash

printf "Checking elevation in \""`pwd`"\"... "
declare -r FILENAME=`head -n 1 file_nam.dat`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.zdt
rm -f $FILENAME.zdf

if [ ! -f ../../../../elevation ]; then
printf "FAILED\n"
printf "Error: missing ../../../../elevation\n"
exit 0
fi
../../../../elevation &> testElevation.log

if [ ! -f $FILENAME.zdf ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.zdf
exit 0
fi

if [ ! -f $FILENAME.zdf.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.zdf.$BENCHMARK
exit 0
fi

if test -z "`diff $FILENAME.zdf $FILENAME.zdf.$BENCHMARK`" ; then
printf "FAILED\n"
printf "Error: %s and %s differ\n" $FILENAME.zdf $FILENAME.zdf.$BENCHMARK
exit 0
fi

if [ ! -f $FILENAME.zdt ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.zdt
exit 0
fi

if [ ! -f $FILENAME.zdt.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.zdt.$BENCHMARK
exit 0
fi

if test -z "`diff $FILENAME.zdt $FILENAME.zdt.$BENCHMARK`" ; then
printf "FAILED\n"
printf "Error: %s and %s differ\n" $FILENAME.zdt $FILENAME.zdt.$BENCHMARK
exit 0
fi

rm -f testElevation.log
rm -f gmon.out
rm -f elevation.log

printf "OK\n"
exit 0
