# NLM (Neural Learning Machine) - API Documentation

## Overview

NLM is a complete artificial brain implementation that combines spiking neural networks with integrated memory, prediction, cognition, and neuromodulation systems. This document provides comprehensive API documentation for the core classes and usage examples.

## Table of Contents

1. [Brain Class](#brain-class)
2. [Neuron Class](#neuron-class)
3. [Synapse Class](#synapse-class)
4. [AgentBrain Class](#agentbrain-class)
5. [Core Types](#core-types)
6. [Memory Systems](#memory-systems)
7. [Configuration](#configuration)

---

# Brain Class

The Brain class is the central coordinator of the NLM system, implementing the complete integrated artificial brain architecture.

## Core Architecture

The Brain implements a 15-step integrated loop that processes neural computation, manages memory systems, applies plasticity, and coordinates cognitive functions:

1. Process pending delayed spike events (deliver synaptic input)
2. Update all neuron membrane potentials (LIF dynamics)
3. Detect spikes and schedule outgoing spike events
4. Update working memory (maintenance and competition)
5. Apply neuromodulation effects on neural excitability
6. Apply plasticity rules (STDP, Hebbian)
7. Update episodic memory with current experience
8. Update prediction system
9. Update attention system
10. Update concept formation
11. Apply structural plasticity (synaptogenesis, pruning)
12. Replay important memories (during rest or periodically)
13. Apply development effects
14. Periodic memory consolidation
15. Checkpoint management

## Class Methods

### Construction and Initialization

```cpp
// Create brain with configuration
explicit Brain(std::shared_ptr<Config> config);

// Initialize brain with configuration
bool initialize();
```

**Initialize** creates and configures all neural subsystems including memory, prediction, cognition, neuromodulation, and development systems. Sets up neuron populations, connectivity, and initializes all plasticity mechanisms.

### Main Simulation Loop

```cpp
// Main simulation step with simulation time
void step(SimulationStep currentStep, Timestamp currentTime);
```

Executes the complete 15-step integrated brain loop. Must be called every simulation step to maintain brain activity and learning.

### Input/Output Interface

```cpp
// Receive sensory input from environment
void receiveSensoryInput(const class SensoryInput& input);

// Inject current directly into specific neuron
void injectCurrent(NeuronId neuron, MembranePotential current);

// Inject current into all neurons of specific type
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

// Get motor/action output based on brain activity
std::unique_ptr<class Action> produceAction();
```

**receiveSensoryInput** processes external stimuli by injecting current into sensory neurons based on input patterns, enabling environmental perception.

**produceAction** analyzes motor neuron activity and selects the most appropriate action based on firing patterns across motor populations.

### Plasticity and Learning

```cpp
// Apply neuromodulatory signals
void applyNeuromodulation(const class Neuromodulator& signal);

// Update plasticity rules (called automatically in step)
void updatePlasticity();

// Apply developmental changes (called automatically in step)
void develop();
```

These methods enable learning through STDP, Hebbian plasticity, reward-modulated learning, and developmental changes.

### State Management

```cpp
// Reset brain to initial state for new episode
void reset();

// Save brain state to checkpoint file
bool save(const std::string& filepath) const;

// Load brain state from checkpoint file
bool load(const std::string& filepath);
```

State persistence enables resuming simulations and testing with specific brain states.

### Statistics and Monitoring

```cpp
// Get global statistics
float getExcitationInhibitionRatio() const;
size_t getTotalSpikeCount() const;
size_t getPendingSpikeEventCount() const;
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getActiveNeuronCount() const;
size_t getFiringNeuronCount() const;
float getAverageFiringRate() const;
```

These methods provide insights into brain activity, network dynamics, and performance metrics.

### Region Management

```cpp
// Add new neural region
RegionId addRegion(const std::string& name = "");

// Get region by ID
NeuralRegion* getRegion(RegionId id);
const NeuralRegion* getRegion(RegionId id) const;

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
```

Regions organize the brain into functionally specialized areas (sensory, motor, association cortex).

### Integrated Systems Access

```cpp
// Memory systems
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

// Prediction system
PredictionSystem* getPredictionSystem();

// Cognition systems
NeuralPlanner* getPlanner();
ConceptFormation* getConceptFormation();
AttentionalSelection* getAttention();

// Development system
DevelopmentSystem* getDevelopmentSystem();
DevelopmentalStage getDevelopmentalStage() const;
void setDevelopmentalStage(DevelopmentalStage stage);

// Neuromodulation systems
Dopamine* getDopamine();
Curiosity* getCuriosity();
Novelty* getNovelty();
PredictionError* getPredictionErrorSignal();
```

These accessors provide control over all integrated brain subsystems.

### Configuration Access

```cpp
// Get current configuration
std::shared_ptr<const Config> getConfig() const;

// Get random generator
RandomGenerator* getRandomGenerator();
```

### Logging

```cpp
// Get detailed status information
void logStatus() const;
```

Provides comprehensive logging of brain state, statistics, and system information.

## Key Features

- **Real Spiking Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multi-Modal Plasticity**: STDP, Hebbian, reward-modulated, and structural plasticity
- **Integrated Memory**: Working, episodic, and associative memory systems
- **Prediction and Cognition**: Forward models, planning, concept formation, attention
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Development**: Dynamic developmental stages from initial to adult
- **Structural Plasticity**: Synaptogenesis and pruning for network development
- **Checkpointing**: State persistence and recovery for long-running simulations

## Usage Example

```cpp
// Create and initialize brain
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);

auto brain = std::make_shared<Brain>(config);
if (brain->initialize()) {
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Process sensory input if available
        // brain->receiveSensoryInput(sensoryInput);
        
        // Run brain computation
        brain->step(step);
        
        // Get action
        auto action = brain->produceAction();
        
        // Apply neuromodulation
        // brain->applyNeuromodulation(modulator);
    }
}
```

---

# Neuron Class

The Neuron class implements Leaky Integrate-and-Fire (LIF) neurons with biological realism and spiking dynamics.

## Neuron Architecture

Neurons integrate synaptic inputs over time, accumulating charge until they reach threshold, at which point they generate a spike and enter a refractory period. This biological spiking mechanism enables event-driven computation.

## Class Methods

### Construction and Identity

```cpp
// Create neuron with unique identifier
explicit Neuron(NeuronId id);

// Get neuron identifier
NeuronId getId() const;

// Get neuron type
NeuronType getType() const;
void setType(NeuronType type);
```

### State Management

```cpp
// Get complete neuron state
const NeuronState& getState() const;
NeuronState& getState();

// Membrane potential control
void setMembranePotential(MembranePotential potential);
void addToMembranePotential(MembranePotential delta);
MembranePotential getMembranePotential() const;

// Threshold and reset potentials
void setThreshold(MembranePotential threshold);
void setRestingPotential(MembranePotential potential);
void setResetPotential(MembranePotential potential);
MembranePotential getRestingPotential() const;
MembranePotential getResetPotential() const;
```

**Membrane potential** represents the electrical state of the neuron, integrating synaptic inputs over time.

### Firing State Control

```cpp
// Firing state management
bool isFiring() const;
bool isRefractory() const;
void setFiringState(FiringState state);
void decrementRefractory();

// Firing rate control
void setFiringRate(FiringRate rate);
FiringRate getFiringRate() const;

// Refractory period control
void setRefractoryPeriod(uint32_t steps);
uint32_t getRefractoryPeriod() const;
```

### LIF Parameters

```cpp
// Leak conductance controls how quickly neuron returns to resting potential
void setLeakConductance(MembranePotential conductance);
MembranePotential getLeakConductance() const;
```

### Input Processing

```cpp
// Synaptic inputs
void receiveExcitatoryInput(MembranePotential amplitude);
void receiveInhibitoryInput(MembranePotential amplitude);
void receiveModulatoryInput(MembranePotential amplitude);

// External current injection
void injectCurrent(MembranePotential current);
MembranePotential getTotalCurrent() const;
void clearTotalCurrent();
```

**Excitatory inputs** increase membrane potential, **inhibitory inputs** decrease it, and **modulatory inputs** affect plasticity without directly changing membrane potential.

### Spike History and Plasticity

```cpp
// Spike recording for plasticity and STDP
void recordSpike(Timestamp timestamp);
const std::vector<Timestamp>& getSpikeHistory() const;
void clearSpikeHistory();

// Plasticity configuration
const PlasticityFlags& getPlasticityFlags() const;
PlasticityFlags& getPlasticityFlags();
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
```

### Network Connectivity

```cpp
// Synapse management
void addIncomingSynapse(SynapseHandle handle);
void addOutgoingSynapse(SynapseHandle handle);
const std::vector<SynapseHandle>& getIncomingSynapses() const;
const std::vector<SynapseHandle>& getOutgoingSynapses() const;
```

### Development and Initialization

```cpp
// Reset to initial state
void reset();

// Initialize with biologically realistic parameters
void initializeRandom(RandomGenerator& rng);
```

### Core Computation

```cpp
// LIF dynamics step
bool stepLIF(Timestamp currentTime, TimestepDuration dt);

// Default step with standard timestep
void step(Timestamp currentTime);
```

**stepLIF** implements the core Leaky Integrate-and-Fire computation, updating membrane potential based on inputs and leak, detecting spikes, and handling refractory periods.

## Neuron State

```cpp
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
};
```

## Key Features

- **Biologically Realistic LIF Dynamics**: Implements Leaky Integrate-and-Fire with exponential Euler integration
- **Spike-Frequency Adaptation**: Models adaptation behavior after spiking
- **Refractory Periods**: Prevents uncontrolled spiking with realistic refractory periods
- **Spike History**: Maintains recent spike timing for STDP and other timing-dependent computations
- **Multiple Input Types**: Excitatory, inhibitory, and modulatory inputs with different effects
- **Plasticity Integration**: Configurable plasticity mechanisms including STDP and Hebbian learning
- **Network Integration**: Manages synaptic connections for efficient spike propagation

## Usage Example

```cpp
// Create neuron
NeuronId id(42);
Neuron neuron(id);

// Configure neuron parameters
neuron.setType(NeuronType::Excitatory);
neuron.setThreshold(-55.0f);
neuron.setRestingPotential(-70.0f);
neuron.setResetPotential(-75.0f);
neuron.setRefractoryPeriod(5);

// Initialize with biologically realistic values
RandomGenerator rng(42);
neuron.initializeRandom(rng);

// Simulate neural activity
for (Timestamp time = 0.0; time < 100.0; time += 0.001f) {
    // Apply inputs
    neuron.receiveExcitatoryInput(10.0f);
    
    // Step neuron
    if (neuron.stepLIF(time, 0.001f)) {
        std::cout << "Neuron " << id.index() << " spiked at time " << time << std::endl;
    }
    
    // Record spikes for plasticity
    if (neuron.isFiring()) {
        neuron.recordSpike(time);
    }
}
```

---

# Synapse Class

The Synapse class implements biological synaptic transmission with delays and short-term plasticity, enabling realistic neural communication.

## Synapse Architecture

Synapses are the fundamental communication channels in the neural network, transmitting signals from presynaptic to postsynaptic neurons with biological delays and dynamic modulation based on recent activity patterns.

## Class Methods

### Construction and Identity

```cpp
// Create synapse with neuron connections
Synapse(SynapseId id, NeuronId source, NeuronId destination);

// Get synapse identifier
SynapseId getId() const;

// Get connection information
NeuronId getSourceNeuron() const;
NeuronId getDestinationNeuron() const;
```

### Weight Management

```cpp
// Get current synaptic weight
SynapticWeight getWeight() const;

// Set synaptic weight
void setWeight(SynapticWeight weight);

// Modify synaptic weight by delta (for plasticity)
void addToWeight(SynapticWeight delta);
```

**Synaptic weight** determines connection strength. Positive weights are excitatory, negative weights are inhibitory.

### Delay and Type Control

```cpp
// Synaptic transmission delay
Delay getDelay() const;
void setDelay(Delay delay);

// Synapse type
SynapseType getType() const;
void setType(SynapseType type);

// Type checking
bool isExcitatory() const;
bbool isInhibitory() const;
```

**Synaptic delay** represents biological transmission time, typically 1-5 simulation steps.

### Plasticity and Learning

```cpp
// Spike history for STDP
void recordPreSpike(Timestamp timestamp);
void recordPostSpike(Timestamp timestamp);
const std::vector<Timestamp>& getPreSpikeHistory() const;
const std::vector<Timestamp>& getPostSpikeHistory() const;
void clearHistory();

// Plasticity configuration
const PlasticityFlags& getPlasticityFlags() const;
PlasticityFlags& getPlasticityFlags();
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
```

**Spike history** enables Spike-Timing-Dependent Plasticity (STDP), where the timing between pre- and post-synaptic spikes determines weight changes.

### Short-Term Plasticity

```cpp
// Eligibility trace for reward-modulated learning
float getEligibilityTrace() const;
void setEligibilityTrace(float trace);
void decayEligibilityTrace(float decayRate);

// Synaptic efficacy (use-dependent modulation)
float getEfficacy() const;
void setEfficacy(float efficacy);
```

**Short-term plasticity** models dynamic changes in synaptic strength based on recent activity patterns, including depression and facilitation.

### State Management

```cpp
// Update synaptic state for one simulation step
void step(Timestamp currentTime);

// Reset synapse to initial state
void reset();

// Initialize with biologically realistic parameters
void initializeRandom(RandomGenerator& rng);
```

### Development and Maintenance

```cpp
// Record pre-synaptic spike time (for short-term dynamics)
// Note: These are set by the neural network during spike processing
```

## Synapse Dynamics

### Short-Term Plasticity Mechanisms

The synapse implements the Tsodyks-Markram model for short-term plasticity:

- **Facilitation**: Recent high-frequency activity increases synaptic efficacy
- **Depression**: Recent activity decreases synaptic efficacy (recovery over time)
- **Eligibility Traces**: Track contribution to long-term plasticity for reward-modulated learning

### STDP Computation

Spike history is maintained for both pre- and post-synaptic neurons, enabling:

- **LTP (Long-Term Potentiation)**: Pre-before-post spikes strengthen synapse
- **LTD (Long-Term Depression)**: Post-before-pre spikes weaken synapse

### Weight Bounds

- **Minimum weight**: -1.0f (strong inhibition)
- **Maximum weight**: 1.0f (strong excitation)
- **Clamp during updates**: Prevents numerical instability

## Key Features

- **Biological Transmission Delays**: Realistic synaptic transmission timing
- **Short-Term Plasticity**: Facilitation and depression based on activity history
- **STDP Learning**: Spike-timing dependent plasticity for learning
- **Reward-Modulated Learning**: Eligibility traces for dopamine-dependent plasticity
- **Synaptic Efficacy**: Use-dependent modulation of transmission strength
- **Weight Clamping**: Prevents instability while allowing reasonable weight ranges
- **Activity-Dependent Dynamics**: Models biological synaptic behavior

## Usage Example

```cpp
// Create synapse
SynapseId synId(1);
NeuronId sourceId(100);
NeuronId destId(200);
Synapse synapse(synId, sourceId, destId);

// Initialize with realistic parameters
RandomGenerator rng(42);
synapse.initializeRandom(rng);
synapse.setType(SynapseType::Excitatory);
synapse.setDelay(2);  // 2ms delay

// Enable STDP plasticity
synapse.enablePlasticity(false, true, false);  // Only STDP

// Simulate synaptic dynamics
for (Timestamp time = 0.0; time < 100.0; time += 0.001f) {
    // Apply synaptic step (updates STP state)
    synapse.step(time);
    
    // Record pre-synaptic spike
    if (some_condition) {
        synapse.recordPreSpike(time);
    }
    
    // Record post-synaptic spike
    if (some_other_condition) {
        synapse.recordPostSpike(time);
    }
    
    // Check for eligibility trace accumulation (for reward learning)
    float eligibility = synapse.getEligibilityTrace();
    if (std::abs(eligibility) > 0.001f) {
        // Apply reward-modulated learning
        float rewardSignal = 1.0f;  // Dopamine level
        float weightChange = eligibility * rewardSignal * 0.1f;
        synapse.addToWeight(weightChange);
        
        // Decay eligibility trace
        synapse.decayEligibilityTrace(0.1f);
    }
}
```

---

# AgentBrain Class

The AgentBrain class connects the NLM brain to the external world, handling sensory transduction and motor command decoding for embodied agents.

## Agent Architecture

The AgentBrain serves as the interface between the internal neural computation and the external environment, enabling the brain to perceive and act upon the world through sensory processing and motor output.

## Class Methods

### Construction and Initialization

```cpp
// Create agent brain interface
AgentBrain(std::shared_ptr<Brain> brain);

// Initialize agent with world
void initialize(const SimpleWorld& world);
```

**initialize** sets up the sensory-motor pipeline and establishes the connection between neural activity and world state.

### Sensory Processing

```cpp
// Process sensory percept and inject into brain
void processSensoryInput(const SensoryPercept& percept);

// Get sensory input size expected by brain
size_t getSensoryInputSize() const;
```

**processSensoryInput** converts environmental observations into neural currents, mapping world sensor values to neural activation patterns.

### Motor Output and Action Selection

```cpp
// Decode brain motor activity into motor command
MotorCommand decodeMotorCommand();

// Get motor output size expected
size_t getMotorOutputSize() const;
```

**decodeMotorCommand** converts neural activity patterns in motor neuron populations into concrete actions through winner-take-all selection.

### Neuromodulation and Learning

```cpp
// Apply neuromodulation based on reward
void applyRewardModulation(float reward, float predictedReward);

// Get neuromodulation levels
float getNeuromodulationLevel() const;      // Dopamine
float getCuriosityLevel() const;           // Curiosity
float getNoveltyLevel() const;             // Novelty
float getPredictionError() const;          // Prediction error
```

### Development and Aging

```cpp
// Update developmental processes
void updateDevelopment(double timestep);

// Get current developmental stage
DevelopmentalStage getDevelopmentalStage() const;
```

### Configuration and Control

```cpp
// Enable/disable subsystems
void enableRewardModulation(bool enable);
void enableStructuralPlasticity(bool enable);
void enableDevelopment(bool enable);
void enableCuriosity(bool enable);

// Check subsystem status
bool isRewardModulationEnabled() const;
bool isStructuralPlasticityEnabled() const;
bool isDevelopmentEnabled() const;
bool isCuriosityEnabled() const;
```

### State Management

```cpp
// Reset agent for new episode
void reset();
```

## Sensory Processing Pipeline

The AgentBrain implements a multi-channel sensory system:

### Vision Processing (256 channels)
- Maps world vision to 256 sensory neurons
- Current injection proportional to vision intensity
- Used for environmental perception and scene understanding

### Touch Processing (8 channels)
- Detects collision and contact signals
- Higher sensitivity for collision detection
- Enables physical interaction with environment

### Internal Signals (4 channels)
- Centered and scaled for neural compatibility
- Used for internal state monitoring
- Enables homeostatic regulation

### Proprioception (6 channels)
- Joint position and movement sensing
- Used for motor control and coordination
- Enables body awareness and movement planning

## Motor Command System

The AgentBrain uses a 6-action motor system:

- **MoveForward**: Advance in current direction
- **MoveBackward**: Reverse direction
- **TurnLeft**: Rotate counterclockwise
- **TurnRight**: Rotate clockwise
- **Interact**: Interact with nearby objects
- **Wait**: No action (rest)

### Action Selection

Action selection uses a winner-take-all mechanism:
1. Calculate activity in each motor neuron group
2. Compare average activation across groups
3. Select action with highest activity
4. Apply curiosity-based exploration when appropriate

### Curiosity-Driven Exploration

When curiosity is high (>0.5), the agent randomly selects actions with probability proportional to curiosity level, promoting exploration of the environment.

## Neuromodulation System

The AgentBrain implements biological neuromodulation:

### Dopamine (Reward Prediction Error)
- Tracks difference between expected and received rewards
- Modulates synaptic plasticity rates
- Increases learning for unexpected positive outcomes

### Curiosity
- Driven by novelty detection
- Encourages exploration of novel environments
- Scales with prediction error

### Novelty
- Detects changes in sensory input
- Decays over time to reset detection
- Contributes to curiosity computation

## Developmental Stages

The agent undergoes biologically inspired development:

1. **Initial** (<60s): High plasticity, rapid learning
2. **Critical Period** (60-300s): Moderate plasticity, skill refinement
3. **Maturation** (300-900s): Lower plasticity, stable behavior
4. **Adult** (>900s): Minimal plasticity, stable performance

## Key Features

- **Multi-Modal Sensory Processing**: Vision, touch, internal signals, proprioception
- **Distributed Motor System**: 6-action system with neural population coding
- **Curiosity-Driven Exploration**: Balances exploitation with exploration
- **Biological Neuromodulation**: Dopamine, curiosity, novelty signals
- **Developmental Aging**: Maturation from high plasticity to stability
- **Embodied Learning**: Direct interaction with environment enables learning
- **Real-Time Interface**: Connects internal computation with external world

## Usage Example

```cpp
#include <pynlm.h>
#include <iostream>

void run_agent_simulation() {
    // 1. Create configuration
    auto config = pynlm::createDefaultConfig();
    config->set("brain.neuron_count", 2000);
    
    // 2. Create and initialize brain
    auto brain = pynlm::createBrain(config);
    brain->initialize();
    
    // 3. Create agent brain interface
    auto agent = pynlm::createAgentBrain(brain);
    
    // 4. Create and configure world
    auto world = pynlm::createSimpleWorld();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent->initialize(*world);
    
    // 6. Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // 7. Run simulation loop
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = percept.getInternal()[0] ? *percept.getInternal()[0] : 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            std::cout << "  Curiosity: " << agent->getCuriosityLevel() << ", Novelty: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "  Dev Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        }
    }
}
```

---

# Core Types

This section documents the fundamental types used throughout the NLM system.

## Basic Data Types

### NeuronId
```cpp
typedef uint32_t NeuronId;
```
Neuron identifier type. Invalid neuron has value 0.

### RegionId
```cpp
typedef uint32_t RegionId;
```
Brain region identifier. Invalid region has value 0.

### PopulationId
```cpp
typedef uint32_t PopulationId;
```
Neuron population identifier. Invalid population has value 0.

### SynapseId
```cpp
typedef uint32_t SynapseId;
```
Synapse identifier. Invalid synapse has value 0.

### Timestamp
```cpp
typedef double Timestamp;
```
Time in seconds. Used for spike timing and simulation time.

### SimulationStep
```cpp
typedef uint64_t SimulationStep;
```
Simulation step number. Used for indexing and progress tracking.

### MembranePotential
```cpp
typedef float MembranePotential;
```
Membrane potential in millivolts (mV). Typical range: -100mV to +50mV.

### SynapticWeight
```cpp
typedef float SynapticWeight;
```
Synaptic weight. Positive for excitatory, negative for inhibitory. Range: [-1.0, 1.0].

### FiringRate
```cpp
typedef float FiringRate;
```
Firing rate in Hertz (spikes per second). Typical range: 0-100 Hz.

### Delay
```cpp
typedef uint32_t Delay;
```
Synaptic delay in simulation steps. Typical values: 1-5 steps.

## Enumerations

### NeuronType
```cpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Modulatory,
    Internal
};
```
Different functional types of neurons in the network.

### FiringState
```cpp
enum class FiringState {
    Resting,
    Active,
    Refractory
};
```
Current state of a neuron in its processing cycle.

### DevelopmentalStage
```cpp
enum class DevelopmentalStage {
    Initial,          // High plasticity, rapid learning
    CriticalPeriod,   // Moderate plasticity, skill refinement
    Maturation,       // Lower plasticity, stable behavior
    Adult,            // Minimal plasticity, stable performance
    Aging             // Declining performance and plasticity
};
```
Biologically inspired developmental stages.

### SynapseType
```cpp
enum class SynapseType {
    Excitatory,
    Inhibitory,
    Neuromodulatory
};
```
Types of synaptic connections based on their effect.

### ActionType
```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Eat,
    Wait,
    // ... and more
};
```
Possible motor actions for agents.

### WorldObjectType
```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};
```
Types of objects in the simulation world.

## PlasticityFlags
```cpp
struct PlasticityFlags {
    bool hebbian;           // Enable Hebbian learning
    bool stdp;             // Enable spike-timing dependent plasticity
    bool reward_modulated; // Enable reward-modulated learning
};
```
Configuration flags for synaptic plasticity mechanisms.

---

# Memory Systems

The NLM brain integrates three distinct memory systems for different types of information storage and processing.

## Neural Working Memory

### Purpose
- Short-term storage of active neural information
- Maintenance and competition of neural traces
- Dynamic updating based on current activity

### Key Methods
```cpp
void storeToNeuron(NeuronId neuron, float activation);
float getActivation(NeuronId neuron) const;
void update(double timestep);
size_t getActiveTraces() const;
void clear();
```

### Characteristics
- **Capacity**: Configurable based on brain size
- **Decay**: Automatic decay over time
- **Competition**: Winner-take-all selection
- **Dynamic**: Updated every simulation step

## Neural Episodic Memory

### Purpose
- Storage of complete experience episodes
- Temporal sequencing of events
- Reward and context association

### Key Methods
```cpp
void storeEpisode(const EpisodicMemoryItem& episode);
const EpisodicMemoryItem* getEpisode(size_t index) const;
std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
void replayEpisode(const EpisodicMemoryItem* episode);
void consolidate(float threshold);
size_t getEpisodeCount() const;
void clear();
```

### Episode Structure
```cpp
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    float reward;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    // ... additional fields
};
```

### Characteristics
- **Temporal**: Stores experiences in sequence
- **Reward-Associated**: Links to outcome values
- **Replay**: Enables memory consolidation through replay
- **Compaction**: Removes weak memories over time

## Neural Associative Memory

### Purpose
- Pattern completion and association
- Fast lookup of related information
- Distributed storage of concepts

### Key Methods
```cpp
void storePattern(const std::vector<float>& pattern);
std::vector<float> recallPattern(const std::vector<float>& cue) const;
void clear();
```

### Characteristics
- **Distributed**: Stores patterns across multiple neurons
- **Associative**: Retrieves related information based on partial cues
- **Pattern Completion**: Fills in missing information based on similarity
- **Plastic**: Connections strengthen with repeated use

## Memory Integration

The memory systems work together to provide:

1. **Working Memory**: Current active information for immediate processing
2. **Episodic Memory**: Long-term storage of experiences and events
3. **Associative Memory**: Fast access to related concepts and patterns

This integration enables the brain to:
- Learn from experiences and store them
- Use current information for immediate tasks
- Recall past experiences to guide future behavior
- Form associations between different pieces of information

---

# Configuration

The NLM system uses a flexible configuration system to control brain parameters, learning mechanisms, and simulation settings.

## Configuration System

Configuration is stored in `Config` objects with hierarchical key names. Common patterns:

- `brain.neuron_count`: Brain-level parameters
- `plasticity.stdp.learning_rate`: Plasticity mechanism parameters
- `neuromod.dopamine.scale`: Neuromodulation parameters

## Key Configuration Categories

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Configuration Usage

### Creating Configuration
```cpp
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 2000);
config->set("plasticity.stdp.learning_rate", 0.01f);
```

### Accessing Configuration
```cpp
if (config->has("brain.neuron_count")) {
    int neuron_count = config->get<int>("brain.neuron_count");
}

std::vector<std::string> keys = config->getKeys();
```

### File I/O
```cpp
config->loadFromFile("config.json");
config->saveToFile("config.json");
```

## Default Configuration

The NLM system provides several preset configurations for common use cases:

### createDefaultConfig()
```cpp
auto config = pynlm::createDefaultConfig();
```
Creates a balanced configuration for general-purpose simulations.

### createSmallConfig()
```cpp
auto config = pynlm::createSmallConfig();
```
Creates a minimal configuration for rapid testing and prototyping.

### createLargeConfig()
```cpp
auto config = pynlm::createLargeConfig();
```
Creates an expanded configuration for complex simulations and research.

## Configuration Validation

The configuration system validates parameters to ensure:

1. **Type Safety**: Parameters match expected types
2. **Range Validation**: Parameters are within valid ranges
3. **Consistency**: Related parameters are compatible
4. **Completeness**: Required parameters are present

## Configuration Performance

Configuration access is optimized for:

- **Fast Lookup**: O(1) access to named parameters
- **Thread Safety**: Safe for concurrent access
- **Memory Efficiency**: Compact storage format
- **Scalability**: Supports thousands of configuration parameters

---

This documentation provides comprehensive reference for the NLM system's API, enabling users to understand, use, and extend the neural learning machine effectively. For additional information, refer to the specific documentation files in the `docs/` directory.