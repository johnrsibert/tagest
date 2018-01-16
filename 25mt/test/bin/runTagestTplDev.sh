#!/bin/bash

printf "Running tpl/tagest-dev in \""`pwd`"\"\n"
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".tpl.benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.p01

if [ ! -f ../../../../../tpl/tagest-dev ]; then
printf "Result tpl/tagest-dev in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing ../../../../../tpl/tagest-dev\n"
exit 0
fi

PATH=../../../../../tpl:$PATH tagest-dev -est &> tagestTplDev.log

if [ ! -f $FILENAME.p01 ]; then
printf "Result tpl/tagest-dev in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.p01.$BENCHMARK ]; then
printf "Result tpl/tagest-dev in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01.%s\n" $FILENAME $BENCHMARK
exit 0
fi

if [ ! `diff $FILENAME.p01 $FILENAME.p01.$BENCHMARK | wc -l` -eq 0 ]; then
printf "Checking tpl/tagest-dev in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: %s.p01 and %s.p01.%s differ\n" $FILENAME $FILENAME $BENCHMARK
exit 0
fi

#rm -f fmin.log
#rm -f gmon.out
#rm -f tagest-dev.bar
#rm -f tagest-dev.rep
#rm -f tagest-dev.log
#rm -f variance
#rm -f eigv.rpt
#rm -f tagestTplDev.log
#rm -f $FILENAME.tmp
#rm -f $FILENAME.ppp

printf "Result tpl/tagest-dev in \""`pwd`"\"... "
printf "OK\n"
exit 0
