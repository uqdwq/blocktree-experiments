#include "NBlock.h"
#include <vector>

class MBLock : public NBlock {
    public:
        MBLock(int length) : NBlock() {
            length_ = length;
        };
        char access(int i) {
            int child_size = length_/children.size();
            int child = i / child_size;
            return children[child]->access(i % child_size);
        };
        std::string toString() {
            return "Marked Block";
        }
        
};