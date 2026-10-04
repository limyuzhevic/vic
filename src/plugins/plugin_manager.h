// Plugin Manager Implementation
// Core plugin management functionality for NLM
// This file contains the main PluginManager implementation that handles
// plugin lifecycle, loading, configuration, and communication

#pragma once

#include "src/plugins/plugin_architecture.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>

namespace nlm {
namespace plugins {

// Forward declarations
class PluginManager;

// Plugin state transition handler
class PluginStateManager {
private:
    std::map<std::string, PluginState> pluginStates;
    std::map<std::string, std::chrono::system_clock::time_point> stateChangeTimes;
    std::map<std::string, std::string> stateChangeReasons;
    
    // State transition callbacks
    std::map<PluginState, std::function<bool(const std::string&, const std::map<std::string, std::any>&)>>
        stateTransitionHandlers;
    
public:
    PluginStateManager() {}
    
    bool setState(const std::string& pluginName, PluginState newState, 
                  const std::string& reason = "") {
        auto it = pluginStates.find(pluginName);
        if (it == pluginStates.end()) {
            return false;
        }
        
        PluginState oldState = it->second;
        
        // Check if state transition is valid
        if (!isValidStateTransition(oldState, newState)) {
            return false;
        }
        
        // Execute pre-transition handler if available
        auto handlerIt = stateTransitionHandlers.find(oldState);
        if (handlerIt != stateTransitionHandlers.end()) {
            std::map<std::string, std::any> handlerData;
            handlerData["plugin_name"] = pluginName;
            handlerData["new_state"] = static_cast<int>(newState);
            handlerData["reason"] = reason;
            
            if (!handlerIt->second(pluginName, handlerData)) {
                return false; // Handler rejected transition
            }
        }
        
        // Update state
        it->second = newState;
        stateChangeTimes[pluginName] = std::chrono::system_clock::now();
        stateChangeReasons[pluginName] = reason;
        
        // Execute post-transition handler if available
        handlerIt = stateTransitionHandlers.find(newState);
        if (handlerIt != stateTransitionHandlers.end()) {
            std::map<std::string, std::any> handlerData;
            handlerData["plugin_name"] = pluginName;
            handlerData["old_state"] = static_cast<int>(oldState);
            handlerData["reason"] = reason;
            
            if (!handlerIt->second(pluginName, handlerData)) {
                // Post-transition handler failed, but transition already happened
                // Log error but don't revert
            }
        }
        
        return true;
    }
    
    PluginState getState(const std::string& pluginName) const {
        auto it = pluginStates.find(pluginName);
        return (it != pluginStates.end()) ? it->second : PluginState::UNLOADED;
    }
    
    std::chrono::system_clock::time_point getLastStateChange(const std::string& pluginName) const {
        auto it = stateChangeTimes.find(pluginName);
        return (it != stateChangeTimes.end()) ? it->second : std::chrono::system_clock::time_point();
    }
    
    std::string getStateChangeReason(const std::string& pluginName) const {
        auto it = stateChangeReasons.find(pluginName);
        return (it != stateChangeReasons.end()) ? it->second : "";
    }
    
    bool isInState(const std::string& pluginName, PluginState state) const {
        return getState(pluginName) == state;
    }
    
    bool isLoaded(const std::string& pluginName) const {
        return isInState(pluginName, PluginState::LOADED) || 
               isInState(pluginName, PluginState::RUNNING);
    }
    
    bool isRunning(const std::string& pluginName) const {
        return isInState(pluginName, PluginState::RUNNING);
    }
    
    bool isError(const std::string& pluginName) const {
        return isInState(pluginName, PluginState::ERROR);
    }
    
    void registerStateTransitionHandler(PluginState state,
        std::function<bool(const std::string&, const std::map<std::string, std::any>&)> handler) {
        stateTransitionHandlers[state] = std::move(handler);
    }
    
    bool removeStateTransitionHandler(PluginState state) {
        auto it = stateTransitionHandlers.find(state);
        if (it != stateTransitionHandlers.end()) {
            stateTransitionHandlers.erase(it);
            return true;
        }
        return false;
    }
    
private:
    bool isValidStateTransition(PluginState from, PluginState to) const {
        static const std::map<PluginState, std::vector<PluginState>> validTransitions = {
            {PluginState::UNLOADED, {PluginState::LOADING, PluginState::ERROR}},
            {PluginState::LOADING, {PluginState::LOADED, PluginState::ERROR, PluginState::UNLOADED}},
            {PluginState::LOADED, {PluginState::RUNNING, PluginState::STOPPING, PluginState::ERROR}},
            {PluginState::RUNNING, {PluginState::STOPPING, PluginState::PAUSED, PluginState::ERROR}},
            {PluginState::STOPPING, {PluginState::LOADED, PluginState::ERROR}},
            {PluginState::PAUSED, {PluginState::RUNNING, PluginState::STOPPING, PluginState::ERROR}},
            {PluginState::ERROR, {PluginState::LOADED, PluginState::UNLOADED, PluginState::STOPPING}},
            {PluginState::UNLOADING, {PluginState::UNLOADED, PluginState::ERROR}}
        };
        
        auto fromIt = validTransitions.find(from);
        if (fromIt == validTransitions.end()) {
            return false;
        }
        
        return std::find(fromIt->second.begin(), fromIt->second.end(), to) != fromIt->second.end();
    }
};

// Plugin message system
class PluginMessageSystem {
private:
    struct Message {
        std::string id;
        std::string fromPlugin;
        std::string toPlugin;
        std::string type;
        std::map<std::string, std::any> data;
        std::chrono::system_clock::time_point timestamp;
        bool processed;
        
        Message(const std::string& from, const std::string& to, 
               const std::string& msgType, const std::map<std::string, std::any>& messageData)
            : fromPlugin(from), toPlugin(to), type(msgType), data(messageData), 
              timestamp(std::chrono::system_clock::now()), processed(false) {
            // Generate unique message ID
            static std::atomic<int> messageCounter(0);
            id = "msg_" + std::to_string(++messageCounter);
        }
    };
    
