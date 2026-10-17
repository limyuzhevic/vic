// CLI Framework Test
// Tests the NLM CLI framework functionality

#include "core/CLI/CLIFramework.hpp"
#include "core/CLI/CLIAdapter.hpp"
#include "core/CLI/CLIMacros.hpp"
#include <iostream>
#include <string>

void testCLIParserCreation() {
    std::cout << "Testing CLIParser creation..." << std::endl;
    auto parser = nlm::CLIFactory::createParser();
    if (!parser) {
        throw std::runtime_error("Failed to create CLI parser");
    }
    std::cout << "✓ CLI parser created successfully" << std::endl;
}

void testVersion() {
    std::cout << "Testing version information..." << std::endl;
    std::cout << "Version: " << nlm::CLI_VERSION << std::endl;
    if (nlm::CLI_VERSION.empty()) {
        throw std::runtime_error("Version string is empty");
    }
    std::cout << "✓ Version information valid" << std::endl;
}

void testHelpText() {
    std::cout << "Testing help text..." << std::endl;
    if (nlm::CLI_HELP.empty()) {
        throw std::runtime_error("Help text is empty");
    }
    // Check for essential help content
    if (nlm::CLI_HELP.find("Usage:") == std::string::npos) {
        throw std::runtime_error("Help text missing usage information");
    }
    if (nlm::CLI_HELP.find("--help") == std::string::npos) {
        throw std::runtime_error("Help text missing --help option");
    }
    std::cout << "✓ Help text contains essential information" << std::endl;
}

void testErrorPrefix() {
    std::cout << "Testing error prefix..." << std::endl;
    if (nlm::CLI_ERROR_PREFIX.empty()) {
        throw std::runtime_error("Error prefix is empty");
    }
    std::cout << "✓ Error prefix valid" << std::endl;
}

void testUtilityFunctions() {
    std::cout << "Testing utility functions..." << std::endl;
    
    // Test startsWith
    if (!nlm::startsWith("hello world", "hello")) {
        throw std::runtime_error("startsWith failed for positive case");
    }
    if (nlm::startsWith("test", "hello")) {
        throw std::runtime_error("startsWith failed for negative case");
    }
    
    // Test extractValue
    std::string value = nlm::extractValue("--key=value");
    if (value != "value") {
        throw std::runtime_error("extractValue failed");
    }
    
    // Test toLower
    std::string lower = nlm::toLower("HELLO WORLD");
    if (lower != "hello world") {
        throw std::runtime_error("toLower failed");
    }
    
    std::cout << "✓ Utility functions working correctly" << std::endl;
}

void testConfigFormatDetection() {
    std::cout << "Testing configuration format detection..." << std::endl;
    
    auto jsonFormat = nlm::detectConfigFormat("test.json");
    if (jsonFormat != nlm::ConfigFormat::JSON) {
        throw std::runtime_error("JSON format detection failed");
    }
    
    auto yamlFormat = nlm::detectConfigFormat("test.yaml");
    if (yamlFormat != nlm::ConfigFormat::YAML) {
        throw std::runtime_error("YAML format detection failed");
    }
    
    auto ymlFormat = nlm::detectConfigFormat("test.yml");
    if (ymlFormat != nlm::ConfigFormat::YAML) {
        throw std::runtime_error("YML format detection failed");
    }
    
    auto cfgFormat = nlm::detectConfigFormat("test.cfg");
    if (cfgFormat != nlm::ConfigFormat::SimpleKeyValue) {
        throw std::runtime_error("CFG format detection failed");
    }
    
    auto unknownFormat = nlm::detectConfigFormat("test.xyz");
    if (unknownFormat != nlm::ConfigFormat::Unknown) {
        throw std::runtime_error("Unknown format detection failed");
    }
    
    std::cout << "✓ Configuration format detection working correctly" << std::endl;
}

void testLogLevelConversion() {
    std::cout << "Testing log level conversion..." << std::endl;
    
    auto traceLevel = nlm::CLIParser::stringToLogLevel("trace");
    if (traceLevel != nlm::LogLevel::Trace) {
        throw std::runtime_error("Trace level conversion failed");
    }
    
    auto debugLevel = nlm::CLIParser::stringToLogLevel("debug");
    if (debugLevel != nlm::LogLevel::Debug) {
        throw std::runtime_error("Debug level conversion failed");
    }
    
    auto infoLevel = nlm::CLIParser::stringToLogLevel("info");
    if (infoLevel != nlm::LogLevel::Info) {
        throw std::runtime_error("Info level conversion failed");
    }
    
    auto warningLevel = nlm::CLIParser::stringToLogLevel("warning");
    if (warningLevel != nlm::LogLevel::Warning) {
        throw std::runtime_error("Warning level conversion failed");
    }
    
    auto errorLevel = nlm::CLIParser::stringToLogLevel("error");
    if (errorLevel != nlm::LogLevel::Error) {
        throw std::runtime_error("Error level conversion failed");
    }
    
    std::cout << "✓ Log level conversion working correctly" << std::endl;
}

