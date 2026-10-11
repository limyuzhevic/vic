#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

// Plugin types enumeration
enum class PluginType {
    NEURAL_NETWORK,
    MEMORY_SYSTEM,
    NEUROMODULATION,
    COGNITION,
    DEVELOPMENT,
    PLASTICITY,
    VISUALIZATION,
    INPUT_OUTPUT,
    UNKNOWN
};

// Plugin state enumeration
enum class PluginState {
    STOPPED,
    PAUSED,
    RUNNING,
    ERROR
};

// Plugin load policy enumeration
enum class PluginLoadPolicy {
    AUTO,
    MANUAL,
    CONDITIONAL
};

// Plugin unload policy enumeration
enum class PluginUnloadPolicy {
    NORMAL,
    FORCE,
    CLEANUP
};

// Plugin information structure
struct PluginInfo {
    std::string name;
    std::string version;
    PluginType type;
    std::string category;
    std::string description;
    std::string author;
    std::string contact;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> dependencies;
    bool enabled = true;
    int priority = 0;
    PluginLoadPolicy loadPolicy = PluginLoadPolicy::AUTO;
    PluginUnloadPolicy unloadPolicy = PluginUnloadPolicy::NORMAL;
    std::map<std::string, std::string> metadata;
};

// Plugin message structure
struct PluginMessage {
    std::string sender;
    std::string receiver;
    std::string type;
    std::string category;
    double timestamp;
    std::map<std::string, std::string> data;
    std::map<std::string, double> numericData;
    std::map<std::string, std::vector<std::string>> stringListData;
    std::string payload;  // For binary data
};

// Plugin configuration structure
struct PluginConfig {
    std::string name;
    std::string version;
    PluginType type;
    std::string category;
    std::string description;
    std::string author;
    std::string contact;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> dependencies;
    bool enabled = true;
    int priority = 0;
    PluginLoadPolicy loadPolicy = PluginLoadPolicy::AUTO;
    PluginUnloadPolicy unloadPolicy = PluginUnloadPolicy::NORMAL;
    std::map<std::string, std::string> metadata;
};

// Plugin statistics structure
struct PluginStats {
    double cpuUsage;
    double memoryUsage;
    size_t threadCount;
    double updateTime;
    int errorCount;
    std::vector<std::string> warnings;
};

// Plugin status structure
struct PluginStatus {
    PluginState state;
    std::string errorMessage;
    PluginStats stats;
    double lastUpdateTime;
    double totalUptime;
};

// Plugin change record for rollback
struct PluginChangeRecord {
    double timestamp;
    std::string action;  // "LOAD", "UNLOAD", "UPDATE", "ERROR"
    std::string version;
    std::string details;
    bool successful;
};

// Base plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual PluginType getType() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const PluginConfig& config, void* manager) = 0;
    virtual void update(double deltaTime) = 0;
    virtual void shutdown() = 0;
    
    // Plugin state management
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual PluginState getState() const = 0;
    
    // Plugin dependencies
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool checkDependencies(const std::unordered_map<std::string, PluginInfo>& plugins) const = 0;
    
    // Plugin communication
    virtual void receiveMessage(const std::string& sender, const PluginMessage& message) = 0;
    virtual void sendMessage(const std::string& receiver, const PluginMessage& message) = 0;
    
    // Plugin configuration
    virtual void configure(const PluginConfig& config) = 0;
    virtual PluginConfig getConfig() const = 0;
};

} // namespace nlm
