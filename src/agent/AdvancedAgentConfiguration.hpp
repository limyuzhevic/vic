#pragma once

// Advanced Agent Configuration for NLM (熙然) Neural Learning Machine
// Phase 6: Final Integration - Enhanced Agent Configuration System

#include "AgentBrain.hpp"
#include "AgentBrainFactory.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

// ===== ADVANCED AGENT CONFIGURATION =====
class AdvancedAgentConfig {
public:
    // ===== NEURAL NETWORK CONFIGURATION =====
    struct NeuralNetworkConfig {
        // Network architecture
        struct Architecture {
            size_t inputNeurons;        // Number of sensory input neurons
            size_t hiddenNeurons;       // Number of hidden neurons
            size_t outputNeurons;       // Number of motor output neurons
            size_t numLayers;           // Number of hidden layers
            float connectionDensity;     // Proportion of possible connections
            float initialWeightRange;   // Range for initial synaptic weights (-range to +range)
        } architecture;
        
        // Activation functions
        struct Activation {
            std::string hiddenActivation;    // Activation for hidden layers (relu, sigmoid, tanh, leaky_relu)
            std::string outputActivation;   // Activation for output layer (softmax, linear, relu)
            bool useBatchNorm;              // Use batch normalization
            float dropoutRate;              // Dropout rate for regularization (0.0 to 1.0)
        } activation;
        
        // Network dynamics
        struct Dynamics {
            float learningRate;            // Overall learning rate
            float decayRate;              // Weight decay for regularization
            float momentum;               // Momentum for optimization
            bool useAdaptiveLearning;     // Use adaptive learning rates (Adam, RMSprop)
            std::string optimizer;       // Optimizer type (sgd, adam, rmsprop, adagrad)
            float epsilon;                // Numerical stability epsilon
        } dynamics;
    } neural;
    
    // ===== MEMORY SYSTEM CONFIGURATION =====
    struct MemorySystemConfig {
        // Working memory configuration
        struct WorkingMemory {
            size_t capacity;              // Maximum number of memory traces
            float decayRate;              // Decay rate for memory traces
            float interferenceStrength;   // Strength of interference between traces
            float consolidationThreshold; // Threshold for memory consolidation
            bool enableReplay;            // Enable memory replay during rest
            size_t replayInterval;        // Replay frequency (steps)
        } workingMemory;
        
        // Episodic memory configuration
        struct EpisodicMemory {
            size_t maxEpisodes;           // Maximum number of stored episodes
            float encodingStrength;       // Strength of episodic encoding
            float retrievalStrength;      // Strength of episodic retrieval
            float compressionRatio;       // Compression ratio for storage
            bool enablePruning;           // Enable memory pruning
            float pruningThreshold;       // Threshold for episode removal
        } episodicMemory;
        
        // Semantic memory configuration
        struct SemanticMemory {
            size_t associativeStrength;    // Strength of associations
            float spreadingActivation;    // Spread activation radius
            bool enableCategoryFormation; // Enable automatic category formation
            size_t categoryMaxSize;        // Maximum size of semantic categories
        } semanticMemory;
        
        // Procedural memory configuration
        struct ProceduralMemory {
            size_t skillCount;            // Number of procedural skills
            float reinforcementLearningRate; // RL learning rate
            float explorationRate;       // Exploration vs. exploitation trade-off
            bool enablePolicyLearning;   // Enable policy-based learning
            float discountFactor;        // Discount factor for future rewards
        } proceduralMemory;
    } memory;
    
    // ===== PLASTICITY CONFIGURATION =====
    struct PlasticityConfig {
        // Synaptic plasticity rules
        struct Synaptic {
            struct STDP {
                float lrPre;             // Pre-synaptic learning rate
                float lrPost;            // Post-synaptic learning rate
                float tauPre;            // Pre-synaptic trace decay
                float tauPost;           // Post-synaptic trace decay
                float maxWeightChange;   // Maximum weight change per event
                bool useNeuromodulation; // Integrate with neuromodulation
                float neuromodulationGain; // Neuromodulation gain factor
            } stdp;
            
            struct Hebbian {
                float learningRate;      // Hebbian learning rate
                float windowSize;        // Time window for Hebbian learning
                bool useOjaRule;         // Use Oja's rule for normalization
                float traceDecay;        // Trace decay rate
            } hebbian;
            
