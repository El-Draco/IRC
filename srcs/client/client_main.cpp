#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include "../../inc/client/client.hpp"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Error: not enough arguements" << endl;
        cerr << "Usage: irc_client [server address] [port number]" << endl;
        exit(1);
    }

    string ip = argv[1];
    int port = atoi(argv[2]);

    Client c1(ip, port);
    
    char buff[1024];
    while (2)
    {
        memset(buff, 0, sizeof(buff));
        recv(c1.getClientSocket(), buff, 1024, 0);
        std::cout << "recieved: "<< buff << endl;
    }
    return 0;
}
