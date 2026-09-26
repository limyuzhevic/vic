#include "DebugConsole.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <SDL.h>
#include <SDL_ttf.h>

namespace nlm {

struct DebugConsole::Impl {
    // Console configuration
    DebugConsoleConfig config;
    
    // Command management
    std::vector<DebugCommand> commands;
    std::vector<std::string> commandHistory;
    size_t historyIndex;
    std::map<std::string, DebugCommand> commandMap;
    
    // Variable and watch management
    std::map<std::string, DebugVariable> variables;
    std::map<std::string, DebugWatch> watches;
    std::mutex variablesMutex;
    std::mutex watchesMutex;
    
    // Break point management
    std::vector<DebugBreakPoint> breakPoints;
    std::mutex breakPointsMutex;
    
    // System state
    SystemStateSnapshot currentState;
    std::vector<SystemStateSnapshot> stateHistory;
    std::mutex stateMutex;
    
    // Performance monitoring
    PerformanceSample currentPerformance;
    std::vector<PerformanceSample> performanceHistory;
    std::mutex performanceMutex;
    bool performanceStatsComputed;
    
    // Error management
    std::vector<ErrorInfo> errors;
    std::mutex errorsMutex;
    
    // Output management
    std::deque<std::string> outputLines;
    std::string currentInput;
    std::string commandBuffer;
    std::mutex outputMutex;
    
    // Rendering
    TTF_Font* font;
    TTF_Font* fontSmall;
    SDL_Texture* textureCache;
    std::map<std::string, SDL_Color> colorMap;
    
    // State tracking
    bool initialized;
    bool active;
    std::chrono::system_clock::time_point lastUpdate;
    uint64_t totalCommandsExecuted;
    double totalCommandExecutionTime;
    
    Impl() : historyIndex(0), performanceStatsComputed(false), initialized(false),
             active(false), totalCommandsExecuted(0), totalCommandExecutionTime(0.0) {
        
        // Initialize color map
        colorMap["text"] = {200, 200, 200, 255};
        colorMap["prompt"] = {100, 200, 100, 255};
        colorMap["error"] = {255, 100, 100, 255};
        colorMap["warning"] = {255, 200, 100, 255};
        colorMap["info"] = {100, 200, 255, 255};
        colorMap["variable"] = {150, 150, 255, 255};
        colorMap["watch"] = {100, 255, 100, 255};
        colorMap["breakpoint"] = {255, 150, 0, 255};
        colorMap["background"] = {30, 30, 30, 255};
        colorMap["border"] = {60, 60, 60, 255};
        colorMap["header"] = {80, 80, 80, 255};
        
        // Setup default commands
        setupDefaultCommands();
    }
    
    void setupDefaultCommands() {
        // Help command
        addCommand(DebugCommand("help", "Show help information", CommandType::Simple, "help [command]",
                              [this](const std::map<std::string, std::string>& args) {
                                  std::string cmd = args.count("command") ? args.at("command") : "";
                                  if (cmd.empty()) {
                                      return getHelpForCategory("general");
                                  } else {
                                      return getCommandHelp(cmd);
                                  }
                              }));
        
        // Clear command
        addCommand(DebugCommand("clear", "Clear console output", CommandType::Simple, "clear",
                              [this](const std::map<std::string, std::string>& args) {
                                  std::lock_guard<std::mutex> lock(outputMutex);
                                  outputLines.clear();
                                  return "Console cleared.";
                              }));
        
        // Status command
        addCommand(DebugCommand("status", "Show system status", CommandType::Function, "status",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getSystemStatus();
                              }));
        
        // Variables command
        addCommand(DebugCommand("vars", "Show all variables", CommandType::Function, "vars",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getVariableInfo();
                              }));
        
