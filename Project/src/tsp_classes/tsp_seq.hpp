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
            population[i].fitness = 0;

            std::iota(population[i].path.begin(), population[i].path.end(), 0);
            std::swap(population[i].path[0], population[i].path[start_vertex]);
            std::shuffle(population[i].path.begin() + 1, population[i].path.end(), gen);
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
        for(int i = 0; i < chr.path.size() - 1; i++){
            int row = chr.path[i];
            int col = chr.path[i+1];
            chr.fitness += graph[row][col];
        }
        chr.fitness += graph[chr.path.size() - 1][0]; //edge between last element and start city

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
        double total_fitness = 0.0;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
    }

    void crossover();
    
    /**
     * Apply, with a certain probability, them mutation of the chromosome.
     * @param path is the chromosome to mutate.
     * @param mutation_rate is the probability that the mutation occurs.
     */
    void mutation(vector<int>& path, double& mutation_rate){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> index_gen(1, path.size() - 1); //generate the value to compare to choose population
        std::uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

        //TODO: print path before and after mutation
        if(prob_gen(gen) <= mutation_rate){
            int parent_1 = index_gen(gen);
            int parent_2 = index_gen(gen);

            std::swap(path[parent_1], path[parent_2]);
        }
    }

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