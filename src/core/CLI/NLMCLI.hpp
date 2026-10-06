// Advanced command-line interface for NLM
// Provides subcommands for different operations and enhanced user experience

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <iomanip>
#include "core/Config/Config.hpp"

namespace nlm {

// Command line argument parser
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(const std::map<std::string, std::string>&)> handler;
        std::map<std::string, std::string> defaultArgs;
        std::vector<std::string> requiredArgs;
    };
    
    CLIParser(const std::string& programName);
    
    // Add a subcommand
    void addCommand(const std::string& name, const std::string& description,
                   const std::string& usage,
                   std::function<void(const std::map<std::string, std::string>&)> handler,
                   const std::map<std::string, std::string>& defaults = {},
                   const std::vector<std::string>& required = {});
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]);
    
    // Get current command
    const Command* getCurrentCommand() const { return currentCommand_; }
    
    // Get parsed arguments
    const std::map<std::string, std::string>& getArgs() const { return parsedArgs_; }
    
    // Show help
    void showHelp() const;
    
    // Show command help
    void showCommandHelp(const std::string& commandName) const;
    
private:
    std::string programName_;
    const Command* currentCommand_;
    std::map<std::string, Command> commands_;
    std::map<std::string, std::string> parsedArgs_;
    
    // Helper functions
    void parseArgs(int argc, char* argv[]);
    void printUsage() const;
};

// Advanced configuration file support
class AdvancedConfig {
public:
    static bool loadYAML(const std::string& filepath, std::shared_ptr<Config> config);
    static bool saveYAML(const std::string& filepath, const std::shared_ptr<Config>& config);
    static bool loadJSON(const std::string& filepath, std::shared_ptr<Config> config);
    static bool saveJSON(const std::string& filepath, const std::shared_ptr<Config>& config);
    
    // Configuration validation
    static bool validateConfig(const std::shared_ptr<Config>& config, std::vector<std::string>& errors);
    static bool applyConfigPresets(std::shared_ptr<Config> config, const std::string& preset);
    
private:
    static void yamlToConfig(const std::string& yaml, std::shared_ptr<Config> config);
    static void configToYAML(const std::shared_ptr<Config>& config, std::string& yaml);
};

// Performance benchmarking
class Benchmarker {
public:
    struct BenchmarkResults {
        double timeSeconds;
        size_t neurons;
        size_t synapses;
        size_t spikes;
        double averageFiringRate;
        std::string details;
    };
    
    static BenchmarkResults runBenchmark(const std::shared_ptr<Config>& config, 
                                         int durationSteps = 1000,
                                         int warmupSteps = 100);
    static void compareConfigurations(const std::vector<std::shared_ptr<Config>>& configs,
                                     const std::string& benchmarkName);
};

// Visualization and monitoring
class Visualizer {
public:
    static void generateNeuralSnapshot(const std::shared_ptr<Config>& config, 
                                       const std::string& outputFile,
                                       const std::string& format = "png");
    static void createNetworkGraph(const std::shared_ptr<Config>& config,
                                   const std::string& outputFile,
                                   bool includeWeights = true);
    static void generateStatisticsReport(const std::shared_ptr<Config>& config,
                                         const std::string& outputFile,
                                         const std::string& format = "txt");
};

// Main CLI application
class NLMApplication {
public:
    NLMApplication(const std::string& programName);
    
    // Setup subcommands
    void setupCommands();
    
    // Run the application
    int run(int argc, char* argv[]);
    
    // Configuration management
    bool loadConfig(const std::string& filepath);
    bool saveConfig(const std::string& filepath);
    void printConfig() const;
    
    // Brain control
    bool createBrain();
    void runSimulation(int steps, int episodes = 1);
    void saveBrainState(const std::string& filepath) const;
    void loadBrainState(const std::string& filepath);
    
    // Experimental controls
    void runPhase3Demo(int episodes, int steps);
    void runPhase4Demo(int trials);
    void runPhase6Demo(int steps);
    
    // Monitoring and profiling
    void startProfiling() const;
    void stopProfiling() const;
    void showPerformanceStats() const;
    
    // Help and information
    void showVersion() const;
    void showBuildInfo() const;
    void showSupportedFormats() const;
    
private:
    std::string programName_;
    CLIParser parser_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    bool isProfiling_;
    double startTime_;
    
    // Command handlers
    void handleRunCommand(const std::map<std::string, std::string>& args);
    void handleConfigCommand(const std::map<std::string, std::string>& args);
    void handleBenchmarkCommand(const std::map<std::string, std::string>& args);
    void handleSaveCommand(const std::map<std::string, std::string>& args);
    void handleLoadCommand(const std::map<std::string, std::string>& args);
    void handleProfileCommand(const std::map<std::string, std::string>& args);
    void handleStatsCommand(const std::map<std::string, std::string>& args);
    void handleHelpCommand(const std::map<std::string, std::string>& args);
    void handleVersionCommand(const std::map<std::string, std::string>& args);
    
    // Helper methods
    void ensureBrainInitialized() const;
    std::string formatTime(double seconds) const;
    void validateArguments(const std::map<std::string, std::string>& args,
                          const std::vector<std::string>& required) const;
};

} // namespace nlm

#endif // NLM_CLI_HPP
