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


//    run_pasta_wavelet_matrix(vec,a_filename,access_queries_, select_queries_, select_c_,1, 0,0);
//    run_pasta_wavelet_tree(vec,a_filename,access_queries_, select_queries_, select_c_,1, 0,0);
    std::vector<int> taus = {8,4,2,};
    std::vector<int> leafs = {16,8,4};
    for (auto t: taus) {
        for (auto l: leafs) {
            run_bench_lpf_heuristics(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_pruned_s_cut_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_pruned_s_cut_no_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
//            run_bench_lpf_pruned_s_no_cut_no_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_pruned_z_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_pruned_z_no_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_theory_s_cut_no_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_lpf_theory_z_no_dp(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_comp_access(input,a_filename,access_queries_,select_queries_, select_c_,1,t,l);
            run_bench_fp_pruned_s_CUT(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_fp_pruned_z_CUT(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_fp_theo_s_CUT(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_fp_theo_z_CUT(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);






//            run_bench_lpf_theory(vec, a_filename,access_queries_,t,l);

//            run_bench_fp_theory(vec, a_filename,access_queries_,1,t,l);
        }
    }


//
//    auto t11 = std::chrono::high_resolution_clock::now();
//
//    auto t03 = std::chrono::high_resolution_clock::now();
//    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t11);
//    auto t04 = std::chrono::high_resolution_clock::now();
//    BV_BlockTree_fp_pruned<uint8_t, int32_t>*  bt3 = new BV_BlockTree_fp_pruned<uint8_t, int32_t>(vec, 2, 1,1);
//    auto t05 = std::chrono::high_resolution_clock::now();
//    auto ms_int3 = std::chrono::duration_cast<std::chrono::milliseconds>(t05 - t04);
//    std::cout << "time " << ms_int2.count() << std::endl;
//    std::cout << "time " << ms_int3.count() << std::endl;
//    auto first = malloc_count_current();
//    std::random_device rnd_device;
//    std::mt19937 mersenne_engine(rnd_device());
//    std::vector<int> access_queries_;
//    std::vector<int> select_queries_;
//    std::vector<uint8_t> select_c_;
//    std::uniform_int_distribution<uint64_t> dist(0, input.size());
//    for (size_t i = 0; i < 1000000; ++i) {
//        access_queries_.push_back(dist(mersenne_engine));
//    }
//    for (size_t i = 0; i < 1000000; ++i) {
//        uint8_t x = 0;
//        int xsum = 0;
//        while (xsum + hist[x] < access_queries_[i]) {
//            xsum += hist[x];
//            x++;
//        }
//        select_c_.push_back(x);
//        select_queries_.push_back(access_queries_[i] - xsum);
//    }
//    std::cout << "Starting Queries" << "\n";
//    size_t result = 0;
//    auto start = std::chrono::high_resolution_clock::now();
//    for (auto const& query : access_queries_) {
//        result += cbt->access(query);
//    }
//    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
//    auto start2 = std::chrono::high_resolution_clock::now();
//    for (int i = 0; i < select_c_.size(); i++) {
//        result += cbt->rank(select_c_[i], access_queries_[i]);
//    }
//    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
//    auto start3 = std::chrono::high_resolution_clock::now();
//    for (int i = 0; i < select_c_.size(); i++) {
//        result += cbt->select(select_c_[i], select_queries_[i]);
//    }
//    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
//
//    std::cout << "Ending Queries " << result << "\n";
//    double nanosec_per_access = elapsed / static_cast<double>(access_queries_.size());
//    double nanosec_per_rank = elapsed2 / static_cast<double>(access_queries_.size());
//    double nanosec_per_select = elapsed3 / static_cast<double>(access_queries_.size());
//    std::cout << "#access queries " << access_queries_.size() << std::endl;
//    std::cout << nanosec_per_access << " ns per query\n";
//    std::cout << "#rank queries " << access_queries_.size() << std::endl;
//    std::cout << nanosec_per_rank << " ns per query\n";
//    std::cout << "#select queries " << access_queries_.size() << std::endl;
//    std::cout << nanosec_per_select << " ns per query\n";
//    delete bt3;
    return 0;
}

