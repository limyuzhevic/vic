#include "PluginDebugInterface.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

DebugPluginBase::DebugPluginBase(const std::string& name, const std::string& version)
    : name(name), version(version), globalDebugLevel(DebugLevel::Info), defaultDebugLevel(DebugLevel::Info), 
      debugInitialized(false), loadStatus(PluginLoadStatus::NotLoaded) {
    metadata.name = name;
    metadata.version = version;
    metadata.creationDate = std::chrono::system_clock::now();
    metadata.lastUpdated = metadata.creationDate;
    metadata.securityLevel = PluginSecurityLevel::Medium;
    
    pluginDebugLevels[name] = DebugLevel::Info;
    
    debugInitialized = false;
}

std::string DebugPluginBase::getName() const {
    return name;
}

std::string DebugPluginBase::getVersion() const {
    return version;
}

std::string DebugPluginBase::getDescription() const {
    return metadata.description;
}

std::string DebugPluginBase::getAuthor() const {
    return metadata.author;
}

const PluginMetadata& DebugPluginBase::getMetadata() const {
    return metadata;
}

void DebugPluginBase::logMessage(const std::string& pluginName, DebugLevel level, const std::string& message,
                                const std::map<std::string, std::string>& metadata) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = level;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    output.metadata = metadata;
    
    addOutput(output);
    
    // Print to console based on level
    std::string levelStr = getDebugLevelStringInternal(level);
    std::string timestamp = std::chrono::system_clock::to_time_t(output.timestamp);
    
    if (level >= DebugLevel::Error) {
        NLM_LOG_ERROR(std::to_string(timestamp) + " [" + levelStr + "] [" + pluginName + "] " + message);
    } else if (level >= DebugLevel::Warning) {
        NLM_LOG_WARNING(std::to_string(timestamp) + " [" + levelStr + "] [" + pluginName + "] " + message);
    } else if (level >= DebugLevel::Info) {
        NLM_LOG_INFO(std::to_string(timestamp) + " [" + levelStr + "] [" + pluginName + "] " + message);
    } else if (level >= DebugLevel::Debug) {
        NLM_LOG_INFO(std::to_string(timestamp) + " [" + levelStr + "] [" + pluginName + "] " + message);
    }
}

void DebugPluginBase::logError(const std::string& pluginName, const std::string& message,
                              const std::vector<std::string>& stackTrace) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = DebugLevel::Error;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    output.stackTrace = stackTrace;
    
    addOutput(output);
    
    NLM_LOG_ERROR(std::to_string(std::chrono::system_clock::to_time_t(output.timestamp)) + 
                  " [Error] [" + pluginName + "] " + message);
}

void DebugPluginBase::logWarning(const std::string& pluginName, const std::string& message) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = DebugLevel::Warning;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    
    addOutput(output);
    
    NLM_LOG_WARNING(std::to_string(std::chrono::system_clock::to_time_t(output.timestamp)) + 
                    " [Warning] [" + pluginName + "] " + message);
}

void DebugPluginBase::logInfo(const std::string& pluginName, const std::string& message) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = DebugLevel::Info;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    
    addOutput(output);
    
    NLM_LOG_INFO(std::to_string(std::chrono::system_clock::to_time_t(output.timestamp)) + 
                 " [Info] [" + pluginName + "] " + message);
}

void DebugPluginBase::logDebug(const std::string& pluginName, const std::string& message) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = DebugLevel::Debug;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    
    addOutput(output);
    
    NLM_LOG_INFO(std::to_string(std::chrono::system_clock::to_time_t(output.timestamp)) + 
                 " [Debug] [" + pluginName + "] " + message);
}

void DebugPluginBase::logTrace(const std::string& pluginName, const std::string& message) {
    DebugOutput output;
    output.pluginName = pluginName;
    output.level = DebugLevel::Trace;
    output.timestamp = std::chrono::system_clock::now();
    output.message = message;
    
    addOutput(output);
    
    NLM_LOG_INFO(std::to_string(std::chrono::system_clock::to_time_t(output.timestamp)) + 
                 " [Trace] [" + pluginName + "] " + message);
}

void DebugPluginBase::addFilter(const DebugPluginFilter& filter) {
    filters.push_back(filter);
}

