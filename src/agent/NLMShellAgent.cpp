#include "NLMShellAgent.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <conio.h> // For _getch on Windows, but we need a cross-platform solution

namespace nlm {

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

NLMShellAgent::NLMShellAgent(std::shared_ptr<NLMCLI> cli)
    : isRunning_(false)
    , cli_(cli)
    , historySize_(100)
    , historyIndex_(0)
    , verboseMode_(false)
    , colorsEnabled_(true)
    , errorHandlingEnabled_(true)
    , continueOnError_(false)
    , hasErrors_(false)
    , logLevel_(2) // Info level
    , logLevel_(2) // Info level
    , commandAliasesEnabled_(true)
    , commandMacrosEnabled_(true)
    , isRecordingMacro_(false)
    , scriptMode_(false)
    , realTimeMode_(true)
    , refreshRate_(10)
    , performanceMonitoringEnabled_(false)
    , commandCount_(0)
    , totalExecutionTime_(0.0)
    , clipboardIntegrationEnabled_(false)
    , terminalInitialized_(false)
    , terminalWidth_(80)
    , terminalHeight_(24) {
    
    // Initialize terminal
    initializeTerminal();
    
    // Initialize performance monitoring
    if (performanceMonitoringEnabled_) {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    NLM_LOG_INFO("NLMShellAgent initialized");
}

NLMShellAgent::~NLMShellAgent() {
    stop();
    restoreTerminalState();
    NLM_LOG_INFO("NLMShellAgent destroyed");
}

void NLMShellAgent::initializeTerminal() {
#ifdef _WIN32
    // Windows terminal initialization
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT;
    SetConsoleMode(hOut, mode);
    
    // Enable virtual terminal sequences for colors
    if (GetVersion() >= 0x0A00) { // Windows 10
        SetConsoleMode(hOut, mode | 0x0004);
    }
#else
    // Unix/Linux terminal initialization
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    
    // Disable canonical mode and echo
    tty.c_lflag &= ~(ICANON | ECHO);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    
    // Save original terminal state
    tcgetattr(STDIN_FILENO, &originalTTY_);
#endif
    
    terminalInitialized_ = true;
}

void NLMShellAgent::start() {
    if (isRunning_) {
        printError("Shell agent is already running");
        return;
    }
    
    isRunning_ = true;
    printSuccess("NLMShellAgent started");
    
    // Start main loop in a separate thread
    agentThread_ = std::thread(&NLMShellAgent::run, this);
}

void NLMShellAgent::stop() {
    if (!isRunning_) {
        return;
    }
    
    isRunning_ = false;
    
    if (agentThread_.joinable()) {
        agentThread_.join();
    }
    
    printSuccess("NLMShellAgent stopped");
}

void NLMShellAgent::run() {
    printPrompt();
    
    while (isRunning_) {
        // Check for input with timeout
        if (waitForInput(100)) {
            std::string input;
            std::getline(std::cin, input);
            
            if (std::cin.eof()) {
                break;
            }
            
            // Process the command
            processCommand(input);
            
            printPrompt();
        }
        
        // Update performance stats if enabled
        if (performanceMonitoringEnabled_) {
            updatePerformanceStats();
        }
        
        // Sleep briefly to prevent high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void NLMShellAgent::processCommand(const std::string& commandLine) {
    auto startTime = std::chrono::steady_clock::now();
    std::string processedCommand = commandLine;
    
    // Handle special commands
    if (commandLine == ".quit" || commandLine == ".exit" || commandLine == "quit" || commandLine == "exit") {
        stop();
        return;
    }
    
    if (commandLine == ".clear") {
        clearScreen();
        return;
    }
    
    if (commandLine == ".history") {
        listCommandHistory();
        return;
    }
    
    if (commandLine == ".save") {
        saveSession("session_" + getCurrentTimestamp() + ".txt");
        return;
    }
    
    // Expand command (aliases, macros)
    processedCommand = expandCommand(commandLine);
    
    // Add to history
    addToHistory(processedCommand);
    
    // Log the command
    logCommand(processedCommand);
    
    // Execute the command
    try {
        if (cli_) {
            cli_->executeCommand(processedCommand);
        } else {
            printError("NLM CLI not connected");
        }
    } catch (const std::exception& e) {
        handleError(e);
        if (!continueOnError_) {
            return;
        }
    }
    
    // Update performance stats
    if (performanceMonitoringEnabled_) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        totalExecutionTime_ += duration.count();
        commandCount_++;
    }
}

void NLMShellAgent::setState(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    state_[key] = value;
}

std::string NLMShellAgent::getState(const std::string& key) const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    auto it = state_.find(key);
    return (it != state_.end()) ? it->second : "";
}

void NLMShellAgent::clearState(const std::string& key) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    state_.erase(key);
}

