# NLM API Documentation

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) system. It covers the public interfaces of all major components, including method signatures, parameters, return values, and usage examples. The API is designed to provide a clean, intuitive interface for controlling and interacting with neural simulations.

## Documentation Standards

### Naming Conventions
- **Methods**: CamelCase (e.g., `initializeBrain()`)
- **Variables**: camelCase (e.g., `simulationTimestep`)
- **Constants**: UPPER_CASE (e.g., `MAX_NEURON_COUNT`)
- **File Names**: snake_case (e.g., `brain_config.cpp`)

### Parameter Documentation
Each parameter is documented with:
- Type (including primitive types and smart pointers)
- Description and purpose
- Units where applicable
- Default values where applicable
- Constraints or valid ranges

### Return Value Documentation
Each return value is documented with:
- Return type
- Description of the returned value
- Special cases or sentinel values
- Thread safety considerations

### Exception Documentation
Exceptions are documented with:
- Exception type
- When the exception is thrown
- Error context information
- Recovery suggestions

## Brain API

The Brain class serves as the central interface for controlling neural simulations.

### Core Brain Management

#### `Brain(std::shared_ptr<Config> config)`

**Constructor**
Creates a new Brain instance with the specified configuration.

**Parameters**
- `config`: Shared pointer to the configuration object containing simulation parameters

**Returns**
- Brain object (moved from temporary)

**Throws**
- `std::invalid_argument` if config is nullptr
- `std::runtime_error` if configuration validation fails

**Example**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 500);
config->set("simulation_timestep", 0.001);

auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

#### `bool initialize()`

**Method**: Initialize the brain with configuration parameters

**Description**
Sets up the neural architecture including regions, neurons, synapses, and all supporting systems (memory, prediction, cognition, neuromodulation). This method must be called before any simulation steps.

**Returns**
- `true` if initialization succeeded
- `false` if initialization failed (e.g., insufficient memory)

**Throws**
- `std::runtime_error` if critical initialization fails

**Example**
```cpp
if (!brain->initialize()) {
    NLM_LOG_ERROR("Brain initialization failed!");
    return 1;
}
```

#### `~Brain()`

**Destructor**
Cleans up brain resources and releases allocated memory.

### Simulation Control

#### `void step(SimulationStep currentStep)`

**Method**: Main simulation step

**Parameters**
- `currentStep`: The current simulation step number (monotonically increasing)

**Description**
Advances the brain state by one simulation step. Updates all neural components including neurons, synapses, and plasticity systems.

**Notes**
- Called by the main simulation loop to drive neural dynamics forward in time
- Updates spike propagation, plasticity, development, and neuromodulation systems

#### `void step(SimulationStep currentStep, Timestamp currentTime)`

**Method**: Advanced simulation step with absolute timing

**Parameters**
- `currentStep`: The current simulation step number
- `currentTime`: The absolute timestamp of this simulation step (in seconds)

**Description**
Version of step() that includes absolute time information, required for time-dependent computations such as spike delays and plasticity updates.

**Notes**
- Provides finer temporal resolution for time-sensitive operations
- Used for precise timing of STDP and other time-dependent processes

### Input and Output Processing

#### `void receiveSensoryInput(const SensoryInput& input)`

**Method**: Receive and process sensory input from the environment

**Parameters**
- `input`: The sensory input containing current injection patterns and timing information for sensory neurons

**Description**
Injects current into sensory neurons based on the input pattern provided by the environment. This drives the brain's response to external stimuli.

**Notes**
- Called by the environment each simulation step to provide input
- Updates sensory neuron membrane potentials
- Triggers neural activity and learning processes

#### `void injectCurrent(NeuronId neuron, MembranePotential current)`

**Method**: Inject current directly into a specific neuron

**Parameters**
- `neuron`: ID of the target neuron to receive current
- `current`: Amount of current to inject (positive for excitatory, negative for inhibitory)

