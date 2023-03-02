#include <thread>
#include <iostream>
#include <vector>
#define N 20

//shared resources
std::vector<int> v(N);
std::vector<int> w(N, 0);


int f(int value){
    return (2 * value) + 1;
}

void job(int start, int end){
    for(int i = start; i < end; i++)
        w[i] = f(v[i]); 
}

void printVector(std::vector<int> vect){
    for(int i = 0; i < vect.size(); i++){
        std::cout << vect[i] << " ";
    }
    std::cout << "\n";
}

//for simplicity we assume that N is a multiple of numThreads
int main(int argc, char* argv[]){
    uint32_t numThreads = (argc == 1) ? 4 : atoi(argv[1]);

    if(numThreads > N){
        std::cerr << "Number of Threads must be <= " << N << std::endl;
        exit(EXIT_FAILURE);
    }
    std::srand(time(nullptr));

    for(int i = 0; i < N; i++){
        v[i] = std::rand() % 10;
    }
    printVector(v);
    //every thread take a portion of vector from i to i+size
    uint32_t size = N / numThreads;  

    std::vector<std::thread> threads;
    int start = 0, end = size;
    for(int i = 0; i < numThreads; i++){
        end = start + size;
        threads.push_back(std::thread(job, start, end));
        start += size;
    }

    for(int i = 0; i < numThreads; i++)    
        threads[i].join();

    printVector(w);
}