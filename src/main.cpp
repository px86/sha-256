#include "include/sha256.hpp"
#include "include/argparser.hpp"

int main(int argc, char **argv) {
  const char *filepath = nullptr;

  auto ap = pr::ArgParser("sha256");
  ap.add_argument(filepath, "Path of the file", "<filepath>");
  ap.parse(argc, argv);

  auto sha = sha256(filepath);
  auto digest_str = sha.digest_str();

  std::cout << digest_str << std::endl;

  return 0;
}
