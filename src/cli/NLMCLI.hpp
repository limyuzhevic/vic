// NLM Command Line Interface
// Provides comprehensive control over NLM simulations and experiments

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <csignal>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

// CLI command categories
enum class CommandCategory {
    General,        // General commands (help, version, quit)
    Simulation,     // Simulation control (run, pause, reset)
    Configuration,  // Configuration management
    DataManagement, // Data export, import, backup
    Debugging,      // Debugging and analysis tools
    Advanced        // Advanced features for power users
};

// NLM CLI application
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Main entry point
    int run(int argc, char* argv[]);
    
    // Command processing
    bool processCommand(const std::string& input);
    
    // Command handlers
    bool handleHelp(const std::vector<std::string>& args);
    bool handleVersion(const std::vector<std::string>& args);
    bool handleRun(const std::vector<std::string>& args);
    bool handleConfig(const std::vector<std::string>& args);
    bool handleExport(const std::vector<std::string>& args);
    bool handleImport(const std::vector<std::string>& args);
    bool handleDebug(const std::vector<std::string>& args);
    bool handleStatus(const std::vector<std::string>& args);
    bool handleReset(const std::vector<std::string>& args);
    bool handleQuit(const std::vector<std::string>& args);
    
    // Simulation control
    bool startSimulation(const std::string& experimentType, uint64_t steps, const std::string& configFile);
    bool pauseSimulation();
    bool resumeSimulation();
    bool stopSimulation();
    
    // Configuration management
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename) const;
    bool showConfig() const;
    bool setConfig(const std::string& key, const std::string& value);
    
    // Data management
    bool exportData(const std::string& filename, const std::string& format = "json");
    bool importData(const std::string& filename);
    bool backupData(const std::string& filename);
    
    // Debugging and analysis
    bool showStatus() const;
    bool showMetrics() const;
    bool generateReport(const std::string& filename);
    bool debugSystem(const std::string& component);
    
    // Advanced features
    bool batchRun(const std::string& configPattern, const std::string& outputDir);
    bool parallelRun(size_t numProcesses, const std::string& config);
    bool enableProfiler(const std::string& outputFile);
    
    // Utility functions
    void displayPrompt() const;
    void displayWelcome() const;
    void displayHelp() const;
    void displayVersion() const;
    
    // State management
    bool isRunning() const { return isRunning_; }
    bool isPaused() const { return isPaused_; }
    const std::string& getCurrentExperiment() const { return currentExperiment_; }
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Current experiment
    std::shared_ptr<Phase6IntegratedExperiment> currentExperiment_;
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<AgentBrain> currentAgent_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    
    // State
    bool isRunning_;
    bool isPaused_;
    uint64_t simulationSteps_;
    std::chrono::steady_clock::time_point startTime_;
    std::string currentExperiment_;
    
    // Output
    std::ofstream logFile_;
    std::ofstream debugFile_;
    
    // Signal handling
    static bool shouldExit_;
    
    // Command history
    std::vector<std::string> commandHistory_;
    size_t historyIndex_;
    
    // Statistics
    uint64_t totalStepsRun_;
    float totalReward_;
    double totalWallTime_;
    
    // Error handling
    void logError(const std::string& message);
    void logWarning(const std::string& message);
    void logInfo(const std::string& message);
    void logDebug(const std::string& message);
    
    // Configuration parsing
    std::string parseKeyValue(const std::string& input, std::string& key, std::string& value) const;
    bool parseConfigLine(const std::string& line);
    
    // File operations
    bool loadFromFile(const std::string& filename, std::stringstream& stream);
    bool saveToFile(const std::string& filename, const std::stringstream& stream) const;
    
    // Signal handler
    static void signalHandler(int signal);
};