            struct Homeostatic {
                float targetRate;        // Target firing rate
                float learningRate;      // Homeostatic learning rate
                float timeConstant;      // Time constant for homeostasis
                bool useActivityNormalization; // Use activity-dependent scaling
            } homeostatic;
        } synaptic;
        
        // Structural plasticity
        struct Structural {
            float synaptogenesisRate;     // Rate of new synapse formation
            float pruningRate;           // Rate of synapse elimination
            float rewiringProbability;   // Probability of synapse rewiring
            bool enableGrowth;           // Enable structural growth
            bool enablePruning;          // Enable structural pruning
            float competitionStrength;   // Strength of synaptic competition
        } structural;
    } plasticity;
    
    // ===== COGNITIVE CONFIGURATION =====
    struct CognitiveConfig {
        // Attention mechanism
        struct Attention {
            std::string attentionType;    // Attention type (dot_product, additive, Bahdanau, Luong)
            size_t attentionHeadCount;   // Number of attention heads
            float attentionDim;          // Dimension of attention space
            float inhibitionStrength;   // Strength of competitive inhibition
            bool enableDynamicAttention; // Enable dynamic attention allocation
            float attentionUpdateRate;   // Rate of attention updating
        } attention;
        
        // Prediction and planning
        struct Prediction {
            size_t predictionHorizon;    // Number of steps to predict ahead
            float predictionAccuracyThreshold; // Minimum prediction accuracy
            bool enableBayesianInference; // Use Bayesian prediction
            float uncertaintyWeight;     // Weight of uncertainty in decision making
        } prediction;
        
        // Concept formation
        struct Concept {
            size_t conceptMaxSize;        // Maximum concepts in semantic memory
            float similarityThreshold;   // Threshold for concept merging
            bool enableConceptAbstraction; // Enable concept abstraction
            float abstractionStrength;   // Strength of concept abstraction
        } concept;
        
        // Planning
        struct Planning {
            std::string planningAlgorithm; // Planning algorithm (pathfinding, reinforcement, MCTS)
            size_t planningHorizon;       // Planning horizon
            float explorationValue;      // Value of exploration in planning
            bool enableMonteCarlo;       // Enable Monte Carlo planning
            size_t simulationRollouts;   // Number of planning simulations
        } planning;
    } cognitive;
    
    // ===== NEUROMODULATION CONFIGURATION =====
    struct NeuromodulationConfig {
        // Dopamine system
        struct Dopamine {
            float baselineLevel;         // Baseline dopamine level
            float learningSignalGain;   // Gain for reward prediction error
            float rewardSensitivity;    // Sensitivity to reward magnitude
            bool enablePhasicResponse;  // Enable phasic dopamine responses
            float tonicDecayRate;       // Decay rate of tonic dopamine
        } dopamine;
        
        // Curiosity system
        struct Curiosity {
            float intrinsicReward;       // Intrinsic reward for exploration
            float noveltyThreshold;     // Novelty threshold for curiosity
            float explorationCost;     // Cost of exploration behavior
            bool enableAnticipatoryCuriosity; // Enable anticipatory curiosity
            float futureDiscount;       // Discount factor for future curiosity
        } curiosity;
        
        // Novelty detection
        struct Novelty {
            std::string noveltyMetric;   // Novelty metric (histogram, autoencoder, reconstruction)
            float sensitivity;           // Sensitivity to novel inputs
            bool enableAdaptiveThreshold; // Enable adaptive novelty thresholds
            float adaptationRate;        // Rate of threshold adaptation
        } novelty;
        
        // Prediction error
        struct PredictionError {
            float errorGain;             // Gain for prediction error signals
            bool enablePrecisionWeighting; // Enable precision-weighted prediction errors
            float confidenceThreshold;  // Confidence threshold for error signals
        } predictionError;
    } neuromodulation;
    
    // ===== DEVELOPMENT CONFIGURATION =====
    struct DevelopmentConfig {
        // Critical periods
        struct CriticalPeriods {
            double initialPeriod;        // Duration of initial critical period
            double socialPeriod;         // Duration of social development period
            double sensitivePeriod;      // Duration of sensitive period
            bool enableStageTransitions; // Enable developmental stage transitions
        } criticalPeriods;
        
