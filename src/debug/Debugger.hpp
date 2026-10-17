#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <stack>
#include <variant>
#include "../core/Logger/Logger.hpp"
#include "DebugConfig.hpp"
#include "AlertSystem.hpp"
#include "Monitor.hpp"
#include "Profiler.hpp"
#include "Visualizer.hpp"

namespace nlm {

// Variable type for debugging
struct DebugVariable {
    std::string name;
    std::string type;
    std::string value;
    std::string file;
    int line;
    bool isEditable;
};

// Stack frame for debugging
struct StackFrame {
    std::string functionName;
    std::string fileName;
    int lineNumber;
    std::vector<DebugVariable> localVariables;
    std::vector<DebugVariable> parameters;
    std::string instructionPointer;
};

// Debug command
enum class DebugCommand {
    Continue,
    StepInto,
    StepOver,
    StepOut,
    Break,
    Resume,
    Evaluate,
    Print,
    List,
    Info,
    Help,
    Quit
};

// Debug session state
enum class DebugState {
    Running,
    Paused,
    Stepping,
    BreakpointTriggered,
    Error,
    Completed
};

// Debug session information
struct DebugSessionInfo {
    std::string sessionId;
    DebugMode mode;
    DebugState state;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastActivityTime;
    std::vector<std::string> breakpoints;
    std::vector<std::string> watchVariables;
    std::map<std::string, std::string> registers;
    std::stack<StackFrame> callStack;
    std::vector<DebugVariable> globalVariables;
    std::vector<DebugVariable> currentVariables;
};

// Debugger interface
class IDebugger {
public:
    virtual ~IDebugger() = default;
    virtual void initialize(std::shared_ptr<DebugConfig> config) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual DebugState getState() const = 0;
    virtual bool isRunning() const = 0;
    virtual void executeCommand(DebugCommand command, const std::string& argument = "") = 0;
    virtual void addBreakpoint(const Breakpoint& breakpoint) = 0;
    virtual void removeBreakpoint(const std::string& breakpointId) = 0;
    virtual void setBreakpoint(const std::string& location, BreakpointType type = BreakpointType::Instruction) = 0;
    virtual std::vector<Breakpoint> getBreakpoints() const = 0;
    virtual void addWatchVariable(const std::string& variable) = 0;
    virtual void removeWatchVariable(const std::string& variable) = 0;
    virtual std::vector<DebugVariable> getWatchVariables() const = 0;
    virtual DebugVariable evaluateExpression(const std::string& expression) = 0;
    virtual std::vector<StackFrame> getCallStack() const = 0;
    virtual StackFrame getCurrentStackFrame() const = 0;
    virtual DebugSessionInfo getSessionInfo() const = 0;
    virtual void setContextAwareDebugging(bool enabled) = 0;
    virtual bool isContextAwareDebuggingEnabled() const = 0;
};

// Debugger implementation
class Debugger : public IDebugger {
public:
    Debugger();
    ~Debugger() override;
    
    // Disable copying, enable moving
    Debugger(const Debugger&) = delete;
    Debugger& operator=(const Debugger&) = delete;
    Debugger(Debugger&&) noexcept;
    Debugger& operator=(Debugger&&) noexcept;
    
    void initialize(std::shared_ptr<DebugConfig> config) override;
    void start() override;
    void stop() override;
    void update() override;
    DebugState getState() const override;
    bool isRunning() const override;
    void executeCommand(DebugCommand command, const std::string& argument = "") override;
    void addBreakpoint(const Breakpoint& breakpoint) override;
    void removeBreakpoint(const std::string& breakpointId) override;
    void setBreakpoint(const std::string& location, BreakpointType type = BreakpointType::Instruction) override;
    std::vector<Breakpoint> getBreakpoints() const override;
    void addWatchVariable(const std::string& variable) override;
    void removeWatchVariable(const std::string& variable) override;
    std::vector<DebugVariable> getWatchVariables() const override;
    DebugVariable evaluateExpression(const std::string& expression) override;
    std::vector<StackFrame> getCallStack() const override;
    StackFrame getCurrentStackFrame() const override;
    DebugSessionInfo getSessionInfo() const override;
    void setContextAwareDebugging(bool enabled) override;
    bool isContextAwareDebuggingEnabled() const override;
    
    // Step-through execution
    void stepInto();
    void stepOver();
    void stepOut();
    
    // Variable inspection
    DebugVariable inspectVariable(const std::string& variableName) const;
    std::vector<DebugVariable> inspectScope(const std::string& scope) const;
    
    // Stack trace and error analysis
    std::vector<StackFrame> analyzeStackTrace() const;
    std::vector<std::string> getErrorStackTraces() const;
    
    // Debug session management
    void startDebugSession();
    void endDebugSession();
    bool isDebugSessionActive() const;
    std::string getCurrentDebugSessionId() const;
    
    // Debug mode distinctions
    void enableDebugMode();
    void enableReleaseMode();
    void enableProfileMode();
    void enableTraceMode();
    DebugMode getDebugMode() const;
    
    // Integration with existing systems
    void integrateWithAlertSystem(std::shared_ptr<AlertSystem> alertSystem);
    void integrateWithMonitor(std::shared_ptr<Monitor> monitor);
    void integrateWithProfiler(std::shared_ptr<Profiler> profiler);
    void integrateWithVisualizer(std::shared_ptr<Visualizer> visualizer);
    
    // Context-aware debugging
    void setDebugContext(const std::string& context);
    std::string getDebugContext() const;
    std::vector<std::string> getRelatedContexts() const;
    
    // Error handling and recovery
    void handleError(const std::string& errorMessage, const std::string& stackTrace = "");
    void recoverFromError();
    bool isErrorRecoveryEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
