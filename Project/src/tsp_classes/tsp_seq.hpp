#ifndef TSP_SEQ_H
#define TSP_SEQ_H

#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <numeric>
#include "../utimer.hpp"

using namespace std;
using Graph = vector<vector<double>>; //Adjiacency Matrix

using chromosome = pair<vector<int>, double>;

//TODO: Test the done phases
class TSPSeq{
    private:
    Graph graph; 
    vector<chromosome> population;
    random_device rd;
    mt19937 generator{rd()};
    /**
     * Create the initial population.
     * @param population_size is the number of chromosome in the population.
     */
    void generation(int& population_size, int& start_vertex){
        utimer t("GENERATION: ");
        population = vector<chromosome>(population_size);

        for(int i = 0; i < population_size; i++){
            population[i].first = vector<int>(graph.size());
            population[i].first[0] = start_vertex;
            population[i].second = 0;

            iota(population[i].first.begin(), population[i].first.end(), 0);
            swap(population[i].first[0], population[i].first[start_vertex]);
            shuffle(population[i].first.begin() + 1, population[i].first.end(), generator);
        }
        //printPopulation();
    }

    //Just for test purposes.
    void printPopulation(){
        for(int i = 0; i < population.size(); i++){
            for(int j = 0; j < graph.size(); j++){
                cout << population[i].first[j] << " ";
            }
            cout << "FITNESS:" << population[i].second;
            cout << " Chromosome: " << i << "\n";
        }
    }

    //Just for test
    void printPath(chromosome& chr){
        cout << "PATH: ";
        for(int i = 0; i < chr.first.size(); i++){
            cout << chr.first[i] << " ";
        }
        cout << "FTNESS: " << chr.second << "\n";
    }
    
    /**
     * Compute the fitness of a given chromosome.
     * @param chr is a chromosome of the population.
     */
    void fitness(chromosome& chr){
        int size = chr.first.size();
        for(int i = 0; i < size - 1; i++){
            chr.second += graph[chr.first[i]][chr.first[i+1]];
        }
        chr.second += graph[chr.first[size-1]][chr.first[0]]; //edge between last element and start city
        chr.second = 1 / chr.second; //lower the distance better the fitness
    }

    /**
     * Compute the fitness of all chromosomes in the population.
     */
    void evaluation(){
        utimer ut("EVALUATION: ");
        for(int i = 0; i < population.size(); i++)
            fitness(population[i]);
        
        //printPopulation();
    }

    /**
     * Select the chromosome for crossover (stochastic acceptance N times).
     * @param selection_number is the number of chromosome to be selected for crossover phase
     * @return the chromosome selected for crossover
     */
    void selection(int& selection_number, vector<chromosome>& selected){
        utimer ut("SELECTION: ");
        double total_fitness = 0.0;
        int size = population.size();
        uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
        uniform_int_distribution<int> index_gen(0, size-1);
        
        double max_fitness = (*max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.second < b.second; })).second;
        int count = 0;
        
        while(count < selection_number){
            int index = index_gen(generator);
            double probability = distribution(generator);
            if(probability <= (population[index].second / max_fitness)){
                selected[count] = population[index];
                count++;
            }
        }
    }   

    /**
     * Apply crossover.
     * @param selected is the vector of parents chromosome
     */
    void crossover(vector<chromosome>& selected){
        uniform_int_distribution<int> index_gen(3, graph.size() - 2); //i want to avoid parts of one element
        //aply crossover (i, i+1)
        for(int i = 0; i < selected.size() - 1; i += 2){
            int index = index_gen(generator);
            auto begin_first = selected[i].first.begin();
            auto begin_second = selected[i+1].first.begin();

            auto end_first = selected[i].first.end();
            auto end_second = selected[i+1].first.end();

            //parents crossover
            swap_ranges(begin_first, begin_first + index, begin_second);
            swap_ranges(begin_second + index, end_second, begin_second + index);
            
            fix_chromosome(selected[i]);
            fix_chromosome(selected[i+1]);
        }
    }

    /**
     * Fix the chromosome path (if necessary).
     * @param chr is the chromosome to fix.
     */
    void fix_chromosome(chromosome& chr){
        //check the occurences of each city
        vector<int> duplicate(chr.first.size(), 0); //ndexes represent cities
        bool need_fix = false;
        vector<int> missing;

        for(int i = 0; i < chr.first.size(); i++){
            duplicate[chr.first[i]] += 1;
            need_fix = need_fix || (duplicate[chr.first[i]] == 2);
            //if(duplicate[chr.first[i]] == 2)
                //need_fix = true;
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
     * Apply, with a certain probability, them mutation of the chromosome.
     * @param path is the chromosome to mutate.
     * @param mutation_rate is the probability that the mutation occurs.
     */
    void mutation(vector<chromosome>& selected, double& mutation_rate){
        utimer ut("MUTATION: ");    
        uniform_int_distribution<int> index_gen(1, graph.size() - 1); //generate the value to compare to choose population
        uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

        for(int i = 0; i < selected.size(); i++){
            
            if(prob_gen(generator) <= mutation_rate){
                int gene_1 = index_gen(generator);
                int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
                swap(selected[i].first[gene_1], selected[i].first[gene_2]);
            }
            fitness(selected[i]);
        }
    }

    void merge(vector<chromosome>& selected){
        utimer ut("MERGE: ");
        //use in this case swap_ranges after sorting population based on fitness (ascending order)
        sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });
        swap_ranges(selected.begin(), selected.end(), population.begin()); //substitute worse chromosome with children
    }
    
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

    Graph graph_init(vector<pair<double, double>>& cities){ 
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
    
    public:
    TSPSeq(int& population_size, int& start_vertex, string file){
        vector<pair<double, double>> cities = read_coord_file(file);
        graph = graph_init(cities);
        generation(population_size, start_vertex);
    }

      /**
     * Create the initial population.
     * @param population_size is the number of iterations.
     * @param crossover_rate is the probability that a crossover occurs.
     * @param mutation_rate is the probability that a mutation occurs
     * @param selection_size is the number of chromosome selected for crossover.
     */
    void genetic_algorithm(int& generations, double& mutation_rate, int& selection_size){
        vector<chromosome> selected(selection_size);
        evaluation(); 
        for(int i = 0; i < generations; i++){
            cout << "Generation: " << i << "\n" ;
            selection(selection_size, selected); 
            crossover(selected); 
            mutation(selected, mutation_rate); 
            merge(selected);
        }
        //printPopulation();
        chromosome max_fitness = (*max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.second < b.second; }));
        cout << "\n\nBEST PATH: " << (int)(1/max_fitness.second) << "\n";
    }
};

#endif