**Description**
Provides external current injection to a single neuron, bypassing synaptic transmission. Used for direct stimulation or experimental control.

#### `void injectCurrentToNeurons(NeuronType type, MembranePotential current)`

**Method**: Inject current into all neurons of a specific type

**Parameters**
- `type`: Type of neurons to receive current (Sensory, Motor, etc.)
- `current`: Amount of current to inject per neuron

**Description**
Provides batch current injection to all neurons of a given type. This is more efficient than individual injections when multiple neurons need the same input.

**Notes**
- Used for creating population-level input patterns
- Applies to all neurons of the specified type across all regions

### Spike System Access

#### `SpikeSystem* getSpikeSystem()`

**Method**: Get access to the spike propagation system

**Returns**
- Pointer to the SpikeSystem instance, or nullptr if not initialized

**Notes**
- The spike system handles event-driven spike propagation with synaptic delays
- Provides access to spike timing and propagation information

#### `const SpikeSystem* getSpikeSystem() const`

**Method**: Get const access to the spike propagation system

**Returns**
- Const pointer to the SpikeSystem instance

### Plasticity System Access

#### `STDP* getSTDP()`

**Method**: Get access to the spike-timing dependent plasticity (STDP) system

**Returns**
- Pointer to the STDP instance for spike-timing dependent learning

#### `Hebbian* getHebbian()`

**Method**: Get access to the Hebbian plasticity system

**Returns**
- Pointer to the Hebbian instance for activity-dependent learning

#### `StructuralPlasticity* getStructuralPlasticity()`

**Method**: Get access to the structural plasticity system

**Returns**
- Pointer to the StructuralPlasticity instance for synapse formation/pruning

### Statistics and Information

#### `float getExcitationInhibitionRatio() const`

**Method**: Calculate the excitation-inhibition balance in the network

**Returns**
- E/I ratio (higher values indicate excitatory dominance, lower values indicate inhibitory dominance)

#### `size_t getTotalSpikeCount() const`

**Method**: Get the total number of spikes generated by the brain

**Returns**
- Total spike count since brain initialization

#### `size_t getPendingSpikeEventCount() const`

**Method**: Get the number of pending spike events awaiting propagation

**Returns**
- Number of spikes in transit to target neurons

**Notes**
- Represents spikes that have been emitted but not yet processed

#### `std::unique_ptr<Action> produceAction()`

**Method**: Produce a motor action based on current neural activity

**Returns**
- Action to execute, or nullptr if no action needed

**Notes**
- Calls the motor system to convert neural activity into behavioral output
- Returns a unique_ptr to ensure proper ownership semantics

### Neuromodulation

#### `void applyNeuromodulation(const Neuromodulator& signal)`

**Method**: Apply neuromodulatory signals

**Parameters**
- `signal`: The neuromodulatory signal to apply (Dopamine, Curiosity, Novelty, etc.)

**Description**
Applies neuromodulatory signals to the brain, affecting plasticity, learning, and behavior.

### System Operations

#### `void updatePlasticity()`

**Method**: Update plasticity rules (called automatically in step)

**Description**
Updates all plasticity rules including STDP, Hebbian, and reward-modulated learning. Called automatically during each simulation step.

#### `void develop()`

**Method**: Apply developmental changes (called automatically in step)

**Description**
Applies developmental plasticity rules including synaptogenesis, pruning, and maturation. Called automatically during each simulation step.

#### `void reset()`

**Method**: Reset brain state

**Description**
Resets the brain to its initial state, clearing all memories and activity patterns.

#### `bool save(const std::string& filepath) const`

**Method**: Save brain state to file (checkpointing)

**Parameters**
- `filepath`: Path to the file to save the brain state

**Returns**
- `true` if save succeeded
- `false` if save failed

#### `bool load(const std::string& filepath)`

**Method**: Load brain state from file

