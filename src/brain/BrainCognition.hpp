#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainCognition {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainCognition() = default;
    ~BrainCognition() = default;
    
    // Neural planner operations
    void initialize(class Brain* brain);
    void update(float timestep);
    void setPlanningDepth(int depth);
    
    // Concept formation operations
    void updateConceptFormation(float timestep);
    void learnConcept(const std::vector<float>& pattern);
    float evaluateConcept(const std::vector<float>& pattern) const;
    
    // Attentional selection operations
    void initializeAttention(class Brain* brain);
    void update(float timestep);
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Cognition statistics
    size_t getConceptCount() const;
    float getAttentionFocus() const;
    
private:
    // Neural planner
    int planningDepth;
    class NeuralPlanner* planner;
    
    // Concept formation
    std::vector<std::vector<float>> concepts;
    float conceptLearningRate;
    
    // Attentional selection
    float attentionInhibitionStrength;
    float attentionExcitationStrength;
    class AttentionalSelection* attention;
    
    // Internal state
    class Brain* brainRef;
};

} // namespace nlm