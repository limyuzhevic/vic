#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

namespace nlm {

// Forward declarations for all neuromodulators
class Neuromodulator;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class DopamineSystem;
class CuriositySystem;
class PredictionErrorSystem;
class NoveltySystem;
class NeuromodulatorNetwork;
class PlasticityController;
class NeuromodulatorDynamics;
class NeuromodulatorSynthesis;
class NeuromodulatorFeedback;

// Enhanced neuromodulation configuration structure
struct NeuromodulationConfig {
    // Multi-layer neuromodulation settings
    bool enableMultiLayerModulation = true;
    float neuromodulatorInteractionStrength = 0.3f;
    float crossSystemCommunicationStrength = 0.2f;
    
    // Dynamic synthesis settings
    bool enableDynamicSynthesis = true;
    float synthesisLearningRate = 0.01f;
    float synthesisNoiseLevel = 0.1f;
    
    // State dynamics settings
    bool enableStateDynamics = true;
    float neuromodulatorTimeConstant = 0.5f;
    float homeostaticTargetLevel = 0.5f;
    
    // Plasticity control settings
    bool enableNeuromodulatorGatedPlasticity = true;
    bool enableNeuromodulatorScaledPlasticity = true;
    bool enableNeuromodulatorTimingDependentPlasticity = true;
    
    // Cross-talk and feedback settings
    bool enableCrossTalk = true;
    bool enableFeedbackLoops = true;
    bool enableFeedforwardNetworks = true;
    
    // Learning parameters
    float dopamineLearningRate = 0.001f;
    float curiosityExplorationStrength = 1.0f;
    float predictionErrorLearningRate = 0.01f;
    
    // Safety and stability parameters
    float safetyLevelThreshold = 1.0f;
    float adaptationRate = 0.05f;
    float optimizationHorizon = 10.0f;
    
    // Multi-omics integration flags
    bool integrateWithMemorySystems = true;
    bool integrateWithPlasticitySystems = true;
    bool integrateWithDevelopmentSystem = true;
    bool integrateWithAgentSystem = true;
    
    // Performance requirements
    float maxComputationTimePerStep = 0.001f;  // 1ms
    float memoryBudget = 100.0f * 1024.0f;  // 100KB
    float cpuBudget = 0.5f;  // 50% CPU
    bool enableRealTimeComputation = true;
};

// Advanced neuromodulator state for complex dynamics
struct NeuromodulatorState {
    float currentLevel;
    float targetLevel;
    float accumulation;
    float decayRate;
    float productionRate;
    float inhibitionRate;
    float lastUpdateTime;
    float homeostasisValue;
    float plasticityInfluence;
    std::vector<float> history;
    
    NeuromodulatorState()
        : currentLevel(0.0f), targetLevel(0.0f), accumulation(0.0f), decayRate(0.1f),
          productionRate(0.0f), inhibitionRate(0.0f), lastUpdateTime(0.0), 
          homeostasisValue(0.0f), plasticityInfluence(1.0f) {}
};

// Cross-talk relationships between neuromodulators
struct NeuromodulatorConnection {
    enum class ConnectionType {
        Excitatory,
        Inhibitory,
        Modulatory,
        Mutualistic,
        Antagonistic
    };
    
    std::string sourceModulator;
    std::string targetModulator;
    ConnectionType type;
    float strength;
    float delay;
    float threshold;
    float saturation;
    
    NeuromodulatorConnection()
        : sourceModulator(""), targetModulator(""), type(ConnectionType::Excitatory),
          strength(1.0f), delay(0.0f), threshold(0.1f), saturation(1.0f) {}
};

// Multi-step prediction error structure for advanced learning
struct MultiStepPrediction {
    float currentPrediction;
    float targetValue;
    std::vector<float> futurePredictions;
    std::vector<float> futureRewards;
    float gamma;  // Discount factor
    float lambda;  // Eligibility trace parameter
    
    MultiStepPrediction()
        : currentPrediction(0.0f), targetValue(0.0f), gamma(0.99f), lambda(0.9f) {}
};

// Curiosity-driven exploration state
struct ExplorationState {
    float intrinsicMotivation;
    float uncertaintyEstimate;
    float informationGain;
    float explorationBonus;
    float exploitationWeight;
    float noveltySensitivity;
    float predictionErrorSensitivity;
    
    ExplorationState()
        : intrinsicMotivation(0.5f), uncertaintyEstimate(0.0f), informationGain(0.0f),
          explorationBonus(0.0f), exploitationWeight(0.5f), noveltySensitivity(1.0f),
          predictionErrorSensitivity(1.0f) {}
};

// Neuromodulation performance metrics
struct NeuromodulationMetrics {
    // Computational metrics
    float computationTime;
    float memoryUsage;
    float cpuUtilization;
    
    // Biological metrics
    float averageNeuromodulatorLevel;
    float homeostasisRatio;
    float learningEfficiency;
    float explorationEfficiency;
    
    // Stability metrics
    float numericalStability;
    float boundedness;
    float convergenceRate;
    
    NeuromodulationMetrics()
        : computationTime(0.0f), memoryUsage(0.0f), cpuUtilization(0.0f),
          averageNeuromodulatorLevel(0.0f), homeostasisRatio(1.0f),
          learningEfficiency(0.0f), explorationEfficiency(0.0f),
          numericalStability(1.0f), boundedness(1.0f), convergenceRate(0.0f) {}
};

// NeuromodulationEngine: Main controller for advanced neuromodulation
class NeuromodulationEngine {
public:
    NeuromodulationEngine(std::shared_ptr<Brain> brain, 
                        const NeuromodulationConfig& config = NeuromodulationConfig());
    ~NeuromodulationEngine();
    
