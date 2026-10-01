#include "DebugConsole.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace nlm {

struct DebugConsole::Impl {
    bool active;
    std::vector<std::string> commandHistory;
    size_t historyIndex;
    std::vector<WatchVariable> watchVariables;
    std::vector<Breakpoint> breakpoints;
    std::vector<CallStackFrame> callStack;
    size_t currentFrameIndex;
    ExecutionState executionState;
    std::shared_ptr<Brain> debugBrain;
    std::unordered_map<std::string, std::string> variables;
    PerformanceStats perfStats;
    std::chrono::steady_clock::time_point lastUpdateTime;
    bool autoCompleteEnabled;
    std::string inputBuffer;
    size_t cursorPosition;
    
    Impl() : active(false), historyIndex(0), currentFrameIndex(0), 
             executionState(ExecutionState::Running), autoCompleteEnabled(true),
             cursorPosition(0), lastUpdateTime(std::chrono::steady_clock::now()) {}
};

DebugConsole::DebugConsole() : pImpl(std::make_unique<Impl>()) {}

DebugConsole::~DebugConsole() = default;

bool DebugConsole::initialize() {
    // Initialize debug console
    pImpl->active = true;
    NLM_LOG_INFO("DebugConsole: Initialized successfully");
    
    // Initialize with some default commands
    pImpl->commandHistory.push_back("help");
    pImpl->commandHistory.push_back("status");
    pImpl->commandHistory.push_back("watch getTotalNeuronCount()");
    
    return true;
}

bool DebugConsole::processInput(const std::string& command) {
    if (!pImpl->active) return false;
    
    // Parse and execute command
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (cmd == "help") {
        // Print help information
        NLM_LOG_INFO("Debug Console Commands:");
        NLM_LOG_INFO("  help - Show this help message");
        NLM_LOG_INFO("  status - Show current execution status");
        NLM_LOG_INFO("  watch <expr> - Add watch variable");
        NLM_LOG_INFO("  break <line> - Set breakpoint");
        NLM_LOG_INFO("  continue - Continue execution");
        NLM_LOG_INFO("  step - Step to next instruction");
        NLM_LOG_INFO("  next - Step over current function");
        NLM_LOG_INFO("  back - Step out of current function");
        NLM_LOG_INFO("  stack - Show call stack");
        NLM_LOG_INFO("  frame <n> - Switch to frame n");
        NLM_LOG_INFO("  eval <expr> - Evaluate expression");
        NLM_LOG_INFO("  var <name> <value> - Set variable");
        NLM_LOG_INFO("  clear - Clear console");
        return true;
    }
    else if (cmd == "status") {
        NLM_LOG_INFO("DebugConsole Status:");
        NLM_LOG_INFO("  Execution State: " + executionStateToString(pImpl->executionState));
        NLM_LOG_INFO("  Watch Variables: " + std::to_string(pImpl->watchVariables.size()));
        NLM_LOG_INFO("  Breakpoints: " + std::to_string(pImpl->breakpoints.size()));
        NLM_LOG_INFO("  Command History Size: " + std::to_string(pImpl->commandHistory.size()));
        return true;
    }
    else if (cmd.substr(0, 5) == "watch ") {
        std::string expr = cmd.substr(6);
        addWatchVariable(expr);
        return true;
    }
    else if (cmd.substr(0, 6) == "break ") {
        try {
            int line = std::stoi(cmd.substr(7));
            setBreakpoint(line);
        } catch (...) {
            NLM_LOG_WARNING("DebugConsole: Invalid breakpoint command");
        }
        return true;
    }
    else if (cmd == "continue") {
        continueExecution();
        return true;
    }
    else if (cmd == "step" || cmd == "step into") {
        stepInto();
        return true;
    }
    else if (cmd == "next" || cmd == "step over") {
        stepOver();
        return true;
    }
    else if (cmd == "back" || cmd == "step out") {
        stepOut();
        return true;
    }
    else if (cmd == "stack" || cmd == "stack trace") {
        std::vector<CallStackFrame> stack = getCallStack();
        for (const auto& frame : stack) {
            NLM_LOG_INFO("  Frame " + std::to_string(frame.id) + ": " + frame.functionName + " at " + frame.fileName + ":" + std::to_string(frame.lineNumber));
        }
        return true;
    }
    else if (cmd.substr(0, 6) == "frame ") {
        try {
            size_t frame = std::stoul(cmd.substr(7));
            if (frame < pImpl->callStack.size()) {
                setCurrentFrame(frame);
            }
        } catch (...) {
            NLM_LOG_WARNING("DebugConsole: Invalid frame command");
        }
        return true;
    }
    else if (cmd.substr(0, 5) == "eval ") {
        std::string expr = cmd.substr(6);
        std::string result = evaluateExpression(expr);
        NLM_LOG_INFO("Result: " + result);
        return true;
    }
    else if (cmd.substr(0, 4) == "var ") {
        // var name value
        size_t space1 = cmd.find(' ', 5);
        size_t space2 = cmd.find(' ', space1 + 1);
        if (space1 != std::string::npos && space2 != std::string::npos) {
            std::string varName = cmd.substr(5, space1 - 5);
            std::string value = cmd.substr(space1 + 1);
            setVariableValue(varName, value);
        }
        return true;
    }
    else if (cmd == "clear") {
        clearConsole();
        return true;
    }
    
    // Unknown command
    NLM_LOG_WARNING("DebugConsole: Unknown command '" + command + "'");
    pImpl->commandHistory.push_back(command);
    return false;
}

