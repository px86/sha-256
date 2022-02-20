#include "include/sha256.hpp"
#include "include/argparser.hpp"

int main(int argc, char **argv) {
  const char *filepath = nullptr;

  auto ap = pr::ArgParser("sha256");
  ap.add_argument(filepath, "Path of the file", "<filepath>");
  ap.parse(argc, argv);

  auto opt_hash = sha256_hash_file(filepath);
  if (opt_hash.has_value())
    std::cout << opt_hash.value() << std::endl;

  return 0;
}
