#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "channel.hpp"
#include <condition_variable>
#include <mutex>
#include <netinet/in.h>
#include <openssl/rsa.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>
#include <vector>

class server {
public:
  std::unordered_map<std::string, User> user_map;

  void clientHandler(int clientSocket);
  void broadcastHandler();
  void recieveConnections();

  std::vector<channel> channelList;

  server(int port);

  bool running = false;

private:
  struct sockaddr_in serverAddr;
  int serverSocket;
  std::vector<int> clientSockets;
  std::vector<std::thread> clientThreads;
  std::queue<std::string> messageBuffer;
  std::mutex bufferMutex;
  std::condition_variable bufferCV;

  std::thread broadcastThread;

  int handleMessage(std::string message);

  bool login(std::string username, std::string password, int clientSocket);
  bool registerUser(std::string username, std::string password,
                    int clientSocket);
};

#endif
