#include "Parser.h"
#include "Writers.h"

#include <cstdlib>
#include <algorithm>
#include <iostream>

int vector_size = 1'000'000;
int repeat = 1'000;

struct TestFileFriter : FileWriter {
  std::vector<int> v;
  TestFileFriter() : FileWriter() {
    v.resize(vector_size);
    std::iota(v.begin(),v.end(), 0);
  }
  void print() override {
    int a = 5;
    for (int i = 0; i < repeat; i++) {
      std::random_shuffle(v.begin(),v.end());
      std::sort(v.begin(),v.end());
    }
    /*FileWriter::print();
    std::remove(name.c_str());*/
  }
}; 

int main(int argc, char *argv[]) 
{      
  const int threads_count = start_parsing(argc,argv); 
  std::vector<std::shared_ptr<TestFileFriter>> v;
  v.reserve(threads_count);
  auto handler = std::make_shared<Handler>(1,false);
  for (auto i = 0; i < threads_count; i++) {
    auto file_writer = std::make_shared<TestFileFriter>();
    file_writer->subscribe(handler);
    v.push_back(file_writer);
  }
  handler->addCommand("cmd1");   
}