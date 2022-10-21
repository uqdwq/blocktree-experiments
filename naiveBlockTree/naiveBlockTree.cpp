#include "naiveBlockTree.h"
#include <iostream>
#include <string>
#include "HashString.h"
#include "RabinKarp.h"
#include <chrono>
void NBlockTree::process_pointer() {
    
 
}
void build_level_fp(std::vector<processBlock*> &level, std::unordered_map<HashString, std::vector<blockpair*>> &pairs,int &block_length, int &child_number) {

}
void hash_pairs_fp(std::vector<processBlock*> &level, std::unordered_map<HashString, std::vector<blockpair*>> &pairs, int &c_pairs) {
    
}
void mark_blocks_fp(std::string *input, std::unordered_map<HashString, std::vector<blockpair*>> &pairs, int &block_length) {

}
void NBlockTree::hash_pairs(std::vector<processBlock*> &level, std::unordered_map<std::string, std::vector<blockpair*>> &pairs, int &c_pairs) {
            for (int i = 0; i < level.size() - 1; i++) {
            
            if (level[i]->cons_) {
                blockpair* p = new blockpair(level[i], level[i + 1]);
                pairs[input_->substr(level[i]->start_S_, 2 * level[i]->length_)].push_back(p);
                c_pairs++;
            }

        }
}
void NBlockTree::mark_blocks(std::string *input, std::unordered_map<std::string, std::vector<blockpair*>> &pairs, int &block_length) {
    if (block_length * 2 <= input_->size()) {
        for (int i = 0; i < input_->size() - block_length * 2; i++) {
            std::string window = input_->substr(i, block_length * 2);
                for (auto blocks : pairs[window]) {
                    if (i != blocks->left_->start_S_) {
                        blocks->increase();
                    }
                }
                pairs.erase(window);
        }
        }
}
void NBlockTree::build_level(std::vector<processBlock*> &level, std::unordered_map<std::string, std::vector<blockpair*>> &pairs, int &block_length, int &child_number) {
    auto t1 = std::chrono::high_resolution_clock::now();
        int c_pairs = 0;
        auto t2 = std::chrono::high_resolution_clock::now();
        int amount_blocks = level.size();
        hash_pairs(level,pairs,c_pairs);
        auto t2e = std::chrono::high_resolution_clock::now();
        auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2e - t2);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " PAIRSCAN TIME: " << ms_int.count() << " PAIRSCAN C: " << c_pairs << std::endl;
        //std::cout << "nexxt " << std::endl;
        // mark blocks
        auto t3 = std::chrono::high_resolution_clock::now();
        mark_blocks(input_, pairs, block_length); 
        auto t3e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t3e - t3);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " MARK TIME: " << ms_int.count() << std::endl;
        // hash umarked blocks
        std::unordered_map<std::string, std::vector<processBlock*>> blocks;
        auto t4 = std::chrono::high_resolution_clock::now();
        int num_unmarked_blocks = 0;
        for (int i = 0; i < amount_blocks; i++) {
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                std::cout << "Block " << i << "is unmarked" << std::endl;
                std::string window = input_->substr(level[i]->start_S_, level[i]->length_);
                blocks[window].push_back(level[i]);
                num_unmarked_blocks++;
                //std::cout << window << " " << blocks[window].size() << std::endl;
            }
        }
        auto t4e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t4e - t4);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " HASH UNMARKED TIME: " << ms_int.count() << " UNMARKED BLOCKS: " << num_unmarked_blocks << std::endl;
        auto t5 = std::chrono::high_resolution_clock::now();
        int c_checked = 0;
        // check pointers for umarked blocks
        
        for (int i = 0; i < level.size() && num_unmarked_blocks > 0; i++) {
            if ((level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1))) {
                continue;
            }
            int guard = 1;
            if (level[i]->cons_) {
                guard = level[i]->length_;
            }
            for(int j = 0; j < guard; j++) {
                std::string window = input_->substr(level[i]->start_S_ + j, level[i]->length_);
                for (auto b : blocks[window]) {
                    b->first_occ_left = level[i];
                    if (j > 0) {
                        b->first_occ_right = level[i + 1];
                    }
                    b->offset_ = j;
                    num_unmarked_blocks--;
                    //std::cout << "Block "<< input_->substr(b->start_S_, b->length_)<<" starting at " << b->start_S_ << " points to Block" << i << " " << j << " " << input_->substr(level[i]->start_S_ + j, level[i]->length_) <<  std::endl; 
                    c_checked++;
                }
                blocks.erase(window);
                
            }
        }
        auto t5e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t5e - t5);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " FIND 1ST OCC TIME: " << ms_int.count()<< " CHECKED BLOCKS: " << c_checked << std::endl;
        auto t6 = std::chrono::high_resolution_clock::now();
        
        // create pointers
        std::vector<processBlock*> next_lvl;
        for(int i = 0; i < level.size(); i++) {
            // unmarked blocks get a pointer to another block on the same lvl
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                UBlock* ub;
                if (level[i]->offset_ == 0) {
                    ub = new UBlock(level[i]->first_occ_1(), nullptr, 0, level[i]->parent_->length_ / child_number );
                } else {
                    ub = new UBlock(level[i]->first_occ_1(), level[i]->first_occ_2(), level[i]->offset_, level[i]->parent_->length_ / child_number);
                }
                level[i]->append_parent(ub);
                
            } else {
                MBLock* mb = new MBLock(level[i]->parent_->length_ / child_number);
                level[i]->append_parent(mb);
                level[i]->replacement_ = mb;
                for (int j = 0; j < tau_; j++) {
                    processBlock* bp = new processBlock(level[i]->start_S_ + j * (block_length/tau_),(block_length/tau_),mb);
                    // if it isnt the last chidl of a marked block its always consecutive
                    if (j < tau_ - 1) {
                        bp->cons_ = true;
                    } else if (j == tau_ - 1 && i < level.size() - 1) {
                        // last child of marked block and not the last
                        // we check if the next block is marked
                        if (!(level[i + 1]->counter == 2 || (level[i + 1]->counter == 1 && i - 2 == level.size()))) {
                            bp->cons_ = true;
                        }
                    }
                    next_lvl.push_back(bp);
                }
            }
            
        }
        auto t6e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t6e - t6);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " CREATE POINTER TIME: " << ms_int.count() << std::endl;
        auto t1e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t1e - t1);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " TOTAL TIME: " << ms_int.count() << std::endl; 
        child_number = tau_;
        level = next_lvl;
}

