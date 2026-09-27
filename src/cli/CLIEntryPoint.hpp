// Main CLI entry point
#pragma once

#include "CLIParser.hpp"
#include "CommandProcessor.hpp"
#include "ConfigCommands.hpp"
#include "RunCommand.hpp"
#include "InfoCommand.hpp"
#include "BenchmarkCommand.hpp"
#include "CheckpointCommand.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Error/Error.hpp"
#include <iostream>
#include <memory>

namespace nlm {
namespace cli {

class CLIEntryPoint {
public:
    static int run(int argc, char** argv) {
        try {
            // Create parser
            auto parser = CLIParser::create(argc, argv);
            
            // Create command processor
            auto processor = std::make_shared<CommandProcessor>();
            
            // Initialize configuration
            auto config = std::make_shared<Config>();
            
            // Load default configuration
            config->loadFromFile("configs/default.cfg");
            
            // Override with command line arguments
            config->loadFromArgs(argc, argv);
            
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Initialize error reporter
            auto errorReporter = std::make_shared<ErrorReporter>();
            ErrorReporter::setGlobal(errorReporter);
            
            // Set up command processor
            processor->setLogger(logger);
            processor->setConfig(config);
            
            // Register all commands
            registerConfigCommands(*processor);
            registerRunCommand(*processor);
            registerInfoCommands(*processor);
            registerBenchmarkCommands(*processor);
            registerCheckpointCommands(*processor);
            
            // Parse and execute command
            auto command = parser->parse(argc, argv);
            
            if (command) {
                return processor->execute(*command).exitCode;
            }
            
            return 0;
            
        } catch (const Error& e) {
            ErrorReporter::getGlobal().logError(std::string("Configuration error: ") + e.what());
            return 1;
        } catch (const std::exception& e) {
            ErrorReporter::getGlobal().logError(std::string("Error: ") + e.what());
            return 1;
        }
    }
};

} // namespace cli
} // namespace nlm
