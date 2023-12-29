# Parallel and Distributed Systems: Paradigms and Models final Project

## Description
Given a list of cities and the distances between each pair of cities, the TSP algorithm aims at finding the shortest possible route that visits each city exactly once and returns to the origin city. The genetic
algorithm to be implemented assumes a path is represented in a chromosome as a vector of integers P. P[k] = h, means the path visits city h as k-th city. Assuming a random initial population of chromosomes representing paths, crossover, mutation and other genetic algorithm techniques can be applied to make the population evolve to represent better and better solutions. The population evolution has to be implemented in parallel. Genetic TSP implementation should take as a parameter the number of iterations/generations to be computed before returning the result.

## How to Compile 
To compile and run the various versions of the programme go to the "src" folder.
The commands to compile are:
- **sequential**: g++ -O3 -std=c++20 tsp\_sq.cpp -o sq;
- **native threads**: g++ -O3 -pthread -std=c++20 tsp\_par.cpp -o par;
- **FastFlow**: g++ -O3 -pthread -std=c++20 -I fastflow/path tsp\_ff.cpp -o ff.

## How to Run
