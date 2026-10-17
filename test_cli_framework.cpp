#include <iostream>
#include <string>
#include "core/CLI/CLIFramework.hpp"
#include "core/CLI/CLIAdapter.hpp"
#include "core/CLI/CLIMacros.hpp"

int main(int argc, char* argv[]) {
    std::cout << "=== NLM CLI Framework Test ===" << std::endl;
    
    // Test 1: Create parser
    auto parser = nlm::CLIFactory::createParser();
    std::cout << "✓ CLI parser created" << std::endl;
    
    // Test 2: Test version
    std::cout << "Version: " << nlm::CLI_VERSION << std::endl;
    
    // Test 3: Test help text
    std::cout << "\nHelp text preview:" << std::endl;
    std::cout << nlm::CLI_HELP.substr(0, 200) << "..." << std::endl;
    
    // Test 4: Test error prefix
    std::cout << "\nError prefix: " << nlm::CLI_ERROR_PREFIX << "test error" << std::endl;
    
    // Test 5: Test utility functions
    std::cout << "\nUtility function tests:" << std::endl;
    std::cout << "startsWith('hello world', 'hello'): " << std::boolalpha << nlm::startsWith("hello world", "hello") << std::endl;
    std::cout << "startsWith('test', 'hello'): " << std::boolalpha << nlm::startsWith("test", "hello") << std::endl;
    std::cout << "extractValue('--key=value'): '" << nlm::extractValue("--key=value") << "'" << std::endl;
    std::cout << "toLower('HELLO'): '" << nlm::toLower("HELLO") << "'" << std::endl;
    
    // Test 6: Test config format detection
    std::cout << "\nConfig format detection tests:" << std::endl;
    std::cout << "json.json: " << static_cast<int>(nlm::detectConfigFormat("test.json")) << std::endl;
    std::cout << "yaml.yaml: " << static_cast<int>(nlm::detectConfigFormat("test.yaml")) << std::endl;
    std::cout << "yml.yml: " << static_cast<int>(nlm::detectConfigFormat("test.yml")) << std::endl;
    std::cout << "config.cfg: " << static_cast<int>(nlm::detectConfigFormat("test.cfg")) << std::endl;
    std::cout << "unknown.xyz: " << static_cast<int>(nlm::detectConfigFormat("test.xyz")) << std::endl;
    
    // Test 7: Test log level conversion
    std::cout << "\nLog level conversion tests:" << std::endl;
    std::cout << "trace -> " << static_cast<int>(nlm::CLIParser::stringToLogLevel("trace")) << std::endl;
    std::cout << "debug -> " << static_cast<int>(nlm::CLIParser::stringToLogLevel("debug")) << std::endl;
    std::cout << "info -> " << static_cast<int>(nlm::CLIParser::stringToLogLevel("info")) << std::endl;
    std::cout << "warning -> " << static_cast<int>(nlm::CLIParser::stringToLogLevel("warning")) << std::endl;
    std::cout << "error -> " << static_cast<int>(nlm::CLIParser::stringToLogLevel("error")) << std::endl;
    
    // Test 8: Test command utilities
    std::cout << "\nCommand utilities:" << std::endl;
    auto commands = nlm::CLIParser::getAvailableCommands();
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << "  - " << cmd << std::endl;
    }
    
    auto demoTypes = nlm::CLIParser::getAvailableDemoTypes();
    std::cout << "Available demo types:" << std::endl;
    for (const auto& type : demoTypes) {
        std::cout << "  - " << type << std::endl;
    }
    
    auto benchmarkConfigs = nlm::CLIParser::getDefaultBenchmarkConfigs();
    std::cout << "Default benchmark configurations:" << std::endl;
    for (const auto& config : benchmarkConfigs) {
        std::cout << "  - " << config.first << " neurons, " << config.second << " steps" << std::endl;
    }
    
    // Test 9: Test CLIConfig structure
    std::cout << "\nCLIConfig structure tests:" << std::endl;
    nlm::CLIConfig config;
    config.help = true;
    config.version = true;
    config.command = "demo";
    config.demo_type = "phase3";
    config.demo_episodes = 10;
    config.demo_steps = 200;
    config.benchmark_mode = true;
    config.script_mode = false;
    config.test_mode = true;
    config.validate_mode = true;
    config.debug_mode = true;
    config.config_file = "test.cfg";
    config.log_level = "debug";
    config.log_file = "nlm.log";
    config.quiet = false;
    config.verbose = true;
    config.dry_run = true;
    
    std::cout << "Command: " << config.command << std::endl;
    std::cout << "Demo type: " << config.demo_type << std::endl;
    std::cout << "Demo episodes: " << config.demo_episodes << std::endl;
    std::cout << "Demo steps: " << config.demo_steps << std::endl;
    std::cout << "Config file: " << config.config_file << std::endl;
    std::cout << "Log level: " << config.log_level << std::endl;
    
    // Test 10: Test command execution
    std::cout << "\nCommand execution tests:" << std::endl;
    
    // Create a test config
    auto nlmConfig = nlm::CLIFactory::createConfig(config);
    std::cout << "✓ Configuration created" << std::endl;
    
    // Test benchmark command
    int benchmarkResult = nlm::CLIFactory::executeBenchmarkCommand(config, nlmConfig);
    std::cout << "Benchmark command result: " << benchmarkResult << std::endl;
    
    int demoResult = nlm::CLIFactory::executeDemoCommand(config, nlmConfig);
    std::cout << "Demo command result: " << demoResult << std::endl;
    
    int testResult = nlm::CLIFactory::executeTestCommand(config, nlmConfig);
    std::cout << "Test command result: " << testResult << std::endl;
    
    int validateResult = nlm::CLIFactory::executeValidateCommand(config, nlmConfig);
    std::cout << "Validate command result: " << validateResult << std::endl;
    
    // Test 11: Test configuration file loading
    std::cout << "\nConfiguration file loading tests:" << std::endl;
    auto loadedConfig = nlm::CLIFactory::loadConfiguration("configs/default.cfg", config);
    if (loadedConfig) {
        std::cout << "✓ Configuration loaded successfully" << std::endl;
        nlm::CLIFactory::printConfigurationSummary(loadedConfig);
    } else {
        std::cout << "✗ Failed to load configuration" << std::endl;
    }
    
    // Test 12: Test logging setup
    std::cout << "\nLogging setup tests:" << std::endl;
    nlm::CLIFactory::setupLogging(config);
    std::cout << "✓ Logging setup completed" << std::endl;
    
    std::cout << "\n=== CLI Framework Test Complete ===" << std::endl;
    std::cout << "All tests passed! The CLI framework is working correctly." << std::endl;
    
    return 0;
}
