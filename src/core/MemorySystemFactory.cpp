// MemorySystemFactory.cpp
#include "MemorySystemFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void MemorySystemFactory::initializeWorkingMemory(NeuralWorkingMemory* memory, 
                                                 const Config& config,
                                                 size_t neuronCount) {
    if (!memory) return;
    
    memory->setCapacity(neuronCount / 10);
    memory->initialize(nullptr);  // Brain pointer passed during initialization
    NLM_LOG_INFO("Working memory initialized with capacity: " + 
                 std::to_string(memory->getCapacity()));
}

void MemorySystemFactory::initializeEpisodicMemory(NeuralEpisodicMemory* memory,
                                                 const Config& config) {
    if (!memory) return;
    
    memory->initialize(nullptr);  // Brain pointer passed during initialization
    memory->setMaxEpisodes(1000);
    NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
}

void MemorySystemFactory::initializeAssociativeMemory(NeuralAssociativeMemory* memory,
                                                    Brain* brain) {
    if (!memory) return;
    
    memory->initialize(brain);
    NLM_LOG_INFO("Associative memory initialized");
}

void MemorySystemFactory::initializePredictionSystem(PredictionSystem* predictionSystem) {
    if (!predictionSystem) return;
    
    // Prediction system initialization (if needed)
    // Current implementation doesn't require explicit initialization
    NLM_LOG_INFO("Prediction system ready for use");
}

void MemorySystemFactory::initializeCognitionSystems(NeuralPlanner* planner,
                                                   ConceptFormation* conceptFormation,
                                                   AttentionalSelection* attention,
                                                   const Config& config) {
    if (planner) {
        planner->initialize(nullptr);
        planner->setPlanningDepth(5);
        NLM_LOG_INFO("Neural planner initialized with planning depth: 5");
    }
    
    if (conceptFormation) {
        conceptFormation->initialize(nullptr);
        NLM_LOG_INFO("Concept formation initialized");
    }
    
    if (attention) {
        attention->initialize(nullptr);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        NLM_LOG_INFO("Attention system initialized with inhibition: 0.5, excitation: 1.5");
    }
}

void MemorySystemFactory::initializeNeuromodulationSystems(Dopamine* dopamine,
                                                           Curiosity* curiosity,
                                                           Novelty* novelty,
                                                           PredictionError* predictionError,
                                                           Brain* brain) {
    if (dopamine) {
        dopamine->initialize(brain);
        NLM_LOG_INFO("Dopamine system initialized");
    }
    
    if (curiosity) {
        curiosity->initialize(brain);
        NLM_LOG_INFO("Curiosity system initialized");
    }
    
    if (novelty) {
        novelty->initialize(brain);
        NLM_LOG_INFO("Novelty system initialized");
    }
    
    if (predictionError) {
        predictionError->initialize(brain);
        NLM_LOG_INFO("Prediction error system initialized");
    }
}

void MemorySystemFactory::initializeAllMemorySystems(MemorySystemInterface& systems,
                                                   const Config& config,
                                                   size_t neuronCount) {
    // Batch initialization through unified interface
    if (auto* workingMemory = systems.getWorkingMemory()) {
        initializeWorkingMemory(workingMemory, config, neuronCount);
    }
    
    if (auto* episodicMemory = systems.getEpisodicMemory()) {
        initializeEpisodicMemory(episodicMemory, config);
    }
    
    if (auto* associativeMemory = systems.getAssociativeMemory()) {
        // This would need brain pointer, handled separately
        NLM_LOG_INFO("Associative memory ready for initialization");
    }
    
    if (auto* predictionSystem = systems.getPredictionSystem()) {
        initializePredictionSystem(predictionSystem);
    }
    
    if (auto* planner = systems.getPlanner()) {
        if (auto* conceptFormation = systems.getConceptFormation()) {
            if (auto* attention = systems.getAttention()) {
                initializeCognitionSystems(planner, conceptFormation, attention, config);
            }
        }
    }
    
    NLM_LOG_INFO("All memory systems initialized through unified factory");
}

std::map<std::string, double> MemorySystemFactory::collectMemoryStatistics(
    const MemorySystemInterface& systems, const Brain& brain) {
    
    std::map<std::string, double> statistics;
    
    // Memory system statistics
    if (auto* workingMemory = systems.getWorkingMemory()) {
        statistics["working_memory_capacity"] = workingMemory->getCapacity();
        statistics["working_memory_current_size"] = workingMemory->getCurrentSize();
    }
    
    if (auto* episodicMemory = systems.getEpisodicMemory()) {
        statistics["episodic_memory_episodes"] = episodicMemory->getEpisodeCount();
    }
    
    if (auto* associativeMemory = systems.getAssociativeMemory()) {
        // Add associative memory metrics if available
        statistics["associative_memory_count"] = 0.0;  // Placeholder
    }
    
    // Brain statistics
    statistics["total_neurons"] = brain.getTotalNeuronCount();
    statistics["total_synapses"] = brain.getTotalSynapseCount();
    statistics["firing_neurons"] = brain.getFiringNeuronCount();
    statistics["average_firing_rate"] = brain.getAverageFiringRate();
    
    return statistics;
}

void MemorySystemFactory::logMemoryStatus(const MemorySystemInterface& systems,
                                         const Brain& brain) {
    
    NLM_LOG_INFO("=== MEMORY SYSTEM STATUS ===");
    
    // Memory system status
    if (auto* workingMemory = systems.getWorkingMemory()) {
        NLM_LOG_INFO("Working memory: " + std::to_string(workingMemory->getActiveTraces()) + 
                    " active traces, capacity: " + std::to_string(workingMemory->getCapacity()));
    }
    
    if (auto* episodicMemory = systems.getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory: " + std::to_string(episodicMemory->getEpisodeCount()) + 
                    " episodes stored");
    }
    
    if (auto* associativeMemory = systems.getAssociativeMemory()) {
        NLM_LOG_INFO("Associative memory: ready for associations");
    }
    
    // Brain status
    NLM_LOG_INFO("Brain status: " + std::to_string(brain.getRegionCount()) + 
                " regions, " + std::to_string(brain.getTotalNeuronCount()) + 
                " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                " synapses");
    
    NLM_LOG_INFO("=== END MEMORY STATUS ===");
}

} // namespace nlm