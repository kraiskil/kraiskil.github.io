#!/bin/bash

g++ code.cc -Wall -pthread

samples=1000
rm -f log.csv
echo "run_no,result" > log.csv

for i in $(seq 0 $(($samples-1)) )
do
	res=$(./a.out)
	echo "$i,$res" >> log.csv
done

python3 plot.py
