#include "../../inc/server/channel.hpp"
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

void Channel::removeUser(string username) {
    std::lock_guard<std::mutex> lock(participantMutex);
    for (auto p = participants.begin(); p != participants.end(); p++) {
        if ((*p.base())->name == username) {
            participants.erase(p);
            return;
        }
    }
}
