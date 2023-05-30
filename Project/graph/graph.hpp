#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <unordered_map>

using Key = uint64_t;
using Value = double;
using std::vector, std::unordered_map, std::pair;

class Graph{
    private:
    //Key of the map is the neighbour and the double double is the distance
        vector<unordered_map<Key, Value>> *graph;
        uint64_t num_nodes;
    public:
        Graph(uint64_t size){
            graph = new vector<unordered_map<Key, double>>(size);
            num_nodes = size;
        }

        //TODO: add vertex and their neighbourhood
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