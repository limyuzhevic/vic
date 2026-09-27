// ==============================================================================
// Plugin Debugger
// ==============================================================================
#ifndef PLUGINDEBUGGER_HPP
#define PLUGINDEBUGGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../include/nlm/plugin/PluginManager.hpp"
#include "../include/nlm/plugin/PluginLoader.hpp"
#include "../include/nlm/plugin/PluginRegistry.hpp"

namespace nlm {
namespace debug {

/**
 * @class PluginDebugger
 * @brief Plugin debugging tool for NLM
 * 
 * Provides tools for plugin debugging including plugin validation, loading analysis,
 * dependency checking, and performance monitoring.
 */
class PluginDebugger {
public:
    /**
     * @enum PluginValidationResult
     * @brief Plugin validation result
     */
    enum class PluginValidationResult {
        SUCCESS,
        MISSING_DEPENDENCIES,
        INVALID_INTERFACE,
        LOAD_FAILURE,
        INITIALIZATION_FAILURE,
        SECURITY_VIOLATION,
        PERFORMANCE_ISSUES,
        COMPATIBILITY_ISSUES,
        METADATA_ERROR
    };

    /**
     * @struct PluginDiagnostics
     * @brief Plugin diagnostics
     */
    struct PluginDiagnostics {
        std::string pluginName;
        std::string version;
        PluginValidationResult validationResult;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::steady_clock::time_point loadTime;
        std::chrono::steady_clock::time_point lastUsed;
        double memoryUsage;
        double cpuUsage;
        uint64_t callCount;
        double averageExecutionTime;
        std::map<std::string, std::string> metadata;
    };

    /**
     * @struct PluginDependency
     * @brief Plugin dependency
     */
    struct PluginDependency {
        std::string name;
        std::string version;
        std::string type;
        bool required;
        bool satisfied;
        std::vector<std::string> alternatives;
    };

    PluginDebugger();
    ~PluginDebugger();

    /**
     * @brief Start plugin debugging
     * @param pluginManager Plugin manager reference
     */
    void startDebugging(PluginManager& pluginManager);

    /**
     * @brief Stop plugin debugging
     */
    void stopDebugging();

    /**
     * @brief Validate plugin
     * @param pluginName Plugin name
     * @return Plugin diagnostics
     */
    PluginDiagnostics validatePlugin(const std::string& pluginName);

    /**
     * @brief Validate all plugins
     * @return Map of plugin names to diagnostics
     */
    std::map<std::string, PluginDiagnostics> validateAllPlugins();

    /**
     * @brief Load plugin
     * @param pluginName Plugin name
     * @param pluginPath Plugin path
     * @return Plugin diagnostics
     */
    PluginDiagnostics loadPlugin(const std::string& pluginName, const std::string& pluginPath);

    /**
     * @brief Unload plugin
     * @param pluginName Plugin name
     * @return true if plugin was unloaded
     */
    bool unloadPlugin(const std::string& pluginName);

    /**
     * @brief Get plugin diagnostics
     * @param pluginName Plugin name
     * @return Plugin diagnostics
     */
    PluginDiagnostics getPluginDiagnostics(const std::string& pluginName) const;

    /**
     * @brief Get all plugin diagnostics
     * @return Map of plugin names to diagnostics
     */
    std::map<std::string, PluginDiagnostics> getAllPluginDiagnostics() const;

    /**
     * @brief Get plugin dependencies
     * @param pluginName Plugin name
     * @return Plugin dependencies
     */
    std::vector<PluginDependency> getPluginDependencies(const std::string& pluginName) const;

    /**
     * @brief Check plugin compatibility
     * @param pluginName Plugin name
     * @return true if plugin is compatible
     */
    bool checkPluginCompatibility(const std::string& pluginName) const;

    /**
     * @brief Get available plugins
     * @return Vector of plugin names
     */
    std::vector<std::string> getAvailablePlugins() const;

    /**
     * @brief Get plugin load status
     * @param pluginName Plugin name
     * @return true if plugin is loaded
     */
    bool isPluginLoaded(const std::string& pluginName) const;

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
     * @brief Get plugin metadata
     * @param pluginName Plugin name
     * @return Plugin metadata
     */
    std::map<std::string, std::string> getPluginMetadata(const std::string& pluginName) const;

    /**
     * @brief Generate plugin validation report
     * @return Validation report
     */
    struct ValidationReport {
        size_t totalPlugins;
        size_t validPlugins;
        size_t invalidPlugins;
        std::map<std::string, PluginDiagnostics> diagnostics;
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
     * @return Vector of plugin types
     */
    std::vector<std::string> getPluginTypes() const;

protected:
    /**
     * @brief Update plugin diagnostics
     */
    void updatePluginDiagnostics();

    /**
     * @brief Validate plugin compatibility
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginCompatibility(const std::string& pluginName, PluginDiagnostics& diagnostics);

    /**
     * @brief Validate plugin interface
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginInterface(const std::string& pluginName, PluginDiagnostics& diagnostics);

    /**
     * @brief Validate plugin loading
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginLoading(const std::string& pluginName, PluginDiagnostics& diagnostics);

    /**
     * @brief Validate plugin initialization
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginInitialization(const std::string& pluginName, PluginDiagnostics& diagnostics);

    /**
     * @brief Validate plugin security
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginSecurity(const std::string& pluginName, PluginDiagnostics& diagnostics);

    /**
     * @brief Validate plugin performance
     * @param pluginName Plugin name
     * @param diagnostics Plugin diagnostics reference
     */
    void validatePluginPerformance(const std::string& pluginName, PluginDiagnostics& diagnostics);

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
     * @param diagnostics Plugin diagnostics
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(const PluginDiagnostics& diagnostics) const;

    /**
     * @brief Generate warning report
     * @param diagnostics Plugin diagnostics
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(const PluginDiagnostics& diagnostics) const;

    /**
     * @brief Generate diagnostic report
     * @param diagnostics Plugin diagnostics
     * @return Diagnostic report
     */
    std::vector<std::string> generateDiagnosticReport(const PluginDiagnostics& diagnostics) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // PLUGINDEBUGGER_HPP
