#include "ConfigCallbacks.hpp"
#include <chrono>
#include <sstream>
#include <unordered_map>

namespace nlm {

// Utility for generating unique callback IDs
namespace {
    static std::unordered_map<std::string, int> idCounters;
    static std::mutex idMutex;
}

std::string CallbackUtils::generateCallbackId() {
    std::lock_guard<std::mutex> lock(idMutex);
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string id = "cb_";
    for (int i = 0; i < 8; ++i) {
        id += chars[rand() % chars.size()];
    }
    return id;
}

// ConfigChangeEvent implementation
ConfigChangeEvent::ConfigChangeEvent(Type type, const std::string& key,
                                     const std::any& oldValue, const std::any& newValue,
                                     const std::string& source, const std::string& message)
    : type(type), key(key), oldValue(oldValue), newValue(newValue), source(source), message(message) {}

bool ConfigChangeEvent::isValid() const {
    return !key.empty() || type != Type::ValueChanged;
}

std::string ConfigChangeEvent::toString() const {
    std::ostringstream oss;
    
    oss << "EventType: ";
    switch (type) {
        case Type::ValueChanged: oss << "ValueChanged"; break;
        case Type::KeyAdded: oss << "KeyAdded"; break;
        case Type::KeyRemoved: oss << "KeyRemoved"; break;
        case Type::ConfigValidated: oss << "ConfigValidated"; break;
        case Type::ConfigSaved: oss << "ConfigSaved"; break;
        case Type::ConfigLoaded: oss << "ConfigLoaded"; break;
    }
    
    oss << " Key: " << key;
    
    if (!message.empty()) {
        oss << " Message: " << message;
    }
    
    return oss.str();
}

} // namespace nlm