#pragma once

// ============================================================================
// Enhanced Experiment System with Plugin Architecture, Configuration, and Iterator Support
// ============================================================================

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <any>
#include <iterator>
#include <map>
#include <set>
#include <chrono>
#include <optional>
#include <stdexcept>

namespace nlm {

// ============================================================================
// Experiment Plugin Architecture
// ============================================================================

// Experiment base class for plugin system
class ExperimentBase {
public:
    virtual ~ExperimentBase() = default;
    
    // Experiment metadata
    virtual const std::string& getName() const = 0;
    virtual void setName(const std::string& name) = 0;
    
    // Experiment type
    virtual std::string getType() const = 0;
    
    // Experiment configuration
    virtual void configure(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual std::unordered_map<std::string, std::any> getConfiguration() const = 0;
    
    // Experiment execution
    virtual bool run(std::shared_ptr<class Brain> brain,
                     std::shared_ptr<class Environment> environment,
                     SimulationStep maxSteps) = 0;
    
    // Experiment results
    virtual std::any getResults() const = 0;
    
    // Experiment hooks
    virtual void onExperimentStart() {}
    virtual void onExperimentEnd() {}
    virtual void onExperimentStep(SimulationStep step) {}
    
    // Experiment validation
    virtual bool validate() const = 0;
    virtual std::vector<std::string> getValidationErrors() const = 0;
    
    // Experiment metadata
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::vector<std::string> getTags() const = 0;
    
    // Experiment lifecycle
    virtual void initialize() {}
    virtual void cleanup() {}
    
    // Experiment configuration schema
    virtual std::unordered_map<std::string, std::any> getDefaultConfiguration() const = 0;
    
    // Experiment type-specific initialization
    virtual void initializeExperimentSpecific() {}
    
    // Experiment statistics
    virtual std::unordered_map<std::string, double> getStatistics() const = 0;
};

// Experiment factory for creating plugins
class ExperimentFactory {
public:
    using ExperimentCreator = std::function<std::shared_ptr<ExperimentBase>()>;
    
    // Register experiment plugin
    static void registerExperiment(const std::string& typeName, ExperimentCreator creator);
    
    // Create experiment by type
    static std::shared_ptr<ExperimentBase> createExperiment(const std::string& typeName);
    
    // Get all registered experiment types
    static std::vector<std::string> getRegisteredExperimentTypes();
    
    // Unregister experiment plugin
    static void unregisterExperiment(const std::string& typeName);
    
    // Get experiment metadata
    static std::unordered_map<std::string, std::string> getExperimentMetadata(const std::string& typeName);
    
    // Check if experiment type is registered
    static bool isExperimentRegistered(const std::string& typeName);
};

// ============================================================================
// Enhanced Experiment with Configuration and Validation
// ============================================================================

class Experiment {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Plugin system
    std::shared_ptr<ExperimentBase> plugin_;
    
    // Configuration management
    std::unordered_map<std::string, std::any> configuration_;
    mutable std::mutex configurationMutex_;
    
    // Validation and error handling
    std::vector<std::string> validationErrors_;
    mutable std::mutex validationMutex_;
    
    // Performance monitoring
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point endTime_;
    mutable std::mutex performanceMutex_;
    
public:
    // ===== PLUGIN SYSTEM =====
    
    // Create enhanced experiment with plugin
    explicit Experiment(const std::string& name);
    explicit Experiment(std::shared_ptr<ExperimentBase> plugin);
    ~Experiment();
    
    // Set experiment plugin
    void setPlugin(std::shared_ptr<ExperimentBase> plugin);
    
    // Get experiment plugin
    std::shared_ptr<ExperimentBase> getPlugin() const;
    
    // Check if experiment has plugin
    bool hasPlugin() const;
    
    // ===== CONFIGURATION SYSTEM =====
    
    // Configure experiment with plugin
    void configureExperiment(const std::unordered_map<std::string, std::any>& config);
    
    // Get experiment configuration
    std::unordered_map<std::string, std::any> getExperimentConfiguration() const;
    
    // Set specific configuration value
    template<typename T>
    void setConfigValue(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(configurationMutex_);
        configuration_[key] = value;
    }
    
    // Get configuration value with type safety
    template<typename T>
    std::optional<T> getConfigValue(const std::string& key) const {
        std::lock_guard<std::mutex> lock(configurationMutex_);
        
        auto it = configuration_.find(key);
        if (it == configuration_.end()) {
            return std::nullopt;
        }
        
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return std::nullopt;
        }
    }
    
    // ===== PLUGIN HOOKS AND CALLBACKS =====
    
    // Register experiment lifecycle hooks
    using LifecycleHook = std::function<void()>;
    
