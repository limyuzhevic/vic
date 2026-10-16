#include "NLMCLI.hpp"
#include "Commands/RunCommand.hpp"
#include "Commands/ConfigCommand.hpp"
#include "Commands/ExperimentCommand.hpp"
#include "Commands/VisualizeCommand.hpp"
#include "Commands/BenchmarkCommand.hpp"
#include "Commands/StatusCommand.hpp"
#include "Commands/HelpCommand.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char** argv) {
    try {
        // Parse global arguments
        CLIConfig config = CLIArgumentParser::parseGlobalArguments(argc, argv);
        
        // Parse command and arguments
        std::string commandName;
        std::vector<std::string> commandArgs = CLIArgumentParser::parseCommandArguments(argc, argv, commandName);
        
        // Get command runner and register commands
        auto& commandRunner = getCommandRunner();
        
        // Register all commands
        static RunCommand runCmd;
        static ConfigCommand configCmd;
        static ExperimentCommand experimentCmd;
        static VisualizeCommand visualizeCmd;
        static BenchmarkCommand benchmarkCmd;
        static StatusCommand statusCmd;
        static HelpCommand helpCmd;
        
        commandRunner.registerCommand(std::make_unique<RunCommand>(runCmd));
        commandRunner.registerCommand(std::make_unique<ConfigCommand>(configCmd));
        commandRunner.registerCommand(std::make_unique<ExperimentCommand>(experimentCmd));
        commandRunner.registerCommand(std::make_unique<VisualizeCommand>(visualizeCmd));
        commandRunner.registerCommand(std::make_unique<BenchmarkCommand>(benchmarkCmd));
        commandRunner.registerCommand(std::make_unique<StatusCommand>(statusCmd));
        commandRunner.registerCommand(std::make_unique<HelpCommand>(helpCmd));
        
        // Handle global options
        if (config.help) {
            CLIArgumentParser::printHelp(config, commandRunner);
            return 0;
        }
        
        if (config.version) {
            CLIArgumentParser::printVersion();
            return 0;
        }
        
        // Execute command
        CommandResult result;
        if (commandName.empty() || commandName == "help") {
            // Show general help
            CLIArgumentParser::printHelp(config, commandRunner);
            result.success = true;
            result.exitCode = 0;
        } else {
            result = commandRunner.executeCommand(commandName, commandArgs, config);
            
            // Print output based on configuration
            if (result.success) {
                std::cout << result.output;
                if (config.verbose && !result.output.empty()) {
                    std::cout << "Execution time: " << std::fixed << std::setprecision(3) 
                             << result.executionTime << "s" << std::endl;
                }
            } else {
                std::cout << OutputFormatter::formatError(result.message, "Command execution");
                if (!result.output.empty()) {
                    std::cout << "Details: " << result.output << std::endl;
                }
            }
        }
        
        return result.exitCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}