#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Config/ConfigManager.hpp"
#include "../core/CommonUtils.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

// Command line interface for NLM brain simulation
class CommandLineInterface {
public:
    struct CommandOptions {
        std::string configFile;
        std::string outputFile;
        std::string logFile;
        std::string checkpointFile;
        std::string checkpointLoad;
        std::string checkpointSave;
        
        int simulationSteps;
        double simulationTime;
        int randomSeed;
        
        bool help;
        bool version;
        bool verbose;
        bool quiet;
        bool benchmark;
        bool demo;
        bool test;
        
        std::vector<std::string> configOverrides;
        std::vector<std::string> pluginPaths;
        
        std::string executionMode;
        std::string brainMode;
        std::string outputFormat;
        
        // Default constructor
        CommandOptions() : simulationSteps(1000), simulationTime(0.0), randomSeed(42),
                          help(false), version(false), verbose(false), quiet(false),
                          benchmark(false), demo(false), test(false),
                          executionMode("standard"), brainMode("integrated"),
                          outputFormat("text") {}
    };
    
    struct CommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> aliases;
        std::function<bool(const CommandOptions&)> handler;
        std::map<std::string, std::string> parameters;
    };
    
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Display help information
    void showHelp(const std::string& command = "");
    void showVersion();
    void showUsage(const std::string& command);
    void showExamples();
    
    // Execute command
    bool executeCommand(const std::string& command, const CommandOptions& options);
    
    // Command handlers
    bool handleRun(const CommandOptions& options);
    bool handleDemo(const CommandOptions& options);
    bool handleBenchmark(const CommandOptions& options);
    bool handleTest(const CommandOptions& options);
    bool handleConfig(const CommandOptions& options);
    bool handleCheckpoint(const CommandOptions& options);
    bool handleValidate(const CommandOptions& options);
    bool handleExport(const CommandOptions& options);
    
    // Configuration management
    bool loadConfiguration(const CommandOptions& options, std::shared_ptr<Config> config);
    bool saveConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config);
    
    // Brain management
    std::shared_ptr<Brain> createBrain(const CommandOptions& options, const std::shared_ptr<Config>& config);
    bool runBrainSimulation(std::shared_ptr<Brain> brain, const CommandOptions& options);
    bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Output and reporting
    void printBanner() const;
    void printConfiguration(const std::shared_ptr<Config>& config) const;
    void printBrainStatus(std::shared_ptr<Brain> brain) const;
    void printBenchmarkResults(const std::string& operation, double duration, const std::string& details = "") const;
    void printError(const std::string& message, const std::string& command = "") const;
    void printWarning(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printDebug(const std::string& message) const;
    
    // Interactive mode
    bool runInteractiveMode();
    std::string getUserInput(const std::string& prompt = "> ") const;
    bool processInteractiveCommand(const std::string& command);
    
    // History and command completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const;
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    
    // Configuration validation
    bool validateConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config);
    std::vector<std::string> getConfigurationWarnings(const std::shared_ptr<Config>& config);
    
    // Plugin system
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    std::vector<std::string> listAvailablePlugins() const;
    
    // Statistics and monitoring
    void printSystemInfo() const;
    void printMemoryUsage() const;
    void printPerformanceStats() const;
    double getElapsedTime() const;
    
    // Configuration template generation
    bool generateConfigTemplate(const std::string& filepath, const std::vector<std::string>& sections);
    bool exportConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config, const std::string& format = "json");
    
    // Command registration
    void registerCommand(const CommandInfo& commandInfo);
    std::vector<CommandInfo> getAvailableCommands() const;
    CommandInfo* findCommand(const std::string& name);
    
    // State management
    void setOptions(const CommandOptions& options) { this->options = options; }
    const CommandOptions& getOptions() const { return options; }
    bool isInitialized() const { return initialized; }
    bool initialize();
    void cleanup();
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&, const std::string&)> handler);
    void setOutputHandler(std::function<void(const std::string&, bool isError)> handler);
    
    // File operations
    static bool fileExists(const std::string& filepath);
    static std::string getAbsolutePath(const std::string& path);
    static std::string getFileExtension(const std::string& filename);
    static std::string getFileName(const std::string& path);
    static std::string getFileNameWithoutExtension(const std::string& filename);
    
private:
    CommandOptions options;
    bool initialized;
    
    // Command registry
    std::map<std::string, CommandInfo> commands;
    
    // Output handlers
    std::function<void(const std::string&, const std::string&)> errorHandler;
    std::function<void(const std::string&, bool)> outputHandler;
    
    // State management
    std::chrono::system_clock::time_point startTime;
    std::vector<std::string> commandHistory;
    
    // Plugin system
    std::map<std::string, void*> loadedPlugins;
    std::vector<std::string> pluginPaths;
    
    // Statistics
    struct PerformanceStats {
        double totalExecutionTime;
        size_t commandCount;
        size_t errorCount;
        size_t warningCount;
        
        PerformanceStats() : totalExecutionTime(0.0), commandCount(0), errorCount(0), warningCount(0) {}
    };
    
    PerformanceStats stats;
    
    // Helper methods
    static std::string formatDuration(double seconds);
    static std::string getTimestamp();
    static void trimString(std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringEndsWith(const std::string& str, const std::string& suffix);
    static bool stringStartsWith(const std::string& str, const std::string& prefix);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static bool isValidFilename(const std::string& filename);
    static std::string getSanitizedFilename(const std::string& filename);
    static std::string generateUniqueFilename(const std::string& basePath, const std::string& baseName);
    
    // Command line parsing
    static bool parseCommandLine(int argc, char** argv, CommandOptions& options);
    static bool parseCommandLineArguments(int argc, char** argv, std::map<std::string, std::string>& parsedArgs);
    static bool parseKeyValuePairs(const std::vector<std::string>& args, std::map<std::string, std::string>& result);
    static bool parseBooleanFlag(int argc, char** argv, const std::string& flag, bool& value);
    static bool parseIntegerOption(int argc, char** argv, const std::string& option, int64_t& value);
    static bool parseFloatOption(int argc, char** argv, const std::string& option, double& value);
    static bool parseStringOption(int argc, char** argv, const std::string& option, std::string& value);
    
    // Configuration loading
    static std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath);
    static bool applyConfigOverrides(std::shared_ptr<Config> config, const std::vector<std::string>& overrides);
    static std::shared_ptr<Config> mergeConfigs(const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& overrides);
    
    // Brain simulation
    static bool runSimulation(std::shared_ptr<Brain> brain, const CommandOptions& options);
    static bool runDemoMode(std::shared_ptr<Brain> brain, const CommandOptions& options);
    static bool runBenchmarkMode(std::shared_ptr<Brain> brain, const CommandOptions& options);
    
    // Output formatting
    static void printTable(const std::vector<std::vector<std::string>>& data);
    static void printProgress(const std::string& message, double progress);
    static void printStatus(const std::string& status, const std::string& details = "");
    
    // Error handling
    static void defaultErrorHandler(const std::string& message, const std::string& command);
    static void defaultOutputHandler(const std::string& message, bool isError);
};

} // namespace nlm