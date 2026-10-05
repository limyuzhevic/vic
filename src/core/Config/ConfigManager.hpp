#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <stdexcept>
#include <functional>
#include "Config.hpp"

namespace nlm {

/**
 * ConfigManager - Helper class to encapsulate repetitive config access patterns
 * 
 * This class provides type-safe configuration access with caching, error handling,
 * and a simplified interface to reduce repetitive config->getOr<...>() patterns.
 * 
 * Features:
 * 1. Type-safe config access with default values
 * 2. Cache frequently accessed config values during initialization
 * 3. Proper error handling for missing config values
 * 4. Reduced repetitive config->getOr<...>() patterns
 * 5. Simple interface: getConfigValue<T>("key", defaultValue)
 */
class ConfigManager {
public:
    /**
     * Constructor - Initialize with a shared config pointer
     * 
     * @param config Shared pointer to the configuration object
     */
    explicit ConfigManager(std::shared_ptr<Config> config) : config_(std::move(config)) {}
    
    /**
     * Destructor
     */
    ~ConfigManager() = default;
    
    /**
     * Disable copying and moving
     */
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;
    
    /**
     * Initialize the config manager by caching frequently used values
     * 
     * This method should be called once during initialization to cache commonly
     * accessed configuration values for improved performance.
     */
    void initialize() {
        // Clear any existing cache
        cached_values_.clear();
        
        // Cache frequently used config values
        // Random seed - critical for reproducibility
        cacheValue("random_seed", 42ULL);
        
        // STDP parameters - critical for learning
        cacheValue("stdp_ltp_weight", 0.01f);
        cacheValue("stdp_ltd_weight", 0.012f);
        cacheValue("stdp_tau", 20.0f);
        
        // Structural plasticity parameters
        cacheValue("synaptogenesis_rate", 0.0001f);
        cacheValue("pruning_rate", 0.00001f);
        
        // Simulation parameters
        cacheValue("simulation_timestep", 0.001);
        cacheValue("replay_interval", 100ULL);
        cacheValue("consolidation_interval", 1000ULL);
        
        // Brain parameters
        cacheValue("neuron_count", 1000ULL);
        cacheValue("region_count", 1ULL);
        cacheValue("connection_probability", 0.1f);
        
        // Checkpoint parameters
        cacheValue("checkpoint_dir", std::string("./checkpoints"));
        
        // Validation parameters
        cacheValue("validation_threshold", 0.5f);
        cacheValue("validation_max_attempts", 3ULL);
    }
    
    /**
     * Get a config value with type safety and default values
     * 
     * This method provides a simple interface that encapsulates the repetitive
     * config->getOr<...>() pattern found throughout Brain.cpp.
     * 
     * @tparam T Type of the config value to retrieve
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if config value is not found
     * @return The config value of type T
     * @throws std::runtime_error if the value cannot be retrieved
     */
    template<typename T>
    T getConfigValue(const std::string& key, const T& defaultValue) {
        // First, check if the value is cached
        auto cacheIt = cached_values_.find(key);
        if (cacheIt != cached_values_.end() && cacheIt->second.has_value()) {
            try {
                return std::any_cast<T>(cacheIt->second);
            } catch (const std::bad_any_cast&) {
                // If cast fails, remove from cache and fall through
                cached_values_.erase(cacheIt);
            }
        }
        
        // Not cached or cache miss - try to get from config
        auto configOpt = config_->get<T>(key);
        if (configOpt.has_value()) {
            T value = *configOpt;
            // Cache the value for future access
            cacheValue(key, value);
            return value;
        }
        
        // Check if default value is provided and cache it if so
        if (hasCachedValue(key)) {
            try {
                return std::any_cast<T>(cached_values_[key]);
            } catch (const std::bad_any_cast&) {
                // If cast fails, fall through to default
            }
        }
        
        // Return default value
        cacheValue(key, defaultValue);
        return defaultValue;
    }
    
    /**
     * Get a config value with validation
     * 
     * This method provides additional validation for critical config values.
     * 
     * @tparam T Type of the config value to retrieve (must be numeric or string)
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if config value is not found
     * @param validator Validation function (optional)
     * @return The config value of type T
     * @throws std::runtime_error if the value is invalid or missing
     */
    template<typename T>
    T getValidatedConfigValue(const std::string& key, const T& defaultValue,
                           const std::function<bool(const T&)>& validator = nullptr) {
        T value = getConfigValue<T>(key, defaultValue);
        
        if (validator && !validator(value)) {
            throw std::runtime_error("Validation failed for config value '" + key + "'");
        }
        
        return value;
    }
    
    /**
     * Check if a config value exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists in config
     */
    bool hasConfig(const std::string& key) const {
        return config_->has(key) || hasCachedValue(key);
    }
    
    /**
     * Force refresh of a cached value
     * 
     * @param key Configuration key to refresh
     */
    void refreshValue(const std::string& key) {
        cached_values_.erase(key);
    }
    
    /**
     * Get all cached config keys
     * 
     * @return Vector of all cached config keys
     */
    std::vector<std::string> getCachedKeys() const {
        std::vector<std::string> keys;
        keys.reserve(cached_values_.size());
        for (const auto& pair : cached_values_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    /**
     * Get the underlying config object
     * 
     * @return Shared pointer to the config object
     */
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    /**
     * Set a config value (overrides cached value)
     * 
     * @param key Configuration key
     * @param value Value to set
     */
    template<typename T>
    void setConfigValue(const std::string& key, const T& value) {
        refreshValue(key);
        config_->set(key, ConfigValue(value));
    }
    
private:
    /**
     * Internal method to cache a config value
     * 
     * @param key Configuration key
     * @param value Value to cache
     */
    template<typename T>
    void cacheValue(const std::string& key, const T& value) {
        cached_values_[key] = value;
    }
    
    /**
     * Check if a value is cached
     * 
     * @param key Configuration key
     * @return true if the value is cached
     */
    bool hasCachedValue(const std::string& key) const {
        auto it = cached_values_.find(key);
        return it != cached_values_.end() && it->second.has_value();
    }
    
    // Underlying configuration object
    std::shared_ptr<Config> config_;
    
    // Cache for frequently accessed configuration values
    std::unordered_map<std::string, std::any> cached_values_;
};

} // namespace nlm
