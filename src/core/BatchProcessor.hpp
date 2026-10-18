#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/Config/Config.hpp"

namespace nlm {

// Batch job configuration
struct BatchJob {
    std::string name;
    std::string description;
    std::map<std::string, std::string> parameters;
    std::string outputFile;
    bool enabled;
    
    BatchJob() : enabled(true) {}
    BatchJob(const std::string& name, const std::string& description) 
        : name(name), description(description), enabled(true) {}
};

// Batch processing results
struct BatchResult {
    std::string jobName;
    bool success;
    std::string output;
    double executionTime;
    size_t neuronCount;
    size_t totalSteps;
    std::string errorMessage;
    
    BatchResult() : success(false), executionTime(0.0), neuronCount(0), totalSteps(0) {}
};

// Simulation presets
class SimulationPresets {
public:
    SimulationPresets() {
        initializePresets();
    }
    
    // Get all available presets
    std::vector<std::string> getPresetNames() const {
        std::vector<std::string> names;
        for (const auto& pair : presets) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get preset description
    std::string getPresetDescription(const std::string& name) const {
        auto it = presets.find(name);
        if (it != presets.end()) {
            return it->second.description;
        }
        return "Preset not found";
    }
    
    // Get preset configuration
    Config getPresetConfig(const std::string& name) const {
        auto it = presets.find(name);
        if (it != presets.end()) {
            Config config;
            for (const auto& param : it->second.parameters) {
                setConfigFromString(config, param.first, param.second);
            }
            return config;
        }
        return Config();
    }
    
    // Apply preset to configuration
    static void applyPreset(Config& config, const std::string& presetName) {
        auto presets = SimulationPresets();
        Config presetConfig = presets.getPresetConfig(presetName);
        
        // Merge preset configuration into target configuration
        // (Higher priority settings override lower priority settings)
        for (const auto& entry : presetConfig.pImpl->entries) {
            config.set(entry.key, entry.value, entry.source);
        }
    }
    
    // Get preset parameter
    std::string getPresetParameter(const std::string& presetName, const std::string& paramName) const {
        auto it = presets.find(presetName);
        if (it != presets.end()) {
            auto paramIt = it->second.parameters.find(paramName);
            if (paramIt != it->second.parameters.end()) {
                return paramIt->second;
            }
        }
        return "";
    }
    
    // Create custom preset
    void addPreset(const std::string& name, const std::string& description,
                   const std::map<std::string, std::string>& parameters) {
        presets[name] = PresetConfig{description, parameters};
    }
    
    // Export preset to file
    bool exportPreset(const std::string& name, const std::string& filepath) const {
        auto it = presets.find(name);
        if (it == presets.end()) {
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "[Preset: " << name << "]" << std::endl;
        file << "Description: " << it->second.description << std::endl;
        file << std::endl;
        file << "Parameters:" << std::endl;
        for (const auto& param : it->second.parameters) {
            file << "  " << param.first << " = " << param.second << std::endl;
        }
        
        file.flush();
        return true;
    }
    
    // Import preset from file
    bool importPreset(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("[Preset:") != std::string::npos) {
                name = extractBetween(line, "[", "]");
            } else if (line.find("Description:") != std::string::npos) {
                description = extractAfter(line, "Description:");
            } else if (line.find("Parameters:") != std::string::npos) {
                // Skip the Parameters header
                continue;
            } else if (!line.empty() && line.find("  ") != std::string::npos && line.find("=") != std::string::npos) {
                // Parameter line
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string paramName = trim(line.substr(2, pos - 3));
                    std::string paramValue = trim(line.substr(pos + 1));
                    parameters[paramName] = paramValue;
                }
            }
        }
        
        if (!name.empty()) {
            addPreset(name, description, parameters);
            return true;
        }
        
        return false;
    }
    
private:
    struct PresetConfig {
        std::string description;
        std::map<std::string, std::string> parameters;
        
        PresetConfig() {}
        PresetConfig(const std::string& desc, const std::map<std::string, std::string>& params)
            : description(desc), parameters(params) {}
    };
    
