#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

//a simplified blocking queue (max length is "infinite")
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

template<typename T>
class blockingqueue{
public:
    blockingqueue(){
        m = new std::mutex();
        condition = new std::condition_variable();
    }

    T pop(){
        std::unique_lock<std::mutex> lc(*m);
        condition->wait(lc, [this]() {return !empty; });

        T value = q.front();
        q.pop();
        empty = q.empty();

        return value;
    }

    void push(T& value){
        std::unique_lock<std::mutex> lc(*m);
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