#pragma once

#include "Brain.hpp"
#include "NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <numeric>

namespace nlm {

// Helper function to validate neuron parameters
inline bool validateNeuronParameters(const Neuron& neuron) {
    const auto& state = neuron.getState();
    return state.membranePotential >= -100.0f && 
           state.membranePotential <= 50.0f &&
           state.threshold >= -100.0f &&
           state.threshold <= 50.0f &&
           state.leakConductance >= 0.0f;
}

// Helper function to initialize neuron for specific region
inline void initializeNeuronForRegion(Neuron* neuron, NeuronType type) {
    if (!neuron) return;
    
    // Set neuron type
    neuron->setType(type);
    
    // Set initial membrane potential based on type
    float initialPotential = -65.0f;  // Default resting potential
    if (type == NeuronType::Sensory) {
        initialPotential = -60.0f;  // Slightly depolarized
    } else if (type == NeuronType::Motor) {
        initialPotential = -65.0f;  // Normal
    }
    
    neuron->setMembranePotential(initialPotential);
    
    // Set threshold based on type
    float threshold = -50.0f;  // Default threshold
    if (type == NeuronType::Sensory) {
        threshold = -45.0f;  // Lower threshold for sensory
    } else if (type == NeuronType::Motor) {
        threshold = -55.0f;  // Higher threshold for motor
    }
    
    neuron->setThreshold(threshold);
    
    // Set leak conductance
    float leakConductance = 0.1f;  // Default
    if (type == NeuronType::Sensory) {
        leakConductance = 0.12f;  // Slightly higher for sensory
    }
    
    neuron->setLeakConductance(leakConductance);
    
    // Set firing rate initial value
    neuron->setFiringRate(0.0f);
    
    // Validate neuron parameters
    if (!validateNeuronParameters(*neuron)) {
        throw SimulationException("Invalid neuron parameters after initialization");
    }
}

// Helper function to create random synaptic weight
inline float generateRandomSynapticWeight(float minWeight = -1.0f, float maxWeight = 1.0f) {
    static RandomGenerator rng(42);  // Fixed seed for reproducibility
    float weight = rng.uniformReal(minWeight, maxWeight);
    return std::clamp(weight, minWeight, maxWeight);
}

// Helper function to calculate E/I ratio
inline float calculateExcitationInhibitionRatio(const NeuralRegion& region) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto* synapse : region.getSynapses()) {
        float weight = synapse->getWeight();
        if (weight > 0) {
            totalExcitatory += weight;
        } else {
            totalInhibitory += std::abs(weight);
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    } else if (totalExcitatory > 0.0f) {
        return std::numeric_limits<float>::infinity();
    } else {
        return 0.0f;
    }
}

// Helper function to find neuron by ID in region
inline Neuron* findNeuronInRegion(NeuralRegion* region, NeuronId neuronId) {
    if (!region) return nullptr;
    
    for (auto& pop : region->getAllPopulations()) {
        for (auto* neuron : pop->getNeurons()) {
            if (neuron->getId() == neuronId) {
                return neuron;
            }
        }
    }
    return nullptr;
}

// Helper function to calculate average synaptic weight
inline float calculateAverageSynapticWeight(const NeuralRegion& region) {
    auto synapses = region.getSynapses();
    if (synapses.empty()) return 0.0f;
    
    float totalWeight = 0.0f;
    for (const auto* synapse : synapses) {
        totalWeight += synapse->getWeight();
    }
    
    return totalWeight / synapses.size();
}

// Helper function to count active neurons
inline size_t countActiveNeurons(const NeuralRegion& region) {
    size_t activeCount = 0;
    for (auto* neuron : region.getAllNeurons()) {
        if (neuron->isFiring()) {
            activeCount++;
        }
    }
    return activeCount;
}

