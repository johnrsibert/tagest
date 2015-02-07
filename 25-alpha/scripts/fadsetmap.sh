#!/bin/bash
REGION=-R110/240/-45/25
echo $REGION
PROJECTION=-JM6i
echo $PROJECTION
MAP=fad-set-history
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

#gmtset ANOT_FONT_SIZE_PRIMARY 12 BASEMAP_TYPE fancy PAPER_MEDIA letter+ PAGE_ORIENTATION portrait HEADER_FONT_SIZE 16 HEADER_FONT 1 MEASURE_UNIT inch PLOT_DEGREE_FORMAT dddF
gmtset ANOT_FONT_SIZE_PRIMARY 12 BASEMAP_TYPE plain PAPER_MEDIA letter+ PAGE_ORIENTATION portrait HEADER_FONT_SIZE 16 HEADER_FONT 1 MEASURE_UNIT inch PLOT_DEGREE_FORMAT dddF


Y1=1977
Y2=1982
LAB=$Y1" - "$Y2
echo $LAB
DOMAIN=~/movemod/SPC2011/ssap/Gen/skj0/gen2
echo $DOMAIN
#psbasemap -B20g5/10g5:."$LAB":WnSe $REGION $PROJECTION -V -K > $EPS
psbasemap -B20g5/10g5:."$LAB":WNSe $REGION $PROJECTION -V -K > $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W -V -O -K  >> $EPS
awk -v Y1=$Y1 -v Y2=$Y2  '{if (($1>=Y1) && ($1<=Y2)) {print $5,$6,$7}}'  fadsets.dat | psxy -Csettype.cpt $REGION $PROJECTION -Sp -V -O -K >> $EPS
#awk  -v Y1=$Y1 -v Y2=$Y2 '{if (($1>=Y1) && ($1<=Y2)) {print $1,$5,$6,$7}}' fadsets.dat 

#draw region boundaries
#psxy $DOMAIN_regions.dat  $REGION $PROJECTION -Ap -M -W1p/$REG_COLOR -V -O -K >> $EPS

#process the prn file to plot the landmask
#prn2gmt.sh $2.prn | psxy $REGION $PROJECTION -Sc0.03125i -G$SHORELINE -V -O -K >> $EPS

# draw region numbers 
#pstext $DOMAIN_region_numbers.dat $REGION $PROJECTION  -G$REG_COLOR -V -O -K >> $EPS

#draw region boundaries
psxy $DOMAIN"_domain.dat"  $REGION $PROJECTION -Ap -M -W3p/$REG_COLOR -V -O -K  >> $EPS

######################

Y1=1989
Y2=1995
LAB=$Y1" - "$Y2
echo $LAB
DOMAIN=~/movemod/SPC2011/rttp/Gen/skj0/gen1
echo $DOMAIN
psbasemap -B20g5/10g5:."$LAB":wNSe $REGION $PROJECTION -V -O -K -X6.25i  >> $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W -V -O -K  >> $EPS
awk -v Y1=$Y1 -v Y2=$Y2  '{if (($1>=Y1) && ($1<=Y2)) {print $5,$6,$7}}'  fadsets.dat | psxy -Csettype.cpt $REGION $PROJECTION -Sp -V -O -K >> $EPS

#draw region boundaries
#psxy $DOMAIN_regions.dat  $REGION $PROJECTION -Ap -M -W1p/$REG_COLOR -V -O -K >> $EPS

#process the prn file to plot the landmask
#prn2gmt.sh $2.prn | psxy $REGION $PROJECTION -Sc0.03125i -G$SHORELINE -V -O -K >> $EPS

# draw region numbers 
#pstext $DOMAIN_region_numbers.dat $REGION $PROJECTION  -G$REG_COLOR -V -O -K >> $EPS

#draw region boundaries
psxy $DOMAIN"_domain.dat"  $REGION $PROJECTION -Ap -M -W3p/$REG_COLOR -V -O -K >> $EPS

######################

Y1=2006
Y2=2011
LAB=$Y1" - "$Y2
echo $LAB
DOMAIN=~/movemod/SPC2011/pttp/OneDegree/Gen/skj0/gen2
echo $DOMAIN
psbasemap -B20g5/10g5:."$LAB":wNSE $REGION $PROJECTION -V -O -K -X6.25i  >> $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W -V -O -K  >> $EPS
awk -v Y1=$Y1 -v Y2=$Y2  '{if (($1>=Y1) && ($1<=Y2)) {print $5,$6,$7}}'  fadsets.dat | psxy -Csettype.cpt $REGION $PROJECTION -Sp -V -O -K >> $EPS

#draw region boundaries
#psxy $DOMAIN_regions.dat  $REGION $PROJECTION -Ap -M -W1p/$REG_COLOR -V -O -K >> $EPS

#process the prn file to plot the landmask
#prn2gmt.sh $2.prn | psxy $REGION $PROJECTION -Sc0.03125i -G$SHORELINE -V -O -K >> $EPS

# draw region numbers 
#pstext $DOMAIN_region_numbers.dat $REGION $PROJECTION  -G$REG_COLOR -V -O -K >> $EPS

#draw region boundaries
psxy $DOMAIN"_domain.dat"  $REGION $PROJECTION -Ap -M -W3p/$REG_COLOR -V -O    >> $EPS


gv $EPS&
rm -v $MAP.png
ps2raster -V -A -Tg -E300 $EPS
rm -v $EPS
eog $MAP.png &


