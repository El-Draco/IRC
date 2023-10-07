#ifndef channel_hpp
#define channel_hpp

#include "User.hpp"
#include <string>
#include <vector>

class channel {
public:
  std::string name;
  std::vector<User *> participants;
};

#endif // !DEBUG
