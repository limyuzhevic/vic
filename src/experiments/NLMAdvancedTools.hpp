// Advanced command-line tools for NLM power users
//
// This module provides enhanced command-line utilities for advanced users
// who need more control, profiling, and analysis capabilities.

#ifndef NLM_ADVANCED_TOOLS_HPP
#define NLM_ADVANCED_TOOLS_HPP

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <iomanip>
#include <filesystem>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "DemoFramework.hpp"

namespace nlm {

/**
 * @brief Advanced brain profiler for performance analysis
 */
class BrainProfiler {
public:
    struct ProfileData {
        uint64_t totalSteps = 0;
        uint64_t totalSpikes = 0;
        uint64_t activeNeurons = 0;
        float averageFiringRate = 0.0f;
        float totalMemoryUsage = 0.0f;
        float totalSimulationTime = 0.0f;
        std::map<uint64_t, uint64_t> spikeHistoryPerStep;
        std::vector<float> firingRateHistory;
        std::vector<float> memoryUsageHistory;
        std::chrono::duration<double> wallClockTime;
    };
    
    BrainProfiler() : currentStep_(0), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void startProfiling(std::shared_ptr<Brain> brain, const std::string& outputFile = "") {
        brain_ = brain;
        outputFile_ = outputFile;
        profileData_.totalSteps = 0;
        profileData_.spikeHistoryPerStep.clear();
        profileData_.firingRateHistory.clear();
        profileData_.memoryUsageHistory.clear();
        
        // Initialize baseline measurements
        baselineNeuronCount_ = brain_->getTotalNeuronCount();
        baselineSynapseCount_ = brain_->getTotalSynapseCount();
        
        startTime_ = std::chrono::high_resolution_clock::now();
        currentStep_ = 0;
        
        std::cout << "Brain profiler started. Use stopProfiling() to end.\n";
    }
    
    void profileStep() {
        if (!brain_) return;
        
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // Record metrics
        profileData_.totalSteps++;
        profileData_.totalSpikes += brain_->getTotalSpikeCount();
        profileData_.activeNeurons += brain_->getFiringNeuronCount();
        profileData_.spikeHistoryPerStep[currentStep_] = brain_->getTotalSpikeCount();
        
        float firingRate = brain_->getAverageFiringRate();
        profileData_.firingRateHistory.push_back(firingRate);
        
        float memoryUsage = getMemoryUsage(brain_);
        profileData_.memoryUsageHistory.push_back(memoryUsage);
        
        // Simulate step execution
        brain_->step(currentStep_++);
        
        auto stepEnd = std::chrono::high_resolution_clock::now();
        profileData_.totalSimulationTime += 
            std::chrono::duration<float>(stepEnd - stepStart).count();
    }
    
    ProfileData stopProfiling() {
        auto endTime = std::chrono::high_resolution_clock::now();
        profileData_.wallClockTime = endTime - startTime_;
        
        // Calculate averages
        if (profileData_.totalSteps > 0) {
            profileData_.averageFiringRate = profileData_.totalSpikes / 
                                             (profileData_.totalSimulationTime * baselineNeuronCount_);
            profileData_.totalMemoryUsage = *std::max_element(
                profileData_.memoryUsageHistory.begin(), 
                profileData_.memoryUsageHistory.end());
        }
        
        // Save to file if specified
        if (!outputFile_.empty()) {
            saveProfile(outputFile_);
        }
        
        std::cout << "Brain profiling stopped.\n";
        return profileData_;
    }
    
    void saveProfile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot open profile output file: " + filename);
        }
        
        file << "NLM Brain Profile\n";
        file << "==================\n\n";
        
        file << "Total Steps: " << profileData_.totalSteps << "\n";
        file << "Total Spikes: " << profileData_.totalSpikes << "\n";
        file << "Average Active Neurons: " << (profileData_.totalSteps > 0 ? 
            profileData_.activeNeurons / profileData_.totalSteps : 0) << "\n";
        file << "Average Firing Rate: " << profileData_.averageFiringRate << " Hz\n";
        file << "Peak Memory Usage: " << profileData_.totalMemoryUsage << "\n";
        file << "Total Simulation Time: " << profileData_.totalSimulationTime << " s\n";
        file << "Wall Clock Time: " << profileData_.wallClockTime.count() << " s\n";
        file << "Steps per Second: " << (profileData_.totalSteps / 
            profileData_.wallClockTime.count()) << "\n\n";
        
