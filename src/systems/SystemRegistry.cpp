// Implementation of SystemRegistry initialization
// This file should be included after all system headers are available

#include "SystemRegistry.hpp"
#include "systems/SpikeSystem.hpp"
#include "systems/NeuromodulationSystem.hpp"
#include "systems/Memory/WorkingMemorySystem.hpp"
#include "systems/Memory/EpisodicMemorySystem.hpp"
#include "systems/Memory/AssociativeMemorySystem.hpp"
#include "systems/PlasticitySystem.hpp"
#include "systems/DevelopmentSystem.hpp"
#include "systems/Cognition/NeuralPlannerSystem.hpp"
#include "systems/Cognition/ConceptFormationSystem.hpp"
#include "systems/Cognition/AttentionalSelectionSystem.hpp"
#include "systems/PredictionSystem.hpp"
#include "systems/CheckpointSystem.hpp"

namespace nlm {

void SystemRegistry::initializeDefaultFactories(Brain* brain) {
    // Clear any existing factories
    systemFactories_.clear();
    
    // Register default system factories
    // Each factory creates a system with the brain reference
    
    // Core neural computation systems
    registerSystem("SpikeSystem", [brain](Brain* b) { return std::make_unique<SpikeSystem>(b); });
    registerSystem("NeuromodulationSystem", [brain](Brain* b) { return std::make_unique<NeuromodulationSystem>(b); });
    
    // Memory systems
    registerSystem("WorkingMemorySystem", [brain](Brain* b) { return std::make_unique<WorkingMemorySystem>(b); });
    registerSystem("EpisodicMemorySystem", [brain](Brain* b) { return std::make_unique<EpisodicMemorySystem>(b); });
    registerSystem("AssociativeMemorySystem", [brain](Brain* b) { return std::make_unique<AssociativeMemorySystem>(b); });
    
    // Plasticity systems
    registerSystem("STDP", [brain](Brain* b) { return std::make_unique<STDP>(b); });
    registerSystem("Hebbian", [brain](Brain* b) { return std::make_unique<Hebbian>(b); });
    registerSystem("StructuralPlasticity", [brain](Brain* b) { return std::make_unique<StructuralPlasticity>(b); });
    
    // Development system
    registerSystem("DevelopmentSystem", [brain](Brain* b) { return std::make_unique<DevelopmentSystem>(b); });
    
    // Cognition systems
    registerSystem("NeuralPlannerSystem", [brain](Brain* b) { return std::make_unique<NeuralPlannerSystem>(b); });
    registerSystem("ConceptFormationSystem", [brain](Brain* b) { return std::make_unique<ConceptFormationSystem>(b); });
    registerSystem("AttentionalSelectionSystem", [brain](Brain* b) { return std::make_unique<AttentionalSelectionSystem>(b); });
    
    // Prediction and monitoring
    registerSystem("PredictionSystem", [brain](Brain* b) { return std::make_unique<PredictionSystem>(b); });
    registerSystem("CheckpointSystem", [brain](Brain* b) { return std::make_unique<CheckpointSystem>(b); });
    
    // Neuromodulators (individual systems)
    registerSystem("Dopamine", [brain](Brain* b) { return std::make_unique<Dopamine>(b); });
    registerSystem("Curiosity", [brain](Brain* b) { return std::make_unique<Curiosity>(b); });
    registerSystem("PredictionError", [brain](Brain* b) { return std::make_unique<PredictionError>(b); });
    registerSystem("Novelty", [brain](Brain* b) { return std::make_unique<Novelty>(b); });
}

} // namespace nlm
