#pragma once

#include "AgentBrain.hpp"
#include "AgentBody.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <sstream>

namespace nlm {

// Enhanced configuration structure with fine-grained control
struct AgentBrainConfig {
    // Neuromodulation parameters
    struct Neuromodulation {
        float rewardLearningRate = 0.01f;        // Learning rate for reward-based updates
        float rewardDecayFactor = 0.95f;         // Exponential decay factor
        float predictionErrorSmoothing = 0.5f;   // Smoothing for prediction error
        float eligibilityTraceDecay = 0.1f;      // Decay rate for eligibility traces
        float dopamineGain = 1.0f;               // Sensitivity to prediction error
        float rewardPredictionErrorGain = 1.0f;   // Scaling factor for prediction error
        float dopamineUptakeRate = 0.1f;         // Rate of dopamine clearance
        float neuromodulationTimeConstant = 1.0f; // Temporal decay factor
        bool homeostaticControl = true;         // Self-regulation of neuromodulators
        bool adaptiveGain = true;                // Whether to adapt gain based on learning progress
    } neuromodulation;
    
    // Plasticity parameters
    struct Plasticity {
        float basePlasticity = 1.0f;              // Base plasticity factor
        float structuralPlasticityRate = 0.0001f; // Rate of structural changes
        float hebbianLearningRate = 0.01f;        // Rate for Hebbian learning
        float stdpLTPWeight = 0.01f;              // LTP weight for STDP
        float stdpLTDWeight = 0.012f;             // LTD weight for STDP
        float stdpTau = 20.0f;                    // Time constant for STDP
        float learningRateLTP = 0.02f;           // Long-term potentiation rate
        float learningRateLTD = 0.015f;          // Long-term depression rate
        float eligibilityTraceTimeConstant = 50.0f; // How long traces persist
        float synapticTaggingThreshold = 0.01f;   // Threshold for tagging synapses
        bool traceReplayEnabled = false;         // Enable replay of eligibility traces
        bool homeostaticPlasticity = true;        // Whether to maintain stability
    } plasticity;
    
    // Curiosity and exploration parameters
    struct Curiosity {
        float curiosityThreshold = 0.3f;          // Threshold for curiosity-driven exploration
        float explorationRate = 0.3f;            // Maximum exploration probability
        float noveltyDecay = 0.99f;               // Decay rate for novelty signals
        float explorationCost = 0.01f;           // Cost of exploration
        float exploitationReward = 1.0f;         // Reward for successful exploitation
        float explorationTemperature = 1.0f;      // Stochasticity in action selection
        float noveltyThresholdVariance = 0.1f;    // Variability in novelty detection
        float intrinsicMotivationBase = 0.1f;     // Baseline drive to explore
        bool goalDirectedExploration = false;    // Exploration toward specific targets
        bool adaptiveExploration = true;          // Whether to adapt exploration based on outcomes
    } curiosity;
    
    // Development parameters
    struct Development {
        double developmentalStages[4] = {60.0, 300.0, 900.0, 1800.0};  // Time thresholds for stages
        float plasticityProgression[4] = {1.0f, 0.8f, 0.5f, 0.2f};     // Plasticity factor per stage
        float synaptogenesisRate = 0.0001f;        // Rate of new synapse formation
        float pruningRate = 0.00001f;             // Rate of synapse elimination
        bool stageTransitions = true;              // Whether to auto-transition stages
        float criticalPeriodLength = 300.0;        // Duration of critical period
    } development;
    
    // Sensory processing parameters
    struct Sensory {
        std::vector<float> visionWeights;          // Weights for vision input channels
        std::vector<float> touchWeights;           // Weights for touch input channels
        std::vector<float> internalWeights;        // Weights for internal signals
        std::vector<float> proprioceptionWeights;  // Weights for proprioception
        bool featureExtraction = false;            // Whether to extract features from raw input
        float noiseLevel = 0.01f;                 // Sensor noise level
        float attentionRadius = 1.0f;             // Radius for attention mechanisms
    } sensory;
    