        file << "Spike History:\n";
        for (const auto& pair : profileData_.spikeHistoryPerStep) {
            file << "  Step " << pair.first << ": " << pair.second << " spikes\n";
        }
        
        file.close();
        std::cout << "Profile saved to: " << filename << "\n";
    }
    
    ProfileData getCurrentProfile() const { return profileData_; }
    
private:
    float getMemoryUsage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage (in arbitrary units)
        // In a real implementation, this would use system-specific APIs
        size_t neuronMemory = brain->getTotalNeuronCount() * sizeof(float);
        size_t synapseMemory = brain->getTotalSynapseCount() * sizeof(float);
        return static_cast<float>(neuronMemory + synapseMemory) / (1024 * 1024); // MB
    }
    
    std::shared_ptr<Brain> brain_;
    std::string outputFile_;
    ProfileData profileData_;
    uint64_t currentStep_ = 0;
    std::chrono::high_resolution_clock::time_point startTime_;
    uint64_t baselineNeuronCount_ = 0;
    uint64_t baselineSynapseCount_ = 0;
};

/**
 * @brief Configuration file management tools
 */
class ConfigManager {
public:
    struct ConfigInfo {
        std::string name;
        std::string path;
        std::string description;
        std::chrono::system_clock::time_point lastModified;
        size_t size;
        std::map<std::string, std::string> settings;
    };
    
    static std::vector<ConfigInfo> listConfigFiles(const std::string& directory = "configs") {
        std::vector<ConfigInfo> configs;
        
        if (!std::filesystem::exists(directory)) {
            std::cout << "Config directory does not exist: " << directory << "\n";
            return configs;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".json" || entry.path().extension() == ".cfg" ||
                 entry.path().extension() == ".txt")) {
                
                ConfigInfo info;
                info.path = entry.path().string();
                info.name = entry.path().filename().string();
                info.size = entry.file_size();
                info.lastModified = std::chrono::system_clock::from_time_t(
                    std::filesystem::last_write_time(entry));
                
                // Extract basic settings from config file
                info.settings = parseConfigFile(info.path);
                
                // Set description based on file content
                if (info.name.find("phase") != std::string::npos) {
                    info.description = "Phase " + info.name.substr(5, 1) + " configuration";
                } else {
                    info.description = "NLM configuration file";
                }
                
                configs.push_back(info);
            }
        }
        
        // Sort by last modified time (most recent first)
        std::sort(configs.begin(), configs.end(),
            [](const ConfigInfo& a, const ConfigInfo& b) {
                return a.lastModified > b.lastModified;
            });
        
        return configs;
    }
    
    static std::map<std::string, std::string> parseConfigFile(const std::string& filename) {
        std::map<std::string, std::string> settings;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            return settings;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            // Parse key-value pairs (simplified)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                settings[key] = value;
            }
        }
        
        return settings;
    }
    
    static void compareConfigs(const std::string& config1, const std::string& config2) {
        auto settings1 = parseConfigFile(config1);
        auto settings2 = parseConfigFile(config2);
        
        std::cout << "Configuration Comparison: " << config1 << " vs " << config2 << "\n";
        std::cout << "==================================================\n\n";
        
        // Find all keys from both configs
        std::set<std::string> allKeys;
        for (const auto& pair : settings1) allKeys.insert(pair.first);
        for (const auto& pair : settings2) allKeys.insert(pair.first);
        
        bool differencesFound = false;
        for (const std::string& key : allKeys) {
            auto it1 = settings1.find(key);
            auto it2 = settings2.find(key);
            
            std::string value1 = (it1 != settings1.end()) ? it1->second : "[NOT SET]";
            std::string value2 = (it2 != settings2.end()) ? it2->second : "[NOT SET]";
            
            if (value1 != value2) {
                differencesFound = true;
                std::cout << "DIFFERENCE: " << key << "\n";
                std::cout << "  " << config1 << ": " << value1 << "\n";
                std::cout << "  " << config2 << ": " << value2 << "\n\n";
            } else if (value1 != "[NOT SET]") {
                std::cout << "SAME:   " << key << " = " << value1 << "\n";
            }
        }
        
        if (!differencesFound) {
            std::cout << "No differences found.\n";
        }
    }
    
    static void exportConfig(std::shared_ptr<Config> config, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot open config output file: " + filename);
        }
        
        file << "NLM Configuration Export\n";
        file << "========================\n\n";
        
        // Export all settings from config
        // Note: This is a simplified implementation - real config would need to
        // traverse the config data structure properly
        file << "# Configuration exported from NLM Brain\n";
        file << "# Generated on: " << getCurrentTimestamp() << "\n\n";
        
        // Write some basic config values for demonstration
        if (config->hasKey("neuron_count")) {
            file << "neuron_count: " << config->getInt64("neuron_count") << "\n";
        }
        if (config->hasKey("region_count")) {
            file << "region_count: " << config->getInt64("region_count") << "\n";
        }
        if (config->hasKey("connection_probability")) {
            file << "connection_probability: " << config->getFloat("connection_probability") << "\n";
        }
        if (config->hasKey("random_seed")) {
            file << "random_seed: " << config->getInt64("random_seed") << "\n";
        }
        
        file.close();
        std::cout << "Configuration exported to: " << filename << "\n";
    }
    
