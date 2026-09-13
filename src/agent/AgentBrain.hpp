#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <deque>

namespace nlm {

// Forward declarations for brain memory systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class AttentionalSelection;

// Advanced feature flags enum
enum class AdvancedFeature {
    TD_Curiosity,
    InformationGain,
    MemoryReplay,
    MetaLearning,
    AdvancedNeuromodulation
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// This class bridges the neural network brain with the physical world,
// processing sensory inputs and generating motor commands through
// neuromodulated neural activity.
// Enhanced with advanced curiosity and memory features
class AgentBrain {
public:
    /**
     * @brief Constructor for AgentBrain
     * @param brain Shared pointer to the neural network brain that will control the agent
     * Creates neural connections between brain and motor neurons for command generation
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor for AgentBrain
     * Cleans up neural connections and releases shared resources
     */
    ~AgentBrain();
    
    // ========== ADVANCED FEATURES ==========
    
    /**
     * @brief Enable/disable Temporal Difference curiosity
     * @param enable Whether to enable TD curiosity-based exploration
     */
    void enableTDCuriosity(bool enable) { tdCuriosityEnabled_ = enable; }
    
    /**
     * @brief Enable/disable Information Gain computation
     * @param enable Whether to enable information gain-based exploration
     */
    void enableInformationGain(bool enable) { informationGainEnabled_ = enable; }
    
    /**
     * @brief Enable/disable enhanced memory replay
     * @param enable Whether to enable reinforcement-based memory replay
     */
    void enableMemoryReplay(bool enable) { memoryReplayEnabled_ = enable; }
    
    /**
     * @brief Enable/disable meta-learning capabilities
     * @param enable Whether to enable plasticity modulation and developmental learning rates
     */
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    
    /**
     * @brief Enable/disable advanced neuromodulation
     * @param enable Whether to enable integrated reward prediction error and homeostatic regulation
     */
    void enableAdvancedNeuromodulation(bool enable) { advancedNeuromodulationEnabled_ = enable; }
    
    /**
     * @brief Set advanced feature flags
     * @param tdCuriosity Enable Temporal Difference curiosity
     * @param infoGain Enable information gain computation
     * @param memoryReplay Enable enhanced memory replay
     * @param metaLearning Enable meta-learning capabilities
     * @param neuromodulation Enable advanced neuromodulation
     */
    void setAdvancedFeatureFlags(bool tdCuriosity, bool infoGain, bool memoryReplay,
                                 bool metaLearning, bool neuromodulation);
    
    /**
     * @brief Check if an advanced feature is enabled
     * @param feature The feature to check
     * @return true if the feature is enabled, false otherwise
     */
    bool getAdvancedFeatureFlag(AdvancedFeature feature) const;
    
    // ===== ADVANCED CURIOUSY SYSTEM =====
    
    /**
     * @brief Compute Temporal Difference curiosity
     * @param currentState Current sensory state
     * @param nextState Predicted next state
     * @param reward Received reward
     * @param dt Time step duration
     */
    void computeTDCuriosity(const std::vector<float>& currentState,
                           const std::vector<float>& nextState,
                           float reward, float dt);
    
    /**
     * @brief Apply curiosity decay and recovery mechanisms
     * @param dt Time step duration
     */
    void applyCuriosityDecay(double dt);
    
    /**
     * @brief Compute information gain from memory systems
     */
    void computeInformationGain();
    
    /**
     * @brief Calculate memory coherence
     * @param episodicMemory Episodic memory system
     * @return Coherence measure (0.0-1.0)
     */
    float calculateMemoryCoherence(NeuralEpisodicMemory* episodicMemory) const;
    
    /**
     * @brief Apply exploration bonus based on memory uncertainty
     */
    void applyExplorationBonus();
    
    // ===== ENHANCED MEMORY REPLAY =====
    
    /**
     * @brief Calculate memory coherence for replay prioritization
     * @param episode Episode to evaluate
     * @return Priority score for replay
     */
    float calculateReplayPriority(const EpisodicMemoryItem& episode) const;
    
    /**
     * @brief Enhanced memory replay with reinforcement prioritization
     */
    void enhancedMemoryReplay();
    
    /**
     * @brief Sleep-inspired memory consolidation
     */
    void calculateSleepInspiredConsolidation();
    
    /**
     * @brief Replay during action gaps (idle periods)
     * @param lastAction Last motor command executed
     */
    void replayDuringActionGaps(const MotorCommand& lastAction);
    
    /**
     * @brief Experience replay for pattern completion
     */
    void experienceReplayForPatternCompletion();
    
