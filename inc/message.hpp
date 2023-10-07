#ifndef message_hpp
#define message_hpp

#include <cstddef>
#include <string>

class message {
public:
  std::string sender;
  std::string message;
  std::string channel;
  std::size_t timestamp;
};

#endif // !message_hpp