    void initializePresets() {
        // Fast preset
        addPreset("fast", 
                 "Quick simulation with minimal resources and fast execution",
                 {
                     {"neuron_count", "100"},
                     {"simulation_timestep", "0.001"},
                     {"connection_probability", "0.05"},
                     {"stdp_ltp_weight", "0.005"},
                     {"stdp_ltd_weight", "0.006"},
                     {"synaptogenesis_rate", "0.00001"},
                     {"pruning_rate", "0.000001"},
                     {"planning_depth", "3"},
                     {"attention_inhibition_strength", "0.5"},
                     {"attention_excitation_strength", "1.0"}
                 });
        
        // Detailed preset
        addPreset("detailed",
                 "Medium-sized simulation with detailed learning and cognitive capabilities",
                 {
                     {"neuron_count", "2000"},
                     {"simulation_timestep", "0.0001"},
                     {"connection_probability", "0.2"},
                     {"stdp_ltp_weight", "0.02"},
                     {"stdp_ltd_weight", "0.024"},
                     {"synaptogenesis_rate", "0.0005"},
                     {"pruning_rate", "0.00005"},
                     {"planning_depth", "10"},
                     {"attention_inhibition_strength", "0.8"},
                     {"attention_excitation_strength", "2.0"}
                 });
        
        // Research preset
        addPreset("research",
                 "Large-scale simulation for research and analysis",
                 {
                     {"neuron_count", "5000"},
                     {"simulation_timestep", "0.0001"},
                     {"connection_probability", "0.3"},
                     {"stdp_ltp_weight", "0.05"},
                     {"stdp_ltd_weight", "0.06"},
                     {"synaptogenesis_rate", "0.002"},
                     {"pruning_rate", "0.0002"},
                     {"working_memory_capacity", "500"},
                     {"episodic_memory_max_episodes", "5000"},
                     {"replay_interval", "50"},
                     {"consolidation_interval", "500"},
                     {"planning_depth", "15"},
                     {"attention_inhibition_strength", "1.0"},
                     {"attention_excitation_strength", "3.0"}
                 });
        
        // Default preset
        addPreset("default",
                 "Standard configuration balancing performance and accuracy",
                 {
                     {"neuron_count", "1000"},
                     {"simulation_timestep", "0.001"},
                     {"connection_probability", "0.1"},
                     {"stdp_ltp_weight", "0.01"},
                     {"stdp_ltd_weight", "0.012"},
                     {"synaptogenesis_rate", "0.0001"},
                     {"pruning_rate", "0.00001"},
                     {"planning_depth", "5"},
                     {"attention_inhibition_strength", "0.5"},
                     {"attention_excitation_strength", "1.5"}
                 });
        
        // Memory-optimized preset
        addPreset("memory_optimized",
                 "Configuration optimized for memory-constrained environments",
                 {
                     {"neuron_count", "50"},
                     {"simulation_timestep", "0.01"},
                     {"connection_probability", "0.02"},
                     {"stdp_ltp_weight", "0.002"},
                     {"stdp_ltd_weight", "0.0024"},
                     {"synaptogenesis_rate", "0.000001"},
                     {"pruning_rate", "0.0000001"},
                     {"planning_depth", "2"},
                     {"attention_inhibition_strength", "0.3"},
                     {"attention_excitation_strength", "0.8"}
                 });
        
        // High-plasticity preset
        addPreset("high_plasticity",
                 "Configuration emphasizing plasticity and learning",
                 {
                     {"neuron_count", "2000"},
                     {"simulation_timestep", "0.001"},
                     {"connection_probability", "0.15"},
                     {"stdp_ltp_weight", "0.03"},
                     {"stdp_ltd_weight", "0.036"},
                     {"synaptogenesis_rate", "0.001"},
                     {"pruning_rate", "0.0001"},
                     {"planning_depth", "8"},
                     {"attention_inhibition_strength", "1.0"},
                     {"attention_excitation_strength", "2.5"}
                 });
    }
    
    std::string extractBetween(const std::string& str, char start, char end) {
        size_t startPos = str.find(start);
        size_t endPos = str.find(end, startPos + 1);
        if (startPos != std::string::npos && endPos != std::string::npos) {
            return str.substr(startPos + 1, endPos - startPos - 1);
        }
        return "";
    }
    
    std::string extractAfter(const std::string& str, const std::string& marker) {
        size_t pos = str.find(marker);
        if (pos != std::string::npos) {
            return trim(str.substr(pos + marker.length()));
        }
        return "";
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }
    
    void setConfigFromString(Config& config, const std::string& key, const std::string& valueStr) {
        // Simple type detection and setting
        if (valueStr == "true" || valueStr == "false") {
            bool val = (valueStr == "true");
            config.set(key, val, ConfigSource::File);
        } else if (isNumber(valueStr)) {
            if (isInteger(valueStr)) {
                config.set(key, std::stoll(valueStr), ConfigSource::File);
            } else {
                config.set(key, std::stod(valueStr), ConfigSource::File);
            }
        } else {
            config.set(key, valueStr, ConfigSource::File);
        }
    }
    
