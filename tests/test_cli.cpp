#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_cli {

void testNLMCLICreation() {
    nlm::NLMCLI cli;
    assert(cli.isActive() == false);
    assert(cli.getMode() == nlm::CLIMode::Interactive);
    assert(cli.getHelpCount() == 0);
    std::cout << "    testNLMCLICreation passed" << std::endl;
}

void testCLIMode() {
    nlm::NLMCLI cli;
    
    // Test different CLI modes
    cli.setMode(nlm::CLIMode::Batch);
    assert(cli.getMode() == nlm::CLIMode::Batch);
    
    cli.setMode(nlm::CLIMode::Script);
    assert(cli.getMode() == nlm::CLIMode::Script);
    
    cli.setMode(nlm::CLIMode::Interactive);
    assert(cli.getMode() == nlm::CLIMode::Interactive);
    
    std::cout << "    testCLIMode passed" << std::endl;
}

void testCLIHelp() {
    nlm::NLMCLI cli;
    
    // Show help
    cli.showHelp();
    assert(cli.getHelpCount() == 1);
    
    // Show version
    cli.showVersion();
    assert(cli.getVersionCount() == 1);
    
    std::cout << "    testCLIHelp passed" << std::endl;
}

void testCLIExecution() {
    nlm::NLMCLI cli;
    
    // Set up CLI
    cli.setup();
    assert(cli.isActive() == true);
    
    // Execute command
    cli.executeCommand("help");
    assert(cli.getCommandCount() == 1);
    
    // Cleanup
    cli.cleanup();
    assert(cli.isActive() == false);
    std::cout << "    testCLIExecution passed" << std::endl;
}

void testCLIValidation() {
    nlm::NLMCLI cli;
    
    // Test command validation
    assert(cli.validateCommand("help") == true);
    assert(cli.validateCommand("version") == true);
    assert(cli.validateCommand("invalid_command") == false);
    
    std::cout << "    testCLIValidation passed" << std::endl;
}

void runAll() {
    std::cout << "Running CLI tests..." << std::endl;
    testNLMCLICreation();
    testCLIMode();
    testCLIHelp();
    testCLIExecution();
    testCLIValidation();
}

} // namespace test_cli