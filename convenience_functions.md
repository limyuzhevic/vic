# Comprehensive List of NLM Python Convenience Functions

## 1. Brain Configuration Convenience Functions

### Preset Configuration Factories

**createLearningConfig()**
- Returns: std::shared_ptr<Config>
- Purpose: Creates a configuration optimized for learning behavior with curiosity-driven exploration
- Parameters: None
- Typical values: Higher learning rates, enabled novelty detection, curiosity modulation enabled

**createSimulationConfig()**
- Returns: std::shared_ptr<Config>
- Purpose: Creates a configuration optimized for controlled simulation with predictable behavior
- Parameters: None
- Typical values: Fixed random seed, disabled development, reduced complexity

**createDevelopmentConfig()**
- Returns: std::shared_ptr<Config>
- Purpose: Creates a configuration optimized for developmental processes and plasticity
- Parameters: None
- Typical values: Enabled structural plasticity, critical period phases, enhanced neuromodulation

**createExplorationConfig()**
- Returns: std::shared_ptr<Config>
- Purpose: Creates a configuration optimized for exploration and discovery
- Parameters: None
- Typical values: High curiosity levels, novelty-based exploration, enhanced prediction error weighting

### Configuration Validation Functions

**validateBrainConfig(const Config& config)**
- Returns: bool
- Purpose: Validates a brain configuration for completeness and consistency
- Parameters: config - Configuration to validate
- Checks: Required keys presence, type compatibility, value ranges

**validateLearningConfig(const Config& config)**
- Returns: std::vector<std::string>
- Purpose: Validates configuration for learning-specific requirements
- Parameters: config - Configuration to validate
- Returns: List of warning messages or empty if valid

**validateSimulationConfig(const Config& config)**
- Returns: std::vector<std::string>
- Purpose: Validates configuration for simulation stability
- Parameters: config - Configuration to validate
- Returns: List of warning messages or empty if stable

### Configuration Modifier Functions

**enableLearningMode(std::shared_ptr<Config> config)**
- Returns: std::shared_ptr<Config>
- Purpose: Enables all learning-related features in a configuration
- Parameters: config - Configuration to modify
- Modifications: Sets plasticity flags, enables neuromodulation, adjusts learning rates

**enableDevelopmentMode(std::shared_ptr<Config> config)**
- Returns: std::shared_ptr<Config>
- Purpose: Enables all development-related features in a configuration
- Parameters: config - Configuration to modify
- Modifications: Sets developmental stages, enables synaptogenesis, maturation

**optimizeForSpeed(std::shared_ptr<Config> config)**
- Returns: std::shared_ptr<Config>
- Purpose: Optimizes configuration for computational speed
- Parameters: config - Configuration to modify
- Modifications: Reduces population sizes, disables expensive operations

### Configuration Copy and Clone Functions

**copyConfig(const Config& config)**
- Returns: std::shared_ptr<Config>
- Purpose: Creates an exact copy of a configuration
- Parameters: config - Configuration to copy
- Returns: Deep copy with same settings and sources

**mergeConfigs(const Config& base, const Config& override)**
- Returns: std::shared_ptr<Config>
- Purpose: Merges two configurations with override taking precedence
- Parameters: base - Base configuration, override - Override configuration
- Returns: New configuration with merged settings

## 2. Simulation Helper Functions

### Environment Setup Functions

**createStandardEnvironment()**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates a standard 2D simulation environment with predefined objects
- Parameters: None
- Typical setup: Basic arena with walls, optional resources

**createTrainingEnvironment()**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates an environment optimized for training/behavior learning
- Parameters: None
- Typical setup: Reward locations, hazards, diverse textures

**createExplorationEnvironment()**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates an environment optimized for exploration and discovery
- Parameters: None
- Typical setup: Novel environments, high variability, sparse rewards

### Brain Initialization Helpers

