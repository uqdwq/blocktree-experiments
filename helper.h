//
// Created by daniel on 27.11.22.
//
#include <pasta/wavelet_tree/wavelet_tree.hpp>
#include <pasta/utils/memory_monitor.hpp>
#ifndef BLOCKTREESEXPERIMENTS_HELPER_H
#define BLOCKTREESEXPERIMENTS_HELPER_H
static constexpr bool DONT_CUT_FIRST_LEVEL = false;
static constexpr bool CUT_FIRST_LEVEL = true;
static constexpr bool SIMPLE_PRUNE = false;
static constexpr bool EXTENDED_PRUNE = true;
static constexpr bool DP = true;
static constexpr bool NO_DP = false;
static constexpr int COMP = 0;
static constexpr int LPF_PRUNED_S_CUT_DP = 1;
static constexpr int LPF_PRUNED_S_CUT_NO_DP = 2;
static constexpr int LPF_PRUNED_S_NO_CUT_NO_DP = 3;
static constexpr int LPF_PRUNED_Z_CUT_DP = 4;
static constexpr int LPF_PRUNED_Z_CUT_NO_DP = 5;
static constexpr int LPF_THEORY_Z_CUT_NO_DP = 6;
static constexpr int LPF_THEORY_S_CUT_NO_DP = 7;
static constexpr int LPF_HEU = 8;
static constexpr int FP_PRUNED_S_CUT = 9;
static constexpr int FP_PRUNED_Z_CUT = 10;
static constexpr int FP_THEORY_Z_CUT = 11;
static constexpr int FP_THEORY_S_CUT = 12;
static constexpr int WAVELET_TREE = 13;
static constexpr int PASTA_WAVELT_MATRIX = 1;
static constexpr int PASTA_WAVELT_TREE= 2;

//std::cout << id << "," << text_id << "," << vec.size() << "," << bt->print_space_usage() << "," <<ms_int1.count() << "," << malloc_count_peak() + small_size<< "," << (double)elapsed/access_queries_.size() << "," << ms_int2.count() << "," << bt->print_space_usage() <<","<< (double)elapsed2/select_queries_.size() << s<<"," << tau << "," << max_leaf_size <<std::endl;
void print_result(int id, const std::string& text_id, int vec_size, int small_size, int first_time, int64_t peak_size, double access_avg, int snd_time, int big_size, double rank_avg, double select_avg, int s, int tau, int leaf_size, int64_t result) {
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
    std::cout << select_avg << ",";
    std::cout << s << ",";
    std::cout << tau << ",";
    std::cout << leaf_size << ",";
    std::cout << result << std::endl;

}
int cbt_size_no_rank(PCBlockTree& cbt) {
    int bt_bv_size = sizeof(void*);
    for (sdsl::bit_vector* bv : cbt.bt_bv_) {
        bt_bv_size += (int)sdsl::size_in_bytes(*bv);
    }
    int bt_bv_rank_size = sizeof(void*);
    for (sdsl::rank_support_v<1>* bvr : cbt.bt_bv_rank_) {
        bt_bv_rank_size +=(int)sdsl::size_in_bytes(*bvr);
    }
    int bt_offsets_size = sizeof(void*);
    for (sdsl::int_vector<>* offsets: cbt.bt_offsets_) {
        bt_offsets_size += (int)sdsl::size_in_bytes(*offsets);
    }
    int leaf_string_size = (int)sdsl::size_in_bytes(*cbt.leaf_string_);
    int alphabet_size = (int)sdsl::size_in_bytes(*cbt.alphabet_);
    int mapping_size = sizeof(int) * 256;
    // int partial_total_size = mapping_size + alphabet_size + bt_bv_size+ bt_bv_rank_size+ bt_offsets_size + leaf_string_size;
    return leaf_string_size + bt_offsets_size + bt_bv_rank_size + bt_bv_size + alphabet_size + mapping_size;
}
int cbt_size_rank(PCBlockTree& cbt) {
    int result = 0;
    int bt_prefix_ranks_first_level_size = (int)cbt.bt_first_level_prefix_ranks_.size()+1 * sizeof(void*);;
    for (auto pair: cbt.bt_first_level_prefix_ranks_) {
        bt_prefix_ranks_first_level_size += (int)sdsl::size_in_bytes(*(pair.second));
    }
    int bt_first_ranks_total_size = (int)(cbt.bt_first_ranks_.size()+1) * sizeof(void*);
    for (const auto& pair: cbt.bt_first_ranks_) {
        int size = 0;
        for (sdsl::int_vector<>* ranks: pair.second) {
            size += (int)sdsl::size_in_bytes(*ranks);
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
    for (auto pair: cbt.bt_prefix_ranks_) {
        int size = 0;
        for (sdsl::int_vector<>* ranks: pair.second) {
            size += sdsl::size_in_bytes(*ranks);
        }
        bt_prefix_ranks_first_level_size += size;
    }
    result = bt_prefix_ranks_first_level_size + bt_first_ranks_total_size + bt_second_ranks_total_size + bt_prefix_ranks_total_size;
    return result + cbt_size_no_rank(cbt);
}
int run_bench_comp(std::string& input, const std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
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
    for (int c: characters)
        bt->add_rank_select_support(c);
    auto* cbt = new PCBlockTree(bt);
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
    delete abt;
    delete bt;
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_queries_.size() - 1; i++) {
        result += cbt->better_paper_rank((char)select_c_[i], select_queries_[i] - 1);
    }
    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_queries_.size() - 1; i++) {
        result += cbt->better_paper_select((char)select_c_[i], select_queries_[i]);
    }
    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    print_result(id, text_id, (int)input.size(), cbt_size_no_rank(*cbt), (int) ms_int1.count() , size, (double)elapsed/access_queries_.size(), ms_int2.count() + ms_int3.count(),cbt_size_rank(*cbt) ,(double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size(), 1, tau, max_leaf_size, result);
    delete cbt;
    return 0;
}

int run_bench_lpf_pruned_s_cut_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_CUT_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s,true, CUT_FIRST_LEVEL, DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_s_cut_no_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_CUT_NO_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    auto*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s,true, CUT_FIRST_LEVEL, NO_DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_s_no_cut_no_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_S_NO_CUT_NO_DP;
    auto wm = pasta::make_wm<pasta::BitVector>(vec.begin(), vec.end(),
                                               256);
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_lpf_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s,true, DONT_CUT_FIRST_LEVEL, NO_DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_z_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_Z_CUT_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,true, CUT_FIRST_LEVEL, DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}

