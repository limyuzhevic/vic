/// Advanced Plugin System for NLM
/// 
/// This module provides advanced features for experienced users:
/// - Plugin system for extending functionality
/// - Scripted experiment runner
/// - Advanced debugging tools
/// - Performance profiling
/// - State analysis and visualization

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>

namespace nlm {

/// Plugin interface for extending NLM functionality
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    /// Get plugin name
    virtual std::string getName() const = 0;
    
    /// Get plugin version
    virtual std::string getVersion() const = 0;
    
    /// Get plugin description
    virtual std::string getDescription() const = 0;
    
    /// Initialize plugin with brain reference
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    
    /// Clean up plugin resources
    virtual void cleanup() = 0;
    
    /// Execute plugin functionality
    virtual bool execute(const std::string& command, const std::vector<std::string>& args) = 0;
    
    /// Get plugin commands
    virtual std::vector<std::string> getCommands() const = 0;
};

/// Scripted experiment runner
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    /// Load experiment script from file
    bool loadScript(const std::string& filepath);
    
    /// Execute experiment script
    bool executeScript(std::shared_ptr<Brain> brain, const std::vector<std::string>& variables);
    
    /// Get available experiments
    std::vector<std::string> getAvailableExperiments() const;
    
    /// Get experiment parameters
    std::vector<std::string> getExperimentParams(const std::string& experimentName) const;
    
    /// Get experiment description
    std::string getExperimentDescription(const std::string& experimentName) const;
    
    /// Execute predefined experiment
    bool runExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName, 
                      const std::map<std::string, std::string>& params);
    
    /// Save experiment results
    bool saveResults(const std::string& filepath, const std::string& experimentName) const;
    
    /// Load experiment results
    bool loadResults(const std::string& filepath, std::string& experimentName,
                    std::map<std::string, std::string>& results);
};

/// Advanced debugging tools
class DebugTools {
public:
    DebugTools();
    ~DebugTools();
    
    /// Start performance profiling
    void startProfiling(const std::string& sessionName);
    
    /// Stop performance profiling
    bool stopProfiling(std::string& outputFile);
    
    /// Get profiling statistics
    std::map<std::string, double> getProfilingStats(const std::string& sessionName) const;
    
    /// Analyze memory usage
    bool analyzeMemoryUsage(std::string& report);
    
    /// Analyze spike activity patterns
    bool analyzeSpikePatterns(std::shared_ptr<Brain> brain, std::string& report);
    
    /// Detect performance bottlenecks
    bool detectBottlenecks(std::string& report);
    
    /// Enable debug mode
    void enableDebugMode(bool enable);
    
    /// Is debug mode enabled?
    bool isDebugModeEnabled() const;
    
    /// Set debug log level
    void setDebugLevel(LogLevel level);
    
    /// Get debug log level
    LogLevel getDebugLevel() const;
    
    /// Export debug information
    bool exportDebugInfo(const std::string& filepath);
};

/// State analysis and visualization
class StateAnalyzer {
public:
    StateAnalyzer();
    ~StateAnalyzer();
    
    /// Analyze brain state
    bool analyzeState(std::shared_ptr<Brain> brain, std::string& analysis);
    
    /// Generate state statistics
    bool generateStateStats(std::shared_ptr<Brain> brain, std::string& stats);
    
    /// Compare two brain states
    bool compareStates(const std::string& state1File, const std::string& state2File, 
                      std::string& comparison);
    
    /// Calculate brain state similarity
    double calculateStateSimilarity(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2);
    
    /// Generate visualization data
    bool generateVisualizationData(std::shared_ptr<Brain> brain, std::string& data);
    
    /// Export neural connectivity graph
    bool exportConnectivityGraph(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /// Export activity patterns
    bool exportActivityPatterns(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /// Export memory statistics
    bool exportMemoryStats(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /// Calculate developmental metrics
    bool calculateDevelopmentalMetrics(std::shared_ptr<Brain> brain, std::string& metrics);
};

/// Plugin manager for handling multiple plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    /// Load plugin from file
    bool loadPlugin(const std::string& pluginPath);
    
    /// Unload plugin
    bool unloadPlugin(const std::string& pluginName);
    
    /// Get loaded plugins
    std::vector<std::string> getLoadedPlugins() const;
    
    /// Execute command through all plugins
    bool executePluginCommand(const std::string& command, const std::vector<std::string>& args);
    
    /// Get plugin information
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        bool enabled;
        std::map<std::string, std::string> metadata;
    };
    
    std::vector<PluginInfo> getPluginInfo() const;
    
    /// Enable/disable plugin
    bool setPluginEnabled(const std::string& pluginName, bool enabled);
    
    /// Is plugin enabled?
    bool isPluginEnabled(const std::string& pluginName) const;
    
    /// Save plugin configuration
    bool savePluginConfig(const std::string& filepath);
    
    /// Load plugin configuration
    bool loadPluginConfig(const std::string& filepath);
};

/// Script interpreter for NLM scripting language
class ScriptInterpreter {
public:
    ScriptInterpreter();
    ~ScriptInterpreter();
    
    /// Execute script from string
    bool executeScript(const std::string& script, std::shared_ptr<Brain> brain,
                      std::map<std::string, std::string>& variables);
    
    /// Execute script from file
    bool executeScriptFile(const std::string& filepath, std::shared_ptr<Brain> brain,
                          std::map<std::string, std::string>& variables);
    
    /// Compile script to bytecode
    bool compileScript(const std::string& script, std::vector<uint8_t>& bytecode);
    
    /// Execute bytecode
    bool executeBytecode(const std::vector<uint8_t>& bytecode, std::shared_ptr<Brain> brain,
                        std::map<std::string, std::string>& variables);
    
    /// Get supported script commands
    std::vector<std::string> getScriptCommands() const;
    
    /// Get script function signatures
    std::map<std::string, std::string> getScriptFunctions() const;
    
    /// Get script variable types
    std::map<std::string, std::string> getScriptVariables() const;
    
    /// Set script timeout
    void setScriptTimeout(double timeoutSeconds);
    
    /// Get script timeout
    double getScriptTimeout() const;
    
    /// Set script memory limit
    void setScriptMemoryLimit(size_t memoryBytes);
    
    /// Get script memory limit
    size_t getScriptMemoryLimit() const;
};

} // namespace nlm
