#include "MemoryPlugin.hpp"

namespace nlm {
namespace plugin {

MemoryPlugin::MemoryPlugin() : state_(PluginState::UNLOADED) {}

MemoryPlugin::~MemoryPlugin() {
    for (auto* memory : memorySystems_) {
        delete memory;
    }
    memorySystems_.clear();
}

std::string MemoryPlugin::getName() const {
    return metadata_.name;
}

std::string MemoryPlugin::getVersion() const {
    return metadata_.version.toString();
}

std::string MemoryPlugin::getDescription() const {
    return metadata_.description;
}

PluginType MemoryPlugin::getType() const {
    return metadata_.type;
}

PluginMetadata MemoryPlugin::getMetadata() const {
    return metadata_;
}

bool MemoryPlugin::initialize(const PluginMetadata& metadata) {
    state_ = PluginState::INITIALIZING;
    
    try {
        metadata_ = metadata;
        
        if (!validateComponentConfiguration()) {
            error_ = "Component configuration validation failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        if (!loadDependencies()) {
            error_ = "Failed to load dependencies";
            state_ = PluginState::ERROR;
            return false;
        }
        
        if (!preInitialize()) {
            error_ = "Pre-initialization failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        // Initialize memory systems based on configuration
        for (const auto& memoryName : config_.memoryNames) {
            memorySystems_.push_back(createMemorySystem(memoryName));
        }
        
        if (!postInitialize()) {
            error_ = "Post-initialization failed";
            state_ = PluginState::ERROR;
            return false;
        }
        
        state_ = PluginState::INITIALIZED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during initialization: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

void MemoryPlugin::shutdown() {
    state_ = PluginState::SHUTDOWN;
    
    if (!preShutdown()) {
        error_ = "Pre-shutdown failed";
        return;
    }
    
    for (auto* memory : memorySystems_) {
        delete memory;
    }
    memorySystems_.clear();
    
    if (!postShutdown()) {
        error_ = "Post-shutdown failed";
        return;
    }
    
    metadata_ = PluginMetadata();
    config_ = MemoryConfig();
    extensions_.clear();
    componentConnections_.clear();
}

bool MemoryPlugin::activate() {
    state_ = PluginState::ACTIVATING;
    
    try {
        if (state_ != PluginState::INITIALIZED) {
            error_ = "Plugin not initialized";
            state_ = PluginState::ERROR;
            return false;
        }
        
        for (auto* memory : memorySystems_) {
            // Reset memory system if needed
        }
        
        state_ = PluginState::ACTIVATED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during activation: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

bool MemoryPlugin::deactivate() {
    state_ = PluginState::DEACTIVATING;
    
    try {
        for (auto* memory : memorySystems_) {
            // Deactivate memory system if needed
        }
        
        state_ = PluginState::DEACTIVATED;
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception during deactivation: ") + e.what();
        state_ = PluginState::ERROR;
        return false;
    }
}

bool MemoryPlugin::loadDependencies() {
    // Load plugin dependencies
    for (const auto& dependency : metadata_.dependencies) {
        // Implement dependency loading logic
    }
    return true;
}

bool MemoryPlugin::checkConflicts() {
    // Check for plugin conflicts
    return true;
}

MemoryPlugin::MemorySystem::MemorySystem(const std::string& name, MemoryPlugin* plugin) 
    : name_(name), plugin_(plugin) {
    reset();
}

MemoryPlugin::MemorySystem::~MemorySystem() {}

bool MemoryPlugin::MemorySystem::store(const std::string& key, const std::string& value) {
    if (data_.size() >= static_cast<size_t>(capacity_)) {
        return false; // Memory full
    }
    
    data_[key] = value;
    currentLoad_ = data_.size() / static_cast<double>(capacity_);
    return true;
}

bool MemoryPlugin::MemorySystem::retrieve(const std::string& key, std::string& value) {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return false;
    }
    
    value = it->second;
    return true;
}

bool MemoryPlugin::MemorySystem::remove(const std::string& key) {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return false;
    }
    
    data_.erase(it);
    currentLoad_ = data_.size() / static_cast<double>(capacity_);
    return true;
}

bool MemoryPlugin::MemorySystem::clear() {
    data_.clear();
    currentLoad_ = 0.0;
    return true;
}

std::vector<std::string> MemoryPlugin::MemorySystem::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(data_.size());
    
    for (const auto& pair : data_) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

MemoryPlugin::MemorySystem* MemoryPlugin::createMemorySystem(const std::string& name) {
    return new MemorySystem(name, this);
}

bool MemoryPlugin::destroyMemorySystem(MemorySystem* memory) {
    if (!memory) return false;
    
    for (auto it = memorySystems_.begin(); it != memorySystems_.end(); ++it) {
        if (*it == memory) {
            delete *it;
            memorySystems_.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<MemoryPlugin::MemorySystem*> MemoryPlugin::getMemorySystems() const {
    return memorySystems_;
}

MemoryPlugin::MemorySystem* MemoryPlugin::getMemorySystem(const std::string& name) const {
    for (auto* memory : memorySystems_) {
        if (memory->getName() == name) {
            return memory;
        }
    }
    return nullptr;
}

} // namespace plugin
} // namespace nlm