        // Structural development
        struct Structural {
            float growthRate;            // Rate of structural growth
            float atrophyRate;          // Rate of structural atrophy
            bool enablePruning;          // Enable structural pruning
            float resourceAllocation;    // Proportion of resources to structure
        } structural;
        
        // Functional development
        struct Functional {
            float connectivityMaturation; // Maturation of connectivity patterns
            bool enableSynaptogenesis;    // Enable activity-dependent synaptogenesis
            bool enablePruning;           // Enable activity-dependent pruning
            float experienceDependency;   // Dependence on experience
        } functional;
    } development;
    
    // ===== BEHAVIORAL CONFIGURATION =====
    struct BehavioralConfig {
        // Action selection
        struct ActionSelection {
            std::string policy;         // Policy type (epsilon_greedy, softmax, UCB, Thompson_sampling)
            float explorationRate;     // Rate of exploratory actions
            float exploitationRate;    // Rate of exploitative actions
            bool enableIntrinsicallyMotivated; // Enable intrinsic motivation
            float intrinsicMotivationalStrength; // Strength of intrinsic motivation
        } actionSelection;
        
        // Motivational systems
        struct Motivation {
            float rewardValue;          // Immediate reward value
            float punishmentValue;       // Immediate punishment value
            float goalImportance;       // Importance of goal achievement
            bool enableSocialMotivation; // Enable social motivation
            float socialBondStrength;   // Strength of social bonds
        } motivation;
        
        // Learning and adaptation
        struct Adaptation {
            float adaptationRate;       // Rate of behavioral adaptation
            bool enableMetaLearning;    // Enable meta-learning
            float learningTrajectory;   // Learning curve trajectory
            bool enableTransferLearning; // Enable transfer learning across tasks
        } adaptation;
    } behavioral;
    
    // ===== ADVANCED SETTINGS =====
    struct AdvancedSettings {
        // Performance settings
        size_t maxThreads;            // Maximum number of threads for parallel processing
        bool enableVectorization;     // Enable SIMD vectorization
        bool enableGPUAcceleration;   // Enable GPU acceleration (if available)
        size_t memoryPoolSize;        // Size of memory pool for allocation
        
        // Simulation settings
        size_t maxSteps;              // Maximum simulation steps
        double timeLimit;             // Time limit for simulation (0 = unlimited)
        bool enableCheckpointing;     // Enable checkpoint saving
        size_t checkpointInterval;    // Checkpoint saving interval
        
        // Logging and debugging
        std::string logLevel;        // Log level (debug, info, warning, error, critical)
        bool enablePerformanceMetrics; // Enable performance metrics collection
        bool enableDebugMode;         // Enable debug mode with additional checks
        
        // Experimental settings
        bool enableAblationStudies;   // Enable ablation studies
        std::vector<std::string> disabledModules; // List of disabled modules
        bool enableCounterfactualReasoning; // Enable counterfactual reasoning
        
        // Security and safety
        bool enableSafetyChecks;      // Enable safety checks
        float safetyThreshold;        // Safety threshold for preventing extreme behaviors
        bool enableEmergencyStop;    // Enable emergency stop on safety violations
    } advanced;
    
    // Constructor with default values
    AdvancedAgentConfig() {
        // Set default values for all configuration
        initializeDefaults();
    }
    
