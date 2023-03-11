#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <algorithm>

#include "../utimer.hpp"

using namespace std;

int f(int value){
    return (2 * value) + 1;
}

void job(vector<int>& v, vector<int>& w, int start, int end, uint32_t threshold, uint32_t k){
    if((end - start) <= threshold){ //base case
        for(int i = start; i < end; i++){
            w[i] = f(v[i]);
        }
    }
    else{ //recursive case
        int mid = (start + end) / 2;
        thread t1(job, ref(v), ref(w), start, mid, threshold);
        thread t2(job, ref(v), ref(w), mid, end, threshold);
        t1.join();
        t2.join();
    }

}

void printVector(vector<int> v){
    for(int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
    cout << "\n";
}

int main(int argc, char* argv[]){
    /*
    * Parameters: 
    * Length of Vector
    * Threshold m (base case of divide and conquer)   
    * k sub-groups   
    */
    if(argc < 4){
        cerr << "Too few Arguments" << endl;
        exit(EXIT_FAILURE);
    }

    uint32_t length = atoi(argv[1]);
    uint32_t threshold = atoi(argv[2]);
    uint32_t k = atoi(argv[3]); 
    vector<int> v(length);
    vector<int> w(v.size(), 0);

    for(int i = 0; i < length; i++){ //initialization of vector
        v[i] = i;
    }

    utimer to("usec:");
    if(length < threshold){ //sequential resolution
        transform(v.begin(), v.end(), w.begin(), f);
    }
    else{ //parallel with divide and conquer
        int mid = length / 2;
        thread t1(job, ref(v), ref(w), 0, mid, threshold, k);
        thread t2(job, ref(v), ref(w), mid, length, threshold, k);

        t1.join();
        t2.join();
    }
}