void testCommandUtilities() {
    std::cout << "Testing command utilities..." << std::endl;
    
    auto commands = nlm::CLIParser::getAvailableCommands();
    if (commands.empty()) {
        throw std::runtime_error("Available commands list is empty");
    }
    
    bool hasDemo = false;
    bool hasBenchmark = false;
    bool hasValidate = false;
    
    for (const auto& cmd : commands) {
        if (cmd == "demo") hasDemo = true;
        if (cmd == "benchmark") hasBenchmark = true;
        if (cmd == "validate") hasValidate = true;
    }
    
    if (!hasDemo || !hasBenchmark || !hasValidate) {
        throw std::runtime_error("Missing essential commands in available commands list");
    }
    
    auto demoTypes = nlm::CLIParser::getAvailableDemoTypes();
    if (demoTypes.empty()) {
        throw std::runtime_error("Available demo types list is empty");
    }
    
    auto benchmarkConfigs = nlm::CLIParser::getDefaultBenchmarkConfigs();
    if (benchmarkConfigs.empty()) {
        throw std::runtime_error("Default benchmark configurations list is empty");
    }
    
    std::cout << "✓ Command utilities working correctly" << std::endl;
}

void testCLIConfig() {
    std::cout << "Testing CLIConfig structure..." << std::endl;
    
    nlm::CLIConfig config;
    
    // Set test values
    config.help = true;
    config.version = true;
    config.command = "demo";
    config.demo_type = "phase3";
    config.demo_episodes = 10;
    config.demo_steps = 200;
    config.config_file = "test.cfg";
    config.log_level = "debug";
    config.quiet = false;
    config.verbose = true;
    
    // Verify values
    if (!config.help) {
        throw std::runtime_error("help flag not set correctly");
    }
    
    if (!config.version) {
        throw std::runtime_error("version flag not set correctly");
    }
    
    if (config.command != "demo") {
        throw std::runtime_error("command not set correctly");
    }
    
    if (config.demo_type != "phase3") {
        throw std::runtime_error("demo_type not set correctly");
    }
    
    if (config.demo_episodes != 10) {
        throw std::runtime_error("demo_episodes not set correctly");
    }
    
    if (config.demo_steps != 200) {
        throw std::runtime_error("demo_steps not set correctly");
    }
    
    if (config.config_file != "test.cfg") {
        throw std::runtime_error("config_file not set correctly");
    }
    
    if (config.log_level != "debug") {
        throw std::runtime_error("log_level not set correctly");
    }
    
    if (config.quiet) {
        throw std::runtime_error("quiet flag not set correctly");
    }
    
    if (!config.verbose) {
        throw std::runtime_error("verbose flag not set correctly");
    }
    
    std::cout << "✓ CLIConfig structure working correctly" << std::endl;
}

void testCommandExecution() {
    std::cout << "Testing command execution..." << std::endl;
    
    nlm::CLIConfig config;
    config.command = "demo";
    config.demo_type = "phase3";
    config.demo_episodes = 1;
    config.demo_steps = 10;
    config.benchmark_mode = false;
    config.script_mode = false;
    config.test_mode = false;
    config.validate_mode = false;
    config.debug_mode = false;
    
    // Create configuration
    auto nlmConfig = nlm::CLIFactory::createConfig(config);
    if (!nlmConfig) {
        throw std::runtime_error("Failed to create NLM configuration");
    }
    
    // Test demo command
    int demoResult = nlm::CLIFactory::executeDemoCommand(config, nlmConfig);
    if (demoResult != 0) {
        throw std::runtime_error("Demo command execution failed");
    }
    
    // Test benchmark command
    config.benchmark_mode = true;
    config.benchmark_configs = {{100, 100}};
    int benchmarkResult = nlm::CLIFactory::executeBenchmarkCommand(config, nlmConfig);
    if (benchmarkResult != 0) {
        throw std::runtime_error("Benchmark command execution failed");
    }
    
    // Test test command
    config.test_mode = true;
    config.validate_configs = true;
    int testResult = nlm::CLIFactory::executeTestCommand(config, nlmConfig);
    if (testResult != 0) {
        throw std::runtime_error("Test command execution failed");
    }
    
    // Test validate command
    config.validate_mode = true;
    int validateResult = nlm::CLIFactory::executeValidateCommand(config, nlmConfig);
    if (validateResult != 0) {
        throw std::runtime_error("Validate command execution failed");
    }
    
    std::cout << "✓ Command execution working correctly" << std::endl;
}

void testConfigurationFileLoading() {
    std::cout << "Testing configuration file loading..." << std::endl;
    
    nlm::CLIConfig config;
    config.config_file = "configs/default.cfg";
    
    auto loadedConfig = nlm::CLIFactory::loadConfiguration(config.config_file, config);
    
    if (!loadedConfig) {
        throw std::runtime_error("Failed to load configuration file");
    }
    
    std::cout << "✓ Configuration file loading working correctly" << std::endl;
}

void testLoggingSetup() {
    std::cout << "Testing logging setup..." << std::endl;
    
    nlm::CLIConfig config;
    config.log_level = "info";
    config.log_file = "nlm_test.log";
    config.quiet = false;
    config.verbose = true;
    
    try {
        nlm::CLIFactory::setupLogging(config);
        std::cout << "✓ Logging setup working correctly" << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Logging setup failed: ") + e.what());
    }
}

void runAllTests() {
    std::cout << "=== NLM CLI Framework Test Suite ===" << std::endl;
    std::cout << "Running all CLI framework tests..." << std::endl << std::endl;
    
    try {
        testCLIParserCreation();
        testVersion();
        testHelpText();
        testErrorPrefix();
        testUtilityFunctions();
        testConfigFormatDetection();
        testLogLevelConversion();
        testCommandUtilities();
        testCLIConfig();
        testCommandExecution();
        testConfigurationFileLoading();
        testLoggingSetup();
        
        std::cout << std::endl;
        std::cout << "=== All Tests Passed! ===" << std::endl;
        std::cout << "The NLM CLI framework is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Test suite completed with failures." << std::endl;
        throw;
    }
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
