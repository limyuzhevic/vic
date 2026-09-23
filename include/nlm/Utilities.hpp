// NLM Utility Functions
// Common helper functions for NLM operations

#pragma once

#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>
#include <memory>

namespace nlm {

// Utility functions for neural operations
namespace utils {

// Generate random connectivity matrix (probability-based)
inline std::vector<std::vector<float>> generateConnectivityMatrix(
    size_t rows, size_t cols, float probability, RandomGenerator& rng) {
    std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols, 0.0f));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (rng.uniformReal(0.0f, 1.0f) < probability) {
                matrix[i][j] = 1.0f;
            }
        }
    }
    
    return matrix;
}

// Calculate statistics for a neuron population
inline struct PopulationStats {
    float averageMembranePotential;
    float averageFiringRate;
    float standardDeviation;
    float minPotential;
    float maxPotential;
    size_t activeNeuronCount;
} getPopulationStats(const std::vector<Neuron*>& neurons) {
    PopulationStats stats = {0.0f, 0.0f, 0.0f, 100.0f, -100.0f, 0};
    
    if (neurons.empty()) {
        return stats;
    }
    
    std::vector<float> potentials;
    potentials.reserve(neurons.size());
    
    for (const auto* neuron : neurons) {
        float potential = neuron->getMembranePotential();
        float firingRate = neuron->getFiringRate();
        
        potentials.push_back(potential);
        stats.averageMembranePotential += potential;
        stats.averageFiringRate += firingRate;
        
        if (potential < stats.minPotential) stats.minPotential = potential;
        if (potential > stats.maxPotential) stats.maxPotential = potential;
        
        if (neuron->isFiring()) {
            stats.activeNeuronCount++;
        }
    }
    
    size_t count = neurons.size();
    stats.averageMembranePotential /= count;
    stats.averageFiringRate /= count;
    
    // Calculate standard deviation
    float sumSqDiff = 0.0f;
    for (float potential : potentials) {
        float diff = potential - stats.averageMembranePotential;
        sumSqDiff += diff * diff;
    }
    stats.standardDeviation = std::sqrt(sumSqDiff / count);
    
    return stats;
}

// Format simulation results for output
inline std::string formatSimulationResults(const Brain& brain, SimulationStep step) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "Step: " << step
       << " | Neurons: " << brain.getTotalNeuronCount()
       << " | Synapses: " << brain.getTotalSynapseCount()
       << " | Total Spikes: " << brain.getTotalSpikeCount()
       << " | Firing Rate: " << std::setprecision(1) << brain.getAverageFiringRate()
       << " Hz"
       << " | Active: " << brain.getActiveNeuronCount()
       << " | E/I Ratio: " << std::setprecision(3) << brain.getExcitationInhibitionRatio();
    
    return ss.str();
}

// Convert configuration to human-readable format
inline std::string formatConfigForHuman(const Config& config) {
    std::stringstream ss;
    ss << std::left << std::setw(25) << "Setting" 
       << std::left << std::setw(15) << "Value" 
       << std::left << std::setw(10) << "Source" 
       << std::endl;
    ss << std::string(50, '-') << std::endl;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::string valueStr;
        
        auto strVal = config.get<std::string>(key);
        if (strVal.has_value()) {
            valueStr = strVal.value();
        } else {
            auto intVal = config.get<int64_t>(key);
            if (intVal.has_value()) {
                valueStr = std::to_string(intVal.value());
            } else {
                auto doubleVal = config.get<double>(key);
                if (doubleVal.has_value()) {
                    valueStr = std::to_string(doubleVal.value());
                } else {
                    auto boolVal = config.get<bool>(key);
                    if (boolVal.has_value()) {
                        valueStr = boolVal.value() ? "true" : "false";
                    }
                }
            }
        }
        
        auto sourceVal = config.get<int>(key);
        std::string sourceStr = sourceVal.has_value() ? std::to_string(sourceVal.value()) : "?";
        
        ss << std::left << std::setw(25) << key 
           << std::left << std::setw(15) << valueStr 
           << std::left << std::setw(10) << sourceStr 
           << std::endl;
    }
    
    return ss.str();
}

// Validate configuration values
inline bool validateConfig(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Check for required configuration keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "simulation_timestep",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate numeric ranges
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0 || neuronCount > 100000) {
        errors.push_back("Invalid neuron_count: must be between 1 and 100000");
    }
    
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    if (regionCount <= 0 || regionCount > 100) {
        errors.push_back("Invalid region_count: must be between 1 and 100");
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        errors.push_back("Invalid simulation_timestep: must be between 0.0 and 1.0 seconds");
    }
    
    auto connectionProb = config.getOr<double>("connection_probability", 0.0);
    if (connectionProb < 0.0 || connectionProb > 1.0) {
        errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
    }
    
    return errors.empty();
}

// Generate debug output for troubleshooting
inline std::string generateDebugOutput(const Brain& brain, const std::string& context) {
    std::stringstream ss;
    ss << "=== Debug Output: " << context << " ===" << std::endl;
    ss << "Brain Statistics:" << std::endl;
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    ss << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    // Get region information
    auto regionIds = brain.getRegionIds();
    ss << "Regions: " << regionIds.size() << std::endl;
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain.getRegion(regionIds[i]);
        if (region) {
            auto neurons = region->getAllNeurons();
            ss << "  Region " << i << ": " << neurons.size() << " neurons" << std::endl;
        }
    }
    
    return ss.str();
}

// Create reproducible random seed sequence
inline std::vector<uint64_t> generateRandomSeedSequence(uint64_t baseSeed, size_t count) {
    std::vector<uint64_t> seeds;
    std::mt19937_64 rng(baseSeed);
    
    for (size_t i = 0; i < count; ++i) {
        seeds.push_back(rng());
    }
    
    return seeds;
}

} // namespace utils

} // namespace nlm