#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <memory>
#include <vector>
#include <mutex>
#include "PluginAPI.hpp"

namespace nlm {

class PluginConfig {
public:
    PluginConfig() = default;
    virtual ~PluginConfig() = default;
    
    // Get value by key with type conversion
    template<typename T>
    T get(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = config_.find(key);
        if (it == config_.end()) {
            throw PluginException("Configuration key not found: " + key);
        }
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            throw PluginException("Configuration value type mismatch for key: " + key);
        }
    }
    
    // Get value by key with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = config_.find(key);
        if (it == config_.end()) {
            return defaultValue;
        }
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    
    // Set value by key
    template<typename T>
    void set(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_[key] = value;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_.find(key) != config_.end();
    }
    
    // Remove key
    void remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_.erase(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> keys;
        keys.reserve(config_.size());
        for (const auto& pair : config_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Clear all configuration
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        config_.clear();
    }
    
    // Merge another configuration
    void merge(const PluginConfig& other) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : other.config_) {
            config_[pair.first] = pair.second;
        }
    }
    
    // Get configuration as JSON string (placeholder)
    std::string toJSON() const {
        // In a real implementation, use a proper JSON library
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& pair : config_) {
            if (!first) ss << ", ";
            first = false;
            ss << "\"" << pair.first << "\": \"" << std::any_cast<std::string>(pair.second) << "\"";
        }
        ss << "}";
        return ss.str();
    }
    
    // Load configuration from file (placeholder)
    bool loadFromFile(const std::string& filepath) {
        // In a real implementation, parse configuration file
        return false;
    }
    
    // Save configuration to file (placeholder)
    bool saveToFile(const std::string& filepath) const {
        // In a real implementation, write configuration file
        return false;
    }
    
    // Get configuration value as string
    std::string getString(const std::string& key) const {
        return get<std::string>(key);
    }
    
    // Get configuration value as integer
    int getInt(const std::string& key) const {
        return get<int>(key);
    }
    
    // Get configuration value as float
    float getFloat(const std::string& key) const {
        return get<float>(key);
    }
    
    // Get configuration value as boolean
    bool getBool(const std::string& key) const {
        return get<bool>(key);
    }
    
    // Set configuration value as string
    void setString(const std::string& key, const std::string& value) {
        set<std::string>(key, value);
    }
    
    // Set configuration value as integer
    void setInt(const std::string& key, int value) {
        set<int>(key, value);
    }
    
    // Set configuration value as float
    void setFloat(const std::string& key, float value) {
        set<float>(key, value);
    }
    
    // Set configuration value as boolean
    void setBool(const std::string& key, bool value) {
        set<bool>(key, value);
    }
    
protected:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::any> config_;
};

} // namespace nlm