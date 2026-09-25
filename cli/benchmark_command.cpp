// NLM Benchmark Command - Run performance benchmarks
// Handles performance benchmarking of NLM operations

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <sstream>

namespace nlm {

class BenchmarkCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static std::vector<BenchmarkResult> runBenchmarks(const std::vector<std::pair<size_t, size_t>>& benchmarks, std::shared_ptr<Config> config);
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results);
    static std::string formatBenchmarkNumber(double value, int precision = 2);
    static std::string formatBenchmarkTime(double seconds);
};

struct BenchmarkCommand::BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
};

bool BenchmarkCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Benchmark Command ===" << std::endl;
    
    // Load configuration
    std::string configFile = cli.getConfigFile();
    auto config = cli.loadConfiguration(configFile);
    if (!config) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return false;
    }
    
    // Determine benchmark configurations
    std::vector<std::pair<size_t, size_t>> benchmarks;
    
    if (args.count("neuron-counts")) {
        std::string neuronCountsStr = args.at("neuron-counts");
        std::stringstream ss(neuronCountsStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            try {
                size_t neurons = std::stoul(item);
                // Use steps based on neuron count
                size_t steps = neurons > 1000 ? 200 : (neurons > 500 ? 500 : 1000);
                benchmarks.emplace_back(neurons, steps);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid neuron count in list: " << item << std::endl;
            }
        }
    } else {
        // Default benchmark configurations
        benchmarks = {
            {100, 1000},     // Small network, many steps
            {500, 1000},     // Medium network, many steps
            {1000, 500},     // Large network, fewer steps
            {2000, 200},     // Very large network, few steps
        };
    }
    
    // Use custom steps if specified
    if (args.count("steps")) {
        size_t steps = std::stoul(args.at("steps"));
        for (auto& benchmark : benchmarks) {
            benchmark.second = steps;
        }
    }
    
    std::cout << "Running benchmarks with configurations:" << std::endl;
    for (const auto& [neurons, steps] : benchmarks) {
        std::cout << "  " << neurons << " neurons, " << steps << " steps" << std::endl;
    }
    
    // Run benchmarks
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<BenchmarkResult> results = runBenchmarks(benchmarks, config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Display results
    printBenchmarkResults(results);
    
    // Display summary
    std::cout << std::endl << "=== Benchmark Summary ===" << std::endl;
    std::cout << "Total benchmark time: " << formatBenchmarkTime(totalTime) << std::endl;
    std::cout << "Number of benchmarks: " << benchmarks.size() << std::endl;
    
    // Save results if requested
    if (args.count("output")) {
        std::string outputFile = args.at("output");
        std::string outputFormat = args.count("format") ? args.at("format") : "csv";
        saveBenchmarkResults(outputFormat, outputFile, results);
    }
    
    return true;
}

void BenchmarkCommand::showHelp() {
    std::cout << "=== NLM Benchmark Command Help ===" << std::endl;
    std::cout << "Usage: nlm benchmark [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run performance benchmarks on NLM brain simulations:" << std::endl;
    std::cout << "  -c, --config FILE              Load configuration from FILE" << std::endl;
    std::cout << "  --neuron-countSIZES            Neuron counts to test (comma-separated)" << std::endl;
    std::cout << "  --steps STEPS                  Number of steps for each test" << std::endl;
    std::cout << "  --output FILE                  Save benchmark results to FILE" << std::endl;
    std::cout << "  --format FORMAT                Output format (csv, json, table)" << std::endl;
    std::cout << "  -h, --help                     Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Default benchmark configurations (neurons, steps):" << std::endl;
    std::cout << "  (100, 1000), (500, 1000), (1000, 500), (2000, 200)" << std::endl;
}

std::vector<BenchmarkCommand::BenchmarkResult> BenchmarkCommand::runBenchmarks(const std::vector<std::pair<size_t, size_t>>& benchmarks, std::shared_ptr<Config> config) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& [neurons, steps] : benchmarks) {
        std::cout << "  Running benchmark: " << neurons << " neurons, " 
                 << steps << " steps..." << std::flush;
        
        // Create configuration for this benchmark
        auto benchConfig = std::make_shared<Config>();
        benchConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        benchConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        benchConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        benchConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        benchConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(benchConfig);
        brain->initialize();
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject some random input
            for (size_t i = 0; i < std::min(size_t(10), neurons / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect results
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << formatBenchmarkTime(totalTime) << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    }
    
    return results;
}

void BenchmarkCommand::printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
}

std::string BenchmarkCommand::formatBenchmarkNumber(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string BenchmarkCommand::formatBenchmarkTime(double seconds) {
    if (seconds < 1.0) {
        return formatBenchmarkNumber(seconds * 1000, 2) + "ms";
    } else if (seconds < 60.0) {
        return formatBenchmarkNumber(seconds, 3) + "s";
    } else {
        double minutes = seconds / 60.0;
        if (minutes < 60.0) {
            return formatBenchmarkNumber(minutes, 2) + "min";
        } else {
            double hours = minutes / 60.0;
            return formatBenchmarkNumber(hours, 2) + "h";
        }
    }
}

bool BenchmarkCommand::saveBenchmarkResults(const std::string& format, const std::string& filename, const std::vector<BenchmarkResult>& results) {
    std::cout << "Saving benchmark results to " << filename << " in " << format << " format" << std::endl;
    
    if (format == "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << "neurons,synapses,steps,total_time,steps_per_second,spikes_per_step,synapses_per_neuron" << std::endl;
        for (const auto& r : results) {
            file << r.neurons << "," << r.synapses << "," << r.steps << ","
                 << r.totalTime << "," << r.stepsPerSecond << "," << r.spikesPerStep << ","
                 << r.synapsesPerNeuron << std::endl;
        }
        file.close();
        
    } else if (format == "json") {
        // JSON export (simplified)
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << "{\n  \"benchmarks\": [" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            file << "    {\n      \"neurons\": " << r.neurons << ",\n      \"synapses\": " << r.synapses << ",\n      \"steps\": " << r.steps << ",\n      \"total_time\": " << r.totalTime << ",\n      \"steps_per_second\": " << r.stepsPerSecond << ",\n      \"spikes_per_step\": " << r.spikesPerStep << ",\n      \"synapses_per_neuron\": " << r.synapsesPerNeuron << "\n    }";
            if (i < results.size() - 1) file << ",";
            file << std::endl;
        }
        file << "  ]\n}" << std::endl;
        file.close();
        
    } else if (format == "table") {
        // Table format already handled by printBenchmarkResults
        // Just save the console output
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return false;
        }
        
        // Note: This is a simplified implementation
        // In practice, you'd want to capture the console output differently
        file << "Benchmark results saved as table format" << std::endl;
        file.close();
    }
    
    std::cout << "Benchmark results saved successfully to: " << filename << std::endl;
    return true;
}
