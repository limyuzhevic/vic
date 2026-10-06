#!/usr/bin/env g++ -std=c++20 -O3 -march=native
// NLM Performance Profiler Utility
// Advanced performance monitoring and profiling for NLM simulations
// Provides detailed performance metrics and optimization insights

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Metrics.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <numeric>

using namespace nlm;

class PerformanceProfiler {
public:
    struct ProfilingData {
        SimulationStep step;
        Timestamp timestamp;
        double wallClockTime;
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t activeNeuronCount;
        uint64_t firingNeuronCount;
        double avgFiringRate;
        double excitationInhibitionRatio;
        double energyEfficiency;
        double memoryUsage;
        double processingSpeed;
        
        ProfilingData(SimulationStep s, Timestamp t, std::shared_ptr<Brain> brain)
            : step(s), timestamp(t), wallClockTime(0.0), 
              neuronCount(brain->getTotalNeuronCount()),
              synapseCount(brain->getTotalSynapseCount()),
              activeNeuronCount(brain->getActiveNeuronCount()),
              firingNeuronCount(brain->getFiringNeuronCount()),
              avgFiringRate(brain->getAverageFiringRate()),
              excitationInhibitionRatio(brain->getExcitationInhibitionRatio()),
              energyEfficiency(0.0), memoryUsage(0.0), processingSpeed(0.0) {}
    };
    
    std::shared_ptr<Brain> brain;
    std::vector<ProfilingData> profileData;
    double startWallClock;
    double totalProcessingTime;
    bool profilingEnabled;
    
    PerformanceProfiler(std::shared_ptr<Brain> b) 
        : brain(b), startWallClock(0.0), totalProcessingTime(0.0), 
          profilingEnabled(false) {}
    
    void startProfiling() {
        std::cout << "=== Starting Performance Profiling ===" << std::endl;
        startWallClock = getWallClockTime();
        profilingEnabled = true;
        profileData.clear();
        std::cout << "Profiling enabled. Press Ctrl+C to stop manually." << std::endl;
    }
    
    void stopProfiling() {
        profilingEnabled = false;
        totalProcessingTime = getWallClockTime() - startWallClock;
        std::cout << "=== Performance Profiling Complete ===" << std::endl;
        std::cout << "Total wall clock time: " << std::fixed << std::setprecision(3) 
                  << totalProcessingTime << "s" << std::endl;
    }
    
    void recordProfileData(SimulationStep step, Timestamp time) {
        if (!profilingEnabled) return;
        
        auto now = getWallClockTime();
        auto data = ProfilingData(step, time, brain);
        
        // Calculate derived metrics
        data.wallClockTime = now - startWallClock;
        data.energyEfficiency = calculateEnergyEfficiency(data);
        data.memoryUsage = estimateMemoryUsage(data);
        data.processingSpeed = calculateProcessingSpeed(data);
        
        profileData.push_back(data);
    }
    