    std::vector<Message> messageQueue;
    std::map<std::string, std::vector<Message>> pluginIncomingMessages;
    std::map<std::string, std::vector<Message>> pluginOutgoingMessages;
    std::mutex messageMutex;
    
public:
    PluginMessageSystem() : messageQueue(), pluginIncomingMessages(), pluginOutgoingMessages() {}
    
    std::string sendMessage(const std::string& fromPlugin, const std::string& toPlugin,
                           const std::string& type, const std::map<std::string, std::any>& data = {}) {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        Message message(fromPlugin, toPlugin, type, data);
        messageQueue.push_back(message);
        
        // Add to outgoing messages
        pluginOutgoingMessages[fromPlugin].push_back(message);
        
        // Add to incoming messages
        pluginIncomingMessages[toPlugin].push_back(message);
        
        return message.id;
    }
    
    std::vector<Message> getMessages(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        std::vector<Message> result;
        
        // Get incoming messages
        auto inIt = pluginIncomingMessages.find(pluginName);
        if (inIt != pluginIncomingMessages.end()) {
            result.insert(result.end(), inIt->second.begin(), inIt->second.end());
        }
        
        return result;
    }
    
    std::vector<Message> getMessagesByType(const std::string& pluginName, const std::string& type) const {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        std::vector<Message> result;
        
        auto messages = getMessages(pluginName);
        for (const auto& msg : messages) {
            if (msg.type == type) {
                result.push_back(msg);
            }
        }
        
        return result;
    }
    
    bool markMessageProcessed(const std::string& messageId) {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        for (auto& msg : messageQueue) {
            if (msg.id == messageId) {
                msg.processed = true;
                return true;
            }
        }
        
        return false;
    }
    
    std::vector<Message> getUnprocessedMessages(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        std::vector<Message> result;
        auto messages = getMessages(pluginName);
        for (const auto& msg : messages) {
            if (!msg.processed) {
                result.push_back(msg);
            }
        }
        
        return result;
    }
    
    void cleanupProcessedMessages() {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        // Remove processed messages from queue
        messageQueue.erase(
            std::remove_if(messageQueue.begin(), messageQueue.end(),
                          [](const Message& msg) { return msg.processed; }),
            messageQueue.end()
        );
    }
    
    void clearPluginMessages(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        pluginIncomingMessages.erase(pluginName);
        pluginOutgoingMessages.erase(pluginName);
    }
    
    size_t getMessageCount(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(messageMutex);
        
        auto inIt = pluginIncomingMessages.find(pluginName);
        if (inIt != pluginIncomingMessages.end()) {
            return inIt->second.size();
        }
        
        return 0;
    }
    
    std::string getMessageQueueStatus() const {
        std::lock_guard<std::mutex> lock(messageMutex);
        return "Total queued messages: " + std::to_string(messageQueue.size()) +
               ", Plugins with incoming messages: " + std::to_string(pluginIncomingMessages.size());
    }
};

// Plugin dependency manager
class PluginDependencyManager {
private:
    std::map<std::string, std::vector<std::string>> pluginDependencies;
    std::map<std::string, std::vector<std::string>> pluginDependents;
    std::map<std::string, PluginState> dependencyStates;
    
public:
    PluginDependencyManager() {}
    
    void addDependency(const std::string& pluginName, const std::string& dependency) {
        pluginDependencies[pluginName].push_back(dependency);
        pluginDependents[dependency].push_back(pluginName);
    }
    
    bool hasDependency(const std::string& pluginName, const std::string& dependency) const {
        auto it = pluginDependencies.find(pluginName);
        if (it == pluginDependencies.end()) {
            return false;
        }
        
        return std::find(it->second.begin(), it->second.end(), dependency) != it->second.end();
    }
    
    std::vector<std::string> getDependencies(const std::string& pluginName) const {
        auto it = pluginDependencies.find(pluginName);
        if (it == pluginDependencies.end()) {
            return {};
        }
        return it->second;
    }
    
    std::vector<std::string> getDependents(const std::string& pluginName) const {
        auto it = pluginDependents.find(pluginName);
        if (it == pluginDependents.end()) {
            return {};
        }
        return it->second;
    }
    
    bool checkDependencySatisfaction(const std::string& pluginName, 
                                     const std::map<std::string, PluginState>& pluginStates) const {
        auto it = pluginDependencies.find(pluginName);
        if (it == pluginDependencies.end()) {
            return true; // No dependencies
        }
        
        for (const std::string& dependency : it->second) {
            auto depIt = pluginStates.find(dependency);
            if (depIt == pluginStates.end() || depIt->second != PluginState::RUNNING) {
                return false; // Dependency not satisfied
            }
        }
        
        return true;
    }
    
    bool satisfyDependencies(const std::string& pluginName,
                            std::function<void(const std::string&, PluginState)> stateUpdater) {
        auto it = pluginDependencies.find(pluginName);
        if (it == pluginDependencies.end()) {
            return true;
        }
        
        for (const std::string& dependency : it->second) {
            auto depIt = dependencyStates.find(dependency);
            if (depIt == dependencyStates.end() || depIt->second != PluginState::RUNNING) {
                return false;
            }
        }
        
        return true;
    }
    
    void updatePluginState(const std::string& pluginName, PluginState state) {
        dependencyStates[pluginName] = state;
    }
    
