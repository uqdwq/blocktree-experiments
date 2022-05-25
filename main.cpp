#include <iostream>
#include <string>
#include <vector>
#include <tlx/cmdline_parser.hpp>
std::string get_file_contents(const char *filename, size_t count)
{
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp)
  {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, count, fp);
    std::fclose(fp);
    return(contents);
  }
  throw(errno);
}

int main(int argc, char* argv[]) {
    tlx::CmdlineParser cp;
    cp.set_author("Daniel Meyer <uqdwq@student.kit.edu>");
    cp.set_description("This program should run all the expriments for my thesis");

    // add an unsigned integer option --rounds <N>
    unsigned rounds = 0;
    cp.add_unsigned('r', "rounds", "N", rounds,
                    "Run N rounds of the experiment.");

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
    std::string content;
    content =  get_file_contents(a_filename.c_str(), a_size);
    // do something useful
    std::cout << content << std::endl;
    return 0;
}

