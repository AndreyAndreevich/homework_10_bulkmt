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

class Handler : public Statistics, public std::enable_shared_from_this<Handler> {
  using Commands = std::vector<std::string>;
  std::list<std::pair<std::weak_ptr<Observer>,std::shared_ptr<std::mutex>>> writers;
  std::shared_ptr<Commands> commands;
  BlockParser parser;
  int N = 0;
  int max_size_commad = 50;

  int job_count;
  std::condition_variable cv;
  std::mutex mtx;

  bool one_of = true;
  std::atomic<bool> one_of_print;
  std::mutex one_of_mtx;

  std::pair<bool,std::string> error;
  std::mutex error_mtx;

private:
  void initialization(const int& n);
protected:
  void print();
  void update();
  void unlock();
public:
  Handler(const int& n);
  Handler(const int& n, bool);
  ~Handler();
  void subscribe(const std::weak_ptr<Observer>& obs);
  void addCommand(const std::string& command);
  void stop();
};

#endif