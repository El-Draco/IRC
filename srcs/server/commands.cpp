#include "../../inc/server/server.hpp"

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
