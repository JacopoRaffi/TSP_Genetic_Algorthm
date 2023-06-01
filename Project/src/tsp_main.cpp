#include <vector> 
#include <iostream>
#include <random>
#include <limits>

#include "./tsp_classes/tsp_seq.hpp"

using namespace std;

//using Graph = vector<vector<double>>; //Adjiacency Matrix (directed graph) 

Graph graph_init(int size, int seed){
    Graph g(size);
    default_random_engine gen;
    gen.seed(seed);
    uniform_real_distribution<double> unif(1, 200); 

    for(int i = 0; i < size; i++){
        g[i] = vector<double>(size);
        for(int j = 0; j < size; j++){
            g[i][j] = unif(gen);
        }
    }
    return g;
}

/*  Parameters from command line:
    * Number of Threads;
    * Number of Vertexes;
    * Number of Generations;
    * population size
    * mode (sq, ff, par)
    * mutation rate (optional);
    * crossover rate (optional);
    * Selection number (optional);
    * seed for graph values (optional);
*/  

int main(int argc, char *argv[]){
    if(argc < 6){
        cerr << "Usage: " << argv[0] << " workers vertexes iterations population mode mutationRate(optional) crossoverRate(optional) selectionNumber(optional) seed(optional) start(optional)\n";
        return -1;
    }
    
    int workers = atoi(argv[1]);
    int size = atoi(argv[2]);
    int generations = atoi(argv[3]);
    int population_size = atoi(argv[4]);
    string mode = argv[5];
    double mutation_rate = 0.3;
    double crossover_rate = 0.7;
    int selection_number = size/4;
    int seed = 1234;
    int start_vertex = 0;

    if(argv[6]){
        mutation_rate = atof(argv[6]);
    }

    if(argv[7]){
        crossover_rate = atof(argv[7]);
    }

    if(argv[8]){
        selection_number = atoi(argv[8]);
    }

    if(argv[9]){
        seed = atoi(argv[9]);
    }

    if(argv[10]){
        start_vertex = atoi(argv[10]);
    }
    
    Graph g = graph_init(size, seed);

    if(mode == "sq"){ //sequential mode

    }

    if(mode == "par"){ //naive C++ threads mode

    }

    if(mode == "ff"){ //fastFlow

    }
   
    TSPSeq tsp(g, population_size, start_vertex);

    return 0;
}