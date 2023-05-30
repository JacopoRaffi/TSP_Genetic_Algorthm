#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <unordered_map>
#include <random>

using Key = uint64_t;
using std::vector, std::unordered_map, std::pair;

class Graph{
    private:
        vector<unordered_map<Key, double>> *graph; //Key is the neighbour and the double value is the wieght of the edge
        uint64_t num_nodes; //size of the graph
    public:
        /* generate a random graph (the goal of the project if the Genetic Algorithm phase)
        * @param size: number of vertexes  
        */
        Graph(uint64_t size){
            graph = new vector<unordered_map<Key, double>>(size);
            num_nodes = size;
        }

        void add_vertex(Key vertex, vector<pair<Key, double>> neighbourhood){
            for(pair<Key, double> p : neighbourhood){
                (*graph)[vertex][p.first] = p.second; //add neghbour and the weight of the edge
            }
        }
        
        //add a weighted edge between vertex1 and vertex2
        inline void add_edge(Key vertex1, Key vertex2, double weight){
            (*graph)[vertex1][vertex2] = weight;
        }
        
        double get_edge_weight(Key vertex1, Key vertex2){
            if(vertex1 < num_nodes && vertex2 < num_nodes){
                return (*graph)[vertex1][vertex2];
            }
            //negative value if vertex1 or vertex2 don't exist
            return -1.0;
        }
};

#endif