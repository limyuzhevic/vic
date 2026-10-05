// Brain Cognition System - Manages all cognitive mechanisms
#ifndef BRAIN_COGNITION_SYSTEM_HPP
#define BRAIN_COGNITION_SYSTEM_HPP

#include "Brain.hpp"
#include <memory>

namespace nlm {

class BrainCognitionSystem {
public:
    explicit BrainCognitionSystem(Brain* brain);
    
    // Initialize cognition systems
    void initializeCognitionSystems();
    
    // Update cognition systems
    void updateCognitionSystems(TimestepDuration timestep);
    
    // Get cognition statistics
    float getCuriosityLevel() const { return curiosityLevel; }
    float getNoveltyLevel() const { return noveltyLevel; }
    float getPredictionError() const { return predictionError; }
    float getNeuromodulationLevel() const { return neuromodulationLevel; }
    
    // Get cognition system instances
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    // Cognitive processing methods
    void processCompetition(const std::vector<NeuronId>& competitors);
    void formConceptsFromCurrentActivity();
    
private:
    Brain* brain;
    
    // Cognition system components
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Neuromodulation systems for cognition
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Statistics
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    float neuromodulationLevel;
    
    // Helper methods
    void updateCuriosity(float novelty);
    void updatePrediction(float predictionError);
    void updateAttentionFromWorkingMemory();
};

} // namespace nlm

#endif // BRAIN_COGNITION_SYSTEM_HPP