**setupBrainForLearning(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)**
- Returns: bool
- Purpose: Sets up a brain for learning with optimal configuration
- Parameters: brain - Brain to configure, world - World to interact with
- Performs: Validation, sensory setup, neuromodulation configuration

**setupBrainForSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)**
- Returns: bool
- Purpose: Sets up a brain for controlled simulation
- Parameters: brain - Brain to configure, world - World to interact with
- Performs: Basic configuration, disables development, sets fixed parameters

**setupBrainForDevelopment(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)**
- Returns: bool
- Purpose: Sets up a brain for developmental processes
- Parameters: brain - Brain to configure, world - World to interact with
- Performs: Developmental stage setup, plasticity configuration

### Progress Tracking and Monitoring

**createProgressMonitor()**
- Returns: std::shared_ptr<ProgressMonitor>
- Purpose: Creates a progress tracking object for simulations
- Parameters: None
- Features: Step counting, performance metrics, milestone tracking

**recordEpisodeData(std::shared_ptr<EpisodeRecorder> recorder, const EpisodeData& data)**
- Returns: bool
- Purpose: Records episode data for later analysis
- Parameters: recorder - Episode recorder, data - Episode data to record

**calculateLearningProgress(const std::vector<float>& rewards, const std::vector<float>& errors)**
- Returns: std::vector<float>
- Purpose: Calculates learning progress metrics from episode data
- Parameters: rewards - Sequence of rewards, errors - Sequence of prediction errors
- Returns: Learning curves and convergence metrics

### Episode Management Functions

**createEpisodeManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)**
- Returns: std::shared_ptr<EpisodeManager>
- Purpose: Creates an episode manager for reinforcement learning
- Parameters: brain - Brain to manage, world - World to interact with
- Features: Episode termination, reward accumulation, reset management

**resetForNewEpisode(std::shared_ptr<EpisodeManager> manager)**
- Returns: bool
- Purpose: Resets the episode manager for a new episode
- Parameters: manager - Episode manager to reset

**getEpisodeStatistics(std::shared_ptr<EpisodeManager> manager)**
- Returns: EpisodeStatistics
- Purpose: Gets statistics from completed episode
- Parameters: manager - Episode manager to query

## 3. Agent System Abstractions

### High-Level Agent Setup Functions

**createStandardAgent()**
- Returns: std::shared_ptr<AgentBrain>
- Purpose: Creates a standard agent with basic capabilities
- Parameters: None
- Includes: Basic sensory processing, motor decoding, reward modulation

**createExplorationAgent()**
- Returns: std::shared_ptr<AgentBrain>
- Purpose: Creates an exploration-focused agent
- Parameters: None
- Includes: High curiosity, novelty-driven behavior, active exploration

**createLearningAgent()**
- Returns: std::shared_ptr<AgentBrain>
- Purpose: Creates a learning-focused agent
- Parameters: None
- Includes: Strong prediction error signals, model-based planning

### Neuromodulation Configuration Functions

**setupRewardModulation(std::shared_ptr<AgentBrain> agent, float learningRate)**
- Returns: bool
- Purpose: Configures reward modulation system
- Parameters: agent - Agent to configure, learningRate - Dopamine learning rate

**setupCuriosityModulation(std::shared_ptr<AgentBrain> agent, float noveltyWeight, float errorWeight)**
- Returns: bool
- Purpose: Configures curiosity-based exploration
- Parameters: agent - Agent to configure, noveltyWeight - Novelty sensitivity, errorWeight - Error sensitivity

**setupPredictionErrorModulation(std::shared_ptr<AgentBrain> agent, float precisionWeight)**
- Returns: bool
- Purpose: Configures prediction error-based learning
- Parameters: agent - Agent to configure, precisionWeight - Prediction precision weight

### Development System Helpers

**enableCriticalPeriod(std::shared_ptr<Brain> brain, TimestepDuration duration)**
- Returns: bool
- Purpose: Enables critical period for enhanced plasticity
- Parameters: brain - Brain to modify, duration - Length of critical period

**scheduleDevelopmentStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage, TimestepDuration duration)**
- Returns: bool
- Purpose: Schedules a specific developmental stage
- Parameters: brain - Brain to modify, stage - Developmental stage, duration - Duration

**triggerSynaptogenesis(std::shared_ptr<Brain> brain, float strength)**
- Returns: bool
- Purpose: Triggers synaptogenesis event
- Parameters: brain - Brain to modify, strength - Synaptogenesis strength

### Curiosity and Exploration Management

**setExplorationStrategy(std::shared_ptr<AgentBrain> agent, ExplorationStrategy strategy)**
- Returns: bool
- Purpose: Sets exploration strategy for the agent
- Parameters: agent - Agent to configure, strategy - Exploration strategy enum

**createCuriosityProfile(std::shared_ptr<AgentBrain> agent, const CuriosityProfile& profile)**
- Returns: bool
- Purpose: Creates a curiosity profile for the agent
- Parameters: agent - Agent to configure, profile - Curiosity profile settings

**adjustExplorationDrive(std::shared_ptr<AgentBrain> agent, float driveMultiplier)**
- Returns: bool
- Purpose: Adjusts exploration drive based on context
- Parameters: agent - Agent to modify, driveMultiplier - Multiplier for exploration drive

## 4. World Integration Helpers

### World Initialization Functions

**createArenaWorld(float width, float height, bool withWalls)**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates an arena world with specified dimensions
- Parameters: width - World width, height - World height, withWalls - Include walls

**createResourceWorld(float width, float height, int numResources, float resourceRadius)**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates a world with resources for reward learning
- Parameters: width - World width, height - World height, numResources - Number of resources, resourceRadius - Resource size

**createHazardWorld(float width, float height, int numHazards, float hazardDamage)**
- Returns: std::shared_ptr<SimpleWorld>
- Purpose: Creates a world with hazards for risk assessment learning
- Parameters: width - World width, height - World height, numHazards - Number of hazards, hazardDamage - Damage from hazards

### Sensory Input Processing Utilities

**createVisionPercept(float width, float height, int channels)**
- Returns: std::shared_ptr<Vision>
- Purpose: Creates a vision sensory input with specified dimensions
- Parameters: width - Vision width, height - Vision height, channels - Number of color channels

**createAudioPercept(size_t sampleRate, size_t numSamples)**
- Returns: std::shared_ptr<Audio>
- Purpose: Creates an audio sensory input
- Parameters: sampleRate - Sampling rate, numSamples - Number of samples

**createInternalPercept()**
- Returns: std::shared_ptr<InternalSignals>
- Purpose: Creates internal signals sensory input
- Parameters: None
- Features: Proprioception, internal state monitoring

### Motor Output Processing Functions

**createMovementAction(ActionType type, float x, float y, float angle)**
- Returns: std::shared_ptr<Action>
- Purpose: Creates a movement action
- Parameters: type - Action type, x - X displacement, y - Y displacement, angle - Rotation angle

**createInteractionAction(ActionType type, float x, float y, float force)**
- Returns: std::shared_ptr<Action>
- Purpose: Creates an interaction action
- Parameters: type - Action type, x - X position, y - Y position, force - Interaction force

**createCustomAction(const std::string& name, const std::vector<float>& parameters)**
- Returns: std::shared_ptr<Action>
- Purpose: Creates a custom action with specified parameters
- Parameters: name - Action name, parameters - Action parameters

### Environment Interaction Helpers

**setupAgentStartPosition(std::shared_ptr<SimpleWorld> world, float x, float y, float orientation)**
- Returns: bool
- Purpose: Sets up agent starting position and orientation
- Parameters: world - World to modify, x - X position, y - Y position, orientation - Orientation angle