int run_bench_lpf_pruned_z_no_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_PRUNED_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,true, CUT_FIRST_LEVEL, NO_DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_theory_z_no_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_THEORY_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_theory<uint8_t, int32_t>(vec, tau, max_leaf_size, NO_DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_theory_s_cut_no_dp(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_THEORY_S_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();

    BV_BlockTree_lpf_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_lpf_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,s, NO_DP);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_lpf_heuristics(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = LPF_HEU;
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
    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
//static constexpr int FP_PRUNED_S_CUT = 9;
//static constexpr int FP_PRUNED_Z_CUT = 10;
int run_bench_fp_pruned_s_CUT(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_PRUNED_S_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,s,256,true, true);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_pruned_z_CUT(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_PRUNED_Z_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    lpf_array_stack(vec, lpf, lpf_ptr);
    calculate_lz_factor(s,lpf, lz);
    BV_BlockTree_fp_pruned<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,lz.size(),256,CUT_FIRST_LEVEL, true);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_theo_s_CUT(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {

    int id = FP_THEORY_S_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    BV_BlockTree_fp_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,s,256,true);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_bench_fp_theo_z_CUT(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
    int id = FP_THEORY_Z_CUT;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    lpf_array_stack(vec, lpf, lpf_ptr);
    calculate_lz_factor(s,lpf, lz);
    BV_BlockTree_fp_theory<uint8_t, int32_t>*  bt = new BV_BlockTree_fp_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,s,256,CUT_FIRST_LEVEL);
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
        result += bt->rank(select_c_[i], select_queries_[i] - 1);
    }

    auto elapsed2 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start2).count();
    auto start3 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < select_c_.size(); i++) {
        result += bt->select(select_c_[i], select_queries_[i]);
    }

    auto elapsed3 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start3).count();
    int64_t size_heap = vec.size() * 24;
    print_result(id, text_id, vec.size(), small_size, ms_int1.count(), size_heap,(double)elapsed/access_queries_.size(), ms_int2.count(), bt->print_space_usage(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result );
    delete bt;
    return 0;
}
int run_pasta_wavelet_matrix(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
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
int run_pasta_wavelet_tree(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size) {
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
#endif //BLOCKTREESEXPERIMENTS_HELPER_H
