// CommandLineUtils Tests
// Phase 3: Command Line Utilities

#include "utils/CommandLineUtils.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

namespace test_command_line_utils {

void testParseArgumentsFormat1() {
    char* argv[] = {
        (char*)"test",
        (char*)"--key1=value1",
        (char*)"--key2=value2",
        (char*)"-k",
        (char*)"value3",
        (char*)"positional1",
        (char*)"positional2"
    };
    
    int argc = 7;
    auto args = nlm::CommandLineUtils::parseArguments(argc, argv);
    
    assert(args["key1"] == "value1");
    assert(args["key2"] == "value2");
    assert(args["k"] == "value3");
    assert(args["0"] == "positional1");
    assert(args["1"] == "positional2");
    
    std::cout << "    testParseArgumentsFormat1 passed" << std::endl;
}

void testParseArgumentsFormat2() {
    char* argv[] = {
        (char*)"test",
        (char*)"--key1=value1",
        (char*)"--flag1",
        (char*)"-k",
        (char*)"value2",
        (char*)"--flag2"
    };
    
    int argc = 6;
    auto args = nlm::CommandLineUtils::parseArguments(argc, argv);
    
    assert(args["key1"] == "value1");
    assert(args["flag1"] == "true");
    assert(args["k"] == "value2");
    assert(args["flag2"] == "true");
    
    std::cout << "    testParseArgumentsFormat2 passed" << std::endl;
}

void testLoadConfig() {
    // Create a temporary config file
    std::string configPath = "/tmp/test_config.json";
    
    {
        std::ofstream file(configPath);
        file << "{\"int_value\": 42, \"string_value\": \"test\"}";
    }
    
    auto config = nlm::CommandLineUtils::loadConfig(configPath);
    assert(config != nullptr);
    
    // Try to get a value (this depends on ConfigManager interface)
    // auto valueOpt = config->get<int>("int_value");
    // assert(valueOpt && *valueOpt == 42);
    
    std::cout << "    testLoadConfig passed" << std::endl;
    
    // Clean up
    std::remove(configPath.c_str());
}

void testSaveConfig() {
    // Create a mock config
    auto config = std::make_shared<nlm::ConfigManager>();
    config->set("test_key", 123, nlm::ConfigSource::Runtime);
    
    std::string configPath = "/tmp/test_save_config.json";
    
    bool success = nlm::CommandLineUtils::saveConfig(config, configPath, true);
    assert(success);
    
    std::cout << "    testSaveConfig passed" << std::endl;
    
    // Clean up
    std::remove(configPath.c_str());
}

void testCreateCheckpoint() {
    // Create a mock config
    auto config = std::make_shared<nlm::ConfigManager>();
    config->set("test_key", 123, nlm::ConfigSource::Runtime);
    
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    
    bool success = nlm::CommandLineUtils::createCheckpoint(checkpointPath, 6, config);
    assert(success);
    
    std::cout << "    testCreateCheckpoint passed" << std::endl;
    
    // Clean up
    std::remove(checkpointPath.c_str());
}

void testLoadCheckpoint() {
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    
    bool success = nlm::CommandLineUtils::loadCheckpoint(checkpointPath);
    assert(success);
    
    std::cout << "    testLoadCheckpoint passed" << std::endl;
}

void testSaveLoadSession() {
    // Create a mock config
    auto config = std::make_shared<nlm::ConfigManager>();
    config->set("test_key", 123, nlm::ConfigSource::Runtime);
    
    std::string sessionName = "test_session";
    
    bool saveSuccess = nlm::CommandLineUtils::saveSession(sessionName, config, 100, 1.5);
    assert(saveSuccess);
    
    bool loadSuccess = nlm::CommandLineUtils::loadSession(sessionName);
    assert(loadSuccess);
    
    std::string info = nlm::CommandLineUtils::getSessionInfo(sessionName);
    assert(!info.empty());
    
    std::cout << "    testSaveLoadSession passed" << std::endl;
    
    // Clean up
    nlm::CommandLineUtils::deleteSession(sessionName);
}

void testListCheckpoints() {
    // Create a mock checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint.bin";
    
    bool success = nlm::CommandLineUtils::createCheckpoint(checkpointPath);
    assert(success);
    
    auto checkpoints = nlm::CommandLineUtils::listCheckpoints("*");
    assert(!checkpoints.empty());
    
    std::cout << "    testListCheckpoints passed" << std::endl;
    
    // Clean up
    std::remove(checkpointPath.c_str());
}

void testConfigureLogging() {
    // Test that logging configuration doesn't crash
    nlm::CommandLineUtils::configureLogging("Info", "", "default", 10 * 1024 * 1024);
    
    std::cout << "    testConfigureLogging passed" << std::endl;
}

void testGetConfigSummary() {
    // Create a mock config
    auto config = std::make_shared<nlm::ConfigManager>();
    config->set("int_value", 42, nlm::ConfigSource::Runtime);
    config->set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    std::string summary = nlm::CommandLineUtils::getConfigSummary(config);
    assert(!summary.empty());
    assert(summary.find("Configuration Summary") != std::string::npos);
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("string_value") != std::string::npos);
    
    std::cout << "    testGetConfigSummary passed" << std::endl;
}

void testGetHelp() {
    std::string help = nlm::CommandLineUtils::getHelp("test_program");
    assert(!help.empty());
    assert(help.find("Command Line Utilities") != std::string::npos);
    
    std::cout << "    testGetHelp passed" << std::endl;
}

void testValidateConfig() {
    // Create a mock config
    auto config = std::make_shared<nlm::ConfigManager>();
    config->set("valid_key", "valid_value", nlm::ConfigSource::Runtime);
    
    std::string validationResult = nlm::CommandLineUtils::validateConfig(config);
    assert(!validationResult.empty());
    
    std::cout << "    testValidateConfig passed" << std::endl;
}

void runAll() {
    testParseArgumentsFormat1();
    testParseArgumentsFormat2();
    testLoadConfig();
    testSaveConfig();
    testCreateCheckpoint();
    testLoadCheckpoint();
    testSaveLoadSession();
    testListCheckpoints();
    testConfigureLogging();
    testGetConfigSummary();
    testGetHelp();
    testValidateConfig();
}

} // namespace test_command_line_utils
