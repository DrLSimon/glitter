#include <functional>
#include <iostream>
#include <vector>
#include "ObjLoader.hpp"
#include "Serialize.hpp"
#include "utils.hpp"

void printUsage(int /* argc */, char * argv[])
{
  std::cout << "Usage: " << argv[0] << " file.obj file.glitter\n";
}

int main(int argc, char * argv[])
{
  if (argc != 3) {
    printUsage(argc, argv);
    return 0;
  }
  ObjLoader objLoader(argv[1]);
  objLoader.saveBinaryFile(argv[2]);
}