void NBlockTree::build_tree() {

    int block_length = input_->size()/ s_;
    int amount_blocks = s_;
    int child_number = s_;
    
    // First we split the input into s blocks of size n/s
    // each block needs a counter set to 0 
    std::vector<processBlock*> level(s_);
    std::unordered_map<std::string, std::vector<blockpair*>> pairs;
    for(int i = 0; i < s_; i++) {
        
        level[i] = new processBlock(i * block_length, block_length, root_);
        if(i < s_ - 1) {
            level[i]->cons_ = true;
        }
    }
    
    while (block_length > leaf_size_) {
        build_level(level, pairs,block_length,child_number);  
        block_length = block_length/tau_;
        
    }
    // create leaf blocks
    for (auto b : level) {
        LBLock* lb = new LBLock(input_, b->start_S_, b->length_);
        b->append_parent(lb);
    }

}

void NBlockTree::build_tree_fp() {
    int N = 6700417; //Large prime
    int block_length = input_->size()/ s_;
    int amount_blocks = s_;
    int child_number = s_;
    
    // First we split the input into s blocks of size n/s
    // each block needs a counter set to 0 
    std::vector<processBlock*> level(s_);
    std::unordered_map<HashString, std::vector<blockpair*>> pairs;
    for(int i = 0; i < s_; i++) {
        
        level[i] = new processBlock(i * block_length, block_length, root_);
        if(i < s_ - 1) {
            level[i]->cons_ = true;
        }
    }
    while (block_length > leaf_size_) {
        std::cout << block_length << " " << level.size() << std::endl;
        amount_blocks = level.size();
        for (int i = 0; i < amount_blocks - 1; i++) {
            if (level[i]->cons_) {
                blockpair* p = new blockpair(level[i], level[i + 1]);
                RabinKarp rk(*input_, level[i]->start_S_,2 * level[i]->length_, N);
                HashString hS(rk.hash(), *input_, level[i]->start_S_, 2 * level[i]->length_ - 1);
                pairs[hS].push_back(p);
            }

        }
        //std::cout << "nexxt " << std::endl;
        // mark blocks
        if (block_length * 2 <= input_->size()) {
            RabinKarp rk(*input_,0 ,2 * block_length, N);
        for (int i = 0; i < input_->size() - block_length * 2; i++) {
            HashString hS(rk.hash(), *input_, i, i + 2 * block_length - 1);
            if(pairs.find(hS) != pairs.end()) {
                for (auto blocks : pairs[hS]) {
                    if (i != blocks->left_->start_S_) {
                        blocks->increase();
                    }
                }
                pairs.erase(hS);
                
            }
            rk.next();
        }
        }
        // hash umarked blocks
        std::unordered_map<std::string, std::vector<processBlock*>> blocks;
        int num_unmarked_blocks = 0;
        
        for (int i = 0; i < amount_blocks; i++) {
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                //std::cout << "Block " << i << "is unmarked" << std::endl;
                std::string window = input_->substr(level[i]->start_S_, level[i]->length_);
                blocks[window].push_back(level[i]);
                num_unmarked_blocks++;
                //std::cout << window << " " << blocks[window].size() << std::endl;
            }
        }
        std::cout << num_unmarked_blocks << std::endl;
        // check pointers for umarked blocks
        RabinKarp rk(*input_, level[0]->start_S_, block_length, N);
        for (int i = 0; i < level.size() && num_unmarked_blocks > 0; i++) {
            if ((level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1))) {
                //RabinKarp rk(*input_, level[i+1]->start_S_, block_length, N);
                continue;
            }
            //HashString hS(rk.hash(), *input_, level[i]->start_S_, level[i]->length_ - 1);
            int guard = 1;
            if (level[i]->cons_) {
                guard = level[i]->length_;
            }
            for(int j = 0; j < guard; j++) {
                std::string window = input_->substr(level[i]->start_S_ + j, level[i]->length_);
                for (auto b : blocks[window]) {
                    b->first_occ_left = level[i];
                    if (j > 0) {
                        b->first_occ_right = level[i + 1];
                    }
                    b->offset_ = j;
                    num_unmarked_blocks--;
                    //std::cout << "Block "<< input_->substr(b->start_S_, b->length_)<<" starting at " << b->start_S_ << " points to Block" << i << " " << j << " " << input_->substr(level[i]->start_S_ + j, level[i]->length_) <<  std::endl; 
                }
                blocks.erase(window);
                //rk.next();
                
            }
        }
    
        
        // create pointers
        std::vector<processBlock*> next_lvl;
        for(int i = 0; i < level.size(); i++) {
            // unmarked blocks get a pointer to another block on the same lvl
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                std::cout << "HI" << std::endl;
                UBlock* ub;
                if (level[i]->offset_ == 0) {
                    ub = new UBlock(level[i]->first_occ_1(), nullptr, 0, level[i]->parent_->length_ / child_number );
                } else {
                    ub = new UBlock(level[i]->first_occ_1(), level[i]->first_occ_2(), level[i]->offset_, level[i]->parent_->length_ / child_number);
                }
                level[i]->append_parent(ub);
                
            } else {
                MBLock* mb = new MBLock(level[i]->parent_->length_ / child_number);
                level[i]->append_parent(mb);
                level[i]->replacement_ = mb;
                for (int j = 0; j < tau_; j++) {
                    processBlock* bp = new processBlock(level[i]->start_S_ + j * (block_length/tau_),(block_length/tau_),mb);
                    // if it isnt the last chidl of a marked block its always consecutive
                    if (j < tau_ - 1) {
                        bp->cons_ = true;
                    } else if (j == tau_ - 1 && i < level.size() - 1) {
                        // last child of marked block and not the last
                        // we check if the next block is marked
                        if (!(level[i + 1]->counter == 2 || (level[i + 1]->counter == 1 && i - 2 == level.size()))) {
                            bp->cons_ = true;
                        }
                    }
                    next_lvl.push_back(bp);
                }
            }
            
        }
        child_number = tau_;
        level = next_lvl;
        block_length = block_length/tau_;
        
    }
    // create leaf blocks
    for (auto b : level) {
        LBLock* lb = new LBLock(input_, b->start_S_, b->length_);
        b->append_parent(lb);
    }
}
void NBlockTree::process_root() {
        
}
void print() {
    std::cout << "Hello World" << std::endl;
}
void NBlockTree::build_tree_fp_inv() {
    int N = 6700417; //Large prime
    int block_length = input_->size()/ s_;
    int amount_blocks = s_;
    int child_number = s_;
    
    // First we split the input into s blocks of size n/s
    // each block needs a counter set to 0 
    std::vector<processBlock*> level(s_);
    std::unordered_map<HashString, std::vector<blockpair*>> pairs;
    
    for(int i = 0; i < s_; i++) {
        
        level[i] = new processBlock(i * block_length, block_length, root_);
        if(i < s_ - 1) {
            level[i]->cons_ = true;
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    while (block_length > leaf_size_) {
        
        //std::cout << block_length << " " << level.size() << std::endl;
        amount_blocks = level.size();
        int c_pairs = 0;
        auto t2 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < amount_blocks - 1; i++) {
            if (level[i]->cons_) {
                blockpair* p = new blockpair(level[i], level[i + 1]);
                RabinKarp rk(*input_, level[i]->start_S_,2 * level[i]->length_, N);
                HashString hS(rk.hash(), *input_, level[i]->start_S_, 2 * level[i]->length_ - 1);
                pairs[hS].push_back(p);
                c_pairs++;
            }

        }
        auto t2e = std::chrono::high_resolution_clock::now();
        auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2e - t2);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " PAIRSCAN TIME: " << ms_int.count() << " PAIRSCAN C: " << c_pairs << std::endl;
        //std::cout << "nexxt " << std::endl;
        // mark blocks
        auto t3 = std::chrono::high_resolution_clock::now();
        if (block_length * 2 <= input_->size()) {
            
        for (int i = 0; i < input_->size() - block_length * 2; i++) {
            RabinKarp rk(*input_,0 ,2 * block_length, N);
            HashString hS(rk.hash(), *input_, i, 2 * block_length - 1);
            
            if(pairs.find(hS) != pairs.end()) {
                
                for (auto blocks : pairs[hS]) {
                    if (i != blocks->left_->start_S_) {
                        std::cout << hS.hash_ << std::endl;
                        blocks->increase();
                    }
                }
                pairs.erase(hS);
                
            }
            // rk.next();
        }
        }
        auto t3e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t3e - t3);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " MARK TIME: " << ms_int.count() << std::endl;
        // hash umarked blocks
        std::unordered_map<std::string, std::vector<processBlock*>> blocks;
        int num_unmarked_blocks = 0;
        auto t4 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < amount_blocks; i++) {
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                //std::cout << "Block " << i << "is unmarked" << std::endl;
                std::string window = input_->substr(level[i]->start_S_, level[i]->length_);
                blocks[window].push_back(level[i]);
                num_unmarked_blocks++;
                //std::cout << window << " " << blocks[window].size() << std::endl;
            }
        }
        auto t4e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t4e - t4);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " HASH UNMARKED TIME: " << ms_int.count() << " UNMARKED BLOCKS: " << num_unmarked_blocks << std::endl;
        auto t5 = std::chrono::high_resolution_clock::now();
        int c_checked = 0;
        // check pointers for umarked blocks
        RabinKarp rk(*input_, level[0]->start_S_, block_length, N);
        for (int i = 0; i < level.size() && num_unmarked_blocks > 0; i++) {
            if ((level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1))) {
                //RabinKarp rk(*input_, level[i+1]->start_S_, block_length, N);
                continue;
            }
            //HashString hS(rk.hash(), *input_, level[i]->start_S_, level[i]->length_ - 1);
            int guard = 1;
            if (level[i]->cons_) {
                guard = level[i]->length_;
            }
            for(int j = 0; j < guard; j++) {
                c_checked++;
                std::string window = input_->substr(level[i]->start_S_ + j, level[i]->length_);
                for (auto b : blocks[window]) {
                    b->first_occ_left = level[i];
                    if (j > 0) {
                        b->first_occ_right = level[i + 1];
                    }
                    b->offset_ = j;
                    num_unmarked_blocks--;
                    //std::cout << "Block "<< input_->substr(b->start_S_, b->length_)<<" starting at " << b->start_S_ << " points to Block" << i << " " << j << " " << input_->substr(level[i]->start_S_ + j, level[i]->length_) <<  std::endl; 
                }
                blocks.erase(window);
                //rk.next();
                
            }
        }
        auto t5e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t5e - t5);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " FIND 1ST OCC TIME: " << ms_int.count()<< " CHECKED BLOCKS: " << c_checked << std::endl;
        auto t6 = std::chrono::high_resolution_clock::now();
        // create pointers
        std::vector<processBlock*> next_lvl;
        for(int i = 0; i < level.size(); i++) {
            // unmarked blocks get a pointer to another block on the same lvl
            if (level[i]->counter == 2 || (level[i]->counter == 1 && i == level.size() -1)) {
                UBlock* ub;
                if (level[i]->offset_ == 0) {
                    ub = new UBlock(level[i]->first_occ_1(), nullptr, 0, level[i]->parent_->length_ / child_number );
                } else {
                    ub = new UBlock(level[i]->first_occ_1(), level[i]->first_occ_2(), level[i]->offset_, level[i]->parent_->length_ / child_number);
                }
                level[i]->append_parent(ub);
                
            } else {
                MBLock* mb = new MBLock(level[i]->parent_->length_ / child_number);
                level[i]->append_parent(mb);
                level[i]->replacement_ = mb;
                for (int j = 0; j < tau_; j++) {
                    processBlock* bp = new processBlock(level[i]->start_S_ + j * (block_length/tau_),(block_length/tau_),mb);
                    // if it isnt the last chidl of a marked block its always consecutive
                    if (j < tau_ - 1) {
                        bp->cons_ = true;
                    } else if (j == tau_ - 1 && i < level.size() - 1) {
                        // last child of marked block and not the last
                        // we check if the next block is marked
                        if (!(level[i + 1]->counter == 2 || (level[i + 1]->counter == 1 && i - 2 == level.size()))) {
                            bp->cons_ = true;
                        }
                    }
                    next_lvl.push_back(bp);
                }
            }
            
        }
        auto t6e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t6e - t6);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " CREATE POINTER TIME: " << ms_int.count() << std::endl;
        auto t1e = std::chrono::high_resolution_clock::now();
        ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t1e - t1);
        std::cout << "NAIVE LVL WITH SIZE: " << block_length << " TOTAL TIME: " << ms_int.count() << std::endl; 
        child_number = tau_;
        level = next_lvl;
        block_length = block_length/tau_;

    }
    // create leaf blocks
    for (auto b : level) {
        LBLock* lb = new LBLock(input_, b->start_S_, b->length_);
        b->append_parent(lb);
    }
}
