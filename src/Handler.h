#ifndef handler_h
#define handler_h

#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <memory>
#include <condition_variable>
#include <thread>
#include <atomic>

#include "Parser.h"
#include "Statistics.h"

class Observer;

class Handler : public Statistics {
  using Commands = std::vector<std::string>;
  using Pending = std::tuple<int,std::condition_variable,std::mutex>;

  std::list<std::weak_ptr<Observer>> writers;
  std::vector<std::shared_ptr<std::mutex>> mtxs;
  std::shared_ptr<Commands> commands;
  BlockParser parser;
  int N = 0;

  std::shared_ptr<Pending> cancel_print;
  std::atomic<bool> one_of_print;
  std::mutex one_of_mtx;

  void print();
  void update();
public:
  Handler();
  void subscribe(const std::weak_ptr<Observer>& obs);
  void setN(const int& n);
  void addCommand(const std::string& command);
  void stop();
};

#endif