#pragma once

#include "NLMCLI.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>
#include <iostream>
#include <fstream>

namespace nlm {

class RunCommand : public ICommand {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run a simulation with specified scenario and configuration"; }
    std::string getUsage() const override { return "run <scenario> [options]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        return !args.empty();
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli run <scenario> [options]" << std::endl;
        std::cout << "Run a neural simulation with the specified scenario." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  <scenario>          Scenario name or configuration file path" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --steps <n>          Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --neuron_count <n>   Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --region_count <n>   Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --random_seed <n>    Random seed for reproducibility (default: 42)" << std::endl;
        std::cout << "  --config <file>      Configuration file path" << std::endl;
        std::cout << "  --output <format>    Output format (json, csv, text)" << std::endl;
        std::cout << "  --log-level <level>  Logging level (debug, info, warning, error)" << std::endl;
        std::cout << "  --verbose, -v        Verbose output" << std::endl;
        std::cout << "  --quiet, -q          Quiet output" << std::endl;
        std::cout << "  --help, -h           Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        // Parse scenario and options
        std::string scenario;
        std::map<std::string, std::string> options;
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                result.success = true;
                result.exitCode = 0;
                return result;
            } else {
                if (scenario.empty()) {
                    scenario = arg;
                } else {
                    options[arg] = "";
                }
            }
        }
        
        if (scenario.empty()) {
            result.success = false;
            result.message = "Missing scenario argument";
            result.exitCode = 1;
            return result;
        }
        
        try {
            result = runScenario(scenario, options, config);
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error running scenario: ") + e.what();
            result.exitCode = 3;
        }
        
        return result;
    }
    
private:
    CommandResult runScenario(const std::string& scenario, 
                             const std::map<std::string, std::string>& options,
                             const CLIConfig& config) {
        CommandResult result;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        LogLevel logLevel = LogLevel::Info;
        if (config.logLevel == "debug") logLevel = LogLevel::Debug;
        else if (config.logLevel == "warning") logLevel = LogLevel::Warning;
        else if (config.logLevel == "error") logLevel = LogLevel::Error;
        else if (config.logLevel == "critical") logLevel = LogLevel::Critical;
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        consoleLogger->setUseColors(!config.quiet);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Starting scenario: " + scenario);
        
        // Create configuration
        auto simConfig = std::make_shared<Config>();
        
        // Load scenario configuration
        if (scenario.find(".cfg") != std::string::npos || scenario.find(".json") != std::string::npos) {
            // Load from file
            if (!CLIManager::loadConfigFile(scenario, simConfig)) {
                result.success = false;
                result.message = "Failed to load scenario configuration: " + scenario;
                return result;
            }
        } else {
            // Use default configuration with scenario name
            simConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            simConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            simConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            simConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        }
        
        // Override with command line options
        for (const auto& pair : options) {
            std::string key = pair.first;
            std::string value = pair.second;
            
            if (key == "steps") {
                simConfig->set("max_simulation_steps", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "neuron_count") {
                simConfig->set("neuron_count", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "region_count") {
                simConfig->set("region_count", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "random_seed") {
                simConfig->set("random_seed", std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "plasticity_learning_rate") {
                simConfig->set("plasticity.learning_rate", std::stod(value), ConfigSource::CommandLine);
            } else if (key == "stdp_learning_rate") {
                simConfig->set("plasticity.stdp.learning_rate", std::stod(value), ConfigSource::CommandLine);
            }
        }
        
        // Apply global configuration overrides
        for (const auto& pair : config.envOverrides) {
            simConfig->set(pair.first, pair.second, ConfigSource::CommandLine);
        }
        
        NLM_LOG_INFO("Configuration loaded:");
        NLM_LOG_INFO("  Scenario: " + scenario);
        NLM_LOG_INFO("  Neurons: " + std::to_string(simConfig->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("  Steps: " + std::to_string(simConfig->getOr<int64_t>("max_simulation_steps", 1000)));
        
        // Create and initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(simConfig);
        
        if (!brain->initialize()) {
            result.success = false;
            result.message = "Failed to initialize brain";
            return result;
        }
        
        brain->logStatus();
        
        // Run simulation
        NLM_LOG_INFO("Running simulation...");
        auto progressReporter = ProgressReporter("Simulation", 
                                                 simConfig->getOr<int64_t>("max_simulation_steps", 1000));
        
        int64_t steps = simConfig->getOr<int64_t>("max_simulation_steps", 1000);
        for (int64_t step = 0; step < steps; ++step) {
            brain->step(step, simConfig->getOr<double>("simulation_timestep", 0.001));
            
            if (config.verbose || (step % 100 == 0)) {
                progressReporter.update(step, 
                    "Step " + std::to_string(step) + ": " +
                    std::to_string(brain->getFiringNeuronCount()) + " neurons firing, " +
                    std::to_string(brain->getTotalSpikeCount()) + " total spikes");
            }
        }
        
        progressReporter.complete("Simulation completed");
        
        brain->logStatus();
        
        // Generate output
        std::ostringstream output;
        output << "=== Simulation Results ===" << std::endl;
        output << "Scenario: " << scenario << std::endl;
        output << "Final Neurons: " << brain->getTotalNeuronCount() << std::endl;
        output << "Final Synapses: " << brain->getTotalSynapseCount() << std::endl;
        output << "Final Spikes: " << brain->getTotalSpikeCount() << std::endl;
        output << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        output << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        result.success = true;
        result.message = "Simulation completed successfully";
        result.output = output.str();
        
        return result;
    }
};

} // namespace nlm