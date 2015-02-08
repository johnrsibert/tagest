#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Usage: preprocess.sh spp prnfile"
   echo "       for example, preprocess skj skj26"
   exit 1
fi

export MOVEMODHOME=../../25
export SCRIPTDIR=$MOVEMODHOME/scripts
echo $SCRIPTDIR

$MOVEMODHOME/makeifr
$MOVEMODHOME/makepar
$MOVEMODHOME/makeifr
$MOVEMODHOME/makepar

WD=$2$1
mv -v makeifr.log makeifr_$WD.log
cp -v $2.ifr $WD.ifr
mv -v makepar.log makepar_$WD.log
mkdir -vp $WD
cp -v $2.tag $WD.tag
cp -v $2.par $WD
echo $2 > ./$WD/file_nam.tmp

##echo "creating .cst file; close jni graphics window to continue"
##/home/jsibert/movemod/25/scripts/makecst.sh $2
$SCRIPTDIR/makecst.sh $2
$SCRIPTDIR/prn2gmt.sh $2
$MOVEMODHOME/landmask2gmt $2
# $SCRIPTDIR/gridmap $2
# run below separate after check the scripts.
#$SCRIPTDIR/tagtab $2
#$SCRIPTDIR/recapmap.sh $1 $2

exit 0