    // Initialize with defaults
    void initializeDefaults() {
        // Neural network defaults
        neural.architecture.inputNeurons = 256;
        neural.architecture.hiddenNeurons = 512;
        neural.architecture.outputNeurons = 6;
        neural.architecture.numLayers = 3;
        neural.architecture.connectionDensity = 0.1f;
        neural.architecture.initialWeightRange = 0.1f;
        
        neural.activation.hiddenActivation = "relu";
        neural.activation.outputActivation = "linear";
        neural.activation.useBatchNorm = true;
        neural.activation.dropoutRate = 0.1f;
        
        neural.dynamics.learningRate = 0.01f;
        neural.dynamics.decayRate = 0.0001f;
        neural.dynamics.momentum = 0.9f;
        neural.dynamics.useAdaptiveLearning = true;
        neural.dynamics.optimizer = "adam";
        neural.dynamics.epsilon = 1e-8f;
        
        // Memory system defaults
        memory.workingMemory.capacity = 1000;
        memory.workingMemory.decayRate = 0.95f;
        memory.workingMemory.interferenceStrength = 0.5f;
        memory.workingMemory.consolidationThreshold = 0.7f;
        memory.workingMemory.enableReplay = true;
        memory.workingMemory.replayInterval = 100;
        
        memory.episodicMemory.maxEpisodes = 1000;
        memory.episodicMemory.encodingStrength = 1.0f;
        memory.episodicMemory.retrievalStrength = 0.8f;
        memory.episodicMemory.compressionRatio = 0.5f;
        memory.episodicMemory.enablePruning = true;
        memory.episodicMemory.pruningThreshold = 0.1f;
        
        memory.semanticMemory.associativeStrength = 1.0f;
        memory.semanticMemory.spreadingActivation = 0.5f;
        memory.semanticMemory.enableCategoryFormation = true;
        memory.semanticMemory.categoryMaxSize = 50;
        
        memory.proceduralMemory.skillCount = 10;
        memory.proceduralMemory.reinforcementLearningRate = 0.01f;
        memory.proceduralMemory.explorationRate = 0.1f;
        memory.proceduralMemory.enablePolicyLearning = true;
        memory.proceduralMemory.discountFactor = 0.95f;
        
        // Plasticity defaults
        plasticity.synaptic.stdp.lrPre = 0.01f;
        plasticity.synaptic.stdp.lrPost = 0.01f;
        plasticity.synaptic.stdp.tauPre = 20.0f;
        plasticity.synaptic.stdp.tauPost = 20.0f;
        plasticity.synaptic.stdp.maxWeightChange = 0.1f;
        plasticity.synaptic.stdp.useNeuromodulation = true;
        plasticity.synaptic.stdp.neuromodulationGain = 1.0f;
        
        plasticity.synaptic.hebbian.learningRate = 0.001f;
        plasticity.synaptic.hebbian.windowSize = 100;
        plasticity.synaptic.hebbian.useOjaRule = true;
        plasticity.synaptic.hebbian.traceDecay = 0.9f;
        
        plasticity.synaptic.homeostatic.targetRate = 0.1f;
        plasticity.synaptic.homeostatic.learningRate = 0.001f;
        plasticity.synaptic.homeostatic.timeConstant = 1000.0f;
        plasticity.synaptic.homeostatic.useActivityNormalization = true;
        
        plasticity.structural.synaptogenesisRate = 0.0001f;
        plasticity.structural.pruningRate = 0.00001f;
        plasticity.structural.rewiringProbability = 0.01f;
        plasticity.structural.enableGrowth = true;
        plasticity.structural.enablePruning = true;
        plasticity.structural.competitionStrength = 0.5f;
        
        // Cognitive defaults
        cognitive.attention.attentionType = "dot_product";
        cognitive.attention.attentionHeadCount = 8;
        cognitive.attention.attentionDim = 64;
        cognitive.attention.inhibitionStrength = 0.5f;
        cognitive.attention.enableDynamicAttention = true;
        cognitive.attention.attentionUpdateRate = 0.1f;
        
        cognitive.prediction.predictionHorizon = 10;
        cognitive.prediction.predictionAccuracyThreshold = 0.7f;
        cognitive.prediction.enableBayesianInference = true;
        cognitive.prediction.uncertaintyWeight = 0.5f;
        
        cognitive.concept.conceptMaxSize = 100;
        cognitive.concept.similarityThreshold = 0.8f;
        cognitive.concept.enableConceptAbstraction = true;
        cognitive.concept.abstractionStrength = 0.3f;
        
        cognitive.planning.planningAlgorithm = "MCTS";
        cognitive.planning.planningHorizon = 20;
        cognitive.planning.explorationValue = 0.5f;
        cognitive.planning.enableMonteCarlo = true;
        cognitive.planning.simulationRollouts = 100;
        
        // Neuromodulation defaults
        neuromodulation.dopamine.baselineLevel = 0.1f;
        neuromodulation.dopamine.learningSignalGain = 0.01f;
        neuromodulation.dopamine.rewardSensitivity = 1.0f;
        neuromodulation.dopamine.enablePhasicResponse = true;
        neuromodulation.dopamine.tonicDecayRate = 0.9f;
        
        neuromodulation.curiosity.intrinsicReward = 0.1f;
        neuromodulation.curiosity.noveltyThreshold = 0.5f;
        neuromodulation.curiosity.explorationCost = 0.01f;
        neuromodulation.curiosity.enableAnticipatoryCuriosity = true;
        neuromodulation.curiosity.futureDiscount = 0.9f;
        
        neuromodulation.novelty.noveltyMetric = "reconstruction";
        neuromodulation.novelty.sensitivity = 1.0f;
        neuromodulation.novelty.enableAdaptiveThreshold = true;
        neuromodulation.novelty.adaptationRate = 0.01f;
        
        neuromodulation.predictionError.errorGain = 1.0f;
        neuromodulation.predictionError.enablePrecisionWeighting = true;
        neuromodulation.predictionError.confidenceThreshold = 0.5f;
        
        // Development defaults
        development.criticalPeriods.initialPeriod = 60.0;
        development.criticalPeriods.socialPeriod = 180.0;
        development.criticalPeriods.sensitivePeriod = 300.0;
        development.criticalPeriods.enableStageTransitions = true;
        
        development.structural.growthRate = 0.01f;
        development.structural.atrophyRate = 0.001f;
        development.structural.enablePruning = true;
        development.structural.resourceAllocation = 0.3f;
        
        development.functional.connectivityMaturation = 0.5f;
        development.functional.enableSynaptogenesis = true;
        development.functional.enablePruning = true;
        development.functional.experienceDependency = 0.8f;
        
        // Behavioral defaults
        behavioral.actionSelection.policy = "softmax";
        behavioral.actionSelection.explorationRate = 0.1f;
        behavioral.actionSelection.exploitationRate = 0.9f;
        behavioral.actionSelection.enableIntrinsicallyMotivated = true;
        behavioral.actionSelection.intrinsicMotivationalStrength = 0.5f;
        
        behavioral.motivation.rewardValue = 1.0f;
        behavioral.motivation.punishmentValue = -1.0f;
        behavioral.motivation.goalImportance = 0.7f;
        behavioral.motivation.enableSocialMotivation = true;
        behavioral.motivation.socialBondStrength = 0.8f;
        
        behavioral.adaptation.adaptationRate = 0.01f;
        behavioral.adaptation.enableMetaLearning = true;
        behavioral.adaptation.learningTrajectory = 0.5f;
        behavioral.adaptation.enableTransferLearning = true;
        
        // Advanced settings defaults
        advanced.maxThreads = std::thread::hardware_concurrency();
        advanced.enableVectorization = true;
        advanced.enableGPUAcceleration = false;  // Enable if GPU available
        advanced.memoryPoolSize = 1024 * 1024 * 1024;  // 1 GB
        
        advanced.maxSteps = 100000;
        advanced.timeLimit = 0.0;
        advanced.enableCheckpointing = true;
        advanced.checkpointInterval = 1000;
        
        advanced.logLevel = "info";
        advanced.enablePerformanceMetrics = true;
        advanced.enableDebugMode = false;
        
        advanced.enableAblationStudies = false;
        advanced.enableCounterfactualReasoning = false;
        
        advanced.enableSafetyChecks = true;
        advanced.safetyThreshold = 10.0f;
        advanced.enableEmergencyStop = true;
    }
    
