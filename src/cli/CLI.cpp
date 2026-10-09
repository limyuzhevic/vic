// CLI implementation for NLM
#include "cli/CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <chrono>
#include <ctime>

namespace nlm {

struct CLI::Impl {
    std::map<std::string, CLICommand> commands;
    std::vector<CLIOption> options;
    std::unordered_map<std::string, std::string> parsedOptions;
    bool initialized;
    std::string lastOutputFile;
    size_t maxOutputHistory;
    bool autoSave;
    std::vector<std::string> outputHistory;
    std::map<std::string, std::string> aliases;
    
    Impl() : initialized(false), maxOutputHistory(100), autoSave(false) {}
};

CLI::CLI() : pImpl(std::make_unique<Impl>()) {}

CLI::~CLI() = default;

CLI::CLI(CLI&&) noexcept = default;

CLI& CLI::operator=(CLI&&) noexcept = default;

void CLI::registerCommand(const CLICommand& command) {
    pImpl->commands[command.name] = command;
    for (const auto& alias : command.aliases) {
        pImpl->aliases[alias] = command.name;
    }
}

void CLI::unregisterCommand(const std::string& name) {
    pImpl->commands.erase(name);
    for (auto it = pImpl->aliases.begin(); it != pImpl->aliases.end();) {
        if (it->second == name) {
            it = pImpl->aliases.erase(it);
        } else {
            ++it;
        }
    }
}

bool CLI::executeCommand(const std::string& commandLine) {
    std::vector<std::string> args = tokenize(commandLine);
    return executeCommand(args);
}

bool CLI::executeCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        return false;
    }
    
    std::string commandName = args[0];
    
    auto aliasIt = pImpl->aliases.find(commandName);
    if (aliasIt != pImpl->aliases.end()) {
        commandName = aliasIt->second;
    }
    
    auto it = pImpl->commands.find(commandName);
    if (it == pImpl->commands.end()) {
        printError("Unknown command: " + commandName);
        return false;
    }
    
    const CLICommand& command = it->second;
    
    if (command.requiresBrain && !brain) {
        printError("Command requires a brain instance");
        return false;
    }
    
    return command.execute(args);
}

void CLI::addOption(const CLIOption& option) {
    pImpl->options.push_back(option);
}

bool CLI::parseOptions(const std::vector<std::string>& args, std::unordered_map<std::string, std::string>& parsedOptions) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parsedOptions[key] = value;
            } else {
                parsedOptions[arg.substr(2)] = "";
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            if (arg.size() > 2 && arg[2] == '=') {
                std::string key = arg.substr(1, 1);
                std::string value = arg.substr(3);
                parsedOptions["-" + key] = value;
            } else {
                parsedOptions["-" + arg.substr(1)] = "";
            }
        }
    }
    return true;
}

void CLI::updateState(const std::string& key, const std::string& value) {
    if (key == "verbose") {
        state.verbose = (value == "true" || value == "1" || value == "yes");
    } else if (key == "quiet") {
        state.quiet = (value == "true" || value == "1" || value == "yes");
    } else if (key == "logLevel") {
        try {
            state.logLevel = std::stoi(value);
        } catch (...) {
            state.logLevel = 2;
        }
    } else if (key == "interactive") {
        state.interactive = (value == "true" || value == "1" || value == "yes");
    } else if (key == "scriptMode") {
        state.scriptMode = (value == "true" || value == "1" || value == "yes");
    } else if (key == "scriptFile") {
        state.scriptFile = value;
    } else if (key == "showHelp") {
        state.showHelp = (value == "true" || value == "1" || value == "yes");
    } else if (key == "showVersion") {
        state.showVersion = (value == "true" || value == "1" || value == "yes");
    } else if (key == "outputFormat") {
        state.outputFormat = value;
    } else if (key == "colorOutput") {
        state.colorOutput = (value == "true" || value == "1" || value == "yes");
    } else if (key == "progressBar") {
        state.progressBar = (value == "true" || value == "1" || value == "yes");
    }
}

void CLI::showHelp(const std::string& command) const {
    if (command.empty()) {
        print("Available commands:");
        for (const auto& pair : pImpl->commands) {
            print("  " + pair.first + " - " + pair.second.description);
        }
        print("");
        print("Use 'help <command>' for more details on a specific command.");
    } else {
        showCommandHelp(command);
    }
}

void CLI::showVersion() const {
    print("NLM Phase 2 - Neural Learning Machine v0.1.0");
}

void CLI::showCommandHelp(const std::string& command) const {
    auto it = pImpl->commands.find(command);
    if (it == pImpl->commands.end()) {
        printError("Unknown command: " + command);
        return;
    }
    
    const CLICommand& cmd = it->second;
    print("Command: " + cmd.name);
    print("Description: " + cmd.description);
    print("Usage: " + cmd.usage);
    print("");
    if (!cmd.examples.empty()) {
        print("Examples:");
        for (const auto& example : cmd.examples) {
            print("  " + example);
        }
    }
}

