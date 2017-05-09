#!/bin/bash


for i in 10 100 1000 10000 100000 1000000; do
	> result_$i.txt
	for j in `seq 1 25`; do
		./hillclimb  iterations $i | grep Solution | awk '{print $4}' >> result_$i.txt
	done
done

echo "N; " > result.txt
for i in 10 100 1000 10000 100000 1000000; do
	echo -n "$i;" >> result.txt
	for j in `cat result_$i.txt`; do
		echo -n "$j;" >> result.txt
	done
	echo >> result.txt
done

