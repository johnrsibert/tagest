#!/bin/bash
LOG="$HOME/movemod/mmsync.log"
LOCALPATH=~/Projects/SPC-Tagging/SPC2012/
REMOTEPATH=katsuo:~/movemod/SPC2012/
touch $LOG
#declare -r DRY_RUN=--dry-run
echo "DRYRUN: "${DRY_RUN} >> $LOG
#
printf "\nFrom "$REMOTEPATH" to "$LOCALPATH"\n" >> $LOG
printf "Start: " >> $LOG
date >> $LOG
rsync ${DRY_RUN} --exclude-from=$HOME/movemod/25/scripts/mm_excludes.txt -avuzb $REMOTEPATH $LOCALPATH >> $LOG
#
printf "\nFrom "$LOCALPATH" to "$REMOTEPATH"\n" >> $LOG
printf "Start: " >> $LOG
date >> $LOG
rsync ${DRY_RUN} --exclude-from=$HOME/movemod/25/scripts/mm_excludes.txt -avuzb $LOCALPATH $REMOTEPATH >> $LOG
printf "Done:  " >> $LOG
date >> $LOG

# version for katsuo and SPC2011 data
##!/bin/bash
##LOG="/tmp/mmsync.log"
#LOG="/home/jsibert/movemod/mmsync.log"
#touch $LOG
##declare -r DRY_RUN=--dry-run
##
#printf "\nFrom Katsuo:\n" >> $LOG
#printf "Start: " >> $LOG
#date >> $LOG
#rsync ${DRY_RUN} --exclude-from=/home/jsibert/movemod/bin/mm_excludes.txt -avuzb katsuo:~/movemod/SPC2011/ ~/movemod/SPC2011/ >> $LOG
##
#printf "\nTo   Katsuo:\n" >> $LOG
#printf "Start: " >> $LOG
#date >> $LOG
#rsync ${DRY_RUN} --exclude-from=/home/jsibert/movemod/bin/mm_excludes.txt -avuzb ~/movemod/SPC2011/ katsuo:~/movemod/SPC2011/ >> $LOG
#printf "Done:  " >> $LOG
#date >> $LOG
#
#
