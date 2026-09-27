// ==============================================================================
// Debug Plugin Interface
// ==============================================================================
#ifndef DEBUGPLUGININTERFACE_HPP
#define DEBUGPLUGININTERFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../include/nlm/plugin/PluginManager.hpp"
#include "../src/brain/Brain.hpp"

namespace nlm {
namespace debug {

/**
 * @class DebugPluginInterface
 * @brief Plugin debugging and validation interface for NLM
 * 
 * Provides tools for plugin debugging and validation including plugin loading analysis,
 * dependency checking, plugin performance monitoring, and plugin compatibility testing.
 */
class DebugPluginInterface {
public:
    /**
     * @enum PluginValidationType
     * @brief Type of plugin validation
     */
    enum class PluginValidationType {
        COMPATIBILITY,
        PERFORMANCE,
        SECURITY,
        INTERFACE,
        DEPENDENCY,
        INITIALIZATION,
        METADATA,
        LIFECYCLE,
        RESOURCE_USAGE
    };

    /**
     * @struct PluginDebugInfo
     * @brief Plugin debug information
     */
    struct PluginDebugInfo {
        std::string pluginName;
        std::string version;
        std::string description;
        std::vector<std::string> authors;
        std::string license;
        std::vector<PluginValidationType> validationTypes;
        std::map<std::string, std::string> metadata;
    };

    /**
     * @struct PluginValidationResult
     * @brief Plugin validation result
     */
    struct PluginValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<PluginValidationType, std::string> validationResults;
        std::chrono::steady_clock::time_point validationTime;
    };

    DebugPluginInterface();
    ~DebugPluginInterface();

    /**
     * @brief Start plugin debugging interface
     * @param pluginManager Plugin manager reference
     */
    void startPluginInterface(PluginManager& pluginManager);

    /**
     * @brief Stop plugin debugging interface
     */
    void stopPluginInterface();

    /**
     * @brief Get available plugins
     * @return Vector of plugin debug information
     */
    std::vector<PluginDebugInfo> getAvailablePlugins() const;

    /**
     * @brief Get plugin by name
     * @param pluginName Plugin name
     * @return Plugin debug information
     */
    PluginDebugInfo getPlugin(const std::string& pluginName) const;

    /**
     * @brief Validate plugin
     * @param pluginName Plugin name
     * @param validationType Validation type
     * @return Plugin validation result
     */
    PluginValidationResult validatePlugin(const std::string& pluginName, PluginValidationType validationType);

    /**
     * @brief Validate all plugins
     * @param validationType Validation type
     * @return Map of plugin names to validation results
     */
    std::map<std::string, PluginValidationResult> validateAllPlugins(PluginValidationType validationType);

    /**
     * @brief Get plugin dependencies
     * @param pluginName Plugin name
     * @return Plugin dependencies
     */
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;

    /**
     * @brief Get plugin dependents
     * @param pluginName Plugin name
     * @return Plugin dependents
     */
    std::vector<std::string> getPluginDependents(const std::string& pluginName) const;

    /**
     * @brief Check plugin compatibility
     * @param pluginName Plugin name
     * @param brain Neural network reference
     * @return true if plugin is compatible
     */
    bool checkPluginCompatibility(const std::string& pluginName, const Brain& brain) const;

    /**
     * @brief Get plugin performance metrics
     * @param pluginName Plugin name
     * @return Performance metrics
     */
    struct PerformanceMetrics {
        double memoryUsage;
        double cpuUsage;
        uint64_t callCount;
        double averageExecutionTime;
        double executionTimeStandardDeviation;
        std::chrono::steady_clock::time_point lastUsed;
    };

    PerformanceMetrics getPluginPerformanceMetrics(const std::string& pluginName) const;

    /**
     * @brief Get plugin security diagnostics
     * @param pluginName Plugin name
     * @return Security diagnostics
     */
    struct SecurityDiagnostics {
        bool isSandboxed;
        std::vector<std::string> permissions;
        std::vector<std::string> securityViolations;
        bool hasAccessToSystemResources;
    };

    SecurityDiagnostics getPluginSecurityDiagnostics(const std::string& pluginName) const;

    /**
     * @brief Get plugin interface diagnostics
     * @param pluginName Plugin name
     * @return Interface diagnostics
     */
    struct InterfaceDiagnostics {
        std::vector<std::string> implementedInterfaces;
        std::vector<std::string> missingInterfaces;
        std::map<std::string, std::string> interfaceSignatures;
    };

    InterfaceDiagnostics getPluginInterfaceDiagnostics(const std::string& pluginName) const;

    /**
     * @brief Get plugin lifecycle diagnostics
     * @param pluginName Plugin name
     * @return Lifecycle diagnostics
     */
    struct LifecycleDiagnostics {
        std::chrono::steady_clock::time_point loadTime;
        std::chrono::steady_clock::time_point unloadTime;
        uint64_t initializationCount;
        uint64_t shutdownCount;
        std::vector<std::string> lifecycleEvents;
    };

