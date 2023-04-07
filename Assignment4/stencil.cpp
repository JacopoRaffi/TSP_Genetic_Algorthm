#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <unistd.h>
#include "../utimer.hpp"

using namespace std;

int main(int argc, char* argv[]){
    /*
    * command line args:
    * -N: dim-matrix,
    * -i: max_iterations,
    * -e: delta-error(epsilon),
    * -w: num_threads 
    */

    int16_t epsilon = -1;
    uint32_t N = 100;
    uint32_t i = 1000;
    uint32_t nw = 1; //thread workers
    char opt;
    while((opt = (char)getopt(argc, argv, "N:i:e:w:h")) != -1){
        switch(opt){
            case 'N':
                N = atoi(optarg);
                break;
            case 'i':
                i = atoi(optarg);
                break;
            case 'e':
                epsilon = atoi(optarg);
                break;
            case 'w':
                nw = atoi(optarg);
                break;
            case 'h':{
                cout << "Usage: " << argv[0] << " " << "[-N dim] [-i iterations] [-e error] [-w threads] \n";
                exit(EXIT_SUCCESS);   
            }break;  
            default: ;
        }
    }
    cout << "Matrix: " << N << " \n";
    cout << "Error: " << epsilon << " \n";
    cout << "Iterations: " << i << " \n";
    cout << "Workers: " << nw << " \n";

    {
        string str = "Threads " + to_string(nw) + ": ";
        utimer t(str);
    }

    return 0;
}

