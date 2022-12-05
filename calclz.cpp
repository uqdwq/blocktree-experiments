//
// Created by daniel on 05.12.22.
//
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
    std::vector<uint8_t> vec(input.begin(), input.end());
    std::vector<int32_t> lpf2(vec.size());
    std::vector<int32_t> lpf_ptr2(vec.size());
    std::vector<int32_t> lz2;
    int32_t lzn = 0;
    lpf_array_stack(vec, lpf2, lpf_ptr2);
    int32_t s = 1;
    calculate_number_lz_factor(s, lpf2);
    std::cout << a_filename << "#LZ: " << s << " : " << vec.size() << std::endl;
}