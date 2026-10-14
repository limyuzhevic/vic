#pragma once

#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "agent/AgentBrain.hpp"
#include <memory>
#include <string>

namespace nlm {

class Phase5CognitiveIntegration {
public:
    Phase5CognitiveIntegration();
    ~Phase5CognitiveIntegration();
    
    // Integration verification
    bool verifyIntegration();
    
    // Individual system tests
    void testSelfModel(size_t numTrials);
    void testSocialLearning(size_t numTrials);
    void testSpatialRepresentation(size_t numTrials);
    void testTemporalRelation(size_t numTrials);
    void testNeuralPrediction(size_t numTrials);
    void testAgentBrainCognitiveAccess(size_t numTrials);
    void testMemorySystemIntegration(size_t numTrials);
    void testBrainLoopIntegration(size_t numTrials);
    
    // System status and metrics
    void logSystemStatus() const;
    float getIntegrationScore() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods for testing
    bool verifySelfModelIntegration() const;
    bool verifySocialLearningIntegration() const;
    bool verifySpatialRepresentationIntegration() const;
    bool verifyTemporalRelationIntegration() const;
    bool verifyNeuralPredictionIntegration() const;
    bool verifyAgentBrainAccess() const;
    bool verifyMemorySystemIntegration() const;
    bool verifyBrainLoopIntegration() const;
};

} // namespace nlm

