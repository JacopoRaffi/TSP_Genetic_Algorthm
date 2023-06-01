#include <vector> 
#include <iostream>
#include <random>

using namespace std;

using Graph = vector<vector<double>>; //Adjiacency Matrix (lower triangular matrix)

Graph graph_init(int size, int seed){
    Graph g(size);
    default_random_engine gen;
    gen.seed(seed);
    uniform_real_distribution<double> unif(0, 200); 

    for(int i = 0; i < size; i++){
        g[i] = vector<double>(i);
        for(int j = 0; j < i; j++){
            g[i][j] = unif(gen);
        }
    }
    return g;
}

/*  Parameters from command line:
    * Number of Threads;
    * Number of Vertexes;
    * Number of Generations;
    * mode (sq, ff, par)
    * mutation rate (optional);
    * crossover rate (optional);
    * Selection number (optional);
    * seed for graph values (optional);
*/  

int main(int argc, char *argv[]){
    if(argc < 5){
        cerr << "Usage: " << argv[0] << " workers vertexes iterations mode mutationRate(optional) crossoverRate(optional) selectionNumber(optional) seed(optional)\n";
        return -1;
    }
    
    int workers = atoi(argv[1]);
    int size = atoi(argv[2]);
    int generations = atoi(argv[3]);
    string mode = argv[4];
    double mutation_rate = 0.3;
    double crossover_rate = 0.7;
    int selection_number = size/4;
    int seed = 1234;

    if(argv[5]){
        mutation_rate = atof(argv[5]);
    }

    if(argv[6]){
        crossover_rate = atof(argv[6]);
    }

    if(argv[7]){
        selection_number = atoi(argv[7]);
    }

    if(argv[8]){
        seed = atoi(argv[8]);
    }
    
    Graph g = graph_init(size, seed);

    /*for(int i = 0; i < g.size(); i++){
        for(int j = 0; j < i; j++){
            cout << g[i][j] << " ";
        }
        cout << "\n";
    }*/

    if(mode == "sq"){ //sequential mode

    }

    if(mode == "par"){ //naive C++ threads mode

    }

    if(mode == "ff"){ //fastFlow

    }

    return 0;
}