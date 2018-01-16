#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Usage: preprocess.sh spp prnfile"
   echo "       for example, preprocess skj skj26"
   exit 0
fi


makeifr
makepar
makeifr
makepar

WD=$2$1
mv -v makeifr.log makeifr_$WD.log
cp -v $2.ifr $WD.ifr
mv -v makepar.log makepar_$WD.log
mkdir -vp $WD
cp -v $2.tag $WD.tag
cp -v $2.par $WD
echo $2 > ./$WD/file_nam.tmp

echo "creating .cst file; close jni graphics window to continue"
/home/jsibert/movemod/25/scripts/makecst.sh $2
#/home/jsibert/movemod/25/scripts/prn2gmt.sh $2
/home/jsibert/movemod/25/landmask2gmt $2
/home/jsibert/movemod/25/scripts/gridmap $2
/home/jsibert/movemod/25/scripts/recapmap.sh $1 $2
