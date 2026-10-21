// NLM Command Line Interface
// Provides common operations for the Neural Learning Machine
// Phase 1: Basic Operations
// Planned Phase 2: Advanced Features

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace nlm {

// CLI Operation types
enum class CliOperation {
    Help,
    Version,
    ListConfig,
    SetConfig,
    GetConfig,
    RunDemo,
    ResetBrain,
    SaveBrain,
    LoadBrain,
    ExportConfig,
    ImportConfig,
    ListRegions,
    ListNeurons,
    RunPlasticityTest,
    RunStdpTest,
    ExportSpikes,
    ExportWeights,
    ClearLog,
    ShowLog,
    SetLogLevel,
    ValidateConfig
};

// Advanced configuration options
struct AdvancedConfig {
    std::string configFile;
    std::string outputDir;
    bool verbose;
    bool quiet;
    std::string logLevel;
    bool saveIntermediate;
    int checkpointInterval;
    std::string format;  // json, yaml, txt
    bool validateOnLoad;
    bool backupBeforeSave;
    std::string encryptionKey;  // future feature
    std::vector<std::string> includePaths;
    std::vector<std::string> excludePaths;
    bool experimentalFeatures;
    int maxNeurons;
    double maxMemoryGB;
    std::string performanceMode;  // fast, balanced, accurate
};

// Advanced brain operations
class AdvancedBrainInterface {
public:
    std::shared_ptr<Brain> brain;
    AdvancedConfig config;
    
    AdvancedBrainInterface(std::shared_ptr<Brain> b, const AdvancedConfig& c) 
        : brain(b), config(c) {}
    
    // Advanced configuration management
    bool exportAdvancedConfig(const std::string& filepath) const;
    bool importAdvancedConfig(const std::string& filepath);
    bool validateBrainState() const;
    bool optimizeForPerformance();
    
    // Advanced simulation control
    bool runControlledSimulation(int steps, double timestep, 
                                 const std::map<std::string, double>& inputs);
    bool runAdaptiveSimulation(const std::string& strategy);
    
    // Advanced analysis
    std::map<std::string, double> analyzeNetworkTopology() const;
    std::vector<std::string> analyzeLearningPatterns() const;
    double calculateNetworkComplexity() const;
    
    // Advanced export/import
    bool exportNetworkState(const std::string& filepath, 
                           const std::vector<std::string>& includeTypes);
    bool importNetworkState(const std::string& filepath,
                           const std::vector<std::string>& includeTypes);
    
    // Utility functions
    static AdvancedConfig createAdvancedConfigFromArgs(int argc, char** argv);
    static std::vector<std::string> getAdvancedConfigOptions();
    static std::map<CliOperation, std::string> getAdvancedCommands();
};

// Command line parser
class NLMCommandLine {
public:
    static bool parseArguments(int argc, char** argv);
    static CliOperation getCommand() { return currentCommand; }
    static std::string getArg(const std::string& key);
    static std::vector<std::string> getRemainingArgs();
    static bool hasFlag(const std::string& flag);
    static void printHelp();
    static void printVersion();
    static void printAdvancedHelp();
    
private:
    static CliOperation currentCommand;
    static std::map<std::string, std::string> argMap;
    static std::vector<std::string> remainingArgs;
    static AdvancedConfig advancedConfig;
};

// Core CLI functions
namespace Cli {
    // Configuration management
    bool handleConfigOperations(const std::string& key, const std::string& value);
    bool handleListConfig();
    bool handleExportConfig();
    bool handleImportConfig();
    
    // Brain operations
    bool handleBrainOperations();
    bool handleResetBrain();
    bool handleSaveBrain();
    bool handleLoadBrain();
    
    // Analysis operations
    bool handleAnalysisOperations();
    bool handleListRegions();
    bool handleListNeurons();
    bool handleExportSpikes();
    bool handleExportWeights();
    
    // Simulation operations
    bool handleSimulationOperations();
    bool handleRunDemo();
    bool handleRunPlasticityTest();
    bool handleRunStdpTest();
    
    // Logging operations
    bool handleLogOperations();
    bool handleClearLog();
    bool handleShowLog();
    bool handleSetLogLevel();
    
    // Validation operations
    bool handleValidationOperations();
    bool handleValidateConfig();
    
    // Advanced features
    bool handleAdvancedFeatures();
}

// Advanced demo modes
namespace AdvancedDemo {
    bool runAdvancedPlasticityDemo();
    bool runAdvancedStdpDemo();
    bool runNetworkOptimizationDemo();
    bool runDevelopmentStagesDemo();
    bool runNeuromodulationDemo();
    bool runMemoryIntegrationDemo();
}

// Utility functions
namespace Utils {
    std::string formatNumber(double num, int precision = 2);
    std::string formatTime(double seconds);
    std::string formatBytes(size_t bytes);
    std::string generateTimestamp();
    std::string sanitizeFilename(const std::string& name);
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    bool stringContains(const std::string& str, const std::string& substr);
}

} // namespace nlm
