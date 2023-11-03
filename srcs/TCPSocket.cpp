#include "../inc/TCPSocket.hpp"

/**
 * @brief       Helper function for debugging purposes
 *
 * @param mesg  Message to log onto the console
 */
static void log(std::string mesg) { std::cout << mesg << std::endl; }

/**
 * @brief Construct a Server Socket (TCP conenction) with default values
 *
 */
TCPSocket::TCPSocket() {
  socket_address.sin_family = AF_INET;
  socket_address.sin_port = htons(port);
  socket_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
}

/**
 * @brief Construct a Server Socket (TCP conenction) with the given parameters
 *
 * @param ip_addr       IP address to access docket
 * @param port          Port to bind the socket to
 */
TCPSocket::TCPSocket(std::string ip_addr, int port)
    : ip_address(ip_addr), port(port), passiveSocket(), socket_address(),
      socket_address_len(sizeof(socket_address)) {
  socket_address.sin_family = AF_INET;
  socket_address.sin_port = htons(port);
  socket_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
}

TCPSocket::~TCPSocket() { close(passiveSocket); }

/**
 * @brief               Read data from a given socket (connection already
 * accepted)
 *
 * @param peerSocket    Socket to read data from
 * @return std::string  Message receieved
 */
std::string TCPSocket::recieveData(int peerSocket) {
  long bytesRecieved;
  char buffer[BUFFER_SIZE] = {0};

  // replace read with receieve
  bytesRecieved = recv(peerSocket, buffer, BUFFER_SIZE, 0);
  if (bytesRecieved <= 0)
    throw "Client Disconnected";

  return (std::string(buffer));
}

/**
 * @brief               Send data to a socket
 *
 * @param peerSocket    Socket to send data to
 * @param message       Data to be sent
 */
void TCPSocket::sendData(int peerSocket, std::string message) {
  size_t bytesSent;
  // replace write with SEND function
  const char *s = message.c_str();
  std::cerr << message;
  bytesSent = send(peerSocket, s, message.size(), 0);
  if (bytesSent != message.size())
    log("Error sending response to client");
}

/**
 * @brief               Open a socket to read and write to
 *
 */
void TCPSocket::startConnection() {
  passiveSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (passiveSocket < 0) {
    log("Socket creation failed\n");
    return;
  }
  if (bind(passiveSocket, (sockaddr *)&socket_address, socket_address_len) <
      0) {
    log("Cannot connect socket to address");
    return;
  }
  std::cout << "Socket connection created and started succesfully!\n";
}

/**
 * @brief               Start listening from the created socket
 *
 */
void TCPSocket::startListening() {
  if (listen(passiveSocket, 20) < 0) {
    log("Socket listen failed\n");
  }
  std::ostringstream ss;
  ss << "\n*** Listening on ADDRESS: " << inet_ntoa(socket_address.sin_addr)
     << " PORT: " << ntohs(socket_address.sin_port) << " ***\n\n";
  log(ss.str());
}

/**
 * @brief               Accept an incoming connection to the listening server
 * socket
 *
 * @return int
 */
int TCPSocket::acceptConnection() {
  int peerSocket =
      accept(passiveSocket, (sockaddr *)&socket_address, &socket_address_len);
  if (peerSocket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(socket_address.sin_addr)
       << "; PORT: " << ntohs(socket_address.sin_port);
    log(ss.str());
  }
  return (peerSocket);
}

int TCPSocket::connectToSocket() {
  passiveSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (passiveSocket < 0) {
    log("Socket creation failed\n");
    return (-1);
  }
  if (connect(passiveSocket, (struct sockaddr *)&socket_address,
              sizeof(socket_address)) < 0) {
    std::cerr << "Error in connection" << std::endl;
    return (-1);
  }
  return (0);
}

/**
 * @brief               Close a peer socket after sending/recieving data
 *
 * @param peerSocket    Peer socket to close
 */
void TCPSocket::closeConnection(int peerSocket) { close(peerSocket); }

/**
 * @brief               Getter to retrieve the server socket file descriptor
 *
 * @return const int&
 */
const int &TCPSocket::getPassiveSocket() { return (passiveSocket); }

/**
 * @brief               overridden what method for the server socket exception
 * class
 *
 * @return const char*
 */
const char *TCPSocket::SocketIOError::what() const throw() {
  return ("Socket IO error!\n");
}
