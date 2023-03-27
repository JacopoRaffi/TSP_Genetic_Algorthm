#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

//a simplified blocking queue (max length is "infinite")
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <optional>

template<typename T>
class blockingqueue{
public:
    blockingqueue(){
        m = new std::mutex();
        condition = new std::condition_variable();
    }

    std::optional<T> pop(){
        std::unique_lock<std::mutex> lc(*m);
        condition->wait(lc, [&]() {return (!empty); });
        std::optional<T> value = q.front();
        q.pop();
        empty = q.empty();
        return value;
    }

    void push(std::optional<T> value){
        std::unique_lock<std::mutex> lc(*m);
        q.push(value);
        empty = false;
        condition->notify_all();
    }

private:
    std::queue<std::optional<T>> q;
    std::mutex *m;
    std::condition_variable *condition;
    bool empty = true;
};

#endif