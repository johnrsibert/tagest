#!/bin/bash

printf "Checking average fishing mortality in \""`pwd`"\" ... "
declare -r FILENAME=`head -n 1 file_nam.tmp`

rm -f gmon.out
rm -f $FILENAME-p*.log
rm -f $FILENAME.ppp
rm -f $FILENAME-p*.par
rm -f variance
rm -f $FILENAME.p0*
rm -f fmin.log
rm -f $FILENAME-p*.bar
rm -f $FILENAME-p*.rep
rm -f $FILENAME-p*-tagest.log
rm -f eigv.rpt

../../../../../tpl/tagest -est &> /dev/null

declare -r LINE=`grep "average_fishing_mortality =" $FILENAME-p01.rep`
#declare -r EXPECTED="average_fishing_mortality =  0.000243605 0.000270474 0.0147825 0.0156899 0.00192086 0.00187844 0.010441 0.0105243 0.00157244 0.00249184"
declare -r EXPECTED="average_fishing_mortality =  0.000451781 6.69695e-05 1.26857e-05 0.000221534 0.00225643 0.000204055 0.000673397 0.00030166 0.000336638 0.00294695 0.0100434 0.00427859"
if [ ! "$LINE" = "$EXPECTED" ]; then
	printf "FAILED\n"
	printf "Error: expected \"$EXPECTED\" \n            got \"$LINE\"\n"
	exit 0
fi

rm -f gmon.out
rm -f $FILENAME-p01.log
rm -f $FILENAME.ppp
rm -f $FILENAME-p01.par
rm -f variance
rm -f $FILENAME.p01
rm -f $FILENAME.tmp
rm -f fmin.log
rm -f $FILENAME-p01.bar
rm -f $FILENAME-p01.rep
rm -f $FILENAME-p01-tagest.log
rm -f eigv.rpt

printf "OK\n"
exit 0
