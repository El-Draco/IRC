#include "../../inc/client/client.hpp"
#include "../../inc/client/tui.hpp"
#include <arpa/inet.h>
#include <curses.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
using namespace std;

/**
 * @brief       Thread handler to recieve incoming messages relayed by the
 * server
 *
 * @param c1    Pointer to client object
 */
void receiveMessages(Client *c1) {
    string message;
    while (true) {
        message = c1->recieveMessage();
        c1->UI.messageRecieved(message);
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

    while (true) {
        int message = c1.UI.readInput();
        if (message == 1) {
            c1.sendMessage(c1.UI.message_buffer);
            c1.UI.clearInput();
        };
        // UI.refresh();
    }
    receiveThread.join();
    return 0;
}
