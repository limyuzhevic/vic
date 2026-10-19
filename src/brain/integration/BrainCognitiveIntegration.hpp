#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainCognitiveIntegration {
public:
    BrainCognitiveIntegration();
    ~BrainCognitiveIntegration();
    
    // Initialize all cognitive systems
    void initializeCognitiveSystems(Brain* brain);
    
    // Integrate planning system
    void integratePlanningSystem(Brain* brain);
    
    // Integrate concept formation system
    void integrateConceptFormationSystem(Brain* brain);
    
    // Integrate attentional selection system
    void integrateAttentionalSelectionSystem(Brain* brain);
    
    // Integrate cognition with memory
    void integrateCognitionWithMemory(Brain* brain);
    
    // Process sensory information through cognitive systems
    std::vector<float> processSensorimotor(Brain* brain, const std::vector<float>& sensoryInput);
    
    // Apply cognitive evaluation
    float evaluateAction(Brain* brain, const std::vector<float>& state,
                        const std::vector<float>& action);
    
    // Update cognitive systems
    void updateCognitiveSystems(Brain* brain, TimestepDuration dt);
    
    // Get cognitive planning
    std::vector<float> getPlanningOutput(Brain* brain, const std::vector<float>& state);
    
    // Get concept representations
    std::vector<float> getConceptRepresentations(Brain* brain, size_t conceptId);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm