#ifndef TSP_SEQ_H
#define TSP_SEQ_H

#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <numeric>
#include "../utimer.hpp"

using std::vector, std::pair;
using Graph = vector<vector<double>>; //Adjiacency Matrix

struct chromosome{
    vector<int> path;
    double fitness;
};

class TSPSeq{
    private:
    Graph& graph; 
    vector<chromosome> population;
    std::random_device rd;
    std::mt19937 generator{rd()};
    /**
     * Create the initial population.
     * @param population_size is the number of chromosome in the population.
     */
    void generation(int& population_size, int& start_vertex){
        utimer t("GENERATION: ");
        population = vector<chromosome>(population_size);

        for(int i = 0; i < population_size; i++){
            population[i].path = vector<int>(graph.size());
            population[i].path[0] = start_vertex;
            population[i].fitness = 0;

            std::iota(population[i].path.begin(), population[i].path.end(), 0);
            std::swap(population[i].path[0], population[i].path[start_vertex]);
            std::shuffle(population[i].path.begin() + 1, population[i].path.end(), generator);
        }
        //printPopulation();
    }

    //Just for test purposes.
    void printPopulation(){
        for(int i = 0; i < population.size(); i++){
            for(int j = 0; j < graph.size(); j++){
                std::cout << population[i].path[j] << " ";
            }
            std::cout << "FITNESS:" << population[i].fitness;
            std::cout << " Chromosome: " << i << "\n";
        }
    }
    
    /**
     * Compute the fitness of a given chromosome.
     * @param chr is a chromosome of the population.
     */
    void fitness(chromosome& chr){
        int row, col;
        bool greater;
        int size = chr.path.size();
        for(int i = 0; i < size - 1; i++){
            //for lower triangular matrix row must be the greater of the two value
            //avoid too many if-else
            greater = (chr.path[i] > chr.path[i+1]);
            row = greater*(chr.path[i]) + (!greater)*(chr.path[i+1]);
            col = greater*(chr.path[i+1]) + (!greater)*(chr.path[i]);

            chr.fitness += graph[row][col];
        }

        greater = (chr.path[0] > chr.path[size-1]);
        row = greater*(chr.path[0]) + (!greater)*(chr.path[size-1]);
        col = greater*(chr.path[size-1]) + (!greater)*(chr.path[0]);

        chr.fitness += graph[row][col]; //edge between last element and start city

        chr.fitness = 1 / chr.fitness; //lower the distance better the fitness
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
     * Select the chromosome for crossover (stochastic acceptance).
     * @param selection_number is the number of chromosome to be selected for crossover phase
     */
    void selection(int& selection_number){
        utimer ut("SELECTION: ");
        double total_fitness = 0.0;
        std::uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population

        double max_fitness = (*std::max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.fitness < b.fitness; })).fitness;
    
    }   

    void crossover(vector<chromosome> selected){
        utimer ut("CROSSOVER: ");
        std::uniform_int_distribution<int> index_gen(3, graph.size() - 2); //i want to avoid parts of one element
        int index = index_gen(generator);
        
        //aply crossover (i, i+1)
        for(int i = 0; i < selected.size() - 1; i++){
            //TODO: print before crossover and after crossover
            auto begin_first = selected[i].path.begin();
            auto begin_second = selected[i+1].path.begin();

            std::swap_ranges(begin_first, begin_first + index, begin_second);
        }
    }
    
    /**
     * Apply, with a certain probability, them mutation of the chromosome.
     * @param path is the chromosome to mutate.
     * @param mutation_rate is the probability that the mutation occurs.
     */
    void mutation(vector<int>& path, double& mutation_rate){
        utimer ut("MUTATION: ");
        std::uniform_int_distribution<int> index_gen(1, path.size() - 1); //generate the value to compare to choose population
        std::uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

        //TODO: print path before and after mutation
        if(prob_gen(generator) <= mutation_rate){
            int parent_1 = index_gen(generator);
            int parent_2 = index_gen(generator);

            std::swap(path[parent_1], path[parent_2]);
        }
    }

    void merge(){
        utimer ut("MERGE: ");
    }

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
    void genetic_algorithm(int& generations, double& mutation_rate, int& selection_size){
        evaluation();
        selection(selection_size);
        /*for(int i = 0; i < generations; i++){
            evaluation();
            selection();
            crossover();
            mutation();
            merge();
        }*/

        evaluation(); //final evaluation to take the best path
    }
};

#endif