#include "Observer.h"

void Observer::subscribe(const std::weak_ptr<Handler>& handler) {
  if (!handler.expired())
    handler.lock()->subscribe(shared_from_this());
}

void Observer::update(const std::weak_ptr<Commands>& commands) {
  _commands = commands;
}