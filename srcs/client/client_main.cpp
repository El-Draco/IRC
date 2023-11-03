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
void receiveMessages(Client *c1, tui *UI) {
  string message;
  while (true) {
    message = c1->recieveMessage();
    UI->messageRecieved(message);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "Error: not enough arguements" << endl;
    cerr << "Usage: irc_client [server address] [port number]" << endl;
    exit(1);
  }
  tui UI;

  string ip = argv[1];
  int port = atoi(argv[2]);

  Client c1(ip, port);

  std::thread receiveThread(receiveMessages, &c1, &UI);

  while (true) {
    int message = UI.readInput();
    if (message == 1) {
      c1.sendMessage(UI.message_buffer);
      UI.clearInput();
    };
    // UI.refresh();
  }
  return 0;
}
