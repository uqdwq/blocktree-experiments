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
#include "malloc_count.h"
#include <lpfconstruction/bv_blocktree_lpf_pruned.hpp>
#include <lpfconstruction/bv_blocktree_lpf_theory.hpp>
#include <lpfconstruction/bv_blocktree_lpf_heuristic.hpp>
#include <fppconstruction/bv_blocktree_fp_pruned.hpp>
#include <fppconstruction/bv_blocktree_fp_theory.hpp>
//std::cout << id << "," << text_id << "," << vec.size() << "," << bt->print_space_usage() << "," <<ms_int1.count() << "," << malloc_count_peak() + small_size<< "," << (double)elapsed/access_queries_.size() << "," << ms_int2.count() << "," << bt->print_space_usage() <<","<< (double)elapsed2/select_queries_.size() << s<<"," << tau << "," << max_leaf_size <<std::endl;
void print_result(int id, std::string text_id, int vec_size, int small_size, int first_time, int peak_size, double access_avg, int snd_time, int big_size, double rank_avg, int s, int tau, int leaf_size, int64_t result) {
    std::cout << id << ",";
    std::cout << text_id << ",";
    std::cout << vec_size << ",";
    std::cout << small_size << ",";
    std::cout << first_time << ",";
    std::cout << peak_size << ",";
    std::cout << access_avg << ",";
    std::cout << snd_time << ",";
    std::cout << big_size << ",";
    std::cout << rank_avg << ",";
    std::cout << s << ",";
    std::cout << tau << ",";
    std::cout << leaf_size << ",";
    std::cout << result << std::endl;

}
int cbt_size_no_rank(PCBlockTree& cbt) {
    int result = 0;
    int bt_bv_size = sizeof(void*);
    for (sdsl::bit_vector* bv : cbt.bt_bv_) {
        bt_bv_size += sdsl::size_in_bytes(*bv);
    }
    int bt_bv_rank_size = sizeof(void*);
    for (sdsl::rank_support_v<1>* bvr : cbt.bt_bv_rank_) {
        bt_bv_rank_size += sdsl::size_in_bytes(*bvr);
    }
    int bt_offsets_size = sizeof(void*);
    for (sdsl::int_vector<>* offsets: cbt.bt_offsets_) {
        bt_offsets_size += sdsl::size_in_bytes(*offsets);
    }
    int leaf_string_size = sdsl::size_in_bytes(*cbt.leaf_string_);
    int alphabet_size = sdsl::size_in_bytes(*cbt.alphabet_);
    int mapping_size = sizeof(int) * 256;
    // int partial_total_size = mapping_size + alphabet_size + bt_bv_size+ bt_bv_rank_size+ bt_offsets_size + leaf_string_size;
    return leaf_string_size + bt_offsets_size + bt_bv_rank_size + bt_bv_size + alphabet_size + mapping_size;
}
int cbt_size_rank(PCBlockTree& cbt) {
    int result = 0;

    int bt_prefix_ranks_first_level_size = (cbt.bt_first_level_prefix_ranks_.size()+1) * sizeof(void*);;
    for (auto pair: cbt.bt_first_level_prefix_ranks_) {
        bt_prefix_ranks_first_level_size += sdsl::size_in_bytes(*(pair.second));
    }
    int bt_first_ranks_total_size = (cbt.bt_first_ranks_.size()+1) * sizeof(void*);
    for (auto pair: cbt.bt_first_ranks_) {
        int size = 0;
        for (sdsl::int_vector<>* ranks: pair.second) {
            size += sdsl::size_in_bytes(*ranks);
        }
        bt_first_ranks_total_size += size;
    }
    int bt_second_ranks_total_size = (cbt.bt_second_ranks_.size()+1) * sizeof(void*);
    for (auto pair: cbt.bt_second_ranks_) {
        int size = 0;
        for (sdsl::int_vector<>* ranks: pair.second) {
            size += sdsl::size_in_bytes(*ranks);
        }
        bt_second_ranks_total_size += size;
    }
    int bt_prefix_ranks_total_size = (cbt.bt_prefix_ranks_.size()+1) * sizeof(void*);
    for (auto pair: cbt.bt_first_level_prefix_ranks_) {
        bt_prefix_ranks_first_level_size += sdsl::size_in_bytes(*(pair.second));
    }
    result = bt_prefix_ranks_first_level_size + bt_first_ranks_total_size + bt_second_ranks_total_size + bt_prefix_ranks_total_size;
    return result + cbt_size_no_rank(cbt);
}
int run_bench_comp(std::string& input, std::string text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 0;
    std::vector<int64_t> hist(256);
    std::unordered_set<int> characters;
    for (char c: input) {
        characters.insert(c);
    }
    auto t01 = std::chrono::high_resolution_clock::now();
    PBlockTree* bt = new PBlockTree(input, tau, max_leaf_size);
    bt->process_back_pointers();
    bt->clean_unnecessary_expansions();
    bt->check();
    auto t02 = std::chrono::high_resolution_clock::now();
    for (int c: characters)
      bt->add_rank_select_support(c);
    PCBlockTree* cbt = new PCBlockTree(bt);
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += (uint8_t) cbt->access(query);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_queries_.size() - 1; i++) {
        result += cbt->rank((char)select_c_[i], select_queries_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    print_result(id, text_id, input.size(), cbt_size_no_rank(*cbt), ms_int1.count() , malloc_count_peak(), (double)elapsed/access_queries_.size(), ms_int2.count(),cbt_size_rank(*cbt) ,(double)elapsed2/select_queries_.size(), 1, tau, max_leaf_size, result);
    delete bt;
    delete cbt;
    return 0;
}
int run_bench_lpf_pruned(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 1;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += bt->access(query);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    auto start2 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->rank(select_c_[i], select_queries_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), malloc_count_peak() + small_size,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_theory(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 2;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += bt->access(query);
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    result = 0;
    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {

        result += bt->rank(select_c_[i], select_queries_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();

    std::cout << id << "," << text_id << "," << vec.size() << "," << bt->print_space_usage() << "," <<ms_int1.count() << "," << malloc_count_peak() + small_size<< "," << (double)elapsed/access_queries_.size() << "," << ms_int2.count() << "," << bt->print_space_usage() <<","<< (double)elapsed2/select_queries_.size() << s<<"," << tau << "," << max_leaf_size <<std::endl;
    delete bt;
    return 0;
}
int run_bench_lpf_heuristics(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 3;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_heuristic<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_heuristic<uint8_t, int32_t>(vec, tau, max_leaf_size);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += bt->access(query);
    }


    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->rank(select_c_[i], select_queries_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();

    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), malloc_count_peak() + small_size,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_pruned(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 4;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += bt->access(query);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->rank(select_c_[i], select_queries_[i]);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();

    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), malloc_count_peak() + small_size,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_theory(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<int>& select_c_, int s, int tau, int max_leaf_size) {
    malloc_count_reset_peak();
    int id = 5;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,s);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    int result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += bt->access(query);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    std::cout << id << "," << text_id << "," << vec.size() << "," << bt->print_space_usage() << "," <<ms_int.count() << "," << malloc_count_peak() + bt->print_space_usage()<< "," << (double)elapsed/access_queries_.size() << "," << s<<"," << tau << "," << max_leaf_size <<std::endl;
    delete bt;
    return 0;
}
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
    std::vector<int> select_c_;
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
        while (xsum + hist[x] < access_queries_[i]) {
            xsum += hist[x];
            x++;
        }
        select_c_.push_back(x);


        select_queries_.push_back(access_queries_[i] - xsum);
    }
    std::vector<uint8_t> vec(input.begin(), input.end());

    std::unordered_set<int> characters;
//    for (char c: input) {
//        characters.insert(c);
//        hist[c]++;
//    }
    std::vector<int> taus = {2};
    std::vector<int> leafs = {8};
    for (auto t: taus) {
        for (auto l: leafs) {
            run_bench_comp(input,a_filename,access_queries_,select_queries_, select_c_,1,t,l);
            run_bench_lpf_pruned(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
//            run_bench_lpf_theory(vec, a_filename,access_queries_,t,l);
            run_bench_lpf_heuristics(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
            run_bench_fp_pruned(vec,a_filename,access_queries_, select_queries_, select_c_,1, t,l);
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

