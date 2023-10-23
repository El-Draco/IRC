#include "../../inc/server/server.hpp"
#include "../../inc/TCPSocket.hpp"

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Error: no port number specified" << endl;
        cerr << "Usage: irc_server [port number]" << endl;
        exit(1);
    }

    std::string ipAddress(argv[1]);
    int portNumber = atoi(argv[2]);
    cout << "Starting server on IP: "<<ipAddress<<" and port: " << portNumber << " ..." << endl;

    Server myserver(ipAddress, portNumber);
    myserver.recieveConnections();
}
