#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <atomic>
#include <nlohmann/json.hpp>
#include "src/core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class Config;

// Configuration Profile Structure
struct ConfigProfile {
    uint64_t id;
    std::string name;
    std::string description;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastModified;
    bool isActive;
    std::unordered_map<std::string, ConfigValue> settings;
    std::vector<ValidationResult> validationResults;
    
    ConfigProfile()
        : id(0), name(), description(), createdAt(), lastModified(), isActive(false) {}
};

// Scenario Structure
struct Scenario {
    uint64_t id;
    std::string name;
    std::string description;
    std::string category;
    std::chrono::system_clock::time_point createdAt;
    std::unordered_map<std::string, ConfigValue> settings;
    
    Scenario()
        : id(0), name(), description(), category(), createdAt() {}
};

// Change Type Enumeration
enum class ChangeType {
    Added,
    Removed,
    Changed,
    Increased,
    Decreased,
    Unchanged
};

// Difference Structure
struct Difference {
    std::string key;
    std::string value1;
    std::string value2;
    ChangeType changeType;
    
    Difference()
        : key(), value1(), value2(), changeType(ChangeType::Unchanged) {}
    Difference(const std::string& k, const std::string& v1, const std::string& v2, ChangeType ct)
        : key(k), value1(v1), value2(v2), changeType(ct) {}
};

// Difference Report Structure
struct DifferenceReport {
    std::string profile1;
    std::string profile2;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> errors;
    std::vector<Difference> differences;
    std::vector<std::string> similarKeys;
    
    DifferenceReport()
        : profile1(), profile2(), timestamp(), errors(), differences(), similarKeys() {}
};

// Optimization Result Structure
struct OptimizationResult {
    uint64_t id;
    std::string profileName;
    std::chrono::system_clock::time_point timestamp;
    bool success;
    std::string message;
    std::vector<std::string> improvements;
    std::vector<std::string> errors;
    
    OptimizationResult()
        : id(0), profileName(), timestamp(), success(false), message(), improvements(), errors() {}
};

// Validation Result Structure
struct ValidationResult {
    uint64_t id;
    std::string profileName;
    std::chrono::system_clock::time_point timestamp;
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult()
        : id(0), profileName(), timestamp(), isValid(true), errors(), warnings() {}
};

// Advanced Configuration Manager Class
class AdvancedConfigManager {
public:
    // Constructor/Destructor
    AdvancedConfigManager(std::shared_ptr<Config> config = nullptr);
    AdvancedConfigManager();
    ~AdvancedConfigManager() = default;
    
    // Disable copying, enable moving
    AdvancedConfigManager(const AdvancedConfigManager&) = delete;
    AdvancedConfigManager& operator=(const AdvancedConfigManager&) = delete;
    AdvancedConfigManager(AdvancedConfigManager&&) noexcept = default;
    AdvancedConfigManager& operator=(AdvancedConfigManager&&) noexcept = default;
    
    // Configuration Profile Management
    bool createProfile(const std::string& name, const std::string& description,
                      const std::unordered_map<std::string, ConfigValue>& settings);
    bool updateProfile(const std::string& name, const std::string& description,
                      const std::unordered_map<std::string, ConfigValue>& settings);
    bool deleteProfile(const std::string& name);
    bool switchToProfile(const std::string& name);
    
    ConfigProfile getProfile(const std::string& name) const;
    std::vector<std::string> getProfileNames() const;
    std::string getCurrentProfile() const;
    void setCurrentProfile(const std::string& profileName);
    
    // Scenario-Based Configuration
    bool applyScenario(const std::string& scenarioName,
                      const std::unordered_map<std::string, ConfigValue>& overrideSettings = {});
    bool registerScenario(const std::string& name, const std::string& description,
                         const std::unordered_map<std::string, ConfigValue>& settings,
                         const std::string& category = "default");
    
    Scenario getScenario(const std::string& name) const;
    std::vector<std::string> getScenarioNames() const;
    std::vector<std::string> getScenarioNamesByCategory(const std::string& category) const;
    
    // Configuration Validation
    ValidationResult validateProfile(const std::string& profileName) const;
    ValidationResult validateConfiguration() const;
    
    // Configuration Comparison
    DifferenceReport compareConfigurations(const std::string& profile1, const std::string& profile2) const;
    
    // Configuration Optimization
    OptimizationResult optimizeConfiguration(const std::string& profileName);
    
    // Export/Import
    bool exportToJson(const std::string& filepath, const std::string& profileName = "");
    bool importFromJson(const std::string& filepath);
    
    // Configuration Access
    std::shared_ptr<Config> getConfig() const;
    bool applyProfile(const std::string& profileName);
    
private:
    // Helper methods
    std::string configValueToString(const ConfigValue& value) const;
    ChangeType determineChangeType(const ConfigValue& oldValue, const ConfigValue& newValue) const;
    
    // Data members
    std::shared_ptr<Config> config_;
    std::string currentProfile_;
    std::unordered_map<std::string, ConfigProfile> profiles_;
    std::unordered_map<std::string, Scenario> scenarios_;
    uint64_t nextProfileId_;
    uint64_t nextScenarioId_;
    uint64_t nextValidationId_;
    uint64_t nextOptimizationId_;
    mutable std::mutex mutex_;
};

} // namespace nlm
