#ifndef TSP_SEQ_H
#define TSP_SEQ_H

#include <vector>
#include <algorithm>
#include <random>
#include <numeric>
#include "../utimer.hpp"

using std::vector;
using Graph = vector<vector<double>>; //Adjiacency Matrix

struct chromosome{
    vector<int> path;
    long fitness;
};

class TSPSeq{
    private:
    Graph& graph; 
    vector<chromosome> population;
    /**
     * Create the initial population.
     * @param population_size is the number of chromosome in the population.
     */
    void generation(int& population_size, int& start_vertex){
        utimer t("GENERATION: ");
        population = vector<chromosome>(population_size);
        std::random_device rd;
        std::mt19937 gen(rd());

        for(int i = 0; i < population_size; i++){
            population[i].path = vector<int>(graph.size());
            population[i].path[0] = start_vertex;
            
            shuffle(population[i].path.begin() + 1, population[i].path.end(), gen);
        }
    }

    void fitness();
    void selection();
    void crossover();
    void mutation();
    void merge();

    public:
    TSPSeq(Graph& g, int& population_size, int& start_vertex) : graph{g} {
        generation(population_size, start_vertex);
    }

      /**
     * Create the initial population.
     * @param population_size is the number of iterations.
     * @param crossover_rate is the probability that a crossover occurs.
     * @param mutation_rate is the probability that a mutation occurs
     * @param selection_size is the number of chromosome selected for crossover.
     */
    void genetic_algorithm(int& generations, double& crossover_rate, double& mutation_rate, int& selection_size){
        for(int i = 0; i < generations; i++){
            fitness();
            selection();
            crossover();
            mutation();
            merge();
        }
    }
};

#endif