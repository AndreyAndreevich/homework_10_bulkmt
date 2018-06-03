#ifndef statistics_h
#define statistics_h

#include <iostream>
#include <string>
#include <memory>
#include <tuple>

class Statistics;

class Counter {
  uint lines = 0;
  uint commands = 0;
  uint blocks = 0;
  std::string name;
  friend Statistics;
  friend std::ostream& operator<<(std::ostream&, const Counter&);
public:
  Counter() = delete;
  Counter(std::string name_) : name(name_) {} 
  std::tuple<int,int,int> get();
};

//--------------------------------------------------------------------------------

class Statistics {
protected:
  std::weak_ptr<Counter> _counter;
  void addLine();
  void addBlock();
  void addCommands(const uint& count);
public:
  void setCounter(const std::weak_ptr<Counter>& counter);
  virtual ~Statistics() {}
};

#endif