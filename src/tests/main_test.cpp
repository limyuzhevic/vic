#include "TestFramework.hpp"
#include <iostream>

using namespace nlm;

// Simple command-line argument parser for test configuration
struct CommandLineArgs {
    bool help = false;
    bool verbose = false;
    TestFramework::TestConfig config;
    std::string testCategory;
};

CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--test-category") {
            if (i + 1 < argc) {
                args.testCategory = argv[++i];
            }
        } else if (arg.find("--seed=") == 0) {
            try {
                args.config.seed = std::stoull(arg.substr(7));
            } catch (...) {
                std::cerr << "Warning: Invalid seed value: " << arg.substr(7) << std::endl;
            }
        } else if (arg.find("--neurons=") == 0) {
            try {
                args.config.neuronCount = std::stoull(arg.substr(10));
            } catch (...) {
                std::cerr << "Warning: Invalid neuron count: " << arg.substr(10) << std::endl;
            }
        } else if (arg.find("--steps=") == 0) {
            try {
                args.config.plasticityExperimentSteps = std::stoull(arg.substr(8));
            } catch (...) {
                std::cerr << "Warning: Invalid steps value: " << arg.substr(8) << std::endl;
            }
        } else if (arg.find("--connection-prob=") == 0) {
            try {
                args.config.connectionProbability = std::stof(arg.substr(19));
            } catch (...) {
                std::cerr << "Warning: Invalid connection probability: " << arg.substr(19) << std::endl;
            }
        } else if (arg.find("--timestep=") == 0) {
            try {
                args.config.simulationTimestep = std::stod(arg.substr(11));
            } catch (...) {
                std::cerr << "Warning: Invalid timestep: " << arg.substr(11) << std::endl;
            }
        } else if (arg.find("--input-level=") == 0) {
            try {
                args.config.inputInjectionLevel = std::stof(arg.substr(15));
            } catch (...) {
                std::cerr << "Warning: Invalid input level: " << arg.substr(15) << std::endl;
            }
        } else if (arg.find("--current-level=") == 0) {
            try {
                args.config.currentInjectionLevel = std::stof(arg.substr(16));
            } catch (...) {
                std::cerr << "Warning: Invalid current level: " << arg.substr(16) << std::endl;
            }
        } else if (arg.find("--stdp-ltp=") == 0) {
            try {
                args.config.stdpLtpWeight = std::stof(arg.substr(11));
            } catch (...) {
                std::cerr << "Warning: Invalid STDP LTP weight: " << arg.substr(11) << std::endl;
            }
        } else if (arg.find("--stdp-ltd=") == 0) {
            try {
                args.config.stdpLtdWeight = std::stof(arg.substr(11));
            } catch (...) {
                std::cerr << "Warning: Invalid STDP LTD weight: " << arg.substr(11) << std::endl;
            }
        } else if (arg.find("--stdp-tau=") == 0) {
            try {
                args.config.stdpTau = std::stof(arg.substr(10));
            } catch (...) {
                std::cerr << "Warning: Invalid STDP tau: " << arg.substr(10) << std::endl;
            }
        }
    }
    
    return args;
}

void printHelp() {
    std::cout << "NLM Neural Network Test Framework" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: run_tests [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --test-category <cat>     Run specific test category (basic, plasticity, stdp, integration)" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --seed=<value>           Random seed (default: 42)" << std::endl;
    std::cout << "  --neurons=<count>        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<count>          Plasticity experiment steps (default: 1000)" << std::endl;
    std::cout << "  --connection-prob=<float> Connection probability (default: 0.15)" << std::endl;
    std::cout << "  --timestep=<float>       Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --input-level=<float>    Input injection level (default: 30.0)" << std::endl;
    std::cout << "  --current-level=<float>  Current injection level (default: 50.0)" << std::endl;
    std::cout << "  --stdp-ltp=<float>       STDP LTP weight (default: 0.02)" << std::endl;
    std::cout << "  --stdp-ltd=<float>       STDP LTD weight (default: 0.015)" << std::endl;
    std::cout << "  --stdp-tau=<float>       STDP tau (default: 20.0)" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Categories:" << std::endl;
    std::cout << "  basic      - Basic neural connectivity tests" << std::endl;
    std::cout << "  plasticity - Plasticity learning experiments" << std::endl;
    std::cout << "  stdp       - STDP verification tests" << std::endl;
    std::cout << "  integration - Complete system integration tests" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Show help and exit if requested
    if (args.help) {
        printHelp();
        return 0;
    }
    
    // Create test framework with parsed configuration
    TestFramework testFramework(args.config);
    
    std::cout << "NLM Neural Network Test Framework" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Seed: " << args.config.seed << std::endl;
    std::cout << "  Neurons: " << args.config.neuronCount << std::endl;
    std::cout << "  Regions: " << args.config.regionCount << std::endl;
    std::cout << "  Connection Probability: " << args.config.connectionProbability << std::endl;
    std::cout << "  Simulation Timestep: " << args.config.simulationTimestep << "s" << std::endl;
    std::cout << "  Plasticity Experiment Steps: " << args.config.plasticityExperimentSteps << std::endl;
    std::cout << "  STDP Verification Trials: " << args.config.stdpVerificationTrials << std::endl;
    std::cout << "  Input Injection Level: " << args.config.inputInjectionLevel << std::endl;
    std::cout << "  Current Injection Level: " << args.config.currentInjectionLevel << std::endl;
    std::cout << "  STDP LTP Weight: " << args.config.stdpLtpWeight << std::endl;
    std::cout << "  STDP LTD Weight: " << args.config.stdpLtdWeight << std::endl;
    std::cout << "  STDP Tau: " << args.config.stdpTau << std::endl;
    std::cout << std::endl;
    
    // Run tests based on category or all tests
    bool success = false;
    
    if (args.testCategory.empty()) {
        // Run all tests
        std::cout << "Running all test categories..." << std::endl;
        success = testFramework.runAllTests();
    } else {
        // Run specific test category
        std::cout << "Running test category: " << args.testCategory << std::endl;
        
        if (args.testCategory == "basic") {
            // For now, run basic test by accessing protected methods
            // This would ideally be improved with proper extension points
            success = testFramework.runAllTests();  // All tests include basic connectivity
        } else if (args.testCategory == "plasticity") {
            success = testFramework.runAllTests();  // All tests include plasticity learning
        } else if (args.testCategory == "stdp") {
            success = testFramework.runAllTests();  // All tests include STDP verification
        } else if (args.testCategory == "integration") {
            success = testFramework.runAllTests();  // All tests include integration
        } else {
            std::cerr << "Error: Unknown test category: " << args.testCategory << std::endl;
            printHelp();
            return 1;
        }
    }
    
    std::cout << std::endl;
    if (success) {
        std::cout << "All tests PASSED! ✓" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests FAILED! ✗" << std::endl;
        return 1;
    }
}