    // Motor control parameters
    struct Motor {
        std::vector<float> actionCurves;           // Curves for action selection
        float actionSelectionTemperature = 1.0f;   // Temperature for stochastic action selection
        float motorNoise = 0.02f;                 // Noise in motor output
        bool smoothMotion = true;                  // Whether to smooth motor commands
        float interpolationFactor = 0.5f;         // Factor for motion interpolation
        float momentumFactor = 0.9f;              // Factor for velocity persistence
    } motor;
    
    // Performance and monitoring
    struct Performance {
        bool enableMetrics = true;                 // Whether to collect performance metrics
        int metricsInterval = 100;                // How often to record metrics
        double maxProcessingTime = 0.1;           // Maximum allowed processing time
        float energyBudget = 1.0f;                 // Energy consumption budget
        bool adaptiveResources = true;             // Whether to adapt resource usage
    } performance;
    
    // Social learning parameters
    struct Social {
        bool enableSocialLearning = false;         // Whether to enable social learning
        float imitationRate = 0.5f;               // Rate of imitation vs. self-learning
        float observationWindow = 10.0;           // Time window for observations
        float trustDecay = 0.9f;                  // Decay of trust in others
        float socialInfluence = 0.3f;              // Strength of social influence
    } social;
    
    // Constructor with default values
    AgentBrainConfig() = default;
    
    // Load from configuration file
    static AgentBrainConfig fromFile(const std::string& filename);
    
    // Save to configuration file
    bool toFile(const std::string& filename) const;
    
    // Print configuration for debugging
    void print(std::ostream& out = std::cout) const;
};

// Performance metrics structure
struct AgentBrainMetrics {
    // Timing
    double processingTime;                        // Total processing time
    double stepTime;                              // Average time per step
    double brainStepTime;                         // Time spent in brain simulation
    double decodeTime;                            // Time spent in motor decoding
    
    // Biological signals
    float energyConsumption;                       // Energy consumed
    float neuromodulationLevel;                    // Current neuromodulation level
    float curiosityLevel;                         // Current curiosity level
    float noveltyLevel;                           // Current novelty level
    float predictionError;                        // Current prediction error
    
    // Neural activity
    size_t totalSpikes;                           // Total spikes generated
    size_t sensorySpikes;                         // Sensory neuron spikes
    size_t motorSpikes;                           // Motor neuron spikes
    float averageFiringRate;                      // Average firing rate
    float excitationInhibitionRatio;              // E/I balance
    
    // Behavioral metrics
    int actionsTaken;                             // Total actions executed
    float totalReward;                            // Total accumulated reward
    float explorationRate;                        // Rate of exploratory actions
    int successfulActions;                        // Successful actions count
    
    // Memory usage
    size_t memoryFootprint;                       // Memory usage in bytes
    size_t synapseCount;                          // Number of synapses
    size_t neuronCount;                           // Number of neurons
    size_t workingMemoryUsage;                    // Working memory usage
    
    // Development
    double developmentalAge;                      // Current age in simulation steps
    DevelopmentalStage developmentalStage;        // Current developmental stage
    
    // Social metrics (if enabled)
    int observationsMade;                         // Number of observations
    int imitationsPerformed;                      // Number of imitations
    float socialLearningRate;                     // Rate of social learning
    
    // Reset metrics
    void reset();
    
    // Merge with another metrics set
    void merge(const AgentBrainMetrics& other);
    
    // Get formatted string for logging
    std::string format() const;
};

// Agent lifecycle states
enum class AgentState {
    INITIALIZING,
    ACTIVE,
    PAUSED,
    EMERGENCY_STOPPED,
    EVOLVING,
    RESETTING,
    DESTROYING
};

// Advanced AgentBrain with enhanced functionality
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
    ~EnhancedAgentBrain();
    
    // Basic interface (backward compatibility)
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    void reset();
    
    // Core configuration
    void configure(const AgentBrainConfig& config);
    const AgentBrainConfig& getConfig() const;
    void setParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
    std::map<std::string, float> getAllParameters() const;
    
    // Agent lifecycle management
    void create(const std::string& id, const AgentBrainConfig& config = AgentBrainConfig());
    void destroy();
    bool isActive() const;
    std::string getID() const;
    AgentState getState() const;
    void pause();
    void resume();
    void emergencyStop();
    
