#include "Handler.h"
#include "Observer.h"

#include <algorithm>

Handler::Handler() {
  commands = std::make_shared<Commands>();
}

void Handler::print() {
  addBlock();
  addCommands(commands->size());
  for(auto& writer : writers) {
    if (!writer.expired()) {
      writer.lock()->print();
    }
  }
}

void Handler::update() {
  for(auto& writer : writers) {
    if (!writer.expired()) {
      writer.lock()->update(commands);
    }
  }
}

void Handler::subscribe(const std::weak_ptr<Observer>& obs) {
  writers.push_back(obs);
}

void Handler::setN(const int& n) {
  int size = commands->size();
  if (n < size) {
    throw std::runtime_error("error set N"); 
  } 
  N = n;
}

void Handler::addCommand(const std::string& command) { 
  if (command.size() > 50) {
    throw std::runtime_error("very large string");
  }
  
  if (N == 0) {
    throw std::runtime_error("parameter is zero"); 
  }  

  addLine();
  switch(parser.parsing(command))
  {
    case BlockParser::Empty: 
      break;

    case BlockParser::StartBlock: 
      N = -1; 
      if (commands->size() > 0)
        print();
      commands->clear();
      break;

    case BlockParser::CancelBlock:
      N = commands->size();
      if (N == 0) N = 1;
      break;

    case BlockParser::Command:
      commands->push_back(command);
      update();
      break;

    default: break;
  }

  if (commands->size() == N) {
    print();
    commands->clear();
  }
}

void Handler::stop() {
  if (N != -1 && commands->size())
    print();
  commands->clear();
}
