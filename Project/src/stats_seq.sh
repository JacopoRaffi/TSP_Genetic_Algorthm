#!/bin/bash

#g++ -O3 -pthread -g -std=c++20 -I ../../../fastflow/ tsp_ff.cpp -o ff
#g++ -O3 -pthread -g -std=c++20 tsp_par.cpp -o par
g++ -O3 -g -std=c++20 tsp_sq.cpp -o sq

echo "Sequential - 1000 iterations"
# file population mut_rate cross_rate generations
echo "500 nodes - 3000 population"
for (( i=0; i < 5; i++ ))    
do
    ./sq ../cities_500.txt 3000 0.3 0.7 1000
done

echo "1000 nodes - 6000 population"
for (( i=0; i < 5; i++ ))    
do
    ./sq ../cities_1000.txt 6000 0.3 0.7 1000
done

echo "2000 nodes - 12000 population"
for (( i=0; i < 5; i++ ))    
do
    ./sq ../cities_2000.txt 12000 0.3 0.7 1000
done

