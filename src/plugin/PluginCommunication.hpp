#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "PluginBase.hpp"
#include "PluginMessage.hpp"

namespace nlm {

class PluginCommunication {
public:
    PluginCommunication();
    ~PluginCommunication();
    
    // Message sending
    void sendMessage(const PluginMessage& message);
    void broadcastMessage(const PluginMessage& message);
    
    // Message receiving
    void registerMessageHandler(const std::string& messageType, 
                                std::function<void(const PluginMessage&)> handler);
    void unregisterMessageHandler(const std::string& messageType);
    
    // Message queue management
    void processMessageQueue();
    size_t getMessageQueueSize() const;
    
    // Message filtering
    void addMessageFilter(const std::string& sender, const std::string& receiver);
    void removeMessageFilter(const std::string& sender, const std::string& receiver);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
