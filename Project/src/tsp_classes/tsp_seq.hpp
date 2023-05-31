#ifndef TSP_SEQ_H
#define TSP_SEQ_H

#include <vector>
#include "../utimer.hpp"

using std::vector;
using Graph = vector<vector<double>>; //Adjiacency Matrix


class TSPSeq{
    private:
    Graph& graph; 
    vector<vector<int>> population;
    int start_vertex = 0; //default value
    double mutation_rate;
    double crossover_rate;

    void generation();
    void fitness();
    void evalutation();
    void selection();
    void crossover();
    void mutation();
    void merge();

    public:
    TSPSeq(Graph& g) : graph{g} {}


};

#endif