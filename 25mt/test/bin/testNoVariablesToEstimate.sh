#!/bin/bash

printf "Checking no variables to estimate in \""`pwd`"\" using $1 ... "
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.p01

if [ ! -f $1 ]; then
printf "FAILED\n"
printf "Error: missing $1\n"
exit 0
fi
$1 &> $0.log

if [ -f $FILENAME.p01 ]; then
printf "FAILED\n"
printf "Error: unexpected %s.p01 found\n" $FILENAME
exit 0
fi

declare -r LASTLINE=`tail -n 1 $0.log`
if [ ! "$LASTLINE" = "Error: nvar=0, no variables to estimate" ]; then
printf "FAILED\n"
printf "Error: unexpected last line \"%s\"\n" $LASTLINE
exit 0
fi

rm -f fmin.log
rm -f gmon.out
rm -f tagest.bar
rm -f tagest.rep
rm -f tagest.log
rm -f variance
rm -f eigv.rpt
rm -f $0.log
rm -f $FILENAME.tmp
rm -f $FILENAME.ppp

printf "OK\n"
exit 0
