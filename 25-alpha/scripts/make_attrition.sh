#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Error: fit not specifid on command line"
   echo "Usage: make_attrition.sh root fit"
   echo "       for example, make_attrition.sh skj16 0b"
   exit 1
fi

BASE=$1
FIT=$2
echo $BASE\.p$FIT
#GRAPHICS="/home/jsibert/movemod/SPC2011/Reports/graphics/"
GRAPHICS="./"
echo $GRAPHICS
#ls $GRAPHICS

export MOVEMODHOME=$HOME/movemod/25-alpha
export SCRIPTDIR=$MOVEMODHOME/scripts
echo $SCRIPTDIR

#PPAR=13
#GPAR=68

## set up to run simulator for "final" plot
#mkdir -v bitmaps
#$SCRIPTDIR/set_ipar.sh  $BASE\.p$FIT $PPAR  1 > t.1
#$SCRIPTDIR/set_ipar.sh t.1 $GPAR 1 >  $BASE.par
#$MOVEMODHOME/obstac
#$MOVEMODHOME/rtagmove
#cp -v bitmaps/final* .
#echo "final-"$FIT*
#eog final-"$FIT".png &


## set up to run simulator and obstac for attrition plot
#$SCRIPTDIR/set_ipar.sh  $BASE\.p$FIT $PPAR  0 > t.1
#echo "first set_ipar"
#$SCRIPTDIR/set_ipar.sh t.1 $GPAR 0 >  $BASE.par
#$MOVEMODHOME/obstac
#$MOVEMODHOME/rtagmove
#rm -fv t.1
#
#OTA="./fit"$FIT"_P0_ota.csv"
#echo $OTA
#PTA="./fit"$FIT"_P0_pta.csv"
#echo $PTA

## sart R source a file and run some functions
echo "starting R"
R --slave <<EOF
print(paste("interactive:",interactive()))
#source("/home/eunjung/movemod/25/scripts/taplot.R")
#source("/home/jsibert/movemod/25/scripts/taplot.R")
source("$SCRIPTDIR/taplot.R")
print("starting ta.plot")
ta.plot("$BASE","$FIT",transform="r")
print("starting hist.plot")
hist.plot("$BASE","$FIT")
EOF

echo "done with R"
echo $GRAPHICS
echo $GRAPHICS$BASE'-'$FIT"-attrition-r.pdf"
#mv -v attrition_r.pdf $GRAPHICS$BASE'-'$FIT"-attrition-r.pdf"
acroread $GRAPHICS$BASE'-p'$FIT"-attrition-r.pdf" &
#mv -v history.pdf $GRAPHICS$BASE'-'$FIT"-history.pdf"
acroread $GRAPHICS$BASE'-p'$FIT"-history.pdf" &
#rm -v Rplots.pdf
#rm -rfv bitmaps
