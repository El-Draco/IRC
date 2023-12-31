#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "channel.hpp"
#include <bits/types/time_t.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <netinet/in.h>
// #include <openssl/rsa.h>
#include "../../inc/TCPSocket.hpp"
#include <queue>
#include <shared_mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;

struct Message {
    string content;
    Channel *channel;
    string sender;
    time_t time;
};

class Server {
  public:
    std::unordered_map<std::string, User> user_map;
    std::unordered_map<int, User *> userSocketMap;

    void clientHandler(int clientSocket);
    void broadcastHandler();
    void recieveConnections();
    int handleUnLoggedIn(std::string message, int clientSocket);

    std::vector<Channel> channelList;

    Server(string ipAddress, int port);

    bool running = false;
    void print(string message);

  protected:
    TCPSocket serverSocket;
    std::vector<int> clientSockets;
    std::vector<std::thread> clientThreads;
    std::queue<Message> messageBuffer;
    std::mutex bufferMutex, mapMutex;
    std::condition_variable bufferCV;
    std::thread broadcastThread;

    int handleMessage(std::string message);
    Message formatMessage(string message, string username);
    void sendMessage(std::string message, int clientSocket);
    void broadcastMessage(Message message);
    void closeConnection(int clientSocket);

    std::unordered_map<string, std::function<void(string, int)>> commandMap;
    void initCommands();
    bool checkCommand(string message);
    void handleCommand(string message, int clientSocket);

    const string motd = "SERVER:Login using /login [username] [password]\n "
                        "Register using /register [username] [password]";
};

#endif
