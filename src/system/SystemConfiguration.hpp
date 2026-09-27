#pragma once

/**
 * System Configuration
 * 
 * System-wide configuration management for the NLM brain simulation project,
 * providing centralized configuration for system-wide settings and behaviors.
 * 
 * Features:
 * - System-wide configuration management
 * - Global configuration settings
 * - System behavior configuration
 * - System performance configuration
 * - System monitoring configuration
 * - System logging configuration
 * - System security configuration
 * - System resource configuration
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
 * System configuration section
 */
enum class SystemConfigSection {
    GENERAL,
    PERFORMANCE,
    MEMORY,
    NETWORK,
    SECURITY,
    LOGGING,
    MONITORING,
    DEBUGGING,
    BACKUP,
    INTEGRATION
};

/**
 * System configuration metadata
 */
struct SystemConfigMetadata {
    std::string sectionName;
    SystemConfigSection section;
    std::string description;
    bool enabled;
    double defaultValue;
    std::string unit;
    std::string dataType;
    
    SystemConfigMetadata(const std::string& name, SystemConfigSection section)
        : sectionName(name), section(section), enabled(true), defaultValue(0.0) {}
};

/**
 * System configuration
 */
class SystemConfiguration {
public:
    SystemConfiguration();
    ~SystemConfiguration();
    
    // Disable copying, enable moving
    SystemConfiguration(const SystemConfiguration&) = delete;
    SystemConfiguration& operator=(const SystemConfiguration&) = delete;
    SystemConfiguration(SystemConfiguration&&) noexcept = default;
    SystemConfiguration& operator=(SystemConfiguration&&) noexcept = default;
    
    // Configuration section management
    bool addSection(const SystemConfigMetadata& section);
    bool removeSection(const std::string& sectionName);
    SystemConfigMetadata* getSection(const std::string& sectionName);
    const SystemConfigMetadata* getSection(const std::string& sectionName) const;
    std::vector<std::string> getSectionNames() const;
    
    // Configuration value management
    void setValue(const std::string& sectionName, const std::string& key, const std::string& value);
    std::string getValue(const std::string& sectionName, const std::string& key) const;
    bool hasValue(const std::string& sectionName, const std::string& key) const;
    
    // Configuration type management
    void setValue(const std::string& sectionName, const std::string& key, int value);
    int getInt(const std::string& sectionName, const std::string& key) const;
    
    void setValue(const std::string& sectionName, const std::string& key, double value);
    double getDouble(const std::string& sectionName, const std::string& key) const;
    
    void setValue(const std::string& sectionName, const std::string& key, bool value);
    bool getBool(const std::string& sectionName, const std::string& key) const;
    
    // Configuration validation
    bool validateSection(const std::string& sectionName) const;
    std::vector<std::string> getSectionErrors(const std::string& sectionName) const;
    
    // Configuration loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Configuration inheritance
    bool addInheritance(const std::string& parentSectionName);
    bool removeInheritance(const std::string& parentSectionName);
    std::vector<std::string> getInheritedSections() const;
    
    // Configuration merging
    bool merge(const SystemConfiguration& otherConfig);
    bool applyDefaults();
    
    // Configuration statistics
    size_t getSectionCount() const { return sections_.size(); }
    size_t getTotalKeyCount() const;
    
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
    
    std::map<std::string, SystemConfigMetadata> sections_;
    std::map<std::string, std::map<std::string, std::string>> sectionValues_;
    
    friend class ConfigurationManager;
};

} // namespace system
} // namespace nlm
