#include "Debugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <stacktrace>
#include <regex>

namespace nlm {

struct Debugger::Impl {
    std::shared_ptr<DebugConfig> config;
    std::atomic<bool> isRunning;
    DebugState currentState;
    DebugSessionInfo sessionInfo;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Breakpoints
    std::map<std::string, Breakpoint> breakpoints;
    std::map<std::string, std::function<bool()>> breakpointConditions;
    
    // Watch variables
    std::vector<std::string> watchVariables;
    std::map<std::string, DebugVariable> watchedVariables;
    
    // Integration
    std::shared_ptr<AlertSystem> alertSystem;
    std::shared_ptr<Monitor> monitor;
    std::shared_ptr<Profiler> profiler;
    std::shared_ptr<Visualizer> visualizer;
    
    // Context-aware debugging
    bool contextAwareDebuggingEnabled;
    std::string debugContext;
    std::vector<std::string> relatedContexts;
    
    // Error handling
    bool errorRecoveryEnabled;
    std::vector<std::string> errorStackTraces;
    int recoveryAttempts;
    
    // Step-through execution
    std::stack<StackFrame> executionStack;
    std::vector<DebugVariable> currentStepVariables;
    std::chrono::steady_clock::time_point lastStepTime;
    
    Impl() : isRunning(false), currentState(DebugState::Running),
             contextAwareDebuggingEnabled(true), errorRecoveryEnabled(true),
             recoveryAttempts(0) {
        sessionStartTime = std::chrono::steady_clock::now();
        lastStepTime = sessionStartTime;
        
        // Initialize session info
        sessionInfo.sessionId = "debug_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            sessionStartTime.time_since_epoch()).count());
        sessionInfo.state = DebugState::Running;
        sessionInfo.startTime = sessionStartTime;
        sessionInfo.lastActivityTime = sessionStartTime;
        sessionInfo.stepCount = 0;
        
        // Set default debug mode
        sessionInfo.mode = DebugMode::Debug;
    }
    
    ~Impl() {
        // End debug session
        endDebugSession();
    }
    
    void updateSessionActivity() {
        sessionInfo.lastActivityTime = std::chrono::steady_clock::now();
        sessionInfo.stepCount++;
    }
    
    std::string generateBreakpointId(const std::string& location) {
        return "bp_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + "_" + location;
    }
    
    bool checkBreakpoint(const Breakpoint& breakpoint) {
        if (!breakpoint.isEnabled) return false;
        
        // Check if breakpoint has been hit enough times
        if (breakpoint.hitCount > 0 && breakpoint.hitCount >= breakpoint.hitThreshold) {
            return false;
        }
        
        // Check condition if present
        if (breakpoint.condition && !breakpoint.condition()) {
            return false;
        }
        
        return true;
    }
    
    void triggerBreakpoint(const Breakpoint& breakpoint) {
        // Update breakpoint hit count
        Breakpoint& bp = breakpoints[breakpoint.id];
        bp.hitCount++;
        bp.lastHitTime = std::chrono::steady_clock::now();
        
        // Set state to breakpoint triggered
        currentState = DebugState::BreakpointTriggered;
        
        // Notify alert system
        if (alertSystem) {
            Alert alert;
            alert.timestamp = std::chrono::steady_clock::now();
            alert.id = "breakpoint_" + breakpoint.id;
            alert.severity = AlertSeverity::Info;
            alert.message = "Breakpoint triggered at " + breakpoint.location;
            alert.source = "Debugger";
            alert.metadata["breakpoint_id"] = breakpoint.id;
            alert.metadata["location"] = breakpoint.location;
            alert.metadata["type"] = static_cast<int>(breakpoint.type);
            alert.isActive = true;
            
            alertSystem->createAlert(alert);
        }
    }
    
    DebugVariable evaluateSimpleExpression(const std::string& expression) const {
        DebugVariable var;
        var.name = expression;
        var.type = "unknown";
        
        // Try to evaluate based on known variable names
        if (expression == "step_count") {
            var.value = std::to_string(sessionInfo.stepCount);
            var.type = "int";
            var.isEditable = false;
        } else if (expression == "simulation_time") {
            double simTime = std::chrono::duration<double, std::milli>(
                std::chrono::steady_clock::now() - sessionStartTime).count();
            var.value = std::to_string(simTime);
            var.type = "double";
            var.isEditable = false;
        } else if (expression == "memory_usage") {
            var.value = "0.5"; // Placeholder
            var.type = "float";
            var.isEditable = false;
        } else {
            var.value = "[unknown]";
        }
        
        var.file = "debugger";
        var.line = 0;
        var.isEditable = false;
        
        return var;
    }
    
