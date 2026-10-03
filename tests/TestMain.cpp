// tests/TestMain.cpp - Main test executable for NLM test framework
#include "TestFramework.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace nlm;

// Convert string to lowercase
std::string toLowercase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

// Parse command line arguments
struct TestArgs {
    std::string testName;
    bool verbose;
    bool help;
    std::string configFile;
    std::unordered_map<std::string, std::string> extraArgs;
};

TestArgs parseArgs(int argc, char** argv) {
    TestArgs args;
    args.verbose = false;
    args.help = false;
    args.testName = "all";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--test" && i + 1 < argc) {
            args.testName = argv[++i];
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(8);
        } else if (arg.find("--") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.extraArgs[key] = value;
            }
        }
    }
    
    return args;
}

void printHelp() {
    std::cout << "NLM Tests - Available Options:" << std::endl;
    std::cout << "  --test <name>              Run specific test (use 'all' for all tests)" << std::endl;
    std::cout << "  --verbose                  Enable verbose output" << std::endl;
    std::cout << "  --help                     Show this help message" << std::endl;
    std::cout << "  --config=<file>            Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    std::cout << "  basic-connectivity         Test basic neural signal propagation" << std::endl;
    std::cout << "  plasticity-learning        Test synaptic learning through experience" << std::endl;
    std::cout << "  stdp-verification          Test spike-timing dependent plasticity" << std::endl;
    std::cout << "  memory-integration         Test memory systems integration" << std::endl;
    std::cout << "  neuromodulation-integration  Test neuromodulation systems integration" << std::endl;
    std::cout << "  development-integration    Test developmental system integration" << std::endl;
    std::cout << "  all                        Run all tests" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "NLM Test Framework" << std::endl;
    std::cout << "===================" << std::endl << std::endl;
    
    TestArgs args = parseArgs(argc, argv);
    
    if (args.help) {
        printHelp();
        return 0;
    }
    
    std::cout << "Test selection: " << args.testName << std::endl;
    std::cout << "Verbose mode: " << (args.verbose ? "Enabled" : "Disabled") << std::endl;
    
    if (!args.configFile.empty()) {
        std::cout << "Config file: " << args.configFile << std::endl;
    }
    
    // Create brain
    auto config = std::make_shared<Config>();
    
    if (!args.configFile.empty()) {
        if (config->loadFromFile(args.configFile)) {
            std::cout << "Configuration loaded from: " << args.configFile << std::endl;
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Apply default Phase 2 configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Apply extra arguments
    for (const auto& pair : args.extraArgs) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        try {
            if (key == "neuron_count" || key == "random_seed") {
                config->set(key, static_cast<int64_t>(std::stoll(value)), ConfigSource::CommandLine);
            } else if (key == "connection_probability") {
                config->set(key, static_cast<float>(std::stof(value)), ConfigSource::CommandLine);
            } else if (key == "simulation_timestep") {
                config->set(key, std::stod(value), ConfigSource::CommandLine);
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to parse argument " << key << ": " << e.what() << std::endl;
        }
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create test runner
    NLMTestRunner testRunner(args.verbose);
    
    // Create tests based on requested test name
    std::string testKey = toLowercase(args.testName);
    
    if (testKey == "all" || testKey == "all tests") {
        testRunner.addTest(std::make_unique<BasicConnectivityTest>());
        testRunner.addTest(std::make_unique<PlasticityLearningTest>());
        testRunner.addTest(std::make_unique<STDPVerificationTest>());
        testRunner.addTest(std::make_unique<MemorySystemIntegrationTest>());
        testRunner.addTest(std::make_unique<NeuromodulationIntegrationTest>());
        testRunner.addTest(std::make_unique<DevelopmentIntegrationTest>());
    } else if (testKey == "basic-connectivity" || testKey == "basic connectivity") {
        testRunner.addTest(std::make_unique<BasicConnectivityTest>());
    } else if (testKey == "plasticity-learning" || testKey == "plasticity learning") {
        testRunner.addTest(std::make_unique<PlasticityLearningTest>());
    } else if (testKey == "stdp-verification" || testKey == "stdp verification") {
        testRunner.addTest(std::make_unique<STDPVerificationTest>());
    } else if (testKey == "memory-integration" || testKey == "memory integration") {
        testRunner.addTest(std::make_unique<MemorySystemIntegrationTest>());
    } else if (testKey == "neuromodulation-integration" || testKey == "neuromodulation integration") {
        testRunner.addTest(std::make_unique<NeuromodulationIntegrationTest>());
    } else if (testKey == "development-integration" || testKey == "development integration") {
        testRunner.addTest(std::make_unique<DevelopmentIntegrationTest>());
    } else {
        std::cerr << "ERROR: Unknown test name: " << args.testName << std::endl;
        printHelp();
        return 1;
    }
    
    // Run tests
    testRunner.runAllTests(brain);
    
    std::cout << std::endl << "Tests completed." << std::endl;
    return 0;
}