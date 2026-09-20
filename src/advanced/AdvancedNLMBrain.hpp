// Main integration header for advanced NLM features
#pragma once

#include "AdvancedFeatures.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Main advanced NLM integration class that combines all advanced features
class AdvancedNLMBrain {
public:
    AdvancedNLMBrain();
    ~AdvancedNLMBrain();
    
    // Initialize with brain reference and config
    bool initialize(Brain* brain, const std::shared_ptr<Config>& config);
    
    // Run advanced simulation with all features
    bool runAdvancedSimulation(int argc, char** argv);
    
    // Enable advanced features
    void enablePerformanceMonitoring(bool enable = true);
    void enableOutputManagement(bool enable = true);
    void enableDebugging(bool enable = true);
    void enableToolIntegration(bool enable = true);
    void enableExperimentalControls(bool enable = true);
    
    // Get feature instances
    PerformanceMonitor* getPerformanceMonitor() { return performanceMonitor_.get(); }
    OutputManager* getOutputManager() { return outputManager_.get(); }
    Debugger* getDebugger() { return debugger_.get(); }
    ToolIntegration* getToolIntegration() { return toolIntegration_.get(); }
    ExperimentController* getExperimentController() { return experimentController_.get(); }
    AdvancedArgumentParser* getArgumentParser() { return argumentParser_.get(); }
    
    // Generate comprehensive report
    std::string generateFeatureReport() const;
    
    // Configuration loading for advanced features
    bool loadAdvancedConfig(const std::string& filename);
    
    // Save advanced configuration
    bool saveAdvancedConfig(const std::string& filename) const;
    
    // Run specific experiment or tool
    bool runExperiment(const std::string& experimentName);
    bool runTool(const std::string& toolName, const std::string& arguments = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Feature components
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    std::unique_ptr<OutputManager> outputManager_;
    std::unique_ptr<Debugger> debugger_;
    std::unique_ptr<ToolIntegration> toolIntegration_;
    std::unique_ptr<ExperimentController> experimentController_;
    std::unique_ptr<AdvancedArgumentParser> argumentParser_;
    
    // Integration helpers
    void setupFeatureIntegration(Brain* brain, const std::shared_ptr<Config>& config);
    void setupArgumentParser();
    void setupExperimentalConditions();
    void setupToolPaths();
};

// Command-line tool entry points
namespace command_line {
    // Advanced simulation tool
    int advancedSimulation(int argc, char** argv);
    
    // Performance benchmarking tool
    int benchmark(int argc, char** argv);
    
    // Data export tool
    int exportData(int argc, char** argv);
    
    // Experiment runner tool
    int runExperiment(int argc, char** argv);
    
    // Debug analysis tool
    int debugAnalysis(int argc, char** argv);
    
    // Configuration tool
    int configTool(int argc, char** argv);
}

// Configuration file formats for advanced features
struct AdvancedConfig {
    std::string filename;
    std::string format; // json, yaml, cfg
    std::map<std::string, std::string> settings;
    
    AdvancedConfig() : filename("advanced_config.cfg"), format("cfg") {}
};

// Feature-specific configuration structures
struct PerformanceConfig {
    bool enableMonitoring;
    float monitoringInterval;
    float cpuThreshold;
    float memoryThreshold;
    size_t maxHistorySize;
    
    PerformanceConfig() : enableMonitoring(true), monitoringInterval(1.0f), 
                         cpuThreshold(80.0f), memoryThreshold(1024.0f), 
                         maxHistorySize(1000) {}
};

struct OutputConfig {
    std::string format;
    std::string destination;
    bool enablePlots;
    bool enableExport;
    std::vector<std::string> visualizationTypes;
    std::string logLevel;
    bool quietMode;
    
    OutputConfig() : format("text"), destination("stdout"), enablePlots(false),
                    enableExport(false), logLevel("INFO"), quietMode(false) {}
};

struct DebugConfig {
    bool enableDebugMode;
    int debugLevel;
    bool enableMemoryDebugging;
    bool enablePerformanceDebugging;
    std::map<std::string, std::string> debugData;
    
    DebugConfig() : enableDebugMode(false), debugLevel(0),
                   enableMemoryDebugging(false), enablePerformanceDebugging(false) {}
};

struct ToolConfig {
    bool enableToolIntegration;
    std::vector<std::string> toolPaths;
    std::map<std::string, std::string> toolConfigurations;
    
    ToolConfig() : enableToolIntegration(true) {}
};

struct ExperimentConfig {
    bool enableExperimentalMode;
    std::map<std::string, std::map<std::string, std::string>> conditions;
    std::map<std::string, std::string> parameters;
    std::string currentCondition;
    
    ExperimentConfig() : enableExperimentalMode(false) {}
};

// Helper functions for advanced features
namespace helpers {
    // Parse advanced configuration file
    AdvancedConfig parseAdvancedConfig(const std::string& filename);
    
    // Merge configuration files
    std::map<std::string, std::string> mergeConfigs(const std::vector<AdvancedConfig>& configs);
    
    // Generate configuration template
    void generateConfigTemplate(const std::string& filename);
    
    // Validate configuration
    bool validateConfig(const AdvancedConfig& config);
    
    // Get feature-specific configuration from global config
    PerformanceConfig getPerformanceConfig(const std::shared_ptr<Config>& config);
    OutputConfig getOutputConfig(const std::shared_ptr<Config>& config);
    DebugConfig getDebugConfig(const std::shared_ptr<Config>& config);
    ToolConfig getToolConfig(const std::shared_ptr<Config>& config);
    ExperimentConfig getExperimentConfig(const std::shared_ptr<Config>& config);
}

} // namespace nlm