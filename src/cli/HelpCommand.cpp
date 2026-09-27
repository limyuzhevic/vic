#include "cli/HelpCommand.hpp"
#include <iostream>

using namespace nlm;
namespace cli {

HelpCommand::HelpCommand() 
    : CommandBase("help", "Show help for commands or specific command") {
}

bool HelpCommand::execute(const std::map<std::string, std::string>& args) {
    std::string command = args.at("command");
    
    if (command == "help" || command.empty()) {
        NLM_LOG_INFO("CLI", "Available commands:");
        NLM_LOG_INFO("CLI", "  run        - Run a simulation with specified configuration");
        NLM_LOG_INFO("CLI", "  experiment - Run a specific experiment (Phase 3, 4, 5, 6)");
        NLM_LOG_INFO("CLI", "  benchmark  - Run performance benchmarks");
        NLM_LOG_INFO("CLI", "  config     - Configuration management (create, validate, view)");
        NLM_LOG_INFO("CLI", "  visualize  - Create visualizations of brain activity");
        NLM_LOG_INFO("CLI", "  analyze    - Analyze simulation results");
        NLM_LOG_INFO("CLI", "  help       - Show help for specific commands");
        return true;
    } else {
        NLM_LOG_INFO("CLI", "Help for command: " + command);
        // In a real implementation, this would show command-specific help
        return true;
    }
}

void HelpCommand::printHelp() const {
    std::cout << "Usage: nlm-cli help [command]" << std::endl;
    std::cout << std::endl;
    std::cout << "Show help information." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "  [command]   Command name to show help for" << std::endl;
}

} // namespace cli
} // namespace nlm
