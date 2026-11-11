#include "DebugConsole.hpp"
#include "CoreBrainController.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <sstream>

namespace nlm {

DebugConsoleState DebugConsole::currentState = {};
std::atomic<bool> DebugConsole::consoleRunning = false;

DebugConsole::DebugConsole() {
    setupConsole();
}

DebugConsole::~DebugConsole() {
    restoreConsole();
}

void DebugConsole::initialize() {
    currentState = {
        isRunning: false,
        prompt: ">>> ",
        startTime: std::chrono::steady_clock::now(),
        commandCount: 0,
        showTimestamps: true,
        logLevel: "INFO",
        autoComplete: true,
        historyIndex: 0
    };
}

void DebugConsole::shutdown() {
    consoleRunning = false;
    if (isRunning()) {
        std::cout << "Debug console shutting down..." << std::endl;
        currentState.isRunning = false;
    }
}

void DebugConsole::setupConsole() {
    // Windows console setup would go here
    // For now, just set basic console properties
    std::cout << std::fixed << std::setprecision(3);
}

void DebugConsole::restoreConsole() {
    // Restore console settings
}

void DebugConsole::startInteractiveSession(CoreBrainController& controller) {
    if (isRunning()) {
        printError("Debug console is already running");
        return;
    }
    
    consoleRunning = true;
    currentState.isRunning = true;
    currentState.startTime = std::chrono::steady_clock::now();
    
    printBanner();
    printSystemInfo(controller);
    
    runInteractiveSession(controller);
}

void DebugConsole::runInteractiveSession(CoreBrainController& controller) {
    std::string input;
    
    while (isRunning() && consoleRunning) {
        try {
            input = readLine();
            if (input.empty()) continue;
            
            currentState.commandHistory.push_back(input);
            currentState.historyIndex = currentState.commandHistory.size();
            currentState.commandCount++;
            
            processCommand(input, controller);
            
        } catch (const std::exception& e) {
            printError("Error: " + std::string(e.what()));
        }
    }
}

bool DebugConsole::processCommand(const std::string& input, CoreBrainController& controller) {
    std::string cmd = trim(input);
    
    if (cmd.empty()) {
        return true;
    }
    
    DebugCommand command = parseCommand(cmd);
    
    switch (command) {
        case DebugCommand::STATUS:
            handleStatusCommand(controller);
            break;
        case DebugCommand::CONFIG:
            handleConfigCommand(controller);
            break;
        case DebugCommand::NEURONS:
            handleNeuronsCommand(controller);
            break;
        case DebugCommand::SYNAPSES:
            handleSynapsesCommand(controller);
            break;
        case DebugCommand::MEMORY:
            handleMemoryCommand(controller);
            break;
        case DebugCommand::PLASITY:
            handlePlacityCommand(controller);
            break;
        case DebugCommand::SYSTEM:
            handleSystemCommand(controller);
            break;
        case DebugCommand::HELP:
            handleHelpCommand();
            break;
        case DebugCommand::QUIT:
            handleQuitCommand();
            break;
        case DebugCommand::EXIT:
            handleExitCommand();
            break;
        default:
            printError("Unknown command: '" + cmd + "'. Type 'help' for available commands.");
            break;
    }
    
    return true;
}

DebugCommand DebugConsole::parseCommand(const std::string& input) {
    std::string cmd = trim(input);
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    // Remove comments
    size_t commentPos = cmd.find('#');
    if (commentPos != std::string::npos) {
        cmd = cmd.substr(0, commentPos);
        cmd = trim(cmd);
    }
    
    if (cmd.empty()) {
        return DebugCommand::UNKNOWN;
    }
    
    // Parse command and arguments
    std::vector<std::string> parts = split(cmd, ' ');
    std::string baseCommand = parts[0];
    
    if (baseCommand == "status") return DebugCommand::STATUS;
    if (baseCommand == "config") return DebugCommand::CONFIG;
    if (baseCommand == "neurons") return DebugCommand::NEURONS;
    if (baseCommand == "synapses") return DebugCommand::SYNAPSES;
    if (baseCommand == "memory") return DebugCommand::MEMORY;
    if (baseCommand == "plasticity") return DebugCommand::PLASITY;
    if (baseCommand == "system") return DebugCommand::SYSTEM;
    if (baseCommand == "help") return DebugCommand::HELP;
    if (baseCommand == "quit") return DebugCommand::QUIT;
    if (baseCommand == "exit") return DebugCommand::EXIT;
    
    return DebugCommand::UNKNOWN;
}

void DebugConsole::handleStatusCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    printSystemInfo(controller);
    std::cout << std::endl;
}

