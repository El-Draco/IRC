#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../TCPSocket.hpp"
#include <string>
using namespace std;

class Client
{
private:
    TCPSocket clientSocket;
public:
    Client(string ipAddress, int port);
    ~Client();

    int getClientSocket();
    void sendMessage(string message);

};




#endif