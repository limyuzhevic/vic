#pragma once

#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "ISubsystem.hpp"
#include <memory>

namespace nlm {

class CognitionSystem : public ISubsystem {
public:
    CognitionSystem();
    ~CognitionSystem() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() const;
    void updatePrediction(float predictionError = 0.0f);
    float getPredictionError() const;
    void resetPredictionHistory();
    
    // Neural planning access
    NeuralPlanner* getPlanner() const;
    void setPlanningDepth(size_t depth);
    void setPlanningHorizon(float horizon);
    std::vector<Action> generateActionPlan();
    
    // Concept formation access
    ConceptFormation* getConceptFormation() const;
    void updateConceptFormation();
    void extractConceptsFromNeurons(const std::vector<NeuronId>& neurons);
    std::vector<std::vector<float>> getExtractedConcepts() const;
    
    // Attention system access
    void updateAttentionDynamics(float dt);
    void processAttentionSelection(const std::vector<NeuronId>& competitors);
    bool isNeuronAttended(NeuronId neuron) const;
    float getAttentionGain(NeuronId neuron) const;
    
    // Working memory integration
    void updateWorkingMemoryWithCognition();
    void retrieveInformationFromWorkingMemory();
    
    // Cognitive statistics
    size_t getPlanningNodes() const;
    size_t getConceptCount() const;
    float getCognitiveLoad() const;
    size_t getAttentionWinnersCount() const;
    
    // Cognitive state management
    void setCognitiveFocus(RegionId regionId);
    void releaseCognitiveFocus();
    std::vector<RegionId> getCognitiveFocusAreas() const;
    
    // Cognitive coordination
    void synchronizeCognitiveProcesses();
    void updatePredictionBasedOnAttention();
    void learnFromPredictionErrors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
