#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include <mutex>
#include <string>
#include <vector>

using namespace std;

class Channel {
  public:
    string name;
    std::vector<User *> participants;
    Channel(string _name);
    Channel(Channel const &other);
    Channel &operator=(Channel const &other);
    void removeUser(string username);
    void addUser(User *user);

  protected:
    mutable std::mutex participantMutex;
};

#endif
