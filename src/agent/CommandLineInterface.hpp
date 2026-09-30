#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

namespace nlm {

// Command-line argument parser with advanced features
class CommandLineInterface {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::string longDescription;
        std::vector<std::string> examples;
        std::function<bool(const std::vector<std::string>&)> handler;
        bool requiresArgs = false;
        bool hidden = false;
    };
    
    struct AgentConfig {
        // Neural network configuration
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
        float connectionProbability = 0.15f;
        
        // Agent behavior configuration
        float curiosityThreshold = 0.3f;
        float explorationChanceMax = 0.3f;
        float motorActivationThreshold = 0.5f;
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
        bool enableDevelopment = true;
        bool enableCuriosity = true;
        
        // Performance configuration
        double simulationTimestep = 0.001;
        int maxSteps = 10000;
        bool enablePerformanceMonitoring = false;
        
        // Advanced configuration
        std::string configFile;
        bool validateConfig = false;
        std::string outputFormat = "json";
        
        // Development/testing options
        bool enableDebugMode = false;
        bool enableBenchmark = false;
        std::string benchmarkFile;
        std::string logLevel = "info";
    };
    
    struct Statistics {
        double startTime;
        double endTime;
        size_t totalSteps;
        size_t spikeCount;
        size_t firingNeuronCount;
        size_t synapseCount;
        float averageSpikesPerSecond;
        float averageFiringRate;
        std::map<std::string, float> customMetrics;
    };

    CommandLineInterface();
    ~CommandLineInterface();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed configuration
    const AgentConfig& getConfig() const { return config_; }
    const Statistics& getStatistics() const { return stats_; }
    
    // Get command help
    void printHelp(const std::string& command = "") const;
    void printVersion() const;
    
    // Get/set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    AgentConfig& getConfig() { return config_; }
    
    // Configuration validation
    bool validateConfig() const;
    
    // Statistics collection
    void recordStep(size_t spikes, size_t firingNeurons);
    void finalizeStatistics();
    
    // Command management
    void registerCommand(const Command& cmd);
    const Command* findCommand(const std::string& name) const;
    
    // Output formatting
    void printConfig(const AgentConfig& config) const;
    void exportConfig(const std::string& filename) const;
    void importConfig(const std::string& filename);
    void printStatistics() const;
    void exportStatistics(const std::string& filename) const;
    
    // Configuration file handling
    static bool isConfigFile(const std::string& filename);
    static std::string expandConfigPath(const std::string& filename);
    
private:
    std::vector<Command> commands_;
    AgentConfig config_;
    Statistics stats_;
    std::chrono::steady_clock::time_point sessionStart_;
    
    // Argument parsing
    void parseArguments(int argc, char** argv);
    void processArgument(const std::string& arg, std::vector<std::string>& remainingArgs);
    
    // Configuration parsing
    void parseConfigFile(const std::string& filename);
    void applyConfigOverrides(const std::map<std::string, std::string>& overrides);
    
    // Command handlers
    bool handleHelp(const std::vector<std::string>& args);
    bool handleVersion(const std::vector<std::string>& args);
    bool handleNeuronConfig(const std::vector<std::string>& args);
    bool handleRegionConfig(const std::vector<std::string>& args);
    bool handleConnectionConfig(const std::vector<std::string>& args);
    bool handleCuriosityConfig(const std::vector<std::string>& args);
    bool handlePlasticityConfig(const std::vector<std::string>& args);
    bool handlePerformanceConfig(const std::vector<std::string>& args);
    bool handleDebugConfig(const std::vector<std::string>& args);
    bool handleExportConfig(const std::vector<std::string>& args);
    bool handleImportConfig(const std::vector<std::string>& args);
    bool handleResetConfig(const std::vector<std::string>& args);
    bool handleListCommands(const std::vector<std::string>& args);
    
    // Output utilities
    void printBanner() const;
    void printSection(const std::string& title) const;
    void printOption(const std::string& option, const std::string& description) const;
    std::string formatTime(double seconds) const;
    std::string formatFloat(float value) const;
    
    // File utilities
    bool fileExists(const std::string& filename) const;
    std::string getCurrentWorkingDirectory() const;
    
    // Statistics utilities
    double getElapsedTime() const;
    float calculateAverageSpikesPerSecond() const;
    float calculateAverageFiringRate() const;
};

} // namespace nlm
