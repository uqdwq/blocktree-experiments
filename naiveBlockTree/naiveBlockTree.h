#include <iostream>
#include <unordered_map>
#include <vector>
#include "UBlock.h"
#include "LBlock.h"
#include "MBlock.h"
#include "HashString.h"
struct processBlock {
    int counter = 0;
    int start_S_;
    int length_;
    bool cons_ = false;
    processBlock* first_occ_left = nullptr;
    processBlock* first_occ_right = nullptr;
    int offset_;
    NBlock* parent_;
    NBlock* replacement_ = nullptr;
    explicit processBlock(int start, int length, NBlock* p)  : start_S_(start), length_(length), parent_(p) {};
    NBlock* first_occ_1() {
        return first_occ_left->replacement_;
    }
    NBlock* first_occ_2() {
        return first_occ_right->replacement_;
    }
    void append_parent(NBlock *b) {
        parent_->children.push_back(b);
    }
};

struct processPointer {
    processBlock* block_;
    int offset_;
    explicit processPointer(int offset, processBlock* p)  : block_(p), offset_(offset) {};
};

struct blockpair {
    processBlock* left_ = nullptr;
    processBlock* right_ = nullptr;
    void increase() {
        right_->counter++;
        left_->counter++;
    };
    explicit blockpair(processBlock* left, processBlock* right) : left_(left), right_(right) {};
};

class NBlockTree {
    public:
        int leaf_size_;
        int s_;
        int tau_;
        std::string* input_;
        NBlock* root_;
        void process_pointer();
        void process_root();
        void build_tree();
        void build_tree_fp();
        void build_tree_fp_inv();
        void build_level(std::vector<processBlock*> &level, std::unordered_map<std::string, std::vector<blockpair*>> &pairs,int &block_length, int &child_number);
        void hash_pairs(std::vector<processBlock*> &level, std::unordered_map<std::string, std::vector<blockpair*>> &pairs, int &c_pairs);
        void mark_blocks(std::string *input, std::unordered_map<std::string, std::vector<blockpair*>> &pairs, int &block_length);
        void build_level_fp(std::vector<processBlock*> &level, std::unordered_map<HashString, std::vector<blockpair*>> &pairs,int &block_length, int &child_number);
        void hash_pairs_fp(std::vector<processBlock*> &level, std::unordered_map<HashString, std::vector<blockpair*>> &pairs, int &c_pairs);
        void mark_blocks_fp(std::string *input, std::unordered_map<HashString, std::vector<blockpair*>> &pairs, int &block_length);
        
        char access(int i) {
            return root_->access(i);
        }
        NBlockTree(std::string* input, int s, int tau, int leaf_size): s_(s), tau_(tau), leaf_size_(leaf_size), input_(input) {
            if (input->size() <= leaf_size_) {
                root_ = new LBLock(input,0, input->size());
            } else {
                root_ = new MBLock(input->size());
            }
        }
        ~NBlockTree();

};
void print();
