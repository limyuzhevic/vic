#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // ========== ADVANCED NEURAL NETWORK CONFIGURATION ==========
    
    // Custom neuron type definitions
    struct CustomNeuronConfig {
        NeuronType type;
        float threshold;
        float resetPotential;
        float restingPotential;
        float membraneTimeConstant;
        float capacitance;
        float excitability;
        float plasticity;
        float adaptation;
        std::string metadata;
        
        CustomNeuronConfig()
            : type(NeuronType::Internal), threshold(-50.0f), resetPotential(-70.0f)
            , restingPotential(-65.0f), membraneTimeConstant(20.0f), capacitance(1.0f)
            , excitability(1.0f), plasticity(0.5f), adaptation(0.0f) {}
    };
    
    // Connection pattern types for advanced network topology
    enum class ConnectionPattern {
        Dense,           // All-to-all connectivity
        SparseRandom,    // Random connections with probability
        SparseDeterministic,  // Deterministic sparse pattern
        SmallWorld,      // Small-world topology
        ScaleFree,       // Scale-free network
        Custom          // User-defined connection rules
    };
    
    // Custom connection rule definition
    struct ConnectionRule {
        float probability;
        float weightMin;
        float weightMax;
        float delayMin;
        float delayMax;
        std::function<bool(NeuronId, NeuronId)> connectivityPredicate;
        bool weightPlasticity;
        bool structuralPlasticity;
        std::string ruleName;
        
        ConnectionRule()
            : probability(0.1f), weightMin(0.1f), weightMax(1.0f)
            , delayMin(1), delayMax(10), weightPlasticity(true)
            , structuralPlasticity(false), ruleName("DefaultRule") {}
    };
    
    // Neural architecture specification for complex networks
    struct NeuralArchitecture {
        std::vector<CustomNeuronConfig> neuronConfigs;
        std::vector<std::vector<ConnectionRule>> regionConnectionRules;
        ConnectionPattern globalConnectionPattern;
        std::unordered_map<std::string, float> networkParameters;
        bool enableLayeredTopology;
        bool enableRecurrentConnectivity;
        bool enableModularity;
        
        NeuralArchitecture() : globalConnectionPattern(ConnectionPattern::SparseRandom)
            , enableLayeredTopology(false), enableRecurrentConnectivity(true)
            , enableModularity(true) {}
    };
    
    // Neural network configuration parameters
    struct NetworkConfig {
        // Architecture specification
        NeuralArchitecture architecture;
        
        // Optimization parameters
        float learningRate;
        float momentum;
        float decayRate;
        float targetError;
        size_t maxIterations;
        
        // Network topology parameters
        float connectionProbability;
        bool addRecurrentConnections;
        bool addSkipConnections;
        float skipConnectionStrength;
        
        // Performance parameters
        bool enableParallelProcessing;
        size_t maxMemoryUsage;
        float convergenceThreshold;
        
        NetworkConfig()
            : learningRate(0.01f), momentum(0.9f), decayRate(0.0001f)
            , targetError(0.01f), maxIterations(1000), connectionProbability(0.15f)
            , addRecurrentConnections(true), addSkipConnections(false)
            , skipConnectionStrength(0.5f), enableParallelProcessing(true)
            , maxMemoryUsage(1024 * 1024 * 1024), convergenceThreshold(1e-6) {}
    };
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== ADVANCED NEURAL NETWORK CONFIGURATION ==========
    
    // Set custom neural architecture
    bool setNeuralArchitecture(const NeuralArchitecture& arch);
    
    // Configure complex connection patterns
    bool configureConnectionPattern(ConnectionPattern pattern,
                                   const std::vector<ConnectionRule>& rules = {});
    
    // Add custom connection rules
    void addConnectionRule(RegionId sourceRegion, RegionId targetRegion,
                          const ConnectionRule& rule);
    
    // Get current neural architecture
    const NeuralArchitecture& getNeuralArchitecture() const;
    
    // Get network statistics
    struct NetworkStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float excitationInhibitionRatio;
        float averageFiringRate;
        float networkActivityLevel;
        std::unordered_map<std::string, float> customMetrics;
    };
    
    NetworkStats getNetworkStats() const;
    
    // Neural parameter optimization
    struct OptimizationResult {
        bool success;
        float finalError;
        size_t iterations;
        std::vector<float> optimizedParameters;
        std::string optimizationMethod;
    };
    
    OptimizationResult optimizeNetworkParameters(const std::vector<float>& initialParams,
                                                const std::function<float(const std::vector<float>&)>& lossFunction);
    
    // Neural architecture search helpers
    struct ArchitectureCandidate {
        NeuralArchitecture architecture;
        float expectedPerformance;
        float complexity;
        float resourceUsage;
    };
    
    std::vector<ArchitectureCandidate> suggestArchitectures(const NetworkConfig& constraints,
                                                         size_t maxCandidates = 10);
    
    // Get specific neuron configuration
    CustomNeuronConfig getNeuronConfig(NeuronId neuronId) const;
    
    // Set specific neuron configuration
    void setNeuronConfig(NeuronId neuronId, const CustomNeuronConfig& config);
    
    // ========== ADVANCED NEUROMODULATION ==========
    
    // Multi-factor neuromodulator system
    class MultiFactorNeuromodulator {
    public:
        MultiFactorNeuromodulator();
        ~MultiFactorNeuromodulator();
        
        // Add a neuromodulator factor
        void addFactor(const std::string& name, Neuromodulator* factor, float weight);
        
        // Get combined neuromodulatory effect
        float getCombinedEffect() const;
        
        // Get individual factor effects
        std::unordered_map<std::string, float> getIndividualEffects() const;
        
        // Update all factors
        void update(TimestepDuration dt);
        
        // Configure interaction rules between factors
        void addInteractionRule(const std::string& factor1, const std::string& factor2,
                              const std::function<float(float, float)>& interactionFunc);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Stress and arousal modulation system
    class StressArousalSystem {
    public:
        enum class StressLevel {
            Relaxed,    // Low stress, high focus
            Moderate,   // Balanced state
            Stressed,   // High stress, reduced flexibility
            Overwhelmed // Extreme stress, impaired function
        };
        
        StressArousalSystem();
        ~StressArousalSystem();
        
        // Update stress level based on simulation events
        void update(float rewardPredictionError, float novelty, float memoryLoad);
        
        // Get current stress level
        StressLevel getStressLevel() const;
        
        // Get stress intensity (0-1)
        float getStressIntensity() const;
        
        // Apply stress effects to brain dynamics
        void applyStressEffects(Brain* brain, TimestepDuration dt);
        
        // Configure stress response parameters
        void setStressThreshold(float threshold);
        void setRecoveryRate(float rate);
        void setAdaptationTime(float time);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Advanced neuromodulation interface
    bool configureAdvancedNeuromodulation(const std::string& configFile);
    bool configureAdvancedNeuromodulation(const std::unordered_map<std::string, float>& parameters);
    
    // Get neuromodulation statistics
    struct NeuromodulationStats {
        float dopamineLevel;
        float acetylcholineLevel;
        float norepinephrineLevel;
        float serotoninLevel;
        float stressLevel;
        float arousalLevel;
        float plasticityFactor;
        float attentionLevel;
        std::unordered_map<std::string, float> customModulators;
    };
    
    NeuromodulationStats getNeuromodulationStats() const;
    
    // Set neuromodulator interaction rules
    void addNeuromodulatorInteraction(const std::string& modulator1,
                                    const std::string& modulator2,
                                    const std::function<float(float, float)>& interactionRule);
    
    // ========== ADVANCED MEMORY SYSTEMS ==========
    
    // Hierarchical memory organization
    class MemoryHierarchy {
    public:
        enum class MemoryLevel {
            Sensory,     // Short-term sensory memory
            Working,     // Active working memory
            Episodic,    // Episodic memory (events)
            Semantic,    // Semantic memory (facts)
            Procedural,  // Procedural memory (skills)
            Meta        // Meta-memory (memory about memory)
        };
        
        MemoryHierarchy();
        ~MemoryHierarchy();
        
        // Store memory with level and importance
        void storeMemory(MemoryLevel level, const std::vector<float>& pattern,
                        float importance, float decayRate, const std::string& metadata = "");
        
        // Retrieve memories at or above a certain level
        std::vector<std::vector<float>> retrieveMemories(MemoryLevel minLevel,
                                                        const std::vector<float>& query = {}) const;
        
        // Consolidate memories from lower to higher levels
        void consolidateMemories(MemoryLevel sourceLevel, MemoryLevel targetLevel,
                               float consolidationStrength);
        
        // Get memory statistics
        struct MemoryStats {
            size_t memoryUsage;
            size_t capacity;
            float retrievalSuccessRate;
            std::unordered_map<MemoryLevel, size_t> memoryCountPerLevel;
            std::unordered_map<std::string, float> customMetrics;
        };
        
        MemoryStats getMemoryStats() const;
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Advanced memory capacity management
    bool configureMemoryCapacityManagement(size_t workingMemoryCapacity,
                                          size_t episodicMemoryCapacity,
                                          float consolidationThreshold,
                                          size_t replayFrequency);
    
    // Get memory capacity statistics
    struct MemoryCapacityStats {
        size_t currentWorkingMemoryUsage;
        size_t maxWorkingMemoryCapacity;
        size_t currentEpisodicMemoryUsage;
        size_t maxEpisodicMemoryCapacity;
        size_t currentAssociativeMemoryUsage;
        size_t maxAssociativeMemoryCapacity;
        float consolidationProgress;
        size_t replayInterval;
    };
    
    MemoryCapacityStats getMemoryCapacityStats() const;
    
    // Force memory consolidation
    void triggerMemoryConsolidation(float threshold);
    
    // Get recent memory replay events
    std::vector<SimulationStep> getRecentReplayEvents(size_t limit = 10) const;
    
    // ========== ADVANCED COGNITION ==========
    
    // Custom planning algorithms
    class CustomPlanner {
    public:
        enum class PlanningAlgorithm {
            BFS,             // Breadth-first search
            DFS,             // Depth-first search
            AStar,           // A* search
            MonteCarlo,      // Monte Carlo tree search
            Evolutionary,    // Evolutionary algorithm
            Reinforcement,   // Actor-critic reinforcement learning
            Hierarchical     // Hierarchical planning
        };
        
        CustomPlanner();
        ~CustomPlanner();
        
        // Set planning algorithm
        void setAlgorithm(PlanningAlgorithm algorithm);
        
        // Configure planning parameters
        void setMaxSteps(size_t maxSteps);
        void setPlanningHorizon(float horizon);
        void setExplorationRate(float rate);
        void setExploitationRate(float rate);
        
        // Plan with custom algorithm
        ActionType plan(const std::vector<float>& currentState,
                       const std::vector<float>& goal,
                       const std::vector<ActionType>& actionSpace);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Advanced concept formation
    class AdvancedConceptFormation {
    public:
        AdvancedConceptFormation();
        ~AdvancedConceptFormation();
        
        // Form concepts from neural patterns
        std::vector<std::string> formConcepts(const std::vector<float>& neuralPattern,
                                             float threshold = 0.7f);
        
        // Get concept relationships
        struct ConceptRelationship {
            std::string concept1;
            std::string concept2;
            float strength;
            std::string relationshipType; // "causal", "correlational", "inferred"
        };
        
        std::vector<ConceptRelationship> getConceptRelationships(const std::string& concept) const;
        
        // Update concepts based on new experiences
        void updateConcepts(const std::vector<float>& neuralPattern,
                           const std::vector<std::string>& observedConcepts);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Social learning mechanisms
    class SocialLearningEngine {
    public:
        SocialLearningEngine();
        ~SocialLearningEngine();
        
        // Observe another agent's behavior
        void observeAgentAction(const std::vector<float>& observerState,
                              const std::vector<ActionType>& actions,
                              const std::vector<std::vector<float>>& outcomes,
                              float reward);
        
        // Learn by imitation
        ActionType learnByImitation(const std::vector<float>& currentState) const;
        
        // Can imitate this action?
        bool canImitate(ActionType action) const;
        
        // Get imitation confidence
        float getImitationConfidence(ActionType action) const;
        
        // Learn communication signals
        void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                    const std::vector<float>& meaning);
        
        // Detect signals in neural patterns
        bool detectSignal(const std::vector<float>& pattern) const;
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Intuitive physics reasoning
    class IntuitivePhysicsReasoner {
    public:
        struct PhysicsObject {
            std::vector<float> position;
            std::vector<float> velocity;
            std::vector<float> acceleration;
            float mass;
            std::string objectType;
            std::vector<std::string> properties;
        };
        
        IntuitivePhysicsReasoner();
        ~IntuitivePhysicsReasoner();
        
        // Predict future state based on current physics
        std::vector<float> predictPhysicsState(const std::vector<PhysicsObject>& objects,
                                             float timeStep) const;
        
        // Infer object properties from observed motion
        std::unordered_map<std::string, float> inferObjectProperties(
            const std::vector<float>& trajectory) const;
        
        // Determine if action is physically possible
        bool isPhysicallyPossible(const std::vector<float>& action,
                                const std::vector<PhysicsObject>& environment) const;
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure advanced cognitive modules
    void configureAdvancedPlanning(CustomPlanner::PlanningAlgorithm algorithm);
    void configureConceptFormation(bool enableHierarchicalConcepts,
                                   float conceptMergingThreshold = 0.8f);
    void configureSocialLearning(bool enableImitation,
                                bool enableCommunication,
                                float observationStrength = 0.5f);
    void configurePhysicsReasoning(bool enableContinuousTime,
                                  bool enableCollisionDetection,
                                  float predictionHorizon = 1.0f);
    
    // Get cognitive statistics
    struct CognitiveStats {
        float planningSuccessRate;
        size_t conceptsFormed;
        float socialLearningEffectiveness;
        float physicsReasoningAccuracy;
        std::unordered_map<std::string, float> customCognitiveMetrics;
    };
    
    CognitiveStats getCognitiveStats() const;
    
    // ========== ADVANCED DEVELOPMENT ==========
    
    // Custom developmental trajectories
    class DevelopmentalTrajectory {
    public:
        enum class TrajectoryType {
            Linear,      // Linear development
            Exponential, // Exponential growth
            Sigmoid,     // Sigmoid growth (critical period)
            Gaussian,    // Gaussian growth
            Custom      // User-defined trajectory
        };
        
        DevelopmentalTrajectory();
        ~DevelopmentalTrajectory();
        
        // Set trajectory type
        void setTrajectoryType(TrajectoryType type);
        
        // Configure trajectory parameters
        void setGrowthRate(float rate);
        void setPeakTime(float time);
        void setSaturationLevel(float level);
        
        // Calculate developmental state at given time
        float calculateDevelopmentalState(double age) const;
        
        // Set custom trajectory function
        void setCustomTrajectory(const std::function<float(double)>& trajectoryFunc);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Phase transition control
    class PhaseTransitionController {
    public:
        enum class TransitionType {
            Gradual,  // Smooth transition
            Abrupt,   // Sudden transition
            Oscillating,  // Periodic transitions
            Critical  // Critical period transition
        };
        
        PhaseTransitionController();
        ~PhaseTransitionController();
        
        // Initiate phase transition
        void initiateTransition(DevelopmentalStage targetStage,
                               TransitionType transitionType,
                               float transitionDuration = 10.0f);
        
        // Get current transition progress
        float getTransitionProgress() const;
        
        // Is transition in progress?
        bool isTransitionActive() const;
        
        // Configure transition parameters
        void setTransitionThreshold(float threshold);
        void setTransitionSensitivity(float sensitivity);
        void setTransitionMemory(float memory);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Epigenetic factors
    class EpigeneticSystem {
    public:
        struct EpigeneticFactor {
            std::string name;
            float baselineValue;
            float currentValue;
            float mutationRate;
            float heritability;
            std::string inheritancePattern;
            std::unordered_map<std::string, float> environmentalDependencies;
        };
        
        EpigeneticSystem();
        ~EpigeneticSystem();
        
        // Add epigenetic factor
        void addFactor(const EpigeneticFactor& factor);
        
        // Update factor based on environmental input
        void updateFactors(const std::unordered_map<std::string, float>& environment);
        
        // Get factor value
        float getFactorValue(const std::string& factorName) const;
        
        // Get all factor values
        std::unordered_map<std::string, float> getAllFactorValues() const;
        
        // Configure environmental influences
        void setEnvironmentalInfluence(const std::string& factor,
                                      const std::string& environment,
                                      float influenceStrength);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure advanced development system
    void configureDevelopmentalTrajectory(DevelopmentalTrajectory::TrajectoryType type,
                                         const std::unordered_map<std::string, float>& parameters = {});
    void configurePhaseTransitions(PhaseTransitionController::TransitionType type,
                                  float transitionSpeed = 1.0f);
    void configureEpigeneticFactors(const std::vector<EpigeneticSystem::EpigeneticFactor>& factors);
    
    // Get development statistics
    struct DevelopmentStats {
        DevelopmentalStage currentStage;
        double developmentalAge;
        float plasticityModifier;
        bool isInCriticalPeriod;
        float criticalPeriodProgress;
        std::unordered_map<std::string, float> epigeneticValues;
        float trajectoryProgress;
        float transitionProgress;
        std::unordered_map<std::string, float> customDevelopmentMetrics;
    };
    
    DevelopmentStats getDevelopmentStats() const;
    
    // Environmental influences
    void applyEnvironmentalInfluence(const std::unordered_map<std::string, float>& environment);
    void setEnvironmentalContext(const std::string& context);
    
    // ========== ADVANCED SIMULATION CONTROL ==========
    
    // Custom timestep scheduling
    class CustomScheduler {
    public:
        enum class ScheduleMode {
            Fixed,           // Fixed timestep
            Variable,        // Variable timestep
            Adaptive,        // Adaptive based on complexity
            EventDriven,     // Event-driven
            Hybrid          // Hybrid approach
        };
        
        CustomScheduler();
        ~CustomScheduler();
        
        // Set scheduler mode
        void setScheduleMode(ScheduleMode mode);
        
        // Configure scheduler parameters
        void setBaseTimestep(double timestep);
        void setMinTimestep(double minTimestep);
        void setMaxTimestep(double maxTimestep);
        void setAdaptationFactor(float factor);
        
        // Schedule simulation step
        double scheduleStep(double currentTime, double currentError);
        
        // Get current timestep
        double getCurrentTimestep() const;
        
        // Configure adaptive scheduler
        void enableAdaptiveScheduler(bool enable,
                                    float complexityThreshold = 0.5f,
                                    float maxTimestepReduction = 0.5f);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Parallel simulation support
    class ParallelSimulator {
    public:
        ParallelSimulator();
        ~ParallelSimulator();
        
        // Configure parallel simulation
        void configureParallelProcessing(size_t numThreads,
                                        bool enableSIMD = true,
                                        bool enableGPU = false,
                                        size_t memoryPoolSize = 1024 * 1024 * 1024);
        
        // Get parallelization statistics
        struct ParallelStats {
            size_t activeThreads;
            size_t totalThreads;
            float CPUUtilization;
            float GPUMemoryUsage;
            std::vector<std::string> activeThreads;
            std::unordered_map<std::string, float> customMetrics;
        };
        
        ParallelStats getParallelStats() const;
        
        // Is parallel processing enabled?
        bool isParallelProcessingEnabled() const;
        
        // Get number of regions processed in parallel
        size_t getParallelRegionCount() const;
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure advanced simulation control
    void configureCustomScheduler(CustomScheduler::ScheduleMode mode,
                                 const std::unordered_map<std::string, float>& parameters = {});
    void configureParallelSimulation(size_t numThreads, bool enableSIMD = true);
    
    // Get simulation control statistics
    struct SimulationControlStats {
        double currentTimestep;
        double simulationTimeScale;
        size_t parallelThreadCount;
        float simulationProgress;
        std::unordered_map<std::string, float> customControlMetrics;
    };
    
    SimulationControlStats getSimulationControlStats() const;
    
    // Real-time monitoring tools
    class RealTimeMonitor {
    public:
        struct PerformanceMetrics {
            double cpuTime;
            double memoryUsage;
            double networkLatency;
            float neuronFiringRate;
            float simulationFPS;
            float queueSize;
            std::unordered_map<std::string, float> customMetrics;
        };
        
        RealTimeMonitor();
        ~RealTimeMonitor();
        
        // Start monitoring
        void startMonitoring(double monitoringInterval = 0.1);
        
        // Get latest performance metrics
        PerformanceMetrics getLatestMetrics() const;
        
        // Configure monitoring
        void enableMetricsCollection(const std::vector<std::string>& metrics,
                                    bool enableStreaming = false,
                                    const std::string& outputFile = "");
        
        // Get monitoring statistics
        struct MonitorStats {
            size_t dataPointsCollected;
            double monitoringDuration;
            double averageCPUUsage;
            double peakMemoryUsage;
            std::vector<double> recentCPUUsage;
            std::vector<double> recentMemoryUsage;
        };
        
        MonitorStats getMonitorStats() const;
        
        // Trigger performance profiling
        void triggerProfiling(bool enableDetailedProfiling = true);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure real-time monitoring
    void configureRealTimeMonitoring(bool enableMonitoring,
                                    double monitoringInterval = 0.1,
                                    const std::vector<std::string>& metrics = {});
    
    // Get monitoring statistics
    RealTimeMonitor::MonitorStats getMonitoringStats() const;
    
    // Performance profiling tools
    class PerformanceProfiler {
    public:
        enum class ProfilingLevel {
            Basic,   // Basic performance metrics
            Detailed, // Detailed profiling with call stacks
            Instruments, // Instruments-based profiling
            GPU       // GPU-specific profiling
        };
        
        PerformanceProfiler();
        ~PerformanceProfiler();
        
        // Start profiling
        void startProfiling(ProfilingLevel level = ProfilingLevel::Detailed,
                           double profileDuration = 60.0);
        
        // Get profiling results
        struct ProfilingResults {
            double totalRuntime;
            double CPUTime;
            double GPUTime;
            std::unordered_map<std::string, double> componentTimes;
            std::vector<std::string> bottlenecks;
            std::unordered_map<std::string, float> customMetrics;
        };
        
        ProfilingResults getProfilingResults() const;
        
        // Configure profiling
        void enableStackTrace(bool enable = true);
        void enableMemoryTracking(bool enable = true);
        void enableNetworkTracking(bool enable = true);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure performance profiling
    void configurePerformanceProfiling(PerformanceProfiler::ProfilingLevel level,
                                      double profileDuration = 60.0);
    
    // Get profiling results
    PerformanceProfiler::ProfilingResults getProfilingResults() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
