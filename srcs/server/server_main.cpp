#include "../../inc/TCPSocket.hpp"
#include "../../inc/server/server.hpp"

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Error: Invalid usage" << endl;
    cerr << "Usage: irc_server [ip address] [port number]" << endl;
    exit(1);
  }

  std::string ipAddress(argv[1]);
  int portNumber = atoi(argv[2]);
  cout << "Starting server on IP: " << ipAddress << " and port: " << portNumber
       << " ..." << endl;

  Server myserver(ipAddress, portNumber);
  myserver.recieveConnections();
}
