#!/bin/bash
# uses awk to process  .prn file and set environment variables
# see http://objectmix.com/awk/26656-returning-variables-shell-awk.html
# invokes gmt2cst
# renames cst file
# close graphics window to finish script
fileroot=$1
echo "prn file:" $fileroot".prn"
gmtfile=$fileroot.gmt
set -a $(/usr/bin/gawk ' 
BEGIN{fr=0
}
{
  if ((match($0,"#")==0) && (fr == 0))
  {
    fr=1
    print $1
    print $2
    print lon_field($5)
    print lat_field($6)
  }
}
function lon_field(field)
{
  sub("E","",field)
  sub("W","",field)
  return(field)
}
 
function lat_field(field)
{
  tmp = field
  sub("S","",tmp)
  sub("N","",tmp)
  nc = length(field)
  if (substr(field,nc) == "S")
    tmp = -tmp
  return(tmp)
}' $1.prn
)
M=$1
N=$2
swlon=$3
swlat=$4

echo $swlat
echo $swlon
echo $N
echo $M

gmt2cst $gmtfile $swlon $swlat $M $N
mv -fv $fileroot.gmt.cst $fileroot.cst 
echo "finished"
