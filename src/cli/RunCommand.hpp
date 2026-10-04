#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "performance/StatusManager.hpp"
#include <thread>
#include <chrono>
#include <atomic>

namespace nlm {

class RunCommand : public SimpleCLICommand {
public:
    RunCommand() : SimpleCLICommand(
        "run",
        "Run neural simulation with current configuration",
        "nlm run [--steps <steps>] [--timestep <timestep>] [--paused] [--dry-run]",
        "Run the neural simulation with the current configuration.\n\n"
        "Options:\n"
        "  --steps <steps>      Number of simulation steps to run (default: 1000)\n"
        "  --timestep <timestep> Simulation timestep (default: 0.001)\n"
        "  --paused             Start simulation paused\n"
        "  --dry-run            Preview simulation without running\n"
        "  --progress           Show progress indicators\n"
        "  --benchmark          Run benchmark mode\n\n"
        "Examples:\n"
        "  nlm run\n"
        "  nlm run --steps 5000 --timestep 0.001\n"
        "  nlm run --paused --progress"
    ) {
        options = {
            {"steps", "", "Number of simulation steps to run", OptionType::RequiredValue, false, "1000"},
            {"timestep", "", "Simulation timestep", OptionType::OptionalValue, false, "0.001"},
            {"paused", "", "Start simulation paused", OptionType::Flag},
            {"dry-run", "", "Preview simulation without running", OptionType::Flag},
            {"progress", "", "Show progress indicators", OptionType::Flag},
            {"benchmark", "", "Run benchmark mode", OptionType::Flag},
            {"output", "", "Output file for simulation results", OptionType::OptionalValue}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get options
        std::string stepsStr = getOptionValue(args, "steps");
        std::string timestepStr = getOptionValue(args, "timestep");
        bool paused = hasOption(args, "paused");
        bool dryRun = hasOption(args, "dry-run");
        bool progress = hasOption(args, "progress");
        bool benchmark = hasOption(args, "benchmark");
        std::string outputFile = getOptionValue(args, "output");
        
        // Parse parameters
        int steps = std::stoi(stepsStr);
        double timestep = std::stod(timestepStr);
        
        // Show configuration preview
        if (dryRun) {
            CLIFormatters::printInfo("Dry run mode - simulation preview:");
            showConfigurationPreview(steps, timestep);
            return 0;
        }
        
        // Get configuration
        auto config = getConfig();
        
        // Override configuration with command line options
        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
        config->set("max_steps", static_cast<int64_t>(steps), ConfigSource::CommandLine);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            errorHandler.addError("Failed to initialize brain");
            return errorHandler.exitWithError();
        }
        
        CLIFormatters::printSuccess("Brain initialized successfully");
        
        // Create progress reporter
        ProgressReporter progressReporter;
        if (progress) {
            progressReporter.startProgress("Running neural simulation", steps);
        }
        
        // Run benchmark if requested
        if (benchmark) {
            return runBenchmark(brain, steps, timestep, progressReporter);
        }
        
        // Run simulation
        double elapsedTime = runSimulation(brain, steps, timestep, paused, progressReporter);
        
        // Save results if requested
        if (!outputFile.empty()) {
            saveResults(brain, outputFile);
        }
        
        CLIFormatters::printSuccess("Simulation completed successfully");
        CLIFormatters::printInfo("Elapsed time: " + CLIFormatters::formatDuration(elapsedTime));
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        // Validate steps parameter
        for (const auto& arg : args) {
            if (arg.find("--steps") != std::string::npos && arg != "--steps") {
                try {
                    std::stoi(arg.substr(arg.find(" ") + 1));
                } catch (...) {
                    return false;
                }
            }
        }
        
        // Validate timestep parameter
        for (const auto& arg : args) {
            if (arg.find("--timestep") != std::string::npos && arg != "--timestep") {
                try {
                    std::stod(arg.substr(arg.find(" ") + 1));
                } catch (...) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    double runSimulation(std::shared_ptr<Brain> brain, int steps, double timestep,
                         bool paused, ProgressReporter& progressReporter) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (paused) {
            CLIFormatters::printInfo("Simulation paused - press Enter to continue...");
            std::cin.get();
        }
        
        for (int step = 0; step < steps; ++step) {
            // Update progress
            if (progressReporter.isActive()) {
                progressReporter.updateProgress(static_cast<double>(step) / steps * 100.0);
            }
            
            // Run simulation step
            brain->step(step, timestep);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                CLIFormatters::printInfo("Step " + std::to_string(step) + 
                                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                        " | Neurons: " + std::to_string(brain->getTotalNeuronCount()));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return duration;
    }
    
    int runBenchmark(std::shared_ptr<Brain> brain, int steps, double timestep,
                     ProgressReporter& progressReporter) {
        CLIFormatters::printInfo("Running benchmark mode...");
        
        // Run multiple iterations
        int iterations = 10;
        double totalTime = 0.0;
        
        for (int iteration = 0; iteration < iterations; ++iteration) {
            CLIFormatters::printInfo("Iteration " + std::to_string(iteration + 1) + "/" + std::to_string(iterations));
            
            double elapsedTime = runSimulation(brain, steps, timestep, false, progressReporter);
            totalTime += elapsedTime;
            
            // Reset brain for next iteration
            brain->reset();
            brain->initialize();
            
            // Small delay between iterations
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        double avgTime = totalTime / iterations;
        CLIFormatters::printSuccess("Benchmark completed");
        CLIFormatters::printInfo("Average time per iteration: " + CLIFormatters::formatDuration(avgTime));
        CLIFormatters::printInfo("Steps per second: " + std::to_string(steps / avgTime));
        
        return 0;
    }
    
    void showConfigurationPreview(int steps, double timestep) {
        auto config = getConfig();
        
        CLIFormatters::printInfo("Simulation Configuration Preview:");
        CLIFormatters::printInfo("  Steps: " + std::to_string(steps));
        CLIFormatters::printInfo("  Timestep: " + std::to_string(timestep) + " s");
        CLIFormatters::printInfo("  Total time: " + CLIFormatters::formatDuration(steps * timestep));
        
        // Show key configuration values
        auto keys = config->getKeys();
        CLIFormatters::printInfo("\nKey Configuration Values:");
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                CLIFormatters::printInfo("  " + key + " = " + value.value());
            }
        }
    }
    
    void saveResults(std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Save simulation results
        std::ofstream file(filepath, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filepath);
        }
        
        file << "Simulation Results\n";
        file << "===================\n\n";
        file << "Brain Statistics:\n";
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n\n";
        
        file.close();
        CLIFormatters::printInfo("Results saved to: " + filepath);
    }
    
    // Helper methods
    std::shared_ptr<Config> getConfig() {
        // Get the global configuration
        // This should be implemented based on how configuration is managed
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
};

} // namespace nlm
