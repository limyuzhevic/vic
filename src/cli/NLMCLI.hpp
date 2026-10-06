#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();

    // Core CLI methods
    void parseCommandLine(int argc, char* argv[]);
    void runInteractiveShell();
    void executeCommand(const std::string& commandLine);
    
    // Help and documentation
    void displayHelp(const std::string& command = "");
    void displayGeneralHelp() const;
    void displayCommandHelp(const std::string& command) const;
    
    // System status and information
    void displayStatus() const;
    void displaySystemInfo() const;
    void displayConfiguration() const;
    
    // Configuration management
    void loadConfigurationFromFile(const std::string& filename);
    void saveConfigurationToFile(const std::string& filename) const;
    
    // System control
    void applyPreset(const std::string& presetName);
    void optimizeParameters();
    void validateConfiguration();
    
    // Testing
    void runIntegrationTests();
    void runPerformanceTests();
    void runMemoryTests();

    // Brain control commands
    void brainStart() const;
    void brainStop() const;
    void brainStatus() const;
    void brainReset() const;
    void brainConfigure() const;
    void brainSave(const std::string& filename) const;
    void brainLoad(const std::string& filename) const;

    // System management commands
    void systemInfo() const;
    void systemConfig() const;
    void systemPreset(const std::string& presetName) const;
    void systemOptimize() const;
    void systemValidate() const;

    // Memory system commands
    void memoryInfo() const;
    void memoryClear() const;
    void memoryStore(const std::string& data) const;
    void memoryRetrieve(const std::string& query) const;
    void memoryReplay() const;

    // Prediction system commands
    void predictionInfo() const;
    void predictionTrain() const;
    void predictionError() const;
    void predictionConfidence() const;

    // Neuromodulation commands
    void neuromodulationInfo() const;
    void neuromodulationSet(const std::string& type, float level) const;
    void neuromodulationAuto() const;
    void neuromodulationManual() const;

    // Cognitive system commands
    void cognitionInfo() const;
    void cognitionPlan() const;
    void cognitionConcepts() const;
    void cognitionAttention(const std::string& action) const;

    // Development commands
    void developmentInfo() const;
    void developmentStage(const std::string& stage) const;
    void developmentAccelerate() const;
    void developmentSlow() const;

    // Checkpoint commands
    void checkpointInfo() const;
    void checkpointSave(const std::string& filename) const;
    void checkpointLoad(const std::string& filename) const;
    void checkpointList() const;

    // Testing commands
    void testIntegration() const;
    void testPerformance() const;
    void testMemory() const;
    void testPrediction() const;
    void testCognitive() const;

    // Help and documentation commands
    void helpCommand(const std::string& command = "");
    void about() const;
    void license() const;
    void version() const;

    // Interactive shell features
    void setHistorySize(size_t size);
    void setTabCompletion(bool enabled);
    void setCommandAlias(const std::string& alias, const std::string& command);
    void setMacro(const std::string& name, const std::string& macro);
    
    // Session management
    void saveSession(const std::string& filename) const;
    void loadSession(const std::string& filename);
    void listSessions() const;

    // Script mode
    void executeScript(const std::string& filename);
    void setScriptMode(bool enabled);

    // Utility methods
    bool isRunning() const { return isRunning_; }
    void stop() { isRunning_ = false; }

protected:
    // Command parsing helpers
    std::vector<std::string> tokenizeCommand(const std::string& commandLine) const;
    std::string extractCommandName(const std::string& commandLine) const;
    std::map<std::string, std::string> extractCommandArguments(const std::string& commandLine) const;
    
    // Command validation
    bool isValidCommand(const std::string& commandName) const;
    bool hasRequiredArguments(const std::string& commandName, size_t requiredArgs) const;
    
    // Output formatting
    void printStatus(const std::string& message) const;
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    void printSuccess(const std::string& success) const;

private:
    bool isRunning_;
    std::map<std::string, std::function<void(const std::map<std::string, std::string>&)>> commandHandlers_;
    std::vector<std::string> commandHistory_;
    std::map<std::string, std::string> commandAliases_;
    std::map<std::string, std::string> macros_;
    size_t historySize_;
    bool tabCompletionEnabled_;
    bool scriptMode_;
};
