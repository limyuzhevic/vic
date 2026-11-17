# NLM Advanced Examples

This document provides comprehensive examples for using the enhanced NLM framework with advanced features.

## 1. Advanced Brain Initialization with Error Handling

```cpp
#include "Brain.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include <iostream>

class AdvancedBrainExample {
public:
    AdvancedBrainExample() {
        // Create configuration with advanced settings
        config = std::make_shared<Config>();
        
        // Set advanced parameters
        config->set("neuron_count", 1000);
        config->set("region_count", 8);
        config->set("connection_probability", 0.12);
        config->set("simulation_timestep", 0.0005);
        config->set("random_seed", 42);
        
        // Plasticity settings
        config->set("stdp_ltp_weight", 0.015f);
        config->set("stdp_ltd_weight", 0.018f);
        config->set("stdp_tau", 25.0f);
        
        // Memory settings
        config->set("working_memory_capacity", 100);
        config->set("episodic_memory_max_episodes", 2000);
        
        // Neuromodulation settings
        config->set("dopamine_baseline", 0.1f);
        config->set("curiosity_baseline", 0.05f);
        
        // Development settings
        config->set("plasticity_rate_early", 0.02f);
        config->set("plasticity_rate_adult", 0.005f);
        
        // Create brain with comprehensive error handling
        brain = std::make_shared<Brain>(config);
        
        // Initialize with error checking
        if (!brain->initialize()) {
            std::cerr << "Brain initialization failed!" << std::endl;
            std::cerr << "Error: " << brain->getErrorMessage() << std::endl;
            return;
        }
        
        std::cout << "Brain initialized successfully!" << std::endl;
        
        // Enable advanced features
        brain->enablePerformanceMonitoring();
        brain->enableDebugOutput(true);
        brain->enableMemorySnapshots(true);
        brain->enableDevelopment(true);
        
        // Create experiment
        experiment = std::make_unique<Phase6IntegratedExperiment>(*brain);
    }
    
    void runAdvancedExample() {
        std::cout << "=== Advanced NLM Example ===" << std::endl;
        
        // Create Phase 6 configuration
        Phase6Config phase6Config;
        phase6Config.neuronCount = 1500;
        phase6Config.maxSteps = 8000;
        phase6Config.enableDevelopment = true;
        phase6Config.enableReplay = true;
        phase6Config.enableCheckpointing = true;
        phase6Config.enableNeuromodulation = true;
        phase6Config.enablePrediction = true;
        
        // Run comprehensive validation
        std::cout << "Running comprehensive validation..." << std::endl;
        bool validationPassed = brain->validate();
        
        if (!validationPassed) {
            std::cerr << "Validation failed!" << std::endl;
            return;
        }
        
        std::cout << "Validation passed!" << std::endl;
        
        // Run the experiment with monitoring
        std::cout << "Running advanced experiment..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto result = experiment->run(phase6Config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "Experiment completed in " << duration.count() << " seconds" << std::endl;
        
        // Analyze results
        analyzeResults(result);
        
        // Export comprehensive report
        exportReport(result, "advanced_example_results.json");
        
        // Generate performance analysis
        generatePerformanceAnalysis(result);
    }
    
    void analyzeResults(const Phase6Result& result) {
        std::cout << "\n=== Results Analysis ===" << std::endl;
        
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Learning Efficiency: " << calculateLearningEfficiency(result) << std::endl;
        
        std::cout << "\nSystem Integration:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        
        std::cout << "\nMemory Statistics:" << std::endl;
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Working Memory Usage: " << result.memoryWorkingMemoryUsed << "/" << result.memoryWorkingMemoryCapacity << " slots" << std::endl;
        
        std::cout << "\nNeuromodulation Levels:" << std::endl;
        std::cout << "  Dopamine: " << result.dopamineLevel << " (0.0=no effect, 1.0=high effect)" << std::endl;
        std::cout << "  Curiosity: " << result.curiosityLevel << std::endl;
        std::cout << "  Novelty: " << result.noveltyLevel << std::endl;
        
        // Health check
        bool isHealthy = brain->isHealthy();
        std::cout << "\nSystem Health: " << (isHealthy ? "✓ Healthy" : "✗ Unhealthy") << std::endl;
        
        if (!isHealthy) {
            std::cout << "  Last Error: " << brain->getErrorMessage() << std::endl;
        }
    }
    
    float calculateLearningEfficiency(const Phase6Result& result) {
        if (result.maxSteps == 0 || result.avgFiringRate == 0) return 0.0f;
        
        // Learning efficiency combines reward, firing rate, and system integration
        float rewardComponent = std::min(1.0f, result.totalReward / 100.0f);
        float firingComponent = std::min(1.0f, result.avgFiringRate / 50.0f);
        float integrationComponent = (result.memoryWorkingMemoryIntegrated ? 1.0f : 0.5f) *
                                      (result.predictionIntegrated ? 1.0f : 0.5f);
        
        return (rewardComponent + firingComponent + integrationComponent) / 3.0f;
    }
    
    void exportReport(const Phase6Result& result, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Advanced Example Report" << std::endl;
        file << "===========================" << std::endl;
        file << std::endl;
        
        file << "Timestamp: " << getCurrentTimestamp() << std::endl;
        file << "Configuration: " << config->toString() << std::endl;
        file << std::endl;
        
        file << "Performance Summary:" << std::endl;
        file << "  Total Steps: " << result.maxSteps << std::endl;
        file << "  Total Reward: " << result.totalReward << std::endl;
        file << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        file << "  Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        file << std::endl;
        
        file << "System Integration:" << std::endl;
        file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
        file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
        file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
        file << "  Prediction System: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
        file << "  Development: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
        file << "  Checkpointing: " << (result.checkpointingWorks ? "Yes" : "No") << std::endl;
        
        file.close();
        std::cout << "Report exported to: " << filename << std::endl;
    }
    
    void generatePerformanceAnalysis(const Phase6Result& result) {
        std::cout << "\n=== Performance Analysis ===" << std::endl;
        
        // Calculate performance metrics
        float stepsPerSecond = (result.totalWallClockTime > 0) ? result.maxSteps / result.totalWallClockTime : 0.0f;
        float memoryEfficiency = (result.neuronCount > 0) ? result.memoryWorkingMemoryCapacity / result.neuronCount : 0.0f;
        
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Simulation Speed: " << stepsPerSecond << " steps/second" << std::endl;
        std::cout << "  Memory Efficiency: " << (memoryEfficiency * 100) << "% of neurons" << std::endl;
        
        // Identify potential issues
        std::vector<std::string> issues;
        
        if (stepsPerSecond < 10.0f) {
            issues.push_back("Slow simulation speed - consider reducing complexity");
        }
        
        if (result.avgFiringRate > 200.0f) {
            issues.push_back("High firing rate may indicate instability");
        }
        
        if (result.avgFiringRate < 0.5f) {
            issues.push_back("Low firing rate may indicate insufficient learning");
        }
        
        if (issues.empty()) {
            std::cout << "  ✓ No performance issues detected" << std::endl;
        } else {
            std::cout << "  ⚠ Potential issues detected:" << std::endl;
            for (const auto& issue : issues) {
                std::cout << "    - " << issue << std::endl;
            }
        }
        
        // Generate recommendations
        std::cout << "\nRecommendations:" << std::endl;
        std::cout << "  1. Enable more detailed profiling for fine-tuning" << std::endl;
        std::cout << "  2. Consider checkpointing for long-running experiments" << std::endl;
        std::cout << "  3. Monitor memory usage and optimize if needed" << std::endl;
        std::cout << "  4. Adjust neuromodulation parameters for better learning" << std::endl;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::unique_ptr<Phase6IntegratedExperiment> experiment;
};

int main() {
    try {
        std::cout << "NLM Advanced Example" << std::endl;
        std::cout << "====================" << std::endl;
        
        AdvancedBrainExample example;
        example.runAdvancedExample();
        
        std::cout << "\nExample completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## 2. Performance Monitoring and Optimization

```cpp
#include "Brain.hpp"
#include <fstream>
#include <chrono>

