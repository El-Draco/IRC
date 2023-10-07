#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Error: not enough arguements" << endl;
    cerr << "Usage: irc_client [server address] [port number]" << endl;
    exit(1);
  }

  string serverIp = argv[1];
  int portNumber = atoi(argv[2]);

  // just to test

  int clientSocket;
  struct sockaddr_in serverAddr;
  char message[1024];

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    std::cerr << "Error in socket creation" << std::endl;
    return 1;
  }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portNumber);
  serverAddr.sin_addr.s_addr = inet_addr(serverIp.c_str());

  if (connect(clientSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) < 0) {
    std::cerr << "Error in connection" << std::endl;
    return 1;
  }
}
