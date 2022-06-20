#include "NBlock.h"

class UBlock : public NBlock {
    public:
        NBlock* first_;
        NBlock* second_;
        int offset_;
        UBlock(NBlock* first, NBlock* second, int offset, int length) : NBlock() {
            first_ = first;
            second_ = second;
            offset_ = offset;
            length_ = length;
        };
        std::string toString() {
            return "Unmarked Block";
        }
        char access(int i) {
            if (offset_ + i < length_) {
                return first_->access(offset_ + i);
            } else {
                return second_->access((offset_ + i) % length_);
            }
        }
};