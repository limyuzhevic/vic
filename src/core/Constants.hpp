// Configuration Constants Header
// =============================
//
// This header file contains named constants that are commonly used throughout
the NLM codebase. These constants are extracted from magic numbers to improve
code readability and maintainability.
//
// These constants can be overridden by defining the same names with different
// values before including this header (via preprocessor directives).

#pragma once

namespace nlm {

// =======================================
// NEURAL NETWORK ARCHITECTURE CONSTANTS
// =======================================

// Default number of neurons in the brain when not specified
const size_t DefaultNeuronCount = 1000;

// Default number of brain regions when not specified
const size_t DefaultRegionCount = 2;

// Default connection probability between neurons (0.0 to 1.0)
const float DefaultConnectionProbability = 0.1f;

// Maximum number of neurons per region
const size_t MaxNeuronsPerRegion = 500;

// Minimum weight threshold for synaptic connections
const float MinWeightThreshold = 0.01f;

// =======================================
// PLASTICITY PARAMETERS
// =======================================

// STDP (Spike-Timing-Dependent Plasticity) parameters
const float DefaultSTDP LTPWeight = 0.01f;
const float DefaultSTDP LTDWeight = 0.012f;
const float DefaultSTDPTimeConstant = 20.0f;

// Hebbian learning rate
const float DefaultHebbianLearningRate = 0.01f;

// Maximum synaptic weight
const float DefaultMaxSynapseWeight = 1.0f;

// Minimum synaptic weight
const float DefaultMinSynapseWeight = -1.0f;

// Sign enforcement threshold (minimum absolute weight)
const float MinWeightMagnitude = 0.01f;

// =======================================
// SIMULATION PARAMETERS
// =======================================

// Default simulation timestep in seconds
const float DefaultSimulationTimestep = 0.001f;

// Simulation timestep for environment updates (0.1 seconds)
const float EnvironmentTimestep = 0.1f;

// Learning event timestep for plasticity (0.001 seconds)
const float PlasticityTimestep = 0.001f;

// =======================================
// MEMORY PARAMETERS
// =======================================

// Working memory decay rate
const float DefaultWorkingMemoryDecayRate = 0.01f;

// Working memory activation threshold
const float DefaultWorkingMemoryActivationThreshold = 0.1f;

// Memory consolidation interval in steps
const size_t DefaultConsolidationInterval = 1000;

// Maximum number of working memory traces
const size_t MaxWorkingMemoryTraces = 1000;

// Maximum number of episodic memories
const size_t MaxEpisodicMemories = 1000;

// Maximum number of associative memories
const size_t MaxAssociativeMemories = 10000;

// =======================================
// NEUROMODULATION PARAMETERS
// =======================================

// Dopamine decay rate
const float DefaultDopamineDecayRate = 0.1f;

// Curiosity decay rate
const float DefaultCuriosityDecayRate = 0.05f;

// Novelty threshold for detection
const float DefaultNoveltyThreshold = 0.3f;

// Default neuromodulator baseline level
const float DefaultNeuromodulatorBaseline = 0.0f;

// Neuromodulator peak level
const float DefaultNeuromodulatorPeak = 1.0f;

// Neuromodulator release rate
const float DefaultNeuromodulatorReleaseRate = 1.0f;

// =======================================
// DEVELOPMENT PARAMETERS
// =======================================

// Synaptogenesis rate (neuron creation during development)
const float DefaultSynaptogenesisRate = 0.001f;

// Pruning rate (synapse removal during development)
const float DefaultPruningRate = 0.0001f;

// Developmental stage transition rate
const float DefaultDevelopmentalTransitionRate = 0.1f;

// =======================================
// PREDICTION SYSTEM PARAMETERS
// =======================================

// Prediction error computation threshold
const float DefaultPredictionErrorThreshold = 0.001f;

// Prediction history size limit
const size_t MaxPredictionHistorySize = 1000;

// Prediction confidence threshold
const float DefaultPredictionConfidenceThreshold = 0.5f;

// =======================================
// COGNITION PARAMETERS
// =======================================

// Concept formation similarity threshold
const float DefaultConceptSimilarityThreshold = 0.05f;

// Planning horizon in steps (number of future actions to consider)
const size_t DefaultPlanningHorizon = 10;

// =======================================
// SENSORY/MOTOR PARAMETERS
// =======================================

// Sensor input normalization threshold
const float DefaultSensorInputThreshold = 0.01f;

// Motor action selection threshold
const float DefaultActionSelectionThreshold = 0.1f;

// Maximum motor speed (units per second)
const float DefaultMaxMotorSpeed = 3.0f;

// Maximum turn speed (radians per second)
const float DefaultMaxTurnSpeed =           2.0f;

// Environment boundary tolerance (when agent hits boundary)
const float EnvironmentBoundaryTolerance = 0.01f;

// =======================================
// REWARD AND FEEDBACK PARAMETERS
// =======================================

// Reward for successful action
const float DefaultPositiveReward = 1.0f;

// Negative reward for failed action
const float DefaultNegativeReward = -1.0f;

// Small movement penalty
const float DefaultMovementPenalty = -0.01f;

// Wall collision penalty
const float DefaultWallCollisionPenalty = -0.05f;

// Passive damage per step
const float DefaultPassiveDamage = -0.01f;

// Learning rate for reward prediction
const float DefaultRewardLearningRate = 0.1f;

// =======================================
// VISUALIZATION PARAMETERS
// =======================================

// Default visualization update rate (Hz)
const float DefaultVisualizationUpdateRate = 30.0f;

// Visualization field of view (radians)
const float DefaultVisualizationFOV = 1.57079632679489661923f;  // PI / 2.0f

// Number of rays for visualization
const size_t DefaultVisualizationRayCount = 8;

// =======================================
// CHECKPOINT/SAVING PARAMETERS
// =======================================

// Checkpoint save interval in simulation steps
const size_t DefaultCheckpointSaveInterval = 10000;

// Maximum number of checkpoints to retain
const size_t DefaultMaxCheckpoints = 10;

// =======================================
// TIMING AND SAMPLING
// =======================================

// Default training steps for comprehensive experiments
const size_t DefaultTrainingSteps = 10000;

// Default testing steps for validation
const size_t DefaultTestingSteps = 1000;

// =======================================
// DEBUG AND LOGGING CONSTANTS
// =======================================

// Default log level when not specified
const std::string DefaultLogLevel = "INFO";

// Maximum log file size before rotation (in MB)
const size_t MaxLogFileSizeMB = 100;

// =======================================
// CONFIGURATION PARSING CONSTANTS
// =======================================

// Default configuration file encoding
const std::string DefaultConfigEncoding = "utf-8";

// =======================================
// SYSTEM CONSTANTS
// =======================================

// Mathematical constant: PI
const float Math_PI = 3.14159265358979323846f;

// Mathematical constant: PI / 2
const float Math_PI_2 = 1.57079632679489661923f;

// Mathematical constant: 2 * PI
const float Math_2PI = 6.28318530717958647692f;

// =======================================
// VALIDATION AND BOUNDARY CHECKS
// =======================================

// Maximum number of regions (architecture limit)
const size_t MaxRegions = 20;

// Maximum number that can be represented as NeuronId
const size_t MaxNeuronId = 1000000;

// =======================================
} // namespace nlm