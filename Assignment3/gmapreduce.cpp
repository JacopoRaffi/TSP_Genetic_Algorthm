#include <thread>
#include <iostream>

/* TODO:
 * mappers take a portion of input vector;
 * mappers produce pair <word, 1>;
 * mappers will fuse pair with same key before sending to reducers;
 * reducers have queues (one for reducer);
 * for hash functions use something like [](string str) {string_to_num % numOfReducers};
 * use a vector of queues (vector<queue>), remember to synchronize queues;
 * reducers are aync, they return a vector of pair (vector<word, int>);
 * thread main wil join async and fuse the vectors into a single one;
*/


int main(int argc, char* arv[]){
    
}
