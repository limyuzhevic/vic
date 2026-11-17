#pragma once

/**
 * @file AgentBrain.hpp
 * @brief Agent brain interface connecting NLM brain to world
 * 
 * This file defines the AgentBrain class, which serves as the bridge between
 * the neural brain and the external world. The AgentBrain handles sensory
 * transduction (converting world states to brain inputs) and motor decoding
 * (converting brain activity to world actions). It implements the core
 * perception-action loop that enables the brain to interact with and learn
 * from its environment.
 * 
 * @details The AgentBrain acts as an interface that:
 * - Takes sensory percepts from the world and converts them to brain inputs
 * - Processes sensory information and updates brain state
 * - Decodes brain motor activity into actionable commands
 * - Applies reward modulation based on outcomes
 * - Maintains developmental state and plasticity
 * - Provides diagnostic information about brain activity
 * 
 * The agent brain integrates multiple advanced systems:
 * - **Neuromodulation**: Reward prediction, curiosity, novelty detection
 * - **Cognition**: Planning, concept formation, self-modeling
 * - **Development**: Age-dependent plasticity and maturation
 * - **Social learning**: Observation and imitation of other agents
 * - **Text as a sense**: Encoding teacher words for association learning
 */

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

/**
 * @brief AgentBrain: Connects NLM brain to the world
 * 
 * This class implements the perception-action loop that enables the artificial
 * brain to interact with and learn from its environment. It handles the conversion
 * between world states (sensory inputs) and brain activity, and between brain
 * decisions (motor commands) and world effects.
 * 
 * @note The AgentBrain integrates multiple advanced cognitive and biological
 *       systems including neuromodulation, prediction, planning, concept
 *       formation, development, and social learning. This represents Phase 6
 *       of the NLM project - final integration of all systems into a coherent
 *       artificial brain.
 */
class AgentBrain {
public:
    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the neural brain that this agent interfaces with
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize agent with world
     * 
     * This method prepares the agent for operation with a specific world,
     * setting up initial state and preparing sensory/motor interfaces.
     * 
     * @param world Reference to the world the agent will interact with
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get expected sensory input size
     * 
     * @return Number of sensory values expected by the brain (vision, touch, internal, proprioception)
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get expected motor output size
     * 
     * @return Number of motor neurons expected by the brain
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * This is the core perception step: take sensory information from the
     * world, process it, and inject it into the brain for learning and decision
     * making. This includes novelty detection, prediction, and memory updates.
     * 
     * @param percept Sensory data from the world (vision, touch, text, etc.)
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * This is the core action selection step: analyze brain motor neuron activity,
     * apply various biases (reflexes, learned preferences, curiosity, planning,
     * concepts, self-model, social learning), and produce a motor command that
     * will be applied to the world.
     * 
     * @return Motor command representing the brain's decision to act
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Diagnostic: per motor group, in decode order
     * 
     * Provides detailed information about each motor group's activity,
     * including membrane potential deviation (old metric), firing rate (new metric),
     * and refractory neuron count. The firing rate is now derived from counted
     * spikes rather than membrane potential, because neurons that just spiked
     * reset their membrane potential to resting, making membrane deviation
     * useless for decoding at sampling time.
     */
    struct MotorGroupActivity {
        const char* name;              ///< Motor group name (for diagnostics)
        float activity;               ///< Mean |membrane - resting| (old metric)
        size_t refractoryCount;       ///< Number of neurons in Refractory state (recently spiked)
        size_t size = 0;              ///< Total neurons in this group
        float firingRate = 0.0f;      ///< Mean firing rate in Hz (new metric)
    };
    
    /**
     * @brief Get motor group activities (diagnostic)
     * 
     * Returns activity information for all motor groups in decode order.
     * Useful for understanding which motor patterns the brain is considering.
     * 
     * @return Vector of motor group activity information
     */
    std::vector<MotorGroupActivity> getMotorGroupActivities() const;
    
    /**
     * @brief Apply neuromodulation based on reward
     * 
     * This method implements reward-based learning: updates the brain's
     * learned preferences, action quality predictions, and applies reward
     * modulated plasticity to synapses. It also updates neuromodulatory systems
     * (dopamine, curiosity, etc.).
     * 
     * @param reward Outcome reward (positive for good outcomes, negative for bad)
     * @param predictedReward Expected reward (prediction error = reward - predicted)
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * Advances the brain's developmental stage based on accumulated time,
     * affecting plasticity rates, neuron properties, and system parameters
     * to simulate brain maturation and aging.
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level
     * 
     * @return Current neuromodulation level (typically dopamine level)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * 
     * @return Current curiosity level (0.0 to 1.0, drives exploration)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * 
     * @return Current novelty level (change detection sensitivity)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * @return Current prediction error (reward - predicted reward)
     */
    float getPredictionError() const;
    
    /**
     * @brief Get thought trace (Phase 5)
     * 
     * Returns a human-readable one-line record of the most recent loop turn,
     * including what was perceived, what the planner suggested from learned
     * action quality, what the brain actually decoded, and the outcome reward.
     * The planner suggestion is advisory: the brain's motor decode still decides
     * the action, so a trace where planned != acted is a real, reportable
     * disagreement, not a script.
     * 
     * @return Human-readable thought trace string
     */
    std::string getThoughtTrace() const;
    
