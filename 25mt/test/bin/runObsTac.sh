#!/bin/bash

printf "Checking obstac in \""`pwd`"\"... "
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f fit02_P1_ota.csv

if [ ! -f ../../../../../obstac ]; then
printf "FAILED\n"
printf "Error: missing ../../../../../obstac \n"
exit 0
fi

../../../../../obstac &> $0.log

if [ ! -z "`diff fit02_P1_ota.csv fit02_P1_ota.csv.i686.benchmark`" ]; then
printf "FAILED\n"
printf "Error: %s and %s differ\n" fit02_P1_ota.csv fit02_P1_ota.csv.i686.benchmark
exit 0
fi

rm -f obstac.log
rm -f gmon.out
rm -f fit02_P1.ota

printf "OK\n"
exit 0
