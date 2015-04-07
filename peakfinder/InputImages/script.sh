#!/bin/sh
for i in `ls ??.tif`
do
	../dist/Debug/GNU-Linux-x86/peakfinder $i&
done;

sleep 5;

haveEnded=1
curTime=0;
timeout=30;

while(($haveEnded > 0 && curTime < timeout));
do
	curTime=$((curTime+1));
	sleep 1;
	haveEnded=`ps | grep peakfinder | wc | awk '{print $1}'`;
done;

pidD=`ps | grep peakfinder | awk '{print $1}'`; echo $pidD; kill -9 $pidD;

mv ??_*.tif ../OutputImages
mv ??_*.csv ../OutputLog