    // Disable copying, enable moving
    NeuromodulationEngine(const NeuromodulationEngine&) = delete;
    NeuromodulationEngine& operator=(const NeuromodulationEngine&) = delete;
    NeuromodulationEngine(NeuromodulationEngine&&) noexcept;
    NeuromodulationEngine& operator=(NeuromodulationEngine&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Main neuromodulation computation step
    void step(TimestepDuration dt, SimulationStep step);
    
    // Neuromodulator system access
    DopamineSystem* getDopamineSystem() { return dopamineSystem_.get(); }
    const DopamineSystem* getDopamineSystem() const { return dopamineSystem_.get(); }
    
    CuriositySystem* getCuriositySystem() { return curiositySystem_.get(); }
    const CuriositySystem* getCuriositySystem() const { return curiositySystem_.get(); }
    
    PredictionErrorSystem* getPredictionErrorSystem() { return predictionErrorSystem_.get(); }
    const PredictionErrorSystem* getPredictionErrorSystem() const { return predictionErrorSystem_.get(); }
    
    NoveltySystem* getNoveltySystem() { return noveltySystem_.get(); }
    const NoveltySystem* getNoveltySystem() const { return noveltySystem_.get(); }
    
    NeuromodulatorNetwork* getNeuromodulatorNetwork() { return neuromodulatorNetwork_.get(); }
    const NeuromodulatorNetwork* getNeuromodulatorNetwork() const { return neuromodulatorNetwork_.get(); }
    
    PlasticityController* getPlasticityController() { return plasticityController_.get(); }
    const PlasticityController* getPlasticityController() const { return plasticityController_.get(); }
    
    NeuromodulatorDynamics* getNeuromodulatorDynamics() { return neuromodulatorDynamics_.get(); }
    const NeuromodulatorDynamics* getNeuromodulatorDynamics() const { return neuromodulatorDynamics_.get(); }
    
    NeuromodulatorSynthesis* getNeuromodulatorSynthesis() { return neuromodulatorSynthesis_.get(); }
    const NeuromodulatorSynthesis* getNeuromodulatorSynthesis() const { return neuromodulatorSynthesis_.get(); }
    
    NeuromodulatorFeedback* getNeuromodulatorFeedback() { return neuromodulatorFeedback_.get(); }
    const NeuromodulatorFeedback* getNeuromodulatorFeedback() const { return neuromodulatorFeedback_.get(); }
    
    // Configuration management
    void setConfig(const NeuromodulationConfig& config);
    const NeuromodulationConfig& getConfig() const { return config_; }
    
    // Neuromodulator state management
    void updateNeuromodulatorState(const std::string& name, const NeuromodulatorState& state);
    NeuromodulatorState getNeuromodulatorState(const std::string& name) const;
    
    // Neuromodulator synthesis
    void synthesizeNewModulator(const std::string& name, float intensity, float duration);
    
    // Neuromodulator cross-talk and interaction
    void establishConnection(const NeuromodulatorConnection& connection);
    void computeCrossTalkEffects();
    
    // Learning system integration
    void processPredictionError(float prediction, float target, float reward);
    void processCuriositySignal(float novelty, float predictionError);
    void processRewardSignal(float reward, float predictedReward);
    
    // Plasticity control
    void applyPlasticityModulation(float synapseWeight, const Neuromodulator& modulator) const;
    float computePlasticityFactor(const std::unordered_set<std::string>& neuromodulators) const;
    
    // Real-time adaptation and optimization
    void adaptToEnvironment(const class EnvironmentState& state);
    void optimizePerformance();
    void maintainHomeostasis();
    
    // Integration with brain systems
    void integrateWithBrainSystems();
    void integrateWithMemorySystems();
    void integrateWithPlasticitySystems();
    void integrateWithDevelopmentSystem();
    void integrateWithAgentSystem();
    
    // Metrics and statistics
    const NeuromodulationMetrics& getMetrics() const { return metrics_; }
    std::vector<std::string> getActiveNeuromodulators() const;
    float getTotalNeuromodulationLevel() const;
    
    // Validation and safety
    bool validateSafetyConstraints() const;
    void enforceSafetyConstraints();
    bool checkNumericalStability() const;
    
    // Reset and cleanup
    void reset();
    void shutdown();
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    NeuromodulationConfig config_;
    NeuromodulationMetrics metrics_;
    
    // Component systems
    std::unique_ptr<DopamineSystem> dopamineSystem_;
    std::unique_ptr<CuriositySystem> curiositySystem_;
    std::unique_ptr<PredictionErrorSystem> predictionErrorSystem_;
    std::unique_ptr<NoveltySystem> noveltySystem_;
    std::unique_ptr<NeuromodulatorNetwork> neuromodulatorNetwork_;
    std::unique_ptr<PlasticityController> plasticityController_;
    std::unique_ptr<NeuromodulatorDynamics> neuromodulatorDynamics_;
    std::unique_ptr<NeuromodulatorSynthesis> neuromodulatorSynthesis_;
    std::unique_ptr<NeuromodulatorFeedback> neuromodulatorFeedback_;
    
    // Neuromodulator collection
    std::map<std::string, std::unique_ptr<Neuromodulator>> neuromodulators_;
    std::map<std::string, NeuromodulatorState> neuromodulatorStates_;
    std::vector<NeuromodulatorConnection> connections_;
    
    // Learning state
    std::vector<MultiStepPrediction> multiStepPredictions_;
    ExplorationState explorationState_;
    
    // Integration flags
    bool brainSystemsIntegrated_;
    bool memorySystemsIntegrated_;
    bool plasticitySystemsIntegrated_;
    bool developmentSystemIntegrated_;
    bool agentSystemIntegrated_;
};

} // namespace nlm