std::vector<std::string> DebugConsole::getCommandHistory() const {
    return pImpl->commandHistory;
}

std::vector<std::string> DebugConsole::getCompletions(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    // Basic completions
    std::vector<std::string> commands = {"help", "status", "watch", "break", "continue", "step", "next", "back", "stack", "clear", "eval", "var"};
    
    for (const auto& cmd : commands) {
        if (cmd.find(partialCommand) == 0) {
            completions.push_back(cmd);
        }
    }
    
    // Variable completions if brain is available
    if (pImpl->debugBrain) {
        // Get variables from current frame
        std::vector<std::string> vars = getVariablesInFrame(pImpl->currentFrameIndex);
        for (const auto& var : vars) {
            if (var.find(partialCommand) == 0) {
                completions.push_back(var);
            }
        }
    }
    
    return completions;
}

bool DebugConsole::addWatchVariable(const std::string& expression) {
    // Check if already exists
    for (const auto& watchVar : pImpl->watchVariables) {
        if (watchVar.expression == expression) {
            NLM_LOG_WARNING("DebugConsole: Watch variable '" + expression + "' already exists");
            return false;
        }
    }
    
    pImpl->watchVariables.push_back(WatchVariable(expression));
    NLM_LOG_INFO("DebugConsole: Added watch variable '" + expression + "'");
    return true;
}

bool DebugConsole::removeWatchVariable(const std::string& expression) {
    auto it = std::find_if(pImpl->watchVariables.begin(), pImpl->watchVariables.end(),
                          [&expression](const WatchVariable& wv) { return wv.expression == expression; });
    if (it != pImpl->watchVariables.end()) {
        pImpl->watchVariables.erase(it);
        NLM_LOG_INFO("DebugConsole: Removed watch variable '" + expression + "'");
        return true;
    }
    return false;
}

std::vector<WatchVariable> DebugConsole::getWatchVariables() const {
    return pImpl->watchVariables;
}

bool DebugConsole::setBreakpoint(int line, const std::string& file) {
    // Check if breakpoint already exists
    for (const auto& bp : pImpl->breakpoints) {
        if (bp.line == line && bp.file == file) {
            NLM_LOG_WARNING("DebugConsole: Breakpoint already exists at " + file + ":" + std::to_string(line));
            return false;
        }
    }
    
    pImpl->breakpoints.emplace_back(line, file);
    NLM_LOG_INFO("DebugConsole: Set breakpoint at " + file + ":" + std::to_string(line));
    return true;
}

bool DebugConsole::removeBreakpoint(int line, const std::string& file) {
    auto it = std::find_if(pImpl->breakpoints.begin(), pImpl->breakpoints.end(),
                          [&line, &file](const Breakpoint& bp) { 
                              return bp.line == line && bp.file == file; 
                          });
    if (it != pImpl->breakpoints.end()) {
        pImpl->breakpoints.erase(it);
        NLM_LOG_INFO("DebugConsole: Removed breakpoint at " + file + ":" + std::to_string(line));
        return true;
    }
    return false;
}

std::vector<Breakpoint> DebugConsole::getBreakpoints() const {
    return pImpl->breakpoints;
}

