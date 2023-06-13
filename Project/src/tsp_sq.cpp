#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <numeric>
#include "utimer.hpp"

using namespace std;
using Graph = vector<vector<double>>; //Adjiacency Matrix
using chromosome = pair<vector<int>, double>;

Graph g;
vector<chromosome> population;
random_device rd;
mt19937 generator{rd()};

double fitness(vector<int> path){
    //utimer ut("FITN: ");
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

void generation(int population_size, int start_vertex){
    population = vector<chromosome>(population_size); 
    for(int i = 0; i < population_size; i++){
        population[i].first = vector<int>(g.size());
        population[i].first[0] = start_vertex;
        population[i].second = 0;

        iota(population[i].first.begin(), population[i].first.end(), 0);
        swap(population[i].first[0], population[i].first[start_vertex]);
        shuffle(population[i].first.begin() + 1, population[i].first.end(), generator);

        population[i].second = fitness(population[i].first);
    }
}

void selection(int selection_number, vector<chromosome>& selected){
    double total_fitness = 0.0;
    int size = population.size();
    uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
    uniform_int_distribution<int> index_gen(0, size-1);
        
    double max_fitness = population[size-1].second; //population is sorted
    int count = 0;
        
    while(count < selection_number){
        int index = index_gen(generator);
        double probability = distribution(generator);
        while(probability > (population[index].second / max_fitness)){
            index = index_gen(generator);
        }
        selected[count] = population[index];
        count++;
    }
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

void mutation(chromosome& chr, double rate){
    uniform_int_distribution<int> index_gen(1, g.size() - 1); //generate the value to compare to choose population
    uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

    if(prob_gen(generator) <= rate){
        int gene_1 = index_gen(generator);
        int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
        swap(chr.first[gene_1], chr.first[gene_2]);
    }
}

void crossover(vector<chromosome>& selected, double mutation_rate){
    uniform_int_distribution<int> index_gen(3, g.size() - 2); //i want to avoid parts of one element
    
    for(int i = 0; i < selected.size() - 1; i += 2){
        int index = index_gen(generator);
        auto begin_first = selected[i].first.begin();
        auto begin_second = selected[i+1].first.begin();

        auto end_first = selected[i].first.end();
        auto end_second = selected[i+1].first.end();

        {
            utimer ut("CROSS: ");
        swap_ranges(begin_first, begin_first + index, begin_second);
        swap_ranges(begin_second + index, end_second, begin_second + index);
        }   
        fix_chromosome(selected[i]);
        fix_chromosome(selected[i+1]);

        mutation(selected[i], mutation_rate);
        mutation(selected[i+1], mutation_rate);

        selected[i].second = fitness(selected[i].first); 
        selected[i+1].second = fitness(selected[i+1].first); 
    }
}

int main(int argc, char* argv[]){
    /*
    * file coordinate 
    * population
    * mutation Rate
    * crossover rate
    */

    if(argc < 5){
        cout << "Usage: " << argv[0] << " file populaton mutation_rate crossover_rate generations" << "\n";
        return -1;
    }
    string file = argv[1];
    int population_size = atoi(argv[2]);
    double mutation_rate = atof(argv[3]);
    int parents = atof(argv[4]) * population_size;
    int generations = atoi(argv[5]);
    
    if(parents%2) 
        parents--;

    vector<pair<double, double>> cities = read_coord_file(file);
    g = graph_init(cities);
    utimer all("ALL: ");
    generation(population_size, 0); //generation + first evaluation
    sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; }); //need sorted
    vector<chromosome> selected(parents);
    for(int i = 0; i < generations; i++){
        selection(parents, selected);
        crossover(selected, mutation_rate);
        {
            swap_ranges(selected.begin(), selected.end(), population.begin());
            sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });
        } 
    }
    return 0;
}