void CLI::showAllCommands() const {
    print("=== All Commands ===");
    for (const auto& pair : pImpl->commands) {
        const CLICommand& cmd = pair.second;
        print(cmd.name + " - " + cmd.description);
    }
}

std::vector<std::string> CLI::getCommandCompletions(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    for (const auto& pair : pImpl->commands) {
        if (pair.first.find(partialCommand) == 0) {
            completions.push_back(pair.first);
        }
    }
    return completions;
}

std::vector<std::string> CLI::getOptionCompletions(const std::string& command, const std::string& partialOption) const {
    std::vector<std::string> completions;
    auto it = pImpl->commands.find(command);
    if (it != pImpl->commands.end()) {
        const CLICommand& cmd = it->second;
        for (const auto& example : cmd.examples) {
            if (example.find(partialOption) == 0) {
                completions.push_back(example);
            }
        }
    }
    return completions;
}

bool CLI::runScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printError("Cannot open script file: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        handleCommand(line);
    }
    
    file.close();
    return true;
}

void CLI::startInteractiveMode() {
    print("NLM CLI Interactive Mode");
    print("Type 'help' for available commands, 'exit' to quit.");
    print("");
    
    std::string line;
    while (state.interactive) {
        print(">>> ", false);
        std::getline(std::cin, line);
        
        if (line == "exit" || line == "quit" || line == "q") {
            state.interactive = false;
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        handleCommand(line);
    }
}

void CLI::handleCommand(const std::string& commandLine) {
    if (commandLine.empty()) {
        return;
    }
    
    executeCommand(commandLine);
}

void CLI::print(const std::string& message, int level) const {
    if (state.quiet && level > 0) {
        return;
    }
    
    if (state.colorOutput && level > 0) {
        if (level == 1) {
            std::cout << "\033[32m" << message << "\033[0m" << std::endl;
        } else if (level == 2) {
            std::cout << "\033[33m" << message << "\033[0m" << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    } else {
        std::cout << message << std::endl;
    }
}

void CLI::printError(const std::string& error) const {
    if (state.colorOutput) {
        std::cerr << "\033[31mERROR: " << error << "\033[0m" << std::endl;
    } else {
        std::cerr << "ERROR: " << error << std::endl;
    }
}

void CLI::printWarning(const std::string& warning) const {
    if (state.colorOutput) {
        std::cout << "\033[33mWARNING: " << warning << "\033[0m" << std::endl;
    } else {
        std::cout << "WARNING: " << warning << std::endl;
    }
}

void CLI::printInfo(const std::string& info) const {
    if (state.colorOutput) {
        std::cout << "\033[36mINFO: " << info << "\033[0m" << std::endl;
    } else {
        std::cout << "INFO: " << info << std::endl;
    }
}

void CLI::printSuccess(const std::string& success) const {
    if (state.colorOutput) {
        std::cout << "\033[32mSUCCESS: " << success << "\033[0m" << std::endl;
    } else {
        std::cout << "SUCCESS: " << success << std::endl;
    }
}

void CLI::printProgress(const std::string& progress, double percentage) const {
    if (!state.progressBar) {
        print(progress);
        return;
    }
    
    if (state.colorOutput) {
        std::cout << "\033[36mProgress: [";
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * percentage / 100.0);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(percentage) << "%\033[0m" << std::endl;
    } else {
        std::cout << "Progress: ["; 
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * percentage / 100.0);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(percentage) << "%" << std::endl;
    }
}

double CLI::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - state.startTime);
    return elapsed.count();
}

bool CLI::isSlowCommand(double threshold) const {
    double elapsed = getElapsedTime();
    if (threshold < 0) {
        threshold = state.performanceThreshold;
    }
    return elapsed > threshold;
}

std::vector<std::string> CLI::tokenize(const std::string& commandLine) const {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    char quoteChar = '\0';
    
    for (char c : commandLine) {
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else if (c == '"' || c == '\'') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = c;
            } else if (quoteChar == c) {
                inQuotes = false;
                quoteChar = '\0';
            } else {
                token += c;
            }
        } else {
            token += c;
        }
    }
    
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string CLI::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool CLI::matchesPattern(const std::string& str, const std::string& pattern) const {
    if (pattern.empty()) return true;
    if (pattern == "*" || pattern == "ALL") return true;
    if (pattern[0] == '*') {
        size_t pos = str.find(pattern.substr(1));
        return pos != std::string::npos;
    }
    return str.find(pattern) != std::string::npos;
}

void CLI::setupDefaultCommands() {
    // Default commands would be registered here
}

void CLI::setupSystemCommands() {
    // System commands would be set up here
}
