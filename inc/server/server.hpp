#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "channel.hpp"
#include <condition_variable>
#include <mutex>
#include <netinet/in.h>
// #include <openssl/rsa.h>
#include "../../inc/TCPSocket.hpp"
#include <queue>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;

class Server
{
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

protected:
    TCPSocket serverSocket;
    std::vector<int> clientSockets;
    std::vector<std::thread> clientThreads;
    std::queue<std::string> messageBuffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCV;
    std::thread broadcastThread;

    int handleMessage(std::string message);
    void sendMessage(std::string message, int clientSocket);
    void closeConnection(int clientSocket);

    const string motd = "Login using /login [username] [password]\n";
};

#endif