        // Watches command
        addCommand(DebugCommand("watches", "Show all watch expressions", CommandType::Function, "watches",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getWatchInfo();
                              }));
        
        // Breakpoints command
        addCommand(DebugCommand("bps", "Show all break points", CommandType::Function, "bps",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getBreakPointInfo();
                              }));
        
        // Performance command
        addCommand(DebugCommand("perf", "Show performance statistics", CommandType::Function, "perf",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getPerformanceInfo();
                              }));
        
        // Errors command
        addCommand(DebugCommand("errors", "Show error history", CommandType::Function, "errors",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getErrorInfo();
                              }));
        
        // State command
        addCommand(DebugCommand("state", "Show system state", CommandType::Function, "state",
                              [this](const std::map<std::string, std::string>& args) {
                                  return getStateInfo();
                              }));
        
        // Export command
        addCommand(DebugCommand("export", "Export debug session", CommandType::Function, "export filename",
                              [this](const std::map<std::string, std::string>& args) {
                                  if (args.count("filename")) {
                                      exportDebugSession(args.at("filename"));
                                      return "Debug session exported to " + args.at("filename");
                                  } else {
                                      return "Error: filename required for export command";
                                  }
                              }));
        
        // Import command
        addCommand(DebugCommand("import", "Import debug session", CommandType::Function, "import filename",
                              [this](const std::map<std::string, std::string>& args) {
                                  if (args.count("filename")) {
                                      importDebugSession(args.at("filename"));
                                      return "Debug session imported from " + args.at("filename");
                                  } else {
                                      return "Error: filename required for import command";
                                  }
                              }));
        
        // Set variable command
        addCommand(DebugCommand("set", "Set variable value", CommandType::Function, "set variable value",
                              [this](const std::map<std::string, std::string>& args) {
                                  if (args.count("variable") && args.count("value")) {
                                      updateVariable(args.at("variable"), args.at("value"));
                                      return "Variable " + args.at("variable") + " set to " + args.at("value");
                                  } else {
                                      return "Error: variable and value required for set command";
                                  }
                              }));
        
        // Watch command
        addCommand(DebugCommand("watch", "Add watch expression", CommandType::Function, "watch expression",
                              [this](const std::map<std::string, std::string>& args) {
                                  if (args.count("expression")) {
                                      addWatch(DebugWatch(args.at("expression")));
                                      return "Watch expression added: " + args.at("expression");
                                  } else {
                                      return "Error: expression required for watch command";
                                  }
                              }));
    }
    
    std::string getHelpForCategory(const std::string& category) const {
        std::stringstream ss;
        ss << "=== NLM Debug Console Help ===" << std::endl;
        ss << "Commands:" << std::endl;
        
        for (const auto& cmd : commands) {
            if (cmd.enabled) {
                ss << "  " << cmd.syntax << " - " << cmd.description << std::endl;
            }
        }
        
        ss << std::endl << "Categories:" << std::endl;
        ss << "  general - Basic console commands" << std::endl;
        ss << "  system  - System state and configuration" << std::endl;
        ss << "  performance - Performance monitoring" << std::endl;
        ss << "  debugging - Debugging and analysis tools" << std::endl;
        
        return ss.str();
    }
    
    std::string getCommandHelp(const std::string& commandName) const {
        if (commandMap.count(commandName)) {
            const auto& cmd = commandMap.at(commandName);
            std::stringstream ss;
            ss << "=== Help: " << cmd.name << " ===" << std::endl;
            ss << cmd.description << std::endl;
            ss << "Syntax: " << cmd.syntax << std::endl;
            ss << "Type: " << static_cast<int>(cmd.type) << std::endl;
            return ss.str();
        }
        return "Error: Command not found: " + commandName;
    }
    
    std::string getSystemStatus() const {
        std::stringstream ss;
        ss << "=== System Status ===" << std::endl;
        ss << "Debug Console: " << (active ? "Active" : "Inactive") << std::endl;
        ss << "Variables: " << variables.size() << std::endl;
        ss << "Watch expressions: " << watches.size() << std::endl;
        ss << "Break points: " << breakPoints.size() << std::endl;
        ss << "Commands executed: " << totalCommandsExecuted << std::endl;
        ss << "Active errors: " << errors.size() << std::endl;
        return ss.str();
    }
    
    std::string getVariableInfo() const {
        std::stringstream ss;
        ss << "=== Variables ===" << std::endl;
        
        if (variables.empty()) {
            ss << "No variables defined." << std::endl;
            return ss.str();
        }
        
        for (const auto& pair : variables) {
            const auto& var = pair.second;
            ss << "  " << var.name << " (" << var.type << ") = " << var.value;
            if (!var.description.empty()) {
                ss << " - " << var.description;
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getWatchInfo() const {
        std::stringstream ss;
        ss << "=== Watch Expressions ===" << std::endl;
        
        if (watches.empty()) {
            ss << "No watch expressions defined." << std::endl;
            return ss.str();
        }
        
        for (const auto& pair : watches) {
            const auto& watch = pair.second;
            ss << "  " << watch.expression;
            if (!watch.description.empty()) {
                ss << " - " << watch.description;
            }
            ss << " = " << watch.result << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getBreakPointInfo() const {
        std::stringstream ss;
        ss << "=== Break Points ===" << std::endl;
        
        if (breakPoints.empty()) {
            ss << "No break points defined." << std::endl;
            return ss.str();
        }
        
        for (const auto& bp : breakPoints) {
            ss << "  " << bp.description << " (hits: " << bp.hitCount << ")";
            ss << " - " << (bp.enabled ? "Enabled" : "Disabled") << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getPerformanceInfo() const {
        std::stringstream ss;
        ss << "=== Performance Statistics ===" << std::endl;
        ss << "Performance samples: " << performanceHistory.size() << std::endl;
        
        if (performanceHistory.empty()) {
            ss << "No performance data available." << std::endl;
            return ss.str();
        }
        
        // Calculate average performance
        double avgUpdateTime = 0.0, avgRenderTime = 0.0, avgTotalTime = 0.0;
        size_t memoryUsage = 0;
        uint32_t totalDrawCalls = 0;
        uint32_t totalVertexCount = 0;
        
        for (const auto& sample : performanceHistory) {
            avgUpdateTime += sample.updateTime.count();
            avgRenderTime += sample.renderTime.count();
            avgTotalTime += sample.totalTime.count();
            memoryUsage += sample.memoryUsage;
            totalDrawCalls += sample.drawCalls;
            totalVertexCount += sample.vertexCount;
        }
        
        size_t count = performanceHistory.size();
        if (count > 0) {
            ss << "Average update time: " << (avgUpdateTime / count) << " μs" << std::endl;
            ss << "Average render time: " << (avgRenderTime / count) << " μs" << std::endl;
            ss << "Average total time: " << (avgTotalTime / count) << " μs" << std::endl;
            ss << "Average memory usage: " << (memoryUsage / count) << " bytes" << std::endl;
            ss << "Average draw calls: " << (totalDrawCalls / count) << std::endl;
            ss << "Average vertex count: " << (totalVertexCount / count) << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getErrorInfo() const {
        std::stringstream ss;
        ss << "=== Error History ===" << std::endl;
        ss << "Total errors: " << errors.size() << std::endl;
        
        if (errors.empty()) {
            ss << "No errors recorded." << std::endl;
            return ss.str();
        }
        
        // Show recent errors
        size_t startIdx = std::max<size_t>(0, errors.size() - 10);
        for (size_t i = startIdx; i < errors.size(); ++i) {
            const auto& error = errors[i];
            ss << "  [" << i - startIdx + 1 << "] " << error.message << " (" << error.severity << ")" << std::endl;
            ss << "     " << error.description << std::endl;
        }
        
        return ss.str();
    }
    
    std::string getStateInfo() const {
        std::stringstream ss;
        ss << "=== System State ===" << std::endl;
        ss << "Current step: " << currentState.step << std::endl;
        ss << "Current time: " << currentState.timestamp << " s" << std::endl;
        ss << "Variables: " << currentState.globalVariables.size() << std::endl;
        ss << "Components: " << currentState.componentStates.size() << std::endl;
        ss << "Active break points: " << currentState.activeBreakPoints.size() << std::endl;
        ss << "Active watches: " << currentState.activeWatches.size() << std::endl;
        ss << "CPU usage: " << currentState.cpuUsage << "%" << std::endl;
        ss << "Memory usage: " << currentState.memoryUsage << " bytes" << std::endl;
        ss << "Total spikes: " << currentState.totalSpikes << std::endl;
        ss << "Average firing rate: " << currentState.averageFiringRate << " Hz" << std::endl;
        return ss.str();
    }
};

DebugConsole::DebugConsole() : pImpl(std::make_unique<Impl>()) {
}

DebugConsole::~DebugConsole() {
    close();
}

bool DebugConsole::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    // Initialize SDL_ttf for text rendering
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Load fonts
    pImpl->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 12);
    pImpl->fontSmall = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 10);
    
    if (!pImpl->font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    pImpl->initialized = true;
    pImpl->active = true;
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    // Add welcome message
    addToOutput(pImpl->config.welcomeMessage, true);
    
    return true;
}

void DebugConsole::update() {
    if (!pImpl->initialized || !pImpl->active) {
        return;
    }
    
    auto currentTime = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pImpl->lastUpdate);
    
    if (elapsed.count() >= static_cast<int>(pImpl->config.updateInterval)) {
        // Update performance monitoring
        updatePerformanceMonitoring();
        
        // Update watch expressions
        updateWatchExpressions();
        
        // Process break points
        processBreakPoints();
        
        pImpl->lastUpdate = currentTime;
    }
    
    updateInputHandling();
    updateOutputBuffer();
}

void DebugConsole::render() {
    if (!pImpl->initialized || !pImpl->active) {
        return;
    }
    
    // Clear with background color
    SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["background"].r,
                          pImpl->colorMap["background"].g, pImpl->colorMap["background"].b,
                          pImpl->colorMap["background"].a);
    SDL_RenderClear(pImpl->renderer);
    
    // Render command input area
    renderCommandInput();
    
    // Render command history
    renderCommandHistory();
    
    // Render variables panel
    renderVariables();
    
    // Render watches panel
    renderWatches();
    
    // Render break points panel
    renderBreakPoints();
    
    // Render performance stats
    renderPerformanceStats();
    
    // Render errors
    renderErrors();
    
    // Render system state
    renderSystemState();
    
    // Render help if needed
    if (pImpl->currentInput.empty()) {
        renderHelp();
    }
    
    SDL_RenderPresent(pImpl->renderer);
}

void DebugConsole::close() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->fontSmall) {
        TTF_CloseFont(pImpl->fontSmall);
        pImpl->fontSmall = nullptr;
    }
    
    if (pImpl->textureCache) {
        SDL_DestroyTexture(pImpl->textureCache);
        pImpl->textureCache = nullptr;
    }
    
    pImpl->initialized = false;
    pImpl->active = false;
}

bool DebugConsole::isActive() const {
    return pImpl->active;
}

void DebugConsole::setUpdateRate(double hz) {
    pImpl->config.updateInterval = 1.0 / hz;
}

double DebugConsole::getUpdateRate() const {
    return 1.0 / pImpl->config.updateInterval;
}

void DebugConsole::processInput(const std::string& input) {
    if (input.empty()) {
        return;
    }
    
    // Add to command history
    pImpl->commandHistory.push_back(input);
    if (pImpl->commandHistory.size() > pImpl->config.commandHistorySize) {
        pImpl->commandHistory.erase(pImpl->commandHistory.begin());
    }
    pImpl->historyIndex = pImpl->commandHistory.size();
    
    // Echo input if configured
    if (pImpl->config.echoCommands) {
        addToOutput("> " + input, false);
    }
    
    // Parse command
    std::string commandName;
    std::map<std::string, std::string> args;
    
    if (parseCommand(input, commandName, args)) {
        if (pImpl->commandMap.count(commandName)) {
            const auto& command = pImpl->commandMap.at(commandName);
            
            if (command.enabled) {
                auto startTime = std::chrono::high_resolution_clock::now();
                std::string result = command.execute(args);
                auto endTime = std::chrono::high_resolution_clock::now();
                
                auto executionTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                pImpl->totalCommandExecutionTime += executionTime.count();
                pImpl->totalCommandsExecuted++;
                
                // Add result to output
                addToOutput(result, true);
                
                // Update performance sample
                PerformanceSample sample;
                sample.step = currentState.step;
                sample.updateTime = executionTime;
                sample.renderTime = std::chrono::microseconds(0);
                sample.totalTime = executionTime;
                sample.memoryUsage = pImpl->currentState.memoryUsage;
                sample.drawCalls = 0;
                sample.vertexCount = 0;
                sample.cpuUtilization = pImpl->currentState.cpuUsage;
                addPerformanceSample(sample);
            } else {
                addToOutput("Error: Command disabled: " + commandName, true);
            }
        } else {
            addToOutput("Error: Unknown command: " + commandName, true);
        }
    } else {
        addToOutput("Error: Invalid command syntax", true);
    }
}

void DebugConsole::executeCommand(const DebugCommand& command, const std::map<std::string, std::string>& args) {
    if (command.enabled) {
        std::string result = command.execute(args);
        addToOutput(result, true);
    }
}

void DebugConsole::addCommand(const DebugCommand& command) {
    pImpl->commands.push_back(command);
    if (command.enabled) {
        pImpl->commandMap[command.name] = command;
    }
}

void DebugConsole::removeCommand(const std::string& name) {
    for (auto it = pImpl->commands.begin(); it != pImpl->commands.end(); ++it) {
        if (it->name == name) {
            pImpl->commands.erase(it);
            break;
        }
    }
    
    pImpl->commandMap.erase(name);
}

void DebugConsole::clearCommands() {
    pImpl->commands.clear();
    pImpl->commandMap.clear();
    setupDefaultCommands();
}

void DebugConsole::addVariable(const DebugVariable& variable) {
    std::lock_guard<std::mutex> lock(pImpl->variablesMutex);
    pImpl->variables[variable.name] = variable;
}

void DebugConsole::updateVariable(const std::string& name, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->variablesMutex);
    if (pImpl->variables.count(name)) {
        pImpl->variables[name].value = value;
        pImpl->variables[name].lastModified = std::chrono::system_clock::now();
    }
}

void DebugConsole::removeVariable(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->variablesMutex);
    pImpl->variables.erase(name);
}

