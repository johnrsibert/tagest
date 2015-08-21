#!/bin/bash
# uses awk to process  .prn file and set environment variables
# see http://objectmix.com/awk/26656-returning-variables-shell-awk.html
# invokes gmt2cst
# renames cst file
# close graphics window to finish script
if [ $# -lt 1 ]; then
   echo "Usage: makecst <prn file> <sw longitude> <sw latitude> <m>  <n>"
   echo "<prn file>: "
   echo "<sw longitude>: southwest corner longitude with range [0, 360) degress"
   echo "<sw latitude>: southwest corner latitude with range [-90, -90] degrees"
   echo "<m>: size of the grid in the x direction"
   echo "<n>: size of the grid in the y direction"
   echo "Usage: gridmap prnfile"
   echo "       for example, makecst skj16 115 -20 80 35"
   exit 0
fi
echo "Close file to finish script"
fileroot=$1
swlon=$2
swlat=$3
M=$4
N=$5
echo "prn file:" $fileroot".prn"
echo "swlon:" $swlon
REGION="-R"$swlon"/"$(($swlon+$M))"/"$swlat"/"$(($swlat+$N))
echo "REGION:" $REGION
PROJECTION=-JM6i
echo "PROJECTION:" $PROJECTION
gmtfile=$fileroot.gmt
#extract coastline data for gmt2cst
pscoast $REGION $PROJECTION -V -Di -W -M > $gmtfile

#convert gmt output to jnigraphics format
# Note: gmt2cst must be on your PATH
gmt2cst $gmtfile $swlon $swlat $M $N
#/home/other/tagest/jnigraphics/jnigraphics/gmt2cst $gmtfile $swlon $swlat $M $N
mv -fv $fileroot.gmt.cst $fileroot.cst 
echo "finished"
