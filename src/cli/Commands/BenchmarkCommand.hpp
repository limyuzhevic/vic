#pragma once

#include "NLMCLI.hpp"
#include "../src/benchmark.cpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class BenchmarkCommand : public ICommand {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Performance benchmarking"; }
    std::string getUsage() const override { return "benchmark [options]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        // No required arguments
        return true;
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli benchmark [options]" << std::endl;
        std::cout << "Run performance benchmarks on NLM." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --neurons <n>           Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --steps <n>             Number of steps (default: 1000)" << std::endl;
        std::cout << "  --repeats <n>           Number of benchmark runs (default: 4)" << std::endl;
        std::cout << "  --output <file>         Output file for results" << std::endl;
        std::cout << "  --log-level <level>     Logging level (debug, info, warning, error)" << std::endl;
        std::cout << "  --verbose, -v           Verbose output" << std::endl;
        std::cout << "  --quiet, -q             Quiet output" << std::endl;
        std::cout << "  --help, -h              Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printHelp();
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        // Parse options
        std::map<std::string, std::string> options;
        std::string outputFile;
        int neurons = 1000;
        int steps = 1000;
        int repeats = 4;
        
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
                
                if (key == "neurons") neurons = std::stoi(value);
                else if (key == "steps") steps = std::stoi(value);
                else if (key == "repeats") repeats = std::stoi(value);
                
            } else if (arg == "--output" && i + 1 < args.size()) {
                outputFile = args[++i];
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                result.success = true;
                result.exitCode = 0;
                return result;
            }
        }
        
        try {
            result = runBenchmark(neurons, steps, repeats, outputFile, config);
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error running benchmark: ") + e.what();
            result.exitCode = 3;
        }
        
        return result;
    }
    
private:
    CommandResult runBenchmark(int neurons,
                               int steps,
                               int repeats,
                               const std::string& outputFile,
                               const CLIConfig& config) {
        CommandResult result;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        LogLevel logLevel = LogLevel::Warning;  // Benchmark uses lower verbosity
        if (config.logLevel == "debug") logLevel = LogLevel::Debug;
        else if (config.logLevel == "info") logLevel = LogLevel::Info;
        else if (config.logLevel == "warning") logLevel = LogLevel::Warning;
        else if (config.logLevel == "error") logLevel = LogLevel::Error;
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        consoleLogger->setUseColors(!config.quiet);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Starting NLM performance benchmark");
        NLM_LOG_INFO("Configuration: " << neurons << " neurons, " << steps << " steps, " << repeats << " repeats");
        
        // Create configuration
        auto benchConfig = std::make_shared<Config>();
        benchConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        benchConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        benchConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        benchConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(benchConfig);
        
        if (!brain->initialize()) {
            result.success = false;
            result.message = "Failed to initialize brain for benchmarking";
            return result;
        }
        
        NLM_LOG_INFO("Brain initialized with " << brain->getTotalNeuronCount() << " neurons");
        
        // Run benchmark
        std::vector<BenchmarkResult> allResults;
        
        for (int repeat = 0; repeat < repeats; ++repeat) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Benchmark run " << repeat + 1 << "/" << repeats);
            
            // Reset brain for each run
            brain->reset();
            brain->initialize();
            
            auto result = runBenchmark(neurons, steps);
            allResults.push_back(result);
        }
        
        // Print summary
        printBenchmarkResults(allResults);
        
        // Save results if output file specified
        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << "NLM Performance Benchmark Results" << std::endl;
                outFile << "=================================" << std::endl;
                outFile << std::endl;
                
                outFile << "Configuration:" << std::endl;
                outFile << "  Neurons: " << neurons << std::endl;
                outFile << "  Steps: " << steps << std::endl;
                outFile << "  Repeats: " << repeats << std::endl;
                outFile << std::endl;
                
                outFile << "Results:" << std::endl;
                for (size_t i = 0; i < allResults.size(); ++i) {
                    const auto& r = allResults[i];
                    outFile << "Run " << (i + 1) << ": "
                          << r.neurons << " neurons, "
                          << r.steps << " steps, "
                          << r.stepsPerSecond << " steps/sec" << std::endl;
                }
                
                outFile << std::endl;
                outFile << "Average:" << std::endl;
                
                double totalStepsPerSec = 0.0;
                double totalSpikesPerStep = 0.0;
                double totalSynapsesPerNeuron = 0.0;
                
                for (const auto& r : allResults) {
                    totalStepsPerSec += r.stepsPerSecond;
                    totalSpikesPerStep += r.spikesPerStep;
                    totalSynapsesPerNeuron += r.synapsesPerNeuron;
                }
                
                double avgStepsPerSec = totalStepsPerSec / allResults.size();
                double avgSpikesPerStep = totalSpikesPerStep / allResults.size();
                double avgSynapsesPerNeuron = totalSynapsesPerNeuron / allResults.size();
                
                outFile << "  Steps/sec: " << avgStepsPerSec << std::endl;
                outFile << "  Spikes/step: " << avgSpikesPerStep << std::endl;
                outFile << "  Synapses/neuron: " << avgSynapsesPerNeuron << std::endl;
                
                outFile.close();
                
                NLM_LOG_INFO("Benchmark results saved to: " << outputFile);
            }
        }
        
        std::ostringstream output;
        output << "=== Benchmark Results ===" << std::endl;
        output << "Benchmark completed successfully" << std::endl;
        output << "Configuration: " << neurons << " neurons, " << steps << " steps, " << repeats << " repeats" << std::endl;
        output << "Total runs: " << allResults.size() << std::endl;
        
        double totalStepsPerSec = 0.0;
        for (const auto& r : allResults) {
            totalStepsPerSec += r.stepsPerSecond;
        }
        double avgStepsPerSec = totalStepsPerSec / allResults.size();
        
        output << "Average performance: " << avgStepsPerSec << " steps/sec" << std::endl;
        
        if (!outputFile.empty()) {
            output << "Results saved to: " << outputFile << std::endl;
        }
        
        result.success = true;
        result.message = "Benchmark completed successfully";
        result.output = output.str();
        
        return result;
    }
    
}; // class BenchmarkCommand

} // namespace nlm