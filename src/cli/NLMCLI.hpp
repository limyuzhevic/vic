#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    int run(int argc, char** argv);
    
private:
    // Core components
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    // State management
    bool interactiveMode;
    bool scriptMode;
    std::string outputFormat;
    
    // Subcommand handlers
    void initializeCommandLine(int argc, char** argv);
    
    // Subcommand implementations
    int handleBrainCommands(std::vector<std::string>& args);
    int handleSimulationCommands(std::vector<std::string>& args);
    int handleAnalysisCommands(std::vector<std::string>& args);
    int handleConfigCommands(std::vector<std::string>& args);
    int handleCheckpointCommands(std::vector<std::string>& args);
    int handleSystemCommands(std::vector<std::string>& args);
    int handleHelpCommands(std::vector<std::string>& args);
    
    // Output formatting
    void formatOutput(const std::string& data, const std::string& format = "pretty");
    
    // Error handling
    void handleError(const std::string& error, int exitCode = 1);
    
    // Configuration management
    void loadConfig(const std::string& filepath = "configs/default.cfg");
    void saveConfig(const std::string& filepath);
    
    // Brain management
    bool createBrain(const std::string& name, const std::string& configFile = "");
    bool loadBrain(const std::string& name);
    bool saveBrain(const std::string& name);
    bool deleteBrain(const std::string& name);
    bool infoBrain(const std::string& name);
    bool listBrains();
    
    // Simulation management
    bool runSimulation(const std::string& brainName, int steps = 100, double duration = 0.0);
    bool runContinuousSimulation(const std::string& brainName, int steps = -1);
    bool stepSimulation(const std::string& brainName, int step = 1);
    bool pauseSimulation();
    bool resumeSimulation();
    bool stopSimulation();
    
    // Analysis operations
    bool analyzeStats(const std::string& brainName);
    bool analyzeMemory(const std::string& brainName);
    bool analyzeNeuromodulation(const std::string& brainName);
    bool analyzePrediction(const std::string& brainName);
    bool analyzeCognition(const std::string& brainName);
    bool analyzeDevelopment(const std::string& brainName);
    
    // Configuration operations
    bool showConfig(const std::string& key = "");
    bool setConfig(const std::string& key, const std::string& value);
    bool loadConfigFile(const std::string& filepath);
    bool saveConfigFile(const std::string& filepath);
    bool validateConfig();
    bool manageTemplates(const std::string& action, const std::string& name = "");
    
    // Checkpoint operations
    bool createCheckpoint(const std::string& name);
    bool listCheckpoints();
    bool loadCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& name);
    bool restoreCheckpoint(const std::string& name);
    bool compareCheckpoints(const std::string& name1, const std::string& name2);
    
    // System operations
    bool showSystemInfo();
    bool showLogs(int lines = 100, const std::string& level = "INFO");
    bool monitorSystem(int duration = 0, double interval = 1.0);
    bool showSystemStats();
    bool showPerformanceMetrics();
    
    // Help operations
    bool showHelp(const std::string& topic = "");
    bool showExamples();
    bool showTutorial();
    bool showReference();
    
    // Utility functions
    std::string getVersion() const;
    std::string getBuildInfo() const;
    bool isBrainLoaded() const;
    std::string getCurrentBrainName() const;
    
    // Command-line parsing helpers
    std::vector<std::string> parseArguments(int argc, char** argv);
    std::string detectOutputFormat(const std::string& arg);
    std::string detectMode(const std::string& arg);
    
    // File system operations
    bool brainExists(const std::string& name) const;
    std::vector<std::string> listBrainFiles() const;
    std::string getBrainPath(const std::string& name) const;
    
    // Validation
    bool validateBrainName(const std::string& name) const;
    bool validateConfigValue(const std::string& key, const std::string& value) const;
};

} // namespace nlm