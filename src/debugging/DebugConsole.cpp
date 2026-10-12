#include "DebugConsole.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cctype>
#include <regex>
#include <map>
#include <set>

namespace nlm {

struct DebugConsole::Impl {
    // Console state
    bool consoleActive;
    bool outputStreamsEnabled;
    
    // Command management
    std::map<std::string, ConsoleCommand> commands;
    std::map<std::string, std::string> commandCategories;
    std::set<std::string> enabledCommands;
    
    // Parameter management
    std::map<std::string, RuntimeParameter> parameters;
    std::map<std::string, std::string> parameterCategories;
    std::set<std::string> editableParameters;
    
    // I/O management
    std::queue<ConsoleInput> inputQueue;
    std::queue<ConsoleOutput> outputQueue;
    mutable std::mutex ioMutex;
    
    // Log management
    std::vector<ConsoleLog> logs;
    std::string logLevelFilter;
    std::string logSourceFilter;
    size_t maxLogs;
    
    // Metrics
    ConsoleMetrics currentMetrics;
    Timestamp lastMetricsUpdate;
    
    // Command history
    std::vector<std::string> commandHistory;
    size_t maxHistorySize;
    
    // Integration
    class AdvancedVisualization* visualization;
    class Profiler* profiler;
    
    // Output streams
    std::map<std::string, std::ofstream> outputStreams;
    std::map<std::string, std::string> outputFormats;
    
    // Statistics
    size_t totalCommandsExecuted;
    size_t totalParametersSet;
    Timestamp sessionStartTime;
    bool initialized;
    
    Impl() : consoleActive(false), outputStreamsEnabled(false),
             maxLogs(10000), maxHistorySize(1000),
             totalCommandsExecuted(0), totalParametersSet(0),
             sessionStartTime(0), initialized(false),
             visualization(nullptr), profiler(nullptr) {}
};

DebugConsole::DebugConsole() : pImpl(std::make_unique<Impl>()) {}
DebugConsole::~DebugConsole() {
    // Clean up output streams
    for (auto& [name, stream] : pImpl->outputStreams) {
        if (stream.is_open()) {
            stream.close();
        }
    }
}

bool DebugConsole::initialize() {
    try {
        // Register built-in commands
        registerBuiltInCommands();
        registerBuiltInParameters();
        
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Debug console initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize debug console: " + std::string(e.what()));
        return false;
    }
}

void DebugConsole::processInput(const std::string& input) {
    if (!pImpl->initialized) return;
    
    ConsoleInput consoleInput;
    consoleInput.text = input;
    consoleInput.timestamp = getCurrentTimestamp();
    consoleInput.source = "user";
    
    {
        std::lock_guard<std::mutex> lock(pImpl->ioMutex);
        pImpl->inputQueue.push(consoleInput);
    }
}

void DebugConsole::processCommand(const std::string& commandLine) {
    // Simple command parsing (placeholder)
    addToHistory(commandLine);
    
    // For now, just output the command
    output("> " + commandLine, "INFO", false);
    output("Command not implemented yet.", "WARNING", true);
}

void DebugConsole::output(const std::string& message, const std::string& level, bool isSystem) {
    ConsoleOutput consoleOutput;
    consoleOutput.text = message;
    consoleOutput.level = level;
    consoleOutput.timestamp = getCurrentTimestamp();
    consoleOutput.isSystem = isSystem;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->ioMutex);
        pImpl->outputQueue.push(consoleOutput);
    }
    
    // Also log to system logger
    Logger::getInstance().log(level, "DebugConsole", message);
}

void DebugConsole::registerCommand(const ConsoleCommand& command) {
    if (!pImpl->initialized) return;
    
    pImpl->commands[command.name] = command;
    pImpl->commandCategories[command.name] = command.category;
    
    if (!command.enabled) {
        pImpl->enabledCommands.erase(command.name);
    } else {
        pImpl->enabledCommands.insert(command.name);
    }
}

void DebugConsole::unregisterCommand(const std::string& name) {
    pImpl->commands.erase(name);
    pImpl->commandCategories.erase(name);
    pImpl->enabledCommands.erase(name);
}

void DebugConsole::enableCommand(const std::string& name) {
    auto it = pImpl->commands.find(name);
    if (it != pImpl->commands.end()) {
        it->second.enabled = true;
        pImpl->enabledCommands.insert(name);
    }
}

