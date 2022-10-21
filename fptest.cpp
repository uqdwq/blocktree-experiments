#include <tlx/cmdline_parser.hpp>
#include <iostream>
#include <unordered_map>
#include "../block_tree/includes/utils/MersenneRabinKarp.h"
#include "../block_tree/includes/utils/MersenneHash.h"
#include <blocktree.utils/RabinKarp.h>
#include <blocktree.utils/HashString.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include "malloc_count.h"

  // static constexpr uint128_t kPrime = 562949953421231ULL;
  // static constexpr uint128_t kPrime = 1125899906842597ULL;
  // static constexpr uint128_t kPrime = 2251799813685119ULL;
  // static constexpr uint128_t kPrime = 4503599627370449ULL;
  // static constexpr uint128_t kPrime = 9007199254740881ULL;
  // static constexpr uint128_t kPrime = 18014398509481951ULL;
  // static constexpr uint128_t kPrime = 36028797018963913ULL;
  // static constexpr uint128_t kPrime = 72057594037927931ULL;
  // static constexpr uint128_t kPrime = 144115188075855859ULL;
  // static constexpr uint128_t kPrime = 288230376151711717ULL;
  // static constexpr uint128_t kPrime = 576460752303423433ULL;
  // static constexpr uint128_t kPrime = 1152921504606846883ULL;
  // static constexpr uint128_t kPrime = 2305843009213693951ULL;
  //2147483647
int main(int argc, char* argv[]) {
//    std::cout << "HI" << std::endl;
    tlx::CmdlineParser cp;
    cp.set_author("Daniel Meyer <uqdwq@student.kit.edu>");
    cp.set_description("This program should run all the expriments for my thesis");
    uint64_t a_size = 0;
    cp.add_bytes('p', "size", a_size, "Number of bytes to process.");
    std::string a_filename;
    cp.add_param_string("filename", a_filename, "A filename to process");
    if (!cp.process(argc, argv)) return -1; // some error occurred and help was always written to user.
    std::cout << a_filename << " " << a_size << std::endl;
    std::string input(a_size, ' ');
    std::ifstream t(a_filename);
    std::stringstream buffer;
    t.read(&input[0], a_size);
    std::vector<char> text(input.begin(), input.end());
    //std::cout << input << std::endl;              
    static constexpr uint128_t kPrime = 2305843009213693951ULL;
    static constexpr uint64_t mersenne_power = 31;
    static constexpr uint128_t sigma = 256;
    static constexpr uint64_t blocksize = 4096 * 2;
//    std::string input = "HalloWieGehtsHallo";
    // a_size = input.size();

    int N = 6700417; //Large prime
    RabinKarp rk2(input,0,blocksize,N);
    std::unordered_map<MersenneHash<char>, int> pairs(0);
    std::unordered_map<HashString, int> pairs2(0);
    // RabinKarp rk(*input_,0 ,2 * block_length, N);
    // for (int i = 0; i < input_->size() - block_length * 2; i++) {
//    HashString hS(rk.hash(), *input_, i, 2 * block_length - 1);
    MersenneHash<char> mh = MersenneHash<char>(text, rk.hash_, 0, blocksize);
    HashString hS(rk2.hash_, input, 0, blocksize - 1);
    pairs[mh] = 0;
    pairs2[hS] = 0;
    int c = 0;
    int c2 = 0;
    auto t11 = std::chrono::high_resolution_clock::now();
    for (int i = 1; i + blocksize - 1 < input.size(); i++) {
        rk2.next();
        HashString hS(rk2.hash_, input, i,i + blocksize - 1);
        if (pairs2.find(hS) != pairs2.end()) {
            c++;
        }
        pairs2[hS] = i;
    }
    auto t12 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t12 - t11);
    std::cout << "PAPER: time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
    std::cout << "matches old " << c << std::endl;
    c = 0;
    int m = 0;
    // std::cout << "SIZE MAP: " << pairs2.size() << std::endl;
    // std::cout << input.substr(0, blocksize) << " " << rk2.hash_ << std::endl;
    pairs2.clear();
    t11 = std::chrono::high_resolution_clock::now();
      for (int i = 1; i + blocksize - 1 < input.size(); i++) {

          rk.next();
          MersenneHash<char> mh = MersenneHash<char>(text, rk.hash_, i, blocksize);
          if (pairs.find(mh) != pairs.end()) {
              c2++;
          }
          pairs[mh] = i;
      }
      std::cout << "My map " << pairs.size() << " other " << pairs2.size() << std::endl;


    t12 = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t12 - t11);
    std::cout << "PAPER: time: " << ms_int.count() << " curMem: " << malloc_count_current() << " peakMem: " << malloc_count_peak() << std::endl;
    std::cout << c2 << " " << m <<  std::endl;
    
}