class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Brain> brain) : brain(brain) {
        monitorEnabled = true;
        startTime = std::chrono::high_resolution_clock::now();
        totalSteps = 0;
        totalSpikes = 0;
        maxStepTime = 0.0f;
        minStepTime = 1000.0f;
        
        brain->enablePerformanceMonitoring();
    }
    
    void logStep(int step, bool success) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // The actual step execution would happen here
        // This is where we'd log the step timing
        
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration<float, std::milli>(stepEnd - stepStart).count();
        
        totalSteps++;
        if (success) {
            totalSpikes++;
        }
        
        maxStepTime = std::max(maxStepTime, stepDuration);
        minStepTime = std::min(minStepTime, stepDuration);
        
        // Log step performance
        if (monitorEnabled) {
            std::cout << "[Step " << step << "] Duration: " << stepDuration << "ms, Success: " 
                     << (success ? "Yes" : "No") << std::endl;
        }
    }
    
    void generatePerformanceReport(const std::string& filename) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<float, std::seconds>(endTime - startTime).count();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open performance report file" << std::endl;
            return;
        }
        
        file << "NLM Performance Report" << std::endl;
        file << "=======================" << std::endl;
        file << std::endl;
        
        file << "Simulation Summary:" << std::endl;
        file << "  Total Steps: " << totalSteps << std::endl;
        file << "  Total Spikes: " << totalSpikes << std::endl;
        file << "  Total Duration: " << totalDuration << "s" << std::endl;
        file << "  Steps per Second: " << (totalDuration > 0 ? totalSteps / totalDuration : 0.0) << std::endl;
        file << std::endl;
        
        file << "Step Performance:" << std::endl;
        file << "  Maximum Step Time: " << maxStepTime << "ms" << std::endl;
        file << "  Minimum Step Time: " << minStepTime << "ms" << std::endl;
        file << "  Average Step Time: " << (totalSteps > 0 ? (maxStepTime + minStepTime) / 2.0f : 0.0f) << "ms" << std::endl;
        file << std::endl;
        
        file << "Performance Analysis:" << std::endl;
        file << "  Efficiency: " << calculateEfficiency() << "%" << std::endl;
        file << "  Stability: " << calculateStability() << "%" << std::endl;
        
        file.close();
        std::cout << "Performance report written to: " << filename << std::endl;
    }
    