void DebugConsole::addWatch(const DebugWatch& watch) {
    std::lock_guard<std::mutex> lock(pImpl->watchesMutex);
    pImpl->watches[watch.expression] = watch;
}

void DebugConsole::updateWatch(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->watchesMutex);
    if (pImpl->watches.count(expression)) {
        pImpl->watches[expression].lastEvaluated = std::chrono::system_clock::now();
        pImpl->watches[expression].result = evaluateExpression(expression);
    }
}

void DebugConsole::removeWatch(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->watchesMutex);
    pImpl->watches.erase(expression);
}

void DebugConsole::addBreakPoint(const DebugBreakPoint& breakPoint) {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    pImpl->breakPoints.push_back(breakPoint);
}

void DebugConsole::removeBreakPoint(const DebugBreakPoint& breakPoint) {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    for (auto it = pImpl->breakPoints.begin(); it != pImpl->breakPoints.end(); ++it) {
        if (it->condition == breakPoint.condition && it->lineNumber == breakPoint.lineNumber) {
            pImpl->breakPoints.erase(it);
            break;
        }
    }
}

void DebugConsole::enableBreakPoint(const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    for (auto& bp : pImpl->breakPoints) {
        if (bp.condition == condition) {
            bp.enabled = true;
            break;
        }
    }
}