void DebugPluginBase::removeFilter(const std::string& name) {
    auto it = std::find_if(filters.begin(), filters.end(),
                          [&name](const DebugPluginFilter& f) { return f.name == name; });
    if (it != filters.end()) {
        filters.erase(it);
    }
}

DebugPluginFilter DebugPluginBase::getFilter(const std::string& name) const {
    auto it = std::find_if(filters.begin(), filters.end(),
                          [&name](const DebugPluginFilter& f) { return f.name == name; });n    if (it != filters.end()) {
        return *it;
    }
    
    return DebugPluginFilter();
}

std::vector<DebugPluginFilter> DebugPluginBase::getFilters() const {
    return filters;
}

bool DebugPluginBase::updateFilters() {
    // TODO: Update filters based on configuration
    return true;
}

void DebugPluginBase::addOutput(const DebugOutput& output) {
    outputs.push_back(output);
    
    // Notify debug manager
    onDebugOutput(output);
    
    // Check if output matches any filter
    for (const auto& filter : filters) {
        if (filter.matches(output)) {
            onDebugEvent(DebugPluginEvent{
                DebugPluginEventType::DebugMessage,
                output.pluginName,
                output.timestamp,
                output.toString(),
                {},
                output.level,
                false
            });
        }
    }
}

std::vector<DebugOutput> DebugPluginBase::getOutputs() const {
    return outputs;
}

std::vector<DebugOutput> DebugPluginBase::getOutputs(const std::string& pluginName) const {
    std::vector<DebugOutput> pluginOutputs;
    
    for (const auto& output : outputs) {
        if (output.pluginName == pluginName) {
            pluginOutputs.push_back(output);
        }
    }
    
    return pluginOutputs;
}

std::vector<DebugOutput> DebugPluginBase::getOutputs(DebugLevel minLevel) const {
    std::vector<DebugOutput> filteredOutputs;
    
    for (const auto& output : outputs) {
        if (output.level >= minLevel) {
            filteredOutputs.push_back(output);
        }
    }
    
    return filteredOutputs;
}

void DebugPluginBase::clearOutputs() {
    outputs.clear();
}

void DebugPluginBase::clearOutputs(const std::string& pluginName) {
    outputs.erase(std::remove_if(outputs.begin(), outputs.end(),
                                 [&pluginName](const DebugOutput& output) { return output.pluginName == pluginName; }),
                  outputs.end());
}

bool DebugPluginBase::saveDebugState(const std::string& filepath) const {
    // TODO: Implement debug state saving
    return true;
}

bool DebugPluginBase::loadDebugState(const std::string& filepath) {
    // TODO: Implement debug state loading
    return true;
}

bool DebugPluginBase::clearDebugState() {
    clearOutputs();
    return true;
}

bool DebugPluginBase::setDebugLevel(const std::string& pluginName, DebugLevel level) {
    pluginDebugLevels[pluginName] = level;
    onDebugLevelChanged(pluginName, level);
    return true;
}

DebugLevel DebugPluginBase::getDebugLevel(const std::string& pluginName) const {
    auto it = pluginDebugLevels.find(pluginName);
    if (it != pluginDebugLevels.end()) {
        return it->second;
    }
    
    return defaultDebugLevel;
}

bool DebugPluginBase::setGlobalDebugLevel(DebugLevel level) {
    globalDebugLevel = level;
    onDebugLevelChanged("system", level);
    return true;
}

DebugLevel DebugPluginBase::getGlobalDebugLevel() const {
    return globalDebugLevel;
}

std::map<std::string, DebugLevel> DebugPluginBase::getPluginDebugLevels() const {
    return pluginDebugLevels;
}

bool DebugPluginBase::executeDebugCommand(const std::string& command,
                                       const std::map<std::string, std::string>& args) {
    // TODO: Implement debug command execution
    return true;
}

std::string DebugPluginBase::getDebugStatus() const {
    std::stringstream ss;
    ss << "DebugManager Status:" << std::endl;
    ss << "  Debug Initialized: " << (debugInitialized ? "true" : "false") << std::endl;
    ss << "  Global Debug Level: " << getDebugLevelStringInternal(globalDebugLevel) << std::endl;
    ss << "  Outputs Count: " << outputs.size() << std::endl;
    ss << "  Filters Count: " << filters.size() << std::endl;
    
    return ss.str();
}

