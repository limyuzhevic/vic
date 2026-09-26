#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <deque>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

// Debug command types
enum class CommandType {
    Simple,
    Function,
    Variable,
    Expression,
    Script,
    Custom
};

// Debug command structure
struct DebugCommand {
    std::string name;
    std::string description;
    CommandType type;
    std::string syntax;
    std::function<std::string(const std::map<std::string, std::string>&)> execute;
    bool enabled;
    
    DebugCommand(const std::string& cmdName, const std::string& cmdDesc, 
                 CommandType cmdType, const std::string& cmdSyntax,
                 std::function<std::string(const std::map<std::string, std::string>&)> cmdExecute)
        : name(cmdName), description(cmdDesc), type(cmdType), syntax(cmdSyntax),
          execute(cmdExecute), enabled(true) {}
};

// Debug variable for inspection
struct DebugVariable {
    std::string name;
    std::string type;
    std::string value;
    std::string description;
    std::chrono::system_clock::time_point lastModified;
    bool isWatchVariable;
    std::string format;
    
    DebugVariable(const std::string& varName, const std::string& varType, 
                  const std::string& varValue, const std::string& varDesc = "")
        : name(varName), type(varType), value(varValue), description(varDesc),
          isWatchVariable(false), format("%s") {
        lastModified = std::chrono::system_clock::now();
    }
};

// Debug watch expression
struct DebugWatch {
    std::string expression;
    std::string description;
    std::string result;
    std::chrono::system_clock::time_point lastEvaluated;
    bool autoUpdate;
    double updateInterval;
    
    DebugWatch(const std::string& expr, const std::string& desc = "",
               bool autoUpd = true, double interval = 1.0)
        : expression(expr), description(desc), autoUpdate(autoUpd),
          updateInterval(interval), lastEvaluated(std::chrono::system_clock::now()) {
        if (desc.empty()) {
            description = "Watch: " + expr;
        }
    }
};

// Debug break point
struct DebugBreakPoint {
    std::string condition;           // Break point condition
    uint32_t lineNumber;              // Line number in source code
    std::string functionName;         // Function name
    bool enabled;                     // Whether break point is active
    mutable uint32_t hitCount;        // How many times break point has been hit
    std::string description;          // Description
    
    DebugBreakPoint(const std::string& cond, uint32_t line, const std::string& func,
                    const std::string& desc = "")
        : condition(cond), lineNumber(line), functionName(func), enabled(true),
          hitCount(0), description(desc.empty() ? "Break: " + cond : desc) {}
};

// System state snapshot
struct SystemStateSnapshot {
    SimulationStep step;
    Timestamp timestamp;
    std::chrono::system_clock::time_point captureTime;
    std::map<std::string, std::string> globalVariables;
    std::map<std::string, std::vector<std::string>> componentStates;
    std::vector<std::string> activeBreakPoints;
    std::vector<DebugWatch> activeWatches;
    float cpuUsage;
    size_t memoryUsage;
    uint32_t totalSpikes;
    float averageFiringRate;
    
    SystemStateSnapshot() : step(0), timestamp(0.0), cpuUsage(0.0f), memoryUsage(0),
                           totalSpikes(0), averageFiringRate(0.0f) {
        captureTime = std::chrono::system_clock::now();
    }
};

// Performance sample for profiling
struct PerformanceSample {
    SimulationStep step;
    std::chrono::microseconds updateTime;
    std::chrono::microseconds renderTime;
    std::chrono::microseconds totalTime;
    size_t memoryUsage;
    uint32_t drawCalls;
    uint32_t vertexCount;
    float cpuUtilization;
    float gpuUtilization;
    std::map<std::string, double> componentTimes;
    
    PerformanceSample() : step(0), updateTime(0), renderTime(0), totalTime(0),
                         memoryUsage(0), drawCalls(0), vertexCount(0),
                         cpuUtilization(0.0f), gpuUtilization(0.0f) {}
};

// Error information
struct ErrorInfo {
    std::string id;
    std::string message;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string sourceFile;
    uint32_t lineNumber;
    std::string stackTrace;
    bool isCritical;
    std::string severity;  // "ERROR", "WARNING", "INFO"
    std::map<std::string, std::string> context;
    
    ErrorInfo(const std::string& errId, const std::string& errMsg, 
              const std::string& errDesc, bool critical = false)
        : id(errId), message(errMsg), description(errDesc), timestamp(std::chrono::system_clock::now()),
          lineNumber(0), isCritical(critical), severity(critical ? "ERROR" : "WARNING") {}
};

