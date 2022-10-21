//
// Created by daniel on 05.07.22.
//
#include <vector>
#include <iostream>
#ifndef BLOCKTREESEXPERIMENTS_BLOCK_H
#define BLOCKTREESEXPERIMENTS_BLOCK_H
template<typename T>
class Block {
public:
    uint64_t length_;
    virtual T access(uint64_t i) = 0;
    virtual void append(Block<T>* c) = 0;
};
#endif //BLOCKTREESEXPERIMENTS_BLOCK_H