**Parameters**
- `filepath`: Path to the file to load the brain state from

**Returns**
- `true` if load succeeded
- `false` if load failed

### Memory Systems

#### `NeuralWorkingMemory* getWorkingMemory()`

**Method**: Get access to the working memory system

**Returns**
- Pointer to the NeuralWorkingMemory instance for storing active information

#### `NeuralEpisodicMemory* getEpisodicMemory()`

**Method**: Get access to the episodic memory system

**Returns**
- Pointer to the NeuralEpisodicMemory instance for storing experiences

#### `NeuralAssociativeMemory* getAssociativeMemory()`

**Method**: Get access to the associative memory system

**Returns**
- Pointer to the NeuralAssociativeMemory instance for pattern associations

### Prediction System

#### `PredictionSystem* getPredictionSystem()`

**Method**: Get access to the prediction system

**Returns**
- Pointer to the PredictionSystem instance for forward models and error computation

### Cognition Systems

#### `NeuralPlanner* getPlanner()`

**Method**: Get access to the neural planner system

**Returns**
- Pointer to the NeuralPlanner instance for action planning and decision making

#### `ConceptFormation* getConceptFormation()`

**Method**: Get access to the concept formation system

**Returns**
- Pointer to the ConceptFormation instance for discovering patterns and categories

#### `AttentionalSelection* getAttention()`

**Method**: Get access to the attentional selection system

**Returns**
- Pointer to the AttentionalSelection instance for selecting focus areas

### Development System

#### `DevelopmentSystem* getDevelopmentSystem()`

**Method**: Get access to the development system

**Returns**
- Pointer to the DevelopmentSystem instance

#### `DevelopmentalStage getDevelopmentalStage() const`

**Method**: Get the current developmental stage of the brain

**Returns**
- Current developmental stage (e.g., Infant, Adult)

#### `void setDevelopmentalStage(DevelopmentalStage stage)`

**Method**: Set the developmental stage of the brain

**Parameters**
- `stage`: Target developmental stage to set

### Neuromodulation Systems

#### `Dopamine* getDopamine()`

**Method**: Get access to the dopamine neuromodulation system

**Returns**
- Pointer to the Dopamine instance for reward processing and reinforcement

#### `Curiosity* getCuriosity()`

**Method**: Get access to the curiosity neuromodulation system

**Returns**
- Pointer to the Curiosity instance for exploration motivation and intrinsic drive

#### `Novelty* getNovelty()`

**Method**: Get access to the novelty detection system

**Returns**
- Pointer to the Novelty instance for detecting unexpected stimuli

#### `PredictionError* getPredictionErrorSignal()`

**Method**: Get access to the prediction error signal system

**Returns**
- Pointer to the PredictionError instance for computing prediction errors

### Configuration and Resources

#### `std::shared_ptr<const Config> getConfig() const`

**Method**: Get the configuration object used to initialize the brain

**Returns**
- Shared pointer to the configuration object

#### `RandomGenerator* getRandomGenerator()`

**Method**: Get access to the random number generator

**Returns**
- Pointer to the RandomGenerator instance for stochastic operations

#### `void logStatus() const`

**Method**: Log the current status of the brain to the global logger

**Notes**
- Logs key statistics including neuron counts, spike activity, and system status

## Neuron API

The Neuron class provides detailed control over individual neuron properties and behavior.

### Neuron Properties

#### `NeuronId getId() const`

**Method**: Get neuron ID

**Returns**
- Neuron ID

#### `NeuronType getType() const`

**Method**: Get neuron type

**Returns**
- Neuron type (Sensory, Motor, Interneuron, etc.)

#### `void setType(NeuronType type)`

**Method**: Set neuron type

**Parameters**
- `type`: Neuron type to set

#### `const NeuronState& getState() const`

**Method**: Get const reference to neuron state

**Returns**
- Const reference to neuron state

#### `NeuronState& getState()`

