# NLM Neural Simulation Framework - API Guide

## Overview

This document provides comprehensive API documentation for the NLM (Neural Learning Machine) neural simulation framework. NLM implements a biologically inspired artificial brain with real spiking neural computation, event-driven dynamics, and integrated memory, prediction, cognition, and neuromodulation systems.

## Core Architecture

The NLM framework is organized into several key layers:

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator  
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Integration Layer
- **AgentBrain**: Connects NLM brain to the world
- **AgentBody**: Physical properties and internal state
- **SimpleWorld**: Simulated environment

## Core Classes

### Brain Class
**File**: `src/brain/Brain.hpp`

The central coordinator of the neural system with integrated memory, prediction, and cognition.

#### Key Methods:

**Constructor/Destructor:**
- `Brain(std::shared_ptr<Config> config)` - Creates brain with configuration
- `~Brain()` - Cleans up brain resources

**Simulation Control:**
- `bool initialize()` - Initializes brain with configured regions and neurons
- `void step(SimulationStep currentStep)` - Performs simulation step with timestep
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Performs step with precise timing

**Sensory Processing:**
- `void receiveSensoryInput(const SensoryInput& input)` - Processes environmental input
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Injects current into specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Injects to neurons of specific type

**System Accessors:**
- `SpikeSystem* getSpikeSystem()` - Access spike processing system
- `STDP* getSTDP()` - Access STDP system
- `Hebbian* getHebbian()` - Access Hebbian learning system
- `NeuralWorkingMemory* getWorkingMemory()` - Access working memory system
- `NeuralEpisodicMemory* getEpisodicMemory()` - Access episodic memory system

**Memory Systems:**
- `NeuralAssociativeMemory* getAssociativeMemory()` - Access associative memory

**Prediction & Cognition:**
- `PredictionSystem* getPredictionSystem()` - Access prediction system
- `NeuralPlanner* getPlanner()` - Access neural planner
- `ConceptFormation* getConceptFormation()` - Access concept formation
- `AttentionalSelection* getAttention()` - Access attentional selection

**Development & Neuromodulation:**
- `DevelopmentSystem* getDevelopmentSystem()` - Access development system
- `Dopamine* getDopamine()` - Access dopamine system
- `Curiosity* getCuriosity()` - Access curiosity system
- `Novelty* getNovelty()` - Access novelty detection
- `PredictionError* getPredictionErrorSignal()` - Access prediction error

**Statistics & Metrics:**
- `float getExcitationInhibitionRatio() const` - Calculates E/I balance
- `size_t getTotalSpikeCount() const` - Gets total spike count
- `size_t getPendingSpikeEventCount() const` - Gets pending spike events
- `size_t getActiveNeuronCount() const` - Gets active neuron count
- `float getAverageFiringRate() const` - Gets average firing rate

**Output Generation:**
- `std::unique_ptr<Action> produceAction()` - Produces motor/action output

**Neuromodulation:**
- `void applyNeuromodulation(const Neuromodulator& signal)` - Applies neuromodulatory signals

**Plasticity Management:**
- `void updatePlasticity()` - Updates plasticity rules

**Development Management:**
- `void develop()` - Applies developmental changes

**State Management:**
- `void reset()` - Resets brain to initial state
- `bool save(const std::string& filepath) const` - Saves brain state to file
- `bool load(const std::string& filepath)` - Loads brain state from file

**Region Management:**
- `RegionId addRegion(const std::string& name = "")` - Creates new neural region
- `NeuralRegion* getRegion(RegionId id)` - Finds region by ID
- `size_t getRegionCount() const` - Gets number of regions
- `std::vector<RegionId> getRegionIds() const` - Gets all region IDs

**Inter-Regional Connectivity:**
- `void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1)` - Creates connection between regions
- `void removeInterRegionConnection(RegionId source, RegionId target)` - Removes region connection

### AgentBrain Class
**File**: `src/agent/AgentBrain.hpp`

Connects NLM brain to the world - handles sensory transduction and motor decoding.

#### Key Methods:

**Constructor/Destructor:**
- `AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<const Config> config = nullptr)` - Constructs agent-brain interface
- `~AgentBrain()` - Cleans up agent resources

**Initialization:**
- `void initialize(const SimpleWorld& world)` - Initializes with world environment

