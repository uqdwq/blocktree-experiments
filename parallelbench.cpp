#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <tlx/cmdline_parser.hpp>
#include <compressed/PCBlockTree.h>
#include <compressed/CBlockTree.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include "naiveBlockTree/naiveBlockTree.h"
#include <chrono>
#include <lpfarray.hpp>
#include <lpfconstruction/bv_blocktree_lpf_pruned.hpp>
#include <lpfconstruction/bv_blocktree_lpf_theory.hpp>
#include <lpfconstruction/bv_blocktree_lpf_heuristic.hpp>
#include <fppconstruction/bv_blocktree_fp_pruned.hpp>
#include <fppconstruction/bv_blocktree_fp_theory.hpp>
#include "helper.h"
#include "accessonlyhelper.h"
#include "parallelhelper.h"

int main(int argc, char* argv[]) {

    tlx::CmdlineParser cp;
    cp.set_author("Daniel Meyer <uqdwq@student.kit.edu>");
    cp.set_description("This program should run all the expriments for my thesis");
    // add a byte size argument which the user can enter like '1gi'
    uint64_t a_size = 0;
    cp.add_bytes('p', "size", a_size,
                 "Number of bytes to process.");
    std::string a_filename;
    cp.add_param_string("filename", a_filename,
                        "A filename to process");

    // process command line
    if (!cp.process(argc, argv))
        return -1; // some error occurred and help was always written to user.
    std::string input(a_size, ' ');
    std::ifstream t(a_filename);
    std::stringstream buffer;
    t.read(&input[0], a_size);
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::vector<int> access_queries_;
    std::vector<int> select_queries_;
    std::vector<uint8_t> select_c_;
    std::unordered_map<char, int64_t> hist;
    for (int i =  0; i < input.size(); i++) {
        hist[input[i]] = hist[input[i]] + 1;
    }
    std::uniform_int_distribution<uint64_t> dist(0, input.size() - 1);
    for (size_t i = 0; i < 1000000; ++i) {
        access_queries_.push_back(dist(mersenne_engine));
    }
    for (size_t i = 0; i < 1000000; ++i) {
        uint8_t x = 0;
        int xsum = 0;
        for (auto p: hist) {
            x = p.first;
            if (xsum + p.second >= access_queries_[i]) {
                break;
            }
            xsum += p.second;


        }
        // while (xsum + hist[x] < access_queries_[i]) {
        //     xsum += hist[x];
        //     x++;
        // }
        if (x != 0) {
            select_c_.push_back(x);
            select_queries_.push_back(access_queries_[i] - xsum);
        }

    }
    std::vector<uint8_t> vec(input.begin(), input.end());

    std::unordered_set<int> characters;
//    for (char c: input) {
//        characters.insert(c);
//        hist[c]++;
//    }

    //run_pasta_wavelet_tree(vec,a_filename,access_queries_, select_queries_, select_c_,1, 0,0);
    run_bench_lpf_heuristics(vec,a_filename,access_queries_, select_queries_, select_c_,1, 2,4);
    //run_pasta_wavelet_matrix(vec,a_filename,access_queries_, select_queries_, select_c_,1, 0,0);

    std::vector<int> taus = {2,4,8};
    std::vector<int> threads = {1,2,4,8,16,32,64,128};
    int l = 4;
    for (auto t: taus) {
        for (auto th: threads) {
            run_bench_lpf_pruned_z_parallel(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l,th);
        }
    }
    return 0;
}

