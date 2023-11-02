#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include <string>
#include <vector>

class Channel
{
public:
    std::string name;
    std::vector<User *> participants;
};

#endif
