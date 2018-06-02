#include "helper.h"

std::string name(const std::weak_ptr<std::time_t>& time, const int& id) {
    std::stringbuf out_buffer;
    std::ostream out_stream(&out_buffer);
    out_stream << "bulk:" << id << ":" << *time.lock() << ".log";
    return out_buffer.str();
}