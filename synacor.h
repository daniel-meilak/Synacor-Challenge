#pragma once

#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>
#include<iostream>
#include<cstdint>
#include<stack>
#include<array>

// computer atchitecture
struct comp_t{
   bool running;
   bool error;
   uint16_t read_pos;

   std::vector<uint16_t> memory;
   std::array<uint16_t,8> reg{0};
   std::stack<uint16_t> stack;
   std::string term_text;
   save_t save_state;

   comp_t(): running(false), error(false), read_pos(0){};


   // function select
   void select();

   // parameter check
   uint16_t &check(uint16_t &param);

   // autosave state
   void autosave();

   // reload last save
   void reload();

   // built-in functions
   void halt();
   void set (uint16_t &a, const uint16_t &b);
   void push(const uint16_t &a);
   void pop (uint16_t &a);
   void eq  (uint16_t &a, const uint16_t &b, const uint16_t &c);
   void gt  (uint16_t &a, const uint16_t &b, const uint16_t &c);
   void jmp (const uint16_t &a);
   void jt  (const uint16_t &a, const uint16_t &b);
   void jf  (const uint16_t &a, const uint16_t &b);
   void add (uint16_t &a, const uint16_t &b, const uint16_t &c);
   void mult(uint16_t &a, const uint16_t &b, const uint16_t &c);
   void mod (uint16_t &a, const uint16_t &b, const uint16_t &c);
   void band(uint16_t &a, const uint16_t &b, const uint16_t &c);
   void bor (uint16_t &a, const uint16_t &b, const uint16_t &c);
   void bnot(uint16_t &a, const uint16_t &b);
   void rmem(uint16_t &a, const uint16_t &b);
   void wmem(uint16_t &a, const uint16_t &b);
   void call(const uint16_t &a);
   void ret ();
   void out (const uint16_t &a);
   void in  (uint16_t &a);
   void noop();

   // run binary
   void run();
};

struct save_t{
   std::vector<uint16_t> memory;
   std::array<uint16_t,8> reg{0};
   std::stack<uint16_t> stack;
   uint16_t read_pos;
};

// function to read binary file and load into vector of int (memory)
void load_mem(std::vector<uint16_t> &memory, std::string filename){

   std::streampos file_size;

   // open input and position read at end of file
   std::ifstream input (filename, std::ios::in|std::ios::binary|std::ios::ate);

   // Check successful open
   if (!input.is_open()){
      std::cerr << "Could not read input file.\n";
      std::exit(EXIT_FAILURE);
   }

   // get file size from end of file
   file_size = input.tellg();

   // move read pos to beginning of file
   input.seekg(0,std::ios::beg);

   // resize vector
   memory.resize(file_size);

   // read file into vector
   input.read((char*) &memory[0], file_size);

   input.close();
}

// run computer
void comp_t::run(){
   running = true;
   while (running){
      select();
   }
}

void comp_t::autosave(){

   save_state.memory   = memory;
   save_state.reg      = reg;
   save_state.stack    = stack;
   save_state.read_pos = read_pos;
}

void comp_t::reload(){

   memory   = save_state.memory;
   reg      = save_state.reg;
   stack    = save_state.stack;
   read_pos = save_state.read_pos;
}

