#!/bin/bash

#-------------------------------------
# REGISTO DE CAMINHO
#-------------------------------------
echo "Working space:"
cam=`pwd`
echo $cam; echo

#-------------------------------------
# INPUTS
#-------------------------------------
lnodes=compute-641-17
ppn=30
pbs=p.pbs

export PATH=$PATH:/share/apps/gnu/gnuplot/4.6.6/bin/
module add gnu/4.9.0
module add intel/2013.1.117
cd ..
make 2>vec.txt
cd InputImages/
rm ??_*

#-------------------------------------
# QSUBS
#-------------------------------------
for i in `ls ??.tif`
do
  #qsub -V -v file=$i -lnodes=$lnodes:ppn=$ppn -d $cam -q mei $pbs;
  for j in `seq 1 30`
  do
    qsub -V -v file=$i -lnodes=$lnodes:ppn=$ppn -d $cam -q mei $pbs;
    sleep 3
  done
done
#COMMENT1

