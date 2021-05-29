#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstdint>

int main(){

   std::streampos file_size;
   //char * memblock;

   std::ifstream input ("challenge.bin", std::ios::in|std::ios::binary|std::ios::ate);

   if (!input.is_open()){
      std::cerr << "Could not read input file.\n";
      std::exit(EXIT_FAILURE);
   }

   // get file size from end of file
   file_size = input.tellg();

   // move read pos to beginning of file
   input.seekg(0,std::ios::beg);

   // vector to store data
   std::vector<uint16_t> file_data(file_size);

   // read file into vector
   input.read((char*) &file_data[0], file_size);

   input.close();

   return 0;
}

