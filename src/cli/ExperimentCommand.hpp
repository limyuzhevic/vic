#pragma once

#include "Command.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {

class ExperimentCommand : public Command {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { return "Run specific experiment (Phase 3, 4, 5, 6)"; }
    std::string getUsage() const override { return "experiment <phase> [OPTIONS]"; }
    std::string getHelp() const override {
        return "Runs specific neural network experiments.\n"
               "Available phases: phase2 (basic neural computation), phase3 (cognitive),\n"
               "phase4 (perception), phase5 (memory), phase6 (integrated).\n"
               "Each experiment tests different aspects of neural intelligence.\n"
               "Supports batch processing and experiment comparison.\n"
               "Examples:\n"
               "  nlm experiment phase2\n"
               "  nlm experiment phase3 --duration=10000\n"
               "  nlm experiment phase4 --trials=5\n"
               "  nlm experiment batch --experiments=phase2,phase3,phase4\n";
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Error: No experiment phase specified.\n";
            std::cout << "Usage: nlm experiment <phase> [OPTIONS]\n";
            std::cout << "Available phases: phase2, phase3, phase4, phase5, phase6, batch\n";
            return false;
        }
        
        std::string phase = args[0];
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        std::cout << "Running NLM " << phase << " experiment...\n";
        
        // Create config
        auto config = std::make_shared<Config>();
        
        // Set defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Parse experiment-specific arguments
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        
        // Create experiment runner
        auto runner = std::make_shared<ExperimentRunner>();
        
        // Create brain and environment based on phase
        auto brain = createBrainForPhase(phase, config);
        auto environment = createEnvironmentForPhase(phase, config);
        
        if (!brain || !brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain!\n";
            return false;
        }
        
        // Create experiment based on phase
        auto experiment = runner->createExperiment(phase);
        if (!experiment) {
            std::cout << "ERROR: Failed to create experiment: " << phase << "\n";
            return false;
        }
        
        // Run experiment
        SimulationStep maxSteps = config->getOr<SimulationStep>("max_steps", 1000);
        bool success = runner->runExperiment(experiment, brain, environment, maxSteps);
        
        if (success) {
            std::cout << "\n✓ Experiment " << phase << " completed successfully.\n";
            // Save results
            runner->saveResults("experiments/results_" + phase + "_" + 
                              std::to_string(std::time(nullptr)) + ".json");
        } else {
            std::cout << "\n✗ Experiment " << phase << " failed.\n";
        }
        
        return success;
    }
    
    bool execute(const Config& config) override {
        std::cout << "Running experiment with existing configuration...\n";
        std::cout << "Note: Run 'nlm experiment --help' for usage.\n";
        return true;
    }

private:
    std::shared_ptr<Brain> createBrainForPhase(const std::string& phase, std::shared_ptr<Config> config) {
        // Implementation would depend on the phase
        // This is a placeholder
        return nullptr;
    }
    
    std::shared_ptr<Environment> createEnvironmentForPhase(const std::string& phase, std::shared_ptr<Config> config) {
        // Implementation would depend on the phase
        // This is a placeholder
        return nullptr;
    }
};

} // namespace nlm