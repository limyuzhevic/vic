// NLM Performance Profiling Tool
// Advanced profiling and analysis for NLM neural simulation

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <fstream>

using namespace nlm;

// Enhanced profiling results structure
struct ProfilingResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    size_t totalSpikes;
    double meanNeuronVoltage;
    double maxNeuronVoltage;
    double minNeuronVoltage;
    std::vector<double> firingRates;
    std::map<std::string, double> resourceUsage;
};

// Profile different aspects of the simulation
ProfilingResult profilePerformance(size_t neuronCount, size_t steps, const std::string& profileMode) {
    std::cout << "  Running profile: " << neuronCount << " neurons, " 
              << steps << " steps (mode: " << profileMode << ")..." << std::flush;
    
    // Create configuration with enhanced settings
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default); // Higher connectivity for realistic profile
    config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
    
    // Create brain with profiling mode
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Track voltages for profiling
    std::vector<double> voltageSamples;
    voltageSamples.reserve(neuronCount);
    
    // Run simulation with enhanced monitoring
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Apply different input patterns based on profile mode
        if (profileMode == "bursty") {
            // Bursty input pattern
            if (step % 10 == 0) {
                for (size_t i = 0; i < std::min(size_t(20), neuronCount / 5); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f + (rand() % 30));
                }
            }
        } else if (profileMode == "constant") {
            // Constant input
            for (size_t i = 0; i < std::min(size_t(15), neuronCount / 8); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
        } else if (profileMode == "random") {
            // Random input bursts
            if (rand() % 5 == 0) {
                for (size_t i = 0; i < std::min(size_t(10), neuronCount / 12); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f + (rand() % 25));
                }
            }
        } else {
            // Standard input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Collect voltage samples every 100 steps for analysis
        if (step % 100 == 0) {
            double meanVoltage = 0.0;
            for (NeuronId i = 0; i < neuronCount; ++i) {
                if (auto* neuron = brain->getNeuron(i)) {
                    meanVoltage += neuron->getVoltage();
                }
            }
            meanVoltage /= static_cast<double>(neuronCount);
            voltageSamples.push_back(meanVoltage);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect comprehensive results
    ProfilingResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.totalSpikes = brain->getTotalSpikeCount();
    result.spikesPerStep = static_cast<double>(result.totalSpikes) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Calculate voltage statistics
    if (!voltageSamples.empty()) {
        double sum = std::accumulate(voltageSamples.begin(), voltageSamples.end(), 0.0);
        result.meanNeuronVoltage = sum / voltageSamples.size();
        result.maxNeuronVoltage = *std::max_element(voltageSamples.begin(), voltageSamples.end());
        result.minNeuronVoltage = *std::min_element(voltageSamples.begin(), voltageSamples.end());
        
        // Calculate firing rates for different neuron types
        result.firingRates = calculateFiringRates(brain);
    }
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    
    return result;
}

// Calculate detailed firing rates for different neuron types
std::vector<double> calculateFiringRates(std::shared_ptr<Brain> brain) {
    std::vector<double> firingRates;
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Count spikes per neuron type (simplified)
    std::map<NeuronType, size_t> typeSpikes;
    std::map<NeuronType, size_t> typeCount;
    
    // This is a simplified version - in practice you'd want to track per-neuron spikes
    for (NeuronId i = 0; i < neuronCount; ++i) {
        if (auto* neuron = brain->getNeuron(i)) {
            // Get neuron type (simplified - would need access to actual type)
            NeuronType type = NeuronType::Excitatory; // Placeholder
            typeCount[type]++;
        }
    }
    
    return firingRates;
}

// Print detailed profiling results
void printProfilingResults(const std::vector<ProfilingResult>& results, const std::string& mode) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          NLM Advanced Profiling Results                           ║" << std::endl;
    std::cout << "║                               Profile Mode: " << std::setw(30) << std::left << mode << "║" << std::endl;
    std::cout << "╠══════════════╦═══════════════╦═══════════╦════════════╦═══════════════╦═════════════╣" << std::endl;
    std::cout << "║    Neurons   ║    Synapses   ║ Steps/sec  ║ Spikes/step ║  Total Spikes  ║  Time (s)   ║" << std::endl;
    std::cout << "╠══════════════╬═══════════════╬═══════════╬════════════╬═══════════════╬═════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(12) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(12) << r.totalSpikes
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.totalTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚══════════════╩═══════════════╩═══════════╩════════════╩═══════════════╩═════════════╝" << std::endl;
    std::cout << std::endl;
}