**Sensory Processing:**
- `size_t getSensoryInputSize() const` - Gets sensory input size expected by brain
- `void processSensoryInput(const SensoryPercept& percept)` - Processes sensory input and injects into brain

**Motor Output:**
- `size_t getMotorOutputSize() const` - Gets motor output size expected
- `MotorCommand decodeMotorCommand()` - Decodes brain motor activity into motor command

**Neuromodulation:**
- `void applyRewardModulation(float reward, float predictedReward)` - Applies neuromodulation based on reward

**Development:**
- `void updateDevelopment(double timestep)` - Updates developmental system
- `DevelopmentalStage getDevelopmentalStage() const` - Gets current developmental stage

**Configuration:**
- `void enableRewardModulation(bool enable)` - Enables/disables reward modulation
- `void enableStructuralPlasticity(bool enable)` - Enables/disables structural plasticity
- `void enableDevelopment(bool enable)` - Enables/disables development
- `void enableCuriosity(bool enable)` - Enables/disables curiosity

**Getters:**
- `float getNeuromodulationLevel() const` - Gets current neuromodulation level
- `float getCuriosityLevel() const` - Gets current curiosity level
- `float getNoveltyLevel() const` - Gets current novelty level
- `float getPredictionError() const` - Gets current prediction error

### AgentBody Class
**File**: `src/agent/AgentBody.hpp`

Physical properties and internal state of an agent in the environment.

#### Key Methods:

**State Management:**
- `void reset()` - Resets agent to initial state
- `void applyMotorCommand(MotorCommand command, float dt, const ActionResult& worldResponse)` - Applies motor command to update physical state

**Position & Orientation:**
- `void setPosition(float x, float y)` - Sets agent position
- `void setOrientation(float angle)` - Sets agent orientation
- `std::pair<float, float> getPosition() const` - Gets current position

**Distance & Proximity:**
- `float getDistanceTo(float targetX, float targetY) const` - Gets distance to target
- `bool isNear(float x, float y, float radius) const` - Checks if near position

**Internal State:**
- Various properties: x, y, orientation, velocityX, velocityY, angularVelocity, energy, health, age, isMoving, isTurning, lastActionTime

### Neuron Class
**File**: `src/brain/Neuron.hpp`

Individual neuron implementation with Leaky Integrate-and-Fire (LIF) dynamics.

#### Key Methods:

**Identity:**
- `NeuronId getId() const` - Gets neuron ID
- `NeuronType getType() const` - Gets neuron type
- `void setType(NeuronType type)` - Sets neuron type

**State Access:**
- `const NeuronState& getState() const` - Gets constant neuron state
- `NeuronState& getState()` - Gets mutable neuron state

**Membrane Potential:**
- `MembranePotential getMembranePotential() const` - Gets membrane potential
- `void setMembranePotential(MembranePotential potential)` - Sets membrane potential
- `void addToMembranePotential(MembranePotential delta)` - Adds to membrane potential

**Threshold & Firing:**
- `MembranePotential getThreshold() const` - Gets firing threshold
- `void setThreshold(MembranePotential threshold)` - Sets firing threshold
- `bool checkThreshold() const` - Checks if neuron should fire
- `bool isFiring() const` - Checks if neuron is firing
- `bool isRefractory() const` - Checks if neuron is refractory

**LIF Dynamics:**
- `bool stepLIF(Timestamp currentTime, TimestepDuration dt)` - LIF step function
- `void step(Timestamp currentTime)` - Updates neuron for one simulation step

**Synaptic Inputs:**
- `void receiveExcitatoryInput(MembranePotential amplitude)` - Receives excitatory input
- `void receiveInhibitoryInput(MembranePotential amplitude)` - Receives inhibitory input
- `void receiveModulatoryInput(MembranePotential amplitude)` - Receives modulatory input

**Current Injection:**
- `void injectCurrent(MembranePotential current)` - Injects current
- `MembranePotential getTotalCurrent() const` - Gets total current
- `void clearTotalCurrent()` - Clears total current

**Spike Management:**
- `void recordSpike(Timestamp timestamp)` - Records spike
- `const std::vector<Timestamp>& getSpikeHistory() const` - Gets spike history
- `void clearSpikeHistory()` - Clears spike history

**Synapse Management:**
- `void addIncomingSynapse(SynapseHandle handle)` - Adds incoming synapse
- `void addOutgoingSynapse(SynapseHandle handle)` - Adds outgoing synapse