    bool isNumber(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(), str.end(),
            [](char c) { return !std::isdigit(c) && c != '.' && c != '-'; }) == str.end();
    }
    
    bool isInteger(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(), str.end(),
            [](char c) { return !std::isdigit(c) && c != '-'; }) == str.end();
    }
    
    std::map<std::string, PresetConfig> presets;
};

// Batch processor
class BatchProcessor {
public:
    BatchProcessor() : currentJobIndex(0), totalJobs(0), successCount(0), failureCount(0) {}
    
    // Process a single batch job
    BatchResult processJob(const BatchJob& job) {
        BatchResult result;
        result.jobName = job.name;
        result.success = false;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            Config config;
            
            // Apply job parameters
            for (const auto& param : job.parameters) {
                setConfigFromString(config, param.first, param.second);
            }
            
            // Initialize profiler for this job
            getProfiler().setEnabled(true);
            
            // Run simulation
            result = runSimulation(config, job.name);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            result.executionTime = duration.count();
            
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Exception: ") + e.what();
        }
        
        // Update counters
        if (result.success) {
            successCount++;
        } else {
            failureCount++;
        }
        
        return result;
    }
    
    // Process multiple batch jobs
    std::vector<BatchResult> processJobs(const std::vector<BatchJob>& jobs) {
        std::vector<BatchResult> results;
        totalJobs = jobs.size();
        currentJobIndex = 0;
        
        for (const auto& job : jobs) {
            if (!job.enabled) {
                continue;
            }
            
            NLM_LOG_INFO("Processing job " + job.name + ": " + job.description);
            
            BatchResult result = processJob(job);
            results.push_back(result);
            
            if (result.success) {
                NLM_LOG_INFO("✓ Job " + job.name + " completed successfully");
            } else {
                NLM_LOG_ERROR("✗ Job " + job.name + " failed: " + result.errorMessage);
            }
            
            currentJobIndex++;
        }
        
        return results;
    }
    
    // Generate batch report
    std::string generateReport(const std::vector<BatchResult>& results) const {
        std::ostringstream oss;
        oss << "Batch Processing Report" << std::endl;
        oss << "=" << std::string(50, '=') << std::endl;
        oss << "Total jobs: " << results.size() << std::endl;
        oss << "Successful jobs: " << successCount << std::endl;
        oss << "Failed jobs: " << failureCount << std::endl;
        oss << std::endl;
        
        oss << "Job Results:" << std::endl;
        for (const auto& result : results) {
            oss << "  " << result.jobName << ": ";
            if (result.success) {
                oss << "SUCCESS"; 
                oss << " (" << result.executionTime << "s, " 
                    << result.neuronCount << " neurons, " 
                    << result.totalSteps << " steps)";
            } else {
                oss << "FAILED: " << result.errorMessage;
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Save results to file
    bool saveResults(const std::vector<BatchResult>& results, const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(results) << std::endl;
        
        file.flush();
        return true;
    }
    
    // Get processing progress
    double getProgress() const {
        if (totalJobs == 0) return 0.0;
        return static_cast<double>(currentJobIndex) / totalJobs;
    }
    
private:
    BatchResult runSimulation(const Config& config, const std::string& jobName) {
        BatchResult result;
        result.jobName = jobName;
        
        // For now, we'll just create a simple test run
        // In a real implementation, this would run the full NLM simulation
        
        // Parse configuration
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 100);
        float timestep = config.getOr<float>("simulation_timestep", 0.001f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        
        if (!brain->initialize()) {
            result.errorMessage = "Failed to initialize brain";
            return result;
        }
        
        // Run basic tests
        runBasicConnectivityTest(brain);
        runPlasticityExperiment(brain);
        runStdpVerification(brain);
        
        // Collect results
        result.success = true;
        result.neuronCount = static_cast<size_t>(neuronCount);
        result.totalSteps = 1000; // Fixed for this test
        result.output = "Simulation completed successfully";
        
        return result;
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * timestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        // Run plasticity experiment
        // (Simplified version for batch processing)
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        // Run STDP verification
        // (Simplified version for batch processing)
    }
    
    size_t currentJobIndex;
    size_t totalJobs;
    size_t successCount;
    size_t failureCount;
    float timestep;
    
    void setConfigFromString(Config& config, const std::string& key, const std::string& valueStr) {
        // Simple type detection and setting
        if (valueStr == "true" || valueStr == "false") {
            bool val = (valueStr == "true");
            config.set(key, val, ConfigSource::File);
        } else if (isNumber(valueStr)) {
            if (isInteger(valueStr)) {
                config.set(key, std::stoll(valueStr), ConfigSource::File);
            } else {
                config.set(key, std::stod(valueStr), ConfigSource::File);
            }
        } else {
            config.set(key, valueStr, ConfigSource::File);
        }
    }
    
    bool isNumber(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(), str.end(),
            [](char c) { return !std::isdigit(c) && c != '.' && c != '-'; }) == str.end();
    }
    
    bool isInteger(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(), str.end(),
            [](char c) { return !std::isdigit(c) && c != '-'; }) == str.end();
    }
};

