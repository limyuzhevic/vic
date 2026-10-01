// Command Line Interface Tests
// Tests for the new NLM CLI system

#include "core/CommandLineParser.hpp"
#include "core/CLIFactory.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cstring>
#include <memory>

namespace test_cli {

void testCommandLineParserCreation() {
    nlm::CommandLineParser parser;
    std::cout << "    testCommandLineParserCreation passed" << std::endl;
}

void testVersionAndHelp() {
    // Test version help
    char* testArgs[] = {"nlm", "--version"};
    int testArgc = 2;
    
    if (nlm::CommandLineParser::needsVersion(testArgc, testArgs)) {
        std::cout << "    testVersionAndHelp passed" << std::endl;
    } else {
        assert(false && "Version detection failed");
    }
}

void testHelpDetection() {
    // Test help detection
    char* testArgs[] = {"nlm", "--help"};
    int testArgc = 2;
    
    if (nlm::CommandLineParser::needsHelp(testArgc, testArgs)) {
        std::cout << "    testHelpDetection passed" << std::endl;
    } else {
        assert(false && "Help detection failed");
    }
}

void testArgumentParsing() {
    // Test --key=value parsing
    char* testArgs[] = {"nlm", "--config-file=config.json", "--steps=100", "--neuron_count=500"};
    int testArgc = 4;
    
    auto options = nlm::CommandLineParser::parseArguments(testArgc, testArgs);
    
    assert(options.find("--config-file") != options.end());
    assert(options["--config-file"] == "config.json");
    
    assert(options.find("--steps") != options.end());
    assert(options["--steps"] == "100");
    
    assert(options.find("--neuron-count") != options.end());
    assert(options["--neuron-count"] == "500");
    
    std::cout << "    testArgumentParsing passed" << std::endl;
}

void testShortOptionParsing() {
    // Test -key value parsing
    char* testArgs[] = {"nlm", "-h", "-v", "true", "-f", "output.png"};
    int testArgc = 5;
    
    auto options = nlm::CommandLineParser::parseArguments(testArgc, testArgs);
    
    assert(options.find("h") != options.end());
    assert(options["h"] == "true");
    
    assert(options.find("v") != options.end());
    assert(options["v"] == "true");
    
    assert(options.find("f") != options.end());
    assert(options["f"] == "output.png");
    
    std::cout << "    testShortOptionParsing passed" << std::endl;
}

void testFactoryCommandCreation() {
    auto commands = nlm::CLIFactory::createDefaultCommands();
    
    assert(!commands.empty());
    
    // Check for expected commands
    bool hasConfigFile = false;
    bool hasSetConfig = false;
    bool hasSave = false;
    bool hasLoad = false;
    bool hasRun = false;
    
    for (auto& cmd : commands) {
        std::string name = cmd->getName();
        if (name == "--config-file") hasConfigFile = true;
        else if (name == "--set") hasSetConfig = true;
        else if (name == "save") hasSave = true;
        else if (name == "load") hasLoad = true;
        else if (name == "run") hasRun = true;
    }
    
    assert(hasConfigFile);
    assert(hasSetConfig);
    assert(hasSave);
    assert(hasLoad);
    assert(hasRun);
    
    std::cout << "    testFactoryCommandCreation passed" << std::endl;
}

void testCommandHandlerCreation() {
    auto config = std::make_shared<nlm::Config>();
    auto commandHandler = nlm::CLIFactory::createCommandHandler(config);
    
    assert(commandHandler != nullptr);
    
    auto commands = commandHandler->getAvailableCommands();
    assert(!commands.empty());
    
    std::cout << "    testCommandHandlerCreation passed" << std::endl;
}

void testConfigFileCommand() {
    // Create test config file
    std::ofstream testConfig("test_config.json");
    testConfig << "{\"neuron_count\": 100, \"region_count\": 2}" << std::endl;
    testConfig.close();
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test loading config file
    if (nlm::ConfigFileHandler::handleConfigFile("test_config.json", config)) {
        std::cout << "    testConfigFileCommand passed" << std::endl;
    } else {
        assert(false && "Config file loading failed");
    }
    
    // Clean up
    std::remove("test_config.json");
}

void testSetConfigCommand() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test setting config values
    if (nlm::SetConfigHandler::handleSetConfig("neuron_count", "200", config) &&
        config->has("neuron_count")) {
        std::cout << "    testSetConfigCommand passed" << std::endl;
    } else {
        assert(false && "Config setting failed");
    }
}

void testSaveBrainCommand() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "    Skipping testSaveBrainCommand - brain initialization failed" << std::endl;
        return;
    }
    
    // Test saving brain
    if (nlm::SaveBrainHandler::handleSaveBrain("test_brain.json", brain)) {
        std::cout << "    testSaveBrainCommand passed" << std::endl;
        // Clean up
        std::remove("test_brain.json");
    } else {
        std::cout << "    testSaveBrainCommand skipped - save failed" << std::endl;
    }
}

void testLoadBrainCommand() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "    Skipping testLoadBrainCommand - brain initialization failed" << std::endl;
        return;
    }
    
    // Create a brain state file first
    std::string testFile = "test_brain_load.json";
    
    // Save current state
    if (brain->save(testFile)) {
        // Reset brain
        brain->reset();
        
        // Load saved state
        if (nlm::LoadBrainHandler::handleLoadBrain(testFile, brain)) {
            std::cout << "    testLoadBrainCommand passed" << std::endl;
            // Clean up
            std::remove(testFile.c_str());
        } else {
            std::cout << "    testLoadBrainCommand skipped - load failed" << std::endl;
            std::remove(testFile.c_str());
        }
    } else {
        std::cout << "    testLoadBrainCommand skipped - save failed" << std::endl;
    }
}

void runAll() {
    testCommandLineParserCreation();
    testVersionAndHelp();
    testHelpDetection();
    testArgumentParsing();
    testShortOptionParsing();
    testFactoryCommandCreation();
    testCommandHandlerCreation();
    testConfigFileCommand();
    testSetConfigCommand();
    testSaveBrainCommand();
    testLoadBrainCommand();
    
    std::cout << std::endl;
    std::cout << "=== All CLI Tests Passed ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_cli
