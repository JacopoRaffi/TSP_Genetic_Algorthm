#ifndef TSP_SEQ_H
#define TSP_SEQ_H

#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <unordered_map>
#include <numeric>
#include "../utimer.hpp"

using std::vector, std::pair;
using Graph = vector<vector<double>>; //Adjiacency Matrix

using chromosome = pair<vector<int>, double>;

//TODO: Test the done phases
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
            population[i].first = vector<int>(graph.size());
            population[i].first[0] = start_vertex;
            population[i].second = 0;

            std::iota(population[i].first.begin(), population[i].first.end(), 0);
            std::swap(population[i].first[0], population[i].first[start_vertex]);
            std::shuffle(population[i].first.begin() + 1, population[i].first.end(), generator);
        }
        //printPopulation();
    }

    //Just for test purposes.
    void printPopulation(){
        for(int i = 0; i < population.size(); i++){
            for(int j = 0; j < graph.size(); j++){
                std::cout << population[i].first[j] << " ";
            }
            std::cout << "FITNESS:" << population[i].second;
            std::cout << " Chromosome: " << i << "\n";
        }
    }

    //Just for test
    void printPath(chromosome& chr){
        std::cout << "PATH: ";
        for(int i = 0; i < chr.first.size(); i++){
            std::cout << chr.first[i] << " ";
        }
        std::cout << "FTNESS: " << chr.second << "\n";
    }
    
    /**
     * Compute the fitness of a given chromosome.
     * @param chr is a chromosome of the population.
     */
    void fitness(chromosome& chr){
        int row, col;
        bool greater;
        int size = chr.first.size();
        for(int i = 0; i < size - 1; i++){
            //for lower triangular matrix row must be the greater of the two value
            //avoid too many if-else
            greater = (chr.first[i] > chr.first[i+1]);
            row = greater*(chr.first[i]) + (!greater)*(chr.first[i+1]);
            col = greater*(chr.first[i+1]) + (!greater)*(chr.first[i]);

            chr.second += graph[row][col];
        }

        greater = (chr.first[0] > chr.first[size-1]);
        row = greater*(chr.first[0]) + (!greater)*(chr.first[size-1]);
        col = greater*(chr.first[size-1]) + (!greater)*(chr.first[0]);

        chr.second += graph[row][col]; //edge between last element and start city

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
        std::uniform_real_distribution<double> distribution(0.0, 1.0); //generate the value to compare to choose population
        std::uniform_int_distribution<int> index_gen(0, population.size()-1);
        double max_fitness = (*std::max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.second < b.second; })).second;

        vector<bool> founds(population.size(), false); //optimized by C++ to be like a bit vector
        int count = 0;

        while(count < selection_number){
            int index = index_gen(generator);
            if(!founds.at(index)){ //not already taken
                double probability = distribution(generator);
                if(probability <= (population.at(index).second / max_fitness)){
                    selected.at(count) = population.at(index);
                    founds[index] = true;
                    count++;
                }
            } 
        }
    }   

    /**
     * Apply crossover.
     * @param selected is the vector of parents chromosome
     */
    void crossover(vector<chromosome>& selected){
        utimer ut("CROSSOVER: ");
        std::uniform_int_distribution<int> index_gen(3, graph.size() - 2); //i want to avoid parts of one element
        //aply crossover (i, i+1)
        for(int i = 0; i < selected.size() - 1; i += 2){
            int index = index_gen(generator);
            auto begin_first = selected[i].first.begin();
            auto begin_second = selected[i+1].first.begin();

            auto end_first = selected[i].first.end();
            auto end_second = selected[i+1].first.end();

            //parents crossover
            std::swap_ranges(begin_first, begin_first + index, begin_second);
            std::swap_ranges(begin_second + index, end_second, begin_second + index);
            
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

        for(int i = 0; i < chr.first.size(); i++){
            duplicate[chr.first[i]] += 1;
            need_fix = need_fix || (duplicate[chr.first[i]] == 2);
            //if(duplicate[chr.first[i]] == 2)
                //need_fix = true;
        }
        
        if(need_fix){
            for(int i = 0; i < chr.first.size(); i++){
                if(duplicate[chr.first[i]] >= 2){
                    int city = std::distance(duplicate.begin(), std::find(duplicate.begin(), duplicate.end(), 0));
                    duplicate[city]++;
                    duplicate[chr.first[i]]--;
                    std::swap(chr.first[i], city);
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
        std::uniform_int_distribution<int> index_gen(1, graph.size() - 1); //generate the value to compare to choose population
        std::uniform_real_distribution<double> prob_gen(0.0, 1.0); //generate the value to compare to choose population

        for(int i = 0; i < selected.size(); i++){
            
            if(prob_gen(generator) <= mutation_rate){
                int gene_1 = index_gen(generator);
                int gene_2 = index_gen(generator);
                //Don't check if gene_1 == gene_2 because is quite unlikely being computed with uniform distribution (it would be (1/n)^2)
                std::swap(selected[i].first[gene_1], selected[i].first[gene_2]);
            }
        }
    }

    void merge(vector<chromosome>& selected){
        utimer ut("MERGE: ");
        //use in this case swap_ranges after sorting population based on fitness
        std::sort(population.begin(), population.end(), [](chromosome& a, chromosome& b) {return a.second < b.second; });
        std::swap_ranges(selected.begin(), selected.end(), population.begin()); //substitute worse chromosome with children
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
        vector<chromosome> selected(selection_size);
        for(int i = 0; i < generations; i++){
            std::cout << "Generation: " << i << "\n" ;
            evaluation(); 
            selection(selection_size, selected); 
            crossover(selected); 
            mutation(selected, mutation_rate); 
            merge(selected);
        }
        //printPopulation();
        chromosome max_fitness = (*std::max_element(population.begin(), population.end(), [](const chromosome& a, const chromosome& b) {return a.second < b.second; }));
        printPath(max_fitness);
        std::cout << "\n\nBEST PATH: " << (int)(1/max_fitness.second) << "\n";
    }
};

#endif