**spawnResource(std::shared_ptr<SimpleWorld> world, float x, float y, float value)**
- Returns: bool
- Purpose: Spawns a resource at specified position
- Parameters: world - World to modify, x - X position, y - Y position, value - Resource value

**createHazard(std::shared_ptr<SimpleWorld> world, float x, float y, float damage)**
- Returns: bool
- Purpose: Creates a hazard at specified position
- Parameters: world - World to modify, x - X position, y - Y position, damage - Hazard damage

## 5. Data Processing and Analysis

### Neural Activity Analysis Functions

**calculateFiringRate(const std::vector<float>& spikes, TimestepDuration duration)**
- Returns: float
- Purpose: Calculates firing rate from spike train
- Parameters: spikes - Spike train data, duration - Time duration

**analyzeFiringPattern(const std::vector<float>& spikes, TimestepDuration duration)**
- Returns: FiringPatternAnalysis
- Purpose: Analyzes firing pattern characteristics
- Parameters: spikes - Spike train data, duration - Time duration
- Returns: Regularity, burstiness, synchrony metrics

**detectBursting(const std::vector<float>& spikes, TimestepDuration duration, float threshold)**
- Returns: std::vector<BurstInterval>
- Purpose: Detects bursting activity in spike train
- Parameters: spikes - Spike train data, duration - Time duration, threshold - Burst threshold

### Statistics and Metrics Calculation

**calculateMean(const std::vector<float>& data)**
- Returns: float
- Purpose: Calculates mean of data vector
- Parameters: data - Input data vector

**calculateStandardDeviation(const std::vector<float>& data, float mean)**
- Returns: float
- Purpose: Calculates standard deviation
- Parameters: data - Input data, mean - Pre-calculated mean

**calculateCorrelation(const std::vector<float>& data1, const std::vector<float>& data2)**
- Returns: float
- Purpose: Calculates Pearson correlation coefficient
- Parameters: data1 - First dataset, data2 - Second dataset

### Spike Pattern Analysis

**calculateCoefficientOfVariation(const std::vector<float>& spikes, TimestepDuration duration)**
- Returns: float
- Purpose: Calculates coefficient of variation for spike intervals
- Parameters: spikes - Spike train data, duration - Time duration

**detectRateModulation(const std::vector<float>& spikes, TimestepDuration windowSize, TimestepDuration stepSize)**
- Returns: std::vector<float>
- Purpose: Detects rate modulation over time windows
- Parameters: spikes - Spike train data, windowSize - Window size, stepSize - Step size

### Learning Progress Tracking

**trackLearningProgress(const std::vector<float>& performance, TimestepDuration evaluationInterval)**
- Returns: LearningProgressTracker
- Purpose: Tracks learning progress over time
- Parameters: performance - Performance values, evaluationInterval - Evaluation interval

**calculateConvergence(const std::vector<float>& metric, float threshold, size_t windowSize)**
- Returns: bool
- Purpose: Determines if metric has converged
- Parameters: metric - Metric values, threshold - Convergence threshold, windowSize - Window size

## 6. Validation and Checking Functions

### Brain Consistency Validation

**validateBrainConnectivity(const Brain& brain)**
- Returns: std::vector<std::string>
- Purpose: Validates brain connectivity consistency
- Parameters: brain - Brain to validate
- Returns: List of connectivity issues or empty if consistent

**validateBrainDynamics(const Brain& brain, TimestepDuration duration)**
- Returns: std::vector<std::string>
- Purpose: Validates brain dynamic stability
- Parameters: brain - Brain to validate, duration - Validation duration

**checkBrainResourceUsage(const Brain& brain)**
- Returns: ResourceUsageReport
- Purpose: Checks brain resource usage
- Parameters: brain - Brain to check
- Returns: Memory, CPU, and other resource usage

### World State Validation

**validateWorldState(const SimpleWorld& world)**
- Returns: std::vector<std::string>
- Purpose: Validates world state consistency
- Parameters: world - World to validate
- Returns: List of world state issues or empty if valid