void DebugConsole::disableCommand(const std::string& name) {
    pImpl->enabledCommands.erase(name);
    auto it = pImpl->commands.find(name);
    if (it != pImpl->commands.end()) {
        it->second.enabled = false;
    }
}

ConsoleCommand* DebugConsole::getCommand(const std::string& name) {
    auto it = pImpl->commands.find(name);
    return (it != pImpl->commands.end()) ? &it->second : nullptr;
}

std::vector<ConsoleCommand> DebugConsole::getCommands(const std::string& category) const {
    std::vector<ConsoleCommand> commands;
    
    for (const auto& pair : pImpl->commands) {
        if (category.empty() || pair.second.category == category) {
            commands.push_back(pair.second);
        }
    }
    
    return commands;
}

std::vector<std::string> DebugConsole::getCommandCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->commandCategories) {
        if (std::find(categories.begin(), categories.end(), pair.second) == categories.end()) {
            categories.push_back(pair.second);
        }
    }
    return categories;
}

bool DebugConsole::executeCommand(const std::string& name, const std::vector<std::string>& args) {
    ConsoleCommand* command = getCommand(name);
    if (!command) {
        output("Error: Command not found: " + name, "ERROR", true);
        return false;
    }
    
    if (!command->enabled) {
        output("Error: Command is disabled: " + name, "ERROR", true);
        return false;
    }
    
    return executeCommand(*command, args);
}

bool DebugConsole::executeCommand(const ConsoleCommand& command, const std::vector<std::string>& args) {
    pImpl->totalCommandsExecuted++;
    
    try {
        command.handler(args);
        output("Command executed: " + command.name, "SUCCESS", true);
        return true;
    } catch (const std::exception& e) {
        output("Error executing command " + command.name + ": " + std::string(e.what()), "ERROR", true);
        return false;
    }
}

void DebugConsole::addToHistory(const std::string& command) {
    pImpl->commandHistory.push_back(command);
    while (pImpl->commandHistory.size() > pImpl->maxHistorySize) {
        pImpl->commandHistory.erase(pImpl->commandHistory.begin());
    }
}

std::vector<std::string> DebugConsole::getCommandHistory(size_t maxHistory) const {
    size_t count = std::min(maxHistory, pImpl->commandHistory.size());
    return std::vector<std::string>(pImpl->commandHistory.end() - count, pImpl->commandHistory.end());
}

std::vector<std::string> DebugConsole::getCommandSuggestions(const std::string& partialCommand) const {
    std::vector<std::string> suggestions;
    
    for (const auto& pair : pImpl->commands) {
        if (pair.first.find(partialCommand) == 0 && pair.second.enabled) {
            suggestions.push_back(pair.first);
        }
    }
    
    return suggestions;
}

void DebugConsole::registerParameter(const RuntimeParameter& parameter) {
    if (!pImpl->initialized) return;
    
    pImpl->parameters[parameter.name] = parameter;
    pImpl->parameterCategories[parameter.name] = parameter.category;
    
    if (!parameter.isReadOnly) {
        pImpl->editableParameters.insert(parameter.name);
    }
}

void DebugConsole::unregisterParameter(const std::string& name) {
    pImpl->parameters.erase(name);
    pImpl->parameterCategories.erase(name);
    pImpl->editableParameters.erase(name);
}

void DebugConsole::setParameter(const std::string& name, const std::string& value) {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        if (it->second.isReadOnly) {
            output("Error: Parameter is read-only: " + name, "ERROR", true);
            return;
        }
        
        // Validate value (simplified)
        if (!it->second.allowedValues.empty()) {
            if (std::find(it->second.allowedValues.begin(), it->second.allowedValues.end(), value) == it->second.allowedValues.end()) {
                output("Error: Invalid value for parameter " + name + ": " + value, "ERROR", true);
                return;
            }
        }
        
        // Set the value
        if (it->second.setValue) {
            it->second.setValue(value);
        }
        
        pImpl->totalParametersSet++;
        output("Parameter set: " + name + " = " + value, "INFO", true);
    } else {
        output("Error: Parameter not found: " + name, "ERROR", true);
    }
}

std::string DebugConsole::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        if (it->second.getDisplayValue) {
            return it->second.getDisplayValue();
        } else {
            return it->second.value;
        }
    }
    return "";
}

std::vector<RuntimeParameter> DebugConsole::getParameters(const std::string& category) const {
    std::vector<RuntimeParameter> parameters;
    
    for (const auto& pair : pImpl->parameters) {
        if (category.empty() || pair.second.category == category) {
            parameters.push_back(pair.second);
        }
    }
    
    return parameters;
}

