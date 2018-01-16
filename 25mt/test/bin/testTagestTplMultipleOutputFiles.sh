#!/bin/bash

printf "Running tpl/tagest in \""`pwd`"\"\n "
declare -r FILENAME=`head -n 1 file_nam.tmp`
#use -m works with cygwin
declare -r BENCHMARK="`uname -m`"".tpl.benchmark"
#printf "%s" $FILENAME

if [ ! -f ../../../../../tpl/tagest ]; then
printf "Result tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing ../../../../../tpl/tagest\n"
exit 0
fi

rm -f $FILENAME.p01
rm -f $FILENAME-p01.bar
rm -f $FILENAME-p01.eva
rm -f $FILENAME-p01.cor
rm -f $FILENAME-p01.log
rm -f $FILENAME-p01.par
rm -f $FILENAME-p01.rep
rm -f $FILENAME-p01.std
rm -f $FILENAME-p01-tagest.log

PATH=../../../../../tpl:$PATH tagest &> /dev/null

if [ ! -f $FILENAME.p01 ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.p01
exit 0
fi
if [ ! -f $FILENAME-p01.bar ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.bar
exit 0
fi
if [ ! -f $FILENAME-p01.eva ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.eva
exit 0
fi
if [ ! -f $FILENAME-p01.cor ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.cor
exit 0
fi
if [ ! -f $FILENAME-p01.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.log
exit 0
fi
if [ ! -f $FILENAME-p01.par ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.par
exit 0
fi
if [ ! -f $FILENAME-p01.rep ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.rep
exit 0
fi
if [ ! -f $FILENAME-p01.std ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01.std
exit 0
fi
if [ ! -f $FILENAME-p01-tagest.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p01-tagest.log
exit 0
fi

rm -f $FILENAME.p02
rm -f $FILENAME-p02.bar
rm -f $FILENAME-p02.cor
rm -f $FILENAME-p02.eva
rm -f $FILENAME-p02.log
rm -f $FILENAME-p02.par
rm -f $FILENAME-p02.rep
rm -f $FILENAME-p02.std
rm -f $FILENAME-p02-tagest.log

PATH=../../../../../tpl:$PATH tagest &> /dev/null

if [ ! -f $FILENAME.p02 ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.p02
exit 0
fi
if [ ! -f $FILENAME-p02.bar ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.bar
exit 0
fi
if [ ! -f $FILENAME-p02.eva ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.eva
exit 0
fi
if [ ! -f $FILENAME-p02.cor ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.cor
exit 0
fi
if [ ! -f $FILENAME-p02.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.log
exit 0
fi
if [ ! -f $FILENAME-p02.par ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.par
exit 0
fi
if [ ! -f $FILENAME-p02.rep ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.rep
exit 0
fi
if [ ! -f $FILENAME-p02.std ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02.std
exit 0
fi
if [ ! -f $FILENAME-p02-tagest.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p02-tagest.log
exit 0
fi

rm -f $FILENAME.p03
rm -f $FILENAME-p03.bar
rm -f $FILENAME-p03.cor
rm -f $FILENAME-p03.eva
rm -f $FILENAME-p03.log
rm -f $FILENAME-p03.par
rm -f $FILENAME-p03.rep
rm -f $FILENAME-p03.std
rm -f $FILENAME-p03-tagest.log

PATH=../../../../../tpl:$PATH tagest &> /dev/null

if [ ! -f $FILENAME.p03 ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME.p03
exit 0
fi
if [ ! -f $FILENAME-p03.bar ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.bar
exit 0
fi
if [ ! -f $FILENAME-p03.cor ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.cor
exit 0
fi
if [ ! -f $FILENAME-p03.eva ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.eva
exit 0
fi
if [ ! -f $FILENAME-p03.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.log
exit 0
fi
if [ ! -f $FILENAME-p03.par ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.par
exit 0
fi
if [ ! -f $FILENAME-p03.rep ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.rep
exit 0
fi
if [ ! -f $FILENAME-p03.std ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03.std
exit 0
fi
if [ ! -f $FILENAME-p03-tagest.log ]; then
printf "Checking tpl/tagest in \""`pwd`"\"... "
printf "FAILED\n"
printf "Error: missing %s\n" $FILENAME-p03-tagest.log
exit 0
fi

printf "Result tpl/tagest in \""`pwd`"\"... "
printf "OK\n"

rm -f $FILENAME.p01
rm -f $FILENAME-p01.bar
rm -f $FILENAME-p01.eva
rm -f $FILENAME-p01.cor
rm -f $FILENAME-p01.log
rm -f $FILENAME-p01.par
rm -f $FILENAME-p01.rep
rm -f $FILENAME-p01.std
rm -f $FILENAME-p01-tagest.log
rm -f $FILENAME.p02
rm -f $FILENAME-p02.bar
rm -f $FILENAME-p02.eva
rm -f $FILENAME-p02.cor
rm -f $FILENAME-p02.log
rm -f $FILENAME-p02.par
rm -f $FILENAME-p02.rep
rm -f $FILENAME-p02.std
rm -f $FILENAME-p02-tagest.log
rm -f $FILENAME.p03
rm -f $FILENAME-p03.bar
rm -f $FILENAME-p03.eva
rm -f $FILENAME-p03.cor
rm -f $FILENAME-p03.log
rm -f $FILENAME-p03.par
rm -f $FILENAME-p03.rep
rm -f $FILENAME-p03.std
rm -f $FILENAME-p03-tagest.log
rm -f fmin.log
rm -f gmon.out
rm -f admodel.cov
rm -f admodel.hes
rm -f admodel.dep
rm -f eigv.rpt
rm -f variance
rm -f $FILENAME.ppp
exit 0