    // Validate configuration
    bool validate() const {
        ValidationResult result;
        
        // Validate neural network configuration
        if (neural.architecture.inputNeurons == 0) {
            result.addError("Input neurons must be greater than 0");
        }
        if (neural.architecture.hiddenNeurons == 0) {
            result.addError("Hidden neurons must be greater than 0");
        }
        if (neural.architecture.outputNeurons == 0) {
            result.addError("Output neurons must be greater than 0");
        }
        if (neural.architecture.numLayers < 1) {
            result.addError("Number of layers must be at least 1");
        }
        if (neural.architecture.connectionDensity < 0.0f || neural.architecture.connectionDensity > 1.0f) {
            result.addError("Connection density must be between 0.0 and 1.0");
        }
        if (neural.architecture.initialWeightRange <= 0.0f) {
            result.addError("Initial weight range must be positive");
        }
        
        // Validate activation functions
        if (neural.activation.hiddenActivation != "relu" && neural.activation.hiddenActivation != "sigmoid" &&
            neural.activation.hiddenActivation != "tanh" && neural.activation.hiddenActivation != "leaky_relu") {
            result.addError("Invalid hidden activation function: " + neural.activation.hiddenActivation);
        }
        if (neural.activation.outputActivation != "softmax" && neural.activation.outputActivation != "linear" &&
            neural.activation.outputActivation != "relu") {
            result.addError("Invalid output activation function: " + neural.activation.outputActivation);
        }
        if (neural.activation.dropoutRate < 0.0f || neural.activation.dropoutRate > 1.0f) {
            result.addError("Dropout rate must be between 0.0 and 1.0");
        }
        
        // Validate dynamics
        if (neural.dynamics.learningRate <= 0.0f) {
            result.addError("Learning rate must be positive");
        }
        if (neural.dynamics.decayRate < 0.0f || neural.dynamics.decayRate >= 1.0f) {
            result.addError("Decay rate must be between 0.0 and 1.0");
        }
        if (neural.dynamics.momentum < 0.0f || neural.dynamics.momentum >= 1.0f) {
            result.addError("Momentum must be between 0.0 and 1.0");
        }
        if (neural.dynamics.epsilon <= 0.0f) {
            result.addError("Epsilon must be positive");
        }
        
        // Validate memory configuration
        if (memory.workingMemory.capacity == 0) {
            result.addError("Working memory capacity must be greater than 0");
        }
        if (memory.workingMemory.decayRate <= 0.0f || memory.workingMemory.decayRate >= 1.0f) {
            result.addError("Working memory decay rate must be between 0.0 and 1.0");
        }
        if (memory.workingMemory.interferenceStrength < 0.0f || memory.workingMemory.interferenceStrength > 1.0f) {
            result.addError("Working memory interference strength must be between 0.0 and 1.0");
        }
        if (memory.workingMemory.consolidationThreshold <= 0.0f || memory.workingMemory.consolidationThreshold > 1.0f) {
            result.addError("Working memory consolidation threshold must be between 0.0 and 1.0");
        }
        if (memory.episodicMemory.maxEpisodes == 0) {
            result.addError("Episodic memory max episodes must be greater than 0");
        }
        if (memory.episodicMemory.encodingStrength < 0.0f || memory.episodicMemory.encodingStrength > 1.0f) {
            result.addError("Episodic memory encoding strength must be between 0.0 and 1.0");
        }
        if (memory.episodicMemory.retrievalStrength < 0.0f || memory.episodicMemory.retrievalStrength > 1.0f) {
            result.addError("Episodic memory retrieval strength must be between 0.0 and 1.0");
        }
        if (memory.episodicMemory.compressionRatio < 0.0f || memory.episodicMemory.compressionRatio > 1.0f) {
            result.addError("Episodic memory compression ratio must be between 0.0 and 1.0");
        }
        if (memory.episodicMemory.pruningThreshold <= 0.0f || memory.episodicMemory.pruningThreshold > 1.0f) {
            result.addError("Episodic memory pruning threshold must be between 0.0 and 1.0");
        }
        
        // Validate plasticity configuration
        if (plasticity.synaptic.stdp.lrPre <= 0.0f) {
            result.addError("STDP pre-synaptic learning rate must be positive");
        }
        if (plasticity.synaptic.stdp.lrPost <= 0.0f) {
            result.addError("STDP post-synaptic learning rate must be positive");
        }
        if (plasticity.synaptic.stdp.tauPre <= 0.0f) {
            result.addError("STDP pre-synaptic time constant must be positive");
        }
        if (plasticity.synaptic.stdp.tauPost <= 0.0f) {
            result.addError("STDP post-synaptic time constant must be positive");
        }
        if (plasticity.synaptic.stdp.maxWeightChange <= 0.0f) {
            result.addError("STDP maximum weight change must be positive");
        }
        
        // Validate attention configuration
        if (cognitive.attention.attentionHeadCount == 0) {
            result.addError("Attention head count must be greater than 0");
        }
        if (cognitive.attention.attentionDim <= 0) {
            result.addError("Attention dimension must be positive");
        }
        
        // Validate planning configuration
        if (cognitive.planning.planningHorizon <= 0) {
            result.addError("Planning horizon must be positive");
        }
        
        // Validate action selection
        if (behavioral.actionSelection.explorationRate < 0.0f || behavioral.actionSelection.explorationRate > 1.0f) {
            result.addError("Exploration rate must be between 0.0 and 1.0");
        }
        if (behavioral.actionSelection.exploitationRate < 0.0f || behavioral.actionSelection.exploitationRate > 1.0f) {
            result.addError("Exploitation rate must be between 0.0 and 1.0");
        }
        
        // Validate motivation
        if (behavioral.motivation.goalImportance < 0.0f || behavioral.motivation.goalImportance > 1.0f) {
            result.addError("Goal importance must be between 0.0 and 1.0");
        }
        if (behavioral.motivation.socialBondStrength < 0.0f || behavioral.motivation.socialBondStrength > 1.0f) {
            result.addError("Social bond strength must be between 0.0 and 1.0");
        }
        
        // Validate adaptation
        if (behavioral.adaptation.adaptationRate < 0.0f || behavioral.adaptation.adaptationRate > 1.0f) {
            result.addError("Adaptation rate must be between 0.0 and 1.0");
        }
        
        // Validate advanced settings
        if (advanced.maxThreads == 0) {
            result.addError("Maximum threads must be greater than 0");
        }
        if (advanced.memoryPoolSize == 0) {
            result.addError("Memory pool size must be greater than 0");
        }
        if (advanced.maxSteps == 0) {
            result.addError("Maximum steps must be greater than 0");
        }
        if (advanced.checkpointInterval == 0) {
            result.addError("Checkpoint interval must be greater than 0");
        }
        if (advanced.safetyThreshold <= 0.0f) {
            result.addError("Safety threshold must be positive");
        }
        
        return result.isValid();
    }
    
