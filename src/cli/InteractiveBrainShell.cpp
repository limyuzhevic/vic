#include "InteractiveBrainShell.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {
namespace cli {

InteractiveBrainShell::InteractiveBrainShell() : running(false), stopRequested(false) {
    currentPath = ".";
}

InteractiveBrainShell::~InteractiveBrainShell() {
    Shutdown();
}

bool InteractiveBrainShell::Initialize() {
    try {
        // Simple initialization
        std::cout << "Initializing Interactive Brain Shell...\n";
        std::cout << "Type 'help' for available commands\n";
        running = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing shell: " << e.what() << std::endl;
        return false;
    }
}

void InteractiveBrainShell::Run() {
    try {
        while (running && !stopRequested) {
            DisplayPrompt();
            
            std::string commandLine;
            std::getline(std::cin, commandLine);
            
            if (commandLine.empty()) {
                continue;
            }
            
            commandHistory.push_back(commandLine);
            
            if (!ProcessCommand(commandLine)) {
                std::cout << "Error executing command\n";
            }
            
            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in shell: " << e.what() << std::endl;
    }
}

void InteractiveBrainShell::Shutdown() {
    running = false;
    stopRequested = true;
    std::cout << "\nShutting down Interactive Brain Shell...\n";
}

void InteractiveBrainShell::RegisterCommand(const std::string& name, const std::string& description,
                                        std::function<bool(const std::vector<std::string>&)> handler) {
    commandMap[name] = handler;
    std::cout << "Registered command: " << name << " - " << description << "\n";
}

bool InteractiveBrainShell::ExecuteCommand(const std::string& commandLine) {
    std::vector<std::string> args = ParseCommandLine(commandLine);
    if (args.empty()) {
        return false;
    }
    
    std::string command = args[0];
    args.erase(args.begin());
    
    auto it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second(args);
    }
    
    std::cout << "Unknown command: " << command << "\n";
    std::cout << "Type 'help' for available commands\n";
    return false;
}

void InteractiveBrainShell::DisplayPrompt() const {
    std::cout << "brain> ";
    std::cout.flush();
}

void InteractiveBrainShell::DisplayHelp() const {
    std::cout << "\nBrain Interactive Shell Commands:\n";
    std::cout << "=============================\n\n";
    
    std::cout << "Basic Commands:\n";
    std::cout << "  help                    Show this help message\n";
    std::cout << "  exit                    Exit the shell\n";
    std::cout << "  history                 Show command history\n\n";
    
    std::cout << "Brain Operations:\n";
    std::cout << "  brain analyze <path>    Analyze brain at specified path\n";
    std::cout << "  brain create <path>     Create a new brain\n";
    std::cout << "  brain visualize <path>  Generate visualization for brain\n";
    std::cout << "  brain benchmark          Run performance benchmark\n\n";
    
    std::cout << "Configuration:\n";
    std::cout << "  config set <key> <value>   Set configuration value\n";
    std::cout << "  config get <key>            Get configuration value\n";
    std::cout << "  config show                 Show current configuration\n\n";
    
    std::cout << "File Operations:\n";
    std::cout << "  load <path>              Load brain from file\n";
    std::cout << "  save <path>              Save brain to file\n";
    std::cout << "  export <format> <path>    Export brain in specified format\n";
    std::cout << "  import <format> <path>    Import brain from file\n\n";
    
    std::cout << "Processing:\n";
    std::cout << "  batch <files>             Process multiple files\n";
    std::cout << "  script <script>           Execute brain script\n";
    std::cout << "  monitor <duration>        Monitor performance for duration\n\n";
}

bool InteractiveBrainShell::ProcessCommand(const std::string& commandLine) {
    // Simple command processing for demonstration
    if (commandLine == "help") {
        DisplayHelp();
        return true;
    } else if (commandLine == "exit") {
        Shutdown();
        return true;
    } else if (commandLine == "history") {
        PrintHistory();
        return true;
    } else if (commandLine.find("brain ") == 0) {
        // Brain operations
        std::string args = commandLine.substr(6); // Remove "brain "
        if (args.find("analyze ") == 0) {
            std::string path = args.substr(8); // Remove "analyze "
            std::cout << "Analyzing brain: " << path << "\n";
            // In a real implementation, we would analyze the brain
            return true;
        } else if (args.find("create ") == 0) {
            std::string path = args.substr(7); // Remove "create "
            std::cout << "Creating brain at: " << path << "\n";
            // In a real implementation, we would create the brain
            return true;
        } else if (args.find("visualize ") == 0) {
            std::string path = args.substr(10); // Remove "visualize "
            std::cout << "Visualizing brain: " << path << "\n";
            // In a real implementation, we would generate visualization
            return true;
        } else if (args == "benchmark") {
            std::cout << "Running performance benchmark...\n";
            // In a real implementation, we would run benchmark
            return true;
        }
    } else if (commandLine.find("config ") == 0) {
        // Configuration operations
        std::string args = commandLine.substr(7); // Remove "config "
        if (args.find("set ") == 0) {
            std::string keyValue = args.substr(4); // Remove "set "
            size_t spacePos = keyValue.find(' ');
            if (spacePos != std::string::npos) {
                std::string key = keyValue.substr(0, spacePos);
                std::string value = keyValue.substr(spacePos + 1);
                std::cout << "Setting config: " << key << " = " << value << "\n";
                return true;
            }
        } else if (args.find("get ") == 0) {
            std::string key = args.substr(4); // Remove "get "
            std::cout << "Getting config for key: " << key << "\n";
            return true;
        } else if (args == "show") {
            std::cout << "Current configuration:\n";
            std::cout << "  config.path: ./config.json\n";
            std::cout << "  output.path: ./output\n";
            std::cout << "  log.level: INFO\n";
            return true;
        }
    } else if (commandLine.find("load ") == 0) {
        // Load brain
        std::string path = commandLine.substr(5); // Remove "load "
        std::cout << "Loading brain from: " << path << "\n";
        return true;
    } else if (commandLine.find("save ") == 0) {
        // Save brain
        std::string path = commandLine.substr(5); // Remove "save "
        std::cout << "Saving brain to: " << path << "\n";
        return true;
    } else if (commandLine.find("export ") == 0) {
        // Export brain
        std::string args = commandLine.substr(7); // Remove "export "
        size_t spacePos = args.find(' ');
        if (spacePos != std::string::npos) {
            std::string format = args.substr(0, spacePos);
            std::string path = args.substr(spacePos + 1);
            std::cout << "Exporting brain to " << format << " format: " << path << "\n";
            return true;
        }
    } else if (commandLine.find("import ") == 0) {
        // Import brain
        std::string args = commandLine.substr(7); // Remove "import "
        size_t spacePos = args.find(' ');
        if (spacePos != std::string::npos) {
            std::string format = args.substr(0, spacePos);
            std::string path = args.substr(spacePos + 1);
            std::cout << "Importing brain from " << format << " format: " << path << "\n";
            return true;
        }
    } else if (commandLine.find("batch ") == 0) {
        // Batch processing
        std::string files = commandLine.substr(6); // Remove "batch "
        std::cout << "Processing batch files: " << files << "\n";
        return true;
    } else if (commandLine.find("script ") == 0) {
        // Script execution
        std::string script = commandLine.substr(7); // Remove "script "
        std::cout << "Executing script: " << script << "\n";
        return true;
    } else if (commandLine.find("monitor ") == 0) {
        // Performance monitoring
        std::string durationStr = commandLine.substr(8); // Remove "monitor "
        try {
            double duration = std::stod(durationStr);
            std::cout << "Monitoring performance for " << duration << " seconds...\n";
            return true;
        } catch (...) {
            std::cout << "Invalid duration: " << durationStr << "\n";
            return false;
        }
    }
    
    std::cout << "Unknown command: " << commandLine << "\n";
    return false;
}

void InteractiveBrainShell::PrintHistory() const {
    std::cout << "\nCommand History:\n";
    std::cout << "==============\n";
    for (size_t i = 0; i < commandHistory.size(); ++i) {
        std::cout << i+1 << ": " << commandHistory[i] << "\n";
    }
}

std::vector<std::string> InteractiveBrainShell::ParseCommandLine(const std::string& commandLine) const {
    std::vector<std::string> args;
    std::stringstream ss(commandLine);
    std::string token;
    
    while (ss >> token) {
        args.push_back(token);
    }
    
    return args;
}

} // namespace cli
} // namespace nlm