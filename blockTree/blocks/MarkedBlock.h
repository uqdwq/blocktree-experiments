//
// Created by daniel on 05.07.22.
//
#include "Block.h"
#ifndef BLOCKTREESEXPERIMENTS_MARKEDBLOCK_H
#define BLOCKTREESEXPERIMENTS_MARKEDBLOCK_H
template<typename T>
class MarkedBlock : public Block<T> {
public:
    std::vector<Block<T>*> children;
    uint64_t length_;
    MarkedBlock(uint64_t length) : Block<T>() {
        length_ = length;
    };
    T access(uint64_t i) {
        uint64_t child_size = length_/children.size();
        uint64_t child = i / child_size;
        return children[child]->access(i % child_size);
    };
    void append(Block<T>* c) {children.push_back(c);};
};
#endif //BLOCKTREESEXPERIMENTS_MARKEDBLOCK_H
