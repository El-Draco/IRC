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

Server::Server(std::string ipAddress, int port) : serverSocket(ipAddress, port)
{
	serverSocket.startConnection();
	serverSocket.startListening();

	broadcastThread = thread(&Server::broadcastHandler, this);

	// read from config file: channel list
	//
	running = true;
}

/**
 * @brief		Recieve any incoming data from established socket connections
 * 
 */
void Server::recieveConnections() {
	while (running)
	{
		// cout << "recieve connections cycle" << endl;
		int clientSocket = serverSocket.acceptConnection();

		if (clientSocket < 0) {
			// cerr << "Error in accepting client connection" << endl;
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
 * @brief		Broadcast a message to the channel by popping from the message queue
 * 				to each channel user. Direct messages are not broadcasted through here
 * 
 */
void Server::broadcastHandler() {
	
	// cout << "broadcast handler cycle" << endl;
	while (true) {
		string message;
		{
			unique_lock<mutex> lock(bufferMutex);
			bufferCV.wait(lock, [this]
						  { return !this->messageBuffer.empty(); });
			message = messageBuffer.front();
			messageBuffer.pop();
			for (int socket : clientSockets)
			{
				send(socket, message.c_str(), message.size(), 0);
			}
		}

		// do something with the message
		// probably call some abstraction to send the message to relevant users
	}
}

void Server::clientHandler(int clientSocket) {
	// listen for register/login
	//  when logged in, listen for messages, and add to queue
	// char message[2048];
	cout << "New client handler " << clientSockets.size() << endl;
	(void) clientSocket;
	std::string message;
	while (true)
	{
		std::cout<<"Enter message: ";
		std::cin >> message;
		{
			std::lock_guard<std::mutex> lock(bufferMutex);
			messageBuffer.push(message);
			bufferCV.notify_all();
		}
		// memset(message, 0, sizeof(message));
		// if (recv(clientSocket, message, sizeof(message), 0) <= 0)
		// {
		// 	// client is disconnected
		// 	close(clientSocket);
		// 	{
		// 		std::lock_guard<std::mutex> lock(bufferMutex);
		// 		clientSockets.erase(std::remove(clientSockets.begin(),
		// 										clientSockets.end(), clientSocket),
		// 							clientSockets.end());
		// 	}
		// 	break;
		// }

		// string message = serverSocket.recieveData(clientSocket);

		// if user not registered, login or register
		// if (userSocketMap.find(clientSocket) == nullptr) {
		// }
		// user is registered, push message to queue
		// else {
		// 	std::lock_guard<std::mutex> lock(bufferMutex);
		// 	messageBuffer.push(message);
		// 	bufferCV.notify_all();
		// }

		// cout << message << endl;
	}
}