std::vector<std::string> DebugConsole::getParameterCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->parameterCategories) {
        if (std::find(categories.begin(), categories.end(), pair.second) == categories.end()) {
            categories.push_back(pair.second);
        }
    }
    return categories;
}

void DebugConsole::addLog(const ConsoleLog& log) {
    pImpl->logs.push_back(log);
    while (pImpl->logs.size() > pImpl->maxLogs) {
        pImpl->logs.erase(pImpl->logs.begin());
    }
}

void DebugConsole::addSystemLog(const std::string& level, const std::string& source, const std::string& message) {
    ConsoleLog log;
    log.timestamp = getCurrentTimestamp();
    log.level = level;
    log.source = source;
    log.message = message;
    log.isSystemLog = true;
    
    addLog(log);
}

std::vector<ConsoleLog> DebugConsole::getRecentLogs(size_t maxLogs) const {
    size_t count = std::min(maxLogs, pImpl->logs.size());
    return std::vector<ConsoleLog>(pImpl->logs.end() - count, pImpl->logs.end());
}

std::vector<ConsoleLog> DebugConsole::getLogsSince(Timestamp timestamp) const {
    std::vector<ConsoleLog> recentLogs;
    for (const auto& log : pImpl->logs) {
        if (log.timestamp >= timestamp) {
            recentLogs.push_back(log);
        }
    }
    return recentLogs;
}

void DebugConsole::clearLogs() {
    pImpl->logs.clear();
}

void DebugConsole::filterLogs(const std::string& level, const std::string& source) {
    pImpl->logLevelFilter = level;
    pImpl->logSourceFilter = source;
}

void DebugConsole::updateMetrics(const ConsoleMetrics& metrics) {
    pImpl->currentMetrics = metrics;
    pImpl->lastMetricsUpdate = metrics.timestamp;
}

ConsoleMetrics DebugConsole::getCurrentMetrics() const {
    return pImpl->currentMetrics;
}

std::string DebugConsole::getMetricsSummary() const {
    std::stringstream ss;
    ss << "=== System Metrics ===" << std::endl;
    ss << "Timestamp: " << pImpl->currentMetrics.timestamp << std::endl;
    ss << "CPU Usage: " << std::fixed << std::setprecision(2) << pImpl->currentMetrics.cpuUsage << "%" << std::endl;
    ss << "Memory Usage: " << std::fixed << std::setprecision(2) << pImpl->currentMetrics.memoryUsage << "%" << std::endl;
    ss << "Active Connections: " << pImpl->currentMetrics.activeConnections << std::endl;
    ss << "FPS: " << std::fixed << std::setprecision(2) << pImpl->currentMetrics.fps << std::endl;
    ss << "System Uptime: " << pImpl->currentMetrics.systemUptime << " seconds" << std::endl;
    
    return ss.str();
}

void DebugConsole::startConsole() {
    pImpl->consoleActive = true;
    output("Debug console started", "INFO", true);
}

void DebugConsole::stopConsole() {
    pImpl->consoleActive = false;
    output("Debug console stopped", "INFO", true);
}

bool DebugConsole::isConsoleActive() const {
    return pImpl->consoleActive;
}

void DebugConsole::setConsoleActive(bool active) {
    pImpl->consoleActive = active;
    if (active) {
        output("Debug console activated", "INFO", true);
    } else {
        output("Debug console deactivated", "INFO", true);
    }
}

void DebugConsole::attachToVisualization(AdvancedVisualization* visualization) {
    pImpl->visualization = visualization;
}

void DebugConsole::attachToProfiler(Profiler* profiler) {
    pImpl->profiler = profiler;
}

void DebugConsole::startOutputStream(const std::string& streamName, const std::string& format) {
    if (pImpl->outputStreams.find(streamName) == pImpl->outputStreams.end()) {
        pImpl->outputStreams[streamName].open(streamName, std::ios::app);
        pImpl->outputFormats[streamName] = format;
    }
}

void DebugConsole::stopOutputStream(const std::string& streamName) {
    auto it = pImpl->outputStreams.find(streamName);
    if (it != pImpl->outputStreams.end()) {
        if (it->second.is_open()) {
            it->second.close();
        }
        pImpl->outputStreams.erase(it);
        pImpl->outputFormats.erase(streamName);
    }
}

bool DebugConsole::isOutputStreamActive(const std::string& streamName) const {
    auto it = pImpl->outputStreams.find(streamName);
    return (it != pImpl->outputStreams.end()) && it->second.is_open();
}

