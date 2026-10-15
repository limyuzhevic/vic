// Debugging and testing example
#include "src/debug/Debugger.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <chrono>

void printHelp() {
    std::cout << "Debugging and Testing Example" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help     - Show this help message" << std::endl;
    std::cout << "  debug    - Enable debugging" << std::endl;
    std::cout << "  trace    - Enable tracing" << std::endl;
    std::cout << "  profile  - Enable profiling" << std::endl;
    std::cout << "  validate - Validate configuration" << std::endl;
    std::cout << "  checkpoint - Create checkpoint" << std::endl;
    std::cout << "  restore  - Restore from checkpoint" << std::endl;
    std::cout << "  quit     - Exit debugging session" << std::endl;
}

void runDebugSession(nlm::Debugger& debugger, std::shared_ptr<nlm::Brain> brain) {
    std::string command;
    std::cout << "Debug session started. Type 'help' for available commands." << std::endl;
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command == "help") {
            printHelp();
        } else if (command == "debug") {
            debugger.enableDebug(true);
            std::cout << "Debugging enabled." << std::endl;
        } else if (command == "trace") {
            debugger.enableTrace(true);
            std::cout << "Tracing enabled." << std::endl;
        } else if (command == "profile") {
            debugger.enableProfile(true);
            std::cout << "Profiling enabled." << std::endl;
        } else if (command == "validate") {
            if (brain->validateConfig()) {
                std::cout << "Configuration is valid." << std::endl;
            } else {
                std::cout << "Configuration is invalid." << std::endl;
                std::cout << "Validation errors:" << std::endl;
                std::vector<std::string> errors = brain->getValidationErrors();
                for (const auto& error : errors) {
                    std::cout << "  - " << error << std::endl;
                }
            }
        } else if (command == "checkpoint") {
            std::string filename = "checkpoint_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".bin";
            brain->saveCheckpoint(filename);
            std::cout << "Checkpoint created: " << filename << std::endl;
        } else if (command == "restore") {
            std::string filename;
            std::cout << "Enter checkpoint filename: ";
            std::cin >> filename;
            if (brain->loadCheckpoint(filename)) {
                std::cout << "Checkpoint restored: " << filename << std::endl;
            } else {
                std::cout << "Failed to restore checkpoint: " << filename << std::endl;
            }
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            printHelp();
        }
    }
}

int main() {
    std::cout << "Debugging and Testing Example" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("plasticity_learning_rate", 0.01);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Create debugger
    nlm::Debugger debugger;
    
    // Start debug session
    runDebugSession(debugger, brain);
    
    std::cout << "Debug session completed." << std::endl;
    return 0;
}