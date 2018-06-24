#include "Handler.h"
#include "Writers.h"

#include <algorithm>

Handler::Handler(const int& n) {
  if (n <= 0) {
    throw std::runtime_error("error set N"); 
  } 
  N = n;
  commands = std::make_shared<Commands>();
  error.first = false;
}

Handler::~Handler() {
  for (auto& writer : writers) {
    writer.second->unlock();
  }
}

void Handler::print() {
  addBlock();
  addCommands(commands->size());
  cv.notify_all();
  std::unique_lock<std::mutex> lk(mtx);
  one_of_print = true;
  job_count = writers.size();
  for (auto& writer : writers) {
    writer.second->unlock();
  }
  cv.wait(lk,[this]{
    if (error.first) {
      std::unique_lock<std::mutex> lk(error_mtx);
      throw std::runtime_error(error.second);
    }
    return job_count == 0;});
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
  std::unique_lock<std::mutex> lk(mtx);
  job_count--;
  cv.notify_one(); 
}

void Handler::subscribe(const std::weak_ptr<Observer>& obs) {
  if (obs.expired()) return;
  
  auto mtx = std::make_shared<std::mutex>();
  mtx->lock();
  writers.emplace_back(obs,mtx);
  std::thread([obs,mtx](std::weak_ptr<Handler> handler){
    try {
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
        if (!handler.expired()) {
          handler_ptr->unlock();
        }
      }
    } catch(const std::exception &e) {
      auto handler_ptr = handler.lock();
      if (!handler.expired()) {
        std::stringstream error_stream;
        error_stream << e.what() << " in " << std::this_thread::get_id();
        {
          std::unique_lock<std::mutex> lk(handler_ptr->error_mtx);
          handler_ptr->error.second = error_stream.str();
          handler_ptr->error.first = true;
        }
        handler_ptr->unlock();
      }
    }
  },shared_from_this()).detach();
}

void Handler::addCommand(const std::string& command) { 
  if (command.size() > max_size_commad) {
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
      if (N == 0) throw std::runtime_error("emty block");
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