    void generatePerformanceReport(const std::string& filename) {
        std::cout << "=== Generating Performance Report ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Profiling Report\n";
        file << "==============================\n\n";
        
        if (profileData.empty()) {
            file << "No profiling data collected." << std::endl;
            file.close();
            return;
        }
        
        file << "Overview:\n";
        file << "  Total steps: " << profileData.size() << "\n";
        file << "  Wall clock time: " << std::fixed << std::setprecision(3) 
             << totalProcessingTime << "s\n";
        file << "  Average processing speed: " << std::fixed << std::setprecision(3) 
             << totalProcessingTime / profileData.size() << "s per step\n\n";
        
        // Time series analysis
        file << "Time Series Analysis:\n";
        file << "Step,WallClock,Neurons,Synapses,Active%,Firing%,FiringRate,E/I,Eff,Mem,Speed\n";
        
        for (size_t i = 0; i < profileData.size(); ++i) {
            const auto& data = profileData[i];
            file << data.step << ","
                 << std::fixed << std::setprecision(6) << data.wallClockTime << ","
                 << data.neuronCount << ","
                 << data.synapseCount << ","
                 << std::fixed << std::setprecision(2) << (data.activeNeuronCount * 100.0 / data.neuronCount) << ","
                 << std::fixed << std::setprecision(2) << (data.firingNeuronCount * 100.0 / data.neuronCount) << ","
                 << std::fixed << std::setprecision(4) << data.avgFiringRate << ","
                 << std::fixed << std::setprecision(4) << data.excitationInhibitionRatio << ","
                 << std::fixed << std::setprecision(6) << data.energyEfficiency << ","
                 << std::fixed << std::setprecision(6) << data.memoryUsage << ","
                 << std::fixed << std::setprecision(6) << data.processingSpeed << "\n";
        }
        
        // Statistical analysis
        file << "\nStatistical Summary:\n";
        
        auto neurons = extractColumn<double>(profileData, &ProfilingData::neuronCount);
        auto synapses = extractColumn<double>(profileData, &ProfilingData::synapseCount);
        auto activePct = extractColumn<double>(profileData, 
            [](const ProfilingData& d) { return d.activeNeuronCount * 100.0 / d.neuronCount; });
        auto firingPct = extractColumn<double>(profileData,
            [](const ProfilingData& d) { return d.firingNeuronCount * 100.0 / d.neuronCount; });
        auto firingRates = extractColumn<double>(profileData, &ProfilingData::avgFiringRate);
        auto eiRatio = extractColumn<double>(profileData, &ProfilingData::excitationInhibitionRatio);
        auto energyEff = extractColumn<double>(profileData, &ProfilingData::energyEfficiency);
        auto memoryUsage = extractColumn<double>(profileData, &ProfilingData::memoryUsage);
        auto speed = extractColumn<double>(profileData, &ProfilingData::processingSpeed);
        
        file << "  Neurons: " << statisticsSummary(neurons) << "\n";
        file << "  Synapses: " << statisticsSummary(synapses) << "\n";
        file << "  Active Neuron %: " << statisticsSummary(activePct) << "\n";
        file << "  Firing Neuron %: " << statisticsSummary(firingPct) << "\n";
        file << "  Firing Rate (Hz): " << statisticsSummary(firingRates) << "\n";
        file << "  E/I Ratio: " << statisticsSummary(eiRatio) << "\n";
        file << "  Energy Efficiency: " << statisticsSummary(energyEff) << "\n";
        file << "  Memory Usage (KB): " << statisticsSummary(memoryUsage) << "\n";
        file << "  Processing Speed (steps/s): " << statisticsSummary(speed) << "\n\n";
        
        // Bottleneck analysis
        file << "Bottleneck Analysis:\n";
        
        // Find maximum and minimum values
        double maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
        double minFiringRate = *std::min_element(firingRates.begin(), firingRates.end());
        double maxEI = *std::max_element(eiRatio.begin(), eiRatio.end());
        double minEI = *std::min_element(eiRatio.begin(), eiRatio.end());
        
        file << "  Firing Rate Extremes: " << std::fixed << std::setprecision(4) 
             << minFiringRate << " - " << maxFiringRate << " Hz\n";
        file << "  E/I Ratio Extremes: " << std::fixed << std::setprecision(4) 
             << minEI << " - " << maxEI << "\n";
        
        // Identify critical points
        double avgFiringRate = calculateMean(firingRates);
        double avgEI = calculateMean(eiRatio);
        
        file << "\nCritical Performance Points:\n";
        
        // Find when firing rate peaked
        size_t maxFiringIdx = std::max_element(firingRates.begin(), firingRates.end()) - firingRates.begin();
        if (maxFiringIdx < profileData.size()) {
            file << "  Maximum firing rate at step " << profileData[maxFiringIdx].step 
                 << ": " << std::fixed << std::setprecision(4) << maxFiringRate << " Hz\n";
        }
        
        // Find when E/I ratio peaked
        size_t maxEIIidx = std::max_element(eiRatio.begin(), eiRatio.end()) - eiRatio.begin();
        if (maxEIIidx < profileData.size()) {
            file << "  Maximum E/I ratio at step " << profileData[maxEIIidx].step 
                 << ": " << std::fixed << std::setprecision(4) << maxEI << "\n";
        }
        
        // Performance trends
        file << "\nPerformance Trends:\n";
        
        // Calculate linear regression for firing rate over time
        if (profileData.size() > 1) {
            double timeTrend = calculateLinearTrend(extractColumn<double>(profileData, 
                &ProfilingData::wallClockTime));
            file << "  Time trend for firing rate: " << std::fixed << std::setprecision(6) 
                 << timeTrend << " Hz/s\n";
        }
        
        file.close();
        std::cout << "Performance report written to: " << filename << std::endl;
    }
    
