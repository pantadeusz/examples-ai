#!/bin/bash

if [ "$1" = "" ]; then
    echo "$0 [nazwa testowanego programu]"
    exit 1
fi

echo "10 10 100 100" > test0.input.txt
echo "100 100 10 10" > test1.input.txt
echo "10 10 1000 1000" > test2.input.txt
echo "1000 1000 10 10" > test3.input.txt
echo "10 100 100 10" > test4.input.txt
rm -f __result.txt
echo "APP: $1" > __result.txt
for i in `seq 0 100`; do
	/usr/bin/time -p --output=__result.txt --append $1 test$((i%5)).png test$(((i%19)%5)).input.txt test_$(((i%19)%5))_$((i%5)).output.txt
done
for i in `seq 0 4`; do
	rm test$((i%5)).input.txt
done


