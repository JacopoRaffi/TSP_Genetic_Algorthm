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

    //Just for test
    void printPath(chromosome chr){
        std::cout << "PATH: ";
        for(int i = 0; i < chr.path.size(); i++){
            std::cout << chr.path[i] << " ";
        }
        std::cout << "\n";
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
     * Select the chromosome for crossover (stochastic acceptance or Stochastic Unversal Sampling).
     * @param selection_number is the number of chromosome to be selected for crossover phase
     * @return the chromosome selected for crossover
     */
    vector<chromosome> selection(int& selection_number){
        utimer ut("SELECTION: ");
        double total_fitness = 0.0;
        std::uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population

        double max_fitness = (*std::max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.fitness < b.fitness; })).fitness;
    
    }   

    /**
     * Apply crossover.
     * @param selected is the vector of parents chromosome
     */
    vector<chromosome> crossover(vector<chromosome>& selected){
        utimer ut("CROSSOVER: ");
        std::uniform_int_distribution<int> index_gen(3, graph.size() - 2); //i want to avoid parts of one element
        int index = index_gen(generator);
        vector<chromosome> offspring; //future children
        //aply crossover (i, i+1)
        for(int i = 0; i < selected.size() - 1; i++){
            printPath(selected[i]);
            printPath(selected[i+1]);
            chromosome child_1, child_2;

            child_1.path = vector<int>(selected[i].path.size());
            child_2.path = vector<int>(selected[i].path.size());

            auto begin_first = selected[i].path.begin();
            auto begin_second = selected[i+1].path.begin();
            auto end_first = selected[i].path.end();
            auto end_second = selected[i+1].path.end();

            //first child
            std::copy(begin_first, begin_first + index, child_1.path.begin());
            std::copy(begin_second+(index+1), end_second, child_1.path.begin()+(index+1));

            //second child
            std::copy(begin_second, begin_second + index, child_2.path.begin());
            std::copy(begin_first+(index+1), end_first, child_2.path.begin()+(index+1));

            fix_chromosome(child_1);
            fix_chromosome(child_2);

            printPath(selected[i]);
            printPath(selected[i+1]);
        }

        return offspring;
    }

    /**
     * Fix the chromosome path (if necessary).
     * @param chr is the chromosome to fix.
     */
    void fix_chromosome(chromosome& chr){
        //check the occurences of each city
        vector<int> duplicate(chr.path.size(), 0); //ndexes represent cities
        bool need_fix = false;

        for(int i = 0; i < chr.path.size(); i++){
            duplicate[chr.path[i]] += 1;
            if(duplicate[chr.path[i]] == 2)
                need_fix = true;

        }

        if(need_fix){
            for(int i = 0; i < chr.path.size(); i++){
                if(duplicate[chr.path[i]] >= 2){
                    vector<int>::iterator it = std::find(chr.path.begin(), chr.path.end(), 0); //first city with 0 occurences
                    int city = std::distance(chr.path.begin(), it);
                    std::swap(chr.path[i], city);
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
        std::uniform_int_distribution<int> index_gen(1, selected.size() - 1); //generate the value to compare to choose population
        std::uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

        for(int i = 0; i < selected.size(); i++){
            if(prob_gen(generator) <= mutation_rate){
                printPath(selected[i]);
                int gene_1 = index_gen(generator);
                int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
                std::swap(selected[i].path[gene_1], selected[i].path[gene_2]);
                printPath(selected[i]);
            }
        }
    }

    void merge(vector<chromosome>& selected){
        utimer ut("MERGE: ");
        //use in this case swap_ranges after sorting population based on fitness
        std::sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.fitness < b.fitness; });
        std::swap_ranges(selected.begin(), selected.end(), population.begin() + selected.size()); //substitute worse chromosome with children
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
            evaluation(); done
            selection(); todo
            crossover(); done
            mutation(); done
            merge(); done
        }*/

        evaluation(); //final evaluation to take the best path
    }
};

#endif