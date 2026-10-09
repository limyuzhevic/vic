#pragma once

#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class CognitionSystem {
public:
    CognitionSystem(NeuralPlanner* planner,
                   ConceptFormation* conceptFormation,
                   AttentionalSelection* attention);
    
    // Update cognitive functions with current neural state
    void updateCognition(TimestepDuration dt);
    
    // Process competition for attentional focus
    void updateAttention(const std::vector<NeuronId>& competitors);
    
    // Form new concepts from neural activity patterns
    void updateConceptFormation(const std::vector<NeuronId>& activeNeurons);
    
    // Generate action plans based on current state
    void updatePlanning(const std::vector<NeuronId>& activeNeurons);
    
    // Get cognitive state information
    float getAttentionStrength() const;
    float getPlanningDepth() const;
    float getConceptActivation() const;
    
    // Cognitive control
    void setAttentionStrength(float strength);
    void setPlanningDepth(size_t depth);
    void enableAttention(bool enable);
    
private:
    NeuralPlanner* planner;
    ConceptFormation* conceptFormation;
    AttentionalSelection* attention;
    
    bool attentionEnabled;
    bool conceptFormationEnabled;
    bool planningEnabled;
};
