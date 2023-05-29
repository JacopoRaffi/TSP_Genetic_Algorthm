#include <iostream>
#include <vector>
#include <cmath>
#include "../utimer.hpp"
#include <ff/ff.hpp>

using namespace ff;

int * worker_job(int *task, ff_node *node){
    int temp = *task;
    *task = pow(temp, 2);

    return task;
}

class emitter : public ff_node_t<int>{
    private:
        std::vector<int> *v;
        int nw;
        int size;

    public:
        emitter(std::vector<int> *v, int nw, int size){
            this->nw = nw;
            this->v = v;
            this->size = size;
        }

        int* svc(int *t){
            for(int i = 0; i < size; i++){
                ff_send_out(&(*v)[i]);
            }
            return EOS;
        }
};

//TODO: add collector

void printVect(std::vector<int> v){
    for(int i = 0; i < v.size(); i++){
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]){
    /* Arguments: 
    * 1) Number of Workers;
    * 2) Size of Vector; 
    */
    
    if(argc < 3){
        std::cout << "./parfor nw spin size\n";
        return 0;
    }

    int nw = atoi(argv[1]);
    int size = atoi(argv[2]);

    std::vector<int> v(size);

    for(int i = 0; i < size; i++){
        v[i] = i;
    }

    ff_Farm<int> map_farm(worker_job, nw);
    emitter e(&v, nw, size);
    map_farm.add_emitter(e);

    utimer t("TIME: ");
    map_farm.run_and_wait_end();

    printVect(v);
}