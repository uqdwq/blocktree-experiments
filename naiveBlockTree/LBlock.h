#include "NBlock.h"
#include "string"

class LBLock : public NBlock {
    public:
        std::string text_;
        LBLock(std::string* text, int start, int length) : NBlock() {
            length_ = length;
            text_ = text->substr(start, length);
        };
        char access(int i) {
            return text_[i];
        };
        std::string toString() {
            return "Leafblock with text: " + text_;
        }
};