    // Get validation errors
    std::string getValidationErrors() const {
        ValidationResult result;
        std::string errors = "";
        
        if (!validate()) {
            errors = "Validation errors:\n";
            for (const auto& error : result.getErrors()) {
                errors += "  - " + error + "\n";
            }
        }
        
        return errors;
    }
}; // end class AdvancedAgentConfig

// ===== ADVANCED AGENT FACTORY =====
class AdvancedAgentFactory {
public:
    static std::unique_ptr<AgentBrain> createAdvancedAgent(
        std::shared_ptr<Brain> brain,
        const SimpleWorld& world,
        const AdvancedAgentConfig& config,
        const std::string& agentId = "");
    
    static std::unique_ptr<AgentBrain> createSpecializedAgent(
        std::shared_ptr<Brain> brain,
        const SimpleWorld& world,
        const std::string& specializationType,
        const std::string& agentId = "");
    
    static std::map<std::string, AdvancedAgentConfig> getBuiltInSpecializations();
    
    static AdvancedAgentConfig createConfigFromProfile(
        const std::string& profile,
        const std::string& specialization = "");
    
    static bool validateAndOptimizeConfig(AdvancedAgentConfig& config);
    
    static std::string generateAgentId(const std::string& prefix = "agent");
    
private:
    static std::map<std::string, AdvancedAgentConfig> builtInSpecializations_;
    static std::mutex configMutex_;
}; // end class AdvancedAgentFactory

// ===== AGENT SPECIALIZATION TYPES =====
enum class AgentSpecialization {
    None = 0,
    Sensorimotor,          // Specialized in sensory-motor integration
    Cognitive,             // Specialized in cognitive processing
    Social,                // Specialized in social interactions
    Developmental,         // Specialized in developmental processes
    Reactive,              // Specialized in reactive behaviors
    Proactive,             // Specialized in proactive behaviors
    Adaptive,              // Specialized in adaptive learning
    Conservative,          // Specialized in conservative behaviors
    Aggressive,            // Specialized in aggressive behaviors
    Balanced,              // Balanced across all domains
    Custom                 // User-defined specialization
};

// ===== HELPER FUNCTIONS =====
std::string advancedConfigToString(const AdvancedAgentConfig& config);
AdvancedAgentConfig defaultAdvancedConfigForSpecialization(AgentSpecialization specialization);
bool operator==(const AdvancedAgentConfig& lhs, const AdvancedAgentConfig& rhs);
bool operator!=(const AdvancedAgentConfig& lhs, const AdvancedAgentConfig& rhs);

} // namespace nlm

#endif // ADVANCED_AGENT_CONFIGURATION_HPP

(End of file - total 894 lines)