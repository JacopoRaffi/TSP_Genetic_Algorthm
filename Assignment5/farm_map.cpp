#include <iostream>
#include <vector>
#include <cmath>
#include "../utimer.hpp"
#include <ff/parallel_for.hpp>

using namespace ff;

int f(int elem){
    return pow((double)elem, 10);
}

int main(int argc, char* argv[]){
    /* Arguments: 
    * 1) Number of Workers;
    * 2) Spinwait;
    * 3) Size of Vector;
    *  */
    
    if(argc < 4){
        std::cout << "./parfor nw spin size\n";
        return 0;
    }

    int nw = atoi(argv[1]);
    //bool spin = static_cast<bool>(atoi(argv[2]));
    int size = atoi(argv[3]);
}