// Helper function to validate configuration
inline bool validateConfiguration(const Config& config) {
    bool valid = true;
    
    // Check required parameters
    if (!config.has("neuron_count")) {
        throw ConfigurationException("Missing required parameter: neuron_count");
    }
    
    if (!config.has("region_count")) {
        throw ConfigurationException("Missing required parameter: region_count");
    }
    
    if (!config.has("simulation_timestep")) {
        throw ConfigurationException("Missing required parameter: simulation_timestep");
    }
    
    // Validate parameter values
    size_t neuronCount = config.getOr<size_t>("neuron_count", 0);
    size_t regionCount = config.getOr<size_t>("region_count", 0);
    double timestep = config.getOr<double>("simulation_timestep", 0.0);
    
    if (neuronCount == 0) {
        throw ConfigurationException("neuron_count must be positive");
    }
    
    if (regionCount == 0) {
        throw ConfigurationException("region_count must be positive");
    }
    
    if (timestep <= 0.0) {
        throw ConfigurationException("simulation_timestep must be positive");
    }
    
    if (regionCount > neuronCount) {
        throw ConfigurationException("region_count cannot exceed neuron_count");
    }
    
    return valid;
}

// Helper function to initialize plasticity systems
inline void initializePlasticitySystems(Brain* brain, const Config& config) {
    if (!brain) return;
    
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    if (stdp) {
        float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.02f);
        float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.015f);
        float tau = config.getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    if (structuralPlasticity) {
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
}

// Helper function to create standardized regions
inline void createStandardizedRegions(Brain* brain, size_t neuronCount, size_t regionCount) {
    if (!brain) return;
    
    // Calculate neurons per region
    size_t neuronsPerRegion = neuronCount / regionCount;
    if (neuronsPerRegion < 1) neuronsPerRegion = 1;
    
    // Create regions with standardized configuration
    for (size_t i = 0; i < regionCount; ++i) {
        std::string regionName = "Region_" + std::to_string(i + 1);
        auto regionId = brain->addRegion(regionName);
        
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        // Create populations with standardized sizes
        size_t sensoryNeurons = neuronsPerRegion / 4;
        size_t internalNeurons = neuronsPerRegion / 2;
        size_t motorNeurons = neuronsPerRegion / 4;
        
        // Ensure at least one neuron per population
        if (sensoryNeurons < 1) sensoryNeurons = 1;
        if (internalNeurons < 1) internalNeurons = 1;
        if (motorNeurons < 1) motorNeurons = 1;
        
        auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
        auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
        
        // Initialize neurons for their regions
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* internalPop = region->getPopulation(internalPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                initializeNeuronForRegion(neuron, NeuronType::Sensory);
            }
        }
        
        if (internalPop) {
            for (auto* neuron : internalPop->getNeurons()) {
                initializeNeuronForRegion(neuron, NeuronType::Internal);
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                initializeNeuronForRegion(neuron, NeuronType::Motor);
            }
        }
    }
}

// Helper function to collect I/O neurons
inline void collectIONeurons(Brain* brain, 
                            std::vector<Neuron*>& sensoryNeurons,
                            std::vector<Neuron*>& motorNeurons) {
    if (!brain) return;
    
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    for (auto* region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            } else if (type == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

// Helper function to initialize random connectivity
inline void initializeRandomConnectivity(
    Brain* brain, 
    float connectionProbability, 
    float weightMean, 
    float weightStdDev) {
    
    if (!brain) return;
    
    static RandomGenerator rng(42);  // Fixed seed for reproducibility
    
    for (auto* region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Create connections between neurons
        for (size_t i = 0; i < neuronCount; ++i) {
            for (size_t j = 0; j < neuronCount; ++j) {
                if (i == j) continue;  // Skip self-connections
                
                // Check if connection should be created
                float connectionChance = rng.uniformReal(0.0f, 1.0f);
                if (connectionChance > connectionProbability) {
                    continue;
                }
                
                // Determine if connection is excitatory or inhibitory
                bool isExcitatory = rng.uniformReal(0.0f, 1.0f) > 0.3f;  // 70% excitatory
                
                // Generate synaptic weight
                float weight = generateRandomSynapticWeight(weightMean - weightStdDev, 
                                                         weightMean + weightStdDev);
                if (!isExcitatory) {
                    weight = -weight;  // Make inhibitory
                }
                
                // Create synapse
                region->addSynapse(neurons[i]->getId(), neurons[j]->getId(), 
                                 weight, 1);  // Default delay of 1
            }
        }
    }
}

} // namespace nlm
