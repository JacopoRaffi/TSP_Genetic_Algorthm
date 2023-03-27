#include <thread>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <future>
#include <cmath>
#include <algorithm>
#include <functional>
#include <atomic>
#include "blockingqueue.hpp"

using namespace std;

pair<string, int> map_f(string word){
    return make_pair(word, 1);
}

int hash_to_reducer(string word, int dim){
    hash<string> hash_fun;
    return hash_fun(word) % dim;        
}

//reducer made with async (parameter is its blockingqueue)
vector<pair<string, int>> reducer(blockingqueue<pair<string, int>>& queue_i){
    vector<pair<string, int>> v;
    map<string, int> mapRed;

    while(true){
        optional<pair<string, int>> op = queue_i.pop();
        pair<string, int> p;
        if(op.has_value())
            p = op.value();
        else
            break;
        if(mapRed.contains(p.first)){
            mapRed[p.first] += p.second;
        }else{
            mapRed[p.first] = p.second;
        }
    }
    for(pair<string, int> pa : mapRed){
        v.push_back(pa);
    }
    return v;
}

void mapper(int start, int end, vector<string>& words, vector<blockingqueue<pair<string, int>>>& reducer_queue){
    //first transform words in a vector of pair
    vector<pair<string, int>> out_map(end - start);
    transform(words.begin()+start, words.begin()+end, out_map.begin(), map_f);
    map<string, int> pairs;

    //mini-reduce so i avoid to calculate the same hash for equal Keys
    for(int i = 0; i < out_map.size(); i++){
        string word = out_map[i].first;
        if(pairs.contains(word)){
            pairs[word] += 1;
        }
        else{
            pairs[word] = 1;
        }
    }
    //send pair <word, value> to reducer
    for(pair<string, int> p : pairs){
        int index = hash_to_reducer(p.first, reducer_queue.size());
        reducer_queue[index].push(p);
    }
}

int main(int argc, char* argv[]){
    if(argc < 3)
        cout << "At least 2 arguments!\n";

    uint8_t mappers = atoi(argv[1]);
    uint8_t reducers = atoi(argv[2]);
    vector<string> words;

    for(int i = 3; i < argc; i++){ //words starts from third argument
        words.push_back(argv[i]); 
    }

    vector<thread> map_pool; 
    vector<blockingqueue<pair<string, int>>> reducers_queues;
    for(int i = 0; i < reducers; i++){
        blockingqueue<pair<string, int>> queue;
        reducers_queues.push_back(queue); 
    }
    int size;
    uint32_t dim = words.size();
    if(mappers <= dim)
        size = ceil((float)dim / (float)mappers); 
    else
        size = 1;

    int start = 0, end;
    for(int i = 0; i < min((int)dim, (int)mappers); i++){
        if((start + size) > (dim - 1)){
            end = dim;
        }
        else
            end = start + size;
            
        map_pool.push_back(thread(mapper, start, end, ref(words), ref(reducers_queues)));
        start += size;
    }
    vector<future<vector<pair<string, int>>>> reduce_pool;
    for(int i = 0; i < reducers; i++){
        reduce_pool.push_back(async(std::launch::async, reducer, ref(reducers_queues[i])));
    }

    for(int i = 0; i < mappers; i++)
        map_pool[i].join();  
    
    for(int i = 0; i < reducers; i++)
        reducers_queues[i].push({});

    vector<pair<string, int>> output; //final output of the map-reduce
    for(int i = 0; i < reducers; i++){
        //merge of results of asyncs
        vector<pair<string, int>> res = reduce_pool[i].get();
        output.insert(output.end(), res.begin(), res.end());
    }

    for(pair<string, int> p : output){
        cout << p.first << "-" << p.second << "\n";
    }
}
