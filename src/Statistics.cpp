#include "Statistics.h"

void Statistics::addLine() {
  if (!_counter.expired())
    _counter.lock()->lines++;
}
  
void Statistics::addBlock() {
  if (!_counter.expired())
    _counter.lock()->blocks++;
}
  
void Statistics::addCommands(const uint& count) {
  if (!_counter.expired())
    _counter.lock()->commands+=count;
}

void Statistics::setCounter(const std::weak_ptr<Counter>& counter) {
  _counter = counter;
}


std::ostream& operator<<(std::ostream& out, const Counter& counter) {
    out << counter.name << " поток - ";
    if (counter.lines) {
      out << counter.lines << ", ";
    }
    out << counter.commands << ", " << counter.blocks;
    return out;
}