**Method**: Get mutable reference to neuron state

**Returns**
- Mutable reference to neuron state

### Membrane Potential Control

#### `MembranePotential getMembranePotential() const`

**Method**: Get membrane potential

**Returns**
- Current membrane potential (mV)

#### `void setMembranePotential(MembranePotential potential)`

**Method**: Set membrane potential

**Parameters**
- `potential`: New membrane potential (mV)

#### `void addToMembranePotential(MembranePotential delta)`

**Method**: Add to membrane potential

**Parameters**
- `delta`: Amount to add to membrane potential (mV)

### Threshold Control

#### `MembranePotential getThreshold() const`

**Method**: Get firing threshold

**Returns**
- Firing threshold (mV)

#### `void setThreshold(MembranePotential threshold)`

**Method**: Set firing threshold

**Parameters**
- `threshold`: New firing threshold (mV)

### Firing State Control

#### `bool isFiring() const`

**Method**: Check if neuron is currently firing

**Returns**
- `true` if neuron is in firing state, `false` otherwise

#### `bool isRefractory() const`

**Method**: Check if neuron is in refractory period

**Returns**

- `true` if neuron is in refractory period, `false` otherwise

#### `void setFiringState(FiringState state)`

**Method**: Set firing state

**Parameters**
- `state`: New firing state

#### `void setRefractoryPeriod(uint32_t steps)`

**Method**: Set refractory period

**Parameters**
- `steps`: Number of steps in refractory period

#### `void decrementRefractory()`

**Method**: Decrement refractory counter

**Description**
Decrements the refractory counter by one step. Called automatically during simulation.

### Firing Rate Control

#### `FiringRate getFiringRate() const`

**Method**: Get firing rate

**Returns**
- Current firing rate (Hz)

#### `void setFiringRate(FiringRate rate)`

**Method**: Set firing rate

**Parameters**
- `rate`: New firing rate (Hz)

### LIF Neuron Parameters

#### `void setLeakConductance(MembranePotential conductance)`

**Method**: Set leak conductance

**Parameters**
- `conductance`: Leak conductance (nS)

#### `MembranePotential getLeakConductance() const`

**Method**: Get leak conductance

**Returns**
- Leak conductance (nS)

#### `uint32_t getRefractoryPeriod() const`

**Method**: Get refractory period

**Returns**
- Refractory period (steps)

#### `void setRestingPotential(MembranePotential potential)`

**Method**: Set resting potential

**Parameters**
- `potential`: Resting potential (mV)

#### `MembranePotential getRestingPotential() const`

**Method**: Get resting potential

**Returns**
- Resting potential (mV)

#### `void setResetPotential(MembranePotential potential)`

**Method**: Set reset potential

**Parameters**
- `potential`: Reset potential (mV)

### Spike Detection

#### `bool checkThreshold() const`

**Method**: Check if neuron has reached firing threshold

**Returns**
- `true` if neuron has reached threshold, `false` otherwise

#### `float getLastSpikeTime() const`

**Method**: Get timestamp of last spike

**Returns**
- Timestamp of last spike (-1 if no spike has occurred)

### LIF Simulation

#### `bool stepLIF(Timestamp currentTime, TimestepDuration dt)`

**Method**: LIF neuron step function

**Parameters**
- `currentTime`: Current simulation time (seconds)
- `dt`: Time step duration (seconds)

**Returns**
- `true` if neuron fired during this step, `false` otherwise

#### `void step(Timestamp currentTime)`

**Method**: Update neuron for one simulation step

**Parameters**
- `currentTime`: Current simulation time (seconds)

**Notes**
- TODO PHASE 2: Implement real integrate-and-fire dynamics

### Input and Output

#### `void receiveExcitatoryInput(MembranePotential amplitude)`

**Method**: Receive excitatory input

**Parameters**
- `amplitude`: Synaptic input amplitude (mV)

