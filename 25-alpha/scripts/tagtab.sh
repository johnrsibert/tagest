#!/bin/bash
if [ $# -lt 2 ]; then
   echo "Usage: tagtab.sh spp size"
   echo "       for example, tagtab SKJ 05"
   exit 0
fi
export MOVEMODHOME=$HOME/movemod/25
export SCRIPTDIR=$MOVEMODHOME/scripts

# sibert_recovery_all_table_SKJ_all_PTTP_05deg.txt
#sibert_recovery_all_table_SKJ_PTTP_1deg.txt
#TAGFILE="./sibert_recovery_all_table_"$1"_PTTP_"$2"deg.txt"
#TAGFILE="../sibert_recovery_verified_table_"$1"_PTTP_"$2"deg.txt"
TAGFILE="../sibert_recovery_all_table_"$1"_RTTP_"$2"deg.txt"
echo $TAGFILE

/usr/bin/gawk --file=$SCRIPTDIR/tagtab.awk $TAGFILE

mv -v rels.xyz $1_rels.xyz
mv -v caps.xyz $1_caps.xyz
#mv -v rels.xyz $1$2rels.xyz
#mv -v caps.xyz $1$2caps.xyz
