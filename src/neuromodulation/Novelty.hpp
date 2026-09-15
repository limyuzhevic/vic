#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class NeuralPlanner;
class ConceptFormation;

// Novelty detection signal
// Extended with effects on memory consolidation, exploration, concept formation, and development

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Enhanced novelty effects on cognitive systems
    void applyMemoryConsolidation(NeuralWorkingMemory* workingMemory, float deltaTime);
    void applyExplorationGuidance(NeuralPlanner* planner, float deltaTime);
    void applyConceptFormationEnhancement(ConceptFormation* conceptFormation, float deltaTime);
    void applyDevelopmentTriggering(class DevelopmentSystem* developmentSystem, float deltaTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