private:
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

/**
 * @brief Advanced batch processing tools for running multiple experiments
 */
class BatchProcessor {
public:
    struct BatchResult {
        std::string experimentName;
        int trialCount;
        float totalReward;
        float averageFiringRate;
        float processingTime;
        bool success;
        std::string errorMessage;
        std::chrono::duration<double> wallClockTime;
    };
    
    static std::vector<BatchResult> runBatchExperiments(
        const std::vector<std::string>& configFiles,
        int trialsPerConfig,
        int stepsPerTrial,
        const std::string& outputFile = "batch_results.json") {
        
        std::vector<BatchResult> results;
        
        for (const std::string& configFile : configFiles) {
            std::cout << "Processing configuration: " << configFile << "\n";
            
            try {
                // Load configuration
                auto config = loadConfigFromFile(configFile);
                
                // Run experiment
                auto result = runSingleExperiment(config, trialsPerConfig, stepsPerTrial);
                
                // Add to results
                BatchResult batchResult;
                batchResult.experimentName = configFile;
                batchResult.trialCount = trialsPerConfig;
                batchResult.totalReward = result.totalReward;
                batchResult.averageFiringRate = result.averageFiringRate;
                batchResult.success = true;
                batchResult.wallClockTime = result.wallClockTime;
                
                results.push_back(batchResult);
                
            } catch (const std::exception& e) {
                BatchResult batchResult;
                batchResult.experimentName = configFile;
                batchResult.trialCount = trialsPerConfig;
                batchResult.success = false;
                batchResult.errorMessage = std::string("Error: ") + e.what();
                
                results.push_back(batchResult);
            }
        }
        
        // Save results
        if (!outputFile.empty()) {
            saveBatchResults(results, outputFile);
        }
        
        return results;
    }
    
    static BatchResult runSingleExperiment(
        std::shared_ptr<Config> config,
        int trials,
        int stepsPerTrial) {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain and world
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        
        // Run trials
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        int totalSpikes = 0;
        
        for (int trial = 0; trial < trials; ++trial) {
            // Reset for each trial
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Run simulation
            for (int step = 0; step < stepsPerTrial; ++step) {
                // Get sensory percept
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Process input and simulate
                agentBrain.processSensoryInput(percept);
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(static_cast<uint64_t>(trial * stepsPerTrial + step));
                }
                
                // Decode and apply action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                world.update(0.1);
                
                // Accumulate rewards
                totalReward += result.reward;
                totalSpikes += brain->getTotalSpikeCount();
            }
            
            totalFiringRate += brain->getAverageFiringRate();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto wallClockTime = endTime - startTime;
        
        BatchResult result;
        result.trialCount = trials;
        result.totalReward = totalReward;
        result.averageFiringRate = totalFiringRate / trials;
        result.wallClockTime = wallClockTime;
        result.success = true;
        
        return result;
    }
    
