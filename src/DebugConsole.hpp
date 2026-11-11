#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <map>
#include <algorithm>

namespace nlm {

// Debug command types
enum class DebugCommand {
    STATUS, CONFIG, NEURONS, SYNAPSES, MEMORY, PLASITY, SYSTEM, HELP, QUIT, EXIT, UNKNOWN
};

// Debug console state
struct DebugConsoleState {
    bool isRunning;
    std::string prompt;
    std::chrono::steady_clock::time_point startTime;
    uint64_t commandCount;
    std::unordered_map<std::string, std::string> variables;
    bool showTimestamps;
    std::string logLevel;
    bool autoComplete;
    std::vector<std::string> commandHistory;
    size_t historyIndex;
};

class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();
    
    // Initialize console
    static void initialize();
    static void shutdown();
    
    // Main console loop
    static void startInteractiveSession(CoreBrainController& controller);
    static void runInteractiveSession(CoreBrainController& controller);
    
    // Command processing
    static bool processCommand(const std::string& input, CoreBrainController& controller);
    static DebugCommand parseCommand(const std::string& input);
    
    // Command handlers
    static void handleStatusCommand(CoreBrainController& controller);
    static void handleConfigCommand(CoreBrainController& controller);
    static void handleNeuronsCommand(CoreBrainController& controller);
    static void handleSynapsesCommand(CoreBrainController& controller);
    static void handleMemoryCommand(CoreBrainController& controller);
    static void handlePlacityCommand(CoreBrainController& controller);
    static void handleSystemCommand(CoreBrainController& controller);
    static void handleHelpCommand();
    static void handleQuitCommand();
    static void handleExitCommand();
    
    // Console utilities
    static void setPrompt(const std::string& prompt);
    static void toggleTimestamps();
    static void setLogLevel(const std::string& level);
    static void toggleAutoComplete();
    static std::vector<std::string> getCommandCompletions(const std::string& partial);
    
    // Output formatting
    static void printBanner();
    static void printSystemInfo(CoreBrainController& controller);
    static void printSeparator();
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    
    // State management
    static DebugConsoleState getState() { return currentState; }
    static bool isRunning() { return currentState.isRunning; }
    
private:
    static DebugConsoleState currentState;
    static std::atomic<bool> consoleRunning;
    
    // Command parsing helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool stringStartsWith(const std::string& str, const std::string& prefix);
    static std::string getCurrentTimestamp();
    static void clearInputBuffer();
    static void setupConsole();
    static void restoreConsole();
    
    // Command validation
    static bool isValidCommand(const std::string& cmd);
    static std::vector<std::string> getAvailableCommands();
    static std::vector<std::string> getAvailableVariables(CoreBrainController& controller);
    
    // Output formatting helpers
    static void printColored(const std::string& text, const std::string& color = "white");
    static void printSuccess(const std::string& text);
    static void printInfo(const std::string& text);
    static void printDebug(const std::string& text);
    static void printWarning(const std::string& text);
    static void printError(const std::string& text);
    
    // Console input/output
    static std::string readLine();
    static void writeLine(const std::string& line);
    static void writeError(const std::string& error);
};

} // namespace nlm