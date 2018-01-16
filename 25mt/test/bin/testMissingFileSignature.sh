#!/bin/bash

printf "Checking par file missing header signature in \""`pwd`"\" using $1 ... "
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".benchmark"

if [ ! -f $1 ]; then
printf "FAILED\n"
printf "Error: missing $1\n"
exit 0
fi

$1 &> missingFileSignature.log

declare -r LASTLINE=`tail -n 1 missingFileSignature.log`
if [ ! "$LASTLINE" = "Error: incorrect file signature \"\" in $FILENAME.par" ]; then
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
rm -f missingFileSignature.log
rm -f $FILENAME.tmp
rm -f $FILENAME.ppp

printf "OK\n"
exit 0