    void generateBenchmarkReport(const std::string& filename) {
        std::cout << "=== Generating Benchmark Report ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Benchmark\n";
        file << "=========================\n\n";
        
        file << "Benchmark Configuration:\n";
        if (brain) {
            auto config = brain->getConfig();
            file << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 0) << "\n";
            file << "  Region count: " << config->getOr<int64_t>("region_count", 0) << "\n";
            file << "  Connection probability: " << config->getOr<float>("connection_probability", 0.0f) << "\n";
            file << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "\n";
        }
        
        file << "\nBenchmark Results:\n";
        
        if (profileData.empty()) {
            file << "No benchmark data available. Run with profiling first." << std::endl;
        } else {
            file << "  Total steps processed: " << profileData.size() << "\n";
            file << "  Total wall clock time: " << std::fixed << std::setprecision(3) 
                 << totalProcessingTime << "s\n";
            file << "  Average steps per second: " << std::fixed << std::setprecision(3) 
                 << profileData.size() / totalProcessingTime << "\n";
            file << "  Average steps per millisecond: " << std::fixed << std::setprecision(3) 
                 << profileData.size() / (totalProcessingTime * 1000.0) << "\n\n";
            
            // Performance metrics
            auto neurons = extractColumn<double>(profileData, &ProfilingData::neuronCount);
            auto spikes = extractColumn<double>(profileData, 
                [](const ProfilingData& d) { return d.firingNeuronCount * d.avgFiringRate * 0.001; });  // estimate
            auto memory = extractColumn<double>(profileData, &ProfilingData::memoryUsage);
            
            file << "  Performance Metrics:\n";
            file << "    Neuron processing rate: " << std::fixed << std::setprecision(2) 
                 << calculateMean(neurons) / (totalProcessingTime / profileData.size()) << " neurons/s\n";
            file << "    Spike generation rate: " << std::fixed << std::setprecision(2) 
                 << calculateMean(spikes) / (totalProcessingTime / profileData.size()) << " spikes/s\n";
            file << "    Memory efficiency: " << std::fixed << std::setprecision(6) 
                 << calculateMean(memory) << " KB per step\n\n";
            
            // Recommendations
            file << "Recommendations:\n";
            
            if (profileData.size() >= 10) {
                // Check for performance degradation
                auto firstHalf = profileData.size() / 2;
                auto secondHalf = profileData.size() - firstHalf;
                
                auto firstProcessing = extractColumn<double>(profileData.begin(), profileData.begin() + firstHalf,
                    &ProfilingData::processingSpeed);
                auto secondProcessing = extractColumn<double>(profileData.begin() + firstHalf, profileData.end(),
                    &ProfilingData::processingSpeed);
                
                double avgFirst = calculateMean(firstProcessing);
                double avgSecond = calculateMean(secondProcessing);
                
                if (avgSecond < avgFirst * 0.9) {
                    file << "  WARNING: Performance degraded over time. Consider memory management.\n";
                } else {
                    file << "  Performance stable over time. Good scaling.\n";
                }
            }
            
            if (calculateMean(extractColumn<double>(profileData, &ProfilingData::avgFiringRate)) > 100.0) {
                file << "  Consider tuning synaptic parameters for lower firing rates.\n";
            }
        }
        
