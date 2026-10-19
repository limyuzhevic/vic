// NLM CLI Advanced Commands Header
// This header provides the main interface for advanced CLI commands

#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <filesystem>

namespace nlm {
namespace cli {

// Command line arguments structure
struct CLIArgs {
    std::string command;
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    bool verbose = false;
    bool quiet = false;
    std::string configFile;
    std::string outputFile;
    int timeout = 0;
    int jobs = 1;
    bool force = false;
    bool dryRun = false;
    std::string profile;
    std::string debugLevel;
    std::string scriptFile;
    std::vector<std::string> scriptArgs;
    std::string experimentFile;
    std::string resultsFile;
    std::string baselineFile;
    std::string comparisonFile;
    std::string batchFile;
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::map<std::string, std::string> customOptions;
};

// Advanced CLI main interface
class NLMAdvancedCLI {
public:
    NLMAdvancedCLI();
    ~NLMAdvancedCLI();
    
    // Main entry point
    int run(int argc, char** argv);
    
private:
    // System components
    class AdvancedConfigManager* configManager_;
    class AdvancedPerformanceMonitor* perfMonitor_;
    class AdvancedDebugger* debugger_;
    class BatchProcessor* batchProcessor_;
    class DataManager* dataManager_;
    class ScriptEngine* scriptEngine_;
    class ExtendedHelpSystem* helpSystem_;
    
    // Command handlers
    std::map<std::string, std::function<int(const CLIArgs&)>> commandHandlers_;
    
    // Parse command line arguments
    void parseArguments(int argc, char** argv);
    
    // Command handler registration
    void registerCommandHandlers();
    
    // System initialization
    void initializeSystems();
    void cleanupSystems();
    void loadDefaultConfiguration();
    
    // Command implementations (forward declarations)
    int handleConfigCommand(const CLIArgs& args);
    int handleConfigValidateCommand(const CLIArgs& args);
    int handleConfigDiffCommand(const CLIArgs& args);
    int handleConfigCompareCommand(const CLIArgs& args);
    int handleConfigExportCommand(const CLIArgs& args);
    int handleConfigImportCommand(const CLIArgs& args);
    int handleConfigRollbackCommand(const CLIArgs& args);
    int handleConfigMergeCommand(const CLIArgs& args);
    int handleConfigInheritCommand(const CLIArgs& args);
    
    int handlePerformanceCommand(const CLIArgs& args);
    int handlePerformanceMonitorCommand(const CLIArgs& args);
    int handlePerformanceThresholdsCommand(const CLIArgs& args);
    int handlePerformanceAlertCommand(const CLIArgs& args);
    int handlePerformanceAnalysisCommand(const CLIArgs& args);
    int handlePerformanceRegressionCommand(const CLIArgs& args);
    int handlePerformanceBenchmarkCommand(const CLIArgs& args);
    
    int handleDebugCommand(const CLIArgs& args);
    int handleDebugTraceCommand(const CLIArgs& args);
    int handleDebugMemoryCommand(const CLIArgs& args);
    int handleDebugBottleneckCommand(const CLIArgs& args);
    int handleDebugProfileCommand(const CLIArgs& args);
    int handleDebugHeapCommand(const CLIArgs& args);
    
    int handleBatchCommand(const CLIArgs& args);
    int handleBatchRunCommand(const CLIArgs& args);
    int handleBatchTestCommand(const CLIArgs& args);
    int handleBatchConfigCommand(const CLIArgs& args);
    int handleBatchExperimentCommand(const CLIArgs& args);
    int handleBatchParallelCommand(const CLIArgs& args);
    
    int handleDataCommand(const CLIArgs& args);
    int handleDataExportCommand(const CLIArgs& args);
    int handleDataImportCommand(const CLIArgs& args);
    int handleDataAnalyzeCommand(const CLIArgs& args);
    int handleDataCompareCommand(const CLIArgs& args);
    int handleDataStatsCommand(const CLIArgs& args);
    int handleDataVisualizeCommand(const CLIArgs& args);
    
    int handleScriptCommand(const CLIArgs& args);
    int handleScriptRunCommand(const CLIArgs& args);
    int handleScriptParseCommand(const CLIArgs& args);
    int handleScriptValidateCommand(const CLIArgs& args);
    
    int handleSystemCommand(const CLIArgs& args);
    int handleSystemStatusCommand(const CLIArgs& args);
    int handleSystemResourcesCommand(const CLIArgs& args);
    int handleSystemProcessesCommand(const CLIArgs& args);
    
    int handleHelpCommand(const CLIArgs& args);
    int handleAdvancedHelpCommand(const CLIArgs& args);
    int handleCommandsHelpCommand(const CLIArgs& args);
    int handleExamplesHelpCommand(const CLIArgs& args);
    
    int handleVersionCommand(const CLIArgs& args);
    int handleInitCommand(const CLIArgs& args);
    int handleCheckCommand(const CLIArgs& args);
    int handleValidateCommand(const CLIArgs& args);
    int handleInfoCommand(const CLIArgs& args);
    
    // Helper methods
    void createDefaultConfigFile();
    void createAdvancedConfigFile();
};

} // namespace cli
} // namespace nlm
