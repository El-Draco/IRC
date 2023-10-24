#include "../../inc/server/server.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

Server::Server(std::string ipAddress, int port)
    : serverSocket(ipAddress, port) {
  serverSocket.startConnection();
  serverSocket.startListening();

  broadcastThread = thread(&Server::broadcastHandler, this);

  // read from config file: channel list
  //
  running = true;
}

/**
 * @brief		Recieve any incoming data from established socket
 * connections
 *
 */
void Server::recieveConnections() {
  while (running) {
    cout << "recieve connections cycle" << clientSockets.size() << endl;
    int clientSocket = serverSocket.acceptConnection();

    if (clientSocket < 0) {
      continue;
    }

    clientSockets.push_back(clientSocket);
    std::thread clientThread(
        [clientSocket, this]() { this->clientHandler(clientSocket); });
    clientThreads.push_back(std::move(clientThread));
  }
}

/**
 * @brief		Broadcast a message to the channel by popping from the
 * message queue to each channel user. Direct messages are not broadcasted
 * through here
 *
 */
void Server::broadcastHandler() {
  while (true) {
    string message;
    {
      unique_lock<mutex> lock(bufferMutex);
      bufferCV.wait(lock, [this] { return !this->messageBuffer.empty(); });
      message = messageBuffer.front();
      messageBuffer.pop();
    }

    for (int socket : clientSockets) {
      sendMessage(message, socket);
    }

    // do something with the message
    // probably call some abstraction to send the message to relevant users
  }
}

void Server::clientHandler(int clientSocket) {
  // listen for register/login
  //  when logged in, listen for messages, and add to queue
  (void)clientSocket;
  send(clientSocket, motd.c_str(), strlen(motd.c_str()), 0);
  std::string message;
  while (true) {
    try {
      message = serverSocket.recieveData(clientSocket);
    }
    // TODO: Add proper catch
    catch (...) {
      closeConnection(clientSocket);
      break;
    }
    if (userSocketMap.find(clientSocket) == nullptr) {
      handleUnLoggedIn(message, clientSocket);
    }
    // user is registered, push message to queue
    else {
      std::lock_guard<std::mutex> lock(bufferMutex);
      messageBuffer.push(message);
      bufferCV.notify_all();
    }
  }
}

void Server::sendMessage(string message, int clientSocket) {
  send(clientSocket, message.c_str(), strlen(message.c_str()), 0);
}

void Server::closeConnection(int clientSocket) {
  close(clientSocket);
  {
    std::lock_guard<std::mutex> lock(bufferMutex);
    clientSockets.erase(
        std::remove(clientSockets.begin(), clientSockets.end(), clientSocket),
        clientSockets.end());
  }
}