#### `void receiveInhibitoryInput(MembranePotential amplitude)`

**Method**: Receive inhibitory input

**Parameters**
- `amplitude`: Synaptic input amplitude (mV)

#### `void receiveModulatoryInput(MembranePotential amplitude)`

**Method**: Receive modulatory input

**Parameters**
- `amplitude`: Modulatory input amplitude (mV)

#### `void injectCurrent(MembranePotential current)`

**Method**: Inject current into neuron

**Parameters**
- `current`: Current to inject (mV)

#### `MembranePotential getTotalCurrent() const`

**Method**: Get total injected current

**Returns**
- Total injected current (mV)

#### `void clearTotalCurrent()`

**Method**: Clear total injected current

### Spike History

#### `void recordSpike(Timestamp timestamp)`

**Method**: Record a spike event

**Parameters**
- `timestamp`: Time when spike occurred

#### `const std::vector<Timestamp>& getSpikeHistory() const`

**Method**: Get spike history

**Returns**
- Const reference to spike history vector

#### `void clearSpikeHistory()`

**Method**: Clear spike history

### Synapse Management

#### `void addIncomingSynapse(SynapseHandle handle)`

**Method**: Add incoming synapse

**Parameters**
- `handle`: Synapse handle for incoming connection

#### `void addOutgoingSynapse(SynapseHandle handle)`

**Method**: Add outgoing synapse

**Parameters**
- `handle`: Synapse handle for outgoing connection

#### `const std::vector<SynapseHandle>& getIncomingSynapses() const`

**Method**: Get incoming synapses

**Returns**
- Const reference to incoming synapses vector

#### `const std::vector<SynapseHandle>& getOutgoingSynapses() const`

**Method**: Get outgoing synapses

**Returns**
- Const reference to outgoing synapses vector

### Plasticity Control

#### `const PlasticityFlags& getPlasticityFlags() const`

**Method**: Get plasticity flags

**Returns**
- Const reference to plasticity flags

#### `PlasticityFlags& getPlasticityFlags()`

**Method**: Get mutable reference to plasticity flags

**Returns**
- Mutable reference to plasticity flags

#### `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)`

**Method**: Enable/disable plasticity rules

**Parameters**
- `hebbian`: Enable Hebbian plasticity
- `stdp`: Enable STDP
- `rewardModulated`: Enable reward-modulated plasticity

### Region Membership

#### `void setRegionId(RegionId region)`

**Method**: Set neuron region ID

**Parameters**
- `region`: Region ID

#### `RegionId getRegionId() const`

**Method**: Get neuron region ID

**Returns**
- Region ID

#### `void setPopulationId(PopulationId population)`

**Method**: Set neuron population ID

**Parameters**
- `population`: Population ID

#### `PopulationId getPopulationId() const`

**Method**: Get neuron population ID

**Returns**
- Population ID

### Simulation Control

#### `void step(Timestamp currentTime)`

**Method**: Update neuron for one simulation step

**Parameters**
- `currentTime`: Current simulation time (seconds)

#### `void reset()`

**Method**: Reset neuron to initial state

#### `void initializeRandom(RandomGenerator& rng)`

**Method**: Initialize neuron with random parameters

**Parameters**
- `rng`: Random number generator reference

## Synapse API

The Synapse class provides detailed control over synaptic properties and transmission.

### Synapse Properties

#### `SynapseId getId() const`

**Method**: Get synapse ID

**Returns**
- Synapse ID

#### `NeuronId getSourceNeuron() const`

**Method**: Get source neuron ID

**Returns**
- Source neuron ID

#### `NeuronId getDestinationNeuron() const`

**Method**: Get destination neuron ID

**Returns**
- Destination neuron ID

### Weight Control

#### `SynapticWeight getWeight() const`

**Method**: Get synaptic weight

**Returns**
- Synaptic weight (dimensionless, typically 0-1)

#### `void setWeight(SynapticWeight weight)`

