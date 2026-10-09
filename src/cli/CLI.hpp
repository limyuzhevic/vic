#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <algorithm>

namespace nlm {

// CLI Command structure
struct CLICommand {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<std::string> examples;
    std::function<bool(const std::vector<std::string>&)> execute;
    std::vector<std::string> aliases;
    std::string category;
    bool requiresBrain;
    bool requiresConfig;
};

// CLI Option structure
struct CLIOption {
    std::string longName;
    char shortName;
    std::string description;
    std::string defaultValue;
    bool hasValue;
    bool required;
    std::string type;
    std::vector<std::string> allowedValues;
    std::function<bool(const std::string&)> validator;
};

// CLI State
struct CLIState {
    bool verbose;
    bool quiet;
    int logLevel;
    bool interactive;
    bool scriptMode;
    std::string scriptFile;
    bool showHelp;
    bool showVersion;
    std::string outputFormat;
    bool colorOutput;
    bool progressBar;
    double performanceThreshold;
    std::chrono::steady_clock::time_point startTime;
    std::string workingDirectory;
    
    CLIState() : verbose(false), quiet(false), logLevel(2), interactive(true), 
                  scriptMode(false), showHelp(false), showVersion(false),
                  outputFormat("text"), colorOutput(true), progressBar(true),
                  performanceThreshold(0.1) {
        startTime = std::chrono::steady_clock::now();
    }
};

// Main CLI class
class CLI {
public:
    CLI();
    ~CLI();
    
    // Disable copying, enable moving
    CLI(const CLI&) = delete;
    CLI& operator=(const CLI&) = delete;
    CLI(CLI&&) noexcept;
    CLI& operator=(CLI&&) noexcept;
    
    // Command management
    void registerCommand(const CLICommand& command);
    void unregisterCommand(const std::string& name);
    bool executeCommand(const std::string& commandLine);
    bool executeCommand(const std::vector<std::string>& args);
    
    // Option management
    void addOption(const CLIOption& option);
    bool parseOptions(const std::vector<std::string>& args, std::unordered_map<std::string, std::string>& parsedOptions);
    
    // State management
    void updateState(const std::string& key, const std::string& value);
    const CLIState& getState() const { return state; }
    
    // Help and documentation
    void showHelp(const std::string& command = "") const;
    void showVersion() const;
    void showCommandHelp(const std::string& command) const;
    void showAllCommands() const;
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partialCommand) const;
    std::vector<std::string> getOptionCompletions(const std::string& command, const std::string& partialOption) const;
    
    // Scripting and interactive mode
    bool runScript(const std::string& filename);
    void startInteractiveMode();
    void handleCommand(const std::string& commandLine);
    
    // Output and display
    void print(const std::string& message, int level = 0) const;
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    void printInfo(const std::string& info) const;
    void printSuccess(const std::string& success) const;
    void printProgress(const std::string& progress, double percentage = -1.0) const;
    
    // Configuration integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    
    // Performance and timing
    void startTimer() { state.startTime = std::chrono::steady_clock::now(); }
    double getElapsedTime() const;
    bool isSlowCommand(double threshold = -1.0) const;
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex); }
    void unlock() { std::unlock_guard<std::mutex> lock(mutex); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    CLIState state;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::mutex mutex;
    
    // Internal helpers
    std::vector<std::string> tokenize(const std::string& commandLine) const;
    std::string trim(const std::string& str) const;
    bool matchesPattern(const std::string& str, const std::string& pattern) const;
    void setupDefaultCommands();
    void setupSystemCommands();
};
