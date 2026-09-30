// MemorySystemFactory.hpp
#pragma once

#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class MemorySystemFactory {
public:
    // Memory system initialization helpers
    static void initializeWorkingMemory(NeuralWorkingMemory* memory, 
                                        const Config& config,
                                        size_t neuronCount);
    
    static void initializeEpisodicMemory(NeuralEpisodicMemory* memory,
                                         const Config& config);
    
    static void initializeAssociativeMemory(NeuralAssociativeMemory* memory,
                                            Brain* brain);
    
    // Prediction system initialization
    static void initializePredictionSystem(PredictionSystem* predictionSystem);
    
    // Cognition system initialization
    static void initializeCognitionSystems(NeuralPlanner* planner,
                                           ConceptFormation* conceptFormation,
                                           AttentionalSelection* attention,
                                           const Config& config);
    
    // Neuromodulation system initialization
    static void initializeNeuromodulationSystems(Dopamine* dopamine,
                                                 Curiosity* curiosity,
                                                 Novelty* novelty,
                                                 PredictionError* predictionError,
                                                 Brain* brain);
    
    // Batch memory operations
    static void initializeAllMemorySystems(MemorySystemInterface& systems,
                                           const Config& config,
                                           size_t neuronCount);
    
    // Memory statistics and monitoring
    static std::map<std::string, double> collectMemoryStatistics(
        const MemorySystemInterface& systems, const Brain& brain);
    
    static void logMemoryStatus(const MemorySystemInterface& systems,
                               const Brain& brain);
};

} // namespace nlm