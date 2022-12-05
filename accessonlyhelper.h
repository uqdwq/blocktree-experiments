//
// Created by daniel on 05.12.22.
//
#include <pasta/wavelet_tree/wavelet_tree.hpp>
#include <pasta/utils/memory_monitor.hpp>
#ifndef BLOCKTREESEXPERIMENTS_ACCESSONLYHELPER_H
#define BLOCKTREESEXPERIMENTS_ACCESSONLYHELPER_H

int run_bench_comp_access(std::string& input, const std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = COMP;
    std::vector<int64_t> hist(256);
    std::unordered_set<int> characters;
    for (char c: input) {
        characters.insert(c);
    }

    auto t01 = std::chrono::high_resolution_clock::now();
    auto* bt = new PBlockTree(input, tau, max_leaf_size);
    bt->process_back_pointers();
    bt->clean_unnecessary_expansions();
    bt->check();
    int64_t size = 0;
    for (const auto& level: bt->levelwise_iterator()) {
        size += (int64_t)level.size() * 12;
    }
    auto t02 = std::chrono::high_resolution_clock::now();
    auto* abt = new PCBlockTree(bt);
    auto t0x = std::chrono::high_resolution_clock::now();

    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t0x - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int3 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t0x);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += abt->access(query);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();

    delete bt;
    auto start2 = std::chrono::high_resolution_clock::now();

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    print_result(id, text_id, (int)input.size(), cbt_size_no_rank(*abt), (int) ms_int1.count() , size, (double)elapsed/access_queries_.size(), ms_int2.count() + ms_int3.count(),cbt_size_rank(*abt) ,(double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size(), 1, tau, max_leaf_size, result);
    delete abt;
    return 0;
}

int run_bench_lpf_pruned_s_cut_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_CUT_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,s,true, CUT_FIRST_LEVEL, DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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


    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();


    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_s_cut_no_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_CUT_NO_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    auto*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,s,true, CUT_FIRST_LEVEL, NO_DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_s_no_cut_no_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_NO_CUT_NO_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,s,true, DONT_CUT_FIRST_LEVEL, NO_DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_z_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_Z_CUT_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,true, CUT_FIRST_LEVEL, DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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


    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_z_no_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,true, CUT_FIRST_LEVEL, NO_DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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


    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_theory_z_no_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_THEORY_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_theory<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_theory<uint8_t, int64_t>(vec, tau, max_leaf_size, NO_DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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


    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();



    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_theory_s_cut_no_dp_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_THEORY_S_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_theory<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_theory<uint8_t, int64_t>(vec, tau, max_leaf_size,s, NO_DP);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();

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


    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();


    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_heuristics_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_HEU;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_heuristic<uint8_t, int64_t>*  bt = new BV_BlockTree_lpf_heuristic<uint8_t, int64_t>(vec, tau, max_leaf_size);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();


    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
//static constexpr int FP_PRUNED_S_CUT = 9;
//static constexpr int FP_PRUNED_Z_CUT = 10;
int run_bench_fp_pruned_s_CUT_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_PRUNED_S_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,s,256,true, true);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();
    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_pruned_z_CUT_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_PRUNED_Z_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    lpf_array_stack(vec, lpf, lpf_ptr);
    calculate_lz_factor(s,lpf, lz);
    BV_BlockTree_fp_pruned<uint8_t, int64_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int64_t>(vec, tau, max_leaf_size,lz.size(),256,CUT_FIRST_LEVEL, true);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_theo_s_CUT_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_THEORY_S_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_theory<uint8_t, int64_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int64_t>(vec, tau, max_leaf_size,s,256,true);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_theo_z_CUT_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = FP_THEORY_Z_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    lpf_array_stack(vec, lpf, lpf_ptr);
    calculate_lz_factor(s,lpf, lz);
    BV_BlockTree_fp_theory<uint8_t, int64_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int64_t>(vec, tau, max_leaf_size,s,256,CUT_FIRST_LEVEL);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
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
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();
    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_pasta_wavelet_matrix_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = WAVELET_TREE;
    auto t01 = std::chrono::high_resolution_clock::now();
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();

    for (auto const& query : access_queries_) {
        result += wm[query];
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += wm.rank(select_queries_[i],select_c_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += wm.select(select_queries_[i],select_c_[i]);
    }
    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    print_result(id, text_id, vec.size(), 0, ms_int1.count(), 0,(double)elapsed/access_queries_.size(), ms_int2.count(), 0, (double)elapsed2/select_queries_.size(), (double)elapsed3/select_queries_.size() , s, PASTA_WAVELT_MATRIX, max_leaf_size, result );
    return 0;
}
int run_pasta_wavelet_tree_access(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = WAVELET_TREE;
    auto t01 = std::chrono::high_resolution_clock::now();
    auto wt = pasta::make_wt<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    int64_t result = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto const& query : access_queries_) {
        result += wt[query];
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += wt.rank(select_queries_[i], select_c_[i]);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += wt.select(select_queries_[i], select_c_[i]);
    }
    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
    print_result(id, text_id, vec.size(), 0, ms_int1.count(), 0,(double)elapsed/access_queries_.size(), ms_int2.count(), 0, (double)elapsed2/select_queries_.size(), (double)elapsed3/select_queries_.size() , s, PASTA_WAVELT_TREE, max_leaf_size, result );
    return 0;
}
//int run_bench_fp_theory(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
//
//    int id = 5;
//    auto t01 = std::chrono::high_resolution_clock::now();
//    BV_BlockTree_fp_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,s,256,true);
//    auto t02 = std::chrono::high_resolution_clock::now();
//    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
//    int result = 0;
//    auto start = std::chrono::high_resolution_clock::now();
//    for (auto const& query : access_queries_) {
//        result += bt->access(query);
//    }
//    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();
//    std::cout << id << "," << text_id << "," << vec.size() << "," << bt->print_space_usage() << "," <<ms_int.count() << "," << 0 + bt->print_space_usage()<< "," << (double)elapsed/access_queries_.size() << "," << s<<"," << tau << "," << max_leaf_size <<std::endl;
//    delete bt;
//    return 0;
//}
#endif //BLOCKTREESEXPERIMENTS_ACCESSONLYHELPER_H
