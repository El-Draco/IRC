#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../TCPSocket.hpp"
#include "tui.hpp"
#include <string>
#include <thread>
using namespace std;

class Client {
  private:
    TCPSocket clientSocket;

  public:
    Client(string ipAddress, int port);
    ~Client();

    tui UI;
    int getClientSocket();
    void sendMessage(string message);
    string recieveMessage();
};

#endif