    void registerStartHook(LifecycleHook hook);
    void registerEndHook(LifecycleHook hook);
    void registerStepHook(std::function<void(SimulationStep)> hook);
    void registerValidationHook(std::function<bool()> hook);
    
    // Execute experiment lifecycle hooks
    void executeStartHook();
    void executeEndHook();
    void executeStepHook(SimulationStep step);
    bool executeValidationHook();
    
    // ===== ITERATOR-BASED APIs =====
    
    // Experiment configuration iterator
    class ConfigurationIterator : public std::iterator<std::forward_iterator_tag, 
                                                       std::pair<std::string, std::any>> {
    private:
        using MapType = std::unordered_map<std::string, std::any>;
        MapType* map_;
        MapType::iterator current_;
        
    public:
        ConfigurationIterator(MapType* map, MapType::iterator current)
            : map_(map), current_(current) {}
        
        reference operator*() const { return *current_; }
        pointer operator->() const { return &(operator*()); }
        
        ConfigurationIterator& operator++() {
            ++current_;
            return *this;
        }
        
        ConfigurationIterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const ConfigurationIterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const ConfigurationIterator& other) const {
            return !(*this == other);
        }
    };
    
    // Get begin and end iterators for configuration
    ConfigurationIterator beginConfiguration() {
        std::lock_guard<std::mutex> lock(configurationMutex_);
        return ConfigurationIterator(&configuration_, configuration_.begin());
    }
    
    ConfigurationIterator endConfiguration() {
        std::lock_guard<std::mutex> lock(configurationMutex_);
        return ConfigurationIterator(&configuration_, configuration_.end());
    }
    
    // Range-based for loop support
    class ConfigurationView {
    private:
        std::reference_wrapper<std::unordered_map<std::string, std::any>> config_;
        
    public:
        explicit ConfigurationView(std::unordered_map<std::string, std::any>& config)
            : config_(config) {}
        
        ConfigurationIterator begin() {
            return ConfigurationIterator(&config_.get(), config_.get().begin());
        }
        
        ConfigurationIterator end() {
            return ConfigurationIterator(&config_.get(), config_.get().end());
        }
    };
    
    ConfigurationView getConfigurationView() {
        std::lock_guard<std::mutex> lock(configurationMutex_);
        return ConfigurationView(configuration_);
    }
    
    // ===== VALIDATION SYSTEM =====
    
    // Validate experiment configuration
    bool validateConfiguration() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Clear validation errors
    void clearValidationErrors();
    
    // ===== PERFORMANCE MONITORING =====
    
    // Get experiment execution time
    std::chrono::duration<double> getExecutionTime() const;
    
    // Get performance statistics
    std::unordered_map<std::string, double> getPerformanceStats() const;
    
    // ===== PLUGIN METADATA =====
    
    // Get experiment type
    std::string getType() const;
    
    // Get experiment description
    std::string getDescription() const;
    
    // Get experiment version
    std::string getVersion() const;
    
    // Get experiment author
    std::string getAuthor() const;
    
    // Get experiment tags
    std::vector<std::string> getTags() const;
    
    // ===== LEGACY COMPATIBILITY =====
    
    // Legacy methods for backward compatibility
    const std::string& getName() const;
    void setName(const std::string& name);
    
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    double getMetric(const std::string& name) const;
    bool hasMetric(const std::string& name) const;
    
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    const std::vector<StepData>& getStepData() const;
    
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
    // Plugin execution methods
    bool runExperiment(std::shared_ptr<class Brain> brain,
                      std::shared_ptr<class Environment> environment,
                      SimulationStep maxSteps);
    
    std::any getResults() const;
    
    // ===== EXPERIMENT LIFECYCLE =====
    
    void initializeExperiment();
    void cleanupExperiment();
    
    bool isExperimentValid() const;
};

// ============================================================================
// Enhanced ExperimentRunner with Plugin Management
// ============================================================================

class ExperimentRunner {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Plugin management
    std::vector<std::shared_ptr<ExperimentBase>> registeredPlugins_;
    mutable std::mutex pluginsMutex_;
    
    // Configuration templates
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> configTemplates_;
    mutable std::mutex templatesMutex_;
    
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // ===== PLUGIN MANAGEMENT =====
    
    // Register experiment plugin
    void registerPlugin(std::shared_ptr<ExperimentBase> plugin);
    
    // Unregister experiment plugin
    void unregisterPlugin(const std::string& name);
    
    // Get registered plugins
    std::vector<std::shared_ptr<ExperimentBase>> getRegisteredPlugins() const;
    
    // Find plugin by type
    std::shared_ptr<ExperimentBase> findPlugin(const std::string& typeName) const;
    