void DebugConsole::disableBreakPoint(const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    for (auto& bp : pImpl->breakPoints) {
        if (bp.condition == condition) {
            bp.enabled = false;
            break;
        }
    }
}

void DebugConsole::clearBreakPoints() {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    pImpl->breakPoints.clear();
}

void DebugConsole::captureSystemState() {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    pImpl->currentState = SystemStateSnapshot();
    pImpl->currentState.globalVariables = pImpl->variables;
    pImpl->currentState.captureTime = std::chrono::system_clock::now();
    
    // Add to history
    pImpl->stateHistory.push_back(pImpl->currentState);
    if (pImpl->stateHistory.size() > 1000) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
    }
}

void DebugConsole::restoreSystemState(const SystemStateSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    // Implementation would restore state from snapshot
}

SystemStateSnapshot DebugConsole::getCurrentSystemState() const {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    return pImpl->currentState;
}

std::vector<SystemStateSnapshot> DebugConsole::getStateHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex);
    return pImpl->stateHistory;
}

void DebugConsole::addPerformanceSample(const PerformanceSample& sample) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    pImpl->performanceHistory.push_back(sample);
    pImpl->currentPerformance = sample;
    
    if (pImpl->performanceHistory.size() > 1000) {
        pImpl->performanceHistory.erase(pImpl->performanceHistory.begin());
    }
    
    pImpl->performanceStatsComputed = false;
}

