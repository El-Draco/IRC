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
            std::lock_guard<std::mutex> lock(bufferMutex);
            string username = userSocketMap.at(clientSocket)->name;
            Message formattedMessage = formatMessage(message, username);
            messageBuffer.push(formattedMessage);
            bufferCV.notify_all();
        }
    }
}

void Server::print(string message) { cout << message; }

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
                    cout << "Found himmmm" << c->name << endl;
                    return c;
                }
            }
        }
        return &channelList[0];
    }();
    cout << formattedMessage.channel->name << endl;
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

int Server::handleUnLoggedIn(std::string message, int clientSocket) {
    size_t spacePos = message.find(' ');
    if (spacePos == std::string::npos) {
        sendMessage("ERROR: You are not logged in!", clientSocket);
        return -1;
    }
    string command = message.substr(0, spacePos);
    string args = message.substr(spacePos + 1);
    if (command == "/login") {
        spacePos = args.find(' ');
        if (spacePos == std::string::npos) {
            return -1;
        }
        string username = args.substr(0, spacePos);
        string password = args.substr(spacePos + 1);
        if (user_map.find(username) != nullptr) {
            sendMessage("ERROR:Username already in use!\n", clientSocket);
            return -1;
        }

        std::lock_guard<std::mutex> lock(mapMutex);
        user_map[username] = User(username, clientSocket);
        userSocketMap[clientSocket] = &user_map[username];
        channelList[0].addUser(&user_map[username]);
        sendMessage("SERVER:You have successfully logged in\n", clientSocket);
        handleCommand("/help ", clientSocket);
        return 1;
    } else if (command == "/register") {
        return -1;
    } else {
        sendMessage("ERROR: You are not logged in!", clientSocket);
        return -1;
    }
}

void checkPassword(string password, string passwordConfirm) {
    (void)password;
    (void)passwordConfirm;
}

void Server::initCommands() {
    commandMap["/help"] = [this](string, int clientSocket) {
        sendMessage("SERVER:Help:\n"
                    "/help : Prints this help message\n"
                    "/channels : Prints the available channels to join\n"
                    "/join [channel] : Joins the specified channel",
                    clientSocket);
    };
    commandMap["/channels"] = [this](string, int clientSocket) {
        string channels = "SERVER: Channel List:\n";
        for (auto c : channelList) {
            channels += "[" + c.name + "]\n";
        }
        sendMessage(channels, clientSocket);
    };
    commandMap["/join"] = [this](string args, int clientSocket) {
        User *user = userSocketMap[clientSocket];
        bool added = false;
        for (size_t i = 0; i < channelList.size(); i++) {
            auto c = &channelList[i];
            c->removeUser(user->name);

            if (c->name == args) {
                c->addUser(user);
                added = true;
                sendMessage("SERVER: You have joined " + args, clientSocket);
            }
        }
        if (!added) {
            sendMessage(
                "ERROR:Invalid channel name, moved you to general channel.",
                clientSocket);
            channelList[0].addUser(user);
        }

        // sendMessage("SERVER:" + args, clientSocket);
    };
}

bool Server::checkCommand(string message) {
    size_t spacePos = message.find(' ');
    string command = message.substr(0, spacePos);
    return commandMap.find(command) != nullptr;
};

void Server::handleCommand(string message, int clientSocket) {
    size_t spacePos = message.find(' ');
    string command = message.substr(0, spacePos);
    string args = message.substr(spacePos + 1);
    if (commandMap.find(command) != nullptr) {
        commandMap[command](args, clientSocket);
    } else
        sendMessage("ERROR: Invalid command", clientSocket);
}
