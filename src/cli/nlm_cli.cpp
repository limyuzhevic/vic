#include "nlm_cli.h"
#include "CLIContext.hpp"
#include "ConfigCommand.hpp"
#include "BrainCommand.hpp"
#include "SimulationCommand.hpp"
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdexcept>

namespace nlm {

void printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

int main(int argc, char** argv) {
    try {
        CLIContext context;
        std::string command;
        std::map<std::string, std::string> options;
        std::vector<std::string> args;
        
        if (!context.parseArgs(argc, argv, command, options, args)) {
            printError("Failed to parse arguments");
            return 1;
        }
        
        if (command == "help") {
            context.printUsage("nlm-cli");
            return 0;
        }
        
        if (command == "version") {
            std::cout << "NLM CLI Tool v0.1.0" << std::endl;
            return 0;
        }
        
        printInfo("Executing command: " + command);
        
        bool result = false;
        
        if (command == "config") {
            if (args.size() < 1) {
                printError("Config command requires subcommand (list, show, set, create, validate)");
                context.printUsage("nlm-cli");
                return 1;
            }
            
            std::string subCommand = args[0];
            if (subCommand == "list") {
                result = ConfigCommand::executeList(context, options);
            } else if (subCommand == "show") {
                if (args.size() < 2) {
                    printError("Config show requires key parameter");
                    return 1;
                }
                result = ConfigCommand::executeShow(context, args[1], options);
            } else if (subCommand == "set") {
                if (args.size() < 2) {
                    printError("Config set requires key and value parameters");
                    return 1;
                }
                result = ConfigCommand::executeSet(context, args[1], args[2], options);
            } else if (subCommand == "create") {
                if (args.size() < 2) {
                    printError("Config create requires filename parameter");
                    return 1;
                }
                result = ConfigCommand::executeCreate(context, args[1], options);
            } else if (subCommand == "validate") {
                if (args.size() < 2) {
                    printError("Config validate requires filename parameter");
                    return 1;
                }
                result = ConfigCommand::executeValidate(context, args[1], options);
            } else {
                printError("Unknown config subcommand: " + subCommand);
                context.printUsage("nlm-cli");
                return 1;
            }
        } else if (command == "brain") {
            if (args.size() < 1) {
                printError("Brain command requires subcommand (status, neurons, synapses, save, load)");
                context.printUsage("nlm-cli");
                return 1;
            }
            
            std::string subCommand = args[0];
            if (subCommand == "status") {
                result = BrainCommand::executeStatus(context, options);
            } else if (subCommand == "neurons") {
                result = BrainCommand::executeNeurons(context, options);
            } else if (subCommand == "synapses") {
                result = BrainCommand::executeSynapses(context, options);
            } else if (subCommand == "save") {
                if (args.size() < 2) {
                    printError("Brain save requires filename parameter");
                    return 1;
                }
                result = BrainCommand::executeSave(context, args[1], options);
            } else if (subCommand == "load") {
                if (args.size() < 2) {
                    printError("Brain load requires filename parameter");
                    return 1;
                }
                result = BrainCommand::executeLoad(context, args[1], options);
            } else {
                printError("Unknown brain subcommand: " + subCommand);
                context.printUsage("nlm-cli");
                return 1;
            }
        } else if (command == "simulate") {
            result = SimulationCommand::executeSimulate(context, options);
        } else {
            printError("Unknown command: " + command);
            context.printUsage("nlm-cli");
            return 1;
        }
        
        if (result) {
            printSuccess("Command completed successfully");
            return 0;
        } else {
            printError("Command failed");
            return 1;
        }
        
    } catch (const std::exception& e) {
        printError(std::string("Exception: ") + e.what());
        return 1;
    } catch (...) {
        printError("Unknown error occurred");
        return 1;
    }
}
