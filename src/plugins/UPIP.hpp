// Universal Plugin Interface (UPIP) Base Classes for NLM
// Plugin system for NLM (Neural Learning Machine) with multi-language support

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <uuid/uuid.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {
namespace plugins {

// Plugin version information
struct PluginVersion {
    int major;
    int minor;
    int patch;
    std::string build;
    
    PluginVersion(int major = 0, int minor = 0, int patch = 0, const std::string& build = "")
        : major(major), minor(minor), patch(patch), build(build) {}
    
    std::string toString() const {
        std::string result = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
        if (!build.empty()) result += "-" + build;
        return result;
    }
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch && build == other.build;
    }
};

// Plugin metadata
struct PluginMetadata {
    std::string id;
    std::string name;
    std::string description;
    std::string author;
    std::string email;
    std::string url;
    PluginVersion version;
    std::string license;
    std::vector<std::string> tags;
    std::string category;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    bool isValid;
    std::string validationError;
    
    PluginMetadata() : isValid(false), createdAt(std::chrono::system_clock::now()) {}
    
    json toJson() const {
        json j;
        j["id"] = id;
        j["name"] = name;
        j["description"] = description;
        j["author"] = author;
        j["email"] = email;
        j["url"] = url;
        j["version"] = version.toString();
        j["license"] = license;
        j["tags"] = tags;
        j["category"] = category;
        j["isValid"] = isValid;
        if (!validationError.empty()) j["validationError"] = validationError;
        return j;
    }
};

// Plugin capability definition
enum class PluginCapability {
    NEURAL_MANIPULATION,
    MEMORY_MANAGEMENT,
    CONFIG_MANAGEMENT,
    SIMULATION_CONTROL,
    DEBUGGING,
    VISUALIZATION,
    DATA_ANALYSIS,
    PERFORMANCE_OPTIMIZATION,
    LOGGING,
    NETWORKING,
    FILE_IO,
    MATH_COMPUTATION,
    AI_INFERENCE,
    CUSTOM
};

// Plugin permission level
enum class PluginPermission {
    NONE,
    READ_ONLY,
    EXECUTION,
    NETWORK_ACCESS,
    FILE_SYSTEM_ACCESS,
    DEBUGGING_ACCESS,
    SYSTEM_ACCESS,
    FULL_ACCESS
};

// Plugin dependency
struct PluginDependency {
    std::string pluginId;
    PluginVersion version;
    bool optional;
    std::string reason;
    
    PluginDependency(const std::string& pluginId, const PluginVersion& version = PluginVersion(), bool optional = false, const std::string& reason = "")
        : pluginId(pluginId), version(version), optional(optional), reason(reason) {}
};

// Base interface for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin information
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual PluginVersion getVersion() const = 0;
    
    // Lifecycle management
    virtual bool initialize(const json& config) = 0;
    virtual bool shutdown() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    
    // Plugin interface
    virtual std::vector<PluginCapability> getCapabilities() const = 0;
    virtual PluginPermission getRequiredPermission() const = 0;
    virtual std::vector<PluginDependency> getDependencies() const = 0;
    
    // Version compatibility
    virtual bool isVersionCompatible(const PluginVersion& version) const = 0;
    
    // Plugin operations
    virtual json execute(const std::string& operation, const json& parameters) = 0;
    virtual bool handleEvent(const std::string& eventName, const json& eventData) = 0;
    
    // Plugin state
    virtual bool isActive() const = 0;
    virtual bool isInitialized() const = 0;
    virtual std::string getState() const = 0;
    virtual void setState(const std::string& state) = 0;
    
    // Plugin metadata update
    virtual void updateMetadata(const PluginMetadata& metadata) = 0;
    
    // Plugin identifier
    virtual std::string getId() const = 0;
};

// Script execution context
struct ScriptContext {
    std::string pluginId;
    std::string scriptId;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> variables;
    std::map<std::string, json> data;
    std::string language;
    bool isSandboxed;
    std::vector<std::string> allowedAPIs;
    std::vector<std::string> blockedAPIs;
    
    ScriptContext(const std::string& pluginId, const std::string& language, bool sandboxed = true)
        : pluginId(pluginId), language(language), isSandboxed(sandboxed), timestamp(std::chrono::system_clock::now()) {}
};

// Script execution result
struct ScriptResult {
    bool success;
    std::string output;
    json resultData;
    std::string errorMessage;
    std::chrono::milliseconds executionTime;
    int exitCode;
    
    ScriptResult() : success(false), executionTime(0), exitCode(0) {}
};

// Universal Plugin Interface for scripting
class IPluginScripting {
public:
    virtual ~IPluginScripting() = default;
    