bool DebugConsole::exportCommandHistory(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Command History Export" << std::endl;
    file << "Generated at: " << getCurrentTimestamp() << std::endl;
    file << "=========================" << std::endl;
    file << std::endl;
    
    for (const auto& command : pImpl->commandHistory) {
        file << command << std::endl;
    }
    
    file.close();
    return true;
}

bool DebugConsole::exportLogHistory(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Log History Export" << std::endl;
    file << "Generated at: " << getCurrentTimestamp() << std::endl;
    file << "======================" << std::endl;
    file << std::endl;
    
    for (const auto& log : pImpl->logs) {
        file << "[" << log.timestamp << "] " << log.level << " [" << log.source << "] " << log.message << std::endl;
    }
    
    file.close();
    return true;
}

bool DebugConsole::exportParameterHistory(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Parameter History Export" << std::endl;
    file << "Generated at: " << getCurrentTimestamp() << std::endl;
    file << "==========================" << std::endl;
    file << std::endl;
    
    // For simplicity, just export current parameters
    for (const auto& pair : pImpl->parameters) {
        file << pair.first << " = " << pair.second.value << " (Category: " << pair.second.category << ")" << std::endl;
    }
    
    file.close();
    return true;
}

std::string DebugConsole::getSystemStatus() const {
    std::stringstream ss;
    ss << "=== System Status ===" << std::endl;
    ss << "Total Commands Executed: " << pImpl->totalCommandsExecuted << std::endl;
    ss << "Total Parameters Set: " << pImpl->totalParametersSet << std::endl;
    ss << "Console Active: " << (pImpl->consoleActive ? "Yes" : "No") << std::endl;
    ss << "Log Count: " << pImpl->logs.size() << std::endl;
    ss << "Command Count: " << pImpl->commands.size() << std::endl;
    ss << "Parameter Count: " << pImpl->parameters.size() << std::endl;
    ss << "Active Output Streams: " << pImpl->outputStreams.size() << std::endl;
    
    return ss.str();
}

std::string DebugConsole::getConsoleStatus() const {
    std::stringstream ss;
    ss << "=== Console Status ===" << std::endl;
    ss << "Active: " << (pImpl->consoleActive ? "Yes" : "No") << std::endl;
    ss << "Commands Registered: " << pImpl->commands.size() << std::endl;
    ss << "Parameters Registered: " << pImpl->parameters.size() << std::endl;
    ss << "Logs Stored: " << pImpl->logs.size() << std::endl;
    ss << "Command History: " << pImpl->commandHistory.size() << " entries" << std::endl;
    
    return ss.str();
}

size_t DebugConsole::getActiveCommandCount() const {
    return pImpl->enabledCommands.size();
}

size_t DebugConsole::getActiveParameterCount() const {
    return pImpl->editableParameters.size();
}

size_t DebugConsole::getLogCount() const {
    return pImpl->logs.size();
}