    LifecycleDiagnostics getPluginLifecycleDiagnostics(const std::string& pluginName) const;

    /**
     * @brief Get plugin resource diagnostics
     * @param pluginName Plugin name
     * @return Resource diagnostics
     */
    struct ResourceDiagnostics {
        uint64_t memoryAllocationCount;
        uint64_t memoryDeallocationCount;
        uint64_t threadCreationCount;
        uint64_t fileAccessCount;
        std::map<std::string, uint64_t> resourceUsage;
    };

    ResourceDiagnostics getPluginResourceDiagnostics(const std::string& pluginName) const;

    /**
     * @brief Get plugin metadata
     * @param pluginName Plugin name
     * @return Plugin metadata
     */
    std::map<std::string, std::string> getPluginMetadata(const std::string& pluginName) const;

    /**
     * @brief Set plugin debugging options
     * @param options Debugging options
     */
    void setPluginDebuggingOptions(const std::map<std::string, std::string>& options);

    /**
     * @brief Get plugin debugging options
     * @return Debugging options
     */
    std::map<std::string, std::string> getPluginDebuggingOptions() const;

    /**
     * @brief Generate plugin validation report
     * @return Validation report
     */
    struct ValidationReport {
        size_t totalPlugins;
        size_t validPlugins;
        size_t invalidPlugins;
        std::map<std::string, PluginValidationResult> validationResults;
        std::vector<std::string> globalErrors;
        std::chrono::steady_clock::time_point reportTime;
    };

    ValidationReport generateValidationReport();

    /**
     * @brief Generate plugin performance report
     * @return Performance report
     */
    struct PerformanceReport {
        size_t totalPlugins;
        double totalMemoryUsage;
        double totalCPUUsage;
        uint64_t totalCallCount;
        double averageExecutionTime;
        std::map<std::string, PerformanceMetrics> metrics;
        std::chrono::steady_clock::time_point reportTime;
    };

    PerformanceReport generatePerformanceReport();

    /**
     * @brief Generate plugin security report
     * @return Security report
     */
    struct SecurityReport {
        size_t totalPlugins;
        size_t securePlugins;
        size_t insecurePlugins;
        std::map<std::string, SecurityDiagnostics> securityDiagnostics;
        std::vector<std::string> securityViolations;
        std::chrono::steady_clock::time_point reportTime;
    };

    SecurityReport generateSecurityReport();

    /**
     * @brief Export plugin diagnostics
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportPluginDiagnostics(const std::string& filename);

    /**
     * @brief Import plugin diagnostics
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importPluginDiagnostics(const std::string& filename);

    /**
     * @brief Clear plugin diagnostics
     */
    void clearPluginDiagnostics();

    /**
     * @brief Generate plugin debugging visualization
     * @param filename Output file path
     */
    void generatePluginDebuggingVisualization(const std::string& filename);

    /**
     * @brief Generate plugin validation visualization
     * @param filename Output file path
     */
    void generatePluginValidationVisualization(const std::string& filename);

    /**
     * @brief Generate plugin performance visualization
     * @param filename Output file path
     */
    void generatePluginPerformanceVisualization(const std::string& filename);

    /**
     * @brief Get plugin categories
     * @return Vector of plugin categories
     */
    std::vector<std::string> getPluginCategories() const;

    /**
     * @brief Get plugin types
     *return Vector of plugin types
     */
    std::vector<std::string> getPluginTypes() const;

    /**
     * @brief Get validation types
     * @return Vector of validation types
     */
    std::vector<PluginValidationType> getAvailableValidationTypes() const;

protected:
    /**
     * @brief Update plugin diagnostics
     */
    void updatePluginDiagnostics();

    /**
     * @brief Validate plugin compatibility
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginCompatibility(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin performance
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginPerformance(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin security
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginSecurity(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin interface
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginInterface(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin dependency
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginDependency(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin initialization
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginInitialization(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin lifecycle
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginLifecycle(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Validate plugin resource usage
     * @param pluginName Plugin name
     * @param result Validation result reference
     */
    void validatePluginResourceUsage(const std::string& pluginName, PluginValidationResult& result);

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create validation visualization
     * @param filename Output file path
     */
    void createValidationVisualization(const std::string& filename);

    /**
     * @brief Create performance visualization
     * @param filename Output file path
     */
    void createPerformanceVisualization(const std::string& filename);

    /**
     * @brief Generate error report
     * @param result Validation result
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(const PluginValidationResult& result) const;

    /**
     * @brief Generate warning report
     * @param result Validation result
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(const PluginValidationResult& result) const;

    /**
     * @brief Generate diagnostic report
     * @param result Validation result
     * @return Diagnostic report
     */
    std::vector<std::string> generateDiagnosticReport(const PluginValidationResult& result) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // DEBUGPLUGININTERFACE_HPP