// Generate profiling report to file
void generateProfilingReport(const std::vector<ProfilingResult>& results, const std::string& filename, const std::string& mode) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "NLM Performance Profiling Report" << std::endl;
    file << "=================================" << std::endl;
    file << "Profile Mode: " << mode << std::endl;
    file << "Generated: " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    file << "Benchmark Results:" << std::endl;
    file << "Neurons, Synapses, Steps, TotalTime, Steps/sec, Spikes/step, TotalSpikes" << std::endl;
    
    for (const auto& r : results) {
        file << r.neurons << ","
             << r.synapses << ","
             << r.steps << ","
             << r.totalTime << ","
             << r.stepsPerSecond << ","
             << r.spikesPerStep << ","
             << r.totalSpikes << std::endl;
    }
    
    file << std::endl;
    file << "Summary Statistics:" << std::endl;
    
    if (!results.empty()) {
        double avgNeurons = std::accumulate(results.begin(), results.end(), 0.0,
            [](double sum, const ProfilingResult& r) { return sum + r.neurons; }) / results.size();
        double avgStepsPerSec = std::accumulate(results.begin(), results.end(), 0.0,
            [](double sum, const ProfilingResult& r) { return sum + r.stepsPerSecond; }) / results.size();
        double avgSpikesPerStep = std::accumulate(results.begin(), results.end(), 0.0,
            [](double sum, const ProfilingResult& r) { return sum + r.spikesPerStep; }) / results.size();
        
        file << "Average Neurons: " << avgNeurons << std::endl;
        file << "Average Steps/sec: " << avgStepsPerSec << std::endl;
        file << "Average Spikes/step: " << avgSpikesPerStep << std::endl;
    }
    
    file.close();
    std::cout << "Profiling report saved to: " << filename << std::endl;
}

// Get current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string mode = "standard";
    std::string outputFile = "";
    bool verbose = false;
    std::vector<std::pair<size_t, size_t>> profiles;
    
    // Default profile configurations
    std::vector<std::pair<size_t, size_t>> defaultProfiles = {
        {100, 1000},      // Small network
        {500, 1000},      // Medium network
        {1000, 500},      // Large network
        {2000, 200},      // Very large network
    };
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--help") {
            printUsage();
            return 0;
        } else {
            // Try to parse as neuron_count,steps (e.g., "500,1000")
            size_t commaPos = arg.find(',');
            if (commaPos != std::string::npos) {
                try {
                    size_t neurons = std::stoul(arg.substr(0, commaPos));
                    size_t steps = std::stoul(arg.substr(commaPos + 1));
                    profiles.emplace_back(neurons, steps);
                } catch (...) {
                    std::cout << "Warning: Invalid profile specification: " << arg << std::endl;
                }
            } else {
                std::cout << "Warning: Unknown argument: " << arg << std::endl;
            }
        }
    }
    
    // Use default profiles if none specified
    if (profiles.empty()) {
        profiles = defaultProfiles;
    }
    
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Performance Profiler                             ║
    ║                                                               ║
    ╚════════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "Starting NLM Performance Profiling..." << std::endl;
    std::cout << "Mode: " << mode << std::endl;
    std::cout << "Profiles to run: " << profiles.size() << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run all profiles
    std::vector<ProfilingResult> allResults;
    
    for (size_t i = 0; i < profiles.size(); ++i) {
        std::cout << "\n--- Profile " << (i + 1) << " of " << profiles.size() << " ---" << std::endl;
        auto result = profilePerformance(profiles[i].first, profiles[i].second, mode);
        allResults.push_back(result);
        
        if (verbose) {
            std::cout << "    Mean voltage: " << std::fixed << std::setprecision(3) << result.meanNeuronVoltage << std::endl;
            std::cout << "    Max voltage: " << std::fixed << std::setprecision(3) << result.maxNeuronVoltage << std::endl;
            std::cout << "    Min voltage: " << std::fixed << std::setprecision(3) << result.minNeuronVoltage << std::endl;
        }
    }
    
    // Print results
    printProfilingResults(allResults, mode);
    
    // Generate report if requested
    if (!outputFile.empty()) {
        generateProfilingReport(allResults, outputFile, mode);
    }
    
    std::cout << std::endl;
    std::cout << "Profiling complete. Processed " << profiles.size() << " profile configurations." << std::endl;
    
    return 0;
}

void printUsage() {
    std::cout << "NLM Performance Profiler Usage:" << std::endl;
    std::cout << "  profiler [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --mode <mode>        Profile mode: standard, bursty, constant, random (default: standard)" << std::endl;
    std::cout << "  --output <file>      Output report to file" << std::endl;
    std::cout << "  --verbose             Enable verbose output" << std::endl;
    std::cout << "  --help                Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Profile specifications (can be repeated):" << std::endl;
    std::cout << "  neurons,steps         Specify network size and duration (e.g., 500,1000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./profiler --mode bursty --output results.csv 100,1000 500,1000" << std::endl;
    std::cout << "  ./profiler --verbose --mode random" << std::endl;
}