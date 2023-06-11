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

void fix_chromosome(chromosome& chr){
        //check the occurences of each city
    vector<int> duplicate(chr.first.size(), 0); //ndexes represent cities
    bool need_fix = false;
    vector<int> missing;

    for(int i = 0; i < chr.first.size(); i++){
        duplicate[chr.first[i]] += 1;
        need_fix = need_fix || (duplicate[chr.first[i]] == 2);
    }
    
    if(need_fix){
        for(int i = 0; i < chr.first.size(); i++){
            if(duplicate[i] == 0)
                missing.push_back(i);
        }
        int miss_index = 0;
        for(int i = 0; i < chr.first.size(); i++){
            if(duplicate[chr.first[i]] == 2){
                duplicate[chr.first[i]]--;
                chr.first[i] = missing[miss_index];
                miss_index++;
            }
        }    
    }
}

pair<int, int> select_parents(mt19937& gen, uniform_real_distribution<double>& prob, double max_fitness, vector<chromosome>& population){
    uniform_int_distribution<int> ind_gen(0, population.size()-1);
    int par_1 = ind_gen(gen), par_2 = ind_gen(gen);
    double probability = prob(gen);

    while(probability > (population[par_1].second / max_fitness)){
        //probability = prob(gen);
        par_1 = ind_gen(gen);
    }

    //select second parent different from first
    while((probability > (population[par_2].second / max_fitness)) && (par_2 != par_1)){
        //probability = prob(gen);
        par_2 = ind_gen(gen);
    }

    return (make_pair(par_1, par_2));
}

void mutation(chromosome& chr, double rate, mt19937& generator){
    uniform_int_distribution<int> index_gen(1, chr.first.size() - 1); //generate the value to compare to choose population
    uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

    if(prob_gen(generator) <= rate){
        int gene_1 = index_gen(generator);
        int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
        swap(chr.first[gene_1], chr.first[gene_2]);
    }
}

class merge_em : public ff_monode{
    private: 
        vector<chromosome>* population;
        vector<chromosome>* selected; 
        int generations;
        int workers;
        int counter = 0;
        bool first_gen = true;
    
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
                    broadcast_task(GO_ON);
                    first_gen = false;
                    return GO_ON;
                }

                if(counter < (workers-1)){
                    counter++;
                    cout << counter << "\n";
                    return GO_ON;
                }
                swap_ranges(selected->begin(), selected->end(), population->begin());
                std::sort(population->begin(), population->end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });
                counter = 0;
                generations--;
                broadcast_task(GO_ON);
                return GO_ON;
            }
            else{
                return EOS;
            }
        }
};

class worker_SCMF : public ff_node{
    private: 
        Graph* g;
        vector<chromosome>* population;
        vector<chromosome>* selected; 
        int start;
        int end;
        double mutation_rate;
        random_device rd;
        mt19937 generator{rd()};

    public: 
        worker_SCMF(vector<chromosome>* plt, vector<chromosome>* slc, Graph* graph, int start, int end, double m_rate){
            this->start = start;
            this->end = end;
            mutation_rate = m_rate;
            population = plt;
            selected = slc;
            g = graph;
        }

        void* svc(void* inp){
            double max_fitness = (*population)[population->size()-1].second; //population is sorted
            int count = start; // count must be in [start, end)
            uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
            uniform_int_distribution<int> cross_gen(3, g->size() - 2); //i want to avoid parts of one element
        
            while(count < end){
                pair<int, int> parents = select_parents(generator, distribution, max_fitness, *population);
                (*selected)[count++] = (*population)[parents.first];
                (*selected)[count++] = (*population)[parents.second];
                
                int cross_index = cross_gen(generator);
            
                auto begin_first = (*selected)[count-2].first.begin();
                auto begin_second = (*selected)[count-1].first.begin();
                auto end_first = (*selected)[count-2].first.end();
                auto end_second = (*selected)[count-1].first.end();
               
                swap_ranges(begin_first, begin_first + cross_index, begin_second);
                swap_ranges(begin_second + cross_index, end_second, begin_second + cross_index);
                
                fix_chromosome((*selected)[count-2]); //first parent
                fix_chromosome((*selected)[count-1]); //second parent
               
                mutation((*selected)[count-2], mutation_rate, generator);
                mutation((*selected)[count-1], mutation_rate, generator);
               
                (*selected)[count-2].second = fitness((*selected)[count-2].first, *g); 
                (*selected)[count-1].second = fitness((*selected)[count-1].first, *g); 
               
            }
            cout << "END W: \n";
            ff_send_out(GO_ON);
            return GO_ON;
        }
};

void printPop(vector<chromosome>& p){
    for(int i = 0; i < p.size(); i++){
        cout << "\n\n" << p[i].first.size() << "\n\n";
        cout << "F: " << p[i].second << "  ";
        for(int j = 0; j < p[i].first.size(); j++){
            cout << "" << p[i].first[j] << "  ";
        }
        cout << "\n";
    }
}

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
    vector<unique_ptr<ff_node>> workers_pool;

    for(int i = 0; i < workers; i++){
        workers_pool.push_back(make_unique<worker_SCMF>(&population, &selected, &g, indexes[i].first, indexes[i].second, mutation_rate));
    }

    ff_Farm<> tsp(move(workers_pool));
    tsp.add_emitter(e);
    tsp.remove_collector();
    tsp.wrap_around();

    tsp.run_and_wait_end();
}