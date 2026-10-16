/*
 * Plugin Base Class Implementation
 * 
 * This file contains the implementation of the base plugin classes
 * and thread-safe plugin functionality.
 */

#include "PluginBase.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {
namespace plugin {

// ThreadSafePlugin implementation
ThreadSafePlugin::ThreadSafePlugin()
    : status_(PluginStatus::UNLOADED)
    , enabled_(true) {
    NLM_LOG_INFO("ThreadSafePlugin: Created plugin instance " + getName());
}

ThreadSafePlugin::~ThreadSafePlugin() {
    NLM_LOG_INFO("ThreadSafePlugin: Destroying plugin instance " + getName());
}

const PluginMetadata& ThreadSafePlugin::getMetadata() const {
    return metadata_;
}

const std::string& ThreadSafePlugin::getName() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metadata_.name;
}

const std::string& ThreadSafePlugin::getVersion() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metadata_.version;
}

PluginStatus ThreadSafePlugin::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

bool ThreadSafePlugin::load(const nlohmann::json& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PluginStatus::UNLOADED && status_ != PluginStatus::FAILED) {
        addError("ThreadSafePlugin: Cannot load plugin - not in UNLOADED or FAILED state");
        return false;
    }
    
    status_ = PluginStatus::LOADING;
    
    try {
        // Validate configuration
        if (!validateConfig(config)) {
            addError("ThreadSafePlugin: Plugin configuration validation failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        // Store configuration
        config_ = config;
        
        // Call subclass-specific load method
        if (!onLoad(config)) {
            addError("ThreadSafePlugin: Plugin-specific load operation failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        status_ = PluginStatus::LOADED;
        onPhaseChanged(PluginPhase::POST_INIT, PluginPhase::PRE_RUN);
        
        NLM_LOG_INFO("ThreadSafePlugin: Successfully loaded plugin '" + getName() + "'");
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception during plugin load: " + std::string(e.what()));
        status_ = PluginStatus::FAILED;
        return false;
    }
}

bool ThreadSafePlugin::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PluginStatus::LOADED) {
        addError("ThreadSafePlugin: Cannot initialize plugin - not in LOADED state");
        return false;
    }
    
    status_ = PluginStatus::INITIALIZING;
    
    try {
        // Call subclass-specific initialization
        if (!onInitialize()) {
            addError("ThreadSafePlugin: Plugin-specific initialization failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        status_ = PluginStatus::RUNNING;
        onPhaseChanged(PluginPhase::PRE_RUN, PluginPhase::RUNNING);
        
        NLM_LOG_INFO("ThreadSafePlugin: Successfully initialized plugin '" + getName() + "'");
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception during plugin initialization: " + std::string(e.what()));
        status_ = PluginStatus::FAILED;
        return false;
    }
}

bool ThreadSafePlugin::run() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PluginStatus::RUNNING) {
        addError("ThreadSafePlugin: Cannot run plugin - not in RUNNING state");
        return false;
    }
    
    try {
        // Call subclass-specific run method
        if (!onRun()) {
            addError("ThreadSafePlugin: Plugin-specific run operation failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        NLM_LOG_DEBUG("ThreadSafePlugin: Plugin '" + getName() + "' completed run cycle");
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception during plugin run: " + std::string(e.what()));
        status_ = PluginStatus::FAILED;
        return false;
    }
}

bool ThreadSafePlugin::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PluginStatus::RUNNING) {
        return true; // Already stopped
    }
    
    status_ = PluginStatus::SHUTTING_DOWN;
    onPhaseChanged(PluginPhase::RUNNING, PluginPhase::PRE_SHUTDOWN);
    
    try {
        // Call subclass-specific shutdown
        if (!onShutdown()) {
            addError("ThreadSafePlugin: Plugin-specific shutdown failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        status_ = PluginStatus::INITIALIZING; // Back to init state after shutdown
        onPhaseChanged(PluginPhase::PRE_SHUTDOWN, PluginPhase::SHUTDOWN);
        
        NLM_LOG_INFO("ThreadSafePlugin: Successfully shut down plugin '" + getName() + "'");
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception during plugin shutdown: " + std::string(e.what()));
        status_ = PluginStatus::FAILED;
        return false;
    }
}

bool ThreadSafePlugin::unload() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (status_ != PluginStatus::LOADED && status_ != PluginStatus::FAILED) {
        return true; // Already unloaded
    }
    
    status_ = PluginStatus::UNLOADING;
    
    try {
        // Call subclass-specific unload
        if (!onUnload()) {
            addError("ThreadSafePlugin: Plugin-specific unload operation failed");
            status_ = PluginStatus::FAILED;
            return false;
        }
        
        // Clear configuration and state
        config_ = nlohmann::json();
        state_ = nlohmann::json();
        clearErrors();
        
        status_ = PluginStatus::UNLOADED;
        onPhaseChanged(PluginPhase::SHUTDOWN, PluginPhase::POST_SHUTDOWN);
        
        NLM_LOG_INFO("ThreadSafePlugin: Successfully unloaded plugin '" + getName() + "'");
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception during plugin unload: " + std::string(e.what()));
        status_ = PluginStatus::FAILED;
        return false;
    }
}

void ThreadSafePlugin::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool ThreadSafePlugin::isEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_ && (status_ == PluginStatus::RUNNING || status_ == PluginStatus::LOADED);
}

nlohmann::json ThreadSafePlugin::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

bool ThreadSafePlugin::setState(const nlohmann::json& state) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        state_ = state;
        onStateChange();
        return true;
    } catch (const std::exception& e) {
        addError("ThreadSafePlugin: Exception setting plugin state: " + std::string(e.what()));
        return false;
    }
}

bool ThreadSafePlugin::hasError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !errors_.empty();
}

std::string ThreadSafePlugin::getError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (errors_.empty()) return "";
    return errors_.back();
}

std::vector<std::string> ThreadSafePlugin::getErrors() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return errors_;
}

nlohmann::json ThreadSafePlugin::getDefaultConfig() const {
    return onGetDefaultConfig();
}

bool ThreadSafePlugin::validateConfig(const nlohmann::json& config) {
    return onValidateConfig(config);
}

void ThreadSafePlugin::lock() {
    mutex_.lock();
}

void ThreadSafePlugin::unlock() {
    mutex_.unlock();
}

bool ThreadSafePlugin::tryLock() {
    return mutex_.try_lock();
}

void ThreadSafePlugin::addError(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    errors_.push_back(error);
    
    NLM_LOG_ERROR("ThreadSafePlugin: " + getName() + " - " + error);
}

void ThreadSafePlugin::clearErrors() {
    std::lock_guard<std::mutex> lock(mutex_);
    errors_.clear();
}

std::string ThreadSafePlugin::getCurrentPhaseAsString() const {
    // This is a helper method, implementation details would depend on specific needs
    return "UNKNOWN";
}

// Default implementations for virtual methods
bool ThreadSafePlugin::onLoad(const nlohmann::json& config) {
    return true;
}

bool ThreadSafePlugin::onInitialize() {
    return true;
}

bool ThreadSafePlugin::onRun() {
    return true;
}

bool ThreadSafePlugin::onShutdown() {
    return true;
}

bool ThreadSafePlugin::onUnload() {
    return true;
}

nlohmann::json ThreadSafePlugin::onGetDefaultConfig() const {
    return nlohmann::json();
}

bool ThreadSafePlugin::onValidateConfig(const nlohmann::json& config) {
    return true;
}

}}} // namespace nlm::plugin