// function select
void comp_t::select(){

   uint16_t &opcode = check(memory[read_pos]);

   switch (opcode){
   case 0:
      halt();
      break;

   case 1:
      set( memory[read_pos+1], check(memory[read_pos+2] ));
      break;
   
   case 2:
      push( check(memory[read_pos+1]) );
      break;

   case 3:
      pop( check(memory[read_pos+1]) );
      break;

   case 4:
      eq( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 5:
      gt( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 6:
      jmp( check(memory[read_pos+1]) );
      break;

   case 7:
      jt( check(memory[read_pos+1]), check(memory[read_pos+2]));
      break;

   case 8:
      jf( check(memory[read_pos+1]), check(memory[read_pos+2]));
      break;

   case 9:
      add( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 10:
      mult( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 11:
      mod( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 12:
      band( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 13:
      bor( check(memory[read_pos+1]), check(memory[read_pos+2]), check(memory[read_pos+3]));
      break;

   case 14:
      bnot( check(memory[read_pos+1]), check(memory[read_pos+2]));
      break;

   case 15:
      rmem( check(memory[read_pos+1]), check(memory[read_pos+2]));
      break;

   case 16:
      wmem( check(memory[read_pos+1]), check(memory[read_pos+2]));
      break;

   case 17:
      call( check(memory[read_pos+1]) );
      break;

   case 18:
      ret();
      break;

   case 19:
      out( check(memory[read_pos+1]) );
      break;

   case 20:
      in( check(memory[read_pos+1]) );
      break;

   case 21:
      noop();
      break;

   default:
      std::cerr << "Undefined opcode.\n";
      error = true;
      running = false;
      break;
   }
}

// parameter check
// returns a reference to a value
uint16_t &comp_t::check(uint16_t &param){

   if (param >= 32768){ return reg[param % 32768]; }
   else               { return param;              } 
}


// stop execution and terminate the program
void comp_t::halt(){

   std::string in;
   std::cout << "Reload last save?\n";
   std::cin  >> in;

   if (in == "yes"){ reload(); }
   else { running = false; }
}

// set register <a> to the value of <b>
void comp_t::set(uint16_t &a, const uint16_t &b){
   reg[a%32768] = b;
   read_pos += 3; 
}

// push <a> onto the stack
void comp_t::push(const uint16_t &a){
   stack.push(a);
   read_pos += 2;
}

// remove the top element from the stack and write it into <a>; empty stack = error
void comp_t::pop(uint16_t &a){
   
   if (stack.empty()){
      error = true;
      running = false;
      std::cerr << "Cannot pop, stack empty.\n";
   }

   a = stack.top();
   stack.pop();
   read_pos += 2;
}

// set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise
void comp_t::eq(uint16_t &a, const uint16_t &b, const uint16_t &c){

   if (b == c){ a = 1; }
   else       { a = 0; }
   read_pos += 4;
}

// set <a> to 1 if <b> is greater than <c>; set it to 0 otherwise
void comp_t::gt(uint16_t &a, const uint16_t &b, const uint16_t &c){

   if (b > c){ a = 1; }
   else      { a = 0; }
   read_pos += 4;
}

// jump to <a>
void comp_t::jmp(const uint16_t &a){
   read_pos = a;
}

// if <a> is nonzero, jump to <b>
void comp_t::jt(const uint16_t &a, const uint16_t &b){
   if (a != 0){ read_pos = b; }
   else       { read_pos+= 3; }
}

// if <a> is zero, jump to <b>
void comp_t::jf(const uint16_t &a, const uint16_t &b){
   if (a == 0){ read_pos = b; }
   else       { read_pos+= 3; }
}

// assign into <a> the sum of <b> and <c> (modulo 32768)
void comp_t::add(uint16_t &a, const uint16_t &b, const uint16_t &c){
   a = (b + c)% 32768;
   read_pos += 4;
}

// store into <a> the product of <b> and <c> (modulo 32768)
void comp_t::mult(uint16_t &a, const uint16_t &b, const uint16_t &c){
   a = (b * c)% 32768;
   read_pos += 4;
}

// store into <a> the remainder of <b> divided by <c>
void comp_t::mod(uint16_t &a, const uint16_t &b, const uint16_t &c){
   a = b % c;
   read_pos += 4;
}

// stores into <a> the bitwise and of <b> and <c>
void comp_t::band(uint16_t &a, const uint16_t &b, const uint16_t &c){
   a = b & c;
   read_pos += 4;
}

// stores into <a> the bitwise or of <b> and <c>
void comp_t::bor(uint16_t &a, const uint16_t &b, const uint16_t &c){
   a = b | c;
   read_pos += 4;
}

// stores 15-bit bitwise inverse of <b> in <a>
void comp_t::bnot(uint16_t &a, const uint16_t &b){
   a = ~b;
   a &= ~(1 << 15);
   read_pos += 3;
}

// read memory at address <b> and write it to <a>
void comp_t::rmem(uint16_t &a, const uint16_t &b){
   a = memory[b];
   read_pos += 3;
}

// write the value from <b> into memory at address <a>
void comp_t::wmem(uint16_t &a, const uint16_t &b){
   memory[a] = b;
   read_pos += 3;
}

// write the address of the next instruction to the stack and jump to <a>
void comp_t::call(const uint16_t &a){
   stack.push(read_pos+2);
   read_pos = a;
}

// remove the top element from the stack and jump to it; empty stack = halt
void comp_t::ret(){

   if (stack.empty()){
      error = true;
      running = false;
      std::cerr << "Cannot pop, stack empty.\n";
   }

   read_pos = stack.top();
   stack.pop();
}

// write the character represented by ascii code <a> to the terminal
void comp_t::out(const uint16_t &a){
   std::cout << static_cast<char>(a);
   read_pos += 2;
}

// read a character from the terminal and write its ascii code to <a>;
// it can be assumed that once input starts, it will continue until a newline is encountered;
// this means that you can safely read whole lines from the keyboard and trust that they will be fully read
void comp_t::in(uint16_t &a){
   if (term_text.empty()){
      autosave();
      std::getline(std::cin,term_text);
      term_text.push_back('\n');
   }

   a = term_text.front();
   // (inefficient)
   term_text = term_text.substr(1);

   read_pos += 2;
}

// no operation
void comp_t::noop(){
   read_pos += 1;
}



