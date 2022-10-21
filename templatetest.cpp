
#include <iostream>
#include <unordered_map>
#include "blockTree/FingerPrintBlockTree.h"
#include <blocktree.utils/RabinKarp.h>
#include <blocktree.utils/HashString.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include "malloc_count.h"
int main(int argc, char* argv[]) {
    std::string a_filename = "/home/daniel/blocktree-experiments/data/dna.200MB";
    uint64_t a_size = 1024 * 1024;
    std::string input(a_size, ' ');
    std::ifstream t(a_filename);
    std::stringstream buffer;
    t.read(&input[0], a_size);
    std::vector<char> text(input.begin(), input.end());
    auto bt = new FingerPrintBlockTree<char>(text, 2,1024*8,a_size,1);
}