void DebugConsole::handleConfigCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    controller.printConfiguration();
    std::cout << std::endl;
}

void DebugConsole::handleNeuronsCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    std::cout << "=== NEURON STATUS ===" << std::endl;
    
    if (auto brain = controller.getBrain()) {
        std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    }
    
    std::cout << std::endl;
}

void DebugConsole::handleSynapsesCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    std::cout << "=== SYNAPSE STATUS ===" << std::endl;
    
    if (auto brain = controller.getBrain()) {
        std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    std::cout << std::endl;
}

void DebugConsole::handleMemoryCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    std::cout << "=== MEMORY STATUS ===" << std::endl;
    
    if (auto brain = controller.getBrain()) {
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        
        if (wm) {
            std::cout << "Working Memory Active Traces: " << wm->getActiveTraces() << std::endl;
            std::cout << "Working Memory Capacity: " << wm->getCapacity() << std::endl;
        }
        
        if (em) {
            std::cout << "Episodic Memory Episodes: " << em->getEpisodeCount() << std::endl;
            std::cout << "Episodic Memory Max Episodes: " << em->getMaxEpisodes() << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void DebugConsole::handlePlacityCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    std::cout << "=== PLASTICITY STATUS ===" << std::endl;
    
    if (auto brain = controller.getBrain()) {
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        if (stdp) {
            std::cout << "STDP: Enabled" << std::endl;
        }
        if (hebbian) {
            std::cout << "Hebbian: Enabled" << std::endl;
        }
        if (structural) {
            std::cout << "Structural Plasticity: Enabled" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void DebugConsole::handleSystemCommand(CoreBrainController& controller) {
    std::cout << std::endl;
    std::cout << "=== SYSTEM STATUS ===" << std::endl;
    
    std::cout << "Simulation State: " << 
        (controller.isRunning() ? "RUNNING" : 
         controller.isPaused() ? "PAUSED" : "STOPPED") << std::endl;
    
    auto metrics = controller.getMetrics();
    std::cout << "Steps Completed: " << metrics.totalSteps << std::endl;
    std::cout << "Average Firing Rate: " << metrics.averageFiringRate << std::endl;
    
    std::cout << std::endl;
}

void DebugConsole::handleHelpCommand() {
    std::cout << std::endl;
    std::cout << "=== DEBUG CONSOLE HELP ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  status      - Display brain and system status" << std::endl;
    std::cout << "  config      - Show current configuration" << std::endl;
    std::cout << "  neurons     - Show neuron statistics" << std::endl;
    std::cout << "  synapses    - Show synapse statistics" << std::endl;
    std::cout << "  memory      - Show memory system status" << std::endl;
    std::cout << "  plasticity  - Show plasticity system status" << std::endl;
    std::cout << "  system      - Show overall system status" << std::endl;
    std::cout << "  help        - Show this help message" << std::endl;
    std::cout << "  quit/exit   - Exit debug console" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Console Options:" << std::endl;
    std::cout << "  Set prompt:    prompt <new_prompt>" << std::endl;
    std::cout << "  Toggle timestamps:  timestamps" << std::endl;
    std::cout << "  Set log level: loglevel <level>" << std::endl;
    std::cout << "  Toggle autocomplete: autocomplete" << std::endl;
    std::cout << std::endl;
}

void DebugConsole::handleQuitCommand() {
    std::cout << "Exiting debug console..." << std::endl;
    consoleRunning = false;
    currentState.isRunning = false;
}

void DebugConsole::handleExitCommand() {
    std::cout << "Exiting debug console..." << std::endl;
    consoleRunning = false;
    currentState.isRunning = false;
}

void DebugConsole::setPrompt(const std::string& prompt) {
    currentState.prompt = prompt;
}

void DebugConsole::toggleTimestamps() {
    currentState.showTimestamps = !currentState.showTimestamps;
    std::cout << "Timestamps: " << (currentState.showTimestamps ? "Enabled" : "Disabled") << std::endl;
}

void DebugConsole::setLogLevel(const std::string& level) {
    std::transform(level.begin(), level.end(), level.begin(), ::toupper);
    currentState.logLevel = level;
    std::cout << "Log level: " << currentState.logLevel << std::endl;
}

void DebugConsole::toggleAutoComplete() {
    currentState.autoComplete = !currentState.autoComplete;
    std::cout << "Auto-complete: " << (currentState.autoComplete ? "Enabled" : "Disabled") << std::endl;
}

std::vector<std::string> DebugConsole::getCommandCompletions(const std::string& partial) {
    std::vector<std::string> completions;
    std::string prefix = toLower(trim(partial));
    
    // Common commands for completion
    std::vector<std::string> commands = {"status", "config", "neurons", "synapses", 
                                         "memory", "plasticity", "system", "help", "quit", "exit"};
    
    for (const auto& cmd : commands) {
        if (stringStartsWith(cmd, prefix)) {
            completions.push_back(cmd);
        }
    }
    
    return completions;
}

void DebugConsole::printBanner() {
    std::cout << std::endl;
    printColored("=== NLM DEBUG CONSOLE ===", "cyan");
    printColored("Phase 6 Neural Learning Machine", "green");
    printColored("Type 'help' for available commands", "yellow");
    printSeparator();
    std::cout << std::endl;
}

void DebugConsole::printSystemInfo(CoreBrainController& controller) {
    std::cout << "System Information:" << std::endl;
    std::cout << "  Version: 0.1.0 (Phase 6 Final Integration)" << std::endl;
    std::cout << "  Build: 2026-10-31" << std::endl;
    
    auto state = controller.getCurrentBrainState();
    std::cout << "  Current Step: " << state.step << std::endl;
    std::cout << "  Current Time: " << state.time << "s" << std::endl;
    
    auto metrics = controller.getMetrics();
    std::cout << "  Steps Completed: " << metrics.totalSteps << std::endl;
    std::cout << "  Average Firing Rate: " << metrics.averageFiringRate << std::endl;
    
    std::cout << "  Uptime: " << getUptime() << std::endl;
    std::cout << std::endl;
}

void DebugConsole::printSeparator() {
    std::cout << std::string(50, '=') << std::endl;
}

void DebugConsole::printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void DebugConsole::printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

std::string DebugConsole::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string DebugConsole::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> DebugConsole::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

bool DebugConsole::stringStartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string DebugConsole::getCurrentTimestamp() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - currentState.startTime).count();
    
    int hours = (duration / 3600000) % 24;
    int minutes = (duration / 60000) % 60;
    int seconds = (duration / 1000) % 60;
    int ms = duration % 1000;
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setw(2) << minutes << ":"
       << std::setw(2) << seconds << "."
       << std::setw(3) << ms;
    
    return ss.str();
}

std::string DebugConsole::getUptime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - currentState.startTime).count();
    
    int hours = duration / 3600;
    int minutes = (duration % 3600) / 60;
    int seconds = duration % 60;
    
    std::stringstream ss;
    if (hours > 0) ss << hours << "h ";
    ss << minutes << "m " << seconds << "s";
    
    return ss.str();
}

