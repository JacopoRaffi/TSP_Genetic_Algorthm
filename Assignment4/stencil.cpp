#include <thread>
#include <iostream>
#include "../utimer.hpp"
#include <omp.h>
#include <utility>
#include <limits>

using namespace std;

void printM(float **M, int N){
    cout << "\n";
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++)
            cout << M[i][j] << " ";
        cout << "\n";
    }
}

int main(int argc, char* argv[]){
    if(argc < 4){
        cerr << "Pochi argomenti!\n";
        return -1;
    }

    int max_it = atoi(argv[1]);
    float epsilon = atof(argv[2]);
    int nw = atoi(argv[3]);
    int N = atoi(argv[4]);

    float **A = new float*[N], **B = new float*[N];

    for(int i = 0; i < N; i++){
        A[i] = new float[N];
        B[i] = new float[N];
    }

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = i + j;
        }

    int it = 0, i, j;
    bool run = true;
    utimer ut("TIME: "); 
    //printM(A,N);

    while(run && (it < max_it)){
        run = false;
        it++;
        /*Vectorizable code:
        * The idea is to split the matrix to create vectorizable code without going out of boundary
        * Split the matrix in 6 parts: 
        * 1) The four corners;
        * 2) Elem without North, South, East and West split them in 4 separated loops;
        * 3) Elem with all cardinal points in a single loop.
        */
        
    }
}