#include "ConfigWatcher.hpp"
#include <algorithm>

namespace nlm {

void ConfigWatcher::addListener(std::shared_ptr<ConfigChangeListener> listener) {
    if (listener) {
        listeners.push_back(listener);
    }
}

void ConfigWatcher::removeListener(std::shared_ptr<ConfigChangeListener> listener) {
    if (listener) {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }
}

void ConfigWatcher::notifyChange(const std::string& key, const std::any& oldValue, const std::any& newValue) {
    for (const auto& listener : listeners) {
        if (listener) {
            try {
                listener->onConfigChanged(key, oldValue, newValue);
            } catch (const std::exception&) {
                // Ignore exceptions in listeners
            }
        }
    }
}

void ConfigWatcher::notifyAdd(const std::string& key, const std::any& value) {
    for (const auto& listener : listeners) {
        if (listener) {
            try {
                listener->onConfigAdded(key, value);
            } catch (const std::exception&) {
                // Ignore exceptions in listeners
            }
        }
    }
}

void ConfigWatcher::notifyRemove(const std::string& key) {
    for (const auto& listener : listeners) {
        if (listener) {
            try {
                listener->onConfigRemoved(key);
            } catch (const std::exception&) {
                // Ignore exceptions in listeners
            }
        }
    }
}

} // namespace nlm