    static void saveBatchResults(const std::vector<BatchResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot open batch results output file: " + filename);
        }
        
        file << "NLM Batch Processing Results\n";
        file << "=============================\n\n";
        
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& result : results) {
            file << "Experiment: " << result.experimentName << "\n";
            file << "  Trials: " << result.trialCount << "\n";
            file << "  Total Reward: " << result.totalReward << "\n";
            file << "  Average Firing Rate: " << result.averageFiringRate << " Hz\n";
            file << "  Success: " << (result.success ? "Yes" : "No") << "\n";
            if (!result.success) {
                file << "  Error: " << result.errorMessage << "\n";
            }
            file << "  Processing Time: " << result.processingTime << " s\n";
            file << "  Wall Clock Time: " << result.wallClockTime.count() << " s\n\n";
        }
        
        file.close();
        std::cout << "Batch results saved to: " << filename << "\n";
    }
    
private:
    static std::shared_ptr<Config> loadConfigFromFile(const std::string& filename) {
        // Simplified config loading - in reality would use proper config parser
        auto config = std::make_shared<Config>();
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot open config file: " + filename);
        }
        
        // Parse config file (simplified)
        std::string line;
        while (std::getline(file, line)) {
            if (line.find('=') != std::string::npos) {
                size_t equalsPos = line.find('=');
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Try to parse based on value type
                if (value.find('.') != std::string::npos) {
                    config->set(key, std::stof(value));
                } else {
                    try {
                        config->set(key, std::stoll(value));
                    } catch (...) {
                        config->set(key, value);
                    }
                }
            }
        }
        
        return config;
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

/**
 * @brief Advanced visualization and analysis tools
 */
class VisualizationTools {
public:
    static void generateActivityHeatmap(std::shared_ptr<Brain> brain, 
                                         const std::string& outputFile = "activity_heatmap.png") {
        std::cout << "Generating activity heatmap...\n";
        
        // Collect activity data over time
        std::vector<std::vector<float>> activityMap(100, std::vector<float>(100, 0.0f));
        
        // Run simulation to collect data (simplified)
        for (uint64_t step = 0; step < 1000; ++step) {
            brain->step(step);
            
            // Get neuron positions and activity
            // This is a simplified implementation
            for (const auto& region : brain->getRegions()) {
                for (const auto& neuron : region->getNeurons()) {
                    // Map neuron to grid position
                    int x = static_cast<int>(neuron->getPosition().x * 0.99f);
                    int y = static_cast<int>(neuron->getPosition().y * 0.99f);
                    
                    if (x >= 0 && x < 100 && y >= 0 && y < 100) {
                        activityMap[x][y] += neuron->getActivityLevel();
                    }
                }
            }
        }
        
        // Generate visualization (in real implementation, would use graphics library)
        saveHeatmap(activityMap, outputFile);
    }
    
