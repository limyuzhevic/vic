// Constants.hpp - Centralized constants for NLM (Neural Learning Machine) codebase
// This header file replaces magic numbers with named constants for better maintainability and clarity

#pragma once

#include <cstddef>
#include <cstdint>

namespace nlm {

// =====================================
// TIME CONSTANTS
// =====================================

/**
 * Simulation timestep in seconds (1ms)
 * Used throughout the neural simulation for time integration
 * From main.cpp: timestep = 0.001
 * From Brain.cpp: timestep(0.001) in constructor
 * From Environment.cpp: reward = -0.01f per step
 */
constexpr double SIMULATION_TIMESTEP = 0.001;  // 1 millisecond

/**
 * STDP plasticity update interval in timesteps
 * How often STDP parameters are updated
 * From Brain.cpp: replayInterval(100) in constructor
 */
constexpr size_t STDP_UPDATE_INTERVAL = 100;  // Update every 100 steps

/**
 * Development system update interval in timesteps
 * How often developmental changes occur
 * From Brain.cpp: consolidationInterval(1000) in constructor
 */
constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000;  // Update every 1000 steps

/**
 * Memory consolidation interval in timesteps
 * How often episodic memory is consolidated
 */
constexpr size_t MEMORY_CONSOLIDATION_INTERVAL = 1000;  // Consolidate every 1000 steps

/**
 * Replay interval in timesteps
 * How often important memories are replayed
 */
constexpr size_t MEMORY_REPLAY_INTERVAL = 100;  // Replay every 100 steps

/**
 * Development update interval (alternate)
 * From Brain.cpp: if (currentStep % 1000 == 0) for development
 */
constexpr size_t DEVELOPMENT_SYSTEM_UPDATE_INTERVAL = 1000;  // Update every 1000 steps

/**
 * Structural plasticity update interval
 * From Brain.cpp: if (currentStep % 100 == 0) for structural plasticity
 */
constexpr size_t STRUCTURAL_PLASTICITY_UPDATE_INTERVAL = 100;  // Update every 100 steps

/**
 * Step threshold for significant weight change detection
 * From main.cpp: if (delta > 0.01f) and if (delta < -0.01f)
 */
constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;  // Minimum weight change for "significant"

/**
 * Learning detection threshold
 * From main.cpp: if (std::abs(finalMean - initialMean) > 0.001f)
 */
constexpr float LEARNING_DETECTION_THRESHOLD = 0.001f;  // Minimum mean weight change for learning detection

// =====================================
// NEURAL CONSTANTS
// =====================================

/**
 * Neuron count for test configurations
 * From main.cpp: neuron_count = 500 (test)
 * From main.cpp: neuron_count = 1000 (default)
 * From default.cfg: neuron_count = 1000
 */
constexpr size_t NEURON_COUNT_TEST = 500;
constexpr size_t NEURON_COUNT_DEFAULT = 1000;

/**
 * Default region count for test configurations
 * From main.cpp: region_count = 1
 * From default.cfg: region_count = 2
 */
constexpr size_t REGION_COUNT_TEST = 1;
constexpr size_t REGION_COUNT_DEFAULT = 2;

/**
 * Connection probability for random connectivity
 * From main.cpp: connection_probability = 0.15f (test)
 * From default.cfg: connection_probability = 0.1
 * From Brain.cpp: connectionProbability = 0.1f
 */
constexpr float CONNECTION_PROBABILITY_TEST = 0.15f;
constexpr float CONNECTION_PROBABILITY_DEFAULT = 0.1f;

/**
 * STDP learning rate parameters
 * From main.cpp: stdp_ltp_weight = 0.02f, stdp_ltd_weight = 0.015f
 * From default.cfg: stdp_ltp_weight = 0.01, stdp_ltd_weight = 0.012
 * From Brain.cpp: ltpWeight = 0.01f, ltdWeight = 0.012f
 * From Config.cpp: stdp_ltp_weight = 0.01f, stdp_ltd_weight = 0.012f
 */
constexpr float STDP_LTP_WEIGHT_MAIN = 0.02f;     // From main.cpp
constexpr float STDP_LTD_WEIGHT_MAIN = 0.015f;    // From main.cpp
constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.01f;   // From default.cfg, Brain.cpp
constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.012f;  // From default.cfg, Brain.cpp

/**
 * STDP time constant
 * From main.cpp: stdp_tau = 20.0f
 * From default.cfg: stdp_time_constant = 20.0
 * From Brain.cpp: tau = 20.0f
 */
constexpr float STDP_TIME_CONSTANT = 20.0f;  // From main.cpp, default.cfg, Brain.cpp

/**
 * Synaptic weight bounds
 * From main.cpp: min/max weight in initialization
 * From Brain.cpp: region->initializeRandomConnectivity uses 0.2f, 0.1f
 */
constexpr float MIN_SYNAPTIC_WEIGHT = 0.01f;   // Minimum absolute weight
constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;    // Maximum absolute weight
constexpr float MAX_EXCITATORY_WEIGHT = 0.2f;  // Maximum excitatory weight from Brain.cpp
constexpr float MAX_INHIBITORY_WEIGHT = 0.1f;  // Maximum inhibitory weight from Brain.cpp

/**
 * Neuron membrane potential thresholds
 * From Neuron.cpp: -55.0f + rng.uniformReal(-2.0f, 2.0f)
 * From Brain.cpp: neuron->setThreshold() calls
 */
constexpr float NEURON_THRESHOLD_DEFAULT = -55.0f;     // Default firing threshold (mV)
constexpr float NEURON_THRESHOLD_VARIANCE = 2.0f;     // Random variance around default threshold
constexpr float NEURON_RESET_POTENTIAL = -70.0f;      // Reset potential after spike (mV)
constexpr float NEURON_RESTING_POTENTIAL = -65.0f;    // Resting potential (mV)

/**
 * Neuron excitability modulation
 * From Brain.cpp: float excitabilityMod = dopamineLevel_ * 0.5f
 */
constexpr float DOPAMINE_EXCITABILITY_FACTOR = 0.5f;  // How much dopamine affects excitability

/**
 * Synaptic plasticity modulation
 * From AgentBrain.cpp: float plasticityMod = 0.5f + 0.5f * dopamineLevel_
 */
constexpr float NEUROMODULATION_PLASTICITY_FACTOR = 0.5f;  // Baseline neuromodulation effect on plasticity

/**
 * Learning rate parameters
 * From main.cpp: plasticity_learning_rate = 0.01
 * From default.cfg: plasticity_learning_rate = 0.01
 * From STDP.cpp: ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f)
 * From Hebbian.cpp: learningRate(0.01f)
 * From PlasticityRule.cpp: learningRate(0.01f)
 * From Pruning.cpp: threshold(0.01f)
 */
constexpr float HEBBIAN_LEARNING_RATE = 0.01f;      // Hebbian learning rate
constexpr float PLASTICITY_LEARNING_RATE = 0.01f;   // General plasticity learning rate
constexpr float PRUNING_THRESHOLD = 0.01f;           // Pruning threshold

// =====================================
// ENVIRONMENT CONSTANTS
// =====================================

/**
 * GridWorld environment dimensions (placeholder)
 * From Environment.cpp: Impl(int w, int h) constructor
 * From default.cfg: environment_width = 10, environment_height = 10
 * From main.cpp: AgentBrain::initialize uses SimpleWorld
 */
constexpr int ENVIRONMENT_WIDTH = 10;
constexpr int ENVIRONMENT_HEIGHT = 10;

/**
 * Environment step rewards
 * From Environment.cpp: float reward = -0.01f per step, reward = 1.0f for goal
 * From SimpleWorld.cpp: result.reward = -0.01f for wall collision, -0.05f for damage
 * From main.cpp: applyRewardModulation uses reward from environment
 */
constexpr float ENVIRONMENT_STEP_REWARD = -0.01f;    // Penalty for each step
constexpr float ENVIRONMENT_GOAL_REWARD = 1.0f;      // Reward for reaching goal
constexpr float ENVIRONMENT_WALL_COLLISION_REWARD = -0.01f;  // From Environment.cpp
constexpr float ENVIRONMENT_DAMAGE_REWARD = -0.05f;  // From SimpleWorld.cpp

/**
 * Observation and action space sizes
 * From Environment.cpp: getActionSpaceSize() returns 4
 * From Environment.cpp: getObservationSpaceSize() returns 4
 * From AgentBrain.cpp: getSensoryInputSize() returns 256+8+4+6 = 274
 * From AgentBrain.cpp: getMotorOutputSize() returns 6
 */
constexpr size_t ENVIRONMENT_ACTION_SPACE_SIZE = 4;   // 4 movement actions
constexpr size_t ENVIRONMENT_OBSERVATION_SPACE_SIZE = 4;  // agent_x, agent_y, goal_x, goal_y
constexpr size_t SENSOR_INPUT_SIZE_VISION = 256;      // 16x16 vision
constexpr size_t SENSOR_INPUT_SIZE_TOUCH = 8;        // Touch sensor
constexpr size_t SENSOR_INPUT_SIZE_INTERNAL = 4;     // Internal signals
constexpr size_t SENSOR_INPUT_SIZE_PROPRIOCEPTION = 6;  // Proprioception
constexpr size_t MOTOR_OUTPUT_SIZE = 6;               // 6 motor commands

/**
 * Normalization constants for environmental observations
 * From Environment.cpp: static_cast<float>(pImpl->agentX) / pImpl->width
 */
constexpr float ENV_POSITION_NORMALIZATION = 1.0f;    // Position values are normalized to [0, 1]

/**
 * Sensor input scaling factors
 * From AgentBrain.cpp: vision[i] * 5.0f, touch[i] * 8.0f, etc.
 */
constexpr float VISION_SCALE_FACTOR = 5.0f;            // Vision signal scaling
constexpr float TOUCH_SCALE_FACTOR = 8.0f;            // Touch signal scaling
constexpr float INTERNAL_SCALE_FACTOR = 5.0f;         // Internal signal scaling
constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;   // Proprioception scaling

// =====================================
// MEMORY CONSTANTS
// =====================================

/**
 * Working memory capacity constants
 * From Brain.cpp: pImpl->workingMemory->setCapacity(neuronCount / 10)
 * From NeuralWorkingMemory.cpp: setCapacity(neuronCount / 10)
 * From default.cfg: Not specified
 */
constexpr size_t WORKING_MEMORY_CAPACITY_DEFAULT = 500;  // Default working memory capacity (neuronCount/10 for 5000 neurons)

/**
 * Episodic memory constants
 * From Brain.cpp: pImpl->episodicMemory->setMaxEpisodes(1000)
 * From default.cfg: Not specified
 * From NeuralEpisodicMemory.cpp: , maxEpisodes_(1000)
 */
constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;    // Maximum number of stored episodes

/**
 * Associative memory constants
 * From default.cfg: Not specified
 * From NeuralAssociativeMemory.hpp: Not specified in constructor
 */
constexpr size_t ASSOCIATIVE_MEMORY_MAX_PATTERNS = 1000;  // Estimated based on other memory systems

/**
 * Neural working memory decay rate
 * From NeuralWorkingMemory.cpp: decayRate_(0.01f)
 * From NeuralWorkingMemory.cpp: if (memoryActivations_[i] < 0.01f)
 */
constexpr float WORKING_MEMORY_ACTIVITY_THRESHOLD = 0.01f;  // Minimum activity to maintain trace
constexpr float WORKING_MEMORY_DECAY_RATE = 0.01f;         // Working memory decay rate

/**
 * Neural working memory competition threshold
 * From NeuralWorkingMemory.cpp: competitionThreshold_ parameter
 */
constexpr float WORKING_MEMORY_COMPETITION_THRESHOLD = 0.3f;  // Competition threshold for working memory

/**
 * Neural working memory capacity formula
 * From Brain.cpp: pImpl->workingMemory->setCapacity(neuronCount / 10)
 */
constexpr float WORKING_MEMORY_CAPACITY_FACTOR = 0.1f;  // Working memory is 10% of total neurons

// =====================================
// DEVELOPMENT CONSTANTS
// =====================================

/**
 * Structural plasticity rates
 * From main.cpp: synaptogenesis_rate = 0.0001f, pruning_rate = 0.00001f
 * From default.cfg: development_synaptogenesis_rate = 0.001, development_pruning_rate = 0.0001
 * From Brain.cpp: synaptogenesisRate = 0.0001f, pruningRate = 0.00001f
 * From AgentBrain.cpp: synRate = 0.0001f * plasticityModifier_
 * From Development.cpp: Synaptogenesis.cpp: formationRate(0.001f), targetDensity(0.1f)
 * From Pruning.cpp: threshold(0.01f), pruningRate(0.0001f)
 */
constexpr float SYNAPTAGENESIS_RATE_MAIN = 0.0001f;    // From main.cpp
constexpr float PRUNING_RATE_MAIN = 0.00001f;        // From main.cpp
constexpr float SYNAPTAGENESIS_RATE_DEFAULT = 0.001f; // From default.cfg
constexpr float PRUNING_RATE_DEFAULT = 0.0001f;      // From default.cfg
constexpr float SYNAPTAGENESIS_RATE_CONFIG = 0.0001f; // From Brain.cpp
constexpr float PRUNING_RATE_CONFIG = 0.00001f;      // From Brain.cpp
constexpr float SYNAPTAGENESIS_RATE_PHASE6 = 0.001f;  // From Synaptogenesis.cpp
constexpr float PRUNING_THRESHOLD_PHASE6 = 0.01f;     // From Pruning.cpp
constexpr float PRUNING_RATE_PHASE6 = 0.0001f;       // From Pruning.cpp

/**
 * Development stages and timing
 * From AgentBrain.cpp: if (developmentalAge_ < 60.0), else if (developmentalAge_ < 300.0), else if (developmentalAge_ < 900.0)
 */
constexpr double DEVELOPMENT_INITIAL_STAGE_DURATION = 60.0;   // Initial stage duration (seconds)
constexpr double DEVELOPMENT_CRITICAL_PERIOD_DURATION = 240.0; // Critical period duration (seconds)
constexpr double DEVELOPMENT_MATURATION_DURATION = 660.0;      // Maturation stage duration (seconds)

/**
 * Development-related modulation factors
 * From AgentBrain.cpp: plasticityModifier_ = 1.0f, 0.8f, 0.5f, 0.2f
 */
constexpr float INITIAL_PLATENCYITY_MODIFIER = 1.0f;     // High plasticity in initial stage
constexpr float CRITICAL_PERIOD_PLATENCYITY_MODIFIER = 0.8f;  // Reduced plasticity
constexpr float MATURATION_PLATENCYITY_MODIFIER = 0.5f;    // Lower plasticity
constexpr float ADULT_PLATENCYITY_MODIFIER = 0.2f;        // Stable, low plasticity

// =====================================
// PERFORMANCE CONSTANTS
// =====================================

/**
 * Buffer and cache sizes
 * From Brain.cpp: pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true)
 * From ScalingBenchmark.hpp: stepsPerScale(10000), warmupSteps(1000)
 * From AblationSystem.hpp: stepsPerTrial(10000)
 * From Phase6IntegratedExperiment.hpp: maxSteps(10000), neuronCount(1000)
 * From CheckpointSystem.hpp: uint64_t saveIntervalSteps = 10000
 */
constexpr size_t CHECKPOINT_BUFFER_SIZE = 10000;      // Checkpoint buffer size
constexpr size_t PERFORMANCE_WARMUP_STEPS = 1000;    // Warmup steps for benchmarks
constexpr size_t MAX_SIMULATION_STEPS = 10000;        // Maximum simulation steps
constexpr size_t EXPERIMENT_NEURON_COUNT = 1000;      // Experiment neuron count
constexpr size_t CHECKPOINT_SAVE_INTERVAL = 10000;    // Checkpoint save interval

/**
 * Simulation limits
 * From default.cfg: max_simulation_steps = 10000
 * From benchmark.cpp: {100, 1000}, {500, 1000}, {1000, 500}
 * From main.cpp: runPlasticityExperiment for 1000 steps
 * From Environment.cpp: maxSteps(1000)
 */
constexpr size_t MAX_STEPS_PER_TEST = 1000;            // Common test duration
constexpr size_t MAX_NEURONS_PER_REGION = 10000;       // Maximum neurons per region
constexpr size_t MAX_SYNAPSES_PER_NEURON = 100;        // Maximum synapses per neuron
constexpr size_t MAX_TOTAL_SYNAPSES = 1000000;        // Maximum total synapses in brain

/**
 * Threading and parallelism constants
 * From default.cfg: Not specified
 * From Performance.hpp: Not specified
 */
constexpr size_t DEFAULT_THREAD_POOL_SIZE = 4;         // Default thread pool size
constexpr size_t PERFORMANCE_OPTIMIZATION_THRESHOLD = 1000;  // Threshold for optimizations

// =====================================
// SENSOR/MOTOR CONSTANTS
// =====================================

/**
 * Motor output thresholds and factors
 * From AgentBrain.cpp: if (bestActivity < 0.5f) return MotorCommand::Wait
 * From AgentBrain.cpp: float exploreChance = curiosityLevel_ * 0.3f
 * From AgentBrain.cpp: expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward
 * From AgentBrain.cpp: plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f)
 * From AgentBrain.cpp: stdp->setLTPWeight(0.01f * plasticityFactor)
 */
constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;       // Minimum activity for motor command
constexpr float MOTOR_EXPLORATION_FACTOR = 0.3f;       // Curiosity-based exploration factor
constexpr float EXPECTED_REWARD_DECAY_RATE = 0.95f;    // Expected reward exponential decay rate
constexpr float EXPECTED_REWARD_UPDATE_RATE = 0.05f;   // Expected reward update rate
constexpr float PLASTICITY_MODULATION_MIN = 0.1f;       // Minimum plasticity factor
constexpr float PLASTICITY_MODULATION_MAX = 2.0f;       // Maximum plasticity factor

/**
 * Reward prediction error clamping
 * From AgentBrain.cpp: dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f)
 * From default.cfg: dopamine_baseline = 0.1 (but appears twice)
 */
constexpr float DOPAMINE_CLAMP_MIN = -1.0f;            // Minimum dopamine level
constexpr float DOPAMINE_CLAMP_MAX = 1.0f;             // Maximum dopamine level
constexpr float DOPAMINE_BASELINE = 0.1f;              // Default dopamine baseline

/**
 * Neuromodulation constants
 * From AgentBrain.cpp: curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f
 * From AgentBrain.cpp: noveltyLevel_ *= sensoryNoveltyDecay_ (0.99f)
 * From Neuromodulator.cpp: decayRate(0.1f)
 * From Novelty.cpp: level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f)
 * From Curiosity.cpp: , decayRate(0.05f)
 */
constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;        // Curiosity = novelty * 2 + prediction error * 0.5
constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;        // Curiosity = novelty * 2 + prediction error * 0.5
constexpr float NOVELTY_DECAY_RATE = 0.99f;            // Novelty decay rate
constexpr float NOVELTY_THRESHOLD = 0.3f;             // Novelty detection threshold
constexpr float NEUROMODULATOR_DECAY_RATE = 0.1f;      // General neuromodulator decay rate
constexpr float CURIOSITY_DECAY_RATE = 0.05f;          // Curiosity decay rate

// =====================================
// VISUALIZATION/STATS CONSTANTS
// =====================================

/**
 * Visualization and logging constants
 * From default.cfg: log_level = INFO, log_to_file = false, log_filename = nlm.log
 * From VisualizationInterface.cpp: // TODO PHASE 2: Implement activity visualization
 * From Phase6IntegratedExperiment.cpp: if (step % 1000 == 0)
 * From Performance.hpp: Not specified
 */
constexpr float STAT_UPDATE_INTERVAL = 100.0f;         // How often stats are updated
constexpr size_t STAT_SAMPLE_SIZE = 100;              // Sample size for statistics
constexpr float VISUALIZATION_UPDATE_RATE = 30.0f;    // Visualization update rate (Hz)

/**
 * Learning and activity thresholds
 * From main.cpp: if (std::abs(eligibility) > 0.001f)
 * From NeuralPrediction.cpp: if (std::abs(eligibility) > 0.001f)
 * From Synapse.cpp: if (std::abs(pImpl->eligibilityTrace) < 0.001f)
 * From development/Synaptogenesis.cpp: formationRate(0.001f)
 * From plasticity/StructuralPlasticity.cpp: activityThreshold(0.001f)
 * From memory/NeuralWorkingMemory.cpp: if (totalActivity < 0.001f)
 * From main.cpp: learningOccurred = (std::abs(finalMean - initialMean) > 0.001f)
 * From main.cpp: if (totalDelta > 0.001f) or if (totalDelta < -0.001f)
 * From AgentBrain.cpp: if (std::abs(eligibility) > 0.001f)
 */
constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;  // Minimum eligibility trace to apply
constexpr float SYNAPTIC_FORMATION_RATE = 0.001f;      // Synaptogenesis rate
constexpr float ACTIVITY_THRESHOLD_STRUCTURE = 0.001f; // Activity threshold for structural plasticity
constexpr float WORKING_MEMORY_ACTIVITY_THRESHOLD_DETAILED = 0.001f;  // Working memory activity threshold
constexpr float LEARNING_THRESHOLD = 0.01f;            // Minimum weight change for learning

// =====================================
// BIOMIMETIC/PARAMETERIZED CONSTANTS
// =====================================

/**
 * Biologically realistic parameters
 * From brain/Neuron.cpp: TimestepDuration dt = 0.001;  // 1ms default
 * From performance/SIMDVectorization.hpp: float leakCoeff = dt / tau;  // (dt * 1000) factor handled in caller
 * From brain/Neuron.cpp: V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
 * From plasticity/Hebbian.cpp: maxWeight(1.0f), minWeight(-1.0f), learningRate(0.01f)
 * From plasticity/STDP.cpp: ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f)
 * From world/SimpleWorld.cpp: , energyDecayRate_(0.01f)
 * From world/SimpleWorld.cpp: agent_.x = width_ - 0.01f; agent_.velocityX = 0.0f;
 * From world/SimpleWorld.cpp: agent_.y = height_ - 0.01f; agent_.velocityY = 0.0f;
 * From development/DevelopmentSystem.cpp: age_ += 0.001;  // Approximate timestep
 */
constexpr float NEURON_TIME_CONSTANT = 20.0f;           // Membrane time constant (ms)
constexpr float LEAK_COEFFICIENT_DIVISOR = 1000.0f;   // dt * 1000 factor
constexpr float SYNAPTIC_WEIGHT_MAX = 1.0f;            // Maximum synaptic weight
constexpr float SYNAPTIC_WEIGHT_MIN = -1.0f;           // Minimum synaptic weight
constexpr float WORLD_AGENT_POSITION_MARGIN = 0.01f; // Agent position margin from walls
constexpr float WORLD_ENERGY_DECAY_RATE = 0.01f;      // World energy decay rate
constexpr float DEVELOPMENT_AGE_INCREMENT = 0.001;   // Development age increment per timestep

/**
 * Structural plasticity bounds
 * From plasticity/StructuralPlasticity.cpp: minWeightThreshold(0.05f), activityThreshold(0.001f)
 * From memory/NeuralWorkingMemory.cpp: competitionThreshold_ parameter (0.3f)
 */
constexpr float MIN_WEIGHT_THRESHOLD_STRUCTURE = 0.05f;  // Minimum weight for structural plasticity
constexpr float ACTIVITY_THRESHOLD_STRUCTURE_DETAILED = 0.001f;  // Activity threshold for structural plasticity

// =====================================
// CONFIGURATION/PARAMETER CONSTANTS
// =====================================

/**
 * Default configuration values (from default.cfg)
 * From default.cfg: random_seed = 42
 * From default.cfg: simulation_timestep = 0.001
 * From default.cfg: neuron_count = 1000
 * From default.cfg: region_count = 2
 * From default.cfg: connection_probability = 0.1
 * From default.cfg: plasticity_learning_rate = 0.01
 * From default.cfg: stdp_ltp_weight = 0.01
 * From default.cfg: stdp_ltd_weight = 0.012
 * From default.cfg: stdp_time_constant = 20.0
 * From default.cfg: development_synaptogenesis_rate = 0.001
 * From default.cfg: development_pruning_rate = 0.0001
 * From default.cfg: reward_discount_factor = 0.99
 * From default.cfg: environment_width = 10
 * From default.cfg: environment_height = 10
 * From default.cfg: log_level = INFO
 * From default.cfg: log_to_file = false
 * From default.cfg: log_filename = nlm.log
 * From default.cfg: max_simulation_steps = 10000
 * From default.cfg: simulation_time_limit = 0.0
 * From default.cfg: visualization_enabled = false
 * From default.cfg: visualization_update_rate = 30.0
 */
constexpr int64_t CONFIG_RANDOM_SEED = 42;
constexpr double CONFIG_SIMULATION_TIMESTEP = 0.001;
constexpr size_t CONFIG_NEURON_COUNT = 1000;
constexpr size_t CONFIG_REGION_COUNT = 2;
constexpr float CONFIG_CONNECTION_PROBABILITY = 0.1f;
constexpr float CONFIG_PLASTICITY_LEARNING_RATE = 0.01f;
constexpr float CONFIG_STDP_LTP_WEIGHT = 0.01f;
constexpr float CONFIG_STDP_LTD_WEIGHT = 0.012f;
constexpr float CONFIG_STDP_TIME_CONSTANT = 20.0f;
constexpr float CONFIG_DEVELOPMENT_SYNAPTAGENESIS_RATE = 0.001f;
constexpr float CONFIG_DEVELOPMENT_PRUNING_RATE = 0.0001f;
constexpr float CONFIG_REWARD_DISCOUNT_FACTOR = 0.99f;
constexpr int CONFIG_ENVIRONMENT_WIDTH = 10;
constexpr int CONFIG_ENVIRONMENT_HEIGHT = 10;
constexpr int CONFIG_MAX_SIMULATION_STEPS = 10000;
constexpr double CONFIG_SIMULATION_TIME_LIMIT = 0.0;
constexpr bool CONFIG_VISUALIZATION_ENABLED = false;
constexpr float CONFIG_VISUALIZATION_UPDATE_RATE = 30.0f;

} // namespace nlm
