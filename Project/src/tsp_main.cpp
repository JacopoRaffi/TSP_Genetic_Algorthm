#include <vector> 
#include <iostream>
#include <random>
#include <sstream>
#include <limits>
#include <fstream>

#include "./tsp_classes/tsp_seq.hpp"

using namespace std;

vector<pair<double, double>> read_coord_file(string file){
    utimer ut("FILE: ");
    ifstream read_file(file);
    string coordinates;
    stringstream parser;

    vector<pair<double, double>> cities;

    while(getline(read_file, coordinates)){
        parser.str(coordinates);
        string x, y;
        parser >> x >> y;
        cities.push_back(make_pair(stod(x), stod(y)));
        parser.clear();
    }

    return cities;
}

inline double euclidean_distance(pair<double, double> coord_a, pair<double, double> coord_b){
    return sqrt(pow(coord_b.first - coord_a.first, 2) + pow(coord_b.second - coord_a.second, 2));
}

Graph graph_init(vector<pair<double, double>>& cities, int seed){ 
    utimer ut("GRAPH: ");
    Graph g(cities.size());

    //lower triangular matrix (un-directed graph) so I save, more or less, half space 
    //Start from 1 because I exclude the diagonal
    for(int i = 0; i < cities.size(); i++){ //10 is just for test in my pc
        g[i] = vector<double>(cities.size());
        for(int j = 0; j < i; j++){
            g[i][j] = euclidean_distance(cities[i], cities[j]);
        }
    }
    return g;
}
 
Graph rand_graph(){
    Graph g(100);
    srand(5400);
    //lower triangular matrix (un-directed graph) so I save, more or less, half space 
    //Start from 1 because I exclude the diagonal
    for(int i = 1; i < 100; i++){ //10 is just for test in my pc
        g[i] = vector<double>(i);
        for(int j = 0; j < i; j++){
            g[i][j] = 1 + rand() % 10;
        }
    }
    return g;
}


int main(int argc, char *argv[]){
    if(argc < 6){
        cerr << "Usage: " << argv[0] << " workers file iterations population mode mutationRate(optional) crossoverRate(optional) seed(optional) start(optional)\n";
        return -1;
    }
    
    int workers = atoi(argv[1]);
    string file = argv[2];
    int generations = atoi(argv[3]);
    int population_size = atoi(argv[4]);
    string mode = argv[5];
    double mutation_rate = 0.3;
    int selection_number = 0.6 * population_size;
    int seed = 1234;
    int start_vertex = 1;

    if(argc >= 7){
        mutation_rate = atof(argv[6]);
    }

    if(argc >= 8){
        selection_number = atof(argv[7]) * population_size;
    }

    if(argc >= 8){
        seed = atoi(argv[8]);
    }

    if(argc >= 9){
        start_vertex = atoi(argv[9]);
    }

    if(!(selection_number % 2)){ //needs to be even
        selection_number++;
    }
    
    utimer time("ALL: ");
    vector<pair<double, double>> cities = read_coord_file(file);
    Graph g = graph_init(cities, seed);
    //Graph rand_g = rand_graph(); //just for simple test

    if(mode == "sq"){ //sequential mode
        TSPSeq tsp(g, population_size, start_vertex);
        tsp.genetic_algorithm(generations, mutation_rate, selection_number);
    }

    if(mode == "par"){ //naive C++ threads mode

    }

    if(mode == "ff"){ //fastFlow

    }

    return 0;
}