void DebugConsole::printColored(const std::string& text, const std::string& color) {
    // ANSI color codes (may not work on all terminals)
    std::string colorCode;
    if (color == "red") colorCode = "\033[31m";
    else if (color == "green") colorCode = "\033[32m";
    else if (color == "yellow") colorCode = "\033[33m";
    else if (color == "blue") colorCode = "\033[34m";
    else if (color == "cyan") colorCode = "\033[36m";
    else colorCode = "\033[0m";
    
    std::cout << colorCode << text << "\033[0m";
}

void DebugConsole::printSuccess(const std::string& text) {
    printColored("[SUCCESS] " + text, "green");
}

void DebugConsole::printInfo(const std::string& text) {
    printColored("[INFO] " + text, "white");
}

void DebugConsole::printDebug(const std::string& text) {
    printColored("[DEBUG] " + text, "cyan");
}

void DebugConsole::printWarning(const std::string& text) {
    printColored("[WARNING] " + text, "yellow");
}

void DebugConsole::printError(const std::string& text) {
    printColored("[ERROR] " + text, "red");
}

std::string DebugConsole::readLine() {
    std::string line;
    
    if (currentState.showTimestamps) {
        std::cout << "[" << getCurrentTimestamp() << "] " << currentState.prompt;
    } else {
        std::cout << currentState.prompt;
    }
    
    std::getline(std::cin, line);
    return line;
}

void DebugConsole::writeLine(const std::string& line) {
    std::cout << line << std::endl;
}

void DebugConsole::writeError(const std::string& error) {
    std::cerr << error << std::endl;
}

} // namespace nlm