# NLM Configuration and Constants - Core System Parameters

This document defines the core constants and configuration parameters for the NLM (Neural Learning Machine) system. These values provide the foundation for neural simulation, learning dynamics, and system behavior.

## Neural Parameters

### Neuron Dynamics
- **RESTING_POTENTIAL**: -70.0 mV (default resting potential)
- **THRESHOLD**: -55.0 mV (action potential threshold)
- **RESET_POTENTIAL**: -70.0 mV (post-spike reset)
- **LEAK_CONDUCTANCE**: 10.0 nS (membrane leak conductance)
- **REFRACTORY_PERIOD**: 5 steps (minimum 5ms at 1ms timestep)
- **MEMBRANE_CAPACITANCE**: 1.0 nF (membrane capacitance)
- **TIME_CONSTANT**: 20.0 ms (membrane time constant)

### Synaptic Properties
- **SYNAPTIC_WEIGHT_MIN**: -1.0 (minimum synaptic weight)
- **SYNAPTIC_WEIGHT_MAX**: 1.0 (maximum synaptic weight)
- **DEFAULT_SYNAPTIC_DELAY**: 1 step (default synaptic delay)
- **MAX_SYNAPTIC_DELAY**: 5 steps (maximum synaptic delay)

## Plasticity Parameters

### STDP (Spike-Timing Dependent Plasticity)
- **STDP_LTP_WEIGHT**: 0.01 (long-term potentiation weight)
- **STDP_LTD_WEIGHT**: 0.012 (long-term depression weight)
- **STDP_TAU**: 20.0 ms (STDP time constant)
- **STDP_DECAY_RATE**: 0.001 (STDP eligibility trace decay)

### Hebbian Learning
- **HEBBIAN_LEARNING_RATE**: 0.001 (Hebbian learning rate)

### Structural Plasticity
- **SYNAPTOGENESIS_RATE**: 0.0001 (new synapse formation rate)
- **PRUNING_RATE**: 0.00001 (synapse elimination rate)

## Neuromodulation Parameters

### Dopamine (Reward Prediction Error)
- **DOPAMINE_GAIN**: 1.0 (dopamine modulation strength)
- **DOPAMINE_DECAY**: 0.1 (dopamine decay rate)
- **PLASTICITY_FACTOR_MIN**: 0.1 (minimum plasticity factor)
- **PLASTICITY_FACTOR_MAX**: 2.0 (maximum plasticity factor)

### Curiosity and Novelty
- **CURiosity_SENSITIVITY**: 0.5 (curiosity response to novelty)
- **NOVELTY_THRESHOLD**: 0.1 (novelty detection threshold)
- **NOVELTY_DECAY**: 0.99 (novelty decay rate)

## Memory System Parameters

### Working Memory
- **WORKING_MEMORY_CAPACITY**: 100 (maximum memory traces)
- **WORKING_MEMORY_DECAY_RATE**: 0.01 (memory decay rate)
- **WORKING_MEMORY_REHEARSAL_STRENGTH**: 1.0 (memory reinforcement strength)

### Episodic Memory
- **EPISODIC_MEMORY_MAX_EPISODES**: 1000 (maximum stored episodes)
- **EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD**: 0.3 (consolidation threshold)
- **EPISODIC_MEMORY_REPLAY_INTERVAL**: 100 steps (memory replay frequency)

### Associative Memory
- **ASSOCIATIVE_MEMORY_PATTERN_THRESHOLD**: 0.5 (pattern matching threshold)
- **ASSOCIATIVE_MEMORY_STRENGTH_DECAY**: 0.001 (strength decay rate)

## Network Architecture Parameters

### Network Topology
- **DEFAULT_NEURON_COUNT**: 1000 (default number of neurons)
- **DEFAULT_REGION_COUNT**: 1 (default number of brain regions)
- **DEFAULT_CONNECTION_PROBABILITY**: 0.1 (default connection probability)
- **DEFAULT_INITIAL_WEIGHT_MEAN**: 0.5 (mean initial synaptic weight)
- **DEFAULT_INITIAL_WEIGHT_STD**: 0.1 (standard deviation of initial weights)

