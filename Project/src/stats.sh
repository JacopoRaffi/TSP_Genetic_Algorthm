#!/bin/bash

g++ -O3 -pthread -g -std=c++20 -I ../../../fastflow/ tsp_ff.cpp -o ff
g++ -O3 -pthread -g -std=c++20 tsp_par.cpp -o par
g++ -O3 -g -std=c++20 tsp_sq.cpp -o sq

echo "Sequential"
# file population mut_rate cross_rate generations
for (( i=1; i <= 7; i++ ))    
do
    ./sq ../cities.txt 1000 0.3 0.7 10
done


echo "Native Threads C++"

#for (( i=1; i <= 7; i++ ))    
#do
 #   ./par ../cities.txt 1000 0.3 0.7 10 ${i}
#done


echo "FastFlow"