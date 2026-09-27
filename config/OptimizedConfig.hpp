#pragma once

#include "OptimizedConfig.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <cmath>

namespace nlm {

class OptimizedConfig {
public:
    struct ConfigEntry {
        std::string key;
        std::string value;
        std::string type;
        bool validated;
        std::chrono::steady_clock::time_point timestamp;
        std::atomic<size_t> accessCount{0};
        mutable bool cached{false};
        mutable std::string cachedValue;
        
        ConfigEntry() : validated(false), timestamp(std::chrono::steady_clock::now()) {}
        ConfigEntry(const std::string& k, const std::string& v, const std::string& t)
            : key(k), value(v), type(t), validated(true), 
              timestamp(std::chrono::steady_clock::now()), cached(false) {}
    };

    OptimizedConfig();
    ~OptimizedConfig();

    // Disable copying, enable moving
    OptimizedConfig(const OptimizedConfig&) = delete;
    OptimizedConfig& operator=(const OptimizedConfig&) = delete;
    OptimizedConfig(OptimizedConfig&&) noexcept;
    OptimizedConfig& operator=(OptimizedConfig&&) noexcept;

    // Configuration management
    void load(const std::string& filepath);
    void save(const std::string& filepath) const;
    
    // Value access with caching
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    bool tryGet(const std::string& key, T& value) const;
    
    // Configuration validation
    bool validate() const;
    bool validateSchema(const std::string& schema) const;
    std::vector<std::string> getValidationErrors() const;

    // Configuration management
    void set(const std::string& key, const std::string& value);
    void remove(const std::string& key);
    bool contains(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;

    // Optimization
    void enableCaching(bool enable);
    void setCacheSize(size_t maxSize);
    void enableLazyLoading(bool enable);
    void optimizeAccessPattern(const std::vector<std::string>& pattern);

    // Statistics
    size_t getTotalKeys() const;
    size_t getCachedKeys() const;
    float getValidationRatio() const;
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
