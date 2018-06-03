#ifndef observer_h
#define observer_h

#include "Handler.h"

class Observer : public std::enable_shared_from_this<Observer> {
protected:
  using Commands = std::vector<std::string>;
  using Pending = std::tuple<int,std::condition_variable,std::mutex>;
  std::weak_ptr<Commands> _commands;
  std::weak_ptr<Pending> cancel_print;
public:
  void subscribe(const std::weak_ptr<Handler>& handler) {
    if (!handler.expired())
      handler.lock()->subscribe(shared_from_this());
  }

  virtual void update(const std::weak_ptr<Commands>& commands) {
    _commands = commands;
  }

  virtual void print() = 0;
  virtual void set_print_cv(const std::weak_ptr<Pending>& cancel_print_) {
    cancel_print = cancel_print_;
  }

  void unlock() {
    if (!cancel_print.expired()) {
      {
        std::lock_guard<std::mutex> lk(std::get<2>(*cancel_print.lock()));
        std::get<0>(*cancel_print.lock())--;
      }
      std::get<1>(*cancel_print.lock()).notify_one();
    }
  }

  virtual ~Observer() {}
};

#endif