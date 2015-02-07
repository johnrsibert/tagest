#!/bin/bash 
#wget http://atlas.nmfs.hawaii.edu/cgi-bin/reynolds_extract.py?lon1=150\&lon2=180\&lat1=0\&lat2=30\&year1=2003\&day1=2\&year2=2004\&day2=57 -O test.zip
#R --vanilla < get.sst.from.server.R

#for 
#> mdy.date(1,1,2003)
#[1] 1Jan2003
#> mdy.date(1:12,1,2003)-mdy.date(1,1,2003)
# [1]   0  31  59  90 120 151 181 212 243 273 304 334
#> mdy.date(1:12,1,2004)-mdy.date(1,1,2004)
# [1]   0  31  60  91 121 152 182 213 244 274 305 335


#declare -r FILES=`ls sst/*`
#for  f in $FILES 
#do
#	echo $f ;
#done
R --vanilla < groupSSTByYearMonthLatitudeLongitude.R
