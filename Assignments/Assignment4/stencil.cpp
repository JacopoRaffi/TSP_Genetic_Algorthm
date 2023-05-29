#include <thread>
#include <iostream>
#include "../utimer.hpp"
#include <omp.h>
#include <utility>
#include <vector>

using namespace std;

void printM(vector<float> M[], int N){
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

    vector<float> *A = new vector<float>[N];
    vector<float> *B = new vector<float>[N];

    for(int i = 0; i < N; i++){
        A[i] = vector<float>(N);
        B[i] = vector<float>(N);
    }

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = i + j;
        }

    int it = 0, i, j;
    int run = 1;
    utimer ut("TIME: "); 

    while(run && (it < max_it)){
        run = 0;
        it++;
        //Four corners  
        A[0][0] = (B[0][0] + B[1][0] + B[0][1])/3.0;
        A[N-1][N-1] = (B[N-1][N-1] + B[N-2][N-1] + B[N-1][N-1])/3.0;
        A[N-1][0] = (B[N-1][0] + B[N-2][0] + B[N-1][1])/3.0;
        A[0][N-1] = (B[0][N-1] + B[1][N-1] + B[0][N-2])/3.0;

        //Borders of Matrix

        #pragma omp simd reduction(+:run)
        //North border
        for(int j = 1; j < N-1; j++){
            A[0][j] = (B[0][j-1] + B[0][j] + B[0][j+1] + B[1][j]) / 4.0;
            run = run + (abs(A[0][j] - B[0][j]) >= epsilon);
        }

        #pragma omp simd reduction(+:run)
        //South Border
        for(int j = 1; j < N-1; j++){
            A[N-1][j] = (B[N-1][j-1] + B[N-1][j] + B[N-1][j+1] + B[N-2][j]) / 4.0;
            run = run + (abs(A[N-1][j] - B[N-1][j]) >= epsilon);
        }

        #pragma omp simd reduction(+:run)
        //East Border
        for(int i = 1; i < N-1; i++){
            A[i][0] = (B[i][0] + B[i-1][0] + B[i+1][0] + B[i][1]) / 4.0;
            run = run + (abs(A[i][0] - B[i][0]) >= epsilon);
        }

        #pragma omp simd reduction(+:run)
        //West Border
        for(int i = 1; i < N-1; i++){
            A[i][N-1] = (B[i][N-1] + B[i-1][N-1] + B[i+1][N-1] + B[i][N-2]) / 4.0;
            run = run + (abs(A[i][0] - B[i][N-1]) >= epsilon);
        }

        //Center of Matrix
        for(int i = 1; i < N-1; i++){
            #pragma omp simd reduction(+:run)
            for(int j = 1; j < N-1; j++){
                A[i][j] = (B[i-1][j] + B[i][j-1] + B[i+1][j] + B[i][j+1] + B[i][j]) / 5.0;
                run = run + (abs(A[i][j] - B[i][j]) >= epsilon);
            }
        }
        swap(A, B);
    }

    printM(A,N);
}