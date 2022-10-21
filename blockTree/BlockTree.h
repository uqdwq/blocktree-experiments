//
// Created by daniel on 26.06.22.
//

#ifndef BLOCKTREESEXPERIMENTS_BLOCKTREE_H
#define BLOCKTREESEXPERIMENTS_BLOCKTREE_H
#include <iostream>
#include "blocks/Block.h"
#include "blocks/MarkedBlock.h"
template<typename T>
struct processBlock {
    int counter = 0;
    uint64_t start_S_;
    uint64_t length_;
    bool cons_ = false;
    processBlock* first_occ_left = nullptr;
    processBlock* first_occ_right = nullptr;
    uint64_t offset_;
    Block<T>* parent_;
    MarkedBlock<T>* replacement_ = nullptr;
    explicit processBlock(uint64_t start, uint64_t length, Block<T>* p)  : start_S_(start), length_(length), parent_(p) {};
    MarkedBlock<T>* first_occ_1() {
        return first_occ_left->replacement_;
    }
    MarkedBlock<T>* first_occ_2() {
        return first_occ_right->replacement_;
    }
    void append_parent(Block<T> *b) {
        parent_->append(b);
    }
};
template<typename T>
class BlockTree {
    public:
        virtual T access(uint64_t i) = 0;
        virtual uint64_t rank(T c, uint64_t n) = 0;
        virtual uint64_t select(T c, uint64_t n) = 0;
        virtual void process_root() = 0;
        virtual void pruning() = 0;
};


#endif //BLOCKTREESEXPERIMENTS_BLOCKTREE_H
