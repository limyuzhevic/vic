#pragma once

#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "BrainInitializer.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;
class Neuron;

class BrainCognitionIntegrator {
public:
    explicit BrainCognitionIntegrator(std::shared_ptr<Config> config);
    ~BrainCognitionIntegrator() = default;
    
    // Initialize cognition systems
    void initializePlanner(Brain* brain);
    void initializeConceptFormation(Brain* brain);
    void initializeAttention(Brain* brain);
    
    // Update cognition systems
    void updatePlanner(Brain* brain);
    void updateConceptFormation(Brain* brain, SimulationStep step, Timestamp time);
    void updateAttention(Brain* brain, TimestepDuration dt);
    
    // Competition and selection
    void processCompetition(Brain* brain, std::vector<NeuronId> competitors);
    void applyAttentionToWorkingMemory(Brain* brain);
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Configuration
    void configurePlanner(Brain* brain, size_t planningDepth);
    void configureAttention(Brain* brain, float inhibitionStrength, float excitationStrength);
    
    // State management
    void resetCognitionSystems(Brain* brain);
    void clearConceptHistory(Brain* brain);
    
    // Advanced cognition features
    void enableMetaCognition(Brain* brain, bool enabled);
    void enableAbstractReasoning(Brain* brain, bool enabled);
    void enableSocialCognition(Brain* brain, bool enabled);

private:
    // Cognition system instances
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Configuration parameters
    bool metaCognitionEnabled;
    bool abstractReasoningEnabled;
    bool socialCognitionEnabled;
    size_t planningDepth;
    float attentionInhibitionStrength;
    float attentionExcitationStrength;
    
    // Helper methods
    void setupCognitionConnections(Brain* brain);
    void updatePlanningFromMemory(Brain* brain);
    void formConceptsFromPatterns(Brain* brain, SimulationStep step, Timestamp time);
    void performAttentionalSelection(Brain* brain, TimestepDuration dt);
    void enableAdvancedCognitionFeatures(Brain* brain);
    
    // Integration with other systems
    void integrateWithMemorySystems(Brain* brain);
    void integrateWithNeuromodulation(Brain* brain);
    void integrateWithDevelopment(Brain* brain);
    
    // Competition and decision making
    void evaluateActionSequences(Brain* brain);
    void selectOptimalAction(Brain* brain);
    void applyLearningToCognition(Brain* brain, float learningRate);
};