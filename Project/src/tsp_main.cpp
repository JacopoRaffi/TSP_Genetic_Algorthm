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
            cout << g[i][j] << " ";
        }
        cout << "\n";
    }
    return g;
}

/*  Parameters from command line:
    * Number of Threads;
    * Number of Vertexes;
    * Number of Generations;
    * mutation rate;
    * crossover rate;
    * Intermediate number (number of future children);
    * seed for graph values (optional);
*/  

int main(int argc, char *argv[]){
    Graph g = graph_init(10, 123);

    for(int i = 0; i < g.size(); i++){
        for(int j = 0; j < i; j++){
            cout << g[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}