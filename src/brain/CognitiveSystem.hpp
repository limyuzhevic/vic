#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// CognitiveSystem: High-level cognitive processes
// Manages planning, concept formation, and attention
class CognitiveSystem {
public:
    CognitiveSystem(std::shared_ptr<Config> config);
    ~CognitiveSystem();
    
    // Initialize cognitive systems
    bool initialize(Brain* brain);
    
    // Cognitive system access
    class NeuralPlanner* getPlanner() { return planner_.get(); }
    class ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    class AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Update cognitive processes
    void update(double timestep);
    
    // Concept formation
    void processConcepts(const std::vector<float>& neuralActivity);
    const std::vector<std::string>& getActiveConcepts() const { return activeConcepts_; }
    
    // Attention management
    void processCompetition(const std::vector<NeuronId>& competitors);
    void setAttentionStrengths(float inhibition, float excitation);
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Cognitive state
    std::vector<std::string> activeConcepts_;
    std::vector<float> conceptActivations_;
};

} // namespace nlm
