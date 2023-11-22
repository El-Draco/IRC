#include "../../inc/server/server.hpp"
#include <mutex>
// #include <fstream>
// #include <sstream>
// #include <string.h>

int Server::handleUnLoggedIn(std::string message, int clientSocket) {
    size_t spacePos = message.find(' ');
    if (spacePos == std::string::npos) {
        sendMessage("ERROR: You are not logged in!", clientSocket);
        return -1;
    }
    string command = message.substr(0, spacePos);
    string args = message.substr(spacePos + 1);
    spacePos = args.find(' ');
    if (spacePos == std::string::npos) {
        return -1;
    }
    string username = args.substr(0, spacePos);
    string password = args.substr(spacePos + 1);

    if (command == "/login") {
        if (user_map.find(username) != nullptr) {
            sendMessage("ERROR:Already logged in on different client!\n",
                        clientSocket);
            return -1;
        }

        mapMutex.lock();
        ifstream db("userDB.data");
        string line, valUsername, valPassword, valRole;
        Role theRole;
        bool loggedIn = false;
        while (getline(db, line)) {
            stringstream ss(line);

            getline(ss, valUsername, ',');
            getline(ss, valPassword, ',');
            getline(ss, valRole);
            if (valUsername != username)
                continue;
            if (valPassword != password)
                continue;
            theRole = static_cast<Role>(stoi(valRole));
            loggedIn = true;
            break;
        }
        // find match in some file
        mapMutex.unlock();

        if (!loggedIn) {
            sendMessage("ERROR:Invalid Credentials.\n", clientSocket);

            return -1;
        }

        mapMutex.lock();
        user_map[username] = User(username, clientSocket, theRole);
        userSocketMap[clientSocket] = &user_map[username];
        channelList[0].addUser(&user_map[username]);
        mapMutex.unlock();
        sendMessage("SERVER:You have successfully logged in", clientSocket);
        handleCommand("/help ", clientSocket);
        return 1;
    } else if (command == "/register") {
        std::lock_guard<std::mutex> lock(mapMutex);
        ofstream db("userDB.data", ios_base::app);
        db << username << "," << password << "," << Role::USER << endl;
        db.close();

        sendMessage("SERVER:You have successfully registered! Use /login "
                    "[username] [password] to login.\n",
                    clientSocket);

        return 2;
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
                    "/join [channel] : Joins the specified channel\n"
                    "/private [username] [message] : Sends a private message "
                    "to a user ",
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
    commandMap["/kick"] = [this](string args, int clientSocket) {
        if (userSocketMap[clientSocket]->role != USER) {
            if (user_map.find(args) != nullptr) {
                broadcastMessage(formatMessage(
                    "User " + args + " has been kicked.", "SERVER"));
                sendMessage("SERVER:You have been kicked. Enter a message or "
                            "Ctrl-C to quit.",
                            user_map.find(args)->second.clientSocket);
                closeConnection(user_map.find(args)->second.clientSocket);
            } else {
                sendMessage("ERROR:No such user " + args, clientSocket);
            }
        } else {
            sendMessage("ERROR:You are not permitted to kick.", clientSocket);
        }
    };
    commandMap["/private"] = [this](string args, int clientSocket) {
        size_t spacePos = args.find(' ');
        string user = args.substr(0, spacePos);
        string msg = args.substr(spacePos + 1);
        msg = userSocketMap[clientSocket]->name + " (private):" + msg;
        if (user_map.find(user) != nullptr) {
            int sock = user_map.find(user)->second.clientSocket;
            sendMessage(msg, sock);
        } else {
            sendMessage("ERROR:Could not find user: " + user, clientSocket);
        }
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
