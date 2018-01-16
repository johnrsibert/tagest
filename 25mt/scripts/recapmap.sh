#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Usage: recapmap.sh spp prnfile"
   echo "       for example, recapmap skj skj26"
   exit 0
fi

REGION=-R110/215/-25/20
#REGION=-R110/280/-25/20
echo $REGION
PROJECTION=-JM6i
echo $PROJECTION
MAP=$2-$1-release-recapture
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

gmtset ANOT_FONT_SIZE_PRIMARY 12 BASEMAP_TYPE fancy PAPER_MEDIA letter+ PAGE_ORIENTATION portrait HEADER_FONT_SIZE 16 HEADER_FONT 1 MEASURE_UNIT inch PLOT_DEGREE_FORMAT dddF

makecpt -Cpolar -T-5000/2000/100 -Z -I > tags.cpt

DAT=~/movemod/SPC2011/pttp/OneDegree/caps.xyz
LAB=$2" "$1" "Recaptures
echo $LAB
echo $DAT
psbasemap -B10g5:."$LAB": $REGION $PROJECTION -V -K > $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W -V -O -K  >> $EPS
psxy $DAT $REGION $PROJECTION -Ctags.cpt -Sc0.0625i -V -O -K >> $EPS

#draw region boundaries
psxy $2_regions.dat  $REGION $PROJECTION -Ap -M -W1p/$REG_COLOR -V -O -K >> $EPS

#process the prn file to plot the landmask
prn2gmt.sh $2.prn | psxy $REGION $PROJECTION -Sc0.03125i -G$SHORELINE -V -O -K >> $EPS

# draw region numbers 
pstext $2_region_numbers.dat $REGION $PROJECTION  -G$REG_COLOR -V -O -K >> $EPS

#draw domain boundary
psxy $2_domain.dat  $REGION $PROJECTION -Ap -M -W3p/$REG_COLOR -V -O -K >> $EPS


DAT=~/movemod/SPC2011/pttp/OneDegree/rels.xyz
echo $DAT
LAB=$2" "$1" "Releases
echo $LAB
psbasemap -B10g5:."$LAB": $REGION $PROJECTION -V -K -O -Y3.75i >> $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W -V -O -K   >> $EPS
psxy $DAT $REGION $PROJECTION -Ctags.cpt -Sc0.0625i -V -O -K >> $EPS

#draw region boundaries
psxy $2_regions.dat  $REGION $PROJECTION -Ap -M -W1p/$REG_COLOR -V -O -K >> $EPS

#process the prn file to plot the landmask
prn2gmt.sh $2.prn | psxy $REGION $PROJECTION -Sc0.03125i -G$SHORELINE -V -O -K >> $EPS

# draw region numbers 
pstext $2_region_numbers.dat $REGION $PROJECTION  -G$REG_COLOR -V -O -K >> $EPS

#draw domain boundary
psxy $2_domain.dat  $REGION $PROJECTION -Ap -M -W3p/$REG_COLOR -V -O    >> $EPS

gv $EPS&
ps2raster -V -A -Tg -E300 $EPS
eog $MAP.png &


