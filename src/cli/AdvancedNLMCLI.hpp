// Advanced NLM CLI Interface
// Enhanced command-line interface for NLM Neural Learning Machine
// Part 3: Advanced commands and power user features

#ifndef ADVANCED_CLI_HPP
#define ADVANCED_CLI_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

namespace nlm {
namespace advanced_cli {

using json = nlohmann::json;

// Advanced configuration options for power users
struct AdvancedConfig {
    // Performance settings
    int maxThreads = std::thread::hardware_concurrency();
    size_t maxMemoryUsage = 1024 * 1024 * 1024; // 1GB
    bool enableProfiling = false;
    bool enableBenchmarking = false;
    
    // Advanced neural parameters
    float learningRate = 0.01f;
    float discountFactor = 0.99f;
    float explorationRate = 0.1f;
    int episodeLength = 1000;
    
    // Memory system settings
    size_t workingMemoryCapacity = 10000;
    size_t episodicMemoryMaxEpisodes = 10000;
    float episodicMemoryDecay = 0.95f;
    
    // Neuromodulation settings
    float dopamineLevel = 1.0f;
    float curiosityLevel = 0.5f;
    float noveltySensitivity = 1.0f;
    
    // Developmental settings
    bool enableDevelopment = true;
    float plasticityDecayRate = 0.001f;
    bool enableCriticalPeriod = true;
    
    // Export/import settings
    std::string checkpointCompression = "balanced";
    bool checkpointEncryption = false;
    std::string defaultExportFormat = "json";
    
    // Monitoring settings
    bool enableLiveMonitoring = false;
    std::chrono::seconds monitoringInterval{5};
    bool enableRemoteMonitoring = false;
    std::string monitoringHost = "localhost";
    int monitoringPort = 8080;
    
    // Experimental settings
    bool enableRandomSeed = true;
    uint64_t defaultRandomSeed = 42;
    bool enableReproducibleResults = false;
    
    // Safety settings
    bool enableSafetyChecks = true;
    float maxSynapseWeight = 1.0f;
    float minSynapseWeight = -1.0f;
    float maxNeuronFiringRate = 100.0f;
    int maxStepCount = 1000000;
};

// Advanced command types for power users
struct AdvancedCommand {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> examples;
    std::map<std::string, std::string> options;
    std::function<bool(const std::map<std::string, std::string>&)> handler;
    bool requiresAdmin = false;
    bool dangerous = false;
    std::string riskLevel = "low";
};

// Advanced NLM CLI with power user features
class AdvancedNLMCLI {
public:
    AdvancedNLMCLI();
    ~AdvancedNLMCLI();
    
    // Initialize with advanced configuration
    void initialize(const AdvancedConfig& config);
    
    // Parse and execute advanced commands
    bool execute(const std::vector<std::string>& args);
    
    // Add advanced command
    void addCommand(const AdvancedCommand& cmd);
    
    // Get available commands
    const std::vector<AdvancedCommand>& getCommands() const { return commands_; }
    
    // Get advanced configuration
    const AdvancedConfig& getConfig() const { return config_; }
    
    // Show advanced help
    void showAdvancedHelp(const std::string& command = "") const;
    
    // Show advanced statistics
    void showAdvancedStats() const;
    
    // Performance profiling
    void startProfiling() { profilingActive_ = true; profilingStart_ = std::chrono::high_resolution_clock::now(); }
    void stopProfiling();
    void showProfilingResults() const;
    
    // Benchmarking
    void runBenchmark(const std::string& benchmarkName, std::function<void()> benchmarkFunc);
    
    // Script execution
    bool executeScript(const std::string& scriptPath);
    
    // Remote monitoring
    bool startRemoteMonitoring();
    void stopRemoteMonitoring();
    
    // Backup and recovery
    bool createBackup(const std::string& backupPath);
    bool restoreFromBackup(const std::string& backupPath);

private:
    AdvancedConfig config_;
    std::vector<AdvancedCommand> commands_;
    bool profilingActive_ = false;
    std::chrono::high_resolution_clock::time_point profilingStart_;
    std::map<std::string, std::chrono::duration<double>> benchmarks_;
    
    // Advanced command handlers
    bool handleAdvancedBrainCommand(const std::map<std::string, std::string>& args);
    bool handleAdvancedExperimentCommand(const std::map<std::string, std::string>& args);
    bool handleAdvancedSystemCommand(const std::map<std::string, std::string>& args);
    bool handleAdvancedMonitoringCommand(const std::map<std::string, std::string>& args);
    bool handleAdvancedExportCommand(const std::map<std::string, std::string>& args);
    bool handleAdvancedImportCommand(const std::map<std::string, std::string>& args);
    
    // Advanced utility functions
    bool parseAdvancedArgs(const std::vector<std::string>& args, std::map<std::string, std::string>& result);
    void printAdvancedUsage() const;
    void printAdvancedCommandHelp(const AdvancedCommand& cmd) const;
    std::string formatTime(const std::chrono::system_clock::time_point& tp) const;
    std::string escapeJsonString(const std::string& str) const;
    
    // Advanced helper functions
    void loadConfigFromFile(const std::string& filepath);
    void saveConfigToFile(const std::string& filepath) const;
    void validateConfiguration() const;
    void setupLogging() const;
    void setupPerformanceSettings() const;
};

} // namespace advanced_cli
} // namespace nlm

#endif // ADVANCED_CLI_HPP