PerformanceSample DebugConsole::getCurrentPerformanceSample() const {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    return pImpl->currentPerformance;
}

std::vector<PerformanceSample> DebugConsole::getPerformanceHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    return pImpl->performanceHistory;
}

void DebugConsole::computePerformanceStats() {
    if (pImpl->performanceHistory.empty()) {
        pImpl->performanceStatsComputed = true;
        return;
    }
    
    pImpl->performanceStatsComputed = true;
}

std::string DebugConsole::analyzePerformanceBottlenecks() const {
    std::stringstream ss;
    ss << "=== Performance Analysis ===" << std::endl;
    ss << "Performance bottlenecks analysis would go here." << std::endl;
    return ss.str();
}

void DebugConsole::addError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    pImpl->errors.push_back(error);
    addErrorToOutput(error.message);
}

void DebugConsole::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    pImpl->errors.clear();
}

std::vector<ErrorInfo> DebugConsole::getErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    return pImpl->errors;
}

void DebugConsole::reportError(const std::string& message, const std::string& description,
                             const std::string& sourceFile, uint32_t lineNumber, bool critical) {
    ErrorInfo error("err_" + std::to_string(errors.size()), message, description, critical);
    error.sourceFile = sourceFile;
    error.lineNumber = lineNumber;
    addError(error);
}

