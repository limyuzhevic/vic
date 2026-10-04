// NLM CLI - Neural Learning Machine Command Line Interface
// Phase 3: Comprehensive CLI with Subcommands and Options

#include "cli/CLIEngine.hpp"
#include "cli/CLICommand.hpp"
#include "cli/CLIOptions.hpp"
#include "cli/CLIBasicTypes.hpp"
#include "cli/CLIGroup.hpp"
#include "cli/LoadCommand.hpp"
#include "cli/SaveCommand.hpp"
#include "cli/RunCommand.hpp"
#include "cli/ValidateCommand.hpp"
#include "cli/StatusCommand.hpp"
#include "cli/ExperimentCommand.hpp"
#include "cli/DebugCommand.hpp"
#include "cli/ProfileCommand.hpp"
#include "cli/ConfigCommand.hpp"
#include "cli/HelpCommand.hpp"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    // Create and configure CLI engine
    nlm::CLIEngine cliEngine;
    
    // Set program name
    cliEngine.setProgramName("nlm");
    
    // Register command groups
    registerCommandGroups(cliEngine);
    
    // Parse command line arguments
    if (!cliEngine.parse(argc, argv)) {
        return 1;
    }
    
    // Execute the command
    return cliEngine.execute();
}

void registerCommandGroups(nlm::CLIEngine& cliEngine) {
    // Create core command group
    auto coreGroup = std::make_shared<nlm::CLIGroup>(
        "Core Commands",
        "Core NLM commands for basic operations",
        "1.0",
        "NLM Development Team",
        "support@nlm.org"
    );
    
    // Add core commands to core group
    coreGroup->addCommand(std::make_shared<nlm::LoadCommand>());
    coreGroup->addCommand(std::make_shared<nlm::SaveCommand>());
    coreGroup->addCommand(std::make_shared<nlm::RunCommand>());
    coreGroup->addCommand(std::make_shared<nlm::ValidateCommand>());
    coreGroup->addCommand(std::make_shared<nlm::StatusCommand>());
    
    // Create experiment command group
    auto experimentGroup = std::make_shared<nlm::CLIGroup>(
        "Experiment Commands",
        "Commands for running and managing experiments",
        "1.0",
        "NLM Development Team",
        "support@nlm.org"
    );
    
    experimentGroup->addCommand(std::make_shared<nlm::ExperimentCommand>());
    
    // Create debugging and analysis command group
    auto debugGroup = std::make_shared<nlm::CLIGroup>(
        "Debug and Analysis",
        "Debugging and performance analysis tools",
        "1.0",
        "NLM Development Team",
        "support@nlm.org"
    );
    
    debugGroup->addCommand(std::make_shared<nlm::DebugCommand>());
    debugGroup->addCommand(std::make_shared<nlm::ProfileCommand>());
    
    // Create configuration command group
    auto configGroup = std::make_shared<nlm::CLIGroup>(
        "Configuration",
        "Configuration management and manipulation",
        "1.0",
        "NLM Development Team",
        "support@nlm.org"
    );
    
    configGroup->addCommand(std::make_shared<nlm::ConfigCommand>());
    
    // Create help command group
    auto helpGroup = std::make_shared<nlm::CLIGroup>(
        "Help",
        "Help and documentation",
        "1.0",
        "NLM Development Team",
        "support@nlm.org"
    );
    
    helpGroup->addCommand(std::make_shared<nlm::HelpCommand>());
    
    // Register all groups with the CLI engine
    cliEngine.registerGroup(coreGroup);
    cliEngine.registerGroup(experimentGroup);
    cliEngine.registerGroup(debugGroup);
    cliEngine.registerGroup(configGroup);
    cliEngine.registerGroup(helpGroup);
}