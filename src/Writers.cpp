#include "Writers.h"

#include <iostream>

ConsoleWriter::ConsoleWriter() {
  out = &std::cout;
}
  
ConsoleWriter::ConsoleWriter(std::ostream& out_stream) {
  out = &out_stream;
}

void ConsoleWriter::print() {
  if (_commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  auto commands = *_commands.lock();
  addBlock();
  addCommands(commands.size());
  unlock();
  *out << "bulk: ";
  for(auto command = commands.cbegin(); command < commands.cend(); command++) {
    if (command != commands.cbegin())
      *out << ", ";
    *out << *command;
  }
  *out << std::endl;
}

//---------------------------------------------------------------------------------

int FileWriter::count = 0;

FileWriter::FileWriter() {
  count++;
  id = count;
}

FileWriter::FileWriter(std::weak_ptr<std::time_t> time_, const int& id_) {
  if (id_) {
    id = id_;
  } else {
    count++;
    id = count;
  }
  time = time_;
}

void FileWriter::update(const std::weak_ptr<Commands>& commands) {
  if (commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  if (commands.lock()->size() == 1) {
    std::stringbuf out_buffer;
    std::ostream out_stream(&out_buffer);
    std::time_t* time_ = time.expired() ? nullptr : time.lock().get();
    out_stream << "bulk:" << id << ":" << std::time(time_) << ".log";
    name = out_buffer.str();
  }
  Observer::update(commands);
} 

void FileWriter::print() {
  if (_commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  auto commands = *_commands.lock();
  file.open(name);
  addBlock();
  addCommands(commands.size());
  unlock();
  file << "bulk: ";
  for(auto command = commands.cbegin(); command < commands.cend(); command++) {
    if (command != commands.cbegin())
      file << ", ";
    file << *command;
  }
  file.close();
}