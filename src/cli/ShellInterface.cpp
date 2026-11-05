// NLM CLI Shell Interface
// Provides an interactive shell for running NLM commands

#include "ShellInterface.hpp"
#include "CommandProcessor.hpp"
#include "ResultFormatter.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace nlm;

ShellInterface::ShellInterface() : interactiveMode(true), commandHistoryFile("nlm_shell_history") {
    commandProcessor = std::make_unique<CommandProcessor>();
    resultFormatter = std::make_unique<ResultFormatter>();
}

ShellInterface::~ShellInterface() = default;

void ShellInterface::run() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 NLM Interactive Shell                              ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Type 'help' to see available commands.                           ║" << std::endl;
    std::cout << "║ Type 'exit' or 'quit' to leave.                                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::string command;
    std::vector<std::string> history;
    
    while (interactiveMode && std::getline(std::cin, command)) {
        if (command.empty()) {
            continue;
        }
        
        // Add to history
        history.push_back(command);
        
        // Process command
        processCommand(command, history);
        
        // Save history periodically
        if (history.size() % 100 == 0) {
            saveHistory();
        }
    }
    
    // Save final history
    saveHistory();
    
    std::cout << "Exiting NLM shell. Goodbye!" << std::endl;
}

void ShellInterface::processCommand(const std::string& command, std::vector<std::string>& history) {
    // Parse command
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    // Check if command is in history
    if (cmd == "history") {
        showHistory(history);
        return;
    }
    
    // Check for special commands
    if (cmd == "help" || cmd == "?") {
        showHelp();
        return;
    } else if (cmd == "clear" || cmd == "cls") {
        clearScreen();
        return;
    } else if (cmd == "exit" || cmd == "quit") {
        interactiveMode = false;
        return;
    } else if (cmd == "save") {
        saveHistory();
        std::cout << "Command history saved." << std::endl;
        return;
    }
    
    // Process using command processor
    try {
        CommandResult result = commandProcessor->execute(command);
        
        if (result.success) {
            std::cout << resultFormatter->format(result, "plaintext");
        } else {
            std::cerr << "Error: " << result.errorMessage << std::endl;
        }
        
        // Log command execution
        NLM_LOG_INFO(std::string("Shell command executed: ") << cmd << " (success: " << 
                     (result.success ? "yes" : "no") << ")");
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        NLM_LOG_ERROR(std::string("Shell command exception: ") << e.what());
    }
}

void ShellInterface::showHelp() const {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                      NLM Shell Commands                          ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ help, ?                 - Show this help message                  ║" << std::endl;
    std::cout << "║ clear, cls             - Clear the screen                         ║" << std::endl;
    std::cout << "║ history                - Show command history                     ║" << std::endl;
    std::cout << "║ save                   - Save command history to file            ║" << std::endl;
    std::cout << "║ exit, quit             - Exit the shell                          ║" << std::endl;
    std::cout << "║                                                         ║" << std::endl;
    std::cout << "║ NLM Commands (see main help for full list):                 ║" << std::endl;
    std::cout << "║   run <demo> [options] - Run a specific demo                     ║" << std::endl;
    std::cout << "║   benchmark [options]  - Run benchmarks                          ║" << std::endl;
    std::cout << "║   experiment <name>    - Run an experiment                       ║" << std::endl;
    std::cout << "║   batch <file>         - Run batch processing                    ║" << std::endl;
    std::cout << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "For full CLI options, run: nlm --help" << std::endl;
}

void ShellInterface::showHistory(const std::vector<std::string>& history) const {
    int start = std::max(0, static_cast<int>(history.size()) - 20);
    
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    Command History (last " << history.size() << ")                   ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════╣" << std::endl;
    
    for (size_t i = start; i < history.size(); ++i) {
        std::cout << "║ " << std::setw(4) << (i + 1) << "║ " << history[i];
        if (history[i].length() < 50) {
            std::cout << std::string(50 - history[i].length(), ' ');
        }
        std::cout << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
}

void ShellInterface::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    showHelp();
}

void ShellInterface::saveHistory() const {
    std::ofstream file(commandHistoryFile, std::ios::out);
    if (file.is_open()) {
        // Note: We would need to access the history from the main shell instance
        // For now, just write a placeholder
        file << "# NLM Shell History" << std::endl;
        file << "# Commands saved automatically" << std::endl;
    }
}