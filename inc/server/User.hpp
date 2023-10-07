#ifndef USER_CLASS
#define USER_CLASS

#include <string>

enum Role { ADMIN, MODERATOR, USER };

class User {
public:
  std::string name;
  Role role;
  void sendMessage(std::string message);

  User();
  User(std::string username, char *_pub_key, long _pub_key_len);

private:
  char *pub_key;
  long pub_key_len;
};

#endif
