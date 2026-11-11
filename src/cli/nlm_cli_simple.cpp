#!/usr/bin/env cpp

/**
 * Simple NLM Command Line Interface (CLI)
 * 
 * A basic CLI for NLM operations - create, run, and checkpoint brains.
 */

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace nlm {

class SimpleCLI {
public:
    SimpleCLI() {
        logger_ = std::make_shared<Logger>();
        consoleLogger_ = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger_->addLogger(consoleLogger_);
        Logger::setGlobal(logger_);
    }

    void run(const std::vector<std::string>& args) {
        if (args.empty()) {
            printUsage();
            return;
        }

        std::string command = args[0];
        
        if (command == "create") {
            handleCreateCommand(args);
        } else if (command == "run") {
            handleRunCommand(args);
        } else if (command == "help") {
            printHelp(args);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printUsage();
        }
    }

private:
    void printUsage() {
        std::cout << "NLM CLI - Simple Brain Interface" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  create <file>   - Create and initialize a brain" << std::endl;
        std::cout << "  run <steps>      - Run a brain simulation" << std::endl;
        std::cout << "  help             - Show this help" << std::endl;
    }

    void printHelp(const std::vector<std::string>& args) {
        std::cout << "NLM CLI Commands:" << std::endl;
        std::cout << "  create <file>   - Create brain from config file" << std::endl;
        std::cout << "  run <steps>      - Run simulation for specified steps" << std::endl;
        std::cout << "  help             - Show help" << std::endl;
    }

    void handleCreateCommand(const std::vector<std::string>& args) {
        auto config = std::make_shared<Config>();
        
        // Load configuration from file if provided
        if (args.size() > 1) {
            std::string configFile = args[1];
            if (config->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
            }
        }
        
        // Set defaults
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            NLM_LOG_INFO("Brain created successfully");
            brain->logStatus();
        } else {
            NLM_LOG_ERROR("Failed to create brain");
        }
    }

    void handleRunCommand(const std::vector<std::string>& args) {
        int steps = 100;
        if (args.size() > 1) {
            steps = std::stoi(args[1]);
        }
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return;
        }
        
        NLM_LOG_INFO("Starting simulation with " + std::to_string(steps) + " steps");
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 20 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                           std::to_string(brain->getFiringNeuronCount()) + " firing neurons,
                           " + std::to_string(brain->getAverageFiringRate()) + " Hz");
            }
        }
        
        NLM_LOG_INFO("Simulation completed");
        brain->logStatus();
    }

    std::shared_ptr<Logger> logger_;
    std::shared_ptr<ConsoleLogger> consoleLogger_;
};

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    SimpleCLI cli;
    cli.run(args);
    
    return 0;
}