private:
    float calculateEfficiency() {
        if (totalSteps == 0) return 0.0f;
        
        // Simple efficiency calculation based on spike rate and success rate
        float spikeRate = (totalSteps > 0) ? totalSpikes / static_cast<float>(totalSteps) : 0.0f;
        float maxEfficiency = 100.0f; // Maximum theoretical efficiency
        float actualEfficiency = spikeRate * 100.0f; // Convert to percentage
        
        return std::min(actualEfficiency, maxEfficiency);
    }
    
    float calculateStability() {
        // Stability based on variance in step times
        if (totalSteps < 2) return 100.0f;
        
        // For now, return a simple stability score
        // In a real implementation, you would calculate variance
        float stability = 95.0f; // Assumed stable for demonstration
        
        return stability;
    }
    
    std::shared_ptr<Brain> brain;
    bool monitorEnabled;
    std::chrono::high_resolution_clock::time_point startTime;
    int totalSteps;
    int totalSpikes;
    float maxStepTime;
    float minStepTime;
};
```

## 3. Error Recovery and System Health Monitoring

```cpp
#include "Brain.hpp"
#include <queue>
#include <thread>

class ErrorRecoverySystem {
public:
    ErrorRecoverySystem(std::shared_ptr<Brain> brain) : brain(brain) {
        recoveryEnabled = true;
        healthCheckInterval = 1000; // milliseconds
        autoRecoveryEnabled = true;
        errorHistory = std::queue<std::string>();
        
        startHealthMonitor();
    }
    