**Plasticity:**
- `const PlasticityFlags& getPlasticityFlags() const` - Gets plasticity flags
- `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enables plasticity

**Region Membership:**
- `void setRegionId(RegionId region)` - Sets region ID
- `RegionId getRegionId() const` - Gets region ID
- `void setPopulationId(PopulationId population)` - Sets population ID
- `PopulationId getPopulationId() const` - Gets population ID

**Initialization & Reset:**
- `void reset()` - Resets to initial state
- `void initializeRandom(RandomGenerator& rng)` - Initializes with random parameters

### Synapse Class
**File**: `src/brain/Synapse.hpp`

Connection between neurons implementing synaptic transmission with delays and short-term plasticity.

#### Key Methods:

**Identity:**
- `SynapseId getId() const` - Gets synapse ID

**Connection:**
- `NeuronId getSourceNeuron() const` - Gets source neuron
- `NeuronId getDestinationNeuron() const` - Gets target neuron

**Weight:**
- `SynapticWeight getWeight() const` - Gets synaptic weight
- `void setWeight(SynapticWeight weight)` - Sets synaptic weight
- `void addToWeight(SynapticWeight delta)` - Adds to synaptic weight

**Delay:**
- `Delay getDelay() const` - Gets transmission delay
- `void setDelay(Delay delay)` - Sets transmission delay

**Type:**
- `SynapseType getType() const` - Gets synapse type
- `void setType(SynapseType type)` - Sets synapse type

**Excitatory/Inhibitory:**
- `bool isExcitatory() const` - Checks if excitatory
- `bool isInhibitory() const` - Checks if inhibitory

**Activity History:**
- `void recordPreSpike(Timestamp timestamp)` - Records presynaptic spike
- `void recordPostSpike(Timestamp timestamp)` - Records postsynaptic spike
- `const std::vector<Timestamp>& getPreSpikeHistory() const` - Gets presynaptic spike history
- `const std::vector<Timestamp>& getPostSpikeHistory() const` - Gets postsynaptic spike history
- `void clearHistory()` - Clears spike history

**Plasticity Flags:**
- `const PlasticityFlags& getPlasticityFlags() const` - Gets plasticity flags
- `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enables plasticity

**Eligibility Trace:**
- `float getEligibilityTrace() const` - Gets eligibility trace
- `void setEligibilityTrace(float trace)` - Sets eligibility trace
- `void decayEligibilityTrace(float decayRate)` - Decays eligibility trace

**Synaptic Efficacy:**
- `float getEfficacy() const` - Gets synaptic efficacy
- `void setEfficacy(float efficacy)` - Sets synaptic efficacy

**Simulation:**
- `void step(Timestamp currentTime)` - Updates synapse for one simulation step

**Reset & Initialization:**
- `void reset()` - Resets to initial state
- `void initializeRandom(class RandomGenerator& rng)` - Initializes with random parameters

## Supporting Types

### NeuronType
Enumeration of neuron functional types:
- Sensory, Internal, Motor

### SynapseType
Enumeration of synapse functional types:
- Excitatory, Inhibitory, Modulatory

### MotorCommand
Enumeration of low-level motor commands:
- MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

### WorldObjectType
Enumeration of object types in the world:
- Empty, Resource, Hazard, Wall, Marker

### FiringState
Enumeration of neural firing states:
- Resting, Firing, Refractory

### DevelopmentalStage
Enumeration of developmental stages:
- Initial, Critical, Maturation, Adult

## Usage Examples

### Basic Brain Simulation
```cpp
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>(...);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent interface
    auto agentBrain = std::make_shared<AgentBrain>(brain, config);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    agentBrain->initialize(*world);
    
    // Simulation loop
    for (int step = 0; step < 1000; ++step) {
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process and inject into brain
        agentBrain->processSensoryInput(percept);
        
        // Run brain simulation
        brain->step(step);
        
        // Decode motor command
        auto motorCommand = agentBrain->decodeMotorCommand();
        
        // Apply to world
        world->applyMotorCommand(motorCommand);
    }
    
    // Save brain state
    brain->save("brain_checkpoint.dat");
    
    return 0;
}
```

