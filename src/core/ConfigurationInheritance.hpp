#pragma once

/**
 * Configuration Inheritance
 * 
 * Configuration inheritance system for managing hierarchical configuration with inheritance,
 * providing support for parent-child configuration relationships and inheritance resolution
 * for the NLM brain simulation project.
 * 
 * Features:
 * - Hierarchical configuration inheritance
 * - Parent-child relationship management
 * - Inheritance resolution
 * - Inheritance validation
 * - Inheritance conflicts detection
 * - Inheritance override management
 * - Inheritance dependency management
 * - Inheritance performance optimization
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
namespace core {

class ConfigurationManager;

/**
 * Inheritance type
 */
enum class InheritanceType {
    SIMPLE,
    DEEP,
    SHALLOW,
    OVERRIDE,
    MERGE,
    CONFLICT_RESOLVE
};

/**
 * Inheritance rule
 */
struct InheritanceRule {
    std::string childKey;
    std::string parentKey;
    InheritanceType type;
    bool enabled;
    std::string description;
    
    InheritanceRule(const std::string& child, const std::string& parent, InheritanceType t)
        : childKey(child), parentKey(parent), type(t), enabled(true) {}
};

/**
 * Inheritance conflict
 */
struct InheritanceConflict {
    std::string key;
    std::string childValue;
    std::string parentValue;
    ConflictType conflictType;
    std::string resolutionStrategy;
    
    InheritanceConflict(const std::string& k, const std::string& child, const std::string& parent,
                       ConflictType type)
        : key(k), childValue(child), parentValue(parent), conflictType(type) {}
};

/**
 * Configuration inheritance
 */
class ConfigurationInheritance {
public:
    ConfigurationInheritance();
    ~ConfigurationInheritance();
    
    // Disable copying, enable moving
    ConfigurationInheritance(const ConfigurationInheritance&) = delete;
    ConfigurationInheritance& operator=(const ConfigurationInheritance&) = delete;
    ConfigurationInheritance(ConfigurationInheritance&&) noexcept = default;
    ConfigurationInheritance& operator=(ConfigurationInheritance&&) noexcept = default;
    
    // Inheritance rule management
    bool addInheritanceRule(const InheritanceRule& rule);
    bool removeInheritanceRule(const std::string& childKey);
    InheritanceRule* getInheritanceRule(const std::string& childKey);
    const InheritanceRule* getInheritanceRule(const std::string& childKey) const;
    std::vector<std::string> getChildKeys() const;
    
    // Inheritance resolution
    bool resolveInheritance(ConfigurationManager& configManager) const;
    bool resolveChildKey(const std::string& childKey, ConfigurationManager& configManager) const;
    
    // Inheritance validation
    bool validateInheritance(ConfigurationManager& configManager) const;
    std::vector<InheritanceConflict> getConflicts(ConfigurationManager& configManager) const;
    
    // Inheritance conflict resolution
    bool resolveConflict(const InheritanceConflict& conflict, ConfigurationManager& configManager);
    bool autoResolveConflicts(ConfigurationManager& configManager);
    
    // Inheritance monitoring
    bool enableInheritanceMonitoring(const std::string& childKey, bool enable);
    bool isInheritanceMonitoringEnabled(const std::string& childKey) const;
    
    // Inheritance statistics
    size_t getInheritanceRuleCount() const { return inheritance_rules_.size(); }
    size_t getActiveInheritanceRuleCount() const;
    
    // Inheritance loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Inheritance merging
    bool merge(const ConfigurationInheritance& otherInheritance);
    
    // Inheritance performance optimization
    void optimizeInheritance(ConfigurationManager& configManager);
    bool isInheritanceOptimized(const std::string& childKey) const;
    
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
    
    std::map<std::string, InheritanceRule> inheritance_rules_;
    std::map<std::string, bool> monitoring_enabled_;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
