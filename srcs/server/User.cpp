#include "../../inc/server/User.hpp"
#include <string>

using namespace std;

User::User(){

};

User::User(string _username, int _clientSocket)
{
    name = _username;
    clientSocket = _clientSocket;
}
