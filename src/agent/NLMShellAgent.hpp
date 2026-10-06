#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "NLMCLI.hpp"

namespace nlm {

class NLMShellAgent {
public:
    NLMShellAgent(std::shared_ptr<NLMCLI> cli);
    ~NLMShellAgent();

    // Core shell agent methods
    void start();
    void stop();
    void run();
    void processCommand(const std::string& commandLine);
    
    // State management
    void setState(const std::string& key, const std::string& value);
    std::string getState(const std::string& key) const;
    void clearState(const std::string& key);
    void clearAllState();
    
    // Command processing
    void processCommandFromHistory();
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const { return commandHistory_; }
    void clearHistory();
    
    // Output formatting
    void setOutputFormat(const std::string& format);
    void setVerboseMode(bool enabled);
    void setColorsEnabled(bool enabled);
    void printPrompt() const;
    
    // Error handling
    void setErrorHandling(bool enabled);
    void setContinueOnError(bool enabled);
    bool hasErrors() const { return hasErrors_; }
    std::string getLastError() const { return lastError_; }
    
    // Logging
    void setLogLevel(int level);
    void log(const std::string& message);
    void logCommand(const std::string& command);
    void logOutput(const std::string& output);
    void logError(const std::string& error);
    
    // Session persistence
    void saveSession(const std::string& filename);
    void loadSession(const std::string& filename);
    void listSessions() const;
    
    // Script mode
    void executeScript(const std::string& filename);
    bool isScriptMode() const { return scriptMode_; }
    void setScriptMode(bool enabled);
    
    // Interactive features
    void enableTabCompletion(bool enabled);
    void setTabCompletionCallback(std::function<std::vector<std::string>(const std::string&)> callback);
    void enableCommandAliases(bool enabled);
    void addCommandAlias(const std::string& alias, const std::string& command);
    void enableCommandMacros(bool enabled);
    void addCommandMacro(const std::string& name, const std::string& macro);
    
    // Macro recording and playback
    void startMacroRecording(const std::string& name);
    void stopMacroRecording();
    void playbackMacro(const std::string& name);
    bool isRecordingMacro() const { return isRecordingMacro_; }
    
    // Real-time interaction
    void setRealTimeMode(bool enabled);
    bool isRealTimeMode() const { return realTimeMode_; }
    void setRefreshRate(int rate) { refreshRate_ = rate; }
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    void printPerformanceStats() const;
    
    // Clipboard integration
    void enableClipboardIntegration(bool enabled);
    void copyToClipboard(const std::string& text);
    void pasteFromClipboard();
    
    // Terminal control
    void clearScreen() const;
    void setTerminalSize(int width, int height);
    void saveTerminalState();
    void restoreTerminalState();

    // Integration with NLM
    void connectToNLM(std::shared_ptr<NLMCLI> cli);
    bool isConnected() const { return cli_ != nullptr; }
    
    // Configuration
    void setConfigFile(const std::string& filename);
    void loadConfiguration();
    void saveConfiguration() const;

protected:
    // Command processing helpers
    std::string expandCommand(const std::string& commandLine) const;
    std::vector<std::string> autocompleteCommand(const std::string& partialCommand) const;
    bool executeMacro(const std::string& macroName) const;
    
    // Output formatting helpers
    std::string formatOutput(const std::string& output) const;
    void printColored(const std::string& text, const std::string& color) const;
    
    // State management helpers
    void saveStateToFile(const std::string& filename) const;
    void loadStateFromFile(const std::string& filename);
    
    // Error handling helpers
    void handleError(const std::exception& e);
    void handleError(const std::string& error);
    
    // Logging helpers
    void writeLogEntry(const std::string& level, const std::string& message);
    void rotateLogFile();

private:
    bool isRunning_;
    std::shared_ptr<NLMCLI> cli_;
    
    // State management
    std::map<std::string, std::string> state_;
    std::mutex stateMutex_;
    
    // Command history
    std::vector<std::string> commandHistory_;
    size_t historySize_;
    size_t historyIndex_;
    
    // Output formatting
    std::string outputFormat_;
    bool verboseMode_;
    bool colorsEnabled_;
    
    // Error handling
    bool errorHandlingEnabled_;
    bool continueOnError_;
    bool hasErrors_;
    std::string lastError_;
    
    // Logging
    int logLevel_;
    std::string logFile_;
    std::ofstream logStream_;
    std::mutex logMutex_;
    
    // Session management
    std::string currentSession_;
    
    // Command aliases and macros
    std::map<std::string, std::string> commandAliases_;
    std::map<std::string, std::string> commandMacros_;
    bool commandAliasesEnabled_;
    bool commandMacrosEnabled_;
    
    // Macro recording
    bool isRecordingMacro_;
    std::string currentMacroName_;
    std::vector<std::string> recordedCommands_;
    
    // Script mode
    bool scriptMode_;
    
    // Real-time mode
    bool realTimeMode_;
    int refreshRate_;
    
    // Performance monitoring
    bool performanceMonitoringEnabled_;
    std::chrono::steady_clock::time_point startTime_;
    int commandCount_;
    double totalExecutionTime_;
    
    // Clipboard integration
    bool clipboardIntegrationEnabled_;
    
    // Terminal control
    bool terminalInitialized_;
    int terminalWidth_;
    int terminalHeight_;
};
