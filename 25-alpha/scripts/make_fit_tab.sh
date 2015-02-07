#!/bin/bash
if [ $# -lt 1 ]; then
   echo "Error: file name root not specifid on command line"
   echo "Usage: make_fit_tab.sh root"
   echo "       for example, make_fit_tab.sh skj16"
   echo "       use '*' for recursive tabulation => global"
   exit 1
fi

export MOVEMODHOME=$HOME/movemod/25
export SCRIPTDIR=$MOVEMODHOME/scripts
echo $SCRIPTDIR
#echo "Running "$SCRIPTDIR"/fit.tab("$1")"
rm -fv all23*fit_tab*
# sart R source a file and run some functions
R --slave <<EOF
source("$SCRIPTDIR/fit_tab.R")
fit.tab("$1")
EOF
libreoffice --calc $1_fit_tab.csv &

