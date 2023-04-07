#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <unistd.h>
#include "../utimer.hpp"

using namespace std;

//compile with -O3 -fopt-info-vec-all grep <file_name>.cpp 2>&1 | grep <file_name>.cpp for seeing vectorization
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
    int32_t seed = 123;
    char opt;
    while((opt = (char)getopt(argc, argv, "N:i:e:w:s:h")) != -1){
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
            case 's':
                seed = atoi(optarg);
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
    int A[N][N], B[N][N];
    
    //initializing matrix
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = rand();
            B[i][j] = A[i][j];
        }
    {
        string str = "Threads " + to_string(nw) + ": ";
        utimer t(str);
    }

    return 0;
}

