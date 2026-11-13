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
    /**
     * @brief Constructor for AgentBrain
     * @param brain Shared pointer to the NLM brain instance
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor for AgentBrain
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with a world instance
     * @param world The SimpleWorld instance to operate in
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size from the brain
     * @return Number of sensory input features expected by the brain
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size for the world
     * @return Number of motor output features expected by the world
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process a sensory percept from the world and inject it into the brain
     * @param percept The sensory percept containing vision, touch, internal, proprioception, and audio data
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into a motor command for the world
     * @return Motor command to execute in the world
     */
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
    /**
     * @brief Get activity information for each motor neuron group
     * @return Vector of motor group activity information in decode order
     */
    std::vector<MotorGroupActivity> getMotorGroupActivities() const;
    
    /**
     * @brief Apply neuromodulation based on reward prediction
     * @param reward The actual reward received
     * @param predictedReward The predicted reward before taking action
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update the development system
     * @param timestep Time step duration for development updates
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get the current developmental stage of the agent
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get the current neuromodulation level (dopamine)
     * @return Current neuromodulation level
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get the current curiosity level
     * @return Current curiosity level
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get the current novelty level
     * @return Current novelty level
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get the prediction error from the brain's prediction system
     * @return Current prediction error
     */
    float getPredictionError() const;

    // Thought trace (Phase 5): a one-line, human-readable record of the
    // most recent loop — what was perceived, what the planner suggested
    // from learned action quality, what the brain actually decoded, and
    // the outcome reward. The planner suggestion is advisory: the
    // brain's motor decode still decides the action, so a trace where
    // planned != acted is a real, reportable disagreement, not a script.
    /**
     * @brief Get a human-readable thought trace of the last loop cycle
     * @return Thought trace string describing the loop
     */
    std::string getThoughtTrace() const;

    // Learned preference trait, stored on the Brain so it survives a
    // checkpoint reload (see Brain::getLearnedPreference).
    /**
     * @brief Get the learned preference trait
     * @return Current learned preference trait value
     */
    float getLearnedPreference() const;
    
    // Phase 5 diagnostic counters (see private section): how often the
    // vision-gated reflex bonus / hazard suppression / WTA pulse fired.
    /**
     * @brief Get the reflex bonus count (vision-gated reflex bonus)
     * @return Number of times the vision-gated reflex bonus fired
     */
    int getReflexBonusCount() const { return reflexBonusCount_; }
    
    /**
     * @brief Get the reflex suppress count (hazard suppression)
     * @return Number of times hazard suppression fired
     */
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    
    /**
     * @brief Get the WTA pulse count (winner-take-all pulse)
     * @return Number of times the WTA pulse fired
     */
    int getWtaPulseCount() const { return wtaPulseCount_; }
    
    // Phase 6: text-as-a-sense. Encode a teacher word into the sparse
    // pattern of text-neuron indices it drives (see .cpp). The string
    // itself is never passed to the brain; only this pattern is
    // injected, so association must be learned by plasticity, not by
    // string matching in the decision logic.
    /**
     * @brief Encode text into a sparse pattern of text-neuron indices
     * @param text The text string to encode
     * @return Vector of neuron indices that encode the text
     */
    static std::vector<size_t> encodeText(const std::string& text);
    // Internal (non-motor, non-sensory) firing rates, for the
    // text-association recall probe: a learned association means the
    // internal pattern evoked by text alone moves toward the pattern
    // evoked by the paired visual scene.
    /**
     * @brief Get internal firing rates (hidden layer neurons)
     * @return Vector of internal firing rates
     */
    std::vector<float> getInternalFiringRates() const;
    // Phase 6 thought/planning counters: how often the planner, a
    // learned concept, the self-model, or a social observation
    // actually changed the decoded action (not just advised it).
    /**
     * @brief Get the planner override count (how often planner overrode brain)
     * @return Number of times the planner overrode the brain's decision
     */
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    
    /**
     * @brief Get the planner agreement count (how often planner agreed with brain)
     * @return Number of times the planner agreed with the brain's decision
     */
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    
    /**
     * @brief Get the planner decision count (how often planner made decision)
     * @return Number of times the planner made a decision
     */
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    
    /**
     * @brief Get the concept bias count (how often concept influenced decision)
     * @return Number of times concept biased the decision
     */
    int getConceptBiasCount() const { return conceptBiasCount_; }
    
    /**
     * @brief Get the self bias count (how often self-injected influenced decision)
     * @return Number of times self-injected influenced the decision
     */
    int getSelfBiasCount() const { return selfBiasCount_; }
    
    /**
     * @brief Get the social bias count (how often social influenced decision)
     * @return Number of times social influenced the decision
     */
    int getSocialBiasCount() const { return socialBiasCount_; }
    
    // Phase 6 wired systems, for demos/tests: concept count and the
    // self-model's mean confidence, plus social observation. A second
    // agent (teacher) is observed through observeOtherAgent; in a
    // single-agent world this stays at zero and social bias stays off.
    /**
     * @brief Get the number of concepts currently stored
     * @return Number of concepts
     */
    size_t getConceptCount() const;
    
    /**
     * @brief Get the self-model's mean confidence
     * @return Mean confidence of the self-model
     */
    float getSelfModelConfidence() const;
    
    /**
     * @brief Get the social observation count
     * @return Number of social observations
     */
    size_t getSocialObservationCount() const;
    
    /**
     * @brief Observe another agent's actions to learn socially
     * @param observed The motor command observed from another agent
     * @param beforeVision Vision data before the observed action
     * @param afterVision Vision data after the observed action
     */
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::            vector<float>& afterVision);
    
    /**
     * @brief Reset the agent for a new episode
     */
    void reset();
    
    /**
     * @brief Get the brain pointer
     * @return Pointer to the underlying brain instance
     */
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    /**
     * @brief Enable or disable reward modulation
     * @param enable Whether to enable reward modulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * @param enable Whether to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * @param enable Whether to enable development
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity
     * @param enable Whether to enable curiosity
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * @return True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * @return True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * @return True if development is enabled
     */
Busqueda bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * @return True if curiosity is enabled
     */
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
