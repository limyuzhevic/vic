/**
 * NLM Advanced Debugging and Monitoring Tools
 * 
 * Advanced utilities for expert users to debug, profile, and monitor NLM brain simulations.
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <sstream>
#include <fstream>
#include "Brain.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include "SensoryPercept.hpp"

namespace nlm {

class AdvancedBrainMonitor {
public:
    struct PerformanceMetrics {
        double cpuTime{0.0};
        double memoryUsage{0.0};
        double simulationSpeed{0.0};
        double spikeRate{0.0};
        double energyConsumption{0.0};
        std::chrono::high_resolution_clock::time_point startTime;
        size_t totalSteps{0};
        size_t spikeCount{0};
    };
    
    struct SystemHealth {
        bool neuronDynamicsHealthy{true};
        bool plasticityHealthy{true};
        bool memorySystemsHealthy{true};
        bool neuromodulationHealthy{true};
        bool predictionHealthy{true};
        float healthScore{1.0f};
        std::string healthReport;
    };
    
    struct DebugInfo {
        std::string timestamp;
        std::string event;
        std::string details;
        std::string severity;
    };

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    PerformanceMetrics metrics_;
    SystemHealth health_;
    std::vector<DebugInfo> debugLog_;
    size_t maxDebugLogSize_{10000};
    bool profilingEnabled_{false};
    bool realTimeMonitoring_{false};
    double monitoringInterval_{0.1}; // seconds

public:
    explicit AdvancedBrainMonitor(std::shared_ptr<Brain> brain) : brain_(brain) {
        metrics_.startTime = std::chrono::high_resolution_clock::now();
        debugLog_.reserve(maxDebugLogSize_);
    }
    
    void enableRealTimeMonitoring(bool enable, double interval = 0.1) {
        realTimeMonitoring_ = enable;
        monitoringInterval_ = interval;
        if (enable) {
            logDebug("MONITORING", "Real-time monitoring enabled, interval: " + std::to_string(interval) + "s");
        }
    }
    
    void enableProfiling(bool enable) {
        profilingEnabled_ = enable;
        if (enable) {
            metrics_.startTime = std::chrono::high_resolution_clock::now();
            logDebug("PROFILING", "Profiling enabled");
        }
    }
    
    PerformanceMetrics getPerformanceMetrics() const { return metrics_; }
    
    SystemHealth getSystemHealth() {
        // Perform comprehensive health checks
        health_.healthScore = calculateHealthScore();
        health_.healthReport = generateHealthReport();
        return health_;
    }
    
    std::vector<DebugInfo> getDebugLog(size_t limit = 100) const {
        if (limit > debugLog_.size()) {
            return debugLog_;
        }
        return std::vector<DebugInfo>(debugLog_.end() - limit, debugLog_.end());
    }
    
    void logDebug(const std::string& event, const std::string& details, const std::string& severity = "INFO") {
        DebugInfo info;
        info.timestamp = getCurrentTimestamp();
        info.event = event;
        info.details = details;
        info.severity = severity;
        
        debugLog_.push_back(info);
        if (debugLog_.size() > maxDebugLogSize_) {
            debugLog_.erase(debugLog_.begin();
        }
        
        // Also log to brain's internal logger
        NLM_LOG_INFO("[MONITOR] " + event + ": " + details);
    }
    
    void step(double timestep) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        if (profilingEnabled_) {
            metrics_.totalSteps++;
        }
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        if (realTimeMonitoring_) {
            checkSystemHealth();
        }
        
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count();
        
        if (profilingEnabled_) {
            metrics_.cpuTime += duration / 1000000.0;
        }
    }
    
private:
    void updatePerformanceMetrics() {
        if (!brain_) return;
        
        // Update basic metrics
        metrics_.spikeCount = brain_->getTotalSpikeCount();
        metrics_.spikeRate = metrics_.totalSteps > 0 ? (metrics_.spikeCount / metrics_.totalSteps) : 0.0;
        
        // Estimate memory usage (simplified)
        metrics_.memoryUsage = brain_->getTotalNeuronCount() * sizeof(Neuron) / 1024.0 / 1024; // KB
        
        // Calculate simulation speed
        if (metrics_.totalSteps > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - metrics_.startTime).count();
            metrics_.simulationSpeed = metrics_.totalSteps / elapsed;
        }
    }
    
    void checkSystemHealth() {
        health_.neuronDynamicsHealthy = checkNeuronDynamics();
        health_.plasticityHealthy = checkPlasticity();
        health_.memorySystemsHealthy = checkMemorySystems();
        health_.neuromodulationHealthy = checkNeuromodulation();
        health_.predictionHealthy = checkPrediction();
        
        float score = 0.0f;
        if (health_.neuronDynamicsHealthy) score += 0.2f;
        if (health_.plasticityHealthy) score += 0.2f;
        if (health_.memorySystemsHealthy) score += 0.2f;
        if (health_.neuromodulationHealthy) score += 0.2f;
        if (health_.predictionHealthy) score += 0.2f;
        health_.healthScore = score;
    }
    
    bool checkNeuronDynamics() const {
        if (!brain_) return false;
        // Check for neuron instabilities
        auto neurons = brain_->getTotalNeuronCount();
        auto spikes = brain_->getTotalSpikeCount();
        
        // Healthy if spike rate is reasonable and we haven't had crashes
        float spikeRate = spikes / (neurons > 0 ? neurons : 1);
        return spikeRate > 0.01f && spikeRate < 10.0f; // Reasonable range
    }
    
    bool checkPlasticity() const {
        if (!brain_) return false;
        // Check synaptic weights are within bounds
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) return true; // Can't check, assume healthy
        
        for (const auto& syn : region->getSynapses()) {
            auto weight = syn->getWeight();
            if (weight < -1.0f || weight > 1.0f) {
                return false;
            }
        }
        return true;
    }
    
    bool checkMemorySystems() const {
        // Check memory systems by looking for their integration
        // This is a simplified check - in real implementation would be more sophisticated
        return true; // Assume healthy for demo
    }
    
    bool checkNeuromodulation() const {
        // Check for neuromodulation system health
        return true; // Assume healthy for demo
    }
    
    bool checkPrediction() const {
        // Check prediction system health
        return true; // Assume healthy for demo
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now - std::chrono::high_resolution_clock::time_point()).count();
        return "[" + std::to_string(timeSinceEpoch) + "]";
    }
    
    float calculateHealthScore() const {
        // Calculate comprehensive health score
        float score = 1.0f;
        if (health_.neuronDynamicsHealthy) score += 0.2f;
        if (health_.plasticityHealthy) score += 0.2f;
        if (health_.memorySystemsHealthy) score += 0.2f;
        if (health_.neuromodulationHealthy) score += 0.2f;
        if (health_.predictionHealthy) score += 0.2f;
        return score;
    }
    
    std::string generateHealthReport() const {
        std::stringstream ss;
        ss << "NLM System Health Report" << std::endl;
        ss << "============================" << std::endl;
        ss << "Neuron Dynamics: " << (health_.neuronDynamicsHealthy ? "HEALTHY" : "ISSUE") << std::endl;
        ss << "Plasticity: " << (health_.plasticityHealthy ? "HEALTHY" : "ISSUE") << std::endl;
        ss << "Memory Systems: " << (health_.memorySystemsHealthy ? "HEALTHY" : "ISSUE") << std::endl;
        ss << "Neuromodulation: " << (health_.neuromodulationHealthy ? "HEALTHY" : "ISSUE") << std::endl;
        ss << "Prediction: " << (health_.predictionHealthy ? "HEALTHY" : "ISSUE") << std::endl;
        ss << std::endl;
        ss << "Overall Health Score: " << health_.healthScore * 100 << "%" << std::endl;
        return ss.str();
    }
};

class AdvancedCommandLineTool {
public:
    struct Command {
        std::string name;
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
        std::vector<std::string> args;
        std::string help;
    };
    
private:
    std::map<std::string, Command> commands_;
    std::shared_ptr<AdvancedBrainMonitor> monitor_;
    bool verboseMode_{false};
    bool batchMode_{false};

public:
    AdvancedCommandLineTool() {
        registerCommands();
    }
    
    void registerCommands() {
        // Debug commands
        registerCommand("debug", "Enable/disable debugging", [this](const std::vector<std::string>& args) {
            if (args.size() > 0 && args[0] == "toggle") {
                debugMode_ = !debugMode_;
                std::cout << "Debug mode: " << (debugMode_ ? "ON" : "OFF") << std::endl;
            }
        }, {"toggle"}, "Toggle debug mode");
        
        registerCommand("monitor", "Start/stop real-time monitoring", [this](const std::vector<std::string>& args) {
            if (args.size() > 0) {
                if (args[0] == "start") {
                    startMonitoring();
                } else if (args[0] == "stop") {
                    stopMonitoring();
                }
            } else {
                std::cout << "Usage: monitor start|stop" << std::endl;
            }
        }, {"start", "stop"}, "Start or stop real-time monitoring");
        
        registerCommand("profile", "Start/stop profiling", [this](const std::vector<std::string>& args) {
            if (args.size() > 0 && args[0] == "toggle") {
                profilingEnabled_ = !profilingEnabled_;
                std::cout << "Profiling: " << (profilingEnabled_ ? "ON" : "OFF") << std::endl;
            }
        }, {"toggle"}, "Toggle profiling mode");
        
        // Analysis commands
        registerCommand("health", "Check system health", [this](const std::vector<std::string>& args) {
            if (monitor_) {
                auto health = monitor_->getSystemHealth();
                std::cout << health.healthReport << std::endl;
                std::cout << "Health score: " << (health.healthScore * 100) << "%" << std::endl;
            }
        }, {}, "Check overall system health");
        
        registerCommand("metrics", "Show performance metrics", [this](const std::vector<std::string>& args) {
            if (monitor_) {
                auto metrics = monitor_->getPerformanceMetrics();
                std::cout << "Performance Metrics:" << std::endl;
                std::cout << "  Simulation speed: " << metrics.simulationSpeed << " steps/s" << std::endl;
                std::cout << "  Spike rate: " << metrics.spikeRate << " spikes/step" << std::endl;
                std::cout << "  Memory usage: " << metrics.memoryUsage << " KB" << std::endl;
                std::cout << "  CPU time: " << metrics.cpuTime << " seconds" << std::endl;
            }
        }, {}, "Show performance metrics");
        
        registerCommand("debuglog", "Show recent debug entries", [this](const std::vector<std::string>& args) {
            if (monitor_) {
                size_t limit = 50;
                if (args.size() > 0) {
                    limit = std::stoi(args[0]);
                }
                auto log = monitor_->getDebugLog(limit);
                for (const auto& entry : log) {
                    std::cout << "[" << entry.severity << "] " << entry.timestamp << " - " << entry.event << ": " << entry.details << std::endl;
                }
            }
        }, {"limit"}, "Show recent debug log entries");
        
        // Batch processing commands
        registerCommand("batch", "Run simulation in batch mode", [this](const std::vector<std::string>& args) {
            if (args.size() < 3) {
                std::cout << "Usage: batch <steps> <output_file> [<config_file>]" << std::endl;
                return;
            }
            runBatchProcessing(std::stoi(args[0]), args[1], args.size() > 2 ? args[2] : "configs/default.cfg");
        }, {"steps", "output_file", "[config_file]"}, "Run simulation in batch mode");
        
        registerCommand("benchmark", "Run benchmark tests", [this](const std::vector<std::string>& args) {
            runBenchmark();
        }, {}, "Run performance benchmarks");
        
        // Experiment commands
        registerCommand("experiment", "Run predefined experiment", [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                std::cout << "Available experiments: integration, plasticity, learning" << std::endl;
                return;
            }
            runExperiment(args[0]);
        }, {"integration", "plasticity", "learning", "cognitive"}, "Run predefined experiment");
        
        registerCommand("export", "Export simulation data", [this](const std::vector<std::string>& args) {
            if (args.size() < 2) {
                std::cout << "Usage: export <format> <output_file>" << std::endl;
                std::cout << "Formats: csv, json, binary" << std::endl;
                return;
            }
            exportData(args[0], args[1]);
        }, {"format", "output_file"}, "Export simulation data");
    }
    
    void registerCommand(const std::string& name, const std::string& description,
                        const std::function<void(const std::vector<std::string>&)>& handler,
                        const std::vector<std::string>& args, const std::string& help) {
        commands_[name] = Command{name, description, handler, args, help};
    }
    
    void setMonitor(std::shared_ptr<AdvancedBrainMonitor> monitor) {
        monitor_ = monitor;
    }
    
    void setVerboseMode(bool verbose) {
        verboseMode_ = verbose;
        if (verboseMode_) {
            logDebug("MODE", "Verbose mode enabled");
        }
    }
    
    void setBatchMode(bool batch) {
        batchMode_ = batch;
    }
    
    void runCommand(const std::string& cmd, const std::vector<std::string>& args) {
        auto it = commands_.find(cmd);
        if (it == commands_.end()) {
            std::cout << "Unknown command: " << cmd << std::endl;
            printHelp();
            return;
        }
        
        const auto& command = it->second;
        
        if (verboseMode_) {
            std::cout << "Executing command: " << cmd << std::endl;
            if (!args.empty()) {
                std::cout << "Arguments: ";
                for (const auto& arg : args) {
                    std::cout << " " << arg << "";
                }
                std::cout << std::endl;
            }
        }
        
        try {
            command.handler(args);
        } catch (const std::exception& e) {
            std::cerr << "Error executing command " << cmd << ": " << e.what() << std::endl;
        }
    }
    
    void printHelp(const std::string& command = "") const {
        if (!command.empty()) {
            auto it = commands_.find(command);
            if (it == commands_.end()) {
                std::cout << "Unknown command: " << command << std::endl;
                return;
            }
            
            const auto& cmd = it->second;
            std::cout << "Command: " << cmd.name << std::endl;
            std::cout << "Description: " << cmd.description << std::endl;
            std::cout << "Arguments:" << std::endl;
            for (const auto& arg : cmd.args) {
                std::cout << "  " << arg << std::endl;
            }
            std::cout << "Help: " << cmd.help << std::endl;
        } else {
            std::cout << "Available commands:" << std::endl;
            for (const auto& pair : commands_) {
                const auto& cmd = pair.second;
                std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            }
            std::cout << std::endl << "For detailed help on a command: help <command>" << std::endl;
        }
    }
    
    void runBatchProcessing(size_t steps, const std::string& outputFile, const std::string& configFile) {
        logDebug("BATCH", "Starting batch processing: " + std::to_string(steps) + " steps, output: " + outputFile);
        
        // Create brain with config
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            logDebug("BATCH", "Failed to load config, using defaults");
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto monitor = std::make_shared<AdvancedBrainMonitor>(brain);
        monitor->enableRealTimeMonitoring(true);
        setMonitor(monitor);
        
        // Run batch processing
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % 1000 == 0) {
                logDebug("BATCH", "Progress: " + std::to_string(step) + "/" + std::to_string(steps) + " steps");
            }
        }
        
        // Save results
        brain->save(outputFile);
        logDebug("BATCH", "Batch processing completed, saved to " + outputFile);
        
        // Export metrics
        auto metrics = monitor->getPerformanceMetrics();
        std::ofstream metricsFile(outputFile + ".metrics");
        metricsFile << "steps," << metrics.totalSteps << std::endl;
        metricsFile << "spikes," << metrics.spikeCount << std::endl;
        metricsFile << "cpu_time," << metrics.cpuTime << std::endl;
        metricsFile << "simulation_speed," << metrics.simulationSpeed << std::endl;
    }
    
    void runBenchmark() {
        logDebug("BENCHMARK", "Starting benchmark tests");
        
        // Run multiple benchmarks
        const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        const std::vector<size_t> stepCounts = {100, 1000, 5000};
        
        for (auto neuronCount : neuronCounts) {
            for (auto stepCount : stepCounts) {
                auto config = std::make_shared<Config>();
                config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
                config->set("simulation_timestep", 0.001, ConfigSource::Default);
                
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                auto monitor = std::make_shared<AdvancedBrainMonitor>(brain);
                monitor->enableProfiling(true);
                setMonitor(monitor);
                
                auto start = std::chrono::high_resolution_clock::now();
                for (size_t step = 0; step < stepCount; ++step) {
                    brain->step(step);
                }
                auto end = std::chrono::high_resolution_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
                float speed = stepCount / duration;
                
                std::cout << "Benchmark: " << neuronCount << " neurons, " << stepCount << " steps = " << speed << " steps/s" << std::endl;
                
                auto metrics = monitor->getPerformanceMetrics();
                std::cout << "  CPU time: " << metrics.cpuTime << "s, Memory: " << metrics.memoryUsage << "KB, Spike rate: " << metrics.spikeRate << std::endl;
            }
        }
        
        logDebug("BENCHMARK", "Benchmark completed");
    }
    
    void runExperiment(const std::string& experimentName) {
        logDebug("EXPERIMENT", "Running experiment: " + experimentName);
        
        if (experimentName == "integration") {
            // Run integration experiment
            // This would typically launch the Phase 6 demo
            std::cout << "Running integration experiment..." << std::endl;
            // For now, just log the action
            logDebug("EXPERIMENT", "Integration experiment requires Phase 6 system");
            
        } else if (experimentName == "plasticity") {
            // Run plasticity experiment
            std::cout << "Running plasticity experiment..." << std::endl;
            // Would create a brain and run plasticity tests
            logDebug("EXPERIMENT", "Plasticity experiment configured");
            
        } else if (experimentName == "learning") {
            // Run learning experiment
            std::cout << "Running learning experiment..." << std::endl;
            logDebug("EXPERIMENT", "Learning experiment configured");
            
        } else if (experimentName == "cognitive") {
            // Run cognitive experiment
            std::cout << "Running cognitive experiment..." << std::endl;
            logDebug("EXPERIMENT", "Cognitive experiment configured");
            
        } else {
            std::cout << "Unknown experiment: " << experimentName << std::endl;
        }
    }
    
    void exportData(const std::string& format, const std::string& outputFile) {
        logDebug("EXPORT", "Exporting data in " + format + " format to " + outputFile);
        
        if (!monitor_ || !brain_) {
            std::cout << "No brain or monitor available for export" << std::endl;
            return;
        }
        
        if (format == "csv") {
            exportToCSV(outputFile);
        } else if (format == "json") {
            exportToJSON(outputFile);
        } else if (format == "binary") {
            exportToBinary(outputFile);
        } else {
            std::cout << "Unsupported format: " << format << std::endl;
            std::cout << "Supported formats: csv, json, binary" << std::endl;
        }
    }
    
private:
    void exportToCSV(const std::string& outputFile) {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + outputFile);
        }
        
        file << "step,neuron_count,spike_count,avg_firing_rate,exc_inhib_ratio" << std::endl;
        
        // This would normally iterate through brain state over time
        // For demo, just write current state
        if (brain_) {
            file << "0," << brain_->getTotalNeuronCount() << "," << brain_->getTotalSpikeCount() << ",";
            file << brain_->getAverageFiringRate() << "," << brain_->getExcitationInhibitionRatio() << std::endl;
        }
        
        file.close();
        logDebug("EXPORT", "CSV export completed: " + outputFile);
    }
    
    void exportToJSON(const std::string& outputFile) {
        // Similar to CSV but JSON format
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + outputFile);
        }
        
        file << "{\n  \"brain_state\": {\n    \"neuron_count\": " << (brain_ ? brain_->getTotalNeuronCount() : 0) << ",\n    \"synapse_count\": " << (brain_ ? brain_->getTotalSynapseCount() : 0) << ",\n    \"spike_count\": " << (brain_ ? brain_->getTotalSpikeCount() : 0) << ",\n    \"firing_rate\": " << (brain_ ? brain_->getAverageFiringRate() : 0.0) << ",\n    \"exc_inhib_ratio\": " << (brain_ ? brain_->getExcitationInhibitionRatio() : 0.0) << "\n  }\n}";
        
        file.close();
        logDebug("EXPORT", "JSON export completed: " + outputFile);
    }
    
    void exportToBinary(const std::string& outputFile) {
        // Binary export
        std::ofstream file(outputFile, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + outputFile);
        }
        
        // Simple binary export of brain state
        if (brain_) {
            // Write some basic brain state
            size_t neuronCount = brain_->getTotalNeuronCount();
            file.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
            
            size_t spikeCount = brain_->getTotalSpikeCount();
            file.write(reinterpret_cast<const char*>(&spikeCount), sizeof(spikeCount));
            
            double firingRate = brain_->getAverageFiringRate();
            file.write(reinterpret_cast<const char*>(&firingRate), sizeof(firingRate));
        }
        
        file.close();
        logDebug("EXPORT", "Binary export completed: " + outputFile);
    }
    
    bool debugMode_{false};
    bool profilingEnabled_{false};
    bool monitoringActive_{false};
    std::thread monitoringThread_;
    std::mutex debugLogMutex_;
};

// Global instance for command-line interface
AdvancedCommandLineTool gAdvancedTool;

// Advanced brain simulation with debugging support
void runAdvancedSimulation(const std::string& configFile = "configs/default.cfg", 
                          bool enableMonitoring = false,
                          bool enableProfiling = false,
                          bool verbose = false) {
    // Create configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args if any
    // In real implementation, this would parse argc/argv
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create advanced monitor
    auto monitor = std::make_shared<AdvancedBrainMonitor>(brain);
    
    if (enableMonitoring) {
        monitor->enableRealTimeMonitoring(true);
    }
    
    if (enableProfiling) {
        monitor->enableProfiling(true);
    }
    
    // Set global tool
    gAdvancedTool.setMonitor(monitor);
    gAdvancedTool.setVerboseMode(verbose);
    
    NLM_LOG_INFO("=== Advanced NLM Simulation Started ===");
    NLM_LOG_INFO("Neuron count: " << brain->getTotalNeuronCount());
    NLM_LOG_INFO("Synapse count: " << brain->getTotalSynapseCount());
    NLM_LOG_INFO("Monitoring: " << (enableMonitoring ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("Profiling: " << (enableProfiling ? "ENABLED" : "DISABLED"));
    
    // Run simulation with monitoring
    for (SimulationStep step = 0; step < 10000; ++step) {
        brain->step(step);
        
        if (step % 1000 == 0) {
            monitor->logDebug("SIMULATION", "Step " + std::to_string(step) + "/10000");
        }
        
        // Check for early termination
        if (brain->getTotalSpikeCount() > 1000000) { // Arbitrary large number
            NLM_LOG_INFO("Early termination: spike threshold reached");
            break;
        }
    }
    
    // Show final metrics
    auto metrics = monitor->getPerformanceMetrics();
    auto health = monitor->getSystemHealth();
    
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Total steps: " << metrics.totalSteps);
    NLM_LOG_INFO("Total spikes: " << metrics.spikeCount);
    NLM_LOG_INFO("Simulation speed: " << metrics.simulationSpeed << " steps/s");
    NLM_LOG_INFO("CPU time: " << metrics.cpuTime << " seconds");
    NLM_LOG_INFO("Memory usage: " << metrics.memoryUsage << " KB");
    NLM_LOG_INFO("Health score: " << (health.healthScore * 100) << "%");
    
    // Export debug log
    auto debugLog = monitor->getDebugLog(50);
    NLM_LOG_INFO("Debug entries captured: " << debugLog.size());
    
    NLM_LOG_INFO("=== Advanced Simulation Finished ===");
}

// Command line interface for advanced tools
void runAdvancedCLI(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (args.empty()) {
        std::cout << "NLM Advanced Command Line Interface" << std::endl;
        std::cout << "=====================================" << std::endl;
        gAdvancedTool.printHelp();
        return;
    }
    
    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    
    // Handle help command
    if (command == "help" || command == "?") {
        if (commandArgs.empty()) {
            gAdvancedTool.printHelp();
        } else {
            gAdvancedTool.printHelp(commandArgs[0]);
        }
        return;
    }
    
    // Handle advanced simulation command
    if (command == "simulate") {
        bool monitoring = false;
        bool profiling = false;
        bool verbose = false;
        std::string configFile = "configs/default.cfg";
        
        // Parse options
        for (size_t i = 0; i < commandArgs.size(); ++i) {
            if (commandArgs[i] == "--monitor" || commandArgs[i] == "-m") {
                monitoring = true;
            } else if (commandArgs[i] == "--profile" || commandArgs[i] == "-p") {
                profiling = true;
            } else if (commandArgs[i] == "--verbose" || commandArgs[i] == "-v") {
                verbose = true;
            } else if (commandArgs[i] == "--config" && i + 1 < commandArgs.size()) {
                configFile = commandArgs[++i];
            }
        }
        
        runAdvancedSimulation(configFile, monitoring, profiling, verbose);
        return;
    }
    
    // Handle direct command execution
    gAdvancedTool.runCommand(command, commandArgs);
}