    /**
     * @brief Get learned preference trait
     * 
     * @return Learned preference (-1 to +1), stored on Brain so it survives
     *         checkpoint reloads. This represents the agent's learned personality
     *         from experience, not human-like personality.
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Phase 5 diagnostic counters
     * 
     * These counters track how often various mechanisms actually fired,
     * so demo reports can tell a live mechanism from a dead one.
     */
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }
    
    /**
     * @brief Phase 6 text-as-a-sense encoder
     * 
     * Encodes a teacher word into the sparse pattern of text-neuron indices
     * it drives. The string itself is never passed to the brain; only this
     * pattern is injected, so association must be learned by plasticity,
     * not by string matching in the decision logic.
     * 
     * @param text Input text string to encode
     * @return Vector of neuron indices that should be activated
     */
    static std::vector<size_t> encodeText(const std::string& text);
    
    /**
     * @brief Get internal firing rates (for text association recall probe)
     * 
     * Returns the firing rates of internal (hidden) neurons, which are used
     * in the text-association recall probe: a learned association means the
     * internal pattern evoked by text alone moves toward the pattern evoked
     * by the paired visual scene.
     */
    std::vector<float> getInternalFiringRates() const;
    
    /**
     * @brief Phase 6 counters for planner/concept/self/social influence
     */
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }
    
    /**
     * @brief Phase 6 concept count and self-model confidence
     */
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    
    /**
     * @brief Phase 6 social observation count
     */
    size_t getSocialObservationCount() const;
    
    /**
     * @brief Observe another agent's behavior
     * 
     * For social learning: observe a teacher/other agent's action and its
     * effects on the world, allowing the brain to learn from social experience.
     * 
     * @param observed Motor command observed from another agent
     * @param beforeVision Visual scene before the action
     * @param afterVision Visual scene after the action
     */
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision);
    
    /**
     * @brief Reset agent for new episode
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Raw pointer to the underlying brain (use carefully)
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Configuration methods for enabling/disabling subsystems
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Motor decoding: convert neural activity to motor command
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Motor command selection with curiosity/exploration
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;  ///< Underlying neural brain
    
    // Motor neuron groups (Phase 5 organization)
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
    
    // Slowly adapting prediction of the visual scene (for novelty detection)
    std::vector<float> predictedVision_;
    float sensoryNoveltyDecay_;
    
    // Strongest touch/proximity signal in the most recent percept
    float maxTouch_;
    
    // Phase 5 vision-guided reflex features
    float forwardTouch_;
    float forwardObjectVision_;
    
    // Phase 5 winner-take-all state
    int lastWinnerGroup_;
    float lastWinnerMargin_;
    int consecutiveWinnerCount_;
    
    // Phase 5 action-diversity state
    int stepsSinceTurn_;
    int repeatCount_;
    
    // Phase 5 diagnostic counters
    int reflexBonusCount_;
    int reflexSuppressCount_;
    int wtaPulseCount_;
    
    // Last decoded command, for the interact no-repeat rule below
    MotorCommand lastDecoded_;
    bool hasLastDecoded_;
    
    // Phase 6 wired cognition: the brain owns the planner and concept
    // formation, but AgentBrain owns the self-model and social learning
    // (Brain never owned them) and calls them from the loop, so they are
    // no longer library-only classes.
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_;
    bool socialReady_;
    
    // Last percept's touch vector, kept so applyRewardModulation can
    // present the pattern that actually led to a reward to concept
    // formation (pattern -> action -> reward, in that order).
    std::vector<float> lastTouch_;
    std::string lastText_;
    
    // Phase 6 counters for planner/concept/self/social influence
    int plannerOverrideCount_;
    int plannerAgreementCount_;
    int plannerDecisionCount_;
    int conceptBiasCount_;
    int selfBiasCount_;
    int socialBiasCount_;
    
    // Concept pattern for the current percept, in closeness form
    // (1 - vision, 0 = empty) plus touch, for concept bias lookup.
    std::vector<float> conceptPattern() const;
    
    // Phase 5 thought trace: last perceived scene summary, last
    // planner suggestion (from learned action quality), last decoded
    // action, and its outcome reward. Filled across one loop turn.
    std::vector<float> lastVision_;
    std::string thoughtPercept_;
    MotorCommand thoughtPlanned_;
    bool hasThoughtPlanned_;
    float thoughtReward_;
    bool hasThoughtOutcome_;
    
    // Additional Phase 6 state
    MotorCommand lastDecoded_ = MotorCommand::Wait;
    bool hasLastDecoded_ = false;
    float maxTouch_ = 0.0f;
    float forwardTouch_ = 0.0f;
    float forwardObjectVision_ = 1.0f;
    int lastWinnerGroup_ = -1;
    float lastWinnerMargin_ = 0.0f;
    int consecutiveWinnerCount_ = 0;
    int stepsSinceTurn_ = 0;
    int repeatCount_ = 0;
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int wtaPulseCount_ = 0;
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_ = false;
    bool socialReady_ = false;
};

} // namespace nlm
