#include "../../inc/client/client.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
using namespace std;

void receiveMessages(Client *c1) {
  string message;
  while (true) {
    message = c1->recieveMessage();
    std::cout << "\nReceived: " << message << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Error: not enough arguements" << endl;
    cerr << "Usage: irc_client [server address] [port number]" << endl;
    exit(1);
  }

  string ip = argv[1];
  int port = atoi(argv[2]);

  Client c1(ip, port);

  std::thread receiveThread(receiveMessages, &c1);

  string message;
  while (2) {
    std::cout << "Enter a message: ";
    getline(cin, message);
    c1.sendMessage(message);
  }
  return 0;
}
