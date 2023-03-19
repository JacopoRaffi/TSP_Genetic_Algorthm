#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

//a simplified blocking queue (max length is "infinite")
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class blockingqueue{
public:
    blockingqueue(){
        m = new std::mutex();
        condition = new std::condition_variable();
    }

    T pop(){
        std::lock_guard<std::mutex>(*m);
        condition->wait(*m, [this]() {return empty; });

        T value = q.front();
        q.pop();
        empty = q.empty();

        return value;
    }

    void push(T& value){
        std::lock_guard<std::mutex>(*m);
        q.push(value);
        empty = false;
        condition->notify_all();
    }

private:
    std::queue<T> q;
    std::mutex *m;
    std::condition_variable *condition;
    bool empty = true;
};

#endif