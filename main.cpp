#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <tlx/cmdline_parser.hpp>
#include "naiveBlocktree/naiveBlocktree.h"

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
    cp.add_bytes('s', "size", a_size,
                 "Number of bytes to process.");

    // add a required parameter
    std::string a_filename;
    cp.add_param_string("filename", a_filename,
                        "A filename to process");

    // process command line
    if (!cp.process(argc, argv))
        return -1; // some error occurred and help was always written to user.

    std::cout << "Command line parsed okay." << std::endl;

    // output for debugging
    cp.print_result();
    // std::vector<char> content;
    // content =  read_all_bytes(a_filename.c_str(), a_size);
    // do something useful
    char name[] = "bla";
    char *test = "Test"; 
    std::cout << name << std::endl;
    std::cout << test << std::endl;
    return 0;
}

