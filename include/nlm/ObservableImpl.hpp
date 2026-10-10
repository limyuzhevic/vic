#pragma once

#include "CommonBaseClasses.hpp"
#include <stdexcept>

namespace nlm {

// Implementation of Observable::Observer
void Observable::addObserver(ObserverPtr observer) {
    if (!observer) {
        throw std::invalid_argument("Observer cannot be null");
    }
    observers.push_back(observer);
}

void Observable::removeObserver(ObserverPtr observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void Observable::notifyObservers(const std::string& event, const std::string& data) {
    for (const auto& observer : observers) {
        observer->update(event, data);
    }
}
}