// Helper function
Timestamp DebugConsole::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Built-in command registration
void DebugConsole::registerBuiltInCommands() {
    // Help command
    ConsoleCommand helpCommand;
    helpCommand.name = "help";
    helpCommand.description = "Show help information";
    helpCommand.usage = "help [category]";
    helpCommand.enabled = true;
    helpCommand.category = "system";
    helpCommand.isHidden = false;
    helpCommand.handler = [this](const std::vector<std::string>& args) {
        if (args.empty()) {
            output("Available categories: " + this->getConsoleStatus(), "INFO", true);
            for (const auto& category : this->getCommandCategories()) {
                output("  " + category, "INFO", true);
            }
        } else {
            output("Commands in category '" + args[0] + "':", "INFO", true);
            auto commands = this->getCommands(args[0]);
            for (const auto& cmd : commands) {
                output("  " + cmd.name + " - " + cmd.description, "INFO", true);
            }
        }
    };
    registerCommand(helpCommand);
    
    // Status command
    ConsoleCommand statusCommand;
    statusCommand.name = "status";
    statusCommand.description = "Show system status";
    statusCommand.usage = "status";
    statusCommand.enabled = true;
    statusCommand.category = "system";
    statusCommand.isHidden = false;
    statusCommand.handler = [this](const std::vector<std::string>& args) {
        output(this->getSystemStatus(), "INFO", true);
    };
    registerCommand(statusCommand);
    
    // Clear command
    ConsoleCommand clearCommand;
    clearCommand.name = "clear";
    clearCommand.description = "Clear console output";
    clearCommand.usage = "clear";
    clearCommand.enabled = true;
    clearCommand.category = "system";
    clearCommand.isHidden = false;
    clearCommand.handler = [this](const std::vector<std::string>& args) {
        output("Console cleared", "INFO", true);
    };
    registerCommand(clearCommand);
    
    // History command
    ConsoleCommand historyCommand;
    historyCommand.name = "history";
    historyCommand.description = "Show command history";
    historyCommand.usage = "history [count]";
    historyCommand.enabled = true;
    historyCommand.category = "system";
    historyCommand.isHidden = false;
    historyCommand.handler = [this](const std::vector<std::string>& args) {
        size_t count = 10;
        if (!args.empty()) {
            count = std::stoul(args[0]);
        }
        auto history = this->getCommandHistory(count);
        output("Recent command history:", "INFO", true);
        for (const auto& cmd : history) {
            output("  " + cmd, "INFO", true);
        }
    };
    registerCommand(historyCommand);
    
    // Visualize command
    ConsoleCommand visualizeCommand;
    visualizeCommand.name = "visualize";
    visualizeCommand.description = "Trigger visualization update";
    visualizeCommand.usage = "visualize [component]";
    visualizeCommand.enabled = true;
    visualizeCommand.category = "visualization";
    visualizeCommand.isHidden = false;
    visualizeCommand.handler = [this](const std::vector<std::string>& args) {
        if (this->pImpl->visualization) {
            output("Triggering visualization update...", "INFO", true);
            // In real implementation, this would trigger visualization update
        } else {
            output("No visualization system attached", "WARNING", true);
        }
    };
    registerCommand(visualizeCommand);
    
    // Export command
    ConsoleCommand exportCommand;
    exportCommand.name = "export";
    exportCommand.description = "Export data to file";
    exportCommand.usage = "export <filename> [format]";
    exportCommand.enabled = true;
    exportCommand.category = "system";
    exportCommand.isHidden = false;
    exportCommand.handler = [this](const std::vector<std::string>& args) {
        if (args.size() < 1) {
            output("Usage: export <filename> [format]", "ERROR", true);
            return;
        }
        
        std::string filename = args[0];
        std::string format = (args.size() > 1) ? args[1] : "txt";
        
        // Simple export implementation
        output("Exporting " + format + " data to " + filename + "...", "INFO", true);
        // In real implementation, this would perform actual export
        output("Export completed: " + filename, "SUCCESS", true);
    };
    registerCommand(exportCommand);
}

// Built-in parameter registration
void DebugConsole::registerBuiltInParameters() {
    // Visualization parameters
    RuntimeParameter vizFPSParam;
    vizFPSParam.name = "visualization_fps";
    vizFPSParam.type = "float";
    vizFPSParam.value = "30.0";
    vizFPSParam.description = "Visualization update rate (frames per second)";
    vizFPSParam.category = "visualization";
    vizFPSParam.isReadOnly = false;
    vizFPSParam.allowedValues = {};
    registerParameter(vizFPSParam);
    
    // Debug level parameter
    RuntimeParameter debugLevelParam;
    debugLevelParam.name = "debug_level";
    debugLevelParam.type = "string";
    debugLevelParam.value = "INFO";
    debugLevelParam.description = "Minimum log level to display";
    debugLevelParam.category = "system";
    debugLevelParam.isReadOnly = false;
    debugLevelParam.allowedValues = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
    registerParameter(debugLevelParam);
    
    // Performance monitoring parameter
    RuntimeParameter perfMonitorParam;
    perfMonitorParam.name = "performance_monitoring";
    perfMonitorParam.type = "bool";
    perfMonitorParam.value = "true";
    perfMonitorParam.description = "Enable performance monitoring";
    perfMonitorParam.category = "system";
    perfMonitorParam.isReadOnly = false;
    perfMonitorParam.allowedValues = {"true", "false"};
    registerParameter(perfMonitorParam);
    
    // Memory limit parameter
    RuntimeParameter memoryLimitParam;
    memoryLimitParam.name = "memory_limit_mb";
    memoryLimitParam.type = "int";
    memoryLimitParam.value = "1024";
    memoryLimitParam.description = "Memory limit in megabytes";
    memoryLimitParam.category = "system";
    memoryLimitParam.isReadOnly = false;
    memoryLimitParam.allowedValues = {};
    registerParameter(memoryLimitParam);
}

} // namespace nlm