    bool canLoadPlugin(const std::string& pluginName, const std::map<std::string, PluginState>& pluginStates) const {
        // Check if all dependencies are loaded
        auto it = pluginDependencies.find(pluginName);
        if (it == pluginDependencies.end()) {
            return true;
        }
        
        for (const std::string& dependency : it->second) {
            auto depIt = pluginStates.find(dependency);
            if (depIt == pluginStates.end() || 
                (depIt->second != PluginState::RUNNING && depIt->second != PluginState::LOADED)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<std::string> getPluginsToStop(const std::string& pluginName) const {
        std::vector<std::string> pluginsToStop;
        
        // Find all plugins that depend on this plugin
        auto dependentsIt = pluginDependents.find(pluginName);
        if (dependentsIt != pluginDependents.end()) {
            pluginsToStop = dependentsIt->second;
        }
        
        return pluginsToStop;
    }
};

// Plugin configuration manager
class PluginConfigurationManager {
private:
    std::map<std::string, std::map<std::string, std::any>> pluginConfigurations;
    std::map<std::string, std::map<std::string, std::any>> pluginDefaultConfigurations;
    
public:
    PluginConfigurationManager() {}
    
    bool setPluginConfiguration(const std::string& pluginName, 
                               const std::map<std::string, std::any>& config) {
        pluginConfigurations[pluginName] = config;
        return true;
    }
    
    std::map<std::string, std::any> getPluginConfiguration(const std::string& pluginName) const {
        auto it = pluginConfigurations.find(pluginName);
        if (it == pluginConfigurations.end()) {
            return getPluginDefaultConfiguration(pluginName);
        }
        return it->second;
    }
    
    bool setPluginDefaultConfiguration(const std::string& pluginName,
                                      const std::map<std::string, std::any>& config) {
        pluginDefaultConfigurations[pluginName] = config;
        return true;
    }
    
    std::map<std::string, std::any> getPluginDefaultConfiguration(const std::string& pluginName) const {
        auto it = pluginDefaultConfigurations.find(pluginName);
        if (it == pluginDefaultConfigurations.end()) {
            return {};
        }
        return it->second;
    }
    
    bool mergeConfigurations(std::map<std::string, std::any>>& target,
                           const std::map<std::string, std::any>>& source) {
        for (const auto& pair : source) {
            target[pair.first] = pair.second;
        }
        return true;
    }
    
    std::map<std::string, std::any> getConfigurationWithDefaults(const std::string& pluginName) const {
        std::map<std::string, std::any> config = getPluginDefaultConfiguration(pluginName);
        mergeConfigurations(config, getPluginConfiguration(pluginName));
        return config;
    }
    
    bool validateConfiguration(const std::string& pluginName,
                              const std::map<std::string, std::any>>& config) const {
        // Basic validation - check for required keys
        auto defaultConfig = getPluginDefaultConfiguration(pluginName);
        for (const auto& pair : defaultConfig) {
            std::string key = pair.first;
            
            // Check if key exists in current config
            if (config.find(key) == config.end()) {
                // Key doesn't exist, but might be optional
                continue;
            }
            
            // Check type compatibility
            const std::any& defaultValue = pair.second;
            const std::any& configValue = config.at(key);
            
            if (!typeCompatible(defaultValue, configValue)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool typeCompatible(const std::any& value1, const std::any& value2) const {
        if (value1.type() == value2.type()) {
            return true; // Same type
        }
        
        // Check for numeric conversions
        if (value1.type() == typeid(int) && value2.type() == typeid(float)) return true;
        if (value1.type() == typeid(float) && value2.type() == typeid(int)) return true;
        if (value1.type() == typeid(int) && value2.type() == typeid(double)) return true;
        if (value1.type() == typeid(double) && value2.type() == typeid(int)) return true;
        if (value1.type() == typeid(float) && value2.type() == typeid(double)) return true;
        if (value1.type() == typeid(double) && value2.type() == typeid(float)) return true;
        
        // Check for string conversions
        if (value1.type() == typeid(std::string) || value2.type() == typeid(std::string)) {
            return true;
        }
        
        return false;
    }
};

// Plugin metrics collector
class PluginMetricsCollector {
private:
    struct PluginMetrics {
        std::string pluginName;
        PluginType pluginType;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::time_point lastUpdateTime;
        uint64_t totalMessagesSent = 0;
        uint64_t totalMessagesReceived = 0;
        uint64_t totalEventsHandled = 0;
        double cpuTime = 0.0;
        double memoryUsage = 0.0;
        uint64_t restartCount = 0;
        std::map<std::string, std::any> customMetrics;
    };
    
    std::map<std::string, PluginMetrics> metrics;
    std::chrono::system_clock::time_point sessionStartTime;
    
public:
    PluginMetricsCollector() {
        sessionStartTime = std::chrono::system_clock::now();
    }
    
    void recordPluginLoad(const std::string& pluginName, PluginType pluginType) {
        PluginMetrics pluginMetrics;
        pluginMetrics.pluginName = pluginName;
        pluginMetrics.pluginType = pluginType;
        pluginMetrics.loadTime = std::chrono::system_clock::now();
        pluginMetrics.lastUpdateTime = pluginMetrics.loadTime;
        
        metrics[pluginName] = pluginMetrics;
    }
    
    void recordPluginMessageSent(const std::string& pluginName) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.totalMessagesSent++;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void recordPluginMessageReceived(const std::string& pluginName) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.totalMessagesReceived++;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void recordEventHandled(const std::string& pluginName) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.totalEventsHandled++;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void recordCpuTime(const std::string& pluginName, double cpuTime) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.cpuTime += cpuTime;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void recordMemoryUsage(const std::string& pluginName, double memoryUsage) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.memoryUsage = memoryUsage;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void recordRestart(const std::string& pluginName) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.restartCount++;
            it->second.lastUpdateTime = std::chrono::system_clock::now();
        }
    }
    
    void setCustomMetric(const std::string& pluginName, const std::string& key, 
                        const std::any& value) {
        auto it = metrics.find(pluginName);
        if (it != metrics.end()) {
            it->second.customMetrics[key] = value;
        }
    }
    
    std::map<std::string, std::any> getPluginMetrics(const std::string& pluginName) const {
        auto it = metrics.find(pluginName);
        if (it == metrics.end()) {
            return {};
        }
        
        std::map<std::string, std::any> result;
        result["plugin_name"] = it->second.pluginName;
        result["plugin_type"] = static_cast<int>(it->second.pluginType);
        result["load_time"] = std::chrono::system_clock::to_time_t(it->second.loadTime);
        result["last_update_time"] = std::chrono::system_clock::to_time_t(it->second.lastUpdateTime);
        result["total_messages_sent"] = it->second.totalMessagesSent;
        result["total_messages_received"] = it->second.totalMessagesReceived;
        result["total_events_handled"] = it->second.totalEventsHandled;
        result["cpu_time"] = it->second.cpuTime;
        result["memory_usage"] = it->second.memoryUsage;
        result["restart_count"] = it->second.restartCount;
        
        for (const auto& pair : it->second.customMetrics) {
            result["custom_" + pair.first] = pair.second;
        }
        
        return result;
    }
    
    std::map<std::string, std::any> getSystemMetrics() const {
        std::map<std::string, std::any> result;
        
        auto sessionDuration = std::chrono::duration<double>(
            std::chrono::system_clock::now() - sessionStartTime
        ).count();
        
        result["session_duration"] = sessionDuration;
        result["total_plugins"] = metrics.size();
        result["total_messages_sent"] = 0;
        result["total_messages_received"] = 0;
        result["total_events_handled"] = 0;
        
        for (const auto& pair : metrics) {
            result["total_messages_sent"] = 
                std::any_cast<size_t>(result["total_messages_sent"]) + pair.second.totalMessagesSent;
            result["total_messages_received"] = 
                std::any_cast<size_t>(result["total_messages_received"]) + pair.second.totalMessagesReceived;
            result["total_events_handled"] = 
                std::any_cast<size_t>(result["total_events_handled"]) + pair.second.totalEventsHandled;
        }
        
        return result;
    }
    
    void resetMetrics() {
        metrics.clear();
        sessionStartTime = std::chrono::system_clock::now();
    }
    
    bool hasPluginMetrics(const std::string& pluginName) const {
        return metrics.find(pluginName) != metrics.end();
    }
    
    std::string getMetricsReport() const {
        std::stringstream ss;
        ss << "=== Plugin System Metrics ===" << std::endl;
        ss << "Session duration: " << 
            std::chrono::duration<double>(std::chrono::system_clock::now() - sessionStartTime).count() << "s" << std::endl;
        ss << "Total plugins loaded: " << metrics.size() << std::endl;
        
        for (const auto& pair : metrics) {
            ss << "\nPlugin: " << pair.second.pluginName << " (" 
               << static_cast<int>(pair.second.pluginType) << ")" << std::endl;
            ss << "  Load time: " << std::chrono::system_clock::to_time_t(pair.second.loadTime) << std::endl;
            ss << "  Messages sent: " << pair.second.totalMessagesSent << std::endl;
            ss << "  Messages received: " << pair.second.totalMessagesReceived << std::endl;
            ss << "  Events handled: " << pair.second.totalEventsHandled << std::endl;
            ss << "  CPU time: " << pair.second.cpuTime << "s" << std::endl;
            ss << "  Memory usage: " << pair.second.memoryUsage << "MB" << std::endl;
            ss << "  Restarts: " << pair.second.restartCount << std::endl;
        }
        
        return ss.str();
    }
};

// Plugin file system utilities
class PluginFileSystem {
public:
    static bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    static bool isDirectory(const std::string& path) {
        return std::filesystem::is_directory(path);
    }
    
    static bool isFile(const std::string& path) {
        return std::filesystem::is_regular_file(path);
    }
    
    static std::vector<std::string> getFilesInDirectory(const std::string& directoryPath,
                                                       const std::string& extension = "") {
        std::vector<std::string> files;
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    if (extension.empty() || 
                        entry.path().extension() == extension) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // Handle error
        }
        
        return files;
    }
    
    static std::vector<std::string> getDirectoriesInPath(const std::string& path) {
        std::vector<std::string> directories;
        
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    directories.push_back(entry.path().string());
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // Handle error
        }
        
        return directories;
    }
    
    static std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        if (pos != std::string::npos) {
            return filename.substr(pos + 1);
        }
        return "";
    }
    
    static std::string getFileNameWithoutExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        if (pos != std::string::npos) {
            return filename.substr(0, pos);
        }
        return filename;
    }
    