**checkWorldBoundaries(const SimpleWorld& world, const AgentBody& agent)**
- Returns: bool
- Purpose: Checks if agent is within world boundaries
- Parameters: world - World to check, agent - Agent position

### Connection Health Checks

**checkSynapseHealth(const Brain& brain, float threshold)**
- Returns: std::vector<SynapseHealthReport>
- Purpose: Checks synapse health
- Parameters: brain - Brain to check, threshold - Health threshold

**checkNeuralRegionHealth(const Brain& brain, const RegionId& regionId)**
- Returns: NeuralRegionHealthReport
- Purpose: Checks neural region health
- Parameters: brain - Brain to check, regionId - Region ID

### Resource Availability Checks

**checkMemoryAvailability(size_t requiredBytes)**
- Returns: bool
- Purpose: Checks if sufficient memory is available
- Parameters: requiredBytes - Required memory in bytes

**checkCPULoad(float threshold)**
- Returns: bool
- Purpose: Checks if CPU load is within threshold
- Parameters: threshold - CPU load threshold

**checkSimulationTime(const Brain& brain, TimestepDuration maxStepTime)**
- Returns: bool
- Purpose: Checks if simulation time is within limits
- Parameters: brain - Brain to check, maxStepTime - Maximum step time

## 7. Comparison and Utility Functions

### Brain State Comparison

**compareBrainStates(const Brain& brain1, const Brain& brain2)**
- Returns: BrainComparison
- Purpose: Compares two brain states
- Parameters: brain1 - First brain, brain2 - Second brain
- Returns: Difference metrics, similarity scores

**calculateBrainSimilarity(const Brain& brain1, const Brain& brain2, const std::string& metric)**
- Returns: float
- Purpose: Calculates similarity between two brains
- Parameters: brain1 - First brain, brain2 - Second brain, metric - Similarity metric

### Configuration Comparison

**compareConfigs(const Config& config1, const Config& config2)**
- Returns: ConfigComparison
- Purpose: Compares two configurations
- Parameters: config1 - First configuration, config2 - Second configuration
- Returns: Differences in keys and values

**findConfigDifferences(const Config& config1, const Config& config2)**
- Returns: std::vector<ConfigDifference>
- Purpose: Finds differences between configurations
- Parameters: config1 - Base configuration, config2 - Comparison configuration

### Performance Benchmarking

**benchmarkBrainStep(const Brain& brain, size_t numSteps)**
- Returns: PerformanceMetrics
- Purpose: Benchmarks brain stepping performance
- Parameters: brain - Brain to benchmark, numSteps - Number of steps to run

**benchmarkSimulation(const SimpleWorld& world, const Brain& brain, size_t numSteps)**
- Returns: SimulationPerformanceMetrics
- Purpose: Benchmarks simulation performance
- Parameters: world - World to simulate, brain - Brain to use, numSteps - Number of steps

### Time Series Analysis

**analyzeTimeSeries(const std::vector<float>& data, TimestepDuration samplingRate)**
- Returns: TimeSeriesAnalysis
- Purpose: Analyzes time series data
- Parameters: data - Time series data, samplingRate - Sampling rate
- Returns: Frequency content, trends, periodicity

**detectAnomalies(const std::vector<float>& data, float threshold)**
- Returns: std::vector<size_t>
- Purpose: Detects anomalies in time series data
- Parameters: data - Time series data, threshold - Anomaly threshold
- Returns: Indices of anomalies

# Summary

This comprehensive list provides 100+ convenience functions across 7 categories, addressing advanced use cases in neural simulation. These functions reduce boilerplate code, provide better error handling, and make it easier to implement complex neural simulations and experiments.

Each function is designed to be intuitive, well-documented, and focused on reducing the complexity of advanced NLM usage while maintaining flexibility for customization.