**Method**: Set synaptic weight

**Parameters**
- `weight`: New synaptic weight (dimensionless, typically 0-1)

#### `void addToWeight(SynapticWeight delta)`

**Method**: Add to synaptic weight

**Parameters**
- `delta`: Amount to add to weight

### Delay Control

#### `Delay getDelay() const`

**Method**: Get synaptic delay

**Returns**
- Delay in simulation steps

#### `void setDelay(Delay delay)`

**Method**: Set synaptic delay

**Parameters**
- `delay`: New delay (steps)

### Synapse Type

#### `SynapseType getType() const`

**Method**: Get synapse type

**Returns**
- Synapse type (Excitatory, Inhibitory, Modulatory)

#### `void setType(SynapseType type)`n

**Method**: Set synapse type

**Parameters**
- `type`: New synapse type

### Activity and Plasticity

#### `bool isExcitatory() const`

**Method**: Check if synapse is excitatory

**Returns**
- `true` if excitatory, `false` otherwise

#### `bool isInhibitory() const`

**Method**: Check if synapse is inhibitory

**Returns**
- `true` if inhibitory, `false` otherwise

#### `void recordPreSpike(Timestamp timestamp)`

**Method**: Record pre-synaptic spike

**Parameters**
- `timestamp`: Time when pre-synaptic neuron spiked

#### `void recordPostSpike(Timestamp timestamp)`

**Method**: Record post-synaptic spike

**Parameters**
- `timestamp`: Time when post-synaptic neuron spiked

#### `const std::vector<Timestamp>& getPreSpikeHistory() const`

**Method**: Get pre-synaptic spike history

**Returns**
- Const reference to pre-synaptic spike history

#### `const std::vector<Timestamp>& getPostSpikeHistory() const`

**Method**: Get post-synaptic spike history

**Returns**
- Const reference to post-synaptic spike history

#### `void clearHistory()`

**Method**: Clear spike history

### Plasticity Flags

#### `const PlasticityFlags& getPlasticityFlags() const`

**Method**: Get plasticity flags

**Returns**
- Const reference to plasticity flags

#### `PlasticityFlags& getPlasticityFlags()`

**Method**: Get mutable reference to plasticity flags

**Returns**
- Mutable reference to plasticity flags

#### `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)`

**Method**: Enable/disable plasticity rules

**Parameters**
- `hebbian`: Enable Hebbian plasticity
- `stdp`: Enable STDP
- `rewardModulated`: Enable reward-modulated plasticity

### Eligibility Traces

#### `float getEligibilityTrace() const`

**Method**: Get eligibility trace value

**Returns**
- Eligibility trace (typically between 0 and 1)

#### `void setEligibilityTrace(float trace)`n

**Method**: Set eligibility trace

**Parameters**
- `trace`: New eligibility trace value

#### `void decayEligibilityTrace(float decayRate)`

**Method**: Decay eligibility trace

**Parameters**
- `decayRate`: Decay rate (typically between 0 and 1)

### Synaptic Efficacy

#### `float getEfficacy() const`

**Method**: Get synaptic efficacy

**Returns**
- Synaptic efficacy (modulation by recent activity)

#### `void setEfficacy(float efficacy)`

**Method**: Set synaptic efficacy

**Parameters**
- `efficacy`: New synaptic efficacy

### Simulation Control

#### `void step(Timestamp currentTime)`

**Method**: Update synapse for one simulation step

**Parameters**
- `currentTime`: Current simulation time (seconds)

**Notes**
- TODO PHASE 2: Implement real synaptic dynamics

#### `void reset()`

**Method**: Reset synapse to initial state

#### `void initializeRandom(class RandomGenerator& rng)`

**Method**: Initialize synapse with random parameters

**Parameters**
- `rng`: Random number generator reference

## Configuration API

The Configuration class provides access to simulation parameters and settings.

