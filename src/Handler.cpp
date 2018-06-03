#include "Handler.h"
#include "Writers.h"

#include <algorithm>

Handler::Handler() {
  commands = std::make_shared<Commands>();
  cancel_print = std::make_shared<Pending>();
}

void Handler::print() {
  addBlock();
  addCommands(commands->size());
  std::get<0>(*cancel_print) = mtxs.size();
  one_of_print = true;
  for (auto& mtx : mtxs) {
    mtx->unlock();
  }
  std::unique_lock<std::mutex> lk(std::get<2>(*cancel_print));
  std::get<1>(*cancel_print).wait(lk,[this]{
    return std::get<0>(*cancel_print) == 0;});
}

void Handler::update() {
  for(auto& writer : writers) {
    if (!writer.expired()) {
      writer.lock()->update(commands);
    }/* else {
      writers.erase(writer);
    }*/
  }
}

void Handler::subscribe(const std::weak_ptr<Observer>& obs) {
  if (!obs.expired()) {
    writers.push_back(obs);
    auto mtx = std::make_shared<std::mutex>();
    mtxs.push_back(mtx);
    mtx->lock();
    obs.lock()->set_print_cv(cancel_print);

    std::thread t([this,obs](std::weak_ptr<std::mutex> mtx){
      while(!obs.expired()) {       
        mtx.lock()->lock();
        if (obs.expired()) {
          mtx.lock()->unlock();
          break;
        }
        if (dynamic_cast<FileWriter*>(obs.lock().get())) {
          std::lock_guard<std::mutex> lk(one_of_mtx);
          if (one_of_print) {
            one_of_print = false;
          } else {
            obs.lock()->unlock();
            continue;
          }
        }
        obs.lock()->print();
      }
    },mtx);
    t.detach();
  }
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
