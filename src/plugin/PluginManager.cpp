// PluginManager.cpp
#include "PluginManager.hpp"
#include <stdexcept>
#include <chrono>

namespace nlm {

template<typename T>
bool PluginManager::registerPlugin(const std::string& name,
                                   const std::string& version,
                                   const std::string& description,
                                   const std::string& author,
                                   const std::function<std::unique_ptr<T>()>& factory) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (hasPlugin(name)) {
        lastError_ = "Plugin with name '" + name + "' already registered";
        return false;
    }

    auto plugin = factory();
    if (!plugin) {
        lastError_ = "Failed to create plugin instance for '" + name + "'";
        return false;
    }

    plugins_[name] = PluginEntry{
        std::move(plugin),
        false, // not initialized
        false, // not enabled
        Config(), // empty config
        "",
        std::chrono::steady_clock::now()
    };

    return true;
}

bool PluginManager::initializePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin '" + name + "' not found";
        return false;
    }

    auto& entry = it->second;
    if (entry.initialized) {
        return true; // already initialized
    }

    try {
        if (!entry.plugin->initialize(entry.configuration)) {
            lastError_ = "Plugin '" + name + "' failed to initialize";
            return false;
        }
        entry.initialized = true;
        return true;
    } catch (const std::exception& e) {
        lastError_ = "Plugin '" + name + "' initialization exception: " + std::string(e.what());
        return false;
    }
}

bool PluginManager::shutdownPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin '" + name + "' not found";
        return false;
    }

    auto& entry = it->second;
    if (!entry.initialized) {
        return true; // already shutdown or not initialized
    }

    try {
        entry.plugin->shutdown();
        entry.initialized = false;
        return true;
    } catch (const std::exception& e) {
        lastError_ = "Plugin '" + name + "' shutdown exception: " + std::string(e.what());
        return false;
    }
}

bool PluginManager::enablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin '" + name + "' not found";
        return false;
    }

    auto& entry = it->second;
    if (entry.enabled) {
        return true; // already enabled
    }

    entry.enabled = true;
    return true;
}

bool PluginManager::disablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        lastError_ = "Plugin '" + name + "' not found";
        return false;
    }

    auto& entry = it->second;
    if (!entry.enabled) {
        return true; // already disabled
    }

    entry.enabled = false;
    return true;
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    return names;
}

bool PluginManager::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.find(name) != plugins_.end();
}

std::string PluginManager::getPluginVersion(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return "";
    }
    return it->second.plugin->getVersion();
}

std::string PluginManager::getPluginDescription(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return "";
    }
    return it->second.plugin->getDescription();
}

bool PluginManager::initializeAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = true;
    for (auto& pair : plugins_) {
        auto& entry = pair.second;
        if (entry.enabled && !entry.initialized) {
            if (!initializePlugin(pair.first)) {
                success = false;
            }
        }
    }
    return success;
}

bool PluginManager::shutdownAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = true;
    for (auto& pair : plugins_) {
        auto& entry = pair.second;
        if (entry.initialized) {
            if (!shutdownPlugin(pair.first)) {
                success = false;
            }
        }
    }
    return success;
}

bool PluginManager::enableAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        pair.second.enabled = true;
    }
    return true;
}

bool PluginManager::disableAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : plugins_) {
        pair.second.enabled = false;
    }
    return true;
}

size_t PluginManager::getInitializedPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : plugins_) {
        if (pair.second.initialized) {
            count++;
        }
    }
    return count;
}

size_t PluginManager::getEnabledPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : plugins_) {
        if (pair.second.enabled) {
            count++;
        }
    }
    return count;
}

size_t PluginManager::getTotalPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.size();
}

std::string PluginManager::getLastError() const {
    return lastError_;
}

} // namespace nlm