#ifndef observer_h
#define observer_h

#include "Handler.h"

class Observer : public std::enable_shared_from_this<Observer> {
protected:
  using Commands = std::vector<std::string>;
  std::weak_ptr<Commands> _commands;
public:
  virtual ~Observer() {};
  virtual void update(const std::weak_ptr<Commands>&);
  virtual void print() = 0;

  void subscribe(const std::weak_ptr<Handler>&);
};

#endif