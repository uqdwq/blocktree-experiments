//
// Created by daniel on 05.07.22.
//
#include "Block.h"
#include "MarkedBlock.h"
#ifndef BLOCKTREESEXPERIMENTS_UNMARKEDBLOCK_H
#define BLOCKTREESEXPERIMENTS_UNMARKEDBLOCK_H
template<typename T>
class UnmarkedBlock : public Block<T> {
public:
    MarkedBlock<T>* first_;
    MarkedBlock<T>* second_;
    uint64_t offset_;
    uint64_t length_;
    UnmarkedBlock(MarkedBlock<T>* first, MarkedBlock<T>* second, uint64_t offset, uint64_t length) : Block<T>() {
        first_ = first;
        second_ = second;
        offset_ = offset;
        length_ = length;
    };
    T access(uint64_t i) {
        if (offset_ + i < length_) {
            return first_->access(offset_ + i);
        } else {
            return second_->access((offset_ + i) % length_);
        }
    };
    void append(Block<T>* c) {};
};

#endif //BLOCKTREESEXPERIMENTS_UNMARKEDBLOCK_H
