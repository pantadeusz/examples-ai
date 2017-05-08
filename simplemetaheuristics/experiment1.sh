#!/bin/bash

export SUMSIM=0
export SUMCLI=0

export N=100
g++ -std=c++11 -O3 simannealing.cpp -o simannealing
g++ -std=c++11 -O3 hillclimb.cpp -o hillclimb

for i in `seq 1 $N`; do
	S=$(./simannealing | grep Solution | awk '{print $3}' | sed -e 's/[eE]+*/\*10\^/')
	C=$(./hillclimb | grep Solution | awk '{print $3}' | sed -e 's/[eE]+*/\*10\^/')
	echo $S $C
	SUMSIM=$(echo "$SUMSIM+$S" | bc -l)
	SUMCLI=$(echo "$SUMCLI+$C" | bc -l)
	
done
echo "Simannealing: $SUMSIM; Hillclimbing: $SUMCLI"
SUMSIM=$(echo "$SUMSIM/$N" | bc -l)
SUMCLI=$(echo "$SUMCLI/$N" | bc -l)

echo "Simannealing: $SUMSIM; Hillclimbing: $SUMCLI"
