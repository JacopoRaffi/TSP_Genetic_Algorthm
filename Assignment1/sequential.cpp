#include <iostream>
#include <thread>
#include <vector>
#define N 20

int f(int value){
    return (2 * value) + 1; //casual function
}

void printVector(std::vector<int> vect){
    for(int i = 0; i < vect.size(); i++)
        std::cout << vect[i] << " ";
    
    std::cout << "\n";
}

int main(int argc, char * argv[]){
    std::srand(time(nullptr));
    std::vector<int> v(N); //start vector
    std::vector<int> w(v.size()); //w[i] = f(v[i])

    std::cout << "Old vector: " << std::endl;
    for(int i = 0; i < N; i++){
        v[i] = std::rand() % 10; //random values
    }
    printVector(v);

    std::cout << "New vector: " << std::endl;
    for(int i = 0; i < v.size(); i++){
        w[i] = f(v[i]);
    }
    printVector(w);
    
}