    static std::string getPluginPathFromFile(const std::string& filePath) {
        // For shared libraries, extract directory path
        size_t lastSlash = filePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            return filePath.substr(0, lastSlash + 1);
        }
        return filePath;
    }
    
    static bool validatePluginFile(const std::string& filePath) {
        // Check if file is a valid plugin library
        std::string extension = getFileExtension(filePath);
        
        // Common plugin library extensions
        std::vector<std::string> validExtensions = {"so", "dylib", "dll", "plugin"};
        
        return std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end();
    }
    
    static std::map<std::string, std::any> parsePluginInfoFile(const std::string& infoFilePath) {
        std::map<std::string, std::any> pluginInfo;
        
        try {
            std::ifstream file(infoFilePath);
            if (!file.is_open()) {
                return pluginInfo;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                // Parse key-value pairs
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t\n\r"));
                    key.erase(key.find_last_not_of(" \t\n\r") + 1);
                    
                    value.erase(0, value.find_first_not_of(" \t\n\r"));
                    value.erase(value.find_last_not_of(" \t\n\r") + 1);
                    
                    pluginInfo[key] = value;
                }
            }
        } catch (const std::exception& e) {
            // Handle error
        }
        
        return pluginInfo;
    }
    
    static bool copyPluginFiles(const std::string& sourceDir, const std::string& destDir) {
        try {
            if (!std::filesystem::exists(destDir)) {
                std::filesystem::create_directories(destDir);
            }
            
            for (const auto& entry : std::filesystem::directory_iterator(sourceDir)) {
                std::filesystem::path sourcePath = entry.path();
                std::filesystem::path destPath = destDir / sourcePath.filename();
                
                if (entry.is_regular_file()) {
                    std::filesystem::copy_file(sourcePath, destPath, 
                                             std::filesystem::copy_options::overwrite_existing);
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
        
        return true;
    }
    
    static bool extractPluginArchive(const std::string& archivePath, const std::string& extractDir) {
        try {
            if (!std::filesystem::exists(extractDir)) {
                std::filesystem::create_directories(extractDir);
            }
            
            // Implementation would depend on archive format (ZIP, tar, etc.)
            // This is a placeholder for archive extraction
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
        
        return true;
    }
};

// Plugin backup system
class PluginBackupSystem {
private:
    std::string backupDirectory;
    size_t maxBackupFiles;
    std::chrono::system_clock::time_point lastBackupTime;
    
public:
    PluginBackupSystem(const std::string& backupDir, size_t maxFiles = 10) 
        : backupDirectory(backupDir), maxBackupFiles(maxFiles) {
        if (!std::filesystem::exists(backupDirectory)) {
            std::filesystem::create_directories(backupDirectory);
        }
        lastBackupTime = std::chrono::system_clock::now();
    }
    
    bool createBackup(const std::string& pluginName) {
        try {
            std::string backupPath = backupDirectory + "/" + pluginName + 
                                   "_backup_" + getTimestampString() + ".bak";
            
            // Create backup directory
            std::string pluginBackupDir = backupPath + "/" + pluginName;
            std::filesystem::create_directories(pluginBackupDir);
            
            // Copy plugin files
            // Implementation would copy plugin files
            
            lastBackupTime = std::chrono::system_clock::now();
            
            // Clean old backups
            cleanOldBackups();
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool restoreBackup(const std::string& pluginName, const std::string& backupPath) {
        try {
            // Implementation would restore plugin from backup
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::vector<std::string> getAvailableBackups(const std::string& pluginName) const {
        std::vector<std::string> backups;
        
        try {
            std::string pattern = backupDirectory + "/" + pluginName + "_backup_*.bak";
            
            // Implementation would find backup files
        } catch (const std::exception& e) {
            // Handle error
        }
        
        return backups;
    }
    
    bool deleteBackup(const std::string& pluginName, const std::string& backupPath) {
        try {
            return std::filesystem::remove(backupPath);
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
private:
    std::string getTimestampString() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    void cleanOldBackups() {
        try {
            std::vector<std::filesystem::path> backupFiles;
            
            for (const auto& entry : std::filesystem::directory_iterator(backupDirectory)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("backup_") != std::string::npos && 
                        filename.find(".bak") != std::string::npos) {
                        backupFiles.push_back(entry.path());
                    }
                }
            }
            
            // Sort by modification time (oldest first)
            std::sort(backupFiles.begin(), backupFiles.end(),
                     [](const std::filesystem::path& a, const std::filesystem::path& b) {
                         return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
                     });
            
            // Remove old backups if we have too many
            if (backupFiles.size() > maxBackupFiles) {
                size_t filesToDelete = backupFiles.size() - maxBackupFiles;
                for (size_t i = 0; i < filesToDelete; ++i) {
                    std::filesystem::remove(backupFiles[i]);
                }
            }
        } catch (const std::exception& e) {
            // Handle error
        }
    }
};

// Main PluginManager implementation
PluginManager::PluginManager() {
    // Initialize plugin system components
    stateManager = std::make_shared<PluginStateManager>();
    messageSystem = std::make_shared<PluginMessageSystem>();
    dependencyManager = std::make_shared<PluginDependencyManager>();
    configManager = std::make_shared<PluginConfigurationManager>();
    metricsCollector = std::make_shared<PluginMetricsCollector>();
    
    // Setup state transition handlers
    setupStateTransitionHandlers();
}

PluginManager::~PluginManager() {
    shutdown();
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    try {
        // Validate plugin file
        if (!PluginFileSystem::validatePluginFile(pluginPath)) {
            return false;
        }
        
        // Extract plugin name from path
        std::string pluginName = PluginFileSystem::getFileNameWithoutExtension(
            std::filesystem::path(pluginPath).filename().string()
        );
        
        // Check if plugin already loaded
        if (hasPlugin(pluginName)) {
            return false; // Already loaded
        }
        
        // Load plugin library (implementation would depend on platform)
        // For now, create a dummy plugin
        auto plugin = createDummyPlugin(pluginName, pluginPath);
        if (!plugin) {
            return false;
        }
        
        // Register the plugin
        return registerPlugin(plugin);
    } catch (const std::exception& e) {
        return false;
    }
}

bool PluginManager::loadPlugin(std::shared_ptr<IPlugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    std::string pluginName = plugin->getName();
    
    // Check if plugin already loaded
    if (hasPlugin(pluginName)) {
        return false;
    }
    
    // Set initial state
    stateManager->setState(pluginName, PluginState::LOADING, "Plugin loading requested");
    
    try {
        // Initialize plugin
        if (!plugin->initialize(getPluginConfig(pluginName))) {
            stateManager->setState(pluginName, PluginState::ERROR, "Plugin initialization failed");
            return false;
        }
        
        // Register the plugin
        if (!registerPlugin(plugin)) {
            stateManager->setState(pluginName, PluginState::ERROR, "Plugin registration failed");
            return false;
        }
        
        // Start plugin
        if (!plugin->start()) {
            stateManager->setState(pluginName, PluginState::ERROR, "Plugin start failed");
            return false;
        }
        
        // Record load metrics
        metricsCollector->recordPluginLoad(pluginName, plugin->getType());
        
        return true;
    } catch (const std::exception& e) {
        stateManager->setState(pluginName, PluginState::ERROR, "Plugin loading exception: " + std::string(e.what()));
        return false;
    }
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false;
    }
    
    // Stop the plugin first
    if (stateManager->isRunning(pluginName)) {
        if (!it->second->stop()) {
            return false;
        }
    }
    
    // Unregister the plugin
    if (!unregisterPlugin(pluginName)) {
        return false;
    }
    
    // Set unloading state
    stateManager->setState(pluginName, PluginState::UNLOADING, "Plugin unloading requested");
    
    // Unload the plugin
    if (!it->second->unload()) {
        return false;
    }
    
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    // Stop the plugin first
    if (stateManager->isRunning(pluginName)) {
        auto it = plugins.find(pluginName);
        if (it != plugins.end()) {
            if (!it->second->stop()) {
                return false;
            }
        }
    }
    
    // Unload the plugin
    if (!unloadPlugin(pluginName)) {
        return false;
    }
    
    // Reload the plugin
    return loadPlugin(pluginName);
}

bool PluginManager::registerPlugin(std::shared_ptr<IPlugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    std::string pluginName = plugin->getName();
    
    // Check if plugin already registered
    if (hasPlugin(pluginName)) {
        return false; // Already registered
    }
    
    // Check dependencies
    std::map<std::string, PluginState> pluginStates;
    for (const auto& pair : plugins) {
        pluginStates[pair.first] = stateManager->getState(pair.first);
    }
    
    if (!dependencyManager->checkDependencySatisfaction(pluginName, pluginStates)) {
        return false; // Dependencies not satisfied
    }
    
    // Register the plugin
    plugins[pluginName] = plugin;
    pluginStates[pluginName] = PluginState::LOADED;
    
    // Update dependency states
    for (const auto& pair : plugins) {
        dependencyManager->updatePluginState(pair.first, stateManager->getState(pair.first));
    }
    
    // Record metrics
    metricsCollector->recordEventHandled(pluginName);
    
    // Trigger plugin registration event
    triggerEvent("plugin_registered", {
        {"plugin_name", pluginName},
        {"plugin_type", static_cast<int>(plugin->getType())},
        {"plugin_version", plugin->getVersion()}
    });
    
    return true;
}

bool PluginManager::unregisterPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false; // Plugin not found
    }
    
    // Handle dependent plugins
    std::vector<std::string> pluginsToStop = dependencyManager->getPluginsToStop(pluginName);
    for (const std::string& dependentPlugin : pluginsToStop) {
        if (stateManager->isRunning(dependentPlugin)) {
            auto depIt = plugins.find(dependentPlugin);
            if (depIt != plugins.end()) {
                depIt->second->stop();
            }
        }
    }
    
    // Remove the plugin
    plugins.erase(it);
    stateManager->setState(pluginName, PluginState::UNLOADED, "Plugin unregistered");
    
    // Clear plugin messages
    messageSystem->clearPluginMessages(pluginName);
    
    // Trigger plugin unregistration event
    triggerEvent("plugin_unregistered", {
        {"plugin_name", pluginName}
    });
    
    return true;
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : plugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::shared_ptr<IPlugin>> PluginManager::getPlugins(PluginType type) const {
    std::vector<std::shared_ptr<IPlugin>> result;
    for (const auto& pair : plugins) {
        if (pair.second->getType() == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return nullptr;
    }
    return it->second;
}

bool PluginManager::hasPlugin(const std::string& pluginName) const {
    return plugins.find(pluginName) != plugins.end();
}

bool PluginManager::registerExtensionPoint(std::shared_ptr<IExtensionPoint> extensionPoint) {
    if (!extensionPoint) {
        return false;
    }
    
    std::string epName = extensionPoint->getName();
    
    // Check if extension point already registered
    if (hasExtensionPoint(epName)) {
        return false;
    }
    
    extensionPoints[epName] = extensionPoint;
    return true;
}

bool PluginManager::unregisterExtensionPoint(const std::string& extensionPointName) {
    auto it = extensionPoints.find(extensionPointName);
    if (it == extensionPoints.end()) {
        return false;
    }
    
    extensionPoints.erase(it);
    return true;
}

std::shared_ptr<IExtensionPoint> PluginManager::getExtensionPoint(const std::string& extensionPointName) const {
    auto it = extensionPoints.find(extensionPointName);
    if (it == extensionPoints.end()) {
        return nullptr;
    }
    return it->second;
}

bool PluginManager::hasExtensionPoint(const std::string& extensionPointName) const {
    return extensionPoints.find(extensionPointName) != extensionPoints.end();
}

bool PluginManager::configurePlugin(const std::string& pluginName,
                                   const std::map<std::string, std::any>>& config) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false;
    }
    
    // Validate configuration
    if (!configManager->validateConfiguration(pluginName, config)) {
        return false;
    }
    
    // Set configuration
    configManager->setPluginConfiguration(pluginName, config);
    
    // Notify plugin of configuration change
    it->second->configure(config);
    
    return true;
}

std::map<std::string, std::any> PluginManager::getPluginConfig(const std::string& pluginName) const {
    return configManager->getConfigurationWithDefaults(pluginName);
}

bool PluginManager::startPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false;
    }
    
    if (!dependencyManager->checkDependencySatisfaction(pluginName, getAllPluginStates())) {
        return false; // Dependencies not satisfied
    }
    
    if (!stateManager->setState(pluginName, PluginState::RUNNING, "Plugin started")) {
        return false;
    }
    
    // Trigger plugin started event
    triggerEvent("plugin_started", {
        {"plugin_name", pluginName}
    });
    
    return true;
}

bool PluginManager::stopPlugin(const std::string& pluginName) {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return false;
    }
    
    if (!stateManager->setState(pluginName, PluginState::STOPPING, "Plugin stopping")) {
        return false;
    }
    
    // Stop the plugin
    if (!it->second->stop()) {
        return false;
    }
    
    if (!stateManager->setState(pluginName, PluginState::LOADED, "Plugin stopped")) {
        return false;
    }
    
    // Trigger plugin stopped event
    triggerEvent("plugin_stopped", {
        {"plugin_name", pluginName}
    });
    
    return true;
}

bool PluginManager::pausePlugin(const std::string& pluginName) {
    return stateManager->setState(pluginName, PluginState::PAUSED, "Plugin paused");
}

bool PluginManager::resumePlugin(const std::string& pluginName) {
    return stateManager->setState(pluginName, PluginState::RUNNING, "Plugin resumed");
}

std::map<std::string, std::any> PluginManager::getPluginInfo(const std::string& pluginName) const {
    std::map<std::string, std::any> info;
    
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return info;
    }
    
    std::shared_ptr<IPlugin> plugin = it->second;
    
    info["name"] = pluginName;
    info["type"] = static_cast<int>(plugin->getType());
    info["version"] = plugin->getVersion();
    info["description"] = plugin->getDescription();
    info["author"] = plugin->getAuthor();
    info["contact"] = plugin->getContact();
    info["state"] = static_cast<int>(stateManager->getState(pluginName));
    
    return info;
}

std::vector<std::map<std::string, std::any>> PluginManager::getAllPluginInfo() const {
    std::vector<std::map<std::string, std::any>> result;
    
    for (const auto& pair : plugins) {
        result.push_back(getPluginInfo(pair.first));
    }
    
    return result;
}

std::map<std::string, PluginState> PluginManager::getAllPluginStates() const {
    std::map<std::string, PluginState> states;
    
    for (const auto& pair : plugins) {
        states[pair.first] = stateManager->getState(pair.first);
    }
    
    return states;
}

std::vector<std::string> PluginManager::findPlugins(PluginType type,
                                                   const std::map<std::string, std::any>>& criteria) const {
    std::vector<std::string> result;
    
    for (const auto& pair : plugins) {
        if (pair.second->getType() == type) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

std::vector<std::string> PluginManager::findPluginsByName(const std::string& namePattern) const {
    std::vector<std::string> result;
    
    for (const auto& pair : plugins) {
        if (pair.first.find(namePattern) != std::string::npos) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

bool PluginManager::savePluginState(const std::string& filename) {
    // Implementation would save plugin state to file
    // For now, return true
    return true;
}

bool PluginManager::loadPluginState(const std::string& filename) {
    // Implementation would load plugin state from file
    // For now, return true
    return true;
}

bool PluginManager::exportPluginConfiguration(const std::string& pluginName, const std::string& filename) {
    // Implementation would export plugin configuration to file
    return true;
}

bool PluginManager::importPluginConfiguration(const std::string& pluginName, const std::string& filename) {
    // Implementation would import plugin configuration from file
    return true;
}

void PluginManager::shutdown() {
    // Stop all plugins
    for (const auto& pair : plugins) {
        if (stateManager->isRunning(pair.first)) {
            pair.second->stop();
        }
    }
    
    // Unregister all plugins
    plugins.clear();
    extensionPoints.clear();
}

void PluginManager::cleanup() {
    // Clean up plugin system
    stateManager.reset();
    messageSystem.reset();
    dependencyManager.reset();
    configManager.reset();
    metricsCollector.reset();
}

bool PluginManager::isHealthy() const {
    // Check if plugin system is healthy
    for (const auto& pair : plugins) {
        if (stateManager->isError(pair.first)) {
            return false; // Plugin in error state
        }
    }
    
    return true;
}

std::map<std::string, std::any> PluginManager::getSystemStatus() const {
    std::map<std::string, std::any> status;
    
    status["healthy"] = isHealthy();
    status["total_plugins"] = plugins.size();
    status["loaded_plugins"] = static_cast<size_t>(std::count_if(plugins.begin(), plugins.end(),
        [this](const auto& pair) { return stateManager->isLoaded(pair.first); }));
    status["running_plugins"] = static_cast<size_t>(std::count_if(plugins.begin(), plugins.end(),
        [this](const auto& pair) { return stateManager->isRunning(pair.first); }));
    status["error_plugins"] = static_cast<size_t>(std::count_if(plugins.begin(), plugins.end(),
        [this](const auto& pair) { return stateManager->isError(pair.first); }));
    
    return status;
}

std::vector<std::string> PluginManager::discoverPlugins() const {
    return PluginFileSystem::getFilesInDirectory(".", "so");
}

bool PluginManager::isPluginAvailable(const std::string& pluginPath) const {
    return PluginFileSystem::validatePluginFile(pluginPath);
}

std::string PluginManager::getPluginVersion(const std::string& pluginName) const {
    auto it = plugins.find(pluginName);
    if (it == plugins.end()) {
        return "";
    }
    return it->second->getVersion();
}

bool PluginManager::enablePluginAutoLoad(const std::string& pluginPath, bool enable) {
    // Implementation would enable/disable auto-loading for plugin
    return true;
}

bool PluginManager::enablePluginAutoStart(const std::string& pluginName, bool enable) {
    // Implementation would enable/disable auto-starting for plugin
    return true;
}

bool PluginManager::enablePluginHotSwap(const std::string& pluginName, bool enable) {
    // Implementation would enable/disable hot-swapping for plugin
    return true;
}

std::map<std::string, std::any> PluginManager::getPluginMetrics(const std::string& pluginName) const {
    return metricsCollector->getPluginMetrics(pluginName);
}

std::map<std::string, std::any> PluginManager::getSystemMetrics() const {
    return metricsCollector->getSystemMetrics();
}

void PluginManager::resetMetrics() {
    metricsCollector->resetMetrics();
}

bool PluginManager::sendPluginMessage(const std::string& fromPlugin, const std::string& toPlugin,
                                    const std::string& message, const std::map<std::string, std::any>>& data) {
    return messageSystem->sendMessage(fromPlugin, toPlugin, message, data) != "";
}

std::vector<std::map<std::string, std::any>> PluginManager::receivePluginMessages(const std::string& pluginName) {
    std::vector<std::map<std::string, std::any>> messages;
    std::vector<Message> pluginMessages = messageSystem->getMessages(pluginName);
    
    for (const Message& msg : pluginMessages) {
        std::map<std::string, std::any> messageData;
        messageData["id"] = msg.id;
        messageData["from_plugin"] = msg.fromPlugin;
        messageData["type"] = msg.type;
        messageData["data"] = msg.data;
        messageData["timestamp"] = std::chrono::system_clock::to_time_t(msg.timestamp);
        messageData["processed"] = msg.processed;
        
        messages.push_back(messageData);
    }
    
    return messages;
}

void PluginManager::clearPluginMessages(const std::string& pluginName) {
    messageSystem->clearPluginMessages(pluginName);
}

void PluginManager::registerEventHandler(const std::string& eventName,
                                        std::function<void(const std::string&, const std::map<std::string, std::any>&)> handler) {
    eventHandlers[eventName].push_back(handler);
}

void PluginManager::triggerEvent(const std::string& eventName, const std::map<std::string, std::any>>& data) {
    auto it = eventHandlers.find(eventName);
    if (it != eventHandlers.end()) {
        for (const auto& handler : it->second) {
            handler(eventName, data);
        }
    }
}

void PluginManager::setupStateTransitionHandlers() {
    // Setup default state transition handlers
    stateManager->registerStateTransitionHandler(PluginState::ERROR,
        [this](const std::string& pluginName, const std::map<std::string, std::any>>& data) {
            // Error handling
            return true;
        });
}

std::shared_ptr<IPlugin> PluginManager::createDummyPlugin(const std::string& pluginName, const std::string& pluginPath) {
    // Create a dummy plugin for testing
    class DummyPlugin : public IPlugin {
    public:
        std::string getName() const override { return pluginName; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { return "Dummy plugin for testing"; }
        PluginType getType() const override { return PluginType::UTILITY; }
        std::string getAuthor() const override { return "System"; }
        std::string getContact() const override { return ""; }
        
        bool initialize(const std::map<std::string, std::any>>& config) override { return true; }
        bool start() override { return true; }
        bool stop() override { return true; }
        bool unload() override { return true; }
        PluginState getState() const override { return PluginState::RUNNING; }
        std::map<std::string, std::any> getStatus() const override { return {}; }
        void onEvent(const std::string& eventName, const std::map<std::string, std::any>>& data) override {}
        bool configure(const std::map<std::string, std::any>>& config) override { return true; }
        std::map<std::string, std::any> getConfiguration() const override { return {}; }
        std::vector<std::string> getDependencies() const override { return {}; }
        std::vector<std::string> getProvides() const override { return {}; }
    };
    
    return std::make_shared<DummyPlugin>();
}

// Global plugin manager instance
global PluginManager& getPluginManager() {
    static PluginManager instance;
    return instance;
}

// Plugin factory implementation
std::shared_ptr<IPlugin> PluginFactory::createPlugin(const std::string& pluginType,
                                                    const std::map<std::string, std::any>>& config) {
    // Implementation would create plugin based on type
    // For now, return nullptr
    return nullptr;
}

bool PluginFactory::registerPluginType(PluginType type,
                                      std::function<std::shared_ptr<IPlugin>(const std::map<std::string, std::any>>&)> factory) {
    // Implementation would register plugin factory
    return true;
}

std::vector<PluginType> PluginFactory::getAvailablePluginTypes() {
    std::vector<PluginType> types;
    // Implementation would return available plugin types
    return types;
}

std::string PluginFactory::getPluginTypeName(PluginType type) {
    // Implementation would return plugin type name
    return "Unknown";
}

std::map<std::string, std::any> PluginFactory::getPluginTypeInfo(PluginType type) {
    // Implementation would return plugin type info
    return {};
}

// Plugin loading utility functions
bool loadPluginFromFile(const std::string& filename) {
    return getPluginManager().loadPlugin(filename);
}

bool loadPluginFromDirectory(const std::string& directory) {
    // Implementation would load all plugins from directory
    return true;
}

bool unloadAllPlugins() {
    return getPluginManager().unloadPlugin("all");
}

std::vector<std::string> getAvailablePlugins() const {
    return getPluginManager().discoverPlugins();
}

bool validatePluginFile(const std::string& filename) {
    return PluginFileSystem::validatePluginFile(filename);
}

std::string getPluginFileType(const std::string& filename) {
    return PluginFileSystem::getFileExtension(filename);
}

} // namespace plugins
} // namespace nlm

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass) \
    extern "C" std::shared_ptr<nlm::IPlugin> createPluginInstance() { \
        return std::make_shared<PluginClass>(); \
    }

#define PLUGIN_VERSION(major, minor, patch) \
    static const std::string PLUGIN_VERSION = #major "." #minor "." #patch

#define PLUGIN_INFO(name, version, description, author, contact) \
    const std::string PLUGIN_NAME = name; \
    const std::string PLUGIN_VERSION = version; \
    const std::string PLUGIN_DESCRIPTION = description; \
    const std::string PLUGIN_AUTHOR = author; \
    const std::string PLUGIN_CONTACT = contact

// Plugin lifecycle macros
#define PLUGIN_INITIALIZE() \
    virtual bool initialize(const std::map<std::string, std::any>>& config) override

#define PLUGIN_START() \
    virtual bool start() override

#define PLUGIN_STOP() \
    virtual bool stop() override

#define PLUGIN_UNLOAD() \
    virtual bool unload() override

#define PLUGIN_CONFIGURE() \
    virtual bool configure(const std::map<std::string, std::any>>& config) override

} // namespace nlm

// Plugin loading utility functions
namespace PluginLoader {
    bool loadPluginFromFile(const std::string& filename);
    bool loadPluginFromDirectory(const std::string& directory);
    bool unloadAllPlugins();
    std::vector<std::string> getAvailablePlugins() const;
    bool validatePluginFile(const std::string& filename);
    std::string getPluginFileType(const std::string& filename);
}

// Plugin security utilities
namespace PluginSecurity {
    bool validatePluginSecurity(const std::shared_ptr<nlm::IPlugin>>& plugin);
    bool checkPluginPermissions(const std::string& pluginName, const std::string& permission);
    bool grantPluginPermission(const std::string& pluginName, const std::string& permission);
    bool revokePluginPermission(const std::string& pluginName, const std::string& permission);
    std::vector<std::string> getPluginPermissions(const std::string& pluginName);
}