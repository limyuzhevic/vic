// NLM Main CLI Entry Point
// Unified command-line interface for NLM subcommands

#include "nlm/CliCommand.hpp"
#include "nlm/Subcommands/RunCommand.hpp"
#include "nlm/Subcommands/AnalyzeCommand.hpp"
#include "nlm/Subcommands/ExportCommand.hpp"
#include "nlm/Subcommands/VisualizeCommand.hpp"
#include "nlm/Subcommands/ConfigCommand.hpp"
#include "nlm/Subcommands/ToolsCommand.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

namespace nlm {
    
    class NLMCLI {
    public:
        int run(int argc, char* argv[]) {
            if (argc < 2) {
                CommandRegistry::getInstance().printHelp(std::cout);
                return 0;
            }
            
            std::string commandName = argv[1];
            std::vector<std::string> args;
            
            // Collect remaining arguments
            for (int i = 2; i < argc; ++i) {
                args.push_back(argv[i]);
            }
            
            // Check for help flag
            bool showHelp = false;
            for (const auto& arg : args) {
                if (arg == "--help" || arg == "-h") {
                    showHelp = true;
                    break;
                }
            }
            
            CommandPtr command = CommandRegistry::getInstance().getCommand(commandName);
            if (!command) {
                std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
                std::cerr << std::endl;
                CommandRegistry::getInstance().printHelp(std::cerr);
                return 1;
            }
            
            if (showHelp) {
                std::string help = command->getHelp();
                if (!help.empty()) {
                    std::cout << help << std::endl;
                } else {
                    std::cout << "Usage: nlm " << commandName << " [options] [args]" << std::endl;
                }
                return 0;
            }
            
            return command->execute(args);
        }
    };
}

int main(int argc, char* argv[]) {
    nlm::NLMCLI cli;
    return cli.run(argc, argv);
}