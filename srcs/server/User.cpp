#include "../../inc/server/User.hpp"
#include <string>

using namespace std;

User::User(){

};

User::User(string _username, int _clientSocket, Role _role) {
    name = _username;
    clientSocket = _clientSocket;
    role = _role;
}
