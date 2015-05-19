#!/bin/sh

./leTempos.sh $1 $2 | wc -l > tempos.txt; ./leTempos.sh $1 $2 >> tempos.txt
cat tempos.txt | kbest $3 $4