### Population Distribution
- **SENSORY_NEURON_FRACTION**: 0.25 (fraction of sensory neurons)
- **INTERNAL_NEURON_FRACTION**: 0.5 (fraction of internal neurons)
- **MOTOR_NEURON_FRACTION**: 0.25 (fraction of motor neurons)

## Simulation Parameters

### Time and Steps
- **DEFAULT_TIMESTEP**: 0.001 (simulation timestep in seconds)
- **DEFAULT_SIMULATION_DURATION**: 10000 (default simulation duration in steps)
- **CHECKPOINT_INTERVAL**: 100 (checkpoint save frequency)
- **MAX_CHECKPOINTS**: 10 (maximum number of checkpoints)

### Development and Aging
- **DEVELOPMENTAL_STAGE_TRANSITION_DURATION**: 1000 (duration per developmental stage in steps)
- **CRITICAL_PERIOD_START**: 60.0 (critical period start age in simulation time)
- **CRITICAL_PERIOD_END**: 300.0 (critical period end age in simulation time)
- **AGING_START**: 900.0 (aging period start age in simulation time)

## Action System Parameters

### Motor Actions
- **NUM_MOTOR_ACTIONS**: 6 (number of available motor actions)
- **ACTION_ENCODING_RESOLUTION**: 6 (motor neuron distribution per action)
- **ACTION_SELECTION_THRESHOLD**: 0.5 (minimum activity for action selection)

### Action Types
- **ACTION_MOVE_FORWARD**: 0 (move forward action)
- **ACTION_MOVE_BACKWARD**: 1 (move backward action)
- **ACTION_TURN_LEFT**: 2 (turn left action)
- **ACTION_TURN_RIGHT**: 3 (turn right action)
- **ACTION_INTERACT**: 4 (interact action)
- **ACTION_WAIT**: 5 (wait/idle action)

## Environment Parameters

### World Simulation
- **DEFAULT_WORLD_WIDTH**: 20.0 (default world width in simulation units)
- **DEFAULT_WORLD_HEIGHT**: 20.0 (default world height in simulation units)
- **DEFAULT_VISION_WIDTH**: 8 (default vision field width)
- **DEFAULT_VISION_HEIGHT**: 8 (default vision field height)
- **DEFAULT_AGENT_START_X**: 10.0 (default agent start X position)
- **DEFAULT_AGENT_START_Y**: 10.0 (default agent start Y position)
- **WORLD_UPDATE_RATE**: 0.1 (world update rate in seconds)

### Sensory Systems
- **VISION_SENSITIVITY**: 1.0 (vision sensitivity factor)
- **TOUCH_SENSITIVITY**: 8.0 (touch sensitivity factor)
- **INTERNAL_SENSITIVITY**: 2.0 (internal state sensitivity factor)
- **PROPRIOCEPTION_SENSITIVITY**: 3.0 (proprioception sensitivity factor)

## Performance and Optimization

### SIMD Configuration
- **SIMD_VECTOR_WIDTH**: 16 (maximum SIMD vector width for AVX-512)
- **SIMD_FALLBACK_WIDTH**: 1 (scalar fallback width)
- **MEMORY_POOL_ALIGNMENT**: 64 (memory pool alignment in bytes)
- **EVENT_QUEUE_SIZE**: 1024 (event queue size)

### Processing Parameters
- **PARALLEL_PROCESSING_ENABLED**: true (enable parallel processing)
- **MULTITHREADING_ENABLED**: true (enable multithreading)
- **THREAD_COUNT**: 4 (number of worker threads)
- **CACHE_OPTIMIZATION_ENABLED**: true (enable cache optimization)

## Learning and Adaptation

### Reinforcement Learning
- **REWARD_MAX_MAGNITUDE**: 10.0 (maximum reward magnitude)
- **REWARD_MIN_MAGNITUDE**: -10.0 (minimum reward magnitude)
- **PREDICTION_ERROR_GAIN**: 0.5 (prediction error weighting)
- **LEARNING_RATE**: 0.001 (global learning rate)

