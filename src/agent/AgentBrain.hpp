#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Cognitive system integration
    enum class CognitiveMode {
        Passive,      // No cognitive processing, only basic sensory/motor
        Active,       // Basic attention and concept formation
        Planning,     // Full planning capabilities
        Reflective     // Memory-based reflection and evaluation
    };
    
    // Cognitive system access
    NeuralPlanner* getPlanner() const { return planner_.get(); }
    ConceptFormation* getConceptFormation() const { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() const { return attention_.get(); }
    Curiosity* getCuriositySystem() const { return curiositySystem_.get(); }
    PredictionSystem* getPredictionSystem() const { return predictionSystem_.get(); }
    DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem_.get(); }
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    
    // Advanced features
    void setCognitiveMode(CognitiveMode mode) { cognitiveMode_ = mode; }
    CognitiveMode getCognitiveMode() const { return cognitiveMode_; }
    
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setPlanningDepth(int depth) { planningDepth_ = depth; }
    void setAttentionSpan(float span) { attentionSpan_ = span; }
    
    void enableAdvancedPlanning(bool enable) { advancedPlanning_ = enable; }
    void enableConceptLearning(bool enable) { conceptLearning_ = enable; }
    void enableSocialLearning(bool enable) { socialLearning_ = enable; }
    
    bool isAdvancedPlanningEnabled() const { return advancedPlanning_; }
    bool isConceptLearningEnabled() const { return conceptLearning_; }
    bool isSocialLearningEnabled() const { return socialLearning_; }
    
    // State tracking for learning
    void trackConcept(const std::string& concept) { concepts_.insert(concept); }
    bool hasConcept(const std::string& concept) const { return concepts_.find(concept) != concepts_.end(); }
    size_t getConceptCount() const { return concepts_.size(); }
    
    void trackSocialInteraction(const std::string& interaction) { socialInteractions_.insert(interaction); }
    bool hasSocialInteraction(const std::string& interaction) const { return socialInteractions_.find(interaction) != socialInteractions_.end(); }
    
    void recordPredictionError(float error) { predictionError_ = error; }
    float getAveragePredictionError() const { return averagePredictionError_; }
    
    // Advanced processing methods
    void processConceptLearning(const std::vector<float>& patterns);
    void processSocialLearning(const std::vector<float>& socialPatterns);
    void updatePlanningSystem(double timestep);
    void updateAttentionSystem(double timestep);
    
    // State save/load
    void saveState(std::ostream& stream) const;
    void loadState(std::istream& stream);

private:
    // Cognitive system pointers
    std::shared_ptr<NeuralPlanner> planner_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<AttentionalSelection> attention_;
    std::shared_ptr<Curiosity> curiositySystem_;
    std::shared_ptr<PredictionSystem> predictionSystem_;
    std::shared_ptr<DevelopmentSystem> developmentSystem_;
    
    // Memory system pointers
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Advanced features
    CognitiveMode cognitiveMode_;
    NeuronId attentionTarget_;
    int planningDepth_;
    float curiosityThreshold_;
    float attentionSpan_;
    
    // Feature flags
    bool advancedPlanning_;
    bool conceptLearning_;
    bool socialLearning_;
    
    // Learning tracking
    std::set<std::string> concepts_;
    std::set<std::string> socialInteractions_;
    std::vector<float> predictionErrors_;
    float averagePredictionError_;
    
    // Planning and attention state
    std::vector<NeuronId> attentionQueue_;
    std::map<NeuronId, float> actionValues_;
    float totalExplorationBonus_;
    
    // Social and interaction state
    std::map<std::string, float> socialValueWeights_;
    std::vector<std::pair<NeuronId, NeuronId>> socialPairs_;
    std::vector<NeuronId> observedAgents_;
};

} // namespace nlm
