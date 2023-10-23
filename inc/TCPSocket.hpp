#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

# include	<iostream>
# include	<string.h>
# include   <vector>
# include	<string>
# include	<sstream>
# include	<sys/socket.h>
# include	<sys/un.h>
# include	<arpa/inet.h>
# include	<fstream>
# include	<unistd.h>
# include <fcntl.h>
#define BUFFER_SIZE 20000

class TCPSocket {
protected:
	std::string ip_address;
	int port;
	int passiveSocket;
	// int peer_socket;
	struct sockaddr_in socket_address;
	unsigned int socket_address_len;

public:
	TCPSocket();
	TCPSocket(std::string ip_addr, int port);
	~TCPSocket();

	static std::string generateDefaultResponse();

	void sendData(int peerSocket, std::string message);
	std::string recieveData(int peerSocket);


	void startConnection();
	void startListening();
	int acceptConnection();
	void closeConnection(int peerSocket);
	int connectToSocket();

	const int &getPassiveSocket();


	class SocketIOError : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};
};

#endif