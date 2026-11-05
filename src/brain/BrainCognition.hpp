#pragma once

#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;

class BrainCognition {
public:
    BrainCognition() = default;
    ~BrainCognition() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setCognitiveParameters(float planningDepth, float conceptFormThreshold, float attentionStrength);
    
    // Cognition updates (called during step)
    void updateAllCognition(Brain* brain, TimestepDuration timestep);
    void updatePlanning(Brain* brain, const std::vector<NeuronId>& activeNeurons);
    void updateConceptFormation(Brain* brain, const std::vector<NeuronId>& activeNeurons);
    void updateAttention(Brain* brain, const std::vector<NeuronId>& activeNeurons, TimestepDuration timestep);
    
    // Cognition access and control
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Cognition statistics and diagnostics
    void collectCognitionStats(Brain* brain);
    float getPlanningDepth() const { return planningDepth_; }
    float getConceptFormationThreshold() const { return conceptFormThreshold_; }
    float getAttentionStrength() const { return attentionStrength_; }
    size_t getActiveConcepts() const { return activeConcepts_; }
    
    // Cognition system management
    void resetCognition(Brain* brain);
    void enableCognitiveFunctions(Brain* brain, bool enablePlanning, bool enableConcepts, bool enableAttention);
    void processCognitionSignals(Brain* brain, const std::vector<NeuronId>& neuralActivity);
    
    // Cognitive-specific processing
    void planActions(Brain* brain, const std::vector<NeuronId>& motorNeurons);
    void formConcepts(Brain* brain, const std::vector<NeuronId>& sensoryNeurons);
    void selectAttention(Brain* brain, const std::vector<NeuronId>& competitors, TimestepDuration timestep);
    
private:
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // Cognition statistics
    float planningDepth_ = 5.0f;
    float conceptFormThreshold_ = 0.5f;
    float attentionStrength_ = 1.0f;
    size_t activeConcepts_ = 0;
    
    // Configuration parameters
    size_t planningSteps_ = 3;
    float conceptFormMinActivation_ = 0.3f;
    float attentionInhibitionStrength_ = 0.5f;
    float attentionExcitationStrength_ = 1.5f;
    
    // Helper methods
    void initializeCognitionSystems(Brain* brain);
    void validateCognitionState(Brain* brain) const;
    void updateCognitiveConnections(Brain* brain);
    void processCognitionLearning(Brain* brain, TimestepDuration timestep);
    
    friend class BrainCore; // For internal access
};

} // namespace nlm