### Configuration Management

#### `bool loadFromFile(const std::string& filepath)`

**Method**: Load configuration from file

**Parameters**
- `filepath`: Path to configuration file

**Returns**
- `true` if configuration loaded successfully
- `false` if file not found or invalid

#### `void loadFromArgs(int argc, char** argv)`

**Method**: Load configuration from command line arguments

**Parameters**
- `argc`: Number of command line arguments
- `argv`: Array of command line argument strings

**Example**
```cpp
config->loadFromArgs(argc, argv);
```

#### `void set(const std::string& key, const T& value, ConfigSource source)`

**Method**: Set configuration value

**Parameters**
- `key`: Configuration key
- `value`: Value to set
- `source`: Source of configuration (File, CommandLine, Default, etc.)

#### `template<typename T> T get(const std::string& key) const`

**Method**: Get configuration value

**Parameters**
- `key`: Configuration key

**Returns**
- Configuration value

#### `template<typename T> T getOr(const std::string& key, T defaultValue) const`

**Method**: Get configuration value with default

**Parameters**
- `key`: Configuration key
- `defaultValue`: Default value if key not found

**Returns**
- Configuration value or default if not found

### Configuration Keys

Common configuration keys include:
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of neural regions
- `simulation_timestep`: Simulation time step size
- `random_seed`: Random seed for reproducible simulations
- `connection_probability`: Probability of synaptic connections
- `stdp_ltp_weight`: STDP potentiation weight
- `stdp_ltd_weight`: STDP depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

## Logging API

The Logger class provides comprehensive logging and monitoring capabilities.

### Logging Levels

- `Trace`: Detailed debugging information
- `Debug`: Development and troubleshooting information
- `Info`: General system information

**Method**: `addLogger(std::shared_ptr<Logger> logger)`

**Method**: `void log(const std::string& message)`

**Method**: `void logError(const std::string& message)`

## Environment API

The Environment class provides world simulation and sensory input.

### Environment Control

#### `void step(double time)`

**Method**: Update environment for one simulation step

**Parameters**
- `time`: Current simulation time

#### `SensoryInput getSensoryInput()`

**Method**: Get sensory input from environment

**Returns**
- Sensory input for the current simulation step

## Motor System API

The MotorSystem class provides action selection and execution.

### Action Control

#### `std::unique_ptr<Action> produceAction()`

**Method**: Produce motor action based on neural activity

**Returns**
- Action to execute, or nullptr if no action needed

#### `void applyAction(const Action& action)`

**Method**: Apply motor action

**Parameters**
- `action`: Action to apply

## Memory System APIs

### Working Memory API

#### `void store(Pattern pattern)`

**Method**: Store pattern in working memory

**Parameters**
- `pattern`: Pattern to store

#### `Pattern recall()`

**Method**: Recall pattern from working memory

**Returns**
- Recalled pattern

### Episodic Memory API

#### `void record(const Event& event)`

**Method**: Record experience in episodic memory

**Parameters**
- `event`: Event to record

#### `Event recallEvent(Timestamp time)`

**Method**: Recall event at specific time

**Parameters**
- `time`: Time of event to recall

**Returns**
- Recorded event

### Associative Memory API

#### `void learn(Pattern cue, Pattern response)`

**Method**: Learn pattern association

**Parameters**
- `cue`: Input pattern
- `response`: Output pattern

#### `Pattern retrieve(Pattern cue)`

**Method**: Retrieve response pattern

**Parameters**
- `cue`: Input pattern

**Returns**
- Associated response pattern

## Design Patterns API

The design patterns are implemented as separate classes with specific interfaces.

### Observer Pattern

Used for event-driven systems where multiple components need to be notified of changes.

### Command Pattern

Used for action execution and undo/redo functionality.

### Factory Pattern

Used for creating different types of neurons and synapses.

### Strategy Pattern

Used for pluggable plasticity and learning rules.

