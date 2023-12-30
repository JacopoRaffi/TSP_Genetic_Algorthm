# Traveling Salesman Problem(TSP) using Genetic Algorithm

## Description
Given a list of cities and the distances between each pair of cities, the TSP algorithm aims at finding the shortest possible route that visits each city exactly once and returns to the origin city. 

The genetic algorithm to be implemented assumes a path is represented in a chromosome as a vector of integers P. P[k] = h, means the path visits city h as k-th city. Assuming a random initial population of chromosomes representing paths, crossover, mutation and other genetic algorithm techniques can be applied to make the population evolve to represent better and better solutions. 

The population evolution has to be implemented in parallel. Genetic TSP implementation should take as a parameter the number of iterations/generations to be computed before returning the result.

## Dependencies
The only dependency is [FastFlow](https://github.com/fastflow/fastflow).

Aldinucci, M. , Danelutto, M. , Kilpatrick, P. and Torquati, M. (2017). Fastflow: High‐Level and Efficient Streaming on Multicore. In Programming multi‐core and many‐core computing systems (eds S. Pllana and F. Xhafa).

## How to Compile 
To compile the three versions of the programme go to the "src" folder.
The commands to compile are:
- **sequential**: g++ -O3 -std=c++20 tsp\_sq.cpp -o sq;
- **native threads**: g++ -O3 -pthread -std=c++20 tsp\_par.cpp -o par;
- **FastFlow**: g++ -O3 -pthread -std=c++20 -I fastflow/path tsp\_ff.cpp -o ff.

## How to Run
To execute the programs the commands are (if we are in the "src" folder):
- **sequential**: ./sq file population mut_rate cross_rate generations
- **native threads**: ./par file population mut_rate cross_rate generations workers
- **FastFlow**: ./ff file population mut_rate cross_rate generations workers

### File .txt format
The .txt files contain the cities' coordinates.

File must contain, each row, the following values:
Coord_x Coord_y

Example:

9983.3333 98550.0000 <br />
10000.0000 98533.3333 <br />
10000.0000 98550.0000 <br />


### Execution examples
Examples on how to run the different versions:
- **sequential**: ./sq ../cities_500.txt 3000 0.3 0.7 1000
- **native threads**: ./par ../cities_500.txt 3000 0.3 0.7 1000 4
- **FastFlow**: ./ff ../cities_500.txt 3000 0.3 0.7 1000 4
