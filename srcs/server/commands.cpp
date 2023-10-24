#include "../../inc/server/server.hpp"
#include <string>

int Server::handleUnLoggedIn(std::string message, int clientSocket) {
  size_t spacePos = message.find(' ');
  if (spacePos == std::string::npos) {
    sendMessage("ERROR: You are not logged in!", clientSocket);
    return -1;
  }
  string command = message.substr(0, spacePos);
  string args = message.substr(spacePos + 1);
  if (command == "/login") {
    spacePos = args.find(' ');
    if (spacePos == std::string::npos) {
      return -1;
    }
    string username = args.substr(0, spacePos);
    string password = args.substr(spacePos + 1);

    user_map[username] = User(username, clientSocket);
    userSocketMap[clientSocket] = &user_map[username];
    return 1;
  } else if (command == "/register") {
    return -1;
  } else {
    sendMessage("ERROR: You are not logged in!", clientSocket);
    return -1;
  }
}

void checkPassword(string password, string passwordConfirm) {}
