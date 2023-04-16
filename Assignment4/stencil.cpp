#include <thread>
#include <iostream>
#include "utimer.hpp"
#include <omp.h>
#include <utility>
#include <limits>

using namespace std;
#define N 1000

float croficihisset(int i, int j, float M[][N]){
    float res = 0.0;
    float count = 1.0;

    if(i != 0){
        count++;
        res += M[i-1][j];
    }

    if(i != (N-1)){
        count++;
        res += M[i+1][j];
    }

    if(j != 0){
        count++;
        res += M[i][j-1];
    }

    if(j != (N-1)){
        count++;
        res += M[i][j+1];
    }

    return res;
}

void printM(float M[][N]){
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

    float A[N][N];
    float B[N][N];

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = A[i][j];
        }
    
    int it = 0, i, j;
    bool run = true;
    float max_err = 0;
    utimer ut("croficihisset: ");

    #pragma omp parallel num_threads(nw) shared(A,B,max_err) firstprivate(it,epsilon) private(i,j)
    {
        #pragma omp single
        {
            while(run && (it < max_it)){
                run = false;
                #pragma omp taskloop
                    for(i = 0; i < N; i++){
                        for(j = 0; j < N; j++){
                            A[i][j] = croficihisset(i, j, B);
                            if(abs(A[i][j] - B[i][j]) >= epsilon){
                                run = true;
                            }
                        }
                    }
                swap(A, B);
                it++;
            }
        }
    } 
}