    /**
     * @brief Replay a specific episode to consolidate learning
     * @param episode Episode to replay
     */
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    /**
     * @brief Calculate episode strength for sleep consolidation
     * @param episode Episode to evaluate
     * @return Strength measure (0.0-1.0)
     */
    float calculateEpisodeStrength(const EpisodicMemoryItem& episode) const;
    
    /**
     * @brief Find similar episodes for pattern completion
     * @param pattern Sensory pattern to match
     * @param maxCount Maximum number of similar episodes to return
     * @return Vector of similar episodes
     */
    std::vector<EpisodicMemoryItem> findSimilarEpisodes(const std::vector<float>& pattern, size_t maxCount) const;
    
    /**
     * @brief Get all episodes from memory systems
     * @return Vector of all episodes
     */
    std::vector<EpisodicMemoryItem> getAllEpisodes() const;
    
    // ===== META-LEARNING FEATURES =====
    
    /**
     * @brief Update meta-learning plasticity modulation
     * @param learningSuccess Success metric from learning (0.0-1.0)
     */
    void updateMetaLearningPlastictyModulation(float learningSuccess);
    
    /**
     * @brief Update developmental learning rate
     */
    void updateDevelopmentalLearningRate();
    
    /**
     * @brief Enable transfer learning between memory systems
     */
    void enableTransferLearning();
    
    /**
     * @brief Calculate pattern strength from an episode
     * @param episode Episode to evaluate
     * @return Pattern strength (0.0-1.0)
     */
    float calculatePatternStrength(const EpisodicMemoryItem& episode) const;
    
    // ===== ADVANCED NEUROMODULATION =====
    
    /**
     * @brief Compute advanced neuromodulation integrating reward prediction error
     * @param reward Received reward
     * @param predictedReward Predicted reward
     */
    void computeAdvancedNeuromodulation(float reward, float predictedReward);
    
    /**
     * @brief Homeostatic regulation of neuromodulators
     * @param rpe Reward prediction error
     */
    void homeostaticRegulation(float rpe);
    
    /**
     * @brief Context-dependent neuromodulation
     * @param rpe Reward prediction error
     */
    void contextDependentModulation(float rpe);
    
    /**
     * @brief Calculate current uncertainty based on prediction confidence
     * @return Uncertainty measure (0.0-1.0)
     */
    float calculateCurrentUncertainty() const;
    
    /**
     * @brief Check if we're in an action gap
     * @param lastAction Last motor command
     * @return true if in action gap, false otherwise
     */
    bool isActionGap(const MotorCommand& lastAction) const;
    
    /**
     * @brief Complete pattern in working memory using an episode
     * @param episode Episode containing pattern
     * @param workingMemory Working memory system
     */
    void completePattern(const EpisodicMemoryItem& episode, NeuralWorkingMemory* workingMemory);
    
    /**
     * @brief Calculate learning success from an episode
     * @param episode Episode to evaluate
     * @return Learning success metric (0.0-1.0)
     */
    float calculateLearningSuccess(const EpisodicMemoryItem* episode) const;
    
    /**
     * @brief Calculate pattern strength
     * @param episode Episode to evaluate
     * @return Pattern strength (0.0-1.0)
     */
    float calculatePatternStrength(const EpisodicMemoryItem& episode) const;
    
    /**
     * @brief Inject current into a specific neuron
     * @param neuron Neuron ID
     * @param current Current value to inject
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Get current sensory pattern
     * @return Current sensory pattern vector
     */
    std::vector<float> getCurrentSensoryPattern() const;
    
    /**
     * @brief Reset all advanced feature state
     */
    void resetAdvancedFeatures();
    
    /**
     * @brief Update all advanced features
     * @param dt Time step duration
     */
    void updateAdvancedFeatures(double dt);
    
    /**
     * @brief Integrate all advanced curiosity and memory systems
     */
    void integrateAdvancedCuriosityAndMemorySystems();
    
    // Initialize with world
     // Establishes initial sensory processing pathways and resets state variables
     // Sets up neuromodulation baseline and developmental state
     void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
     // Returns the dimensionality of sensory patterns that the brain expects as input
     // This determines the size of the concatenated sensory vector
     size_t getSensoryInputSize() const;
    