    // ===== CONFIGURATION TEMPLATES =====
    
    // Add configuration template
    void addConfigTemplate(const std::string& name,
                          const std::unordered_map<std::string, std::any>& templateConfig);
    
    // Get configuration template
    std::unordered_map<std::string, std::any> getConfigTemplate(const std::string& name) const;
    
    // List all configuration templates
    std::vector<std::string> listConfigTemplates() const;
    
    // ===== EXPERIMENT CREATION WITH PLUGINS =====
    
    // Create experiment with plugin
    std::shared_ptr<Experiment> createExperimentWithPlugin(const std::string& name,
                                                           const std::string& pluginType);
    
    // Create experiment with configuration
    std::shared_ptr<Experiment> createExperimentWithConfig(const std::string& name,
                                                           const std::string& pluginType,
                                                           const std::unordered_map<std::string, std::any>& config);
    
    // ===== ENHANCED EXPERIMENT EXECUTION =====
    
    // Run single experiment with enhanced error handling
    bool runExperimentEnhanced(std::shared_ptr<Experiment> experiment,
                               std::shared_ptr<class Brain> brain,
                               std::shared_ptr<class Environment> environment,
                               SimulationStep maxSteps);
    
    // Run batch of experiments with progress tracking
    std::vector<std::shared_ptr<Experiment>> runBatchEnhanced(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<class Brain> brainTemplate,
        std::shared_ptr<class Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // ===== VALIDATION AND ERROR HANDLING =====
    
    // Validate experiment before running
    bool validateExperiment(std::shared_ptr<Experiment> experiment);
    
    // Get all experiment validation errors
    std::vector<std::pair<std::string, std::vector<std::string>>> getAllExperimentValidationErrors(
        const std::vector<std::shared_ptr<Experiment>>& experiments);
    
    // ===== RESULTS MANAGEMENT =====
    
    // Save experiment results with enhanced formatting
    bool saveResultsEnhanced(const std::string& filepath,
                            const std::vector<std::shared_ptr<Experiment>>& experiments);
    
    // Load experiment results
    std::vector<std::shared_ptr<Experiment>> loadResults(const std::string& filepath);
    
    // ===== EXPERIMENT LIFECYCLE MANAGEMENT =====
    
    // Initialize all registered plugins
    void initializeAllPlugins();
    
    // Cleanup all registered plugins
    void cleanupAllPlugins();
    
    // ===== UTILITY FUNCTIONS =====
    
    // Get experiment statistics
    std::unordered_map<std::string, double> getExperimentStatistics() const;
    
    // Get experiment summary
    std::string getExperimentSummary() const;
    
    // Check if experiment runner is healthy
    bool isHealthy() const;
    
    // ===== LEGACY COMPATIBILITY =====
    
    // Legacy methods for backward compatibility
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<class Brain> brain,
                      std::shared_ptr<class Environment> environment,
                      SimulationStep maxSteps);
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<class Brain> brainTemplate,
        std::shared_ptr<class Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    void clearExperiments();
    void setGlobalSeed(uint64_t seed);
    bool saveResults(const std::string& filepath) const;
};

// ============================================================================
// Experiment Plugin Registry
// ============================================================================

// Global experiment plugin registry
class ExperimentPluginRegistry {
private:
    static std::unordered_map<std::string, std::function<std::shared_ptr<ExperimentBase>()>> plugins_;
    static std::mutex registryMutex_;
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> pluginMetadata_;
    
public:
    // Register experiment plugin with metadata
    static void registerPlugin(const std::string& typeName,
                               std::function<std::shared_ptr<ExperimentBase>()> creator,
                               const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Unregister experiment plugin
    static void unregisterPlugin(const std::string& typeName);
    
    // Get all registered plugin types
    static std::vector<std::string> getRegisteredPluginTypes();
    
    // Get plugin metadata
    static std::unordered_map<std::string, std::string> getPluginMetadata(const std::string& typeName);
    
    // Check if plugin is registered
    static bool isPluginRegistered(const std::string& typeName);
    
    // Create plugin instance
    static std::shared_ptr<ExperimentBase> createPlugin(const std::string& typeName);
    
    // Get available experiment types for UI
    static std::vector<std::pair<std::string, std::string>> getAvailableExperimentsForUI();
    
    // Initialize all plugins
    static void initializeAllPlugins();
    
    // Cleanup all plugins
    static void cleanupAllPlugins();
    
    // Validate plugin
    static bool validatePlugin(const std::string& typeName);
    
    // Get plugin validation errors
    static std::vector<std::string> getPluginValidationErrors(const std::string& typeName);
};

// ============================================================================
// Experiment Configuration Builder
// ============================================================================

// Configuration builder for creating complex experiment configurations
class ExperimentConfigBuilder {
private:
    std::unordered_map<std::string, std::any> config_;
    
public:
    ExperimentConfigBuilder() = default;
    
