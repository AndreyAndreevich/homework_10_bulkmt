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

void FileWriter::update(const std::weak_ptr<Commands>& commands) {
  if (commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  if (commands.lock()->size() == 1) {
    std::stringbuf out_buffer;
    std::ostream out_stream(&out_buffer);
    auto time_ = time;
    auto current_time = std::time(&time);
    if (time_ == current_time) {
      section++;
    } else {
      section = 0;
    }
    out_stream << "bulk:" << id << ":" ;
    if (section) {
      out_stream << section << ":";
    }
    out_stream << current_time << ".log";
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
  file << "bulk: ";
  for(auto command = commands.cbegin(); command < commands.cend(); command++) {
    if (command != commands.cbegin())
      file << ", ";
    file << *command;
  }
  file.close();
}

std::string FileWriter::getName() {
  return name;
}

std::time_t FileWriter::getTime() {
  return time;
}