std::string DebugConsole::evaluateExpression(const std::string& expression) {
    // Simple expression evaluation - in real implementation would be more sophisticated
    if (expression == "getTotalNeuronCount()" && pImpl->debugBrain) {
        return std::to_string(pImpl->debugBrain->getTotalNeuronCount());
    }
    else if (expression == "getTotalSpikeCount()" && pImpl->debugBrain) {
        return std::to_string(pImpl->debugBrain->getTotalSpikeCount());
    }
    else if (expression == "getAverageFiringRate()" && pImpl->debugBrain) {
        return std::to_string(pImpl->debugBrain->getAverageFiringRate());
    }
    else {
        return "Cannot evaluate expression: " + expression;
    }
}

std::vector<CallStackFrame> DebugConsole::getCallStack() const {
    return pImpl->callStack;
}

CallStackFrame DebugConsole::getCurrentFrame() const {
    if (pImpl->callStack.empty()) {
        return CallStackFrame(0, "main", "main.cpp", 0);
    }
    return pImpl->callStack[pImpl->currentFrameIndex];
}

void DebugConsole::setCurrentFrame(size_t frameIndex) {
    if (frameIndex < pImpl->callStack.size()) {
        pImpl->currentFrameIndex = frameIndex;
        NLM_LOG_INFO("DebugConsole: Switched to frame " + std::to_string(frameIndex));
    }
}

std::vector<std::string> DebugConsole::getVariablesInFrame(size_t frameIndex) const {
    // Mock variables - in real implementation would query actual variables
    if (pImpl->debugBrain) {
        return {"totalNeuronCount", "totalSpikeCount", "activeNeuronCount", "firingNeuronCount"};
    }
    return {};
}

std::string DebugConsole::getVariableValue(const std::string& varName) const {
    auto it = pImpl->variables.find(varName);
    return (it != pImpl->variables.end()) ? it->second : "Variable not found: " + varName;
}

bool DebugConsole::setVariableValue(const std::string& varName, const std::string& value) {
    pImpl->variables[varName] = value;
    NLM_LOG_INFO("DebugConsole: Set variable '" + varName + "' to '" + value + "'");
    return true;
}

void DebugConsole::continueExecution() {
    pImpl->executionState = ExecutionState::Running;
    NLM_LOG_INFO("DebugConsole: Execution continued");
}

void DebugConsole::stepInto() {
    pImpl->executionState = ExecutionState::Stepping;
    NLM_LOG_INFO("DebugConsole: Stepping into next instruction");
}

void DebugConsole::stepOver() {
    pImpl->executionState = ExecutionState::Stepping;
    NLM_LOG_INFO("DebugConsole: Stepping over current function");
}

void DebugConsole::stepOut() {
    pImpl->executionState = ExecutionState::Stepping;
    NLM_LOG_INFO("DebugConsole: Stepping out of current function");
}

DebugConsole::ExecutionState DebugConsole::getExecutionState() const {
    return pImpl->executionState;
}

bool DebugConsole::isActive() const {
    return pImpl->active;
}

bool DebugConsole::hasBreakpoint(int line, const std::string& file) const {
    for (const auto& bp : pImpl->breakpoints) {
        if (bp.line == line && bp.file == file) {
            return true;
        }
    }
    return false;
}

DebugConsole::PerformanceStats DebugConsole::getPerformanceStats() const {
    return pImpl->perfStats;
}

void DebugConsole::clearConsole() {
    // In a real implementation, this would clear the console buffer
    NLM_LOG_INFO("DebugConsole: Console cleared");
}

bool DebugConsole::exportDebugData(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("DebugConsole: Exporting debug data to " + filename + "." + format);
    // Export implementation would go here
    return true;
}

bool DebugConsole::importDebugData(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("DebugConsole: Importing debug data from " + filename + "." + format);
    // Import implementation would go here
    return true;
}

void DebugConsole::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->debugBrain = brain;
    NLM_LOG_INFO("DebugConsole: Debug brain set");
}

std::shared_ptr<Brain> DebugConsole::getBrain() const {
    return pImpl->debugBrain;
}

std::string DebugConsole::executionStateToString(ExecutionState state) const {
    switch (state) {
        case ExecutionState::Running: return "Running";
        case ExecutionState::Paused: return "Paused";
        case ExecutionState::Stepping: return "Stepping";
        case ExecutionState::Breaking: return "Breaking";
        default: return "Unknown";
    }
}

} // namespace nlm