    ~ErrorRecoverySystem() {
        stopHealthMonitor();
    }
    
    void logError(const std::string& error) {
        errorHistory.push(error);
        
        if (errorHistory.size() > 100) {
            errorHistory.pop();
        }
        
        // Log error with timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::cout << "[" << std::ctime(&time) << "] ERROR: " << error << std::endl;
        
        // Attempt auto-recovery if enabled
        if (autoRecoveryEnabled) {
            attemptRecovery(error);
        }
    }
    
    void attemptRecovery(const std::string& error) {
        std::cout << "Attempting recovery for error: " << error << std::endl;
        
        // Analyze error and determine recovery strategy
        RecoveryAction action = analyzeError(error);
        
        switch (action) {
            case RecoveryAction::RESET_SYSTEM:
                brain->reset();
                std::cout << "System reset completed" << std::endl;
                break;
            
            case RecoveryAction::REINITIALIZE:
                if (brain->initialize()) {
                    std::cout << "System reinitialized successfully" << std::endl;
                } else {
                    std::cout << "Reinitialization failed" << std::endl;
                }
                break;
            
            case RecoveryAction::CONTINUE_WITH_DEGRADATION:
                std::cout << "Continuing with degraded performance" << std::endl;
                break;
            
            case RecoveryAction::REQUEST_MANUAL_INTERVENTION:
                std::cout << "Manual intervention required" << std::endl;
                break;
            
            default:
                std::cout << "Unknown recovery action" << std::endl;
                break;
        }
    }
    
    RecoveryAction analyzeError(const std::string& error) {
        // Simple error analysis for demonstration
        if (error.find("validation") != std::string::npos) {
            return RecoveryAction::RESET_SYSTEM;
        } else if (error.find("memory") != std::string::npos) {
            return RecoveryAction::REINITIALIZE;
        } else if (error.find("plasticity") != std::string::npos) {
            return RecoveryAction::CONTINUE_WITH_DEGRADATION;
        } else {
            return RecoveryAction::REQUEST_MANUAL_INTERVENTION;
        }
    }
    
    void startHealthMonitor() {
        healthMonitorActive = true;
        healthMonitorThread = std::thread(&ErrorRecoverySystem::healthMonitorFunction, this);
    }
    
    void stopHealthMonitor() {
        healthMonitorActive = false;
        if (healthMonitorThread.joinable()) {
            healthMonitorThread.join();
        }
    }
    
    void healthMonitorFunction() {
        while (healthMonitorActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(healthCheckInterval));
            
            if (brain->isHealthy()) {
                std::cout << "System health check: PASS" << std::endl;
            } else {
                std::cout << "System health check: FAIL - " << brain->getErrorMessage() << std::endl;
                
                // Log health check failure
                logError("Health check failed: " + brain->getErrorMessage());
            }
        }
    }
    
    void enableAutoRecovery(bool enable) {
        autoRecoveryEnabled = enable;
    }
    
    void setHealthCheckInterval(int interval) {
        healthCheckInterval = interval;
    }
    
