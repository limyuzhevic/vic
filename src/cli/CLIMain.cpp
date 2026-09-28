#include "cli/AdvancedCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

namespace nlm {

class ConsoleLogger;

} // namespace nlm

int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Create and run AdvancedCLI
    nlm::AdvancedCLI cli;
    
    nlm::CLIStatus result = cli.parseArguments(argc, argv);
    
    if (result == nlm::CLIStatus::HelpRequested) {
        cli.displayHelp("nlm_cli_main");
        return 0;
    } else if (result == nlm::CLIStatus::VersionRequested) {
        cli.displayVersion("nlm_cli_main");
        return 0;
    } else if (result == nlm::CLIStatus::Error) {
        std::cerr << "Error parsing arguments." << std::endl;
        cli.displayHelp("nlm_cli_main");
        return 1;
    }
    
    return cli.execute() == nlm::CLIStatus::Success ? 0 : 1;
}