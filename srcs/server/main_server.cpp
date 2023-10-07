#include "../../inc/server/server.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Error: no port number specified" << endl;
    cerr << "Usage: irc_server [port number]" << endl;
    exit(1);
  }

  int portNumber = atoi(argv[1]);
  cout << "Starting server on port " << portNumber << " ..." << endl;

  server myserver(portNumber);
  myserver.recieveConnections();
}