    // World integration
    void connectToWorld(std::shared_ptr<SimpleWorld> world);
    void disconnectFromWorld();
    bool isConnectedToWorld() const;
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Checkpoint and persistence
    bool save(const std::string& path) const;
    bool load(const std::string& path);
    bool createCheckpoint(const std::string& path) const;
    bool restoreFromCheckpoint(const std::string& path);
    bool exportState(const std::string& path) const;
    bool importState(const std::string& path);
    
    // Advanced neural operations
    void injectCurrentToNeuron(NeuronId neuron, MembranePotential current);
    void injectCurrentToAllNeuronsOfType(NeuronType type, MembranePotential current);
    void stepBrain(SimulationStep step, Timestamp time = 0.0);
    
    // Evolutionary capabilities
    void evolve(const std::vector<float>& geneticCode);
    std::vector<float> getGeneticCode() const;
    void mutate(float mutationRate = 0.01f);
    float calculateFitness() const;
    
    // Social learning
    void observe(const EnhancedAgentBrain& otherAgent);
    void teach(const EnhancedAgentBrain& student);
    void enableSocialLearning(bool enable);
    bool isSocialLearningEnabled() const;
    void setImitationRate(float rate);
    float getImitationRate() const;
    
    // Performance monitoring
    void startPerformanceTimer();
    void stopPerformanceTimer();
    void recordMetrics();
    AgentBrainMetrics getMetrics() const;
    void resetMetrics();
    std::string getMetricsSummary() const;
    
    // Advanced sensory processing
    void setSensoryProcessor(std::unique_ptr<class ISensoryProcessor> processor);
    void setMotorDecoder(std::unique_ptr<class IMotorDecoder> decoder);
    void setNeuromodulator(std::unique_ptr<class INeuromodulator> modulator);
    
    // Multi-agent coordination
    void addFollower(EnhancedAgentBrain* agent);
    void removeFollower(EnhancedAgentBrain* agent);
    void coordinateWith(EnhancedAgentBrain* agent);
    void enableEmergentBehaviors(bool enable);
    bool areEmergentBehaviorsEnabled() const;
    
    // Development control
    void accelerateDevelopment();
    void decelerateDevelopment();
    void setDevelopmentStage(DevelopmentalStage stage);
    void forceDevelopmentalStageTransition();
    void setCriticalPeriodActive(bool active);
    
    // Action planning and sequencing
    void setActionSequence(const std::vector<MotorCommand>& sequence);
    void clearActionSequence();
    bool hasActionSequence() const;
    MotorCommand getNextAction();
    void skipCurrentAction();
    void repeatCurrentAction();
    
    // Debug and visualization
    void enableDebugOutput(bool enable);
    bool isDebugOutputEnabled() const;
    void logState() const;
    std::string getDebugInfo() const;
    
    // Memory management
    void enableMemoryManagement(bool enable);
    bool isMemoryManagementEnabled() const;
    void optimizeMemoryUsage();
    size_t getMemoryUsage() const;
    void garbageCollect();
    
    // Resource management
    void setEnergyBudget(float budget);
    float getEnergyBudget() const;
    float getCurrentEnergy() const;
    bool hasEnergy(float amount) const;
    void consumeEnergy(float amount);
    void regenerateEnergy(float amount);
    
    // Threading and concurrency
    void enableThreading(bool enable);
    bool isThreadingEnabled() const;
    void setThreadPoolSize(size_t size);
    size_t getThreadPoolSize() const;
    void processAsync();
    
    // Signal handling
    void setSignalHandler(void (*handler)(EnhancedAgentBrain*, const std::string&));
    void triggerSignal(const std::string& signal, const std::map<std::string, std::string>& data = {});
    
private:
    // Internal implementation details
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Disable copying, enable moving
    EnhancedAgentBrain(const EnhancedAgentBrain&) = delete;
    EnhancedAgentBrain& operator=(const EnhancedAgentBrain&) = delete;
    EnhancedAgentBrain(EnhancedAgentBrain&&) noexcept;
    EnhancedAgentBrain& operator=(EnhancedAgentBrain&&) noexcept;
};