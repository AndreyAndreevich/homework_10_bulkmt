#ifndef handler_h
#define handler_h

#include <string>
#include <vector>
#include <memory>

#include "Parser.h"
#include "Statistics.h"

class Observer;

class Handler : public Statistics {
  using Commands = std::vector<std::string>;

  std::vector<std::weak_ptr<Observer>> writers;
  std::shared_ptr<Commands> commands;
  BlockParser parser;
  int N = 0;

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