void NLMShellAgent::clearAllState() {
    std::lock_guard<std::mutex> lock(stateMutex_);
    state_.clear();
}

void NLMShellAgent::addToHistory(const std::string& command) {
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    // Skip empty commands and duplicates
    if (command.empty() || !commandHistory_.empty() && commandHistory_.back() == command) {
        return;
    }
    
    commandHistory_.push_back(command);
    historyIndex_ = commandHistory_.size();
    
    // Trim history if it gets too large
    if (commandHistory_.size() > historySize_) {
        commandHistory_.erase(commandHistory_.begin());
    }
}

void NLMShellAgent::listCommandHistory() const {
    std::cout << "Command History:" << std::endl;
    std::cout << "========" << std::endl;
    
    size_t start = std::max<size_t>(0, commandHistory_.size() - 20);
    for (size_t i = start; i < commandHistory_.size(); ++i) {
        std::cout << (i + 1) << ". " << commandHistory_[i] << std::endl;
    }
}

void NLMShellAgent::clearHistory() {
    std::lock_guard<std::mutex> lock(historyMutex_);
    commandHistory_.clear();
    historyIndex_ = 0;
    printSuccess("Command history cleared");
}

void NLMShellAgent::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void NLMShellAgent::setVerboseMode(bool enabled) {
    verboseMode_ = enabled;
    printStatus("Verbose mode " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::setColorsEnabled(bool enabled) {
    colorsEnabled_ = enabled;
    printStatus("Colors " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::printPrompt() const {
    // Show current directory/file if in script mode
    std::string prompt = "[cli]";
    
    if (scriptMode_) {
        prompt += "[script]";
    }
    
    if (!currentSession_.empty()) {
        // Extract filename from path
        size_t pos = currentSession_.find_last_of("/\\");
        std::string sessionName = (pos != std::string::npos) ? currentSession_.substr(pos + 1) : currentSession_;
        prompt += "[" + sessionName + "]";
    }
    
    // Show current working directory
    prompt += " ";
    
    // Display prompt
    std::cout << prompt << " ";
    std::cout.flush();
}

void NLMShellAgent::setErrorHandling(bool enabled) {
    errorHandlingEnabled_ = enabled;
}

void NLMShellAgent::setContinueOnError(bool enabled) {
    continueOnError_ = enabled;
}

void NLMShellAgent::log(const std::string& message) {
    if (logLevel_ <= 2) { // Info level and above
        writeLogEntry("INFO", message);
    }
}

void NLMShellAgent::logCommand(const std::string& command) {
    if (logLevel_ <= 2) { // Info level
        writeLogEntry("COMMAND", command);
    }
}

void NLMShellAgent::logOutput(const std::string& output) {
    if (logLevel_ <= 3) { // Debug level
        writeLogEntry("OUTPUT", output);
    }
}

void NLMShellAgent::logError(const std::string& error) {
    writeLogEntry("ERROR", error);
}

void NLMShellAgent::saveSession(const std::string& filename) {
    currentSession_ = filename;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Cannot save session: " + filename);
        return;
    }
    
    // Save session metadata
    file << "# NLM Session" << std::endl;
    file << "# Saved: " << getCurrentTimestamp() << std::endl;
    file << "# Commands: " << commandHistory_.size() << std::endl;
    file << std::endl;
    
    // Save command history
    file << "=== Command History ===" << std::endl;
    for (const auto& cmd : commandHistory_) {
        file << cmd << std::endl;
    }
    
    // Save state
    file << std::endl << "=== State ===" << std::endl;
    for (const auto& pair : state_) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    // Save aliases and macros
    if (!commandAliases_.empty()) {
        file << std::endl << "=== Command Aliases ===" << std::endl;
        for (const auto& pair : commandAliases_) {
            file << pair.first << " -> " << pair.second << std::endl;
        }
    }
    
    if (!commandMacros_.empty()) {
        file << std::endl << "=== Command Macros ===" << std::endl;
        for (const auto& pair : commandMacros_) {
            file << pair.first << " -> " << pair.second << std::endl;
        }
    }
    
    printStatus("Session saved to: " + filename);
    printSuccess("Session saved successfully");
}

void NLMShellAgent::loadSession(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printError("Cannot load session: " + filename);
        return;
    }
    
    std::string line;
    std::string section;
    
    // Clear current state
    clearAllState();
    clearHistory();
    commandAliases_.clear();
    commandMacros_.clear();
    
    while (std::getline(file, line)) {
        // Skip comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line.find("===") != std::string::npos) {
            section = line;
            continue;
        }
        
        // Parse based on section
        if (section.find("State") != std::string::npos) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                setState(key, value);
            }
        } else if (section.find("Command Aliases") != std::string::npos) {
            size_t pos = line.find("->");
            if (pos != std::string::npos) {
                std::string alias = line.substr(0, pos);
                std::string command = line.substr(pos + 2);
                // Trim whitespace
                alias.erase(0, alias.find_first_not_of(" \t"));
                alias.erase(alias.find_last_not_of(" \t") + 1);
                command.erase(0, command.find_first_not_of(" \t"));
                command.erase(command.find_last_not_of(" \t") + 1);
                commandAliases_[alias] = command;
            }
        } else if (section.find("Command Macros") != std::string::npos) {
            size_t pos = line.find("->");
            if (pos != std::string::npos) {
                std::string name = line.substr(0, pos);
                std::string macro = line.substr(pos + 2);
                // Trim whitespace
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                macro.erase(0, macro.find_first_not_of(" \t"));
                macro.erase(macro.find_last_not_of(" \t") + 1);
                commandMacros_[name] = macro;
            }
        } else {
            // Command history
            addToHistory(line);
        }
    }
    
    currentSession_ = filename;
    printStatus("Session loaded from: " + filename);
    printSuccess("Session loaded successfully");
    printStatus("Loaded " + std::to_string(commandHistory_.size()) + " commands");
}

