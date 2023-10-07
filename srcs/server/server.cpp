#include "../../inc/server/server.hpp"
#include <functional>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <thread>

using namespace std;

server::server(int port) {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    cerr << "Error in socket creation" << endl;
    exit(1);
  }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    cerr << "Error in binding" << endl;
    exit(1);
  }

  if (listen(serverSocket, 5) == 0) {
    cout << "Listening to port" << endl;
  }

  broadcastThread = thread(&server::broadcastHandler, this);

  // read from config file: channel list
  //
  running = true;
}

void server::recieveConnections() {
  while (running) {
    cout << "recieve connections cycle" << endl;
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);

    clientSocket =
        accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);

    if (clientSocket < 0) {
      cerr << "Error in accepting client connection" << endl;
      continue;
    }

    clientSockets.push_back(clientSocket);
    thread clientThread(
        [clientSocket, this]() { this->clientHandler(clientSocket); });
    clientThreads.push_back(std::move(clientThread));
  }
}

void server::broadcastHandler() {
  while (true) {
    string message;
    {
      cout << "broadcast handler cycle" << endl;
      unique_lock<mutex> lock(bufferMutex);
      bufferCV.wait(lock, [this] { return !this->messageBuffer.empty(); });
      message = messageBuffer.front();
      messageBuffer.pop();
    }

    // do something with the message
    // probably call some abstraction to send the message to relevant users
  }
}

void server::clientHandler(int clientSocket) {
  // listen for register/login
  //  when logged in, listen for messages, and add to queue
  cout << "new client handler " << clientSocket;
}
