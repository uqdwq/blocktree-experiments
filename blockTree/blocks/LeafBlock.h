//
// Created by daniel on 05.07.22.
//
#include "Block.h"
#ifndef BLOCKTREESEXPERIMENTS_LEAFBLOCK_H
#define BLOCKTREESEXPERIMENTS_LEAFBLOCK_H
template<typename T>
class LeafBlock : public Block<T> {
public:
    std::vector<T> text_;
    uint64_t length_;
    LeafBlock(std::vector<T> const& text, uint64_t start, uint64_t length) : Block<T>() {
        length_ = length;
        text_ = {text.begin() + start, text.begin() + start + length - 1};
    };
    T access(uint64_t i) {
        return text_[i];
    };
    void append(Block<T>* c) {};
};
#endif //BLOCKTREESEXPERIMENTS_LEAFBLOCK_H
