#!/bin/bash

declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".tpl.benchmark"
#printf "%s" $FILENAME
rm -f $FILENAME.p01

if [ ! -f ../../../../../tpl/tagest ]; then
printf "Error: missing tpl/tagest in \""`pwd`"\"\n"
exit 0
fi

printf "Running tpl/tagest in \""`pwd`"\"\n "; PATH=../../../../../tpl:$PATH tagest -est &> tagestTpl.log

if [ ! -f $FILENAME.p01 ]; then
printf "Result tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01\n" $FILENAME
exit 0
fi

if [ ! -f $FILENAME.p01.$BENCHMARK ]; then
printf "Result tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s.p01.%s\n" $FILENAME $BENCHMARK
exit 0
fi

if [ ! `diff $FILENAME.p01 $FILENAME.p01.$BENCHMARK | wc -l` -eq 0 ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
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
#rm -f tagestTpl.log
#rm -f $FILENAME.tmp
#rm -f $FILENAME.ppp

printf "Result tpl/tagest in \""`pwd`"\"... "
printf "OK\n"
exit 0
