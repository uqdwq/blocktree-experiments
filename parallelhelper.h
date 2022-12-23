//
// Created by daniel on 12.12.22.
//
#include <vector>
#ifndef BLOCKTREESEXPERIMENTS_PARALLELHELPER_H
#define BLOCKTREESEXPERIMENTS_PARALLELHELPER_H
void print_result_par(int64_t id, const std::string& text_id, int64_t vec_size, int64_t lpf_time, int64_t first_time, double access_avg, int64_t snd_time, double rank_avg, double select_avg, int s, int tau, int leaf_size, int64_t result, int threads) {
    std::cout << id << ",";
    std::cout << text_id << ",";
    std::cout << threads << ",";
    std::cout << vec_size << ",";
    std::cout << lpf_time << ",";
    std::cout << first_time << ",";
    std::cout << access_avg << ",";
    std::cout << snd_time << ",";
    std::cout << rank_avg << ",";
    std::cout << select_avg << ",";
    std::cout << s << ",";
    std::cout << tau << ",";
    std::cout << leaf_size << ",";
    std::cout << result << std::endl;

}
int run_bench_lpf_pruned_z_parallel(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size, int threads) {
    int id = LPF_PRUNED_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    int32_t lzn = 0;
    lpf_array_ansv(vec,lpf,lpf_ptr,threads);
    calculate_lz_factor(lzn,lpf,lz);
    auto t0x = std::chrono::high_resolution_clock::now();
    auto*  bt = new BV_BlockTree_lpf_pruned<uint8_t, int32_t>(vec, tau, max_leaf_size,lpf,lpf_ptr,lz,true,true);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support_omp(threads);
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    auto ms_intx = std::chrono::duration_cast<std::chrono::milliseconds>(t0x - t01);
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

    print_result_par(id, text_id, vec.size(), ms_intx.count(), ms_int1.count(),(double)elapsed/access_queries_.size(), ms_int2.count(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result, threads);
    delete bt;
    return 0;
}
int run_bench_lpf_heu_z_parallel(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size, int threads) {
    int id = LPF_HEU;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    int32_t lzn = 0;
    lpf_array_ansv(vec,lpf,lpf_ptr,threads);
    calculate_lz_factor(lzn,lpf,lz);
    auto t0x = std::chrono::high_resolution_clock::now();
    auto*  bt = new BV_BlockTree_lpf_heuristic<uint8_t, int32_t>(vec, tau, max_leaf_size,lpf,lpf_ptr,lz);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support_omp(threads);
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    auto ms_intx = std::chrono::duration_cast<std::chrono::milliseconds>(t0x - t01);
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

    print_result_par(id, text_id, vec.size(), ms_intx.count(), ms_int1.count(),(double)elapsed/access_queries_.size(), ms_int2.count(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result, threads);
    delete bt;
    return 0;
}
int run_bench_lpf_theo_z_parallel(std::vector<uint8_t>& vec, std::string& text_id, std::vector<int>& access_queries_, std::vector<int>& select_queries_, std::vector<uint8_t>& select_c_, int s, int tau, int max_leaf_size, int threads) {
    int id = LPF_THEORY_Z_CUT_NO_DP;
    auto t01 = std::chrono::high_resolution_clock::now();
    std::vector<int32_t> lpf(vec.size());
    std::vector<int32_t> lpf_ptr(vec.size());
    std::vector<int32_t> lz;
    int32_t lzn = 0;
    lpf_array_ansv(vec,lpf,lpf_ptr,threads);
    calculate_lz_factor(lzn,lpf,lz);
    auto t0x = std::chrono::high_resolution_clock::now();
    auto*  bt = new BV_BlockTree_lpf_theory<uint8_t, int32_t>(vec, tau, max_leaf_size,lpf,lpf_ptr,lz);
    auto t02 = std::chrono::high_resolution_clock::now();
    auto small_size = bt->print_space_usage();
    bt->add_rank_support_omp(threads);
    auto t03 = std::chrono::high_resolution_clock::now();
    auto ms_int1 = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
    auto ms_int2 = std::chrono::duration_cast<std::chrono::milliseconds>(t03 - t01);
    auto ms_intx = std::chrono::duration_cast<std::chrono::milliseconds>(t0x - t01);
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

    print_result_par(id, text_id, vec.size(), ms_intx.count(), ms_int1.count(),(double)elapsed/access_queries_.size(), ms_int2.count(), (double)elapsed2/select_queries_.size(),(double)elapsed3/select_queries_.size() , s, tau, max_leaf_size, result, threads);
    delete bt;
    return 0;
}
#endif //BLOCKTREESEXPERIMENTS_PARALLELHELPER_H
