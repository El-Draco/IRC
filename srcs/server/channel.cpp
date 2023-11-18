#include "../../inc/server/channel.hpp"
#include <algorithm>
#include <mutex>

Channel::Channel(string _name) : name(_name) {}

Channel::Channel(Channel const &other) {
    std::unique_lock<std::mutex> lock_other(other.participantMutex);
    name = other.name;
    participants = other.participants;
}

Channel &Channel::operator=(Channel const &other) {
    if (&other != this) {
        std::unique_lock<std::mutex> lock_this(participantMutex,
                                               std::defer_lock);
        std::unique_lock<std::mutex> lock_other(other.participantMutex,
                                                std::defer_lock);

        std::lock(lock_this, lock_other);
        name = other.name;
        participants = other.participants;
    }
    return *this;
}

#include <iostream>
void Channel::removeUser(string username) {
    auto it = std::find_if(
        participants.begin(), participants.end(),
        [username](User *element) { return element->name == username; });
    std::lock_guard<std::mutex> lock(participantMutex);
    if (it != participants.end())
        participants.erase(it);
}

void Channel::addUser(User *user) {
    std::lock_guard<std::mutex> lock(participantMutex);
    participants.push_back(user);
}
