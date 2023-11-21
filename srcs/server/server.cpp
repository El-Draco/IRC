#include "../../inc/server/server.hpp"
#include "../../inc/server/channel.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

/**
 * @brief Construct a new Server:: Server object
 *
 * @param ipAddress    IP Address to establish the server connection to
 * @param port         Port to bind server to
 */
Server::Server(std::string ipAddress, int port)
    : serverSocket(ipAddress, port) {
    initCommands();
    serverSocket.startConnection();
    serverSocket.startListening();

    broadcastThread = thread(&Server::broadcastHandler, this);

    channelList.push_back(Channel("general"));
    channelList.push_back(Channel("cmp"));
    channelList.push_back(Channel("other"));

    // read from config file: channel list
    running = true;
}

/**
 * @brief		Recieve any incoming data from established socket
 * connections
 *
 */
void Server::recieveConnections() {
    while (running) {
        cout << "Recieved connection " << clientSockets.size() << endl;
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
        Message message;
        {
            unique_lock<mutex> lock(bufferMutex);
            bufferCV.wait(lock,
                          [this] { return !this->messageBuffer.empty(); });
            message = messageBuffer.front();
            messageBuffer.pop();
        }

        for (auto c : message.channel->participants) {
            sendMessage(message.sender + ":" + message.content,
                        c->clientSocket);
        }
    }
}

/**
 * @brief       Thread handler to receive incoming messages from clients and
 * push to the message queue
 *
 * @param clientSocket Socket file descriptor of the client
 */
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
        } else if (checkCommand(message)) {
            handleCommand(message, clientSocket);
        }
        // user is registered, push message to queue
        else {
            string username = userSocketMap.at(clientSocket)->name;
            Message formattedMessage = formatMessage(message, username);
            broadcastMessage(formattedMessage);
        }
    }
}

void Server::broadcastMessage(Message formattedMessage) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    messageBuffer.push(formattedMessage);
    bufferCV.notify_all();
};

/**
 * @brief               Format a message into Message object
 *
 * @param message       The message content
 * @param username      The username of the sender
 */
Message Server::formatMessage(string message, string username) {
    Message formattedMessage;
    formattedMessage.content = message;
    formattedMessage.sender = username;
    formattedMessage.time = std::time(0);
    formattedMessage.channel = [username, this]() {
        for (size_t i = 0; i < channelList.size(); i++) {
            auto c = &channelList[i];
            for (auto p : c->participants) {
                if (p->name == username) {
                    return c;
                }
            }
        }
        return &channelList[0];
    }();
    return formattedMessage;
}

/**
 * @brief               Send a message to a specific client
 *
 * @param message       Message to send
 * @param clientSocket  Socket file descriptor of client
 */
void Server::sendMessage(string message, int clientSocket) {
    send(clientSocket, message.c_str(), strlen(message.c_str()), 0);
}

/**
 * @brief               Close a client socket connection
 *
 * @param clientSocket  client socket fd to close
 */
void Server::closeConnection(int clientSocket) {
    close(clientSocket);
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        clientSockets.erase(std::remove(clientSockets.begin(),
                                        clientSockets.end(), clientSocket),
                            clientSockets.end());
    }
    if (userSocketMap.find(clientSocket) != nullptr) {
        std::lock_guard<std::mutex> lock(mapMutex);
        string username = userSocketMap[clientSocket]->name;
        user_map.erase(username);
        userSocketMap.erase(clientSocket);
        for (size_t i = 0; i < channelList.size(); i++) {
            auto c = &channelList[i];
            c->removeUser(username);
        }
    }
}
