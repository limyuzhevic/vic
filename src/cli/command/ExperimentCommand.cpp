#include "cli/command/experiment/ExperimentCommand.hpp"
#include <iostream>

using namespace nlm::cli;

int ExperimentCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Experiment Commands:" << std::endl;
        std::cout << "  run <name>        Run specific experiment" << std::endl;
        std::cout << "  list               List available experiments" << std::endl;
        std::cout << "  help               Show this help message" << std::endl;
        return 0;
    }
    
    std::string action = args[0];
    
    if (action == "run") {
        if (args.size() < 2) {
            std::cout << "Usage: nlm experiment run <name>" << std::endl;
            return 1;
        }
        std::string name = args[1];
        std::cout << "Running experiment: " << name << std::endl;
        
    } else if (action == "list") {
        std::cout << "Listing experiments..." << std::endl;
        
    } else if (action == "help") {
        std::cout << "Usage: nlm experiment <action> [--help]" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  run <name>        Run specific experiment" << std::endl;
        std::cout << "  list               List available experiments" << std::endl;
        std::cout << "  help               Show this help message" << std::endl;
        return 0;
        
    } else {
        std::cout << "Error: Unknown experiment action: " << action << std::endl;
        return 1;
    }
    
    return 0;
}
