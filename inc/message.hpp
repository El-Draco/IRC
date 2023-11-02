#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstddef>
#include <string>

class Message
{
public:
    std::string sender;
    std::string message;
    std::string channel;
    std::size_t timestamp;
};

#endif
