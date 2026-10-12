// main.cpp
// NLM - Neural Learning Machine
// Main entry point with improved CLI and modular test runner

#include "CLI/CommandLineParser.h"
#include "CLI/TestRunner.h"
#include "CLI/ConfigurationManager.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "utils/Benchmark.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

using namespace nlm;

void printLogo() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0\n";
    std::cout << "Copyright (c) 2024 NLM Project\n";
    std::cout << "MIT License\n";
    std::cout << "\n";
    std::cout << "Phase 2: Real Neural Computation\n";
    std::cout << "- LIF neuron dynamics\n";
    std::cout << "- Event-driven spike propagation\n";
    std::cout << "- STDP and Hebbian plasticity\n";
    std::cout << "- Structural plasticity\n";
}

void runBenchmark(std::shared_ptr<Config> config, int duration) {
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmarking!\n";
        return;
    }
    
    std::cout << "\n=== Benchmarking ===\n";
    std::cout << "Running benchmark for " << duration << " simulation steps...\n\n";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < duration; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "Benchmark completed!\n";
    std::cout << "Brain statistics:\n";
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "\nWall time: " << elapsed.count() << " seconds\n";
}

void runInteractive(std::shared_ptr<Config> config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return;
    }
    
    std::cout << "\n=== Interactive Mode ===\n";
    std::cout << "Brain initialized successfully.\n";
    std::cout << "Type 'help' for available commands.\n\n";
    
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command == "help" || command == "h") {
            std::cout << "Available commands:\n";
            std::cout << "  step <steps>     Run simulation steps\n";
            std::cout << "  status           Print brain status\n";
            std::cout << "  config          Show current configuration\n";
            std::cout << "  exit            Exit interactive mode\n";
            continue;
        }
        
        if (command == "exit" || command == "q") {
            break;
        }
        
        if (command.substr(0, 4) == "step") {
            try {
                int steps = std::stoi(command.substr(5));
                for (nlm::SimulationStep step = 0; step < steps; ++step) {
                    brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
                    std::cout << "Step " << step << ": " 
                              << brain->getFiringNeuronCount() << " firing neurons, "
                              << brain->getTotalSpikeCount() << " total spikes\n";
                }
            } catch (...) {
                std::cout << "Invalid command. Type 'help' for available commands.\n";
            }
            continue;
        }
        
        if (command == "status") {
            brain->logStatus();
            continue;
        }
        
        if (command == "config") {
            std::cout << "Current configuration:\n";
            std::cout << config->summary() << std::endl;
            continue;
        }
        
        std::cout << "Unknown command: " << command << ". Type 'help' for available commands.\n";
    }
}

void runExperiment(std::shared_ptr<Config> config, const std::string& experimentName, int steps) {
    // For now, we'll run a simple simulation that demonstrates learning
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for experiment!\n";
        return;
    }
    
    std::cout << "\n=== Running Experiment: " << experimentName << " ===\n";
    std::cout << "Running " << steps << " simulation steps...\n";
    
    // Enable learning systems
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
        }
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < steps; ++step) {
        // Apply sensory input to stimulate learning
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getFiringNeuronCount() << " firing neurons, "
                      << brain->getTotalSpikeCount() << " total spikes\n";
        }
    }
    
    std::cout << "\nExperiment completed!\n";
    std::cout << "Final statistics:\n";
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
}

int main(int argc, char** argv) {
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    printLogo();
    
    // Create command line parser
    CommandLineParser cli;
    
    // Add commands
    cli.addCommand("test", "Run neural network tests", {
        CommandLineOption{"test", 't', "Test to run", false, "", nullptr},
        CommandLineOption{"all", 'a', "Run all tests", false, "", nullptr}
    });
    
    cli.addCommand("benchmark", "Run performance benchmark", {
        CommandLineOption{"steps", 's', "Number of simulation steps to run", false, "1000", nullptr},
        CommandLineOption{"config", 'c', "Configuration file", false, "configs/default.cfg", nullptr}
    });
    
    cli.addCommand("interactive", "Start interactive mode", {
        CommandLineOption{"config", 'c', "Configuration file", false, "configs/default.cfg", nullptr}
    });
    
    cli.addCommand("experiment", "Run experiment", {
        CommandLineOption{"name", 'n', "Experiment name", false, "default", nullptr},
        CommandLineOption{"steps", 's', "Number of steps", false, "1000", nullptr},
        CommandLineOption{"config", 'c', "Configuration file", false, "configs/default.cfg", nullptr}
    });
    
    // Parse command line
    ParsedCommand parsed = cli.parse(argc, argv);
    
    // Handle global options
    if (parsed.helpRequested) {
        cli.showHelp();
        return 0;
    }
    
    if (parsed.versionRequested) {
        printVersion();
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    ConfigurationManager configManager;
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (size_t i = 0; i < parsed.args.size(); ++i) {
        if (parsed.args[i] == "--config" && i + 1 < parsed.args.size()) {
            configFile = parsed.args[i + 1];
            i++;
        }
    }
    
    std::cout << "Loading configuration from: " << configFile << "...\n";
    if (config->loadFromFile(configFile)) {
        std::cout << "Configuration loaded successfully.\n";
    } else {
        std::cout << "Using default configuration.\n";
    }
    
    // Override with parsed options
    for (const auto& pair : parsed.options) {
        config->set(pair.first, pair.second);
    }
    
    // Apply defaults and validate
    configManager.applyDefaults(config);
    if (!configManager.validateConfig(config)) {
        std::cerr << "Configuration validation failed! Exiting.\n";
        return 1;
    }
    
    configManager.showConfigSummary(config);
    
    // Execute command
    if (parsed.command == "test") {
        TestRunner testRunner;
        
        std::string testName = "all";
        for (const auto& pair : parsed.options) {
            if (pair.first == "test") {
                testName = pair.second;
            }
        }
        
        if (testName == "all") {
            auto results = testRunner.runAllTests(config);
            int passed = 0, failed = 0;
            for (const auto& result : results) {
                if (result.passed) {
                    passed++;
                } else {
                    failed++;
                    std::cout << "FAILED: " << result.name << " - " << result.message << std::endl;
                }
            }
            std::cout << "\nTest results: " << passed << " passed, " << failed << " failed\n";
            
            if (failed > 0) {
                return 1;
            }
        } else {
            TestResult result = testRunner.runTest(testName, config);
            if (result.passed) {
                std::cout << "Test passed!\n";
                return 0;
            } else {
                std::cout << "Test failed: " << result.message << std::endl;
                return 1;
            }
        }
    }
    
    else if (parsed.command == "benchmark") {
        int steps = 1000;
        for (const auto& pair : parsed.options) {
            if (pair.first == "steps") {
                try {
                    steps = std::stoi(pair.second);
                } catch (...) {}
            }
        }
        runBenchmark(config, steps);
        return 0;
    }
    
    else if (parsed.command == "interactive") {
        runInteractive(config);
        return 0;
    }
    
    else if (parsed.command == "experiment") {
        std::string experimentName = "default";
        int steps = 1000;
        for (const auto& pair : parsed.options) {
            if (pair.first == "name") {
                experimentName = pair.second;
            } else if (pair.first == "steps") {
                try {
                    steps = std::stoi(pair.second);
                } catch (...) {}
            }
        }
        runExperiment(config, experimentName, steps);
        return 0;
    }
    
    else {
        std::cout << "Unknown command: " << parsed.command << "\n";
        std::cout << "Use 'nlm help' for available commands.\n";
        return 1;
    }
}