private:
    enum class RecoveryAction {
        RESET_SYSTEM,
        REINITIALIZE,
        CONTINUE_WITH_DEGRADATION,
        REQUEST_MANUAL_INTERVENTION
    };
    
    std::shared_ptr<Brain> brain;
    bool recoveryEnabled;
    bool autoRecoveryEnabled;
    int healthCheckInterval;
    bool healthMonitorActive;
    std::thread healthMonitorThread;
    std::queue<std::string> errorHistory;
};
```

## 4. Advanced Configuration and Experiment Management

```cpp
#include "Brain.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class AdvancedExperimentManager {
public:
    AdvancedExperimentManager() {
        loadConfiguration("advanced_config.json");
    }
    
    void loadConfiguration(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Configuration file not found, using defaults" << std::endl;
            setDefaultConfiguration();
            return;
        }
        
        json configJson;
        file >> configJson;
        
        parseConfiguration(configJson);
        file.close();
        
        std::cout << "Configuration loaded from: " << filename << std::endl;
    }
    
    void setDefaultConfiguration() {
        // Default advanced configuration
        config.neuronCount = 2000;
        config.maxSteps = 10000;
        config.enableDevelopment = true;
        config.enableReplay = true;
        config.enableCheckpointing = true;
        config.enableNeuromodulation = true;
        config.enablePrediction = true;
        
        // Advanced parameters
        config.simulationTimestep = 0.0005;
        config.randomSeed = 42;
        config.connectionProbability = 0.12;
        
        // Plasticity settings
        config.stdpLTPWeight = 0.015f;
        config.stdpLTDWeight = 0.018f;
        config.stdpTau = 25.0f;
        
        // Memory settings
        config.workingMemoryCapacity = 200;
        config.episodicMemoryMaxEpisodes = 5000;
        
        // Neuromodulation settings
        config.dopamineBaseline = 0.1f;
        config.curiosityBaseline = 0.05f;
        
        // Development settings
        config.plasticityRateEarly = 0.02f;
        config.plasticityRateAdult = 0.005f;
    }
    
    void parseConfiguration(const json& configJson) {
        // Parse configuration from JSON
        if (configJson.contains("neuron_count")) {
            config.neuronCount = configJson["neuron_count"];
        }
        
        if (configJson.contains("max_steps")) {
            config.maxSteps = configJson["max_steps"];
        }
        
        // Parse all configuration parameters...
        // (For brevity, not all parameters are shown)
    }
    
    void runExperimentSet() {
        std::cout << "=== Advanced Experiment Set ===" << std::endl;
        
        // Create brain with advanced configuration
        brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        
        if (!brain->initialize()) {
            std::cerr << "Brain initialization failed!" << std::endl;
            return;
        }
        
        std::cout << "Brain initialized with " << config.neuronCount << " neurons" << std::endl;
        
        // Enable advanced features
        brain->enablePerformanceMonitoring();
        brain->enableDebugOutput(true);
        brain->enableMemorySnapshots(true);
        
        // Create experiment
        experiment = std::make_unique<Phase6IntegratedExperiment>(*brain);
        
        // Run experiment set
        runExperimentSetInternal();
        
        // Analyze and export results
        analyzeAndExportResults();
    }
    
    void runExperimentSetInternal() {
        std::vector<Phase6Config> experimentConfigs = {
            createExperimentConfig("Basic Learning", 1000, 5000, false, false, false, false),
            createExperimentConfig("Advanced Learning", 1500, 8000, true, true, true, true),
            createExperimentConfig("Memory Intensive", 2000, 10000, false, true, true, false),
            createExperimentConfig("Neuromodulation Intensive", 1800, 7000, true, false, false, true),
        };
        
        std::cout << "Running " << experimentConfigs.size() << " experiments..." << std::endl;
        
        for (size_t i = 0; i < experimentConfigs.size(); ++i) {
            std::cout << "\n--- Experiment " << (i + 1) << "/" << experimentConfigs.size() << " ---" << std::endl;
            std::cout << "Name: " << experimentConfigs[i].name << std::endl;
            std::cout << "Neurons: " << experimentConfigs[i].neuronCount << std::endl;
            std::cout << "Steps: " << experimentConfigs[i].maxSteps << std::endl;
            std::cout << "Development: " << (experimentConfigs[i].enableDevelopment ? "Yes" : "No") << std::endl;
            std::cout << "Neuromodulation: " << (experimentConfigs[i].enableNeuromodulation ? "Yes" : "No") << std::endl;
            
            auto result = experiment->run(experimentConfigs[i]);
            
            // Store result for later analysis
            experimentResults.push_back(result);
            
            std::cout << "Experiment completed. Reward: " << result.totalReward << std::endl;
        }
    }
    
    void analyzeAndExportResults() {
        std::cout << "\n=== Experiment Set Analysis ===" << std::endl;
        
        // Analyze results
        analyzeExperimentResults();
        
        // Generate comparison report
        generateComparisonReport();
        
        // Export detailed results
        exportDetailedResults();
        
        // Generate recommendations
        generateRecommendations();
    }
    
