#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <unistd.h>
#include <utility>
#include "../utimer.hpp"

using namespace std;

#define N 10

inline float f(int i, int j, float M[][N]){
    float res = M[i][j] + (i!=0)*M[i-1][j] + (j!=0)*M[i][j-1] + (j!=(N-1))*M[i][j+1] + (i!=(N-1))*M[i+1][j];
    float count = (i!=0) + (j!=0) + (i!=(N-1)) + (j!=(N-1)) + 1;

    return (res/count);
}

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
    uint32_t max_it = 1000;
    uint32_t nw = 1; //thread workers
    int32_t seed = 123;
    char opt;
    while((opt = (char)getopt(argc, argv, "i:e:w:s:h")) != -1){
        switch(opt){
            case 'i':
                max_it = atoi(optarg);
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
                cout << "Usage: " << argv[0] << " " << "[-i iterations] [-e error] [-w threads] \n";
                exit(EXIT_SUCCESS);   
            }break;  
            default: ;
        }
    }
    cout << "Matrix: " << N << " \n";
    cout << "Error: " << epsilon << " \n";
    cout << "Iterations: " << max_it << " \n";
    cout << "Workers: " << nw << " \n";
    float A[N][N], B[N][N];
    srand(seed);
    
    //initializing matrix
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = rand();
            B[i][j] = A[i][j];
        }
    int it = 0;
    bool run = true;
    string str = "Threads " + to_string(nw) + ": ";
    {
        utimer t(str);
        while(run && (it <= max_it)){
            run = false;

            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    A[i][j] = f(i, j, B);
                    
                    if(abs(A[i][j] - B[i][j]) > epsilon)
                        run = true;
                }
            }
            swap(A, B);
            it++;
        }
    
    }
    return 0;
}