    // Get motor output size expected
     // Returns the dimensionality of motor commands that the brain will produce
     // This determines the number of independent motor control signals
     size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
     // Converts external sensory signals into neural activity patterns
     // Applies preprocessing and normalization based on sensory modality
     // Injects processed signals into brain for prediction
     void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
     // Converts neural population activity into actionable motor commands
     // Selects strongest motor neuron patterns and transforms to motor control signals
     MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
     // Modifies neural gain and plasticity based on received reward signals
     // Dopamine-level reinforcement learning modulation
     // @param reward Received environmental reward (typically 0.0-1.0)
     // @param predictedReward Predicted reward based on current state (for prediction error)
     void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
     // Advances the developmental stage based on internal age and environmental inputs
     // Applies plasticity modifiers and developmental constraints
     // @param timestep Time elapsed since last update in milliseconds
     void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage enum value
     * Represents the agent's maturation state (e.g., juvenile, adult, senescent)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level
     * @return Current neuromodulation level (typically 0.0-1.0)
     * Represents the gain modulation of neural circuits by dopamine
     */
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
     // Represents the agent's drive for novel exploration and learning
     // Higher values indicate stronger exploratory behavior (typically 0.0-1.0)
     float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * @return Current novelty signal (typically 0.0-1.0)
     * Measures unexpectedness of current sensory input relative to memory
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * @return Prediction error signal from the brain (typically -1.0 to 1.0)
     * Difference between expected and actual sensory input
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * Resets internal state including developmental age, neuromodulation levels,
     * and previous sensory memories for fresh learning
     */
    void reset();
    
    // Get brain pointer
     /**
     * @brief Get brain pointer
     * @return Pointer to the underlying Brain object
     * Provides direct access to the neural network controller for advanced operations
     */
     Brain* getBrain() { return brain_.get(); }
     
     /**
     * @brief Integrate sensory input with brain systems
     * @param sensoryPattern Concatenated sensory vector (size must match getSensoryInputSize())
     * Injects processed sensory data into multiple brain subsystems
     */
     void integrateSensoryInput(const std::vector<float>& sensoryPattern);
     
     /**
     * @brief Integrate curiosity-driven learning signals
     * Activates exploration and novelty-based learning pathways in the brain
     */
     void integrateCuriosityDrivenLearning();
     
     /**
     * @brief Integrate memory replay signals
     * Replays recent experiences to strengthen learning and consolidate memory
     */
     void integrateMemoryReplay();
     
     /**
     * @brief Enable/disable reward modulation
     * @param enable Whether to enable reward-based neuromodulation
     * When enabled, reward signals modulate neural gain and plasticity
     */
     void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
     
     /**
     * @brief Enable/disable structural plasticity
     * @param enable Whether to enable structural changes in neural connections
     * When enabled, rewards can create or prune synaptic connections
     */
     void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
     
     /**
     * @brief Enable/disable development
     * @param enable Whether to enable developmental progression
     * When enabled, the agent can mature and adapt its architecture
     */
     void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
     
     /**
     * @brief Enable/disable curiosity-driven exploration
     * @param enable Whether to enable novelty-seeking behavior
     * When enabled, the agent actively seeks novel sensory experiences
     */
     void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
     
     /**
     * @brief Check if reward modulation is enabled
     * @return true if reward modulation is enabled, false otherwise
     */
     bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
     
     /**
     * @brief Check if structural plasticity is enabled
     * @return true if structural plasticity is enabled, false otherwise
     */
     bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
     
     /**
     * @brief Check if development is enabled
     * @return true if development is enabled, false otherwise
     */
     bool isDevelopmentEnabled() const { return developmentEnabled_; }
     
     /**
     * @brief Check if curiosity is enabled
     * @return true if curiosity-driven exploration is enabled, false otherwise
     */
     bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<std::shared_ptr<Neuron>> motorForward_;
    std::vector<std::shared_ptr<Neuron>> motorBackward_;
    std::vector<std::shared_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::shared_ptr<Neuron>> motorTurnRight_;
    std::vector<std::shared_ptr<Neuron>> motorInteract_;
    std::vector<std::shared_ptr<Neuron>> motorWait_;
    
    // Sensory neuron groups
    std::vector<std::shared_ptr<Neuron>> sensoryVision_;
    std::vector<std::shared_ptr<Neuron>> sensoryTouch_;
    std::vector<std::shared_ptr<Neuron>> sensoryInternal_;
    std::vector<std::shared_ptr<Neuron>> sensoryProprioception_;
    
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced feature flags
    bool tdCuriosityEnabled_;
    bool informationGainEnabled_;
    bool memoryReplayEnabled_;
    bool metaLearningEnabled_;
    bool advancedNeuromodulationEnabled_;
    
    // Advanced feature state
    float tdCuriosityLevel_;
    float informationGain_;
    float developmentalLearningRate_;
    std::deque<float> tdCuriosityHistory_;
    std::deque<float> informationGainHistory_;
    std::deque<float> plasticityHistory_;
    std::vector<std::pair<float, const EpisodicMemoryItem*>> replayPriorities_;
    
    // Constants for advanced features
    static constexpr size_t maxTDHistorySize_ = 100;
    static constexpr size_t maxInfoGainHistorySize_ = 100;
    static constexpr size_t maxPlasticityHistorySize_ = 50;
};

} // namespace nlm
