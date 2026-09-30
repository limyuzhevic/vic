// HelperFunctions.cpp
#include "HelperFunctions.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

namespace Helper {

// Population creation and management
std::vector<std::unique_ptr<NeuralPopulation>> createBalancedPopulations(
    size_t totalNeurons, size_t regionCount) {
    
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    
    if (regionCount == 0) return populations;
    
    size_t neuronsPerRegion = totalNeurons / regionCount;
    
    // Create balanced populations for each neuron type
    auto createPopulation = [&](NeuronType type, size_t neuronCount) {
        if (neuronCount > 0) {
            // This would need access to NeuralPopulation constructor
            // For now, return empty vector as placeholder
            NLM_LOG_INFO("Would create " + std::to_string(type) + 
                        " population with " + std::to_string(neuronCount) + " neurons");
        }
    };
    
    createPopulation(NeuronType::Sensory, neuronsPerRegion / 4);
    createPopulation(NeuronType::Internal, neuronsPerRegion / 2);
    createPopulation(NeuronType::Motor, neuronsPerRegion / 4);
    
    return populations;
}

std::vector<Neuron*> extractSensoryNeurons(NeuralRegion* region) {
    std::vector<Neuron*> neurons;
    if (!region) return neurons;
    
    for (const auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == NeuronType::Sensory) {
            const auto& popNeurons = pop->getNeurons();
            neurons.insert(neurons.end(), popNeurons.begin(), popNeurons.end());
        }
    }
    
    return neurons;
}

std::vector<Neuron*> extractMotorNeurons(NeuralRegion* region) {
    std::vector<Neuron*> neurons;
    if (!region) return neurons;
    
    for (const auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == NeuronType::Motor) {
            const auto& popNeurons = pop->getNeurons();
            neurons.insert(neurons.end(), popNeurons.begin(), popNeurons.end());
        }
    }
    
    return neurons;
}

// Neuron and synapse utilities
float computeNeuronActivation(const Neuron* neuron) {
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    return std::abs(state.membranePotential - state.restingPotential);
}

std::vector<NeuronId> findActiveNeurons(const std::vector<Neuron*>& neurons) {
    std::vector<NeuronId> activeNeurons;
    
    for (const auto* neuron : neurons) {
        if (neuron && neuron->isActive()) {
            activeNeurons.push_back(neuron->getId());
        }
    }
    
    return activeNeurons;
}

size_t countFiringNeurons(const std::vector<Neuron*>& neurons) {
    size_t count = 0;
    for (const auto* neuron : neurons) {
        if (neuron && neuron->isFiring()) {
            ++count;
        }
    }
    return count;
}

float calculateAverageFiringRate(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float totalRate = 0.0f;
    size_t activeCount = 0;
    
    for (const auto* neuron : neurons) {
        if (neuron && neuron->isActive()) {
            totalRate += neuron->getFiringRate();
            ++activeCount;
        }
    }
    
    return activeCount > 0 ? totalRate / activeCount : 0.0f;
}

// Memory system utilities
std::vector<NeuronId> getActiveMemoryNeurons(const NeuralWorkingMemory* memory) {
    std::vector<NeuronId> neurons;
    if (!memory) return neurons;
    
    // This would extract active neurons from working memory
    // For now, return empty vector as placeholder
    return neurons;
}

float computeMemoryActivity(const NeuralWorkingMemory* memory) {
    if (!memory) return 0.0f;
    
    // Calculate average memory activity
    // This would access memory's internal state
    return 0.0f;  // Placeholder
}

// Logging and monitoring helpers
void logRegionInfo(const NeuralRegion* region) {
    if (!region) return;
    
    NLM_LOG_INFO("Region " + std::to_string(region->getId().index()) + 
                " (" + region->getName() + "): " +
                std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                std::to_string(region->getSynapseCount()) + " synapses");
}

void logMemorySystemStatus(const MemorySystemInterface& systems) {
    NLM_LOG_INFO("=== MEMORY SYSTEM STATUS ===");
    
    if (auto* workingMemory = systems.getWorkingMemory()) {
        NLM_LOG_INFO("Working memory: " + std::to_string(workingMemory->getActiveTraces()) + 
                    " active traces");
    }
    
    if (auto* episodicMemory = systems.getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory: " + std::to_string(episodicMemory->getEpisodeCount()) + 
                    " episodes");
    }
    
    if (auto* associativeMemory = systems.getAssociativeMemory()) {
        NLM_LOG_INFO("Associative memory: ready for associations");
    }
    
    NLM_LOG_INFO("=== END MEMORY STATUS ===");
}

void logStepProgress(int stepNumber, const std::string& description) {
    NLM_LOG_INFO("Step " + std::to_string(stepNumber) + ": " + description);
}

// Configuration helpers
std::string getLogPrefix(const std::string& systemName) {
    return "[" + systemName + "] ";
}

float getConfigOrDefault(const Config& config, const std::string& key, float defaultValue) {
    return config->getOr<float>(key, defaultValue);
}

size_t getConfigOrDefault(const Config& config, const std::string& key, size_t defaultValue) {
    return config->getOr<size_t>(key, defaultValue);
}

// Batch operations helpers
void injectCurrentToAllNeurons(NeuralRegion* region, NeuronType type, 
                              MembranePotential current) {
    if (!region) return;
    
    for (const auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == type) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->injectCurrent(current);
            }
        }
    }
}

void collectNeuronStatistics(const std::vector<Neuron*>& neurons,
                            size_t& total, size_t& firing, float& avgRate) {
    total = neurons.size();
    firing = countFiringNeurons(neurons);
    avgRate = calculateAverageFiringRate(neurons);
}

} // namespace Helper

} // namespace nlm