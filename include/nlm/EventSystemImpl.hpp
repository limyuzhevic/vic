#pragma once

#include "EventSystem.hpp"
#include <stdexcept>
#include <algorithm>

namespace nlm {

void EventSystem::emit(const std::string& event, const std::string& data) {
    auto it = subscriptions.find(event);
    if (it != subscriptions.end()) {
        for (const auto& callback : it->second) {
            callback(event, data);
        }
    }
}

void EventSystem::subscribe(const std::string& event, EventCallback callback) {
    if (!callback) {
        throw std::invalid_argument("Callback cannot be null");
    }
    subscriptions[event].push_back(callback);
}

void EventSystem::unsubscribe(const std::string& event, EventCallback callback) {
    auto it = subscriptions.find(event);
    if (it != subscriptions.end()) {
        it->second.erase(
            std::remove(it->second.begin(), it->second.end(), callback),
            it->second.end()
        );
        if (it->second.empty()) {
            subscriptions.erase(it);
        }
    }
}

void EventSystem::clear(const std::string& event) {
    subscriptions.erase(event);
}

bool EventSystem::hasSubscribers(const std::string& event) const {
    return subscriptions.find(event) != subscriptions.end() && !subscriptions.at(event).empty();
}

std::vector<std::string> EventSystem::getEvents() const {
    std::vector<std::string> events;
    events.reserve(subscriptions.size());
    for (const auto& pair : subscriptions) {
        events.push_back(pair.first);
    }
    return events;
}
}