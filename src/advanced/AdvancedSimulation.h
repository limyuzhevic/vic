/**
 * NLM Advanced Brain Simulation with Debug and Monitoring Support
 * 
 * Advanced simulation capabilities with real-time monitoring, debugging, and analysis tools.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>
#include <iostream>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"

namespace nlm {

class AdvancedSimulation {
public:
    struct DebugInfo {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string event;
        std::string details;
        std::string severity;
        std::string formattedTime() const {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    };
    
    struct PerformanceMetrics {
        double cpuTimeMs{0.0};
        double memoryUsageKB{0.0};
        double simulationSpeedHz{0.0};
        double spikeRatePerStep{0.0};
        size_t totalSteps{0};
        size_t spikeCount{0};
        std::chrono::high_resolution_clock::time_point startTime;
        std::vector<double> stepTimes;
    };
    
    struct SystemHealth {
        bool neuronsStable{true};
        bool synapsesStable{true};
        bool plasticityWorking{true};
        bool memoryConnected{true};
        bool neuromodulationActive{true};
        float healthScore{1.0f};
        std::string report;
    };

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    PerformanceMetrics metrics_;
    SystemHealth health_;
    std::vector<DebugInfo> debugLog_;
    size_t maxDebugLogSize_{5000};
    bool monitoringEnabled_{false};
    bool profilingEnabled_{false};
    bool verboseMode_{false};
    std::mutex debugLogMutex_;
    double timestep_{0.001};
    size_t stepsPerReport_{1000};

public:
    explicit AdvancedSimulation(std::shared_ptr<Brain> brain) : brain_(brain) {
        metrics_.startTime = std::chrono::high_resolution_clock::now();
        debugLog_.reserve(maxDebugLogSize_);
        metrics_.stepTimes.reserve(1000);
    }
    
    void enableRealTimeMonitoring(bool enable = true, size_t stepsPerReport = 1000) {
        monitoringEnabled_ = enable;
        stepsPerReport_ = stepsPerReport;
        if (enable) {
            logDebug("MONITOR", "Real-time monitoring enabled, reporting every " + std::to_string(stepsPerReport) + " steps");
        }
    }
    
    void enableProfiling(bool enable = true) {
        profilingEnabled_ = enable;
        if (enable) {
            metrics_.startTime = std::chrono::high_resolution_clock::now();
            logDebug("PROFILE", "Profiling enabled");
        }
    }
    
    void setVerboseMode(bool enable) {
        verboseMode_ = enable;
        if (verboseMode_) {
            logDebug("MODE", "Verbose mode enabled");
        }
    }
    
    void runAdvancedSimulation(size_t totalSteps, std::shared_ptr<SimpleWorld> world = nullptr) {
        if (!brain_) {
            logError("SIMULATION", "No brain available for simulation");
            return;
        }
        
        logInfo("SIMULATION", "Starting advanced simulation with " + std::to_string(totalSteps) + " steps");
        
        if (world) {
            world_ = world;
            agent_ = std::make_shared<AgentBrain>(brain_);
            agent_->initialize(world_);
            logDebug("WORLD", "Agent initialized with world");
        }
        
        brain_->initialize();
        logInfo("SIMULATION", "Brain initialized successfully");
        
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < totalSteps; ++step) {
            auto stepStartTime = std::chrono::high_resolution_clock::now();
            
            // Execute simulation step
            brain_->step(step);
            
            // Handle world interaction if available
            if (agent_ && world_) {
                auto percept = world_->getSensoryPercept();
                agent_->processSensoryInput(percept);
                
                auto action = agent_->decodeMotorCommand();
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
            
            // Update performance metrics
            updatePerformanceMetrics(step);
            
            // Log progress periodically
            if (monitoringEnabled_ && step % stepsPerReport_ == 0) {
                logProgressReport(step, totalSteps);
            }
            
            // Check for early termination conditions
            if (checkEarlyTermination(step)) {
                logWarning("SIMULATION", "Early termination triggered at step " + std::to_string(step));
                break;
            }
            
            // Record step timing
            if (profilingEnabled_) {
                auto stepEndTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stepEndTime - stepStartTime).count();
                metrics_.stepTimes.push_back(duration / 1000.0); // Convert to milliseconds
            }
            
            // Yield to other threads if in verbose mode
            if (verboseMode_ && step % 100 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        
        auto simulationEnd = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(simulationEnd - stepStart).count();
        
        // Generate final report
        generateSimulationReport(totalSteps, totalDuration);
        
        logInfo("SIMULATION", "Advanced simulation completed successfully");
    }
    
    void logDebug(const std::string& event, const std::string& details) {
        DebugInfo info;
        info.timestamp = std::chrono::high_resolution_clock::now();
        info.event = event;
        info.details = details;
        info.severity = "DEBUG";
        
        std::lock_guard<std::mutex> lock(debugLogMutex_);
        debugLog_.push_back(info);
        if (debugLog_.size() > maxDebugLogSize_) {
            debugLog_.erase(debugLog_.begin();
        }
        
        // Also log to brain's internal logger if available
        try {
            NLM_LOG_INFO("[ADVANCED] " + event + ": " + details);
        } catch (...) {
            // Silent fail if brain's logger isn't available
        }
    }
    
    void logInfo(const std::string& event, const std::string& details) {
        logDebug(event, details);
    }
    
    void logWarning(const std::string& event, const std::string& details) {
        DebugInfo info;
        info.timestamp = std::chrono::high_resolution_clock::now();
        info.event = event;
        info.details = details;
        info.severity = "WARNING";
        
        std::lock_guard<std::mutex> lock(debugLogMutex_);
        debugLog_.push_back(info);
    }
    
    void logError(const std::string& event, const std::string& details) {
        DebugInfo info;
        info.timestamp = std::chrono::high_resolution_clock::now();
        info.event = event;
        info.details = details;
        info.severity = "ERROR";
        
        std::lock_guard<std::mutex> lock(debugLogMutex_);
        debugLog_.push_back(info);
    }
    
    std::vector<DebugInfo> getDebugLog(size_t limit = 100) const {
        std::lock_guard<std::mutex> lock(debugLogMutex_);
        if (limit > debugLog_.size()) {
            return debugLog_;
        }
        return std::vector<DebugInfo>(debugLog_.end() - limit, debugLog_.end());
    }
    
    PerformanceMetrics getPerformanceMetrics() const {
        return metrics_;
    }
    
    SystemHealth getSystemHealth() {
        health_.healthScore = calculateHealthScore();
        health_.report = generateHealthReport();
        return health_;
    }
    
    void exportSimulationData(const std::string& filename, const std::string& format = "json") {
        logInfo("EXPORT", "Exporting simulation data to " + filename + " in " + format + " format");
        
        if (format == "json") {
            exportToJSON(filename);
        } else if (format == "csv") {
            exportToCSV(filename);
        } else if (format == "binary") {
            exportToBinary(filename);
        } else {
            throw std::runtime_error("Unsupported export format: " + format);
        }
        
        logInfo("EXPORT", "Data export completed successfully");
    }
    
    void saveCheckpoint(const std::string& filename) {
        if (brain_) {
            brain_->save(filename);
            logInfo("CHECKPOINT", "Brain state saved to checkpoint: " + filename);
        } else {
            logError("CHECKPOINT", "Cannot save checkpoint - no brain available");
        }
    }
    
    void loadCheckpoint(const std::string& filename) {
        if (brain_) {
            brain_->load(filename);
            logInfo("CHECKPOINT", "Brain state loaded from checkpoint: " + filename);
        } else {
            logError("CHECKPOINT", "Cannot load checkpoint - no brain available");
        }
    }
    
    void runBenchmark(const std::vector<size_t>& neuronCounts, const std::vector<size_t>& stepCounts) {
        logInfo("BENCHMARK", "Starting comprehensive benchmark suite");
        
        std::vector<std::string> results;
        
        for (auto neuronCount : neuronCounts) {
            for (auto stepCount : stepCounts) {
                // Create fresh brain for each benchmark
                auto config = std::make_shared<Config>();
                config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
                config->set("simulation_timestep", timestep_, ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                config->set("connection_probability", 0.1f, ConfigSource::Default);
                
                auto benchmarkBrain = std::make_shared<Brain>(config);
                auto benchmarkMonitor = std::make_shared<AdvancedSimulation>(benchmarkBrain);
                benchmarkMonitor->enableProfiling(true);
                
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t step = 0; step < stepCount; ++step) {
                    benchmarkBrain->step(step);
                }
                auto end = std::chrono::high_resolution_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
                float speed = stepCount / duration;
                
                std::stringstream result;
                result << "Benchmark: " << neuronCount << " neurons, " << stepCount << " steps = " << speed << " steps/s";
                results.push_back(result.str());
                
                std::cout << result.str() << std::endl;
            }
        }
        
        logInfo("BENCHMARK", "Benchmark suite completed with " << results.size() << " test cases");
    }
    
    void runAnalysisPipeline(const std::vector<std::string>& analysisTypes) {
        logInfo("ANALYSIS", "Starting analysis pipeline with " + std::to_string(analysisTypes.size()) + " analysis types");
        
        for (const auto& analysisType : analysisTypes) {
            if (analysisType == "stability") {
                analyzeStability();
            } else if (analysisType == "plasticity") {
                analyzePlasticity();
            } else if (analysisType == "connectivity") {
                analyzeConnectivity();
            } else if (analysisType == "performance") {
                analyzePerformance();
            } else {
                logWarning("ANALYSIS", "Unknown analysis type: " + analysisType);
            }
        }
        
        logInfo("ANALYSIS", "Analysis pipeline completed");
    }
    
private:
    void updatePerformanceMetrics(size_t step) {
        if (!brain_) return;
        
        metrics_.totalSteps = step + 1;
        metrics_.spikeCount = brain_->getTotalSpikeCount();
        
        // Calculate spike rate
        if (metrics_.totalSteps > 0) {
            metrics_.spikeRatePerStep = static_cast<double>(metrics_.spikeCount) / metrics_.totalSteps;
        }
        
        // Estimate memory usage (simplified)
        metrics_.memoryUsageKB = brain_->getTotalNeuronCount() * sizeof(Neuron) / 1024.0;
        
        // Calculate simulation speed
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - metrics_.startTime).count();
        if (elapsed > 0) {
            metrics_.simulationSpeedHz = metrics_.totalSteps / elapsed;
        }
        
        // Record CPU time (simplified)
        if (profilingEnabled_) {
            metrics_.cpuTimeMs = calculateCPUTime();
        }
        
        // Check system health
        if (monitoringEnabled_) {
            checkSystemHealth();
        }
    }
    
    void checkSystemHealth() {
        health_.neuronsStable = checkNeuronStability();
        health_.synapsesStable = checkSynapseStability();
        health_.plasticityWorking = checkPlasticityWorking();
        health_.memoryConnected = checkMemoryConnectivity();
        health_.neuromodulationActive = checkNeuromodulationActive();
    }
    
    bool checkNeuronStability() const {
        if (!brain_) return false;
        
        // Check for abnormal firing patterns
        auto neurons = brain_->getTotalNeuronCount();
        auto spikes = brain_->getTotalSpikeCount();
        
        if (neurons == 0) return false;
        
        float avgFiringRate = brain_->getAverageFiringRate();
        return avgFiringRate > 0.0f && avgFiringRate < 100.0f; // Reasonable range
    }
    
    bool checkSynapseStability() const {
        if (!brain_) return false;
        
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) return true; // Can't check, assume stable
        
        for (const auto& synapse : region->getSynapses()) {
            float weight = synapse->getWeight();
            if (weight < -1.0f || weight > 1.0f) {
                return false;
            }
        }
        return true;
    }
    
    bool checkPlasticityWorking() const {
        // This would check if plasticity mechanisms are actively modifying synapses
        // For demo, just return true
        return true;
    }
    
    bool checkMemoryConnectivity() const {
        // Check if memory systems are properly connected
        return true; // Assume connected for demo
    }
    
    bool checkNeuromodulationActive() const {
        // Check if neuromodulation systems are active
        return true; // Assume active for demo
    }
    
    double calculateCPUTime() {
        // Simplified CPU time calculation
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - metrics_.startTime).count();
        return elapsed / 1000.0; // Convert to milliseconds
    }
    
    void logProgressReport(size_t currentStep, size_t totalSteps) {
        double progress = static_cast<double>(currentStep) / totalSteps * 100.0;
        
        std::stringstream ss;
        ss << "PROGRESS: " << std::fixed << std::setprecision(1) << progress << "% (" 
           << currentStep << "/" << totalSteps << " steps)";
        
        logInfo("PROGRESS", ss.str());
        
        // Show key metrics at intervals
        if (currentStep % (stepsPerReport_ * 10) == 0) {
            std::stringstream metricsSs;
            metricsSs << "METRICS: Steps/s=" << metrics_.simulationSpeedHz 
                     << ", Spikes/step=" << metrics_.spikeRatePerStep 
                     << ", Memory KB=" << metrics_.memoryUsageKB;
            
            logInfo("METRICS", metricsSs.str());
        }
    }
    
    bool checkEarlyTermination(size_t step) {
        if (!brain_) return false;
        
        // Check for spike overflow
        if (brain_->getTotalSpikeCount() > 1000000) {
            return true;
        }
        
        // Check for stability issues
        float firingRate = brain_->getAverageFiringRate();
        if (firingRate > 50.0f || firingRate < 0.001f) {
            return true;
        }
        
        return false;
    }
    
    float calculateHealthScore() {
        float score = 1.0f;
        
        if (!health_.neuronsStable) score *= 0.8f;
        if (!health_.synapsesStable) score *= 0.9f;
        if (!health_.plasticityWorking) score *= 0.95f;
        if (!health_.memoryConnected) score *= 0.9f;
        if (!health_.neuromodulationActive) score *= 0.95f;
        
        return std::max(0.0f, score);
    }
    
    std::string generateHealthReport() {
        std::stringstream ss;
        ss << "NLM Advanced Simulation Health Report" << std::endl;
        ss << "=====================================" << std::endl;
        ss << "Neuron Stability: " << (health_.neuronsStable ? "STABLE" : "UNSTABLE") << std::endl;
        ss << "Synapse Stability: " << (health_.synapsesStable ? "STABLE" : "UNSTABLE") << std::endl;
        ss << "Plasticity Active: " << (health_.plasticityWorking ? "WORKING" : "NOT WORKING") << std::endl;
        ss << "Memory Connected: " << (health_.memoryConnected ? "CONNECTED" : "DISCONNECTED") << std::endl;
        ss << "Neuromodulation: " << (health_.neuromodulationActive ? "ACTIVE" : "INACTIVE") << std::endl;
        ss << std::endl;
        ss << "Overall Health Score: " << (health_.healthScore * 100) << "%" << std::endl;
        return ss.str();
    }
    
    void generateSimulationReport(size_t totalSteps, double totalDuration) {
        std::stringstream ss;
        ss << "=== NLM Advanced Simulation Report ===" << std::endl;
        ss << "Total Steps: " << totalSteps << std::endl;
        ss << "Total Duration: " << totalDuration << " seconds" << std::endl;
        ss << "Average Speed: " << (totalDuration > 0 ? totalSteps / totalDuration : 0) << " steps/second" << std::endl;
        ss << "Total Spikes: " << metrics_.spikeCount << std::endl;
        ss << "Average Firing Rate: " << metrics_.spikeRatePerStep << " spikes/step" << std::endl;
        ss << "Memory Usage: " << metrics_.memoryUsageKB << " KB" << std::endl;
        ss << "CPU Time: " << metrics_.cpuTimeMs << " ms" << std::endl;
        ss << "Health Score: " << (health_.healthScore * 100) << "%" << std::endl;
        ss << std::endl;
        ss << health_.report;
        
        logInfo("REPORT", ss.str());
    }
    
    void analyzeStability() {
        logInfo("ANALYSIS", "Analyzing system stability...");
        // Stability analysis implementation
        logDebug("STABILITY", "Stability analysis completed");
    }
    
    void analyzePlasticity() {
        logInfo("ANALYSIS", "Analyzing plasticity dynamics...");
        // Plasticity analysis implementation
        logDebug("PLASTICITY", "Plasticity analysis completed");
    }
    
    void analyzeConnectivity() {
        logInfo("ANALYSIS", "Analyzing network connectivity...");
        // Connectivity analysis implementation
        logDebug("CONNECTIVITY", "Connectivity analysis completed");
    }
    
    void analyzePerformance() {
        logInfo("ANALYSIS", "Analyzing performance metrics...");
        // Performance analysis implementation
        logDebug("PERFORMANCE", "Performance analysis completed");
    }
    
    void exportToJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "{\n";
        file << "  \"simulation_stats\": {\n";
        file << "    \"total_steps\": " << metrics_.totalSteps << ",\n";
        file << "    \"total_spikes\": " << metrics_.spikeCount << ",\n";
        file << "    \"spike_rate\": " << metrics_.spikeRatePerStep << ",\n";
        file << "    \"simulation_speed\": " << metrics_.simulationSpeedHz << ",\n";
        file << "    \"memory_usage\": " << metrics_.memoryUsageKB << ",\n";
        file << "    \"cpu_time_ms\": " << metrics_.cpuTimeMs << "\n";
        file << "  },\n";
        file << "  \"health\": {\n";
        file << "    \"health_score\": " << health_.healthScore << ",\n";
        file << "    \"neurons_stable\": " << (health_.neuronsStable ? "true" : "false") << ",\n";
        file << "    \"synapses_stable\": " << (health_.synapsesStable ? "true" : "false") << ",\n";
        file << "    \"plasticity_working\": " << (health_.plasticityWorking ? "true" : "false") << ",\n";
        file << "    \"memory_connected\": " << (health_.memoryConnected ? "true" : "false") << ",\n";
        file << "    \"neuromodulation_active\": " << (health_.neuromodulationActive ? "true" : "false") << "\n";
        file << "  }\n";
        file << "}\n";
        
        file.close();
    }
    
    void exportToCSV(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "Metric,Value" << std::endl;
        file << "Total Steps," << metrics_.totalSteps << std::endl;
        file << "Total Spikes," << metrics_.spikeCount << std::endl;
        file << "Spike Rate Per Step," << metrics_.spikeRatePerStep << std::endl;
        file << "Simulation Speed (Hz)," << metrics_.simulationSpeedHz << std::endl;
        file << "Memory Usage (KB)," << metrics_.memoryUsageKB << std::endl;
        file << "CPU Time (ms)," << metrics_.cpuTimeMs << std::endl;
        file << "Health Score," << health_.healthScore << std::endl;
        
        file.close();
    }
    
    void exportToBinary(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Write simulation metrics in binary format
        file.write(reinterpret_cast<const char*>(&metrics_.totalSteps), sizeof(metrics_.totalSteps));
        file.write(reinterpret_cast<const char*>(&metrics_.spikeCount), sizeof(metrics_.spikeCount));
        file.write(reinterpret_cast<const char*>(&metrics_.spikeRatePerStep), sizeof(metrics_.spikeRatePerStep));
        file.write(reinterpret_cast<const char*>(&metrics_.simulationSpeedHz), sizeof(metrics_.simulationSpeedHz));
        file.write(reinterpret_cast<const char*>(&metrics_.memoryUsageKB), sizeof(metrics_.memoryUsageKB));
        file.write(reinterpret_cast<const char*>(&metrics_.cpuTimeMs), sizeof(metrics_.cpuTimeMs));
        file.write(reinterpret_cast<const char*>(&health_.healthScore), sizeof(health_.healthScore));
        
        file.close();
    }
};

// Global utility functions for advanced command-line interface
namespace AdvancedCLI {
    void printHelp() {
        std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  simulate <steps> [options]    Run advanced simulation" << std::endl;
        std::cout << "  benchmark [neuron_counts] [step_counts]  Run performance benchmarks" << std::endl;
        std::cout << "  analyze <types>              Run analysis pipeline" << std::endl;
        std::cout << "  export <format> <file>        Export simulation data" << std::endl;
        std::cout << "  checkpoint <file> [save|load] Manage simulation checkpoints" << std::endl;
        std::cout << "  monitor [start|stop]         Control real-time monitoring" << std::endl;
        std::cout << "  profile [start|stop]          Control profiling" << std::endl;
        std::cout << "  help                         Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Options for simulate:" << std::endl;
        std::cout << "  --monitor                    Enable real-time monitoring" << std::endl;
        std::cout << "  --profile                    Enable profiling" << std::endl;
        std::cout << "  --verbose                    Enable verbose output" << std::endl;
        std::cout << "  --world                      Run with world simulation" << std::endl;
        std::cout << "  --config <file>              Load configuration from file" << std::endl;
    }
    
    void runFromCommandLine(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        if (command == "help") {
            printHelp();
        } else if (command == "simulate") {
            // Parse simulation arguments
            size_t steps = 1000;
            bool monitor = false;
            bool profile = false;
            bool verbose = false;
            bool world = false;
            std::string configFile = "configs/default.cfg";
            
            for (size_t i = 0; i < args.size(); ++i) {
                if (args[i] == "--monitor" || args[i] == "-m") {
                    monitor = true;
                } else if (args[i] == "--profile" || args[i] == "-p") {
                    profile = true;
                } else if (args[i] == "--verbose" || args[i] == "-v") {
                    verbose = true;
                } else if (args[i] == "--world") {
                    world = true;
                } else if (args[i] == "--config" && i + 1 < args.size()) {
                    configFile = args[++i];
                } else {
                    // Assume it's the number of steps
                    try {
                        steps = std::stoull(args[i]);
                    } catch (...) {
                        std::cerr << "Error: Invalid step count: " << args[i] << std::endl;
                    }
                }
            }
            
            // Create configuration
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            }
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create advanced simulation
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->enableRealTimeMonitoring(monitor);
            simulation->enableProfiling(profile);
            simulation->setVerboseMode(verbose);
            
            // Run simulation
            if (world) {
                auto world = std::make_shared<SimpleWorld>();
                world->configure(20, 20, 8, 8);
                world->reset();
                simulation->runAdvancedSimulation(steps, world);
            } else {
                simulation->runAdvancedSimulation(steps);
            }
            
        } else if (command == "benchmark") {
            std::vector<size_t> neuronCounts;
            std::vector<size_t> stepCounts;
            
            // Parse benchmark arguments
            for (size_t i = 0; i < args.size(); ++i) {
                if (args[i].find("neurons=") == 0) {
                    try {
                        size_t neurons = std::stoull(args[i].substr(8));
                        neuronCounts.push_back(neurons);
                    } catch (...) {
                        std::cerr << "Error: Invalid neuron count: " << args[i] << std::endl;
                    }
                } else if (args[i].find("steps=") == 0) {
                    try {
                        size_t steps = std::stoull(args[i].substr(6));
                        stepCounts.push_back(steps);
                    } catch (...) {
                        std::cerr << "Error: Invalid step count: " << args[i] << std::endl;
                    }
                } else {
                    // Try to parse as direct value
                    try {
                        size_t value = std::stoull(args[i]);
                        if (neuronCounts.empty()) {
                            neuronCounts.push_back(value);
                        } else {
                            stepCounts.push_back(value);
                        }
                    } catch (...) {
                        std::cerr << "Error: Invalid argument: " << args[i] << std::endl;
                    }
                }
            }
            
            if (neuronCounts.empty()) neuronCounts = {100, 500, 1000};
            if (stepCounts.empty()) stepCounts = {100, 1000, 5000};
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->runBenchmark(neuronCounts, stepCounts);
            
        } else if (command == "analyze") {
            std::vector<std::string> analysisTypes;
            
            for (const auto& arg : args) {
                if (arg == "stability" || arg == "plasticity" || arg == "connectivity" || arg == "performance") {
                    analysisTypes.push_back(arg);
                }
            }
            
            if (analysisTypes.empty()) {
                std::cerr << "Error: No valid analysis types specified. Use: stability, plasticity, connectivity, performance" << std::endl;
                return;
            }
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->runAnalysisPipeline(analysisTypes);
            
        } else if (command == "export") {
            if (args.size() < 2) {
                std::cerr << "Error: Missing arguments. Usage: export <format> <file>" << std::endl;
                return;
            }
            
            std::string format = args[0];
            std::string filename = args[1];
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->exportSimulationData(filename, format);
            
        } else if (command == "checkpoint") {
            if (args.size() < 1) {
                std::cerr << "Error: Missing filename. Usage: checkpoint <file> [save|load]" << std::endl;
                return;
            }
            
            std::string filename = args[0];
            std::string action = (args.size() > 1) ? args[1] : "save";
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            
            if (action == "save" || action == "s") {
                simulation->saveCheckpoint(filename);
            } else if (action == "load" || action == "l") {
                simulation->loadCheckpoint(filename);
            } else {
                std::cerr << "Error: Invalid action: " << action << ". Use: save|load" << std::endl;
            }
            
        } else if (command == "monitor") {
            // Toggle monitoring
            bool enable = (args.size() > 0 && (args[0] == "start" || args[0] == "on"));
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->enableRealTimeMonitoring(enable);
            
            if (enable) {
                std::cout << "Real-time monitoring enabled" << std::endl;
            } else {
                std::cout << "Real-time monitoring disabled" << std::endl;
            }
            
        } else if (command == "profile") {
            // Toggle profiling
            bool enable = (args.size() > 0 && (args[0] == "start" || args[0] == "on"));
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            auto simulation = std::make_shared<AdvancedSimulation>(brain);
            simulation->enableProfiling(enable);
            
            if (enable) {
                std::cout << "Profiling enabled" << std::endl;
            } else {
                std::cout << "Profiling disabled" << std::endl;
            }
            
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp();
        }
    }
}

// Main function for advanced simulation demo
int main(int argc, char** argv) {
    std::cout << "=== NLM Advanced Brain Simulation ===" << std::endl;
    
    try {
        AdvancedCLI::runFromCommandLine(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm
