#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations
class WatchVariable;
class Breakpoint;
class CallStackFrame;

// Debug console for interactive debugging
class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();

    // Initialize debug console
    bool initialize();

    // Process input command
    bool processInput(const std::string& command);

    // Get command history
    std::vector<std::string> getCommandHistory() const;

    // Get command completion suggestions
    std::vector<std::string> getCompletions(const std::string& partialCommand) const;

    // Add watch variable
    bool addWatchVariable(const std::string& expression);

    // Remove watch variable
    bool removeWatchVariable(const std::string& expression);

    // Get watch variables
    std::vector<WatchVariable> getWatchVariables() const;

    // Set breakpoint
    bool setBreakpoint(int line, const std::string& file = "");

    // Remove breakpoint
    bool removeBreakpoint(int line, const std::string& file = "");

    // Get breakpoints
    std::vector<Breakpoint> getBreakpoints() const;

    // Evaluate expression
    std::string evaluateExpression(const std::string& expression);

    // Get call stack
    std::vector<CallStackFrame> getCallStack() const;

    // Get current frame
    CallStackFrame getCurrentFrame() const;

    // Set current frame
    void setCurrentFrame(size_t frameIndex);

    // Get variables in current frame
    std::vector<std::string> getVariablesInFrame(size_t frameIndex) const;

    // Get variable value
    std::string getVariableValue(const std::string& varName) const;

    // Set variable value
    bool setVariableValue(const std::string& varName, const std::string& value);

    // Continue execution
    void continueExecution();

    // Step into
    void stepInto();

    // Step over
    void stepOver();

    // Step out
    void stepOut();

    // Get execution state
    enum class ExecutionState {
        Running,
        Paused,
        Stepping,
        Breaking
    };

    ExecutionState getExecutionState() const;

    // Check if debug console is active
    bool isActive() const;

    // Check if there's a breakpoint at given location
    bool hasBreakpoint(int line, const std::string& file = "") const;

    // Get performance statistics
    struct PerformanceStats {
        double cpuUsage;
        size_t memoryUsage;
        size_t allocatedObjects;
        double executionTime;
        size_t instructionCount;
    };

    PerformanceStats getPerformanceStats() const;

    // Clear console
    void clearConsole();

    // Export debug data
    bool exportDebugData(const std::string& filename, const std::string& format = "json");

    // Import debug data
    bool importDebugData(const std::string& filename, const std::string& format = "json");

    // Set brain for debugging
    void setBrain(std::shared_ptr<Brain> brain);

    // Get brain
    std::shared_ptr<Brain> getBrain() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Watch variable structure
struct WatchVariable {
    std::string expression;
    std::string currentValue;
    std::string type;
    std::chrono::steady_clock::time_point lastUpdated;
    bool isExpanded;
    std::vector<WatchVariable> children;

    WatchVariable(const std::string& expr) 
        : expression(expr), currentValue(""), isExpanded(false) {}
};

// Breakpoint structure
struct Breakpoint {
    int line;
    std::string file;
    std::string condition;
    bool enabled;
    std::chrono::steady_clock::time_point setTime;

    Breakpoint(int ln, const std::string& f = "", const std::string& cond = "")
        : line(ln), file(f), condition(cond), enabled(true), setTime(std::chrono::steady_clock::now()) {}
};

// Call stack frame structure
struct CallStackFrame {
    size_t id;
    std::string functionName;
    std::string fileName;
    int lineNumber;
    std::vector<std::string> localVariables;
    std::chrono::steady_clock::time_point timestamp;

    CallStackFrame(size_t frameId, const std::string& func, const std::string& file, int line)
        : id(frameId), functionName(func), fileName(file), lineNumber(line),
          timestamp(std::chrono::steady_clock::now()) {}
};

} // namespace nlm