### Curiosity-Driven Learning
- **EXPLORATION_RATE**: 0.3 (exploration rate based on curiosity)
- **NOVELTY_SENSITIVITY**: 0.5 (sensitivity to novel experiences)
- **EXPLORATION_BONUS**: 1.0 (bonus for exploration)

## Checkpoint and Persistence

### Save/Load Parameters
- **CHECKPOINT_COMPRESSION**: 1 (compression level for checkpoints)
- **CHECKPOINT_ENCRYPTION_ENABLED**: false (enable checkpoint encryption)
- **MAX_CHECKPOINT_SIZE**: 10000 (maximum checkpoint size in MB)
- **CHECKPOINT_BACKUP_ENABLED**: true (enable checkpoint backup)

## Logging and Monitoring

### Log Levels
- **LOG_LEVEL_DEBUG**: 0 (debug logging level)
- **LOG_LEVEL_INFO**: 1 (info logging level)
- **LOG_LEVEL_WARNING**: 2 (warning logging level)
- **LOG_LEVEL_ERROR**: 3 (error logging level)
- **LOG_LEVEL_CRITICAL**: 4 (critical logging level)

### Logging Parameters
- **LOG_FILE_MAX_SIZE**: 10000000 (maximum log file size in bytes)
- **LOG_RETENTION_DAYS**: 30 (log file retention period)
- **LOG_CONSOLE_ENABLED**: true (enable console logging)
- **LOG_FILE_ENABLED**: true (enable file logging)

## Configuration File Parameters

### Default Configuration File
- **DEFAULT_CONFIG_FILE**: "configs/default.cfg" (default configuration filename)
- **CONFIG_FILE_FORMAT**: "key=value" (configuration file format)
- **CONFIG_COMMENT_CHAR**: "#" (configuration comment character)
- **CONFIG_SECTION_SEPARATOR**: "[" (configuration section separator)

## System Limits and Constraints

### Resource Limits
- **MAX_REGIONS**: 100 (maximum number of brain regions)
- **MAX_POPULATIONS_PER_REGION**: 50 (maximum populations per region)
- **MAX_NEURONS_PER_POPULATION**: 10000 (maximum neurons per population)
- **MAX_SYNAPSES_PER_REGION**: 1000000 (maximum synapses per region)

### Performance Limits
- **MAX_SIMULATION_STEPS**: 1000000 (maximum simulation steps)
- **MAX_WALL_CLOCK_TIME**: 3600.0 (maximum wall clock time in seconds)
- **MAX_MEMORY_USAGE**: 8192 (maximum memory usage in MB)

## Validation Parameters

### State Validation
- **NEURON_STATE_VALIDITY_CHECK**: true (validate neuron state)
- **SYNAPSE_STATE_VALIDITY_CHECK**: true (validate synapse state)
- **MEMORY_STATE_VALIDITY_CHECK**: true (validate memory state)
- **CONFIGURATION_VALIDITY_CHECK**: true (validate configuration)

### Simulation Validation
- **STEP_VALIDATION_ENABLED**: true (validate each simulation step)
- **CONNECTION_VALIDATION_ENABLED**: true (validate network connections)
- **CONSISTENCY_VALIDATION_ENABLED**: true (validate system consistency)

## Version and Compatibility

### Version Information
- **SYSTEM_VERSION_MAJOR**: 1 (major version)
- **SYSTEM_VERSION_MINOR**: 0 (minor version)
- **SYSTEM_VERSION_PATCH**: 0 (patch version)
- **COMPATIBLE_API_VERSION**: "1.0" (compatible API version)

### Compatibility Settings
- **BACKWARD_COMPATIBILITY_ENABLED**: true (enable backward compatibility)
- **FORWARD_COMPATIBILITY_ENABLED**: true (enable forward compatibility)
- **VERSION_MISMATCH_ACTION**: "error" (action on version mismatch)