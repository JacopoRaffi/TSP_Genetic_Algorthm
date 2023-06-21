#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <numeric>
#include <thread>
#include <barrier>
#include "utimer.hpp"

using namespace std;
using Graph = vector<vector<double>>; //Adjiacency Matrix
using chromosome = pair<vector<int>, double>;

Graph g;
vector<chromosome> population;
vector<chromosome> selected;
bool go_on = false;
mutex worker_lock;
condition_variable wait_merge; //condition variable used by work to understand when they can start their work

/**
     * @brief Computes the fitness of a chromosome 
     * @param path is a possible solution for TSP
     * @return the fitness value
     */
double fitness(vector<int>& path){
    double sum = 0.0;
    int size = path.size();
    for(int i = 0; i < size-1; i++){
        sum += g[path[i]][path[i+1]];
    }
    sum += g[path[size-1]][path[0]];

    return (1/sum);
}

/**
     * @brief Save into a vector of double the coordinates of the cities 
     * @param file is the txt file with city coordinates.
     * @return a vector containing the coordinates of each city
     */
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

/**
     * @brief Computes the euclidean distance between two points 
     * @param coord_a is the first coordinates
     * @param coord_b is the second coordnates
     * @return the distance computed
     */
inline double euclidean_distance(pair<double, double> coord_a, pair<double, double> coord_b){
    return sqrt(pow(coord_b.first - coord_a.first, 2) + pow(coord_b.second - coord_a.second, 2));
}

/**
     * @brief Initialize the adjiacency matrix
     * @param cities is a vector with coordinates of the cities
     * @return the Graph
     */
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

/**
     * @brief Fix a chromosome after the swap between two parents (avoid repeated nodes)
     * @param chr is the chromosome to fix
     */
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

/**
     * @brief Select two parents from population
     * @param gen is the generator for random values
     * @param prob is the probability distributon for random values
     * @param max_fitness is the maximum fitness of the previous generation
     */
pair<int, int> select_parents(mt19937& gen, uniform_real_distribution<double>& prob, double max_fitness){
    uniform_int_distribution<int> ind_gen(0, population.size()-1);
    int par_1 = ind_gen(gen), par_2 = ind_gen(gen);
    double probability = prob(gen);

    while(probability > (population[par_1].second / max_fitness)){
        //probability = prob(gen);
        par_1 = ind_gen(gen);
    }

    //select second parent different from first
    while(probability > (population[par_2].second / max_fitness) && (par_2 != par_1)){
        //probability = prob(gen);
        par_2 = ind_gen(gen);
    }

    return (make_pair(par_1, par_2));
}

/**
     * @brief Apply, with a certain probability, the mutation on a chromosome
     * @param chr is a chromosome
     * @param rate the probability that the mutation occurs
     * @param generator generator used for random value
     */
void mutation(chromosome& chr, double rate, mt19937& generator){
    uniform_int_distribution<int> index_gen(1, g.size() - 1); //generate the value to compare to choose population
    uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

    if(prob_gen(generator) <= rate){
        int gene_1 = index_gen(generator);
        int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
        swap(chr.first[gene_1], chr.first[gene_2]);
    }
}

/**
     * @brief Task executed by workers threads
     * @param start first index of the interval of a worker
     * @param end last index(not included) of the interval of a worker
     * @param generatons is the number of iteration to execute
     * @param b is the barrier used to understand when merge phase can start
     * @param mutation_rate is the probability that the mutaton occurs
     */
void task(int start, int end, int generations, barrier<void(*)(void) noexcept>& b, double mutation_rate){
    int pop_size = population.size();
    random_device rd;
    mt19937 generator{rd()};
    uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
    uniform_int_distribution<int> cross_gen(3, g.size() - 2); //i want to avoid parts of one element
    
    for(int i = 0; i < generations; i++){
       {
        unique_lock<mutex> lock(worker_lock);
        wait_merge.wait(lock, []() {return go_on;});
       }
       
       //selection + crossover + mutation + fitness(select 2 parents, crossover them, mutate and fitness)
        double max_fitness = population[pop_size-1].second; //population is sorted
        int count = start; // count must be in [start, end)
        
        while(count < end){
            pair<int, int> parents = select_parents(generator, distribution, max_fitness);
            selected[count++] = population[parents.first];
            selected[count++] = population[parents.second];
            
            int cross_index = cross_gen(generator);
            
            auto begin_first = selected[count-2].first.begin();
            auto begin_second = selected[count-1].first.begin();
            auto end_first = selected[count-2].first.end();
            auto end_second = selected[count-1].first.end();
            
            swap_ranges(begin_first, begin_first + cross_index, begin_second);
            swap_ranges(begin_second + cross_index, end_second, begin_second + cross_index);
            
            fix_chromosome(selected[count-2]); //first parent
            fix_chromosome(selected[count-1]); //second parent
            
            mutation(selected[count-2], mutation_rate, generator);
            mutation(selected[count-1], mutation_rate, generator);
            
            selected[count-2].second = fitness(selected[count-2].first); 
            selected[count-1].second = fitness(selected[count-1].first); 
        }

        b.arrive_and_wait();
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
    g = graph_init(cities);
    population = vector<chromosome>(population_size);
    utimer ut("ALL: ");

    //worker function for the first generation of chromosome
    auto gen_work = [](int start, int end){
        random_device rd;
        mt19937 generator{rd()};
        for(int i = start; i < end; i++){
            population[i].first = vector<int>(g.size());
            population[i].first[0] = 0;
            population[i].second = 0;

            iota(population[i].first.begin(), population[i].first.end(), 0);
            shuffle(population[i].first.begin() + 1, population[i].first.end(), generator);

            population[i].second = fitness(population[i].first);
        }
    };
    vector<thread> pool;

    int remainder = population_size % workers; //try to load balance (distribute remainder work among threads)
    int size = population_size / workers;
    int start = 0, end = 0;
    for(int i = 0; i < workers; i++){
        start = end;
        end += (remainder > 0) ? (size+1) : size;
        remainder--;
        pool.push_back(thread(gen_work, start, end));
    }

    for(int i = 0; i < workers; i++){
        pool[i].join();
    }
    pool.clear();
    std::sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; }); //need sorted
    //Selection + Crossover + Mutation + Fitness
    //Distribution of couples among workers
    
    if((parents/2) < workers){ //too many workers for the size of parents to select
        workers = parents/2;
    }
    
    vector<pair<int, int>> indexes(workers);
    int load = parents / workers; //load to distribute between workers
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

    selected = vector<chromosome>(parents);
    barrier<void(*)(void) noexcept> b(workers+1, []() noexcept{ go_on = false;}); //barrier used to understand when the merge phase can start

    for (int i = workers - loads_to_add; i < workers; i++){
        indexes[i].first = indexes[i-1].second;
        indexes[i].second = indexes[i].second + 2 * count;
        count++;
    }

    for(int i = 0; i < workers; i++){
        pool.push_back(thread(task, indexes[i].first, indexes[i].second, generations, ref(b), mutation_rate));
    }
   
    for(int i = 0; i < generations; i++){//thread main is "Merge phase"
        {
            unique_lock lck(worker_lock);
            go_on = true;
            wait_merge.notify_all(); 
        }
        b.arrive_and_wait(); //merge wait
        swap_ranges(selected.begin(), selected.end(), population.begin());
        std::sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });
    }

    for(int i = 0; i < pool.size(); i++){
        pool[i].join();
    }
}