#!/bin/bash
#LOG="/tmp/mmsync.log"
LOG="/home/jsibert/movemod/mmsync.log"
touch $LOG
#declare -r DRY_RUN=--dry-run
#
printf "\nFrom Katsuo:\n" >> $LOG
printf "Start: " >> $LOG
date >> $LOG
rsync ${DRY_RUN} --exclude-from=/home/jsibert/movemod/bin/mm_excludes.txt -avuzb katsuo:~/movemod/SPC2011/ ~/movemod/SPC2011/ >> $LOG
#
printf "\nTo   Katsuo:\n" >> $LOG
printf "Start: " >> $LOG
date >> $LOG
rsync ${DRY_RUN} --exclude-from=/home/jsibert/movemod/bin/mm_excludes.txt -avuzb ~/movemod/SPC2011/ katsuo:~/movemod/SPC2011/ >> $LOG
printf "Done:  " >> $LOG
date >> $LOG
