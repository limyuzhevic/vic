#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <chrono>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

namespace nlm {

class NLMCLI {
public:
    struct CLIConfig {
        std::string configFile;
        std::string outputFile;
        std::string logFile;
        std::string pluginDirectory;
        std::string performanceReport;
        bool verbose;
        bool quiet;
        int verbosityLevel;
        std::string defaultWorld;
        std::string defaultAgent;
        std::string defaultBrain;
        std::string experimentName;
        std::string experimentDescription;
        bool autoSave;
        bool profilePerformance;
        bool enablePlugins;
        std::string pluginConfig;
    };
    
    NLMCLI();
    ~NLMCLI();
    
    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);
    
    // Execute command
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Main CLI commands
    bool runSimulation(const std::map<std::string, std::string>& options);
    bool createBrain(const std::map<std::string, std::string>& options);
    bool createWorld(const std::map<std::string, std::string>& options);
    bool createAgent(const std::map<std::string, std::string>& options);
    bool runExperiment(const std::map<std::string, std::string>& options);
    bool configurePlugin(const std::map<std::string, std::string>& options);
    bool listPlugins(const std::map<std::string, std::string>& options);
    bool loadPlugin(const std::map<std::string, std::string>& options);
    bool unloadPlugin(const std::map<std::string, std::string>& options);
    bool showConfig(const std::map<std::string, std::string>& options);
    bool saveConfig(const std::map<std::string, std::string>& options);
    bool loadConfig(const std::map<std::string, std::string>& options);
    bool runBenchmark(const std::map<std::string, std::string>& options);
    bool runPerformanceProfiling(const std::map<std::string, std::string>& options);
    bool generatePerformanceReport(const std::map<std::string, std::string>& options);
    bool showHelp(const std::map<std::string, std::string>& options);
    bool showVersion(const std::map<std::string, std::string>& options);
    bool runTestSuite(const std::map<std::string, std::string>& options);
    bool validateConfiguration(const std::map<std::string, std::string>& options);
    
    // Utility functions
    bool parseConfigFile(const std::string& filepath, std::map<std::string, std::string>& config);
    bool saveConfigToFile(const std::string& filepath, const std::map<std::string, std::string>& config);
    bool loadConfigFromFile(const std::string& filepath, std::map<std::string, std::string>& config);
    std::string getConfigValue(const std::map<std::string, std::string>& config, const std::string& key, const std::string& defaultValue = "");
    int getConfigValueAsInt(const std::map<std::string, std::string>& config, const std::string& key, int defaultValue = 0);
    double getConfigValueAsDouble(const std::map<std::string, std::string>& config, const std::string& key, double defaultValue = 0.0);
    bool getConfigValueAsBool(const std::map<std::string, std::string>& config, const std::string& key, bool defaultValue = false);
    
    // Simulation utilities
    bool createDefaultConfig(const std::string& filepath);
    bool runSimpleSimulation(const std::map<std::string, std::string>& options);
    bool runInteractiveSimulation(const std::map<std::string, std::string>& options);
    
    // Plugin utilities
    bool listAvailablePlugins(const std::string& directory);
    bool validatePluginConfiguration(const std::string& pluginName, const std::string& configFile);
    
    // Performance utilities
    bool generatePerformanceReport(const std::string& profileName = "");
    bool runPerformanceBenchmark(const std::map<std::string, std::string>& options);
    bool profileCurrentSimulation(const std::string& outputFile);
    
    // Test utilities
    bool runTestSuite(const std::string& testSuiteName = "");
    bool runSpecificTest(const std::string& testName);
    bool listAvailableTests();
    
    // Output functions
    void printBanner() const;
    void printUsage() const;
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    void printInfo(const std::string& info) const;
    void printSuccess(const std::string& success) const;
    
    // Configuration
    void setConfig(const CLIConfig& config) { cliConfig = config; }
    const CLIConfig& getConfig() const { return cliConfig; }
    
    // Command history
    void addCommandToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const;
    void clearCommandHistory();
    
    // Auto-completion
    std::vector<std::string> getCommandCompletions(const std::string& prefix) const;
    std::vector<std::string> getArgumentCompletions(const std::string& command, const std::string& prefix) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    CLIConfig cliConfig;
    std::vector<std::string> commandHistory;
};

class NLMCmd {
public:
    NLMCmd();
    ~NLMCmd();
    
    bool parseArguments(int argc, char* argv[]);
    bool executeCommand(const std::string& commandLine);
    void printHelp(const std::string& command = "") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class NLMBatch {
public:
    NLMBatch();
    ~NLMBatch();
    
    bool runBatch(const std::string& scriptFile);
    bool runCommands(const std::vector<std::string>& commands);
    bool exportResults(const std::string& outputFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class NLMScript {
public:
    NLMScript();
    ~NLMScript();
    
    bool executeScript(const std::string& scriptFile);
    bool executeScriptContent(const std::string& content);
    std::string getLastOutput() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
inline NLMCLI& getCLI() {
    static NLMCLI cli;
    return cli;
}

inline NLMCmd& getCmd() {
    static NLMCmd cmd;
    return cmd;
}

inline NLMBatch& getBatch() {
    static NLMBatch batch;
    return batch;
}

inline NLMScript& getScript() {
    static NLMScript script;
    return script;
}

inline bool runCLI(int argc, char* argv[]) {
    return getCLI().parseArguments(argc, argv);
}

inline bool runCmd(const std::string& commandLine) {
    return getCmd().executeCommand(commandLine);
}

inline bool runBatch(const std::string& scriptFile) {
    return getBatch().runBatch(scriptFile);
}

inline bool runScript(const std::string& scriptFile) {
    return getScript().executeScript(scriptFile);
}

} // namespace nlm