    // Script execution
    virtual ScriptResult executeScript(const std::string& script, const ScriptContext& context) = 0;
    virtual ScriptResult executeScriptFile(const std::string& filepath, const ScriptContext& context) = 0;
    
    // Script management
    virtual std::vector<std::string> listScripts() const = 0;
    virtual bool saveScript(const std::string& scriptId, const std::string& script, const std::string& language) = 0;
    virtual bool deleteScript(const std::string& scriptId) = 0;
    virtual std::string getScript(const std::string& scriptId) const = 0;
    
    // Script context management
    virtual ScriptContext createContext(const std::string& pluginId, const std::string& language, bool sandboxed = true) = 0;
    virtual ScriptResult evaluateExpression(const std::string& expression, const ScriptContext& context) = 0;
    
    // Script debugging
    virtual bool addBreakpoint(const std::string& scriptId, int lineNumber) = 0;
    virtual bool removeBreakpoint(const std::string& scriptId, int lineNumber) = 0;
    virtual std::vector<int> getBreakpoints(const std::string& scriptId) const = 0;
    virtual void setDebugMode(bool enabled) = 0;
    virtual bool isDebugMode() const = 0;
    
    // Script profiling
    virtual void startProfiling(const std::string& scriptId) = 0;
    virtual void stopProfiling(const std::string& scriptId) = 0;
    virtual json getProfilingData(const std::string& scriptId) const = 0;
    virtual void clearProfilingData(const std::string& scriptId) = 0;
    
    // Script validation
    virtual bool validateScript(const std::string& script, const std::string& language) const = 0;
    virtual std::string lintScript(const std::string& script, const std::string& language) const = 0;
    
    // Script error handling
    virtual std::vector<std::string> getErrorHistory(const std::string& scriptId) const = 0;
    virtual void clearErrorHistory(const std::string& scriptId) = 0;
    virtual json getLastError(const std::string& scriptId) const = 0;
};

// Plugin statistics
struct PluginStatistics {
    std::string pluginId;
    int loadCount;
    int unloadCount;
    int activateCount;
    int deactivateCount;
    int executeCount;
    int errorCount;
    std::chrono::milliseconds totalExecutionTime;
    std::chrono::system_clock::time_point lastLoaded;
    std::chrono::system_clock::time_point lastUsed;
    
    PluginStatistics() : loadCount(0), unloadCount(0), activateCount(0), deactivateCount(0), 
                       executeCount(0), errorCount(0), totalExecutionTime(0) {}
    
    json toJson() const {
        json j;
        j["pluginId"] = pluginId;
        j["loadCount"] = loadCount;
        j["unloadCount"] = unloadCount;
        j["activateCount"] = activateCount;
        j["deactivateCount"] = deactivateCount;
        j["executeCount"] = executeCount;
        j["errorCount"] = errorCount;
        j["totalExecutionTimeMs"] = totalExecutionTime.count();
        j["lastLoaded"] = std::chrono::system_clock::to_time_t(lastLoaded);
        j["lastUsed"] = std::chrono::system_clock::to_time_t(lastUsed);
        return j;
    }
};

// Plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::unique_ptr<IPlugin> createPlugin(const json& config) = 0;
    virtual bool canCreatePlugin(const std::string& pluginType) const = 0;
    virtual std::vector<std::string> getSupportedPluginTypes() const = 0;
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Plugin lifecycle management
    virtual bool loadPlugin(const std::string& pluginPath) = 0;
    virtual bool unloadPlugin(const std::string& pluginId) = 0;
    virtual bool activatePlugin(const std::string& pluginId) = 0;
    virtual bool deactivatePlugin(const std::string& pluginId) = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Plugin information
    virtual PluginMetadata getPluginMetadata(const std::string& pluginId) const = 0;
    virtual PluginStatistics getPluginStatistics(const std::string& pluginId) const = 0;
    virtual std::vector<PluginMetadata> getAllPluginMetadata() const = 0;
    
    // Plugin execution
    virtual json executePlugin(const std::string& pluginId, const std::string& operation, const json& parameters) = 0;
    virtual bool handlePluginEvent(const std::string& pluginId, const std::string& eventName, const json& eventData) = 0;
    
    // Plugin scripting
    virtual std::shared_ptr<IPluginScripting> getPluginScripting(const std::string& pluginId) = 0;
    virtual ScriptResult executeScript(const std::string& pluginId, const std::string& script, const ScriptContext& context) = 0;
    
    // Plugin dependency management
    virtual bool checkDependencies(const std::string& pluginId) const = 0;
    virtual std::vector<std::string> getMissingDependencies(const std::string& pluginId) const = 0;
    
    // Plugin security
    virtual bool validatePlugin(const std::string& pluginPath) const = 0;
    virtual bool verifyPluginSignature(const std::string& pluginPath) const = 0;
    
    // Plugin configuration
    virtual bool updatePluginConfig(const std::string& pluginId, const json& config) = 0;
    virtual json getPluginConfig(const std::string& pluginId) const = 0;
    
    // Plugin discovery
    virtual std::vector<std::string> discoverPlugins(const std::string& directory) = 0;
    virtual bool registerPlugin(const PluginMetadata& metadata) = 0;
    
    // Plugin hot-swapping
    virtual bool hotSwapPlugin(const std::string& pluginId) = 0;
    virtual bool canHotSwap(const std::string& pluginId) const = 0;
    
    // Plugin statistics
    virtual void updatePluginStatistics(const std::string& pluginId, const PluginStatistics& stats) = 0;
    virtual std::map<std::string, PluginStatistics> getAllPluginStatistics() const = 0;
    
    // Plugin search and filtering
    virtual std::vector<std::string> searchPlugins(const std::string& query) const = 0;
    virtual std::vector<std::string> filterPluginsByCapability(const PluginCapability& capability) const = 0;
    virtual std::vector<std::string> filterPluginsByCategory(const std::string& category) const = 0;
};