## Usage Examples

### Basic Simulation

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    return 0;
}
```

### Learning Experiment

```cpp
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"

void runLearningExperiment() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("stdp_ltp_weight", 0.02);
    config->set("stdp_ltd_weight", 0.015);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity
    auto* region = brain->getRegion(RegionId(1));
    for (auto& synapse : region->getSynapses()) {
        synapse->enablePlasticity(true, true, false);
    }
    
    // Run learning simulation
    for (int step = 0; step < 5000; ++step) {
        // Apply sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        // Apply neuromodulation
        auto* neuromodulator = brain->getDopamine();
        if (neuromodulator) {
            brain->applyNeuromodulation(*neuromodulator);
        }
    }
    
    // Record final state
    brain->logStatus();
}
```

### Plasticity Experiment

```cpp
#include "plasticity/STDP.hpp"

void demonstrateSTDP() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        std::cerr << "Not enough synapses for STDP demonstration" << std::endl;
        return;
    }
    
    auto& synapses = region->getSynapses();
    
    // Record initial weights
    std::vector<float> initial_weights;
    for (size_t i = 0; i < 5; ++i) {
        initial_weights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Create correlated activity to trigger LTP
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            // Pre fires first
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Check for weight changes (LTP)
    float total_delta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - initial_weights[i];
        total_delta += delta;
    }
    
    if (total_delta > 0.001f) {
        std::cout << "STDP working: Pre-before-post produced potentiation" << std::endl;
    }
}
```

## API Evolution and Compatibility

### Versioning

The NLM API follows semantic versioning:

- **Major version**: Breaking changes to the API
- **Minor version**: New features without breaking changes
- **Patch version**: Bug fixes and improvements

### Backward Compatibility

The NLM API maintains backward compatibility:

1. **Method Signatures**: Existing method signatures are not modified
2. **Return Types**: Return types remain consistent
3. **Exception Behavior**: Exception behavior is preserved
4. **Default Values**: Default parameter values are maintained

### Extension Points

The API is designed to be extensible:

1. **Plugin Architecture**: New systems can be added via plugin interfaces
2. **Configuration Expansion**: New configuration parameters can be added
3. **Algorithm Extensions**: New plasticity and learning rules can be implemented
4. **Interface Extensions**: New interfaces can be added without breaking existing code

## Future API Developments

### Planned Extensions

1. **Enhanced Neuromodulation**: Additional neuromodulators and signals
2. **Advanced Learning**: Meta-learning and transfer learning capabilities
3. **Multi-Modal Interfaces**: Support for additional input modalities
4. **Distributed Computing**: APIs for distributed and parallel simulations
5. **Hardware Acceleration**: APIs for GPU and specialized hardware acceleration

### Research Interfaces

New research interfaces are planned:

1. **Genomics Interface**: Integration with biological data
2. **Environmental Interface**: Real-world environmental integration
3. **Cognitive Testing Interface**: Standardized testing frameworks
4. **Educational Interface**: Teaching and learning tools

## Implementation Notes

### Memory Management

- Use smart pointers (std::shared_ptr, std::unique_ptr) for memory management
- Avoid raw pointers where possible
- Implement proper copy/move constructors and assignment operators
- Use const references for parameter passing where appropriate

### Thread Safety

- Brain operations are not thread-safe
- Use external synchronization for concurrent access
- Implement thread-safe interfaces for external systems

### Error Handling

- Throw exceptions for recoverable errors
- Use error codes for non-recoverable errors
- Provide detailed error context and recovery suggestions

### Performance Considerations

- Minimize memory allocations during simulation
- Use reference parameters where appropriate
- Cache frequently accessed values
- Use efficient data structures for common operations

This API documentation provides comprehensive coverage of the NLM system's public interfaces, including detailed method documentation, usage examples, and implementation notes. It serves as both a reference for developers and a guide for extending and modifying the system.