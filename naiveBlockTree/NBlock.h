#pragma once
#include <string>
class NBlock {
    public:
        std::vector<NBlock*> children;
        int length_;
        virtual char access(int i) = 0;
        virtual std::string toString() = 0;
};