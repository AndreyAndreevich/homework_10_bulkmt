#include "Statistics.h"

std::tuple<int,int,int> Counter::get() {
  return std::make_tuple(lines,commands,blocks);
}

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
      out << "строк: " << counter.lines << ", ";
    }
    out << "комманд: " << counter.commands << ", блоков: " << counter.blocks;
    return out;
}