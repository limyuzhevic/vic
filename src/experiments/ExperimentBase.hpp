#pragma once

/**
 * Experiment base class
 * 
 * Abstract base class for all NLM experiments. Provides common functionality
 * for recording experiment state, metrics, and computing statistics.
 */

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

namespace nlm {

/**
 * Experiment configuration entry
 */
struct ExperimentConfig {
    std::string key;
    std::string value;
    std::string description;
    
    ExperimentConfig() : key(), value(), description() {}
    ExperimentConfig(const std::string& k, const std::string& v, const std::string& d = "")
        : key(k), value(v), description(d) {}
};

/**
 * Step data structure for recording experiment progression
 */
struct StepData {
    SimulationStep step;
    size_t neuronCount;
    size_t synapseCount;
    size_t firingCount;
    float avgFiringRate;
    float totalReward;
    float simulationTime;
    
    StepData() : step(0), neuronCount(0), synapseCount(0), firingCount(0),
                avgFiringRate(0.0f), totalReward(0.0f), simulationTime(0.0f) {}
};

/**
 * Base experiment class
 * 
 * Abstract base class that provides common functionality for all NLM experiments.
 * Includes methods for recording state, computing statistics, and managing experiment lifecycle.
 */
class Experiment {
public:
    Experiment() = default;
    virtual ~Experiment() = default;
    
    // Disable copying, enable moving
    Experiment(const Experiment&) = delete;
    Experiment& operator=(const Experiment&) = delete;
    Experiment(Experiment&&) noexcept = default;
    Experiment& operator=(Experiment&&) noexcept = default;
    
    // Experiment metadata
    virtual const std::string& getName() const { return name; }
    virtual void setName(const std::string& name) { this->name = name; }
    
    // Experiment configuration
    virtual void addConfig(const std::string& key, const std::string& value, const std::string& description = "") {
        config.push_back({key, value, description});
    }
    
    virtual std::string getConfigValue(const std::string& key) const {
        for (const auto& entry : config) {
            if (entry.key == key) {
                return entry.value;
            }
        }
        return "";
    }
    
    virtual const std::vector<ExperimentConfig>& getConfig() const { return config; }
    
    // Experiment metrics
    virtual void recordMetric(const std::string& name, double value) {
        for (auto& pair : metrics) {
            if (pair.first == name) {
                pair.second = value;
                return;
            }
        }
        metrics.emplace_back(name, value);
    }
    
    virtual double getMetric(const std::string& name) const {
        for (const auto& pair : metrics) {
            if (pair.first == name) {
                return pair.second;
            }
        }
        return 0.0;
    }
    
    virtual bool hasMetric(const std::string& name) const {
        for (const auto& pair : metrics) {
            if (pair.first == name) {
                return true;
            }
        }
        return false;
    }
    
    virtual const std::vector<std::pair<std::string, double>>& getMetrics() const { return metrics; }
    
    // Step recording
    virtual void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                           size_t firingCount, float avgFiringRate) {
        StepData data;
        data.step = step;
        data.neuronCount = neuronCount;
        data.synapseCount = synapseCount;
        data.firingCount = firingCount;
        data.avgFiringRate = avgFiringRate;
        stepData.push_back(data);
    }
    
    virtual void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                           size_t firingCount, float avgFiringRate, float totalReward,
                           float simulationTime = 0.0f) {
        StepData data;
        data.step = step;
        data.neuronCount = neuronCount;
        data.synapseCount = synapseCount;
        data.firingCount = firingCount;
        data.avgFiringRate = avgFiringRate;
        data.totalReward = totalReward;
        data.simulationTime = simulationTime;
        stepData.push_back(data);
    }
    
    virtual const std::vector<StepData>& getStepData() const { return stepData; }
    
    // Notes and logging
    virtual void addNote(const std::string& note) {
        notes.push_back(note);
    }
    
    virtual const std::vector<std::string>& getNotes() const { return notes; }
    
    // Step range
    virtual SimulationStep getStartStep() const { return startStep; }
    virtual void setStartStep(SimulationStep step) { startStep = step; }
    
    virtual SimulationStep getEndStep() const { return endStep; }
    virtual void setEndStep(SimulationStep step) { endStep = step; }
    
    // Abstract methods for experiment-specific functionality
    virtual void initialize() {}
    virtual bool run() = 0;
    virtual void cleanup() {}
    
    // Utility methods
    virtual void printSummary() const {
        if (name.empty()) {
            std::cout << "Experiment: " << name << std::endl;
        }
        
        // Print configuration
        std::cout << "Configuration:" << std::endl;
        for (const auto& entry : config) {
            std::cout << "  " << entry.key << " = " << entry.value << " (" << entry.description << ")" << std::endl;
        }
        
        // Print metrics
        if (!metrics.empty()) {
            std::cout << "Metrics:" << std::endl;
            for (const auto& pair : metrics) {
                std::cout << "  " << pair.first << " = " << std::fixed << std::setprecision(6) << pair.second << std::endl;
            }
        }
        
        // Print step statistics if available
        if (!stepData.empty()) {
            std::cout << "Step statistics (" << stepData.size() << " steps):" << std::endl;
            
            size_t totalNeurons = 0, totalSynapses = 0, totalFiring = 0;
            double totalFiringRate = 0.0;
            
            for (const auto& data : stepData) {
                totalNeurons += data.neuronCount;
                totalSynapses += data.synapseCount;
                totalFiring += data.firingCount;
                totalFiringRate += data.avgFiringRate;
            }
            
            std::cout << "  Average neurons: " << std::fixed << std::setprecision(2) << (totalNeurons / (double)stepData.size()) << std::endl;
            std::cout << "  Average synapses: " << std::fixed << std::setprecision(2) << (totalSynapses / (double)stepData.size()) << std::endl;
            std::cout << "  Average firing neurons: " << std::fixed << std::setprecision(2) << (totalFiring / (double)stepData.size()) << std::endl;
            std::cout << "  Average firing rate: " << std::fixed << std::setprecision(4) << (totalFiringRate / (double)stepData.size()) << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Static utility methods
    static std::string formatTime(double seconds) {
        std::ostringstream oss;
        if (seconds < 60.0) {
            oss << std::fixed << std::setprecision(2) << seconds << "s";
        } else if (seconds < 3600.0) {
            oss << std::fixed << std::setprecision(2) << (seconds / 60.0) << "min";
        } else {
            oss << std::fixed << std::setprecision(2) << (seconds / 3600.0) << "h";
        }
        return oss.str();
    }
    
protected:
    std::string name;
    std::vector<ExperimentConfig> config;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<StepData> stepData;
    std::vector<std::string> notes;
    SimulationStep startStep;
    SimulationStep endStep;
};

} // namespace nlm
