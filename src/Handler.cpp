#include "Handler.h"
#include "Writers.h"

#include <algorithm>

Handler::Handler() {
  commands = std::make_shared<Commands>();
  cancel_print = std::make_shared<Pending>();
}

Handler::~Handler() {
  for (auto& writer : writers) {
    writer.second->unlock();
  }
}

void Handler::print() {
  addBlock();
  addCommands(commands->size());
  std::unique_lock<std::mutex> lk(std::get<2>(*cancel_print));
  one_of_print = true;
  std::get<0>(*cancel_print) = writers.size();
  for (auto& writer : writers) {
    writer.second->unlock();
  }
  std::get<1>(*cancel_print).wait(lk,[this]{
    return std::get<0>(*cancel_print) == 0;});
}

void Handler::update() {
  auto iter = writers.begin();
  while(iter != writers.end()) {
    if(iter->first.expired()) {
      iter->second->unlock();
      auto dIter = iter;
      iter++;
      writers.erase(dIter);
    } else {
      iter->first.lock()->update(commands);
      iter++;
    }
  }
}

void Handler::unlock() {
  std::unique_lock<std::mutex> lk(std::get<2>(*cancel_print));
  std::get<0>(*cancel_print)--;
  std::get<1>(*cancel_print).notify_one(); 
}

void Handler::subscribe(const std::weak_ptr<Observer>& obs) {
  if (obs.expired()) return;
  
  auto mtx = std::make_shared<std::mutex>();
  mtx->lock();
  writers.emplace_back(obs,mtx);

  std::thread([obs,mtx](std::weak_ptr<Handler> handler){
    while(true) { 
      mtx->lock();
      auto handler_ptr = handler.lock();
      {
        auto observer_ptr = obs.lock();
        if (obs.expired() || handler.expired()) {
          break;
        }
        if (typeid(FileWriter) == typeid(*observer_ptr)) {
          std::lock_guard<std::mutex> lk(handler_ptr->one_of_mtx);
          if (handler_ptr->one_of_print) {
            handler_ptr->one_of_print = false;
            observer_ptr->print();
          } 
        } else {
          observer_ptr->print();
        }
      }
      handler_ptr->unlock(); 
    }
  },shared_from_this()).detach();
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
