// Interactive console interface for NLM
#pragma once

#include "CommandSystem.hpp"
#include "NLMUI.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <conio.h>  // For Windows console input

namespace nlm {

class InteractiveConsole {
private:
    std::shared_ptr<CommandSystem> commandSystem_;
    std::shared_ptr<NLMUI> ui_;
    std::shared_ptr<Logger> logger_;
    
    std::atomic<bool> running_ = false;
    std::thread inputThread_;
    
    std::string prompt_ = "> ";
    std::string currentCommand_;
    size_t commandHistoryIndex_ = 0;
    std::vector<std::string> commandHistory_;
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& prefix) {
        return commandSystem_->getAllCommands();
    }
    
    // Command history navigation
    void navigateHistory(int direction) {
        // TODO: Implement command history navigation
    }
    
    // Auto-complete command
    std::string completeCommand(const std::string& prefix) {
        auto completions = getCommandCompletions(prefix);
        if (completions.size() == 1) {
            return completions[0];
        } else if (completions.size() > 1) {
            // TODO: Show multiple completions
            return prefix;
        }
        return prefix;
    }
    
public:
    InteractiveConsole(std::shared_ptr<CommandSystem> commandSystem, 
                      std::shared_ptr<NLMUI> ui) 
        : commandSystem_(commandSystem), ui_(ui) {
        logger_ = Logger::getGlobal();
    }
    
    ~InteractiveConsole() {
        stop();
    }
    
    // Start console
    void start() {
        running_ = true;
        
        // Start input thread
        inputThread_ = std::thread(&InteractiveConsole::inputThreadFunc, this);
        
        if (logger_) {
            logger_->info("Interactive console started");
        }
    }
    
    // Stop console
    void stop() {
        running_ = false;
        
        if (inputThread_.joinable()) {
            inputThread_.join();
        }
        
        if (logger_) {
            logger_->info("Interactive console stopped");
        }
    }
    
    // Set prompt
    void setPrompt(const std::string& prompt) {
        prompt_ = prompt;
    }
    
    // Print help information
    void printHelp() {
        std::cout << "=== NLM Interactive Console Help ===\n";
        std::cout << "Commands:\n";
        std::cout << "  help [command]          - Show help for commands\n";
        std::cout << "  exit/quit              - Exit console\n";
        std::cout << "  clear                   - Clear screen\n";
        std::cout << "  history                 - Show command history\n";
        std::cout << "  Ctrl+L                  - Clear screen\n";
        std::cout << "\nCommand Groups:\n";
        std::cout << "  monitoring              - Brain monitoring commands\n";
        std::cout << "  network_explore         - Neural network analysis\n";
        std::cout << "  learning_control        - Learning control\n";
        std::cout << "  simulation_control      - Simulation control\n";
        std::cout << "  experiment_control      - Experiment control\n";
        std::cout << "  visualization_control   - Visualization control\n";
        std::cout << "\nUse TAB for command completion.\n";
    }
    
    // Print command history
    void printHistory() {
        std::cout << "=== Command History ===\n";
        auto history = commandSystem_->getHistory();
        auto entries = history->getRecent(50);
        
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            std::cout << std::setw(4) << i + 1 << ". ";
            std::cout << std::setw(20) << entry.commandLine << " ";
            std::cout << (entry.success ? "✓" : "✗") << " ";
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            std::cout << std::put_time(std::localtime(&time), "%H:%M:%S");
            std::cout << "\n";
        }
    }
    
private:
    // Input thread function
    void inputThreadFunc() {
#ifdef _WIN32
        // Windows console input
        while (running_) {
            if (_kbhit()) {
                char ch = _getch();
                
                switch (ch) {
                    case 27:  // Escape key
                        std::cout << "\nExiting console...\n";
                        stop();
                        return;
                    case 8:   // Backspace
                        if (!currentCommand_.empty()) {
                            currentCommand_.pop_back();
                            std::cout << "\r" << prompt_ << currentCommand_ << " ";
                        }
                        break;
                    case 9:   // Tab key (auto-complete)
                        if (!currentCommand_.empty()) {
                            currentCommand_ = completeCommand(currentCommand_);
                            std::cout << "\r" << prompt_ << currentCommand_ << " ";
                        }
                        break;
                    case 13:  // Enter key
                        if (!currentCommand_.empty()) {
                            executeCurrentCommand();
                            currentCommand_.clear();
                            commandHistoryIndex_ = commandHistory_.size();
                        }
                        std::cout << "\r" << prompt_;
                        break;
                    default:
                        if (ch >= 32 && ch <= 126) {
                            currentCommand_ += ch;
                            std::cout << ch;
                        }
                        break;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#else
        // Unix/Linux console input
        while (running_) {
            char ch = getchar();
            
            switch (ch) {
                case 27:  // Escape key
                    std::cout << "\nExiting console...\n";
                    stop();
                    return;
                case 127:  // Backspace
                case 8:   // Backspace (some terminals)
                    if (!currentCommand_.empty()) {
                        currentCommand_.pop_back();
                        std::cout << "\r" << prompt_ << currentCommand_ << " ";
                    }
                    break;
                case 9:   // Tab key (auto-complete)
                    if (!currentCommand_.empty()) {
                        currentCommand_ = completeCommand(currentCommand_);
                        std::cout << "\r" << prompt_ << currentCommand_ << " ";
                    }
                    break;
                case 10:  // Enter key
                case 13:  // Enter key (Windows)
                    if (!currentCommand_.empty()) {
                        executeCurrentCommand();
                        currentCommand_.clear();
                        commandHistoryIndex_ = commandHistory_.size();
                    }
                    std::cout << "\r" << prompt_;
                    break;
                default:
                    if (ch >= 32 && ch <= 126) {
                        currentCommand_ += ch;
                        std::cout << ch;
                    }
                    break;
            }
        }
#endif
    }
    
    // Execute current command
    void executeCurrentCommand() {
        if (currentCommand_.empty()) return;
        
        // Add to history
        commandHistory_.push_back(currentCommand_);
        
        // Execute command
        std::string result = ui_->executeCommand(currentCommand_);
        std::cout << result << "\n";
    }
};

} // namespace nlm
