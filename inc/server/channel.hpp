#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include <string>
#include <vector>

using namespace std;

class Channel {
  public:
    string name;
    std::vector<User *> participants;
    Channel(string _name);
};

#endif
