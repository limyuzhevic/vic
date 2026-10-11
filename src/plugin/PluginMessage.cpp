// Plugin message implementation for NLM

#include "PluginMessage.hpp"
#include <chrono>

namespace nlm {

PluginMessage::PluginMessage() : timestamp(0.0) {
}

PluginMessage::PluginMessage(const std::string& sender, const std::string& receiver,
                             const std::string& type, const std::string& category)
    : sender(sender), receiver(receiver), type(type), category(category) {
    setTimestamp();
}

void PluginMessage::setTimestamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    timestamp = std::chrono::duration<double>(duration).count();
}

double PluginMessage::getTimestamp() const {
    return timestamp;
}

template<typename T>
T PluginMessage::getData(const std::string& key) const {
    // Template specialization for basic types
    if constexpr (std::is_same_v<T, std::string>) {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        auto it = numericData.find(key);
        if (it != numericData.end()) {
            return it->second;
        }
    }
    return T();
}

template<typename T>
void PluginMessage::setData(const std::string& key, const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        data[key] = value;
    } else if constexpr (std::is_same_v<T, double>) {
        numericData[key] = value;
    }
}

double PluginMessage::getNumericData(const std::string& key) const {
    auto it = numericData.find(key);
    if (it != numericData.end()) {
        return it->second;
    }
    return 0.0;
}

void PluginMessage::setNumericData(const std::string& key, double value) {
    numericData[key] = value;
}

std::vector<std::string> PluginMessage::getStringListData(const std::string& key) const {
    auto it = stringListData.find(key);
    if (it != stringListData.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

void PluginMessage::setStringListData(const std::string& key, const std::vector<std::string>& value) {
    stringListData[key] = value;
}

const std::string& PluginMessage::getPayload() const {
    return payload;
}

void PluginMessage::setPayload(const std::string& payload) {
    this->payload = payload;
}

} // namespace nlm