// DebugConsole configuration
struct DebugConsoleConfig {
    bool enabled;                    // Whether debug console is enabled
    bool visible;                    // Whether debug console is visible
    double updateInterval;           // Update interval in seconds
    size_t commandHistorySize;       // Size of command history
    bool echoCommands;               // Whether to echo commands
    bool autoComplete;               // Whether to enable auto-completion
    bool showTimestamps;             // Whether to show timestamps
    bool showCommandDetails;         // Whether to show command details
    std::string prompt;              // Command prompt
    std::string title;               // Console title
    std::string theme;               // Color theme
    bool allowScripting;             // Whether to allow scripting
    bool allowFileAccess;            // Whether to allow file access
    bool enableBreakPoints;          // Whether to enable break points
    bool enableWatchExpressions;     // Whether to enable watch expressions
    std::string welcomeMessage;      // Welcome message
    
    DebugConsoleConfig() : enabled(true), visible(true), updateInterval(0.1),
                          commandHistorySize(100), echoCommands(true),
                          autoComplete(true), showTimestamps(true), showCommandDetails(true),
                          prompt("> "), title("NLM Debug Console"), theme("default"),
                          allowScripting(true), allowFileAccess(false),
                          enableBreakPoints(true), enableWatchExpressions(true),
                          welcomeMessage("NLM Neural Learning Machine Debug Console") {}
};

// DebugConsole class
class DebugConsole : public VisualizationInterface {
public:
    DebugConsole();
    ~DebugConsole();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Debug console methods
    void processInput(const std::string& input);
    void executeCommand(const DebugCommand& command, const std::map<std::string, std::string>& args);
    void addCommand(const DebugCommand& command);
    void removeCommand(const std::string& name);
    void clearCommands();
    
    // Variable and watch management
    void addVariable(const DebugVariable& variable);
    void updateVariable(const std::string& name, const std::string& value);
    void removeVariable(const std::string& name);
    void addWatch(const DebugWatch& watch);
    void updateWatch(const std::string& expression);
    void removeWatch(const std::string& expression);
    
    // Break point management
    void addBreakPoint(const DebugBreakPoint& breakPoint);
    void removeBreakPoint(const DebugBreakPoint& breakPoint);
    void enableBreakPoint(const std::string& condition);
    void disableBreakPoint(const std::string& condition);
    void clearBreakPoints();
    
    // System state management
    void captureSystemState();
    void restoreSystemState(const SystemStateSnapshot& snapshot);
    SystemStateSnapshot getCurrentSystemState() const;
    std::vector<SystemStateSnapshot> getStateHistory() const;
    
    // Performance monitoring
    void addPerformanceSample(const PerformanceSample& sample);
    PerformanceSample getCurrentPerformanceSample() const;
    std::vector<PerformanceSample> getPerformanceHistory() const;
    void computePerformanceStats();
    std::string analyzePerformanceBottlenecks() const;
    
    // Error management
    void addError(const ErrorInfo& error);
    void clearErrors();
    std::vector<ErrorInfo> getErrors() const;
    void reportError(const std::string& message, const std::string& description = "",
                   const std::string& sourceFile = "", uint32_t lineNumber = 0,
                   bool critical = false);
    
    // Configuration
    void setConfig(const DebugConsoleConfig& config);
    void toggleVisibility();
    void setCommandHistorySize(size_t size);
    
    // Command completion and help
    std::vector<std::string> completeCommand(const std::string& partialCommand) const;
    std::vector<std::string> getAvailableCommands() const;
    std::string getCommandHelp(const std::string& commandName) const;
    std::vector<std::string> getCommandCategories() const;
    
    // Export and import
    void exportDebugSession(const std::string& filename);
    void importDebugSession(const std::string& filename);
    
    // Statistics
    size_t getCommandCount() const;
    size_t getVariableCount() const;
    size_t getWatchCount() const;
    size_t getBreakPointCount() const;
    size_t getErrorCount() const;
    uint64_t getTotalCommandsExecuted() const;
    double getAverageCommandExecutionTime() const;
    
    // Status
    bool isCommandHistoryFull() const;
    std::string getCommandHistory() const;
    std::string getConsoleOutput() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateInputHandling();
    void updateOutputBuffer();
    void processBreakPoints();
    void updateWatchExpressions();
    void updatePerformanceMonitoring();
    void renderCommandInput();
    void renderCommandHistory();
    void renderVariables();
    void renderWatches();
    void renderBreakPoints();
    void renderPerformanceStats();
    void renderErrors();
    void renderSystemState();
    void renderHelp();
    void formatTimestamp(std::stringstream& ss, const std::chrono::system_clock::time_point& time) const;
    std::string evaluateExpression(const std::string& expression) const;
    bool parseCommand(const std::string& input, std::string& commandName, 
                     std::map<std::string, std::string>& args) const;
    void addToOutput(const std::string& text, bool addNewLine = true);
    void addErrorToOutput(const std::string& error);
    void addWarningToOutput(const std::string& warning);
};

} // namespace nlm
