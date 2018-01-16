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

#PLINE=84
#GLINE=184
#PLINE=76
#GLINE=176
PPAR=13
GPAR=68

# set up to run simulator for "final" plot
mkdir -v bitmaps
# set_ipar.sh  $BASE\.p$FIT $PLINE 3 1 > t.1
# set_ipar.sh t.1 $GLINE 8 1 >  $BASE.par
set_ipar.sh  $BASE\.p$FIT $PPAR  1 > t.1
set_ipar.sh t.1 $GPAR 1 >  $BASE.par
obstac
rtagmove
cp -v bitmaps/final* .
echo "final-"$FIT*
eog final-"$FIT".png &

##final-crop.sh "final-"$FIT* $GRAPHICS$BASE'-'$FIT"-final.png" 
##convert -verbose "final-"$FIT* -crop 565x270+39+14 $GRAPHICS$BASE'-'$FIT"-final.png" 
#convert -verbose final-07.png -crop 572x202+36+14 t.png
#convert -verbose "final-"$FIT".png" -crop 571x202+36+15 $GRAPHICS$BASE'-'$FIT"-final.png" 
#eog $GRAPHICS$BASE'-'$FIT"-final.png" &

##
## set up to run simulator and obstac for attrition plot
set_ipar.sh  $BASE\.p$FIT $PPAR  0 > t.1
set_ipar.sh t.1 $GPAR 0 >  $BASE.par
rtagmove
obstac
#
OTA="./fit"$FIT"_P0_ota.csv"
echo $OTA
PTA="./fit"$FIT"_P0_pta.csv"
echo $PTA
#
R --slave <<EOF
print(paste("interactive:",interactive()))
source("/home/jsibert/movemod/25/scripts/taplot.R")
ta.plot("$OTA","$PTA",transform="r")
#ta.plot("$OTA","$PTA",transform="y")
#ta.plot("$OTA","$PTA",transform="n")
EOF
echo $GRAPHICS
echo $GRAPHICS$BASE'-'$FIT"-attrition-r.pdf"
mv -v attrition_r.pdf $GRAPHICS$BASE'-'$FIT"-attrition-r.pdf"
acroread $GRAPHICS$BASE'-'$FIT"-attrition-r.pdf" &
rm -v Rplots.pdf
rm -rfv bitmaps
