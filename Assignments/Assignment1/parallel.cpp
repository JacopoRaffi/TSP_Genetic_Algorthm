#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <cmath>

using namespace std; 

int f(int value){
    return (2 * value) + 1;
}

void job(vector<int>& v, vector<int>& w, int start, int end){
    for(int i = start; i < end; i++){
        w[i] = f(v[i]);
    }
}

void printVector(vector<int> v){
    for(int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
    cout << "\n";
}

int main(int argc, char* argv[]){    
    int dim, numthreads;
    
    if(argc < 3){
        cerr << "Too few Arguments!" << "\n";
        exit(EXIT_FAILURE);
    }
    else{
        dim = atoi(argv[1]); //dim vector
        numthreads = atoi(argv[2]); //number of threads
    }

    vector<int> v(dim);
    vector<int> u(v.size(), 0);
    vector<thread> pool;
   
    for(int i = 0; i < dim; i++){
        v[i] = i;
    }
    printVector(v);

    int size;
    if(numthreads <= dim)
        size = ceil((float)dim / (float)numthreads); 
    else
        size = 1;

    int start = 0, end;
    for(int i = 0; i < min(dim, numthreads); i++){
        if((start + size) > (dim - 1)){
            end = dim;
        }
        else
            end = start + size;
            
        pool.push_back(thread(job, ref(v), ref(u), start, end));
        start += size;
    }

    for(int i = 0; i < pool.size(); i++){
        pool[i].join();
    }

    printVector(u);
    
}