// Plugin security manager interface
class IPluginSecurityManager {
public:
    virtual ~IPluginSecurityManager() = default;
    
    // Plugin validation
    virtual bool validatePluginIntegrity(const std::string& pluginPath) const = 0;
    virtual bool verifyPluginSignature(const std::string& pluginPath, const std::string& signature) const = 0;
    virtual std::string generatePluginSignature(const std::string& pluginPath) const = 0;
    
    // Plugin permissions
    virtual bool checkPluginPermissions(const std::string& pluginId, PluginPermission requiredPermission) const = 0;
    virtual bool grantPluginPermission(const std::string& pluginId, PluginPermission permission) = 0;
    virtual bool revokePluginPermission(const std::string& pluginId, PluginPermission permission) = 0;
    virtual PluginPermission getPluginPermission(const std::string& pluginId) const = 0;
    
    // Plugin sandboxing
    virtual bool enablePluginSandboxing(const std::string& pluginId) = 0;
    virtual bool disablePluginSandboxing(const std::string& pluginId) = 0;
    virtual bool isPluginSandboxed(const std::string& pluginId) const = 0;
    virtual void setPluginIsolationLevel(const std::string& pluginId, int level) = 0;
    virtual int getPluginIsolationLevel(const std::string& pluginId) const = 0;
    
    // Plugin whitelist/blacklist
    virtual bool addPluginToWhitelist(const std::string& pluginId) = 0;
    virtual bool removePluginFromWhitelist(const std::string& pluginId) = 0;
    virtual bool isPluginWhitelisted(const std::string& pluginId) const = 0;
    virtual bool addPluginToBlacklist(const std::string& pluginId) = 0;
    virtual bool removePluginFromBlacklist(const std::string& pluginId) = 0;
    virtual bool isPluginBlacklisted(const std::string& pluginId) const = 0;
    
    // Plugin security logging
    virtual void logSecurityEvent(const std::string& pluginId, const std::string& eventType, const json& eventData) = 0;
    virtual std::vector<json> getSecurityEvents(const std::string& pluginId, const std::string& eventType) const = 0;
    
    // Plugin recovery
    virtual bool recoverPlugin(const std::string& pluginId) = 0;
    virtual bool quarantinePlugin(const std::string& pluginId) = 0;
    virtual bool restorePlugin(const std::string& pluginId) = 0;
};

// Plugin version manager interface
class IPluginVersionManager {
public:
    virtual ~IPluginVersionManager() = default;
    
    // Plugin versioning
    virtual bool registerPluginVersion(const std::string& pluginId, const PluginVersion& version, const std::string& description) = 0;
    virtual bool unregisterPluginVersion(const std::string& pluginId, const PluginVersion& version) = 0;
    virtual PluginVersion getLatestVersion(const std::string& pluginId) const = 0;
    virtual std::vector<PluginVersion> getAvailableVersions(const std::string& pluginId) const = 0;
    
    // Plugin update management
    virtual bool checkForUpdates(const std::string& pluginId) = 0;
    virtual bool downloadUpdate(const std::string& pluginId, const PluginVersion& version) = 0;
    virtual bool installUpdate(const std::string& pluginId, const PluginVersion& version) = 0;
    
    // Plugin rollback
    virtual bool rollbackPlugin(const std::string& pluginId, const PluginVersion& version) = 0;
    virtual std::vector<PluginVersion> getRollbackVersions(const std::string& pluginId) const = 0;
};

} // namespace plugins
} // namespace nlm