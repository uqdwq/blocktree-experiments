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

std::vector<char> read_all_bytes(char const* filename, size_t size) {
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    if (pos == 0) {
        return std::vector<char>{};
    }

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], size);

    return result;
}

int main(int argc, char* argv[]) {

    tlx::CmdlineParser cp;
    cp.set_author("Daniel Meyer <uqdwq@student.kit.edu>");
    cp.set_description("This program should run all the expriments for my thesis");

    // add a byte size argument which the user can enter like '1gi'
    uint64_t a_size = 0;
    cp.add_bytes('p', "size", a_size,
                 "Number of bytes to process.");
    int s = 128;
    int tau = 2;
    int l = 4;
    int m = 2;
    cp.add_int('m', "which mode is to test", m, "");
    // add a required parameter
    std::string a_filename;
    cp.add_param_string("filename", a_filename,
                        "A filename to process");

    // process command line
    if (!cp.process(argc, argv))
        return -1; // some error occurred and help was always written to user.

    //std::cout << "Command line parsed okay." << std::endl;

    // output for debugging
    //cp.print_result();
    // std::vector<char> content;
    // content =  read_all_bytes(a_filename.c_str(), a_size);
    // do something useful
    std::string input(a_size, ' ');
    std::ifstream t(a_filename);
    std::stringstream buffer;
    t.read(&input[0], a_size);
    
    switch (m) {
        case 0:
        {
            auto t01 = std::chrono::high_resolution_clock::now();
            NBlockTree* bt0 = new NBlockTree(&input,1,2,32);  
            bt0->build_tree();
            auto t02 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t02 - t01);
            std::cout << input.size() << "," << s << "," << tau << "," << l << "," << ms_int.count() << "," << malloc_count_current() << "," << malloc_count_peak() << std::endl;
            break;
        }
        case 1:
        {
            auto t11 = std::chrono::high_resolution_clock::now();
            PBlockTree* bt1 = new PBlockTree(input, 2, 32);
            bt1->process_back_pointers();
            auto t12 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t12 - t11);
            std::cout << "PAPER: time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
            break;
        }
        case 2:
        {
            auto t21 = std::chrono::high_resolution_clock::now();
            NBlockTree* bt2 = new NBlockTree(&input,1,2,32);  
            bt2->build_tree_fp();
            auto t22 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t22 - t21);
            std::cout << "NAIVE: time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
            break;
        }
        case 3:
        {
            auto t21 = std::chrono::high_resolution_clock::now();
            NBlockTree* bt2 = new NBlockTree(&input,128,2,32);  
            bt2->build_tree();
            auto t22 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t22 - t21);
            std::cout << "NAIVE: time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
            break;
            break;
        }
        default:
        {
            auto td1 = std::chrono::high_resolution_clock::now();
            NBlockTree* bt3 = new NBlockTree(&input,s,tau,l);  
            bt3->build_tree();
            auto td2 = std::chrono::high_resolution_clock::now();
            auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(td2 - td1);
            std::cout << "s: " << s << " tau: " << tau << " leaf: " << l << " time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
        }
    }
    
 
    // for (int c: characters) {
    //     bt->add_rank_select_support(c);
    // }
    // auto tx = std::chrono::high_resolution_clock::now();
    // ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(tx - t2);
    // std::cout << ms_int.count() << "ms\n";
    // std::cout << bt->rank(71,71) << std::endl;
    
   
   

    
    return 0;
}