void NLMShellAgent::listSessions() const {
    std::cout << "Available Sessions:" << std::endl;
    
    // Look for session files
    std::vector<std::string> sessionFiles;
    for (int i = 1; i <= 20; ++i) {
        std::string filename = "session_" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (file.is_open()) {
            sessionFiles.push_back(filename);
        }
    }
    
    if (sessionFiles.empty()) {
        std::cout << "  No saved sessions found." << std::endl;
    } else {
        for (size_t i = 0; i < sessionFiles.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << sessionFiles[i] << std::endl;
        }
    }
}

void NLMShellAgent::executeScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printError("Cannot open script file: " + filename);
        return;
    }
    
    setScriptMode(true);
    printStatus("Executing script: " + filename);
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Show line number for debugging
        std::cout << ">>> " << lineNum << ": " << line << std::endl;
        
        // Execute the command
        try {
            processCommand(line);
        } catch (const std::exception& e) {
            handleError(e);
            if (!continueOnError_) {
                break;
            }
        }
        
        // Small delay for readability
        if (!realTimeMode_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    setScriptMode(false);
    printSuccess("Script execution completed");
}

void NLMShellAgent::setScriptMode(bool enabled) {
    scriptMode_ = enabled;
    printStatus("Script mode " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::enableTabCompletion(bool enabled) {
    // Tab completion is handled by the CLI
    printStatus("Tab completion " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::setTabCompletionCallback(std::function<std::vector<std::string>(const std::string&)> callback) {
    // Store the callback for later use
    tabCompletionCallback_ = callback;
}

void NLMShellAgent::enableCommandAliases(bool enabled) {
    commandAliasesEnabled_ = enabled;
    printStatus("Command aliases " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::addCommandAlias(const std::string& alias, const std::string& command) {
    if (commandAliasesEnabled_) {
        commandAliases_[alias] = command;
        printStatus("Added alias: " << alias " -> " << command);
    }
}

void NLMShellAgent::enableCommandMacros(bool enabled) {
    commandMacrosEnabled_ = enabled;
    printStatus("Command macros " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::addCommandMacro(const std::string& name, const std::string& macro) {
    if (commandMacrosEnabled_) {
        commandMacros_[name] = macro;
        printStatus("Added macro: " << name " -> " << macro);
    }
}

void NLMShellAgent::startMacroRecording(const std::string& name) {
    if (!commandMacrosEnabled_) {
        printError("Command macros are disabled");
        return;
    }
    
    isRecordingMacro_ = true;
    currentMacroName_ = name;
    recordedCommands_.clear();
    printStatus("Started recording macro: " + name);
}

void NLMShellAgent::stopMacroRecording() {
    if (!isRecordingMacro_) {
        printError("No macro is being recorded");
        return;
    }
    
    // Convert recorded commands to macro
    std::string macro;
    for (const auto& cmd : recordedCommands_) {
        macro += cmd + "; ";
    }
    
    addCommandMacro(currentMacroName_, macro);
    isRecordingMacro_ = false;
    currentMacroName_.clear();
    recordedCommands_.clear();
    printSuccess("Macro recording stopped and saved");
}

void NLMShellAgent::playbackMacro(const std::string& name) {
    if (!commandMacrosEnabled_) {
        printError("Command macros are disabled");
        return;
    }
    
    auto it = commandMacros_.find(name);
    if (it == commandMacros_.end()) {
        printError("Macro not found: " + name);
        return;
    }
    
    std::string macro = it->second;
    std::stringstream ss(macro);
    std::string cmd;
    
    printStatus("Playing back macro: " + name);
    while (std::getline(ss, cmd, ';')) {
        if (!cmd.empty()) {
            processCommand(cmd);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    printSuccess("Macro playback completed");
}

void NLMShellAgent::setRealTimeMode(bool enabled) {
    realTimeMode_ = enabled;
    printStatus("Real-time mode " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::enablePerformanceMonitoring(bool enabled) {
    performanceMonitoringEnabled_ = enabled;
    if (enabled) {
        startTime_ = std::chrono::steady_clock::now();
        commandCount_ = 0;
        totalExecutionTime_ = 0.0;
    }
    printStatus("Performance monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::printPerformanceStats() const {
    if (!performanceMonitoringEnabled_ || commandCount_ == 0) {
        return;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    
    std::cout << "\nPerformance Statistics:" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Commands executed: " << commandCount_ << std::endl;
    std::cout << "Total execution time: " << totalExecutionTime_ << " ms" << std::endl;
    std::cout << "Average time per command: " << (totalExecutionTime_ / commandCount_) << " ms" << std::endl;
    std::cout << "Total session time: " << totalDuration.count() << " ms" << std::endl;
    std::cout << "Commands per second: " << (commandCount_ * 1000.0 / totalDuration.count()) << std::endl;
}

void NLMShellAgent::enableClipboardIntegration(bool enabled) {
    clipboardIntegrationEnabled_ = enabled;
    printStatus("Clipboard integration " + std::string(enabled ? "enabled" : "disabled"));
}

void NLMShellAgent::copyToClipboard(const std::string& text) {
    if (!clipboardIntegrationEnabled_) {
        return;
    }
    
#ifdef _WIN32
    // Windows clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    
    // Allocate global memory
    HGLOBAL hClip = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
    if (hClip) {
        memcpy(GlobalLock(hClip), text.c_str(), text.size() + 1);
        GlobalUnlock(hClip);
        SetClipboardData(CF_TEXT, hClip);
    }
    
    CloseClipboard();
#else
    // Unix/Linux clipboard (simplified)
    // Note: This is a simplified implementation
    // In a real implementation, you'd use xclip, xsel, or similar
    std::string command = "echo \"" + text + "\" | pbcopy";
    system(command.c_str());
#endif
}

void NLMShellAgent::pasteFromClipboard() {
    if (!clipboardIntegrationEnabled_) {
        return;
    }
    
#ifdef _WIN32
    // Windows clipboard
    OpenClipboard(NULL);
    
    HANDLE hClip = GetClipboardData(CF_TEXT);
    if (hClip) {
        char* buffer = (char*)GlobalLock(hClip);
        if (buffer) {
            std::string text = buffer;
            GlobalUnlock(hClip);
            
            // Process the text
            processCommand(text);
        }
    }
    
    CloseClipboard();
#else
    // Unix/Linux clipboard (simplified)
    // Note: This is a simplified implementation
    std::string command = "pbpaste";
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[1024];
        std::string text;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            text += buffer;
        }
        pclose(pipe);
        
        // Process the text
        processCommand(text);
    }
#endif
}

void NLMShellAgent::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void NLMShellAgent::setTerminalSize(int width, int height) {
    terminalWidth_ = width;
    terminalHeight_ = height;
}

void NLMShellAgent::saveTerminalState() {
#ifdef _WIN32
    // Windows - save console mode
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hOut, &terminalMode_);
#else
    // Unix/Linux - save terminal attributes
    tcgetattr(STDIN_FILENO, &originalTTY_);
#endif
}

void NLMShellAgent::restoreTerminalState() {
#ifdef _WIN32
    // Windows - restore console mode
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hOut, terminalMode_);
#else
    // Unix/Linux - restore terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTTY_);
#endif
    
    terminalInitialized_ = false;
}

void NLMShellAgent::connectToNLM(std::shared_ptr<NLMCLI> cli) {
    cli_ = cli;
}

void NLMShellAgent::setConfigFile(const std::string& filename) {
    configFile_ = filename;
}

void NLMShellAgent::loadConfiguration() {
    // Load configuration from file
    // This would typically use the NLMCLI to load configuration
    if (cli_) {
        cli_->loadConfigurationFromFile(configFile_);
    }
}

void NLMShellAgent::saveConfiguration() const {
    // Save configuration to file
    // This would typically use the NLMCLI to save configuration
    if (cli_) {
        cli_->saveConfigurationToFile(configFile_);
    }
}

// Protected and private method implementations

std::string NLMShellAgent::expandCommand(const std::string& commandLine) const {
    std::string result = commandLine;
    
    // Check for command aliases
    if (commandAliasesEnabled_) {
        // Find the first word in the command
        size_t spacePos = commandLine.find(' ');
        std::string cmd = commandLine;
        if (spacePos != std::string::npos) {
            cmd = commandLine.substr(0, spacePos);
        }
        
        auto it = commandAliases_.find(cmd);
        if (it != commandAliases_.end()) {
            result = it->second;
            if (spacePos != std::string::npos) {
                result += commandLine.substr(spacePos);
            }
        }
    }
    
    return result;
}

std::vector<std::string> NLMShellAgent::autocompleteCommand(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    // Simple auto-completion (would be enhanced with tab completion)
    if (!partialCommand.empty() && tabCompletionEnabled_) {
        // Find commands that start with the partial command
        for (const auto& pair : commandAliases_) {
            if (pair.first.find(partialCommand) == 0) {
                completions.push_back(pair.first);
            }
        }
    }
    
    return completions;
}

bool NLMShellAgent::executeMacro(const std::string& macroName) const {
    auto it = commandMacros_.find(macroName);
    if (it == commandMacros_.end()) {
        return false;
    }
    
    std::string macro = it->second;
    std::stringstream ss(macro);
    std::string cmd;
    
    while (std::getline(ss, cmd, ';')) {
        if (!cmd.empty()) {
            // In a real implementation, this would execute the command
            // For now, just log it
            NLM_LOG_INFO("Executing macro command: " + cmd);
        }
    }
    
    return true;
}

std::string NLMShellAgent::formatOutput(const std::string& output) const {
    if (outputFormat_ == "json") {
        // JSON formatting
        return "{\"output\": \"" + output + "\"}";
    } else if (outputFormat_ == "xml") {
        // XML formatting
        return "<output>" + output + "</output>";
    } else if (outputFormat_ == "raw") {
        // Raw formatting
        return output;
    }
    
    // Default formatting
    return output;
}

void NLMShellAgent::printColored(const std::string& text, const std::string& color) const {
    if (!colorsEnabled_) {
        std::cout << text;
        return;
    }
    
#ifdef _WIN32
    // Windows - set console color
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (color == "red") {
        SetConsoleTextAttribute(hOut, 4);
    } else if (color == "green") {
        SetConsoleTextAttribute(hOut, 2);
    } else if (color == "yellow") {
        SetConsoleTextAttribute(hOut, 6);
    } else if (color == "blue") {
        SetConsoleTextAttribute(hOut, 1);
    } else {
        SetConsoleTextAttribute(hOut, 7);
    }
    
    std::cout << text;
    
    // Reset to default
    SetConsoleTextAttribute(hOut, 7);
#else
    // Unix/Linux - ANSI escape codes
    if (color == "red") {
        std::cout << "\033[31m" << text << "\033[0m";
    } else if (color == "green") {
        std::cout << "\033[32m" << text << "\033[0m";
    } else if (color == "yellow") {
        std::cout << "\033[33m" << text << "\033[0m";
    } else if (color == "blue") {
        std::cout << "\033[34m" << text << "\033[0m";
    } else {
        std::cout << text;
    }
#endif
}

void NLMShellAgent::saveStateToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "# NLM Shell Agent State" << std::endl;
    file << "# Saved: " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    // Save state
    file << "State:" << std::endl;
    for (const auto& pair : state_) {
        file << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    // Save command history
    file << std::endl << "Command History:" << std::endl;
    for (const auto& cmd : commandHistory_) {
        file << "  " << cmd << std::endl;
    }
}

void NLMShellAgent::loadStateFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    std::string section;
    
    // Clear current state
    clearAllState();
    clearHistory();
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line.find(':') != std::string::npos && (line.find("State") != std::string::npos || 
                                                     line.find("Command History") != std::string::npos)) {
            section = line;
            continue;
        }
        
        // Parse based on section
        if (section.find("State") != std::string::npos) {
            // Skip indentation
            size_t pos = line.find_first_not_of(" \t");
            if (pos != std::string::npos && line.substr(pos, 2) == "  ") {
                pos += 2;
                size_t eqPos = line.find('=', pos);
                if (eqPos != std::string::npos) {
                    std::string key = line.substr(pos, eqPos - pos);
                    std::string value = line.substr(eqPos + 1);
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    setState(key, value);
                }
            }
        } else if (section.find("Command History") != std::string::npos) {
            // Skip indentation
            size_t pos = line.find_first_not_of(" \t");
            if (pos != std::string::npos && line.substr(pos, 2) == "  ") {
                pos += 2;
                addToHistory(line.substr(pos));
            }
        }
    }
}

void NLMShellAgent::handleError(const std::exception& e) {
    handleError(std::string(e.what()));
}

void NLMShellAgent::handleError(const std::string& error) {
    lastError_ = error;
    hasErrors_ = true;
    
    if (errorHandlingEnabled_) {
        if (continueOnError_) {
            logError("Error (continuing): " + error);
        } else {
            logError("Error: " + error);
            // In a real implementation, this would show the error to the user
        }
    }
}

void NLMShellAgent::writeLogEntry(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::string timestamp = getCurrentTimestamp();
    std::string logEntry = "[" + timestamp + "] [" + level + "] " + message;
    
    // Print to console
    if (colorsEnabled_) {
        if (level == "ERROR") {
            printColored(logEntry, "red");
        } else if (level == "WARNING") {
            printColored(logEntry, "yellow");
        } else if (level == "INFO") {
            printColored(logEntry, "green");
        } else {
            std::cout << logEntry;
        }
    } else {
        std::cout << logEntry;
    }
    
    std::cout << std::endl;
    
    // Write to log file
    if (!logFile_.empty()) {
        if (!logStream_.is_open()) {
            logStream_.open(logFile_, std::ios::app);
        }
        if (logStream_.is_open()) {
            logStream_ << logEntry << std::endl;
        }
    }
}

void NLMShellAgent::rotateLogFile() {
    if (logStream_.is_open()) {
        logStream_.close();
        
        // Rotate log file
        std::string newName = logFile_ + "." + getCurrentTimestamp();
        std::rename(logFile_.c_str(), newName.c_str());
        
        // Reopen for writing
        logStream_.open(logFile_, std::ios::app);
    }
}

void NLMShellAgent::updatePerformanceStats() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_);
    
    // Update display every second
    static auto lastUpdate = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate);
    
    if (elapsed.count() >= 1) {
        if (commandCount_ > 0) {
            std::cout << "\r" << commandCount_ << " commands executed ("
                     << (commandCount_ * 1000.0 / duration.count()) << " cmds/sec)" << std::flush;
        }
        lastUpdate = now;
    }
}

std::string NLMShellAgent::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool NLMShellAgent::waitForInput(int timeout_ms) {
#ifdef _WIN32
    // Windows implementation using Select
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD waitResult = WaitForSingleObject(hStdin, timeout_ms);
    return waitResult == WAIT_OBJECT_0;
#else
    // Unix/Linux implementation using poll
    struct pollfd fd[1];
    fd[0].fd = STDIN_FILENO;
    fd[0].events = POLLIN;
    
    int pollResult = poll(fd, 1, timeout_ms);
    return pollResult > 0 && (fd[0].revents & POLLIN);
#endif
}

void NLMShellAgent::listCommandHistory() const {
    std::cout << "Command History (" << commandHistory_.size() << " commands):" << std::endl;
    std::cout << "=========" << std::endl;
    
    // Show recent commands
    size_t start = std::max<size_t>(0, commandHistory_.size() - 50);
    for (size_t i = start; i < commandHistory_.size(); ++i) {
        std::cout << (i + 1) << ". " << commandHistory_[i] << std::endl;
    }
}
