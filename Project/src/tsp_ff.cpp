#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <thread>
#include <numeric>
#include "utimer.hpp"
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>

using namespace std;
using namespace ff;

using namespace std;
using Graph = vector<vector<double>>; //Adjiacency Matrix
using chromosome = pair<vector<int>, double>;

double fitness(vector<int>& path, Graph& g){
    double sum = 0.0;
    int size = path.size();
    for(int i = 0; i < size-1; i++){
        sum += g[path[i]][path[i+1]];
    }
    sum += g[path[size-1]][path[0]];

    return (1/sum);
}

vector<pair<double, double>> read_coord_file(string file){
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

Graph graph_init(vector<pair<double, double>>& cities){ 
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

class merge_em : public ff_minode{
    private: 
        vector<chromosome>* population;
        vector<chromosome>* selected; 
        int generations;
        int workers;
        bool first_gen = true;
        void *res;
    
    public:
        merge_em(int generations, int workers, vector<chromosome>* pops, vector<chromosome>* parents){
            this->generations = generations;
            this->workers = workers;
            population = pops;
            selected = parents;
        }

        void* svc(void* inp){
            if(generations != 0){
                if(first_gen){ 
                    for(int i = 0; i < workers; i++){
                        ff_send_out(GO_ON);
                    }
                    first_gen = false;
                    return GO_ON;
                }
                all_gather(inp, &res); //wait all workers
                generations--; //decrease generations
                swap_ranges(selected->begin(), selected->end(), population->begin());
                std::sort(population->begin(), population->end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });

                return GO_ON;
            }
            else{
                return EOS;
            }
        }
};

class worker_SCMF : public ff_node{
    private: 
        vector<chromosome>* population;
        vector<chromosome>* selected; 
        int start;
        int end;

    public: 
        worker_SCMF(vector<chromosome>* plt, vector<chromosome>* slc, int start, int end){
            this->start = start;
            this->end = end;
            population = plt;
            selected = slc;
        }
};

int main(int argc, char* argv[]){
     if(argc < 6){
        cout << "Usage: " << argv[0] << " file populaton mutation_rate crossover_rate generations workers" << "\n";
        return -1;
    }

    string file = argv[1];
    int population_size = atoi(argv[2]);
    double mutation_rate = atof(argv[3]);
    int parents = atof(argv[4]) * population_size;
    int generations = atoi(argv[5]);

    unsigned int max_workers = thread::hardware_concurrency();
    int workers = atoi(argv[6]);

    if(workers > max_workers)
        workers = max_workers - 1; //exclude thread main

    if(parents%2) 
        parents--;

    vector<pair<double, double>> cities = read_coord_file(file);
    Graph g = graph_init(cities);
    vector<chromosome> population = vector<chromosome>(population_size);
    vector<chromosome> selected = vector<chromosome>(parents);

    auto gen_work = [&](unsigned int i){
        random_device rd;
        mt19937 generator{rd()};
        population[i].first = vector<int>(g.size());
        population[i].first[0] = 0;
        population[i].second = 0;

        iota(population[i].first.begin(), population[i].first.end(), 0);
        shuffle(population[i].first.begin() + 1, population[i].first.end(), generator);

        population[i].second = fitness(population[i].first, g);
    };

    utimer ut("ALL: ");
    //Generation phase (parallelized)
    parallel_for(0, population_size, gen_work, workers);
    std::sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; }); //need sorted
    //Selection + Crossover + Mutation + Fitness
    //Distribution of couples among workers
    
    if((parents/2) < workers){ //too many workers for the size of parents to select
        workers = parents/2;
    }
    
    vector<pair<int, int>> indexes(workers);
    int load = parents / workers;
    if (load % 2)
        load--; 
    int last_index = -1;

    for (int i = 0; i < workers; i++){
        indexes[i].first = ++last_index;
        indexes[i].second = indexes[i].first + load;
        last_index = indexes[i].second - 1;
    }

    // distribution of the remaining load between workers
    int loads_to_add = (parents - last_index - 1) / 2;
    int count = 1;

    for (int i = workers - loads_to_add; i < workers; i++){
        indexes[i].first = indexes[i-1].second;
        indexes[i].second = indexes[i].second + 2 * count;
        count++;
    }

    merge_em e(generations, workers, &population, &selected);
}