std::string DebugPluginBase::getDebugInfo() const {
    std::stringstream ss;
    ss << "DebugManager Information:" << std::endl;
    ss << "  Default Debug Level: " << getDebugLevelStringInternal(defaultDebugLevel) << std::endl;
    ss << "  System Debug Level: " << getDebugLevelStringInternal(globalDebugLevel) << std::endl;
    ss << "  Plugin Debug Levels: " << pluginDebugLevels.size() << std::endl;
    
    return ss.str();
}

std::vector<std::string> DebugPluginBase::getDebugMessages() const {
    std::vector<std::string> messages;
    
    for (const auto& output : outputs) {
        if (output.level >= DebugLevel::Error) {
            messages.push_back(output.toString());
        }
    }
    
    return messages;
}

std::vector<std::string> DebugPluginBase::getDebugWarnings() const {
    std::vector<std::string> warnings;
    
    for (const auto& output : outputs) {
        if (output.level >= DebugLevel::Warning && output.level < DebugLevel::Error) {
            warnings.push_back(output.toString());
        }
    }
    
    return warnings;
}

std::vector<std::string> DebugPluginBase::getDebugErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& output : outputs) {
        if (output.level >= DebugLevel::Error) {
            errors.push_back(output.toString());
        }
    }
    
    return errors;
}

std::string DebugPluginBase::getAbout() const {
    return "DebugManager - Plugin Debug Management System for NLM";
}

std::string DebugPluginBase::getVersion() const {
    return "1.0.0";
}

std::string DebugPluginBase::getBuildInfo() const {
    return "DebugManager Build Information";
}

void DebugPluginBase::onDebugEvent(const DebugPluginEvent& event) {
    // Default implementation - can be overridden
    // Handle debug events
}

void DebugPluginBase::onDebugOutput(const DebugOutput& output) {
    // Default implementation - can be overridden
    // Handle debug output
}

void DebugPluginBase::onDebugLevelChanged(const std::string& pluginName, DebugLevel level) {
    // Default implementation - can be overridden
    // Handle debug level change
}

void DebugPluginBase::onDebugFilterAdded(const DebugPluginFilter& filter) {
    // Default implementation - can be overridden
    // Handle filter addition
}

void DebugPluginBase::onDebugFilterRemoved(const std::string& name) {
    // Default implementation - can be overridden
    // Handle filter removal
}

void DebugPluginBase::onSystemInitialized() {
    // Default implementation - can be overridden
    debugInitialized = true;
}

void DebugPluginBase::onSystemShutdown() {
    // Default implementation - can be overridden
    debugInitialized = false;
}

void DebugPluginBase::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    logError("system", error);
}

void DebugPluginBase::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    logWarning("system", warning);
}

// Plugin lifecycle callbacks implementation
void DebugPluginBase::onPluginLoadStarted(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Loading;
}

void DebugPluginBase::onPluginLoadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Loaded;
    } else {
        loadStatus = PluginLoadStatus::Failed;
    }
}

void DebugPluginBase::onPluginLoadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void DebugPluginBase::onPluginStartRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Starting;
}

void DebugPluginBase::onPluginStartCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Running;
    } else {
        loadStatus = PluginLoadStatus::Failed;
    }
}

void DebugPluginBase::onPluginStartFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void DebugPluginBase::onPluginStopRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Stopping;
}

void DebugPluginBase::onPluginStopCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Stopped;
    } else {
        loadStatus = PluginLoadStatus::Errored;
    }
}

void DebugPluginBase::onPluginStopFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void DebugPluginBase::onPluginUnloadRequested(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Unloading;
}

void DebugPluginBase::onPluginUnloadCompleted(const std::string& name, bool success) {
    // Default implementation - can be overridden
    if (success) {
        loadStatus = PluginLoadStatus::Unloaded;
    } else {
        loadStatus = PluginLoadStatus::Errored;
    }
}

void DebugPluginBase::onPluginUnloadFailed(const std::string& name, const std::string& error) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Errored;
}

void DebugPluginBase::onPluginUpdated(const std::string& name, double deltaTime) {
    // Default implementation - can be overridden
    // Update plugin based on deltaTime
}

void DebugPluginBase::onPluginRecovered(const std::string& name) {
    // Default implementation - can be overridden
    loadStatus = PluginLoadStatus::Loaded;
}

} // namespace nlm