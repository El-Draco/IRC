#ifndef USER_CLASS
#define USER_CLASS

#include <string>

enum Role { ADMIN, MODERATOR, USER };

class User {
public:
  std::string name;
  Role role;
  int clientSocket;

  User();
  User(std::string _username, int _clientSocket);
};

#endif
