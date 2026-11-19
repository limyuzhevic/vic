#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <string>
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

    // Diagnostic: per motor group, in decode order. `activity` is mean
    // |membrane - resting| (the old decode metric); `firingRate` is mean
    // firing rate in Hz (now derived from counted spikes). Decode uses
    // firingRate, because a neuron that just spiked resets its membrane
    // to rest, so membrane deviation at sampling time is 0 for exactly
    // the neurons that fired — the old metric erased its own signal.
    struct MotorGroupActivity {
        const char* name;
        float activity;
        size_t refractoryCount;  // neurons in Refractory state (recently spiked)
        size_t size = 0;
        float firingRate = 0.0f;
    };
    std::vector<MotorGroupActivity> getMotorGroupActivities() const;
    
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

    // Thought trace (Phase 5): a one-line, human-readable record of the
    // most recent loop — what was perceived, what the planner suggested
    // from learned action quality, what the brain actually decoded, and
    // the outcome reward. The planner suggestion is advisory: the
    // brain's motor decode still decides the action, so a trace where
    // planned != acted is a real, reportable disagreement, not a script.
    std::string getThoughtTrace() const;

    // Learned preference trait, stored on the Brain so it survives a
    // checkpoint reload (see Brain::getLearnedPreference).
    float getLearnedPreference() const;

    // Phase 5 diagnostic counters (see private section): how often the
    // vision-gated reflex bonus / hazard suppression / WTA pulse fired.
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }

    // Phase 6: text-as-a-sense. Encode a teacher word into the sparse
    // pattern of text-neuron indices it drives (see .cpp). The string
    // itself is never passed to the brain; only this pattern is
    // injected, so association must be learned by plasticity, not by
    // string matching in the decision logic.
    static std::vector<size_t> encodeText(const std::string& text);
    // Internal (non-motor, non-sensory) firing rates, for the
    // text-association recall probe: a learned association means the
    // internal pattern evoked by text alone moves toward the pattern
    // evoked by the paired visual scene.
    std::vector<float> getInternalFiringRates() const;
    // Phase 6 thought/planning counters: how often the planner, a
    // learned concept, the self-model, or a social observation
    // actually changed the decoded action (not just advised it).
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }
    // Phase 6 wired systems, for demos/tests: concept count and the
    // self-model's mean confidence, plus social observation. A second
    // agent (teacher) is observed through observeOtherAgent; in a
    // single-agent world this stays at zero and social bias stays off.
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    size_t getSocialObservationCount() const;
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision);
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced configuration for expert users
    void setLearningRate(float rate);
    void setTemperature(float temp); // For exploration vs exploitation control
    void setExplorationRate(float rate);
    void setInhibitionStrength(float strength);
    void setExcitability(float exc);
    void setSpikingThreshold(float threshold);
    void setRefractoryPeriod(size_t period);
    void setNeuronCount(size_t count);
    void setConnectionDensity(float density);
    void setSensoryGain(float gain);
    void setMotorNoise(float noise);
    void setNoveltyThreshold(float threshold);
    void setCuriosityDrive(float drive);
    void setPredictionHorizon(float horizon);
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get current configuration
    float getLearningRate() const;
    float getTemperature() const;
    float getExplorationRate() const;
    float getInhibitionStrength() const;
    float getExcitability() const;
    float getSpikingThreshold() const;
    size_t getRefractoryPeriod() const;
    size_t getNeuronCount() const;
    float getConnectionDensity() const;
    float getSensoryGain() const;
    float getMotorNoise() const;
    float getNoveltyThreshold() const;
    float getCuriosityDrive() const;
    float getPredictionHorizon() const;
    
    // Advanced control functions
    void setGoal(const std::string& goal);
    void setPersonality(const std::string& personality);
    void setBehaviorStyle(const std::string& style);
    void setMemoryCapacity(size_t capacity);
    void setReplayInterval(size_t interval);
    void setConsolidationInterval(size_t interval);
    void setDevelopmentSpeed(float speed);
    
    // Debug and monitoring
    void enableDebugLogging(bool enable);
    void setDebugLevel(int level);
    bool isDebugLoggingEnabled() const;
    int getDebugLevel() const;
    
    std::string getConfigurationSummary() const;
    std::string getPerformanceStats() const;
    std::string getMemoryStats() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    // Internal (hidden) neurons, for the recall probe. Sensory neurons
    // are the input layer; the association between a word and a scene
    // lives in which internal neurons the two inputs jointly recruit.
    std::vector<Neuron*> internalNeurons_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Debug and monitoring state
    bool debugLoggingEnabled_;
    int debugLevel_;
    
    // Advanced configuration state
    float learningRate_;
    float temperature_;
    float explorationRate_;
    float inhibitionStrength_;
    float excitability_;
    float spikingThreshold_;
    size_t refractoryPeriod_;
    size_t neuronCount_;
    float connectionDensity_;
    float sensoryGain_;
    float motorNoise_;
    float noveltyThreshold_;
    float curiosityDrive_;
    float predictionHorizon_;
    
    // Behavioral control state
    std::string goal_;
    std::string personality_;
    std::string behaviorStyle_;
    size_t memoryCapacity_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    float developmentSpeed_;
    
    // Behavior rules for policy management
    std::vector<std::string> behaviorRules_;

} // namespace nlm