    static void generateNetworkGraph(std::shared_ptr<Brain> brain,
                                     const std::string& outputFile = "network_graph.png") {
        std::cout << "Generating network graph...\n";
        
        // Collect connectivity data
        std::vector<std::pair<int, int>> connections;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                connections.push_back(std::make_pair(
                    synapse->getPreNeuron()->getId(),
                    synapse->getPostNeuron()->getId()));
            }
        }
        
        // Generate graph visualization (simplified)
        saveNetworkGraph(connections, outputFile);
    }
    
    static void generateStatisticalReport(std::shared_ptr<Brain> brain,
                                          const std::string& outputFile = "stats_report.txt") {
        std::cout << "Generating statistical report...\n";
        
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            throw DemoException("Cannot open stats report output file: " + outputFile);
        }
        
        file << "NLM Brain Statistical Report\n";
        file << "===========================\n\n";
        
        file << "Neuron Statistics:\n";
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "  Active Neurons: " << brain->getFiringNeuronCount() << "\n";
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n\n";
        
        file << "Synapse Statistics:\n";
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        
        // Regional statistics
        file << "\nRegional Statistics:\n";
        for (size_t i = 0; i < brain->getRegionCount(); ++i) {
            const auto& region = brain->getRegions()[i];
            file << "  Region " << i << ":\n";
            file << "    Neurons: " << region->getNeuronCount() << "\n";
            file << "    Synapses: " << region->getSynapseCount() << "\n";
            file << "    Average Activity: " << (region->getNeuronCount() > 0 ? 
                region->getAverageActivity() : 0.0f) << "\n";
        }
        
        // Memory statistics
        if (brain->getWorkingMemory()) {
            file << "\nWorking Memory:\n";
            file << "  Active Traces: " << brain->getWorkingMemory()->getActiveTraces() << "\n";
            file << "  Memory Neurons: " << brain->getWorkingMemory()->getMemoryNeuronCount() << "\n";
        }
        
        if (brain->getEpisodicMemory()) {
            file << "\nEpisodic Memory:\n";
            file << "  Episodes Stored: " << brain->getEpisodicMemory()->getEpisodeCount() << "\n";
        }
        
        file.close();
        std::cout << "Statistical report saved to: " << outputFile << "\n";
    }
    
private:
    static void saveHeatmap(const std::vector<std::vector<float>>& data, 
                           const std::string& filename) {
        // Simplified heatmap generation
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot create heatmap file: " + filename);
        }
        
        file << "Activity Heatmap Data (simplified format)\n";
        file << "Data size: " << data.size() << " x " << data[0].size() << "\n";
        
        // This would be replaced with actual image generation
        file << "[Visualization would be generated here with graphics library]\n";
        
        file.close();
    }
    
    static void saveNetworkGraph(const std::vector<std::pair<int, int>>& connections,
                                const std::string& filename) {
        // Simplified network graph generation
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot create network graph file: " + filename);
        }
        
        file << "Network Graph Data (simplified format)\n";
        file << "Number of connections: " << connections.size() << "\n";
        
        for (const auto& conn : connections) {
            file << "Connection: " << conn.first << " -> " << conn.second << "\n";
        }
        
        file << "[Graph visualization would be generated here]\n";
        
        file.close();
    }
};

/**
 * @brief Advanced benchmarking tools
 */
