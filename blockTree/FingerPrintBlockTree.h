//
// Created by daniel on 26.06.22.
//

#ifndef BLOCKTREESEXPERIMENTS_FINGERPRINTBLOCKTREE_H
#define BLOCKTREESEXPERIMENTS_FINGERPRINTBLOCKTREE_H
#include "BlockTree.h"
#include "blocks/Block.h"
#include "blocks/LeafBlock.h"
#include "blocks/MarkedBlock.h"
#include "blocks/UnmarkedBlock.h"
#include "../../block_tree/includes/utils/MersenneHash.h"
#include "../../block_tree/includes/utils/MersenneRabinKarp.h"
#include <chrono>
#include <unordered_map>
template <class T>
class FingerPrintBlockTree : public BlockTree<T> {
    public:
        std::vector<T> const& text_;
        uint32_t tau_;
        uint32_t s_;
        uint64_t length_;
        uint64_t max_leaf_size_;
        Block<T>* root_;
        FingerPrintBlockTree(std::vector<T> const& text, uint32_t tau, uint32_t s, uint64_t length, uint64_t max_leaf_size) : text_(text) {
            tau_ = tau;
            s_ = s;
            length_ = length;
            max_leaf_size_ = max_leaf_size;
            if (length <= max_leaf_size_) {
                root_ = new LeafBlock<T>(text_,0, length_);
            } else {
                root_ = new MarkedBlock<T>(length_);
                process_root();
            }
        }
        ~FingerPrintBlockTree();
    T access(uint64_t i) {
        return 0;
    }
    uint64_t rank(T c, uint64_t n) {
        return 0;
    }
    uint64_t select(T c, uint64_t n){
        return 0;
    }


    void process_lvl(std::vector<processBlock<T>*>& level, uint64_t child_number, uint64_t block_length) {
        std::unordered_map<MersenneHash<T>, std::vector<std::pair<processBlock<T>*, processBlock<T>*>>> pairs;
        // hash pairs
        for(int i = 0; i < level.size(); i++) {
            if (level[i]->cons_) {
                MersenneRabinKarp<T> mrk = MersenneRabinKarp<T>(text_,256, level[i]->start_S_,2 * level[i]->length_);
                MersenneHash<T> mh = MersenneHash<T>(text_,mrk.hash_,mrk.init_,mrk.length_);
                pairs[mh].push_back(std::pair<processBlock<T>*, processBlock<T>*>(level[i],level[i+1]));
            }
        }
        MersenneRabinKarp<T> mrk = MersenneRabinKarp<T>(text_,256, 0,2 * level[0]->length_);
        for (int i = 0; i < text_.size(); i++) {
            MersenneHash<T> mh = MersenneHash<T>(text_,mrk.hash_,mrk.init_,mrk.length_);
            for (std::pair<processBlock<T>*, processBlock<T>*> blocks : pairs[mh]) {
                if (i != blocks.first->start_S_) {
                    blocks.first->counter++;
                    blocks.second->counter++;
                }
            }
            mrk.next();
        }
        std::unordered_map<MersenneHash<T>, std::vector<processBlock<T>*>> blocks;
        int num_unmarked_blocks = 0;
        for (int i = 0; i < level.size(); i++) {
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                std::cout << "Block " << i << "is unmarked" << std::endl;
                auto mrk2 = MersenneRabinKarp<T>(text_,256, level[i]->start_S_, level[i]->length_);
                MersenneHash<T> mh = MersenneHash<T>(text_,mrk2.hash_,mrk2.init_,mrk2.length_);
                blocks[mh].push_back(level[i]);
                num_unmarked_blocks++;
            }
        }
        for (int i = 0; i < level.size() && num_unmarked_blocks > 0; i++) {
            if ((level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1))) {
                continue;
            }
            int guard = 1;
            if (level[i]->cons_) {
                guard = level[i]->length_;
            }
            auto mrk2 = MersenneRabinKarp<T>(text_,256, level[i]->start_S_, level[i]->length_);
            for(int j = 0; j < guard; j++) {
                MersenneHash<T> mh = MersenneHash<T>(text_,mrk2.hash_,mrk2.init_,mrk2.length_);
                for (auto b : blocks[mh]) {
                    b->first_occ_left = level[i];
                    if (j > 0) {
                        b->first_occ_right = level[i + 1];
                    }
                    b->offset_ = j;
                    num_unmarked_blocks--;
                }
                blocks.erase(mh);
                mrk2.next();
            }
        }
//        std::vector<processBlock<T>*> next_lvl;
//        for(int i = 0; i < level.size(); i++) {
//            // unmarked blocks get a pointer to another block on the same lvl
//            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
//                UnmarkedBlock<T>* ub;
//                if (level[i]->offset_ == 0) {
//                    ub = new UnmarkedBlock<T>(level[i]->first_occ_1(), nullptr, 0, level[i]->parent_->length_ / child_number );
//                } else {
//                    ub = new UnmarkedBlock<T>(level[i]->first_occ_1(), level[i]->first_occ_2(), level[i]->offset_, level[i]->parent_->length_ / child_number);
//                }
//                level[i]->append_parent(ub);
//
//            } else {
//                MarkedBlock<T>* mb = new MarkedBlock<T>(level[i]->parent_->length_ / child_number);
//                level[i]->append_parent(mb);
//                level[i]->replacement_ = mb;
//                for (int j = 0; j < tau_; j++) {
//                    processBlock<T>* bp = new processBlock<T>(level[i]->start_S_ + j * (block_length/tau_),(block_length/tau_),mb);
//                    // if it isnt the last chidl of a marked block its always consecutive
//                    if (j < tau_ - 1) {
//                        bp->cons_ = true;
//                    } else if (j == tau_ - 1 && i < level.size() - 1) {
//                        // last child of marked block and not the last
//                        // we check if the next block is marked
//                        if (!(level[i + 1]->counter == 2 || (level[i + 1]->counter == 1 && i - 2 == level.size()))) {
//                            bp->cons_ = true;
//                        }
//                    }
//                    next_lvl.push_back(bp);
//                }
                for (auto b: level) {
                    delete b;
                }
//                level = next_lvl;
//            }

//        }
    }

    void process_root() {
        uint64_t block_size = text_.size()/s_;
        uint64_t childs = s_;
        // First we split the input into s blocks of size n/s
        std::vector<processBlock<T>*> level(s_);
        for(int i = 0; i < s_; i++) {
            level[i] = new processBlock<T>(i * block_size, block_size, root_);
            if(i < s_ - 1) {
                level[i]->cons_ = true;
            }
        }
        while (block_size > max_leaf_size_) {
            auto t_start = std::chrono::high_resolution_clock::now();
            process_lvl(level,childs, block_size);
            auto t_end = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
            std::cout << "Processed Level of size: " << block_size << " ";
            std::cout << "took " << dur.count() << "ms" << std::endl;
            block_size /= tau_;
            childs = tau_;
        }
        for (auto b : level) {
            delete b;
        }

    };
    void pruning() {

    };
};


#endif //BLOCKTREESEXPERIMENTS_FINGERPRINTBLOCKTREE_H
