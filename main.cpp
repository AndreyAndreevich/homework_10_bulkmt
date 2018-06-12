#include <iostream>

#include "Writers.h"
#include "Parser.h"

int main(int argc, char *argv[]) 
{
  auto main_counter = std::make_shared<Counter>("main");
  auto log_counter = std::make_shared<Counter>("log");
  auto file1_counter = std::make_shared<Counter>("file1");
  auto file2_counter = std::make_shared<Counter>("file2");
  try {
    auto N = start_parsing(argc,argv);
    auto handler = std::make_shared<Handler>();
    auto console_writer = std::make_shared<ConsoleWriter>();
    auto file1_writer = std::make_shared<FileWriter>();
    auto file2_writer = std::make_shared<FileWriter>();
    handler->setCounter(main_counter);
    console_writer->setCounter(log_counter);
    file1_writer->setCounter(file1_counter);
    file2_writer->setCounter(file2_counter);
    console_writer->subscribe(handler);
    file1_writer->subscribe(handler);
    file2_writer->subscribe(handler);
    handler->setN(N);   
    std::string line;
    while (std::getline(std::cin, line)) {
      handler->addCommand(line);
    }
    handler->stop();
    std::cout << *main_counter << "\n"
              << *log_counter << "\n"
              << *file1_counter << "\n"
              << *file2_counter << std::endl;
  } catch(const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}