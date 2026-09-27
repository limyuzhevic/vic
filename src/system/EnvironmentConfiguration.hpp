#pragma once

/**
 * Environment Configuration
 * 
 * Environment-specific configuration management for the NLM brain simulation project,
 * providing configuration for different runtime environments (development, testing, production).
 * 
 * Features:
 * - Environment-specific configuration management
 * - Development environment configuration
 * - Testing environment configuration
 * - Production environment configuration
 * - Environment inheritance
 * - Environment-specific validation
 * - Environment switching
 * - Environment-specific logging
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace system {

class ConfigurationManager;

/**
 * Environment type
 */
enum class EnvironmentType {
    DEVELOPMENT,
    TESTING,
    STAGING,
    PRODUCTION,
    QA,
    CI_CD,
    CUSTOM
};

/**
 * Environment configuration metadata
 */
struct EnvironmentConfigMetadata {
    std::string environmentName;
    EnvironmentType environmentType;
    std::string description;
    std::string version;
    bool isActive;
    double priority;
    
    EnvironmentConfigMetadata(const std::string& name, EnvironmentType type)
        : environmentName(name), environmentType(type), isActive(false), priority(0.0) {}
};

/**
 * Environment configuration
 */
class EnvironmentConfiguration {
public:
    EnvironmentConfiguration();
    ~EnvironmentConfiguration();
    
    // Disable copying, enable moving
    EnvironmentConfiguration(const EnvironmentConfiguration&) = delete;
    EnvironmentConfiguration& operator=(const EnvironmentConfiguration&) = delete;
    EnvironmentConfiguration(EnvironmentConfiguration&&) noexcept = default;
    EnvironmentConfiguration& operator=(EnvironmentConfiguration&&) noexcept = default;
    
    // Environment management
    bool addEnvironment(const EnvironmentConfigMetadata& environment);
    bool removeEnvironment(const std::string& environmentName);
    EnvironmentConfigMetadata* getEnvironment(const std::string& environmentName);
    const EnvironmentConfigMetadata* getEnvironment(const std::string& environmentName) const;
    std::vector<std::string> getEnvironmentNames() const;
    
    // Environment switching
    bool activateEnvironment(const std::string& environmentName);
    std::string getActiveEnvironment() const { return active_environment_; }
    std::vector<std::string> getAllActiveEnvironments() const { return active_environments_; }
    
    // Environment configuration inheritance
    bool addInheritance(const std::string& parentEnvironmentName);
    bool removeInheritance(const std::string& parentEnvironmentName);
    std::vector<std::string> getInheritedEnvironments() const { return inherited_environments_; }
    
    // Environment configuration values
    void setValue(const std::string& environmentName, const std::string& key, const std::string& value);
    std::string getValue(const std::string& environmentName, const std::string& key) const;
    bool hasValue(const std::string& environmentName, const std::string& key) const;
    
    // Environment-specific configuration
    bool configureDevelopment(const std::map<std::string, std::string>& settings);
    bool configureTesting(const std::map<std::string, std::string>& settings);
    bool configureProduction(const std::map<std::string, std::string>& settings);
    
    // Environment validation
    bool validateEnvironment(const std::string& environmentName) const;
    std::vector<std::string> getEnvironmentErrors(const std::string& environmentName) const;
    
    // Environment loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Environment merging
    bool merge(const EnvironmentConfiguration& otherConfig);
    bool applyDefaults();
    
    // Environment statistics
    size_t getEnvironmentCount() const { return environments_.size(); }
    size_t getActiveEnvironmentCount() const { return active_environments_.size(); }
    
    // Environment monitoring
    bool enableEnvironmentMonitoring(const std::string& environmentName, bool enable);
    bool isEnvironmentMonitoringEnabled(const std::string& environmentName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    std::map<std::string, EnvironmentConfigMetadata> environments_;
    std::map<std::string, std::map<std::string, std::string>> environmentValues_;
    std::string active_environment_;
    std::vector<std::string> active_environments_;
    std::vector<std::string> inherited_environments_;
    
    friend class ConfigurationManager;
};

} // namespace system
} // namespace nlm
