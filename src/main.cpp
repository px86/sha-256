#include "include/sha256.hpp"
#include "include/argparser.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>

int main(int argc, char **argv) {
  const char *filepath = nullptr;

  auto ap = pr::ArgParser("sha256");
  ap.add_argument(filepath, "Path of the file", "<filepath>");
  ap.parse(argc, argv);

  std::cout << hash_file(filepath) << std::endl;

  return 0;
}