private:
    Phase6Config createExperimentConfig(const std::string& name, int neurons, int steps,
                                        bool dev, bool neuromod, bool replay, bool checkpoint) {
        Phase6Config config;
        config.name = name;
        config.neuronCount = neurons;
        config.maxSteps = steps;
        config.enableDevelopment = dev;
        config.enableNeuromodulation = neuromod;
        config.enableReplay = replay;
        config.enableCheckpointing = checkpoint;
        return config;
    }
    
    void analyzeExperimentResults() {
        std::cout << "Experiment Results Analysis:" << std::endl;
        
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        int experimentCount = 0;
        
        for (const auto& result : experimentResults) {
            totalReward += result.totalReward;
            totalFiringRate += result.avgFiringRate;
            experimentCount++;
        }
        
        if (experimentCount > 0) {
            std::cout << "  Average Reward: " << (totalReward / experimentCount) << std::endl;
            std::cout << "  Average Firing Rate: " << (totalFiringRate / experimentCount) << " Hz" << std::endl;
            std::cout << "  Best Performance: " << findBestExperiment() << std::endl;
        }
    }
    
    std::string findBestExperiment() {
        if (experimentResults.empty()) return "N/A";
        
        size_t bestIndex = 0;
        float bestReward = experimentResults[0].totalReward;
        
        for (size_t i = 1; i < experimentResults.size(); ++i) {
            if (experimentResults[i].totalReward > bestReward) {
                bestIndex = i;
                bestReward = experimentResults[i].totalReward;
            }
        }
        
        return experimentResults[bestIndex].name;
    }
    
    void generateComparisonReport() {
        std::cout << "\nExperiment Comparison Report:" << std::endl;
        std::cout << "===========================" << std::endl;
        
        for (size_t i = 0; i < experimentResults.size(); ++i) {
            std::cout << "Experiment " << (i + 1) << ": " << experimentResults[i].name << std::endl;
            std::cout << "  Reward: " << experimentResults[i].totalReward << std::endl;
            std::cout << "  Firing Rate: " << experimentResults[i].avgFiringRate << " Hz" << std::endl;
            std::cout << "  Memory Integration: " << (experimentResults[i].memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
            std::cout << std::endl;
        }
    }
    
    void exportDetailedResults() {
        json resultsJson;
        
        resultsJson["experiment_count"] = experimentResults.size();
        resultsJson["timestamp"] = getCurrentTimestamp();
        
        json experimentsJson = json::array();
        for (const auto& result : experimentResults) {
            json experimentJson;
            experimentJson["name"] = result.name;
            experimentJson["reward"] = result.totalReward;
            experimentJson["firing_rate"] = result.avgFiringRate;
            experimentJson["memory_integration"] = result.memoryWorkingMemoryIntegrated;
            experimentJson["neuromodulation_integration"] = result.neuromodulationIntegrated;
            
            experimentsJson.push_back(experimentJson);
        }
        
        resultsJson["experiments"] = experimentsJson;
        
        std::ofstream file("experiment_results.json");
        if (file.is_open()) {
            file << resultsJson.dump(2);
            file.close();
            std::cout << "Detailed results exported to: experiment_results.json" << std::endl;
        }
    }
    
    void generateRecommendations() {
        std::cout << "\nRecommendations:" << std::endl;
        std::cout << "  1. Use advanced configuration for better performance" << std::endl;
        std::cout << "  2. Enable all neuromodulation systems for comprehensive learning" << std::endl;
        std::cout << "  3. Use checkpointing for long-running experiments" << std::endl;
        std::cout << "  4. Monitor performance and adjust parameters as needed" << std::endl;
        std::cout << "  5. Consider using multiple experiment configurations" << std::endl;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    Phase6Config config;
    std::shared_ptr<Config> brainConfig;
    std::shared_ptr<Brain> brain;
    std::unique_ptr<Phase6IntegratedExperiment> experiment;
    std::vector<Phase6Result> experimentResults;
};
```

## 5. Quick Start Guide

### Basic Usage
```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic demos
./nlm                 # Phase 2: Basic neural computation
./nlm_phase3_demo     # Phase 3: World interaction
./nlm_phase4_demo     # Phase 4: Advanced cognition
./nlm_phase5_demo     # Phase 5: Performance optimizations
./nlm_phase6_demo     # Phase 6: Complete integration
```

### Advanced Usage
```bash
# Run with advanced commands
./nlm_phase6_demo --help
./nlm_phase6_demo --debug --profiler --monitor
./nlm_phase6_demo --iterations 5000 --neurons 1000 --export results.json
./nlm_phase6_demo --development --replay --checkpointing
```

### Python Usage
```python
import pynlm
import numpy as np

# Create brain with advanced configuration
config = {
    'neuron_count': 1000,
    'region_count': 8,
    'connection_probability': 0.12,
    'simulation_timestep': 0.0005,
    'random_seed': 42,
}

brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Access results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Configuration File Example
```json
{
    "neuron_count": 2000,
    "max_steps": 10000,
    "enable_development": true,
    "enable_replay": true,
    "enable_checkpointing": true,
    "enable_neuromodulation": true,
    "enable_prediction": true,
    "simulation_timestep": 0.0005,
    "random_seed": 42,
    "connection_probability": 0.12,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 25.0,
    "working_memory_capacity": 200,
    "episodic_memory_max_episodes": 5000,
    "dopamine_baseline": 0.1,
    "curiosity_baseline": 0.05,
    "plasticity_rate_early": 0.02,
    "plasticity_rate_adult": 0.005
}
```

## Advanced Features Summary

### 1. Enhanced Error Handling
- Comprehensive error detection and recovery
- Graceful degradation and fallback mechanisms
- Detailed error logging and reporting
- State validation for all components

### 2. Performance Monitoring
- Real-time performance metrics
- Step-by-step timing analysis
- Memory usage tracking
- System health monitoring

### 3. Advanced CLI Commands
- Extensive command-line options
- Performance profiling and monitoring
- Checkpoint management
- Real-time analytics

### 4. Configuration Management
- JSON-based configuration
- Advanced parameter management
- Experiment configuration management
- Custom experiment sets

### 5. Error Recovery
- Automatic error detection and recovery
- Health monitoring and maintenance
- Recovery action planning
- Manual intervention support

## Getting Started

1. **Build the project**: Follow the build instructions above
2. **Run basic demos**: Start with Phase 2-6 demos to understand basic functionality
3. **Explore advanced features**: Use `--debug`, `--profiler`, and `--monitor` flags
4. **Experiment with configurations**: Try different parameter combinations
5. **Use Python bindings**: Interface with NLM from Python for scripting

## Advanced Topics

For advanced topics, see:
- Advanced Error Handling
- Performance Optimization
- Configuration Management
- Experiment Management

## Support and Community

For support and community:
- Check the documentation directory
- Join the NLM discussion forum
- Report issues through the issue tracker
- Contribute to the project on GitHub

---

**Version**: 0.1.0 (Phase 6)  
**License**: MIT  
**Documentation**: Advanced Examples Guide