        file.close();
        std::cout << "Benchmark report written to: " << filename << std::endl;
    }
    
    void displayLiveProfiler() {
        std::cout << "=== Live Performance Profiler ===" << std::endl;
        
        if (profileData.empty()) {
            std::cout << "No data collected yet. Start profiling first." << std::endl;
            return;
        }
        
        const auto& latest = profileData.back();
        
        std::cout << "Current Status (Step " << latest.step << "):" << std::endl;
        std::cout << "  Wall clock time: " << std::fixed << std::setprecision(3) 
                  << latest.wallClockTime << "s" << std::endl;
        std::cout << "  Neurons: " << latest.neuronCount << std::endl;
        std::cout << "  Synapses: " << latest.synapseCount << std::endl;
        std::cout << "  Active neurons: " << latest.activeNeuronCount << " ("
                  << std::fixed << std::setprecision(1) << (latest.activeNeuronCount * 100.0 / latest.neuronCount)
                  << "%)" << std::endl;
        std::cout << "  Firing neurons: " << latest.firingNeuronCount << " ("
                  << std::fixed << std::setprecision(1) << (latest.firingNeuronCount * 100.0 / latest.neuronCount)
                  << "%)" << std::endl;
        std::cout << "  Firing rate: " << std::fixed << std::setprecision(4) 
                  << latest.avgFiringRate << " Hz" << std::endl;
        std::cout << "  E/I ratio: " << std::fixed << std::setprecision(4) 
                  << latest.excitationInhibitionRatio << std::endl;
        std::cout << "  Energy efficiency: " << std::fixed << std::setprecision(6) 
                  << latest.energyEfficiency << std::endl;
        std::cout << "  Processing speed: " << std::fixed << std::setprecision(6) 
                  << latest.processingSpeed << " steps/s" << std::endl;
        std::cout << "  Memory usage: " << std::fixed << std::setprecision(6) 
                  << latest.memoryUsage << " KB" << std::endl;
    }
    
