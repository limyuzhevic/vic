// AgentBrain.hpp - Enhanced AgentBrain with cognition integration
#pragma once

#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;

class AgentBrainEnhanced : public AgentBrain {
public:
    AgentBrainEnhanced(std::shared_ptr<Brain> brain);
    ~AgentBrainEnhanced() override;
    
    // Enhanced initialization with cognition systems
    void initialize(const SimpleWorld& world) override;
    
    // Enhanced sensory processing with cognition integration
    void processSensoryInput(const SensoryPercept& percept) override;
    
    // Enhanced action selection with cognitive planning
    MotorCommand decodeMotorCommand() override;
    
    // Enhanced reward modulation with learning integration
    void applyRewardModulation(float reward, float predictedReward) override;
    
    // Update cognitive systems
    void updateCognitiveSystems(double timestep);
    
    // Get cognitive system states
    float getPlanningConfidence() const;
    float getConceptClarity() const;
    float getAttentionFocus() const;
    
    // Get memory usage statistics
    size_t getWorkingMemoryUsage() const;
    size_t getEpisodicMemoryUsage() const;
    
    // Enable/disable cognitive features
    void enablePlanning(bool enable);
    void enableConceptFormation(bool enable);
    void enableAttention(bool enable);
    void enablePrediction(bool enable);
    
    bool isPlanningEnabled() const;
    bool isConceptFormationEnabled() const;
    bool isAttentionEnabled() const;
    bool isPredictionEnabled() const;
    
private:
    // Cognitive systems integration
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<AttentionalSelection> attention_;
    std::shared_ptr<PredictionSystem> predictionSystem_;
    std::shared_ptr<Curiosity> curiosity_;
    std::shared_ptr<Dopamine> dopamine_;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    
    // Cognitive state tracking
    float planningConfidence_;
    float conceptClarity_;
    float attentionFocus_;
    
    // Cognitive feature flags
    bool planningEnabled_;
    bool conceptFormationEnabled_;
    bool attentionEnabled_;
    bool predictionEnabled_;
    
    // Processing buffers for cognitive integration
    std::vector<float> recentActions_;
    std::vector<float> recentRewards_;
    std::vector<float> recentObservations_;
    
    // Process cognitive state for action planning
    void updateCognitiveState();
    
    // Apply cognitive influences to motor command selection
    MotorCommand applyCognitiveInfluence(MotorCommand baseCommand);
    
    // Extract cognitive features from current brain state
    void extractCognitiveFeatures();
    
    // Store cognitive observations for learning
    void storeCognitiveObservation(float reward, const std::vector<float>& state);
};

} // namespace nlm