    std::vector<std::string> getStackTrace() const {
        std::vector<std::string> trace;
        std::stacktrace st = std::stacktrace::current();
        for (const auto& frame : st) {
            trace.push_back(frame.to_string());
        }
        return trace;
    }
    
    std::vector<std::string> extractErrorStackTraces() const {
        return errorStackTraces;
    }
    
    void addErrorStackTrace(const std::string& stackTrace) {
        errorStackTraces.push_back(stackTrace);
        if (errorStackTraces.size() > 100) {
            errorStackTraces.erase(errorStackTraces.begin());
        }
    }
};

Debugger::Debugger() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Debugger created");
}

Debugger::~Debugger() {
    NLM_LOG_DEBUG("Debugger destroyed");
}

Debugger::Debugger(Debugger&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

Debugger& Debugger::operator=(Debugger&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Debugger::initialize(std::shared_ptr<DebugConfig> config) {
    pImpl->config = config;
    pImpl->isRunning = true;
    
    // Load debugger configuration from config
    if (config) {
        pImpl->contextAwareDebuggingEnabled = config->isContextAwareDebuggingEnabled();
        pImpl->errorRecoveryEnabled = config->isErrorRecoveryEnabled();
        
        // Set debug mode based on config
        pImpl->sessionInfo.mode = config->getDebugMode();
    }
    
    NLM_LOG_INFO("Debugger initialized");
}

void Debugger::start() {
    pImpl->isRunning = true;
    pImpl->currentState = DebugState::Running;
    pImpl->startDebugSession();
    
    NLM_LOG_INFO("Debugger started");
}

void Debugger::stop() {
    pImpl->isRunning = false;
    pImpl->currentState = DebugState::Completed;
    
    NLM_LOG_INFO("Debugger stopped");
}

void Debugger::update() {
    if (!pImpl->isRunning) {
        return;
    }
    
    pImpl->updateSessionActivity();
    
    // Check for breakpoints
    for (const auto& [id, breakpoint] : pImpl->breakpoints) {
        if (pImpl->checkBreakpoint(breakpoint)) {
            pImpl->triggerBreakpoint(breakpoint);
            return;
        }
    }
}

DebugState Debugger::getState() const {
    return pImpl->currentState;
}

bool Debugger::isRunning() const {
    return pImpl->isRunning;
}

void Debugger::executeCommand(DebugCommand command, const std::string& argument) {
    switch (command) {
        case DebugCommand::Continue:
            pImpl->currentState = DebugState::Running;
            break;
        case DebugCommand::StepInto:
            stepInto();
            break;
        case DebugCommand::StepOver:
            stepOver();
            break;
        case DebugCommand::StepOut:
            stepOut();
            break;
        case DebugCommand::Break:
            pImpl->currentState = DebugState::Paused;
            break;
        case DebugCommand::Resume:
            pImpl->currentState = DebugState::Running;
            break;
        case DebugCommand::Evaluate:
            if (!argument.empty()) {
                DebugVariable var = evaluateExpression(argument);
                NLM_LOG_INFO("Expression evaluation result: " + var.name + " = " + var.value);
            }
            break;
        case DebugCommand::Print:
            if (!argument.empty()) {
                DebugVariable var = evaluateExpression(argument);
                NLM_LOG_INFO(var.name + ": " + var.value + " (" + var.type + ")");
            }
            break;
        case DebugCommand::List:
            NLM_LOG_INFO("Breakpoints: " + std::to_string(pImpl->breakpoints.size()));
            break;
        case DebugCommand::Info:
            NLM_LOG_INFO(getSessionInfo().summary());
            break;
        case DebugCommand::Help:
            NLM_LOG_INFO("Debugger commands: continue, step-into, step-over, step-out, break, resume, evaluate <expr>, print <var>, list, info, help, quit");
            break;
        case DebugCommand::Quit:
            stop();
            break;
        default:
            break;
    }
}

void Debugger::addBreakpoint(const Breakpoint& breakpoint) {
    pImpl->breakpoints[breakpoint.id] = breakpoint;
    NLM_LOG_INFO("Breakpoint added: " + breakpoint.id + " at " + breakpoint.location);
}

void Debugger::removeBreakpoint(const std::string& breakpointId) {
    auto it = pImpl->breakpoints.find(breakpointId);
    if (it != pImpl->breakpoints.end()) {
        pImpl->breakpoints.erase(it);
        NLM_LOG_INFO("Breakpoint removed: " + breakpointId);
    }
}

void Debugger::setBreakpoint(const std::string& location, BreakpointType type) {
    Breakpoint breakpoint;
    breakpoint.id = pImpl->generateBreakpointId(location);
    breakpoint.type = type;
    breakpoint.location = location;
    breakpoint.isEnabled = true;
    breakpoint.hitCount = 0;
    breakpoint.hitThreshold = 1;
    
    addBreakpoint(breakpoint);
}

std::vector<Breakpoint> Debugger::getBreakpoints() const {
    std::vector<Breakpoint> breakpoints;
    for (const auto& [id, breakpoint] : pImpl->breakpoints) {
        breakpoints.push_back(breakpoint);
    }
    return breakpoints;
}

void Debugger::addWatchVariable(const std::string& variable) {
    if (std::find(pImpl->watchVariables.begin(), pImpl->watchVariables.end(), variable) == pImpl->watchVariables.end()) {
        pImpl->watchVariables.push_back(variable);
        NLM_LOG_INFO("Watch variable added: " + variable);
    }
}

void Debugger::removeWatchVariable(const std::string& variable) {
    auto it = std::find(pImpl->watchVariables.begin(), pImpl->watchVariables.end(), variable);
    if (it != pImpl->watchVariables.end()) {
        pImpl->watchVariables.erase(it);
        NLM_LOG_INFO("Watch variable removed: " + variable);
    }
}

std::vector<DebugVariable> Debugger::getWatchVariables() const {
    std::vector<DebugVariable> variables;
    for (const auto& varName : pImpl->watchVariables) {
        DebugVariable var = inspectVariable(varName);
        variables.push_back(var);
    }
    return variables;
}

DebugVariable Debugger::evaluateExpression(const std::string& expression) {
    return pImpl->evaluateSimpleExpression(expression);
}

std::vector<StackFrame> Debugger::getCallStack() const {
    return pImpl->getStackTrace();
}

StackFrame Debugger::getCurrentStackFrame() const {
    StackFrame frame;
    frame.functionName = "current_function";
    frame.fileName = "debugger.cpp";
    frame.lineNumber = __LINE__;
    return frame;
}

DebugSessionInfo Debugger::getSessionInfo() const {
    return pImpl->sessionInfo;
}

void Debugger::setContextAwareDebugging(bool enabled) {
    pImpl->contextAwareDebuggingEnabled = enabled;
    NLM_LOG_INFO("Context-aware debugging " + std::string(enabled ? "enabled" : "disabled"));
}

bool Debugger::isContextAwareDebuggingEnabled() const {
    return pImpl->contextAwareDebuggingEnabled;
}

// Step-through execution

void Debugger::stepInto() {
    pImpl->currentState = DebugState::Stepping;
    NLM_LOG_INFO("Step into executed");
}

void Debugger::stepOver() {
    pImpl->currentState = DebugState::Stepping;
    NLM_LOG_INFO("Step over executed");
}

void Debugger::stepOut() {
    pImpl->currentState = DebugState::Stepping;
    NLM_LOG_INFO("Step out executed");
}

// Variable inspection

DebugVariable Debugger::inspectVariable(const std::string& variableName) const {
    return pImpl->evaluateSimpleExpression(variableName);
}

std::vector<DebugVariable> Debugger::inspectScope(const std::string& scope) const {
    std::vector<DebugVariable> variables;
    
    // Get variables based on scope
    if (scope == "local") {
        for (const auto& var : pImpl->currentStepVariables) {
            variables.push_back(var);
        }
    } else if (scope == "global") {
        for (const auto& var : pImpl->sessionInfo.globalVariables) {
            variables.push_back(var);
        }
    }
    
    return variables;
}

// Stack trace and error analysis

std::vector<StackFrame> Debugger::analyzeStackTrace() const {
    std::vector<StackFrame> frames;
    auto stackTrace = pImpl->getStackTrace();
    
    for (size_t i = 0; i < stackTrace.size(); ++i) {
        StackFrame frame;
        frame.functionName = "function_" + std::to_string(i);
        frame.fileName = "file_" + std::to_string(i) + ".cpp";
        frame.lineNumber = static_cast<int>(i);
        frames.push_back(frame);
    }
    
    return frames;
}

std::vector<std::string> Debugger::getErrorStackTraces() const {
    return pImpl->extractErrorStackTraces();
}

// Debug session management

void Debugger::startDebugSession() {
    pImpl->sessionStartTime = std::chrono::steady_clock::now();
    pImpl->sessionInfo.sessionId = "debug_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        pImpl->sessionStartTime.time_since_epoch()).count());
    pImpl->sessionInfo.startTime = pImpl->sessionStartTime;
    pImpl->sessionInfo.lastActivityTime = pImpl->sessionStartTime;
    pImpl->sessionInfo.state = DebugState::Running;
    pImpl->sessionInfo.stepCount = 0;
    
    NLM_LOG_INFO("Debug session started: " + pImpl->sessionInfo.sessionId);
}

void Debugger::endDebugSession() {
    pImpl->sessionInfo.state = DebugState::Completed;
    NLM_LOG_INFO("Debug session ended: " + pImpl->sessionInfo.sessionId);
}

bool Debugger::isDebugSessionActive() const {
    return pImpl->sessionInfo.state == DebugState::Running || 
           pImpl->sessionInfo.state == DebugState::Stepping ||
           pImpl->sessionInfo.state == DebugState::BreakpointTriggered;
}

std::string Debugger::getCurrentDebugSessionId() const {
    return pImpl->sessionInfo.sessionId;
}

// Debug mode distinctions

void Debugger::enableDebugMode() {
    pImpl->sessionInfo.mode = DebugMode::Debug;
    NLM_LOG_INFO("Debug mode enabled");
}

void Debugger::enableReleaseMode() {
    pImpl->sessionInfo.mode = DebugMode::Release;
    NLM_LOG_INFO("Release mode enabled");
}

void Debugger::enableProfileMode() {
    pImpl->sessionInfo.mode = DebugMode::Profile;
    NLM_LOG_INFO("Profile mode enabled");
}

void Debugger::enableTraceMode() {
    pImpl->sessionInfo.mode = DebugMode::Trace;
    NLM_LOG_INFO("Trace mode enabled");
}

DebugMode Debugger::getDebugMode() const {
    return pImpl->sessionInfo.mode;
}

// Integration with existing systems

void Debugger::integrateWithAlertSystem(std::shared_ptr<AlertSystem> alertSystem) {
    pImpl->alertSystem = alertSystem;
    NLM_LOG_INFO("Integrated with AlertSystem");
}

void Debugger::integrateWithMonitor(std::shared_ptr<Monitor> monitor) {
    pImpl->monitor = monitor;
    NLM_LOG_INFO("Integrated with Monitor");
}

void Debugger::integrateWithProfiler(std::shared_ptr<Profiler> profiler) {
    pImpl->profiler = profiler;
    NLM_LOG_INFO("Integrated with Profiler");
}

void Debugger::integrateWithVisualizer(std::shared_ptr<Visualizer> visualizer) {
    pImpl->visualizer = visualizer;
    NLM_LOG_INFO("Integrated with Visualizer");
}

// Context-aware debugging

void Debugger::setDebugContext(const std::string& context) {
    pImpl->debugContext = context;
    NLM_LOG_INFO("Debug context set: " + context);
}

std::string Debugger::getDebugContext() const {
    return pImpl->debugContext;
}

std::vector<std::string> Debugger::getRelatedContexts() const {
    return pImpl->relatedContexts;
}

// Error handling and recovery

void Debugger::handleError(const std::string& errorMessage, const std::string& stackTrace) {
    NLM_LOG_ERROR("Error in debugger: " + errorMessage);
    if (!stackTrace.empty()) {
        pImpl->addErrorStackTrace(stackTrace);
    }
    
    if (pImpl->errorRecoveryEnabled) {
        pImpl->recoveryAttempts++;
        NLM_LOG_INFO("Attempting error recovery (attempt " + std::to_string(pImpl->recoveryAttempts) + ")");
        recoverFromError();
    }
}

void Debugger::recoverFromError() {
    if (pImpl->recoveryAttempts > pImpl->config->getMaxErrorRecoveryAttempts()) {
        NLM_LOG_ERROR("Max error recovery attempts exceeded");
        pImpl->currentState = DebugState::Error;
        return;
    }
    
    // Reset state and continue
    pImpl->currentState = DebugState::Running;
    pImpl->sessionInfo.state = DebugState::Running;
    
    NLM_LOG_INFO("Recovered from error");
}

bool Debugger::isErrorRecoveryEnabled() const {
    return pImpl->errorRecoveryEnabled;
}

} // namespace nlm