private:
    double getWallClockTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
    
    double calculateEnergyEfficiency(const ProfilingData& data) {
        // Simplified energy efficiency calculation
        double activeFraction = static_cast<double>(data.activeNeuronCount) / data.neuronCount;
        double firingFraction = static_cast<double>(data.firingNeuronCount) / data.neuronCount;
        return activeFraction * firingFraction * 100.0; // percentage
    }
    
    double estimateMemoryUsage(const ProfilingData& data) {
        // Rough memory usage estimation (bytes to KB)
        size_t neuronMemory = data.neuronCount * sizeof(Neuron) * 3; // voltage, threshold, state
        size_t synapseMemory = data.synapseCount * sizeof(Synapse) * 2; // weight, type
        return (neuronMemory + synapseMemory) / 1024.0; // KB
    }
    
    double calculateProcessingSpeed(const ProfilingData& data) {
        // Steps per second based on data collection interval
        if (profileData.size() < 2) return 0.0;
        double timeInterval = profileData.back().wallClockTime - profileData.front().wallClockTime;
        if (timeInterval <= 0.0) return 0.0;
        return (profileData.size() - 1) / timeInterval;
    }
    
    template<typename T>
    std::vector<T> extractColumn(const std::vector<ProfilingData>& data, 
        T (ProfilingData::*member)() const) {
        std::vector<T> column;
        column.reserve(data.size());
        for (const auto& d : data) {
            column.push_back((d.*member)());
        }
        return column;
    }
    
    template<typename T, typename Func>
    std::vector<T> extractColumn(const std::vector<ProfilingData>& data, Func func) {
        std::vector<T> column;
        column.reserve(data.size());
        for (const auto& d : data) {
            column.push_back(func(d));
        }
        return column;
    }
    
    std::string statisticsSummary(const std::vector<double>& values) {
        if (values.empty()) return "N/A";
        
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());
        double mean = calculateMean(values);
        double stddev = calculateStdDev(values, mean);
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4) << "mean=" << mean << ", stddev=" << stddev 
           << ", min=" << min << ", max=" << max;
        return ss.str();
    }
    
    double calculateMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }
    
    double calculateStdDev(const std::vector<double>& values, double mean) {
        if (values.size() <= 1) return 0.0;
        
        double sum = 0.0;
        for (double v : values) {
            sum += (v - mean) * (v - mean);
        }
        return std::sqrt(sum / (values.size() - 1));
    }
    
    double calculateLinearTrend(const std::vector<double>& xValues) {
        if (xValues.size() <= 1) return 0.0;
        
        // Calculate linear regression slope
        size_t n = xValues.size();
        double sumX = std::accumulate(xValues.begin(), xValues.end(), 0.0);
        double sumY = std::accumulate(xValues.begin(), xValues.end(), 0.0);
        double sumXY = 0.0, sumX2 = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            double x = xValues[i];
            sumXY += x * x;
            sumX2 += x;
        }
        
        double slope = (n * sumXY - sumX * sumX) / (n * sumX2 - sumX * sumX);
        return slope;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Performance Profiler (Advanced Tool) ===" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string reportFile = "";
    std::string benchmarkFile = "";
    std::string liveDisplay = "false";
    int profileSteps = 100;
    double customTimestep = 0.001;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--steps" && i + 1 < argc) {
            profileSteps = std::atoi(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            customTimestep = std::atof(argv[++i]);
        } else if (arg == "--report" && i + 1 < argc) {
            reportFile = argv[++i];
        } else if (arg == "--benchmark" && i + 1 < argc) {
            benchmarkFile = argv[++i];
        } else if (arg == "--live") {
            liveDisplay = "true";
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_perf_profiler [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)\n";
            std::cout << "  --steps <n>          Number of profiling steps (default: 100)\n";
            std::cout << "  --timestep <t>       Custom simulation timestep (default: 0.001)\n";
            std::cout << "  --report <file>      Generate profiling report to file\n";
            std::cout << "  --benchmark <file>   Generate benchmark report to file\n";
            std::cout << "  --live               Enable live display during profiling\n";
            std::cout << "  --help               Show this help\n";
            return 0;
        }
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set performance profiling parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", customTimestep, ConfigSource::Default);
    
    // Initialize brain
    std::cout << "Initializing NLM Brain for performance profiling..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create performance profiler
    PerformanceProfiler profiler(brain);
    
    // Start profiling
    profiler.startProfiling();
    
    // Enable neuromodulation for more realistic activity
    brain->getAgentBrainInterface()->enableRewardModulation(true);
    
    // Run profiling steps
    std::cout << "Running " << profileSteps << " profiling steps..." << std::endl;
    
    for (SimulationStep step = 0; step < profileSteps; ++step) {
        // Simulate some activity
        brain->step(step, step * customTimestep);
        
        // Apply some input to generate interesting dynamics
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        }
        
        // Update development
        brain->getDevelopmentSystem()->update(0.001);
        
        // Record profiling data
        profiler.recordProfileData(step, step * customTimestep);
        
        // Live display if requested
        if (liveDisplay == "true" && step % 10 == 0) {
            profiler.displayLiveProfiler();
            std::cout << std::endl;
        }
        
        // Log progress
        if (step % 20 == 0) {
            std::cout << "Step " << step << "/" << profileSteps << " completed." << std::endl;
        }
    }
    
    // Stop profiling
    profiler.stopProfiling();
    
    // Generate reports if requested
    if (!reportFile.empty()) {
        profiler.generatePerformanceReport(reportFile);
    }
    
    if (!benchmarkFile.empty()) {
        profiler.generateBenchmarkReport(benchmarkFile);
    }
    
    // Display final statistics
    std::cout << "\n=== Final Performance Statistics ===" << std::endl;
    std::cout << "Total steps processed: " << profileSteps << std::endl;
    std::cout << "Total wall clock time: " << std::fixed << std::setprecision(3) 
              << profiler.totalProcessingTime << "s" << std::endl;
    std::cout << "Average steps per second: " << std::fixed << std::setprecision(3) 
              << profileSteps / profiler.totalProcessingTime << std::endl;
    std::cout << "Average processing time per step: " << std::fixed << std::setprecision(6) 
              << profiler.totalProcessingTime / profileSteps << "s" << std::endl;
    
    std::cout << "\n=== Performance Profiling Complete ===" << std::endl;
    return 0;
}
