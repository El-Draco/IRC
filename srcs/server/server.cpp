#include "../../inc/server/server.hpp"
#include <arpa/inet.h>
#include <cstring>
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
    : serverSocket(ipAddress, port)
{
    serverSocket.startConnection();
    serverSocket.startListening();

    broadcastThread = thread(&Server::broadcastHandler, this);

    // read from config file: channel list
    running = true;
}

/**
 * @brief		Recieve any incoming data from established socket
 * connections
 *
 */
void Server::recieveConnections()
{
    while (running)
    {
        cout << "Recieved connection " << clientSockets.size() << endl;
        int clientSocket = serverSocket.acceptConnection();

        if (clientSocket < 0)
        {
            continue;
        }

        clientSockets.push_back(clientSocket);
        std::thread clientThread(
            [clientSocket, this]()
            { this->clientHandler(clientSocket); });
        clientThreads.push_back(std::move(clientThread));
    }
}

/**
 * @brief		Broadcast a message to the channel by popping from the
 * message queue to each channel user. Direct messages are not broadcasted
 * through here
 *
 */
void Server::broadcastHandler()
{
    while (true)
    {
        string message;
        {
            unique_lock<mutex> lock(bufferMutex);
            bufferCV.wait(lock, [this]
                          { return !this->messageBuffer.empty(); });
            message = messageBuffer.front();
            messageBuffer.pop();
        }

        for (int socket : clientSockets)
        {
            sendMessage(message, socket);
        }

        // do something with the message
        // probably call some abstraction to send the message to relevant users
    }
}

/**
 * @brief       Thread handler to receive incoming messages from clients and push to the message queue
 *
 * @param clientSocket Socket file descriptor of the client
 */
void Server::clientHandler(int clientSocket)
{
    // listen for register/login
    //  when logged in, listen for messages, and add to queue
    (void)clientSocket;
    send(clientSocket, motd.c_str(), strlen(motd.c_str()), 0);
    std::string message;
    while (true)
    {
        try
        {
            message = serverSocket.recieveData(clientSocket);
        }
        // TODO: Add proper catch
        catch (...)
        {
            closeConnection(clientSocket);
            break;
        }
        if (userSocketMap.find(clientSocket) == nullptr)
        {
            handleUnLoggedIn(message, clientSocket);
        }
        // user is registered, push message to queue
        else
        {
            std::lock_guard<std::mutex> lock(bufferMutex);
            messageBuffer.push(message);
            bufferCV.notify_all();
        }
    }
}

/**
 * @brief               Send a message to a specific client
 *
 * @param message       Message to send
 * @param clientSocket  Socket file descriptor of client
 */
void Server::sendMessage(string message, int clientSocket)
{
    send(clientSocket, message.c_str(), strlen(message.c_str()), 0);
}

/**
 * @brief               Close a client socket connection
 *
 * @param clientSocket  client socket fd to close
 */
void Server::closeConnection(int clientSocket)
{
    close(clientSocket);
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        clientSockets.erase(
            std::remove(clientSockets.begin(), clientSockets.end(), clientSocket),
            clientSockets.end());
    }
}

int Server::handleUnLoggedIn(std::string message, int clientSocket)
{
    size_t spacePos = message.find(' ');
    if (spacePos == std::string::npos)
    {
        sendMessage("ERROR: You are not logged in!", clientSocket);
        return -1;
    }
    string command = message.substr(0, spacePos);
    string args = message.substr(spacePos + 1);
    if (command == "/login")
    {
        spacePos = args.find(' ');
        if (spacePos == std::string::npos)
        {
            return -1;
        }
        string username = args.substr(0, spacePos);
        string password = args.substr(spacePos + 1);

        user_map[username] = User(username, clientSocket);
        userSocketMap[clientSocket] = &user_map[username];
        return 1;
    }
    else if (command == "/register")
    {
        return -1;
    }
    else
    {
        sendMessage("ERROR: You are not logged in!", clientSocket);
        return -1;
    }
}

void checkPassword(string password, string passwordConfirm)
{
    (void)password;
    (void)passwordConfirm;
}