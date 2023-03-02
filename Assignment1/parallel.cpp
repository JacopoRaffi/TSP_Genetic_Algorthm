#include <thread>
#include <iostream>
#include <vector>
#define N 20

int f(int value){
    return (2 * value) + 1;
}

void job(std::vector<int> v, std::vector<int> w){

}

void printVector(std::vector<int> vect){
    for(int i = 0; i < vect.size(); i++){
        std::cout << vect[i] << " ";
    }
    std::cout << "\n";
}


int main(int argc, char* argv[]){
    uint32_t numThreads = (argc == 1) ? 4 : atoi(argv[1]);

    if(numThreads > N){
        std::cerr << "Number of Threads must be <= " << N << std::endl;
        exit(EXIT_FAILURE);
    }
    std::srand(time(nullptr));

    std::vector<int> v(N);
    for(int i = 0; i < N; i++){
        v[i] = std::rand() % 10;
    }

    std::vector<std::thread> threads(numThreads);
    for(int i = 0; i < numThreads; i++){
        threads.push_back(std::thread(job, v));
    }

}