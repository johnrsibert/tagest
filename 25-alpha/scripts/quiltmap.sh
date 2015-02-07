#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Usage: quitmap rooot fit"
   echo "       for example, allskj p001"
   exit 0
fi

WEST=115
EAST=210
SOUTH=-25
NORTH=15
#REGION=-R115/210/-25/15
REGION=-R$WEST/$EAST/$SOUTH/$NORTH
echo $REGION

WIDTH=6
echo $WIDTH
#use bc for floating point computations
#interest_r=$(echo "scale=9; $interest_r/100.0" | bc) # Convert to decimal.
# ^^^^^^^^^^^^^^^^^ Divide by 100.
# "scale" determines how many decimal places.
SCALE=$(echo "scale=3; $WIDTH/($EAST-$WEST)" | bc)
echo "SCALE = "$SCALE" inches per degree along equator"
PROJECTION=-Jm$SCALE
echo $PROJECTION
MAP=$1"-"$2"-residuals"
echo $MAP
EPS=$MAP.eps
echo $EPS
rm $EPS

EEZ_COLOR=1P/0/0/255
WET_COLOR=194/241/253
DRY_COLOR=237/214/151
SHORELINE=154/139/98
REG_COLOR=0/128/0
DOM_COLOR=$REG_COLOR

rm -fv .gmt*
gmtset ANOT_FONT_SIZE_PRIMARY 10 ANOT_FONT_SIZE_SECONDARY 4 BASEMAP_TYPE plain PAPER_MEDIA letter+ PAGE_ORIENTATION portrait HEADER_FONT_SIZE 12 HEADER_FONT 1 MEASURE_UNIT inch PLOT_DEGREE_FORMAT dddF FRAME_PEN 1P TICK_PEN 1P TIC_LENGTH 0.05c
#COLOR_FOREGROUND red COLOR_BACKGROUND lightgray
#makecpt -Cpanoply -Tresid_levels.dat -V -Q -M -Z > resid.cpt

echo "1		4	14	216	2		32	80	255 ;1
2		32	80	255	4		109	193	255 ;2
4		109	193	255	8		156	238	255 ;4
8		156	238	255	16		206	255	255 ;8
16		206	255	255	32		255	254	71 ;16
32		255	254	71	64		255	196	0  ;32
64		255	196	0	128		255	72	0  ;64
128		255	72	0	256		213	0	0 ;128
256		213	0	0	512		158	0	0 ;256
B	211	211	211
F	255	0	0
N	128	128	128" > t.cpt


DAT=$1"-"$2"-resid.xyz"
echo $DAT
LAB="Cummulative returns for fit "$1"."$2
echo $LAB
awk -F"," '{print $1,$2,$3}' $DAT | psxy  $REGION $PROJECTION -SkUL/$SCALE"i" -Ct.cpt -V    -K >  $EPS
awk -F"," '{print $1,$2,$4}' $DAT | psxy  $REGION $PROJECTION -SkLR/$SCALE"i" -Ct.cpt -V -O -K >> $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W              -V -O -K >> $EPS
psbasemap -B20g5/10g5:."$LAB":WNSE $REGION $PROJECTION       -V -O -K >> $EPS

psscale -D3i/-0.4i/6i/0.1ih -Ct.cpt -S -D -L -Ef -V -O    >> $EPS

#gmt psscale -D2.75i/-0.4i/4i/0.15ih -Cgrav.cpt -Bx20f10 -By+l"mGal" -O -K >> $ps


#gv $EPS&
rm -v t.cpt
rm -v $MAP.png
ps2raster -V -A -Tg -E300 $EPS
rm -v $EPS
eog $MAP.png &


