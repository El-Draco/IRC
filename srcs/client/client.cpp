#include "../../inc/client/client.hpp"

Client::Client(string ipAddress, int port) : clientSocket(ipAddress, port) {
  clientSocket.connectToSocket();
}

Client::~Client() {}

int Client::getClientSocket() { return (clientSocket.getPassiveSocket()); }

void Client::sendMessage(string message) {
  clientSocket.sendData(clientSocket.getPassiveSocket(), message);
}

string Client::recieveMessage() {
  string message;
  try {
    message = clientSocket.recieveData(clientSocket.getPassiveSocket());
  } catch (...) {
    cerr << "Server disconnected";
    exit(-1);
  }
  return message;
}
