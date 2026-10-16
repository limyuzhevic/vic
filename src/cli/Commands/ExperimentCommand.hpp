#pragma once

#include "NLMCLI.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class ExperimentCommand : public ICommand {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { return "Run predefined experiments"; }
    std::string getUsage() const override { return "experiment <name> [options]"; }
    std::string getUsage() const override { return "experiment <name> [options]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        return !args.empty();
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli experiment <name> [options]" << std::endl;
        std::cout << "Run a predefined experiment." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "  <name>                 Experiment name (Phase3Demo, Phase4Demo, Phase5Experiment, Phase6Demo, etc.)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --steps <n>            Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --repeats <n>          Number of times to repeat experiment (default: 1)" << std::endl;
        std::cout << "  --neuron_count <n>     Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --region_count <n>     Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --output <file>        Output file for results" << std::endl;
        std::cout << "  --record_metrics       Record performance metrics" << std::endl;
        std::cout << "  --log-level <level>    Logging level (debug, info, warning, error)" << std::endl;
        std::cout << "  --verbose, -v          Verbose output" << std::endl;
        std::cout << "  --quiet, -q            Quiet output" << std::endl;
        std::cout << "  --help, -h             Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printHelp();
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        std::string experimentName = args[0];
        std::vector<std::string> optionsArgs(args.begin() + 1, args.end());
        
        // Parse options
        std::map<std::string, std::string> options;
        std::string outputFile;
        bool recordMetrics = false;
        
        for (size_t i = 0; i < optionsArgs.size(); ++i) {
            std::string arg = optionsArgs[i];
            
            if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            } else if (arg == "--output" && i + 1 < optionsArgs.size()) {
                outputFile = optionsArgs[++i];
            } else if (arg == "--record_metrics") {
                recordMetrics = true;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                result.success = true;
                result.exitCode = 0;
                return result;
            }
        }
        
        try {
            result = runExperiment(experimentName, options, outputFile, recordMetrics, config);
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error running experiment: ") + e.what();
            result.exitCode = 3;
        }
        
        return result;
    }
    
private:
    CommandResult runExperiment(const std::string& experimentName,
                                const std::map<std::string, std::string>& options,
                                const std::string& outputFile,
                                bool recordMetrics,
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
        
        NLM_LOG_INFO("Starting experiment: " + experimentName);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("experiment.steps", std::stoll(options.count("steps") ? options.at("steps") : "1000"), ConfigSource::Default);
        config->set("neuron_count", std::stoll(options.count("neuron_count") ? options.at("neuron_count") : "1000"), ConfigSource::Default);
        config->set("region_count", std::stoll(options.count("region_count") ? options.at("region_count") : "1"), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("experiment.record_metrics", recordMetrics, ConfigSource::CommandLine);
        
        // Create experiment
        auto experimentRunner = std::make_shared<ExperimentRunner>();
        auto experiment = experimentRunner->createExperiment(experimentName);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            result.success = false;
            result.message = "Failed to initialize brain for experiment";
            return result;
        }
        
        NLM_LOG_INFO("Experiment created: " + experimentName);
        NLM_LOG_INFO("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        
        // Run experiment
        NLM_LOG_INFO("Running experiment...");
        
        int64_t steps = config->getOr<int64_t>("experiment.steps", 1000);
        int repeats = options.count("repeats") ? std::stoi(options.at("repeats")) : 1;
        
        for (int repeat = 0; repeat < repeats; ++repeat) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Experiment Run " + std::to_string(repeat + 1) + "/" + std::to_string(repeats) + " ===");
            
            // Reset brain for each repeat
            brain->reset();
            brain->initialize();
            
            // Run experiment steps
            for (int64_t step = 0; step < steps; ++step) {
                brain->step(step, config->getOr<double>("simulation_timestep", 0.001));
                
                if (config.verbose || (step % 100 == 0)) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                                 std::to_string(brain->getFiringNeuronCount()) + " neurons firing, " +
                                 std::to_string(brain->getTotalSpikeCount()) + " total spikes");
                }
            }
            
            NLM_LOG_INFO("Experiment run " + std::to_string(repeat + 1) + " completed");
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Experiment Results ===");
        NLM_LOG_INFO("Experiment: " + experimentName);
        NLM_LOG_INFO("Repeats: " + std::to_string(repeats));
        NLM_LOG_INFO("Steps per run: " + std::to_string(steps));
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Final spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Final firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        
        // Save results if output file specified
        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << "Experiment: " << experimentName << std::endl;
                outFile << "Repeats: " << repeats << std::endl;
                outFile << "Steps per run: " << steps << std::endl;
                outFile << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
                outFile << "Final spikes: " << brain->getTotalSpikeCount() << std::endl;
                outFile << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                outFile.close();
                
                NLM_LOG_INFO("Results saved to: " + outputFile);
            }
        }
        
        std::ostringstream output;
        output << "=== Experiment Results ===" << std::endl;
        output << "Experiment: " << experimentName << std::endl;
        output << "Repeats: " << repeats << std::endl;
        output << "Steps per run: " << steps << std::endl;
        output << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        output << "Final spikes: " << brain->getTotalSpikeCount() << std::endl;
        output << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        if (!outputFile.empty()) {
            output << "Results saved to: " << outputFile << std::endl;
        }
        
        result.success = true;
        result.message = "Experiment completed successfully";
        result.output = output.str();
        
        return result;
    }
    
}; // class ExperimentCommand

} // namespace nlm