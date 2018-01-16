#!/bin/bash

for file in $1.p[0-9,a-f][0-9,a-f]
do
  echo Processing $file
# ls -l $file
  cp -fv $file $1.par
  par2xml
done