class BenchmarkTools {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        std::string testDescription;
        double executionTime;
        uint64_t stepsPerSecond;
        uint64_t memoryUsage;
        bool passed;
        std::string errorMessage;
        std::chrono::duration<double> wallClockTime;
    };
    
    static BenchmarkResult runBenchmark(const std::string& name, 
                                        const std::string& description,
                                        std::function<void()> benchmarkFunction,
                                        int64_t neuronCount = 1000,
                                        int64_t steps = 10000) {
        
        BenchmarkResult result;
        result.benchmarkName = name;
        result.testDescription = description;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup benchmark environment
            auto config = ConfigFactory::createPhase4Config(neuronCount);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            
            // Run benchmark function
            benchmarkFunction();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            
            // Calculate results
            result.wallClockTime = endTime - startTime;
            result.executionTime = result.wallClockTime.count();
            result.stepsPerSecond = static_cast<uint64_t>(steps / result.executionTime);
            result.memoryUsage = getMemoryUsage(brain);
            result.passed = true;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            result.wallClockTime = endTime - startTime;
            result.executionTime = result.wallClockTime.count();
            result.passed = false;
            result.errorMessage = std::string("Error: ") + e.what();
        }
        
        return result;
    }
    
    static void runStandardBenchmarks(const std::string& outputFile = "benchmark_results.json") {
        std::vector<BenchmarkResult> results;
        
        // Benchmark 1: Basic Brain Simulation
        auto benchmark1 = []() {
            auto config = ConfigFactory::createPhase4Config(100);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            
            for (uint64_t step = 0; step < 1000; ++step) {
                brain->step(step);
            }
        };
        
        results.push_back(runBenchmark("Basic Simulation", "Test basic brain simulation performance", benchmark1, 100, 1000));
        
        // Benchmark 2: Complex Brain Simulation
        auto benchmark2 = []() {
            auto config = ConfigFactory::createPhase4Config(500);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            
            for (uint64_t step = 0; step < 5000; ++step) {
                brain->step(step);
            }
        };
        
        results.push_back(runBenchmark("Complex Simulation", "Test complex brain simulation performance", benchmark2, 500, 5000));
        
        // Benchmark 3: Large Scale Simulation
        auto benchmark3 = []() {
            auto config = ConfigFactory::createPhase4Config(1000);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            
            for (uint64_t step = 0; step < 10000; ++step) {
                brain->step(step);
            }
        };
        
        results.push_back(runBenchmark("Large Scale Simulation", "Test large scale brain simulation performance", benchmark3, 1000, 10000));
        
        // Save results
        if (!outputFile.empty()) {
            saveBenchmarkResults(results, outputFile);
        }
        
        // Display summary
        printBenchmarkSummary(results);
    }
    
    static void saveBenchmarkResults(const std::vector<BenchmarkResult>& results, 
                                     const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw DemoException("Cannot open benchmark output file: " + filename);
        }
        
        file << "NLM Benchmark Results\n";
        file << "======================\n\n";
        
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& result : results) {
            file << "Benchmark: " << result.benchmarkName << "\n";
            file << "  Description: " << result.testDescription << "\n";
            file << "  Execution Time: " << result.executionTime << " s\n";
            file << "  Steps per Second: " << result.stepsPerSecond << "\n";
            file << "  Memory Usage: " << result.memoryUsage << " bytes\n";
            file << "  Passed: " << (result.passed ? "Yes" : "No") << "\n";
            if (!result.passed) {
                file << "  Error: " << result.errorMessage << "\n";
            }
            file << "  Wall Clock Time: " << result.wallClockTime.count() << " s\n\n";
        }
        
        file.close();
        std::cout << "Benchmark results saved to: " << filename << "\n";
    }
    
    static void printBenchmarkSummary(const std::vector<BenchmarkResult>& results) {
        std::cout << "\nNLM Benchmark Summary\n";
        std::cout << "======================\n\n";
        
        double totalTime = 0.0;
        bool allPassed = true;
        
        for (const auto& result : results) {
            std::cout << result.benchmarkName << ":\n";
            std::cout << "  Time: " << result.executionTime << " s\n";
            std::cout << "  Speed: " << result.stepsPerSecond << " steps/s\n";
            std::cout << "  Memory: " << result.memoryUsage << " bytes\n";
            std::cout << "  Status: " << (result.passed ? "PASS" : "FAIL") << "\n";
            
            if (!result.passed) {
                std::cout << "  Error: " << result.errorMessage << "\n";
            }
            
            std::cout << "\n";
            totalTime += result.executionTime;
            if (!result.passed) allPassed = false;
        }
        
        std::cout << "Overall Summary:\n";
        std::cout << "  Total Time: " << totalTime << " s\n";
        std::cout << "  All Tests Passed: " << (allPassed ? "Yes" : "No") << "\n";
    }
    
private:
    static uint64_t getMemoryUsage(std::shared_ptr<Brain> brain) {
        // Estimate memory usage in bytes
        // In real implementation, would use system-specific APIs
        uint64_t neuronMemory = brain->getTotalNeuronCount() * sizeof(float);
        uint64_t synapseMemory = brain->getTotalSynapseCount() * sizeof(float);
        
        // Add overhead for brain object
        neuronMemory += sizeof(Brain);
        synapseMemory += sizeof(std::vector<float>);
        
        return neuronMemory + synapseMemory;
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

} // namespace nlm

#endif // NLM_ADVANCED_TOOLS_HPP
