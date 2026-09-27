#pragma once

/**
 * Component Configuration
 * 
 * Component-specific configuration management for the NLM brain simulation project,
 * providing configuration for individual system components and their behaviors.
 * 
 * Features:
 * - Component-specific configuration management
 * - Component behavior configuration
 * - Component interaction configuration
 * - Component dependency configuration
 * - Component lifecycle configuration
 * - Component performance configuration
 * - Component monitoring configuration
 * - Component optimization configuration
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
 * Component type
 */
enum class ComponentType {
    NEURON,
    SYNAPSE,
    NEURAL_POPULATION,
    BRAIN,
    MEMORY,
    SENSORY,
    MOTOR,
    COGNITIVE,
    PLACILITY,
    DYNAMICS,
    DEVELOPMENTAL,
    NEUROMODULATION,
    BEHAVIORAL,
    PERFORMANCE,
    INTEGRATION,
    CUSTOM
};

/**
 * Component configuration metadata
 */
struct ComponentConfigMetadata {
    std::string componentName;
    ComponentType componentType;
    std::string version;
    std::string description;
    bool enabled;
    std::vector<std::string> dependencies;
    double initPriority;
    double cleanupPriority;
    
    ComponentConfigMetadata(const std::string& name, ComponentType type)
        : componentName(name), componentType(type), version("1.0"), enabled(true),
          initPriority(0.0), cleanupPriority(0.0) {}
};

/**
 * Component configuration
 */
class ComponentConfiguration {
public:
    ComponentConfiguration();
    ~ComponentConfiguration();
    
    // Disable copying, enable moving
    ComponentConfiguration(const ComponentConfiguration&) = delete;
    ComponentConfiguration& operator=(const ComponentConfiguration&) = delete;
    ComponentConfiguration(ComponentConfiguration&&) noexcept = default;
    ComponentConfiguration& operator=(ComponentConfiguration&&) noexcept = default;
    
    // Component metadata
    ComponentConfigMetadata& getMetadata() { return metadata_; }
    const ComponentConfigMetadata& getMetadata() const { return metadata_; }
    
    void setComponentName(const std::string& name) { metadata_.componentName = name; }
    void setVersion(const std::string& version) { metadata_.version = version; }
    
    // Configuration parameter management
    bool addParameter(const std::string& name, const std::string& value, const std::string& type = "string");
    bool removeParameter(const std::string& paramName);
    std::string getParameter(const std::string& paramName) const;
    std::map<std::string, std::string> getAllParameters() const { return parameters_; }
    
    // Component dependency management
    bool addDependency(const std::string& componentName);
    bool removeDependency(const std::string& componentName);
    std::vector<std::string> getDependencies() const { return dependencies_; }
    
    // Component configuration management
    bool isEnabled() const { return metadata_.enabled; }
    void setEnabled(bool enabled) { metadata_.enabled = enabled; }
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Configuration inheritance
    bool addInheritance(const std::string& parentComponentName);
    bool removeInheritance(const std::string& parentComponentName);
    std::vector<std::string> getInheritedComponents() const { return inherited_components_; }
    
    // Configuration merging
    bool merge(const ComponentConfiguration& otherConfig);
    bool applyDefaults();
    
    // Component lifecycle configuration
    double getInitPriority() const { return metadata_.initPriority; }
    void setInitPriority(double priority) { metadata_.initPriority = priority; }
    
    double getCleanupPriority() const { return metadata_.cleanupPriority; }
    void setCleanupPriority(double priority) { metadata_.cleanupPriority = priority; }
    
    // Component performance configuration
    bool configurePerformance(const std::map<std::string, double>& settings);
    std::map<std::string, double> getPerformanceSettings() const { return performance_settings_; }
    
    // Component monitoring configuration
    bool configureMonitoring(const std::map<std::string, bool>& settings);
    std::map<std::string, bool> getMonitoringSettings() const { return monitoring_settings_; }
    
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
    ComponentConfigMetadata metadata_;
    
    std::map<std::string, std::string> parameters_;
    std::vector<std::string> dependencies_;
    std::vector<std::string> inherited_components_;
    std::map<std::string, double> performance_settings_;
    std::map<std::string, bool> monitoring_settings_;
    
    friend class ConfigurationManager;
};

} // namespace system
} // namespace nlm