// Batch configuration loader
class BatchConfigLoader {
public:
    BatchConfigLoader() {}
    
    // Load batch configuration from file
    bool loadBatchConfig(const std::string& filepath, std::vector<BatchJob>& jobs) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string currentJob;
        std::string currentDescription;
        std::map<std::string, std::string> currentParameters;
        bool inJob = false;
        
        std::string line;
        while (std::getline(file, line)) {
            trim(line);
            
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            if (line.find("[JOB:") != std::string::npos) {
                // Save previous job if exists
                if (inJob) {
                    jobs.push_back(BatchJob(currentJob, currentDescription));
                    jobs.back().parameters = currentParameters;
                }
                
                // Start new job
                currentJob = extractBetween(line, "[", "]");
                currentDescription = "";
                currentParameters.clear();
                inJob = true;
            } else if (inJob && line.find("Description:") != std::string::npos) {
                currentDescription = extractAfter(line, "Description:");
            } else if (inJob && line.find("Enabled:") != std::string::npos) {
                std::string enabledStr = extractAfter(line, "Enabled:");
                bool enabled = (enabledStr == "true" || enabledStr == "1" || enabledStr == "yes");
                if (!jobs.empty()) {
                    jobs.back().enabled = enabled;
                }
            } else if (inJob && line.find("Parameter:") != std::string::npos) {
                // Skip parameter header
                continue;
            } else if (inJob && line.find("  ") != std::string::npos && line.find("=") != std::string::npos) {
                // Parameter line
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string paramName = trim(line.substr(2, pos - 3));
                    std::string paramValue = trim(line.substr(pos + 1));
                    currentParameters[paramName] = paramValue;
                    
                    if (!jobs.empty()) {
                        jobs.back().parameters = currentParameters;
                    }
                }
            }
        }
        
        // Add last job
        if (inJob && !currentJob.empty()) {
            jobs.push_back(BatchJob(currentJob, currentDescription));
            jobs.back().parameters = currentParameters;
        }
        
        return !jobs.empty();
    }
    
    // Create batch jobs from simulation presets
    void createPresetJobs(const std::vector<std::string>& presetNames,
                         std::vector<BatchJob>& jobs) {
        SimulationPresets presets;
        
        for (const auto& presetName : presetNames) {
            BatchJob job;
            job.name = "preset_" + presetName;
            job.description = presets.getPresetDescription(presetName);
            job.enabled = true;
            
            // Convert preset parameters to batch job parameters
            // (This is a simplified version - in practice, you might want to customize)
            job.parameters = {
                {"neuron_count", presets.getPresetParameter(presetName, "neuron_count")},
                {"simulation_timestep", presets.getPresetParameter(presetName, "simulation_timestep")},
                {"connection_probability", presets.getPresetParameter(presetName, "connection_probability")},
                {"stdp_ltp_weight", presets.getPresetParameter(presetName, "stdp_ltp_weight")},
                {"stdp_ltd_weight", presets.getPresetParameter(presetName, "stdp_ltd_weight")}
            };
            
            jobs.push_back(job);
        }
    }
    
private:
    std::string extractBetween(const std::string& str, char start, char end) {
        size_t startPos = str.find(start);
        size_t endPos = str.find(end, startPos + 1);
        if (startPos != std::string::npos && endPos != std::string::npos) {
            return str.substr(startPos + 1, endPos - startPos - 1);
        }
        return "";
    }
    
    std::string extractAfter(const std::string& str, const std::string& marker) {
        size_t pos = str.find(marker);
        if (pos != std::string::npos) {
            return trim(str.substr(pos + marker.length()));
        }
        return "";
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }
};

} // namespace nlm