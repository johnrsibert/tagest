#!/bin/bash

printf "Checking distance in \""`pwd`"\"... "
declare -r FILENAME=`head -n 1 file_nam.dat`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.dst

if [ ! -f ../../../../distance ]; then
printf "FAILED\n"
printf "Error: missing ../../../../distance\n"
exit 0
fi
../../../../distance &> $0.log

if [ ! -f $FILENAME.dst ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.dst
exit 0
fi

if [ ! -f $FILENAME.dst.$BENCHMARK ]; then
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.dst.$BENCHMARK
exit 0
fi

if [ ! -z "`diff $FILENAME.dst $FILENAME.dst.$BENCHMARK`" ]; then
printf "FAILED\n"
printf "Error: %s and %s differ\n" $FILENAME.dst $FILENAME.dst.$BENCHMARK
exit 0
fi

rm -f $0.log
rm -f gmon.out
rm -f distance.log

printf "OK\n"
exit 0
