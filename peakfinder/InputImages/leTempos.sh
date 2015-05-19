#!/bin/bash

#-------------------------------------
# INPUTS
#-------------------------------------
prefix=out.o2
ini=$1
end=$2
text=Total

#-------------------------------------
# GET LINES
#-------------------------------------
for i in `seq $ini $end`
do
#  echo $prefix$i;
  cat $prefix$i | grep "The code to" | sed -e 's/[^0-9\.]//g';
done

