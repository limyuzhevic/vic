#include "cli/command/help/HelpCommand.hpp"
#include "cli/CLIManager.hpp"
#include <iostream>

using namespace nlm::cli;

int HelpCommand::execute(const std::vector<std::string>& args) {
    std::string command = "";
    if (!args.empty()) {
        command = args[0];
    }
    
    CLIManager cliManager;
    cliManager.printHelp("NLM Phase 2", command, "");
    
    return 0;
}
