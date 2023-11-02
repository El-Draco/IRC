#include "../../inc/client/client.hpp"

/**
 * @brief           Construct a new Client:: Client object
 *
 * @param ipAddress IP address of the server
 * @param port      Port number to connect to
 */
Client::Client(string ipAddress, int port) : clientSocket(ipAddress, port)
{
    clientSocket.connectToSocket();
}

/**
 * @brief Destroy the Client:: Client object
 *
 */
Client::~Client() {}

/**
 * @brief       Get the Socket file descriptor of the client
 *
 * @return int  socket file descriptor
 */
int Client::getClientSocket()
{
    return (clientSocket.getPassiveSocket());
}

/**
 * @brief           Send a message to other clients connected to the server
 *
 * @param message   Message to relay
 */
void Client::sendMessage(string message)
{
    clientSocket.sendData(clientSocket.getPassiveSocket(), message);
}

/**
 * @brief           Recieve messages relayed by the server to the client
 *
 * @return string   Message recieved from the server
 */
string Client::recieveMessage()
{
    string message;
    try
    {
        message = clientSocket.recieveData(clientSocket.getPassiveSocket());
    }
    catch (...)
    {
        cerr << "Server disconnected";
        exit(-1);
    }
    return message;
}