    // Set configuration values
    template<typename T>
    ExperimentConfigBuilder& set(const std::string& key, const T& value);
    
    // Set nested configuration
    ExperimentConfigBuilder& setNested(const std::string& parentKey,
                                    const std::string& childKey,
                                    const std::any& value);
    
    // Enable/disable experiment features
    ExperimentConfigBuilder& enableFeature(const std::string& featureName, bool enabled = true);
    
    // Set experiment parameters
    ExperimentConfigBuilder& setParameter(const std::string& paramName,
                                         const std::any& value);
    
    // Set experiment constraints
    ExperimentConfigBuilder& setConstraint(const std::string& constraintName,
                                         const std::any& value);
    
    // Set experiment metadata
    ExperimentConfigBuilder& setMetadata(const std::string& key,
                                          const std::string& value);
    
    // Add experiment tag
    ExperimentConfigBuilder& addTag(const std::string& tag);
    
    // Set experiment difficulty
    ExperimentConfigBuilder& setDifficulty(const std::string& difficulty);
    
    // Set experiment duration
    ExperimentConfigBuilder& setDuration(SimulationStep duration);
    
    // Set experiment budget
    ExperimentConfigBuilder& setBudget(const std::any& budget);
    
    // Build final configuration
    std::unordered_map<std::string, std::any> build() const;
    
    // Reset builder
    void reset();
    
    // Check if builder has values
    bool hasValues() const;
};

// ============================================================================
// Experiment Utility Functions
// ============================================================================

// Utility functions for experiment management
namespace ExperimentUtils {
    // Validate experiment name
    bool validateExperimentName(const std::string& name);
    
    // Generate unique experiment name
    std::string generateUniqueExperimentName(const std::string& baseName);
    
    // Format experiment configuration for display
    std::string formatConfiguration(const std::unordered_map<std::string, std::any>& config);
    
    // Calculate experiment checksum
    std::string calculateExperimentChecksum(const std::unordered_map<std::string, std::any>& config);
    
    // Merge configurations
    std::unordered_map<std::string, std::any> mergeConfigurations(
        const std::vector<std::unordered_map<std::string, std::any>>& configs);
    
    // Get experiment type from name
    std::string getExperimentTypeFromName(const std::string& name);
    
    // Create experiment configuration from template
    std::unordered_map<std::string, std::any> createExperimentConfiguration(
        const std::string& templateName,
        const std::unordered_map<std::string, std::any>& overrides);
    
    // Validate experiment configuration
    bool validateExperimentConfiguration(
        const std::unordered_map<std::string, std::any>& config,
        const std::string& experimentType);
    
    // Get default configuration for experiment type
    std::unordered_map<std::string, std::any> getDefaultConfiguration(
        const std::string& experimentType);
    
    // Convert configuration value to string
    std::string configValueToString(const std::any& value);
    
    // Convert string to configuration value
    std::any stringToConfigValue(const std::string& str, const std::string& typeName);
    
    // Get configuration schema
    std::unordered_map<std::string, std::any> getConfigurationSchema(
        const std::string& experimentType);
    
    // Validate configuration against schema
    bool validateConfigurationAgainstSchema(
        const std::unordered_map<std::string, std::any>& config,
        const std::unordered_map<std::string, std::any>& schema);
}

// ============================================================================
// Experiment Macros and Convenience Functions
// ============================================================================

// Convenience macros for experiment creation
#define EXPERIMENT_FACTORY(reg_name, exp_class) \
    nlm::ExperimentPluginRegistry::registerPlugin( \
        #exp_class, []() { return std::make_shared<exp_class>(); }, \
        {{"name", #exp_class, {"description", "Created by " #reg_name}} } \
    )

// Experiment validation macro
#define VALIDATE_EXPERIMENT(exp, ...) \
    if (!(exp->validateConfiguration())) { \
        auto errors = (exp->getValidationErrors()); \
        for (const auto& error : errors) { \
            std::cerr << "Validation error: " << error << std::endl; \
        } \
        return false; \
    }

// Experiment execution macro
#define RUN_EXPERIMENT(exp, brain, env, steps) \
    if (!(exp->runExperiment(brain, env, steps))) { \
        std::cerr << "Experiment failed: " << (exp->getName()) << std::endl; \
        return false; \
    }

// ============================================================================

} // namespace nlm

#endif // ENHANCED_EXPERIMENT_RUNNER_HPP