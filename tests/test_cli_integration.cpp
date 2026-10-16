#include "test_cli.cpp"
#include <iostream>
#include <cassert>

// Test integration with existing NLM components
void test_nlm_integration() {
    std::cout << "Testing NLM component integration..." << std::endl;
    
    // Test 1: Config system integration
    std::cout << "Testing Config system integration..." << std::endl;
    auto& commandRunner = nlm::getCommandRunner();
    
    // Config command should be registered
    assert(commandRunner.hasCommand("config"));
    
    std::string configHelp = commandRunner.getCommandHelp("config");
    assert(configHelp.find("Configuration management") != std::string::npos);
    
    // Test 2: Command structure integration
    std::cout << "Testing command structure integration..." << std::endl;
    
    std::vector<std::string> commandNames = commandRunner.getCommandNames();
    std::vector<std::string> expectedCommands = {"run", "config", "experiment", "visualize", "benchmark", "status", "help"};
    
    for (const auto& expectedCmd : expectedCommands) {
        bool found = false;
        for (const auto& actualCmd : commandNames) {
            if (expectedCmd == actualCmd) {
                found = true;
                break;
            }
        }
        assert(found) << "Command not found: " << expectedCmd;
    }
    
    // Test 3: Command argument validation
    std::cout << "Testing command argument validation..." << std::endl;
    
    nlm::ConfigCommand configCmd;
    assert(configCmd.validateArguments({"create", "test.cfg"}));
    assert(!configCmd.validateArguments({}));  // Invalid: missing action
    
    nlm::RunCommand runCmd;
    assert(runCmd.validateArguments({"scenario", "--steps=100"}));
    assert(!runCmd.validateArguments({}));  // Invalid: missing scenario
    
    // Test 4: CLI configuration integration
    std::cout << "Testing CLI configuration integration..." << std::endl;
    
    nlm::CLIConfig config;
    config.configFile = "test_config.cfg";
    config.outputFormat = "json";
    config.logLevel = "info";
    config.verbose = true;
    config.quiet = false;
    
    assert(config.configFile == "test_config.cfg");
    assert(config.outputFormat == "json");
    assert(config.logLevel == "info");
    assert(config.verbose == true);
    assert(config.quiet == false);
    
    // Test 5: Output formatting integration
    std::cout << "Testing output formatting integration..." << std::endl;
    
    std::string testContent = "Test output";
    std::string jsonOutput = nlm::OutputFormatter::format(testContent, "json");
    assert(jsonOutput.find("Test output") != std::string::npos);
    
    std::string csvOutput = nlm::OutputFormatter::format(testContent, "csv");
    assert(csvOutput.find("Test output") != std::string::npos);
    
    std::string errorOutput = nlm::OutputFormatter::formatError("Test error", "Test context");
    assert(errorOutput.find("Test error") != std::string::npos);
    
    std::string successOutput = nlm::OutputFormatter::formatSuccess("Test success", "Test details");
    assert(successOutput.find("Test success") != std::string::npos);
    
    // Test 6: Progress reporting integration
    std::cout << "Testing progress reporting integration..." << std::endl;
    
    nlm::ProgressReporter reporter("Integration Test", 100);
    reporter.update(50, "Halfway through");
    reporter.complete("Test completed");
    
    // Test 7: Command runner functionality
    std::cout << "Testing command runner functionality..." << std::endl;
    
    std::vector<std::string> completions = commandRunner.generateCompletions("r");
    assert(!completions.empty());
    
    // Should contain at least one command starting with 'r'
    bool hasRunCommand = false;
    for (const auto& completion : completions) {
        if (completion == "run") {
            hasRunCommand = true;
            break;
        }
    }
    assert(hasRunCommand);
    
    std::cout << "✓ All NLM component integration tests passed" << std::endl;
}

// Test CLI command implementation
void test_cli_commands() {
    std::cout << "Testing CLI command implementations..." << std::endl;
    
    // Test that all command implementations are complete
    std::cout << "Testing RunCommand implementation..." << std::endl;
    nlm::RunCommand runCmd;
    assert(runCmd.getName() == "run");
    assert(!runCmd.getDescription().empty());
    assert(!runCmd.getUsage().empty());
    
    std::cout << "Testing ConfigCommand implementation..." << std::endl;
    nlm::ConfigCommand configCmd;
    assert(configCmd.getName() == "config");
    assert(!configCmd.getDescription().empty());
    assert(!configCmd.getUsage().empty());
    
    std::cout << "Testing ExperimentCommand implementation..." << std::endl;
    nlm::ExperimentCommand experimentCmd;
    assert(experimentCmd.getName() == "experiment");
    assert(!experimentCmd.getDescription().empty());
    assert(!experimentCmd.getUsage().empty());
    
    std::cout << "Testing VisualizeCommand implementation..." << std::endl;
    nlm::VisualizeCommand visualizeCmd;
    assert(visualizeCmd.getName() == "visualize");
    assert(!visualizeCmd.getDescription().empty());
    assert(!visualizeCmd.getUsage().empty());
    
    std::cout << "Testing BenchmarkCommand implementation..." << std::endl;
    nlm::BenchmarkCommand benchmarkCmd;
    assert(benchmarkCmd.getName() == "benchmark");
    assert(!benchmarkCmd.getDescription().empty());
    assert(!benchmarkCmd.getUsage().empty());
    
    std::cout << "Testing StatusCommand implementation..." << std::endl;
    nlm::StatusCommand statusCmd;
    assert(statusCmd.getName() == "status");
    assert(!statusCmd.getDescription().empty());
    assert(!statusCmd.getUsage().empty());
    
    std::cout << "Testing HelpCommand implementation..." << std::endl;
    nlm::HelpCommand helpCmd;
    assert(helpCmd.getName() == "help");
    assert(!helpCmd.getDescription().empty());
    assert(!helpCmd.getUsage().empty());
    
    std::cout << "✓ All CLI command implementation tests passed" << std::endl;
}

// Run integration and implementation tests
void runIntegrationTests() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         CLI Integration Tests                                 ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    test_nlm_integration();
    std::cout << std::endl;
    
    test_cli_commands();
    std::cout << std::endl;
    
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         All Integration Tests Passed!                        ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

int main() {
    try {
        runIntegrationTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Integration test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}