void DebugConsole::setConfig(const DebugConsoleConfig& config) {
    pImpl->config = config;
}

void DebugConsole::toggleVisibility() {
    pImpl->active = !pImpl->active;
}

void DebugConsole::setCommandHistorySize(size_t size) {
    pImpl->config.commandHistorySize = size;
    if (pImpl->commandHistory.size() > size) {
        pImpl->commandHistory.erase(pImpl->commandHistory.begin(),
                                   pImpl->commandHistory.begin() + (pImpl->commandHistory.size() - size));
    }
}

std::vector<std::string> DebugConsole::completeCommand(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    for (const auto& cmd : commands) {
        if (cmd.enabled && cmd.name.find(partialCommand) == 0) {
            completions.push_back(cmd.name);
        }
    }
    
    return completions;
}

std::vector<std::string> DebugConsole::getAvailableCommands() const {
    std::vector<std::string> cmdNames;
    for (const auto& cmd : commands) {
        if (cmd.enabled) {
            cmdNames.push_back(cmd.name);
        }
    }
    return cmdNames;
}

std::string DebugConsole::getCommandHelp(const std::string& commandName) const {
    return "Help for command: " + commandName;
}

std::vector<std::string> DebugConsole::getCommandCategories() const {
    return {"general", "system", "performance", "debugging"};
}

void DebugConsole::exportDebugSession(const std::string& filename) {
    // Export debug session to file
    // Placeholder implementation
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Debug session exported to " << filename << std::endl;
        file.close();
    }
}

void DebugConsole::importDebugSession(const std::string& filename) {
    // Import debug session from file
    // Placeholder implementation
    std::ifstream file(filename);
    if (file.is_open()) {
        file << "Debug session imported from " << filename << std::endl;
        file.close();
    }
}

size_t DebugConsole::getCommandCount() const {
    return pImpl->commands.size();
}

size_t DebugConsole::getVariableCount() const {
    std::lock_guard<std::mutex> lock(pImpl->variablesMutex);
    return pImpl->variables.size();
}

