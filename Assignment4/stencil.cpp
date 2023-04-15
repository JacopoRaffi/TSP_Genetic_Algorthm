#include <thread>
#include <iostream>
#include "../utimer.hpp"
#include <omp.h>
#include <utility>
#include <limits>

using namespace std;

float f(int i, int j, float** M, int N){
    float res = M[i][j];
    float count = 1 + (i!=0) + (j!=0) + (j!=(N-1)) + (i != (N-1));

    res += (i!=0)*M[i-1][j]; //Nord
    res += (i!=(N-1))*M[i+1][j]; //Sud
    res += (j!=(N-1))*M[i][j+1]; //Est
    res += (j!=0)*M[i][j-1]; //Ovest
    
    return (res/count);
}

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

    float **A = new float*[N];
    float **B = new float*[N];

    for(int i = 0; i < N; i++){
        A[i] = new float[N];
        B[i] = new float[N];
    }

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = A[i][j];
        }
    
    int it = 0, i, j;
    bool run = true;
    utimer ut("TIME: ");

    #pragma omp parallel num_threads(nw) shared(A,B,run) firstprivate(it,epsilon,max_it)
    {
        #pragma omp single
        {
            while(run && (it < max_it)){
                run = false;
                #pragma omp taskloop private(i,j) collapse(2)
                    for(i = 0; i < N; i++){
                        for(j = 0; j < N; j++){
                            A[i][j] = f(i, j, B, N);
                            run = abs(A[i][j] - B[i][j]) >= epsilon;
                        }
                    }
                swap(A, B);
                it++;
            }
        }
    } 
}