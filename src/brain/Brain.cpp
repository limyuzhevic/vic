// Brain.cpp - Main interface implementation
// Contains minimal constructor, destructor, and bridge to specialized files

#pragma once

#include "Brain.hpp"
#include "Brain_Impl.hpp"
#include "NeuralComputation.hpp"
#include "Plasticity.hpp"
#include "MemorySystems.hpp"
#include "CognitiveSystems.hpp"
#include "Neuromodulation.hpp"
#include "Development.hpp"
#include "Checkpoint.hpp"

namespace nlm {
    
    // Forward declarations for specialized systems
    class NeuralComputationSystem;
    class PlasticitySystem;
    class MemorySystems;
    class CognitiveSystems;
    class NeuromodulationSystem;
    class DevelopmentSystem;
    class CheckpointSystem;
    
    // Create global instances of specialized systems
    std::unique_ptr<NeuralComputationSystem> g_neuralComputation;
    std::unique_ptr<PlasticitySystem> g_plasticity;
    std::unique_ptr<MemorySystems> g_memorySystems;
    std::unique_ptr<CognitiveSystems> g_cognitiveSystems;
    std::unique_ptr<NeuromodulationSystem> g_neuromodulation;
    std::unique_ptr<DevelopmentSystem> g_development;
    std::unique_ptr<CheckpointSystem> g_checkpoint;
    
    // Initialize all specialized systems
    void initializeSpecializedSystems(std::shared_ptr<Config> config, Brain* brain) {
        g_neuralComputation = std::make_unique<NeuralComputationSystem>();
        g_plasticity = std::make_unique<PlasticitySystem>();
        g_memorySystems = std::make_unique<MemorySystems>();
        g_cognitiveSystems = std::make_unique<CognitiveSystems>();
        g_neuromodulation = std::make_unique<NeuromodulationSystem>();
        g_development = std::make_unique<DevelopmentSystem>();
        g_checkpoint = std::make_unique<CheckpointSystem>();
        
        if (g_neuralComputation) g_neuralComputation->initialize(config, brain);
        if (g_plasticity) g_plasticity->initialize(config, brain);
        if (g_memorySystems) g_memorySystems->initialize(config, brain);
        if (g_cognitiveSystems) g_cognitiveSystems->initialize(config, brain);
        if (g_neuromodulation) g_neuromodulation->initialize(config, brain);
        if (g_development) g_development->initialize(config, brain);
        if (g_checkpoint) {
            g_checkpoint->configure("./checkpoints", 10000, 5, true);
            g_checkpoint->setBrain(brain->getSharedPtr());
        }
    }
    
    // Get references to specialized systems
    NeuralComputationSystem* getNeuralComputation() { return g_neuralComputation.get(); }
    PlasticitySystem* getPlasticity() { return g_plasticity.get(); }
    MemorySystems* getMemorySystems() { return g_memorySystems.get(); }
    CognitiveSystems* getCognitiveSystems() { return g_cognitiveSystems.get(); }
    NeuromodulationSystem* getNeuromodulation() { return g_neuromodulation.get(); }
    DevelopmentSystem* getDevelopment() { return g_development.get(); }
    CheckpointSystem* getCheckpoint() { return g_checkpoint.get(); }
    
} // namespace nlm