size_t DebugConsole::getWatchCount() const {
    std::lock_guard<std::mutex> lock(pImpl->watchesMutex);
    return pImpl->watches.size();
}

size_t DebugConsole::getBreakPointCount() const {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    return pImpl->breakPoints.size();
}

size_t DebugConsole::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    return pImpl->errors.size();
}

uint64_t DebugConsole::getTotalCommandsExecuted() const {
    return pImpl->totalCommandsExecuted;
}

double DebugConsole::getAverageCommandExecutionTime() const {
    if (pImpl->totalCommandsExecuted == 0) return 0.0;
    return pImpl->totalCommandExecutionTime / pImpl->totalCommandsExecuted;
}

bool DebugConsole::isCommandHistoryFull() const {
    return pImpl->commandHistory.size() >= pImpl->config.commandHistorySize;
}

std::string DebugConsole::getCommandHistory() const {
    std::stringstream ss;
    for (const auto& cmd : pImpl->commandHistory) {
        ss << cmd << std::endl;
    }
    return ss.str();
}

std::string DebugConsole::getConsoleOutput() const {
    std::lock_guard<std::mutex> lock(pImpl->outputMutex);
    std::stringstream ss;
    for (const auto& line : pImpl->outputLines) {
        ss << line << std::endl;
    }
    return ss.str();
}

// Private helper methods
void DebugConsole::updateInputHandling() {
    // Handle input events
    // Placeholder implementation
}

void DebugConsole::updateOutputBuffer() {
    // Update output buffer
    // Placeholder implementation
}

void DebugConsole::processBreakPoints() {
    std::lock_guard<std::mutex> lock(pImpl->breakPointsMutex);
    // Process break points
    // Placeholder implementation
}

void DebugConsole::updateWatchExpressions() {
    std::lock_guard<std::mutex> lock(pImpl->watchesMutex);
    // Update watch expressions
    // Placeholder implementation
}

void DebugConsole::updatePerformanceMonitoring() {
    // Update performance monitoring
    // Placeholder implementation
}

void DebugConsole::renderCommandInput() {
    // Render command input area
    // Placeholder implementation
}

void DebugConsole::renderCommandHistory() {
    // Render command history
    // Placeholder implementation
}

void DebugConsole::renderVariables() {
    // Render variables panel
    // Placeholder implementation
}

void DebugConsole::renderWatches() {
    // Render watches panel
    // Placeholder implementation
}

void DebugConsole::renderBreakPoints() {
    // Render break points panel
    // Placeholder implementation
}

void DebugConsole::renderPerformanceStats() {
    // Render performance statistics
    // Placeholder implementation
}

void DebugConsole::renderErrors() {
    // Render error messages
    // Placeholder implementation
}

void DebugConsole::renderSystemState() {
    // Render system state
    // Placeholder implementation
}

void DebugConsole::renderHelp() {
    // Render help information
    // Placeholder implementation
}

void DebugConsole::formatTimestamp(std::stringstream& ss, const std::chrono::system_clock::time_point& time) const {
    // Format timestamp
    // Placeholder implementation
}

std::string DebugConsole::evaluateExpression(const std::string& expression) const {
    // Evaluate expression
    return "0.0"; // Placeholder
}

bool DebugConsole::parseCommand(const std::string& input, std::string& commandName,
                               std::map<std::string, std::string>& args) const {
    // Parse command
    return false; // Placeholder
}

void DebugConsole::addToOutput(const std::string& text, bool addNewLine) {
    std::lock_guard<std::mutex> lock(pImpl->outputMutex);
    pImpl->outputLines.push_back(text);
    if (addNewLine) {
        pImpl->outputLines.back() += "\n";
    }
    
    // Limit output size
    if (pImpl->outputLines.size() > 1000) {
        pImpl->outputLines.erase(pImpl->outputLines.begin());
    }
}

void DebugConsole::addErrorToOutput(const std::string& error) {
    addToOutput("ERROR: " + error, true);
}

void DebugConsole::addWarningToOutput(const std::string& warning) {
    addToOutput("WARNING: " + warning, true);
}

} // namespace nlm
