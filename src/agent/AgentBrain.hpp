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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
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
    
    // Slowly adapting prediction of the visual scene, for novelty
    // detection (see processSensoryInput). Not the previous frame:
    // per-frame differencing cannot work here, because the world
    // changes vision by only ~0-0.004 per action step while moving.
    std::vector<float> predictedVision_;
    float sensoryNoveltyDecay_;
    // Strongest touch/proximity signal in the most recent percept
    // (0 = nothing within touch range, 1 = touching). Used only for the
    // innate proximity reflex in decodeFromMotorNeurons().
    float maxTouch_ = 0.0f;
    // Phase 5 vision-guided reflex features, derived from the same
    // percept the brain already receives (no world label is leaked):
    // forwardTouch_ is the strongest touch in the forward cone (touch
    // sensors 0, 1, 7 of 8, i.e. -45..+45 degrees); forwardObjectVision_
    // is the closest forward-cone vision value (centre rays 5..10 of
    // 16) below the empty value 1.0. The world's vision encoding makes
    // a close bright object ~0.65-0.8 (resource), a close dark object
    // ~0.24-0.3 (hazard) and a close wall ~0.1-0.2 — so a value above
    // 0.5 with forward touch is a resource cue, and a value below 0.35
    // is a hazard/wall cue. Touch alone cannot tell them apart; vision
    // in the same percept can, and only that is used here.
    float forwardTouch_ = 0.0f;
    float forwardObjectVision_ = 1.0f;
    // Phase 5 winner-take-all state: the motor group that won the last
    // decode, and by how much. On the next presentation its competitors
    // receive a lateral-inhibition pulse (see processSensoryInput), so
    // a decisive winner suppresses its rivals instead of every group
    // saturating together at 70-90 Hz.
    int lastWinnerGroup_ = -1;
    float lastWinnerMargin_ = 0.0f;
    int consecutiveWinnerCount_ = 0;
    // Phase 5 action-diversity state, updated from final decoded
    // actions: steps since the last turn, and how many times the same
    // movement has repeated in a row. Used for a turn-diversity bias
    // and a repetition-fatigue penalty in decode, not a pathfinder.
    int stepsSinceTurn_ = 0;
    int repeatCount_ = 0;
    // Phase 5 diagnostic counters (not used for decisions): how often
    // the vision-gated reflex bonus / hazard suppression / WTA pulse
    // actually fired, so demo reports can tell a live mechanism from
    // a dead one.
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int wtaPulseCount_ = 0;
    // Last decoded command, for the interact no-repeat rule below.
    MotorCommand lastDecoded_ = MotorCommand::Wait;
    bool hasLastDecoded_ = false;
    // Phase 6 wired cognition: the brain owns the planner and concept
    // formation, but AgentBrain owns the self-model and social
    // learning (Brain never owned them) and calls them from the loop,
    // so they are no longer library-only classes. The self-model is
    // fed every loop turn (previous percept -> current percept for
    // the action just taken); social learning is fed only when a
    // teacher/other agent is observed, via observeOtherAgent().
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_ = false;
    bool socialReady_ = false;
    // Last percept's touch vector, kept so applyRewardModulation can
    // present the pattern that actually led to a reward to concept
    // formation (pattern -> action -> reward, in that order).
    std::vector<float> lastTouch_;
    std::string lastText_;
    // Phase 6 counters for planner/concept/self/social influence.
    int plannerOverrideCount_ = 0;
    int plannerAgreementCount_ = 0;
    int plannerDecisionCount_ = 0;
    int conceptBiasCount_ = 0;
    int selfBiasCount_ = 0;
    int socialBiasCount_ = 0;
    // Concept pattern for the current percept, in closeness form
    // (1 - vision, 0 = empty) plus touch, for concept bias lookup.
    std::vector<float> conceptPattern() const;
    // Phase 5 thought trace: last perceived scene summary, last
    // planner suggestion (from learned action quality), last decoded
    // action, and its outcome reward. Filled across one loop turn.
    std::vector<float> lastVision_;
    std::string thoughtPercept_ = "none";
    MotorCommand thoughtPlanned_ = MotorCommand::Wait;
    bool hasThoughtPlanned_ = false;
    float thoughtReward_ = 0.0f;
    bool hasThoughtOutcome_ = false;
};

} // namespace nlm
