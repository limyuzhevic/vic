#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Exploration strategy enumeration
enum class ExplorationStrategy {
    None,              // Never explore, always exploit
    Random,            // Pure random exploration
    EpsilonGreedy,     // With probability epsilon, explore randomly
    UCB1,              // Upper Confidence Bound 1
    Boltzmann,         // Temperature-based exploration
    ThompsonSampling   // Bayesian approach
};

// Motor command selection parameters
struct MotorSelectionParams {
    float baseExcitability = 1.0f;        // Base firing threshold adjustment
    float noveltyThreshold = 0.3f;        // Novelty level needed for exploration
    float curiosityWeight = 2.0f;        // How much curiosity affects exploration
    float errorWeight = 0.5f;             // How much prediction error affects exploration
    float temporalDecay = 0.95f;          // How curiosity decays over time
    float habituationRate = 0.99f;        // How curiosity habituates with repeated exposure
};

// Sensory signal processor with configurable scaling
class SensoryProcessor {
public:
    struct SignalConfig {
        float visionScale = 5.0f;          // Scale factor for vision signals
        float touchScale = 8.0f;           // Scale factor for touch signals
        float internalScale = 5.0f;        // Scale factor for internal signals
        float proprioceptionScale = 3.0f; // Scale factor for proprioception
        bool centerZeroSignals = true;      // Center signals around zero
        float minSignal = 0.0f;            // Minimum signal value
        float maxSignal = 1.0f;            // Maximum signal value
    };

    explicit SensoryProcessor(const SignalConfig& config = SignalConfig()) 
        : config_(config) {}

    // Process and inject a vision signal
    void processVisionSignal(float signal, Neuron* neuron) const {
        float processed = processSignal(signal, config_.visionScale);
        if (neuron) neuron->injectCurrent(processed);
    }

    // Process and inject a touch signal
    void processTouchSignal(float signal, Neuron* neuron) const {
        float processed = processSignal(signal, config_.touchScale);
        if (neuron) neuron->injectCurrent(processed);
    }

    // Process and inject an internal signal
    void processInternalSignal(float signal, Neuron* neuron) const {
        float processed = processSignal(signal, config_.internalScale, true);
        if (neuron) neuron->injectCurrent(processed);
    }

    // Process and inject a proprioception signal
    void processProprioceptionSignal(float signal, Neuron* neuron) const {
        float processed = processSignal(signal, config_.proprioceptionScale, true);
        if (neuron) neuron->injectCurrent(processed);
    }

    const SignalConfig& getConfig() const { return config_; }
    void setConfig(const SignalConfig& config) { config_ = config; }

private:
    float processSignal(float signal, float scale, bool center = false) const {
        float processed = signal * scale;
        if (center) {
            processed = (signal * 2.0f - 1.0f) * scale;
        }
        return std::clamp(processed, config_.minSignal, config_.maxSignal);
    }

    SignalConfig config_;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with improved modularity
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
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void setExplorationStrategy(ExplorationStrategy strategy) { explorationStrategy_ = strategy; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    ExplorationStrategy getExplorationStrategy() const { return explorationStrategy_; }
    
    // Advanced exploration control
    void setExplorationParameters(const MotorSelectionParams& params) { motorSelectionParams_ = params; }
    void setCuriosityMax(float max) { curiosityMax_ = max; }
    void setCuriosityDecay(float decay) { curiosityDecay_ = decay; }
    float getCuriosityMax() const { return curiosityMax_; }
    float getCuriosityDecay() const { return curiosityDecay_; }
    
    // Sensory processing configuration
    void setSensoryProcessor(const SensoryProcessor& processor) { sensoryProcessor_ = processor; }
    const SensoryProcessor& getSensoryProcessor() const { return sensoryProcessor_; }
    
    // Curiosity management
    void triggerExplorationEvent();  // External event to trigger exploration
    void resetCuriosity();           // Reset curiosity for new episode
    
    // Statistics and debugging
    size_t getVisionNeuronCount() const { return sensoryVision_.size(); }
    size_t getTouchNeuronCount() const { return sensoryTouch_.size(); }
    size_t getInternalNeuronCount() const { return sensoryInternal_.size(); }
    size_t getProprioceptionNeuronCount() const { return sensoryProprioception_.size(); }
    size_t getMotorNeuronCount() const { return motorForward_.size() + motorBackward_.size() + 
                                            motorTurnLeft_.size() + motorTurnRight_.size() + 
                                            motorInteract_.size() + motorWait_.size(); }

private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Exploration strategies implementation
    MotorCommand exploreRandom();
    MotorCommand exploreUCB1(const std::vector<MotorCommand>& commands, const std::vector<float>& activities);
    MotorCommand exploreBoltzmann(const std::vector<MotorCommand>& commands, const std::vector<float>& activities, float temperature);
    
    // Neuromodulation and learning
    void updateNeuromodulationState();
    void updateCuriosityState(double timestep);
    
    // Neuron group management
    void initializeNeuronGroups();
    void setupMotorNeuronGroups();
    void setupSensoryNeuronGroups();
    
    // Statistical tracking for exploration
    struct ActionStatistics {
        std::vector<float> actionValues;     // Estimated value of each action
        std::vector<float> actionCounts;    // Number of times each action was taken
        std::vector<float> actionRewards;   // Total reward received for each action
        float explorationBonus;              // Current exploration bonus
        
        ActionStatistics(size_t numActions) 
            : actionValues(numActions, 0.0f), actionCounts(numActions, 0.0f), 
              actionRewards(numActions, 0.0f), explorationBonus(0.0f) {}
    };
    
    std::unique_ptr<ActionStatistics> actionStats_;

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
    
    // Advanced exploration configuration
    ExplorationStrategy explorationStrategy_;
    MotorSelectionParams motorSelectionParams_;
    float curiosityMax_;
    float curiosityDecay_;
    std::vector<float> actionExplorationCounts_;
    std::vector<double> actionExplorationTimestamps_;
    
    // Sensory processing
    SensoryProcessor sensoryProcessor_;
    
    // Event tracking
    bool explorationEventTriggered_;
    double lastExplorationEventTime_;
    
    // Statistics
    size_t stepsWithoutExploration_;
    float totalExplorationBonus_;
};

} // namespace nlm