### Working with Neurons
```cpp
#include "src/brain/Neuron.hpp"

void configureNeuronDemo() {
    // Create neuron
    NeuronId neuronId(42);
    Neuron neuron(neuronId);
    
    // Configure neuron parameters
    neuron.setThreshold(-50.0f);           // Lower threshold = more excitable
    neuron.setLeakConductance(15.0f);      // Higher conductance = faster decay
    neuron.setRestingPotential(-65.0f);   // Different resting potential
    neuron.setType(NeuronType::Internal);  // Internal neuron type
    
    // Inject input and simulate
    neuron.receiveExcitatoryInput(5.0f);
    neuron.step(1000, 0.001);  // timestamp, dt
    
    // Check firing status
    if (neuron.checkThreshold()) {
        neuron.recordSpike(1000);
    }
    
    // Check state
    auto state = neuron.getState();
    float potential = neuron.getMembranePotential();
    bool firing = neuron.isFiring();
}
```

### Working with Synapses
```cpp
#include "src/brain/Synapse.hpp"

void configureSynapseDemo() {
    // Create synapse
    SynapseId synapseId(1);
    NeuronId sourceId(100);
    NeuronId targetId(200);
    
    Synapse synapse(synapseId, sourceId, targetId);
    
    // Configure synaptic properties
    synapse.setType(SynapseType::Excitatory);
    synapse.setWeight(1.5f);    // Strong excitation
    synapse.setDelay(2);        // 2 timestep delay
    
    // Record spike activity for plasticity
    synapse.recordPreSpike(1000);
    synapse.recordPostSpike(1002);
    
    // Simulate synaptic transmission
    synapse.step(1002);
    
    // Check properties
    float weight = synapse.getWeight();
    float efficacy = synapse.getEfficacy();
    bool excitatory = synapse.isExcitatory();
}
```

## Configuration Options

The NLM framework uses a configuration system managed by the `Config` class. Key configuration options include:

- Neuron count and population sizes
- Brain region definitions and connectivity
- Simulation parameters (timestep, duration)
- Plasticity rule parameters
- Memory system capacities and decay rates
- Neuromodulation parameters

Configuration objects should be created before constructing Brain instances and passed to AgentBrain constructors.

## File Structure

### Source Files
- `src/brain/` - Core neural computation (Brain, Neuron, Synapse)
- `src/agent/` - Agent interface (AgentBrain, AgentBody)
- `src/plasticity/` - Plasticity rules (STDP, Hebbian, StructuralPlasticity)
- `src/memory/` - Memory systems (WorkingMemory, EpisodicMemory)
- `src/neuromodulation/` - Neuromodulation systems (Dopamine, Curiosity, etc.)
- `src/development/` - Developmental systems
- `src/cognition/` - Cognitive systems (Planning, Attention)
- `src/prediction/` - Prediction systems
- `src/sensory/` - Sensory processing
- `src/motor/` - Motor output
- `src/environment/` - Environment integration
- `src/experiments/` - Experiment frameworks

### Documentation
- `docs/ARCHITECTURE.md` - System architecture documentation
- `docs/API_GUIDE.md` - This API reference
- `docs/ALGORITHMS.md` - Mathematical formulations and implementations
- `docs/DESIGN_PATTERNS.md` - Design patterns used
- `docs/PERFORMANCE.md` - Performance characteristics

## Dependencies

The NLM framework depends on:

### Internal Dependencies
- Custom type system (`src/core/Types/`)
- Configuration management (`src/core/Config/`)
- Random number generation (`src/core/Random/`)
- Logging (`src/core/Logger/`)
- Simulation time management (`src/core/SimulationClock/`)
- Error handling and validation (`src/core/ErrorHandling/`)

### External Dependencies
- C++11 standard library
- Platform-specific libraries (as needed)

## Building and Running

### Build System
The project uses CMake for building. To build the framework:

```bash
cd /path/to/nlm/source
mkdir build
cd build
cmake ..
cmake --build .
```

### Running Examples
After building, executable examples can be run from the build directory:

```bash
./examples/basic_simulation
./examples/agent_world_interaction
./examples/plasticity_demo
```

### Running Tests
Unit tests are located in the `tests/` directory:

```bash
cd build
ctest
```

## License

This framework is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This neural simulation framework implements biologically inspired algorithms and design patterns derived from computational neuroscience research. Key inspirations include:

- Leaky Integrate-and-Fire neuron model
- Spike-timing dependent plasticity (STDP)
- Working memory models using neural dynamics
- Predictive coding frameworks
- Neuromodulation systems (dopamine, curiosity, etc.)

The framework is designed to be incrementally improved with more biologically accurate models in future phases.
