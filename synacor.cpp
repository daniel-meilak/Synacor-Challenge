#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include "synacor.h"

int main(){

   comp_t comp;

   // read data into memory
   load_mem(comp.memory,"challenge.bin");

   comp.run();

   return comp.error;
}


