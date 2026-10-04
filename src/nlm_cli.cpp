// NLM - Neural Learning Machine - Unified Command Line Interface
// Provides a comprehensive interface for running experiments, demos, and configuration

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

// Forward declarations
class NLMCommandHandler;

// Command structure
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    bool requiresConfig;
    bool requiresBrain;
    std::function<void(NLMCommandHandler&)> handler;
};

// Command handler interface
class NLMCommandHandler {
public:
    virtual ~NLMCommandHandler() = default;
    
    virtual void handleCommand(const std::vector<std::string>& args) = 0;
    virtual void printHelp() = 0;
    virtual bool hasValidState() const = 0;
    
protected:
    // Common utility functions
    void logInfo(const std::string& message) const {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void logError(const std::string& message) const {
        std::cout << "[ERROR] " << message << std::endl;
    }
    
    void logWarning(const std::string& message) const {
        std::cout << "[WARNING] " << message << std::endl;
    }
};

// Main NLM CLI application
class NLMApplication {
public:
    NLMApplication() {
        initializeCommands();
    }
    
    void run(int argc, char** argv) {
        if (argc < 2) {
            printWelcome();
            printMainHelp();
            return;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args(argv + 2, argv + argc);
        
        executeCommand(command, args);
    }
    
private:
    std::map<std::string, Command> commands_;
    std::shared_ptr<NLMCommandHandler> currentHandler_;
    
    void initializeCommands() {
        // Main commands
        commands_["help"] = {
            "help",
            "Show help information for all commands",
            "nlm help [command]",
            false,
            false,
            [this](NLMCommandHandler& handler) {
                if (!args.empty()) {
                    // Show specific command help
                    handler.printHelp();
                } else {
                    printMainHelp();
                }
            }
        };
        
        // Demo commands
        commands_["demo"] = {
            "Run demo mode",
            "Run NLM in demo mode",
            "nlm demo [phase] [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        // Experiment commands
        commands_["experiment"] = {
            "Run experiment",
            "Run NLM in experiment mode",
            "nlm experiment [experiment_type] [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        // Configuration commands
        commands_["config"] = {
            "Manage configuration",
            "Manage NLM configuration files",
            "nlm config [action] [key] [value]",
            false,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        // Benchmark commands
        commands_["benchmark"] = {
            "Run benchmark",
            "Run NLM performance benchmarks",
            "nlm benchmark [options]",
            false,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        // Phase-specific commands
        commands_["phase2"] = {
            "Run Phase 2 demo",
            "Run NLM Phase 2 neural computation demo",
            "nlm phase2 [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        commands_["phase3"] = {
            "Run Phase 3 demo",
            "Run NLM Phase 3 world interaction demo",
            "nlm phase3 [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        commands_["phase4"] = {
            "Run Phase 4 demo",
            "Run NLM Phase 4 cognition demo",
            "nlm phase4 [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        commands_["phase6"] = {
            "Run Phase 6 demo",
            "Run NLM Phase 6 integration demo",
            "nlm phase6 [config_file]",
            true,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
        
        // Utility commands
        commands_["version"] = {
            "Show version information",
            "Show NLM version and build information",
            "nlm version",
            false,
            false,
            [this](NLMCommandHandler& handler) {
                std::cout << "NLM - Neural Learning Machine" << std::endl;
                std::cout << "Version: 0.1.0" << std::endl;
                std::cout << "Phase 6: Final Integration" << std::endl;
                std::cout << "Built with C++20" << std::endl;
            }
        };
        
        commands_["status"] = {
            "Show system status",
            "Show NLM system status and configuration",
            "nlm status",
            false,
            false,
            [this](NLMCommandHandler& handler) {
                // Implementation will be added
                handler.printHelp();
            }
        };
    }
    
    void executeCommand(const std::string& command, const std::vector<std::string>& args) {
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        auto it = commands_.find(command);
        if (it == commands_.end()) {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Use 'nlm help' for available commands." << std::endl;
            return;
        }
        
        Command& cmd = it->second;
        
        // Check if command is available
        std::cout << "Command: " << cmd.name << std::endl;
        std::cout << "Description: " << cmd.description << std::endl;
        
        // TODO: Implement proper command execution
        std::cout << "Command execution not yet implemented." << std::endl;
        std::cout << "Usage: " << cmd.usage << std::endl;
    }
    
    void printWelcome() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║     NLM — 熙然                                                 ║" << std::endl;
        std::cout << "║     Neural Learning Machine                                    ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║     Phase 6: Final Integration                                 ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
        std::cout << std::endl;
    }
    
    void printMainHelp() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                     Available Commands                          ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        
        for (const auto& pair : commands_) {
            std::cout << "║ " << std::setw(25) << std::left << pair.first 
                      << " - " << pair.second.description << " ║" << std::endl;
        }
        
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Use 'nlm help <command>' for detailed information about a  ║" << std::endl;
        std::cout << "║ specific command.                                           ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    }
};

int main(int argc, char** argv) {
    NLMApplication app;
    app.run(argc, argv);
    
    return 0;
}
