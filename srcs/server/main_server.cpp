#include "../../inc/server/server.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  server myserver(12345);
  cout << "Created server" << endl;
  myserver.recieveConnections();
}
