#pragma once

#include "SystemRegistry.hpp"
#include "SystemWrappers.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// System factory implementations
inline std::unique_ptr<BrainSystem> createWorkingMemorySystem(Brain* brain) {
    // Access the existing working memory from the brain
    if (brain) {
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            return createWorkingMemoryWrapper(workingMemory);
        }
    }
    return nullptr;
}

inline std::unique_ptr<BrainSystem> createEpisodicMemorySystem(Brain* brain) {
    if (brain) {
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            return createEpisodicMemoryWrapper(episodicMemory);
        }
    }
    return nullptr;
}

inline std::unique_ptr<BrainSystem> createAssociativeMemorySystem(Brain* brain) {
    // For now, return nullptr as NeuralAssociativeMemory doesn't have a get method
    // This would need to be added to Brain for full implementation
    return nullptr;
}

inline std::unique_ptr<BrainSystem> createPredictionSystem(Brain* brain) {
    if (brain) {
        auto* prediction = brain->getPredictionSystem();
        if (prediction) {
            return createPredictionSystemWrapper(prediction);
        }
    }
    return nullptr;
}

inline std::unique_ptr<BrainSystem> createDevelopmentSystem(Brain* brain) {
    if (brain) {
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            return createDevelopmentSystemWrapper(devSystem);
        }
    }
    return nullptr;
}

// Function to register all default systems
inline void registerDefaultSystems() {
    auto& registry = SystemRegistry::getInstance();
    
    // Register memory systems
    registry.registerSystem("WorkingMemory", createWorkingMemorySystem);
    registry.registerSystem("EpisodicMemory", createEpisodicMemorySystem);
    registry.registerSystem("AssociativeMemory", createAssociativeMemorySystem);
    
    // Register other systems (simplified for now)
    registry.registerSystem("PredictionSystem", createPredictionSystem);
    registry.registerSystem("DevelopmentSystem", createDevelopmentSystem);
}

} // namespace nlm
