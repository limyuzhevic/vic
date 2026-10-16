# NLM C++ API Documentation
# 
# This document provides comprehensive API documentation for the NLM C++ library.
# It covers all major classes and their methods, with examples and usage patterns.

# Table of Contents
# 1. Core Classes
# 2. Brain Classes
# 3. Agent Classes
# 4. Environment Classes
# 5. Experiment Classes
# 6. Visualization Classes
# 7. Utility Classes
# 8. Configuration System
# 9. Example Code

# 1. Core Classes

## nlm::Config

Configuration management for NLM simulation parameters.

### Class Overview
The Config class provides a key-value configuration system for managing all simulation parameters including neural network settings, plasticity rules, neuromodulation, memory systems, and environment configuration.

### Constructor
```cpp
Config();
```
Creates an empty configuration object with default values.

### Methods

#### loadFromFile(std::string filepath)
Load configuration from a JSON file.

**Parameters:**
- `filepath`: Path to the configuration file

**Returns:**
- `true` if configuration was loaded successfully
- `false` if file not found or invalid JSON

#### loadFromArgs(int argc, char** argv)
Load configuration from command line arguments.

**Parameters:**
- `argc`: Number of command line arguments
- `argv`: Array of command line argument strings

**Returns:**
- `true` if configuration was loaded successfully

#### saveToFile(std::string filepath)
Save configuration to a JSON file.

**Parameters:**
- `filepath`: Path where configuration should be saved

#### set(std::string key, T value, ConfigSource source = ConfigSource::Default)
Set a configuration parameter.

**Template Parameters:**
- `T`: Type of the value (int64_t, double, float, std::string, etc.)

**Parameters:**
- `key`: Configuration parameter name
- `value`: Value to set
- `source`: Source of the configuration (Default, CommandLine, File)

#### getOr(std::string key, T defaultValue) const
Get a configuration parameter with a default value.

**Template Parameters:**
- `T`: Type of the value to retrieve

**Parameters:**
- `key`: Configuration parameter name
- `defaultValue`: Default value if key not found

**Returns:**
- Value associated with the key, or defaultValue if not found

#### has(std::string key) const
Check if a configuration key exists.

**Parameters:**
- `key`: Configuration parameter name to check

**Returns:**
- `true` if key exists, `false` otherwise

#### getKeys() const
Get all configuration keys.

**Returns:**
- Vector of all configuration key strings

#### clear()
Clear all configuration entries.

#### summary() const
Get a summary string of the configuration.

**Returns:**
- String containing all configuration parameters and values

#### operator<< (std::ostream& os, const Config& config)
Stream output operator for easy printing.

**Parameters:**
- `os`: Output stream
- `config`: Config object to print

### Configuration Parameters

#### Neural Network Parameters
- `random_seed`: Random number generator seed
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections
- `excitatory_neuron_percentage`: Percentage of excitatory neurons
- `inhibitory_neuron_percentage`: Percentage of inhibitory neurons

#### Neuron Parameters
- `neuron_resting_potential`: Resting membrane potential
- `neuron_threshold`: Firing threshold
- `neuron_refractory_period`: Refractory period duration
- `neuron_membrane_resistance`: Membrane resistance
- `neuron_capacitance`: Membrane capacitance

#### Synapse Parameters
- `synapse_weight_initial`: Initial synaptic weight
- `synapse_weight_range`: Range of synaptic weights
- `synapse_delay_min`: Minimum synaptic delay
- `synapse_delay_max`: Maximum synaptic delay
- `synapse_plasticity_enabled`: Whether synapse supports plasticity

#### Plasticity Parameters
- `stdp_ltp_weight`: STDP LTP learning rate
- `stdp_ltd_weight`: STDP LTD learning rate
- `stdp_time_constant`: STDP time constant
- `hebbian_learning_rate`: Hebbian learning rate
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

#### Neuromodulation Parameters
- `dopamine_baseline`: Baseline dopamine level
- `dopamine_sensitivity`: Dopamine sensitivity
- `curiosity_baseline`: Baseline curiosity level
- `novelty_threshold`: Novelty detection threshold

#### Memory Parameters
- `working_memory_capacity`: Working memory capacity
- `episodic_memory_capacity`: Episodic memory capacity
- `semantic_memory_capacity`: Semantic memory capacity
- `procedural_memory_capacity`: Procedural memory capacity
- `replay_frequency`: Memory replay frequency

#### Environment Parameters
- `environment_name`: Environment type name
- `environment_width`: Environment width
- `environment_height`: Environment height
- `vision_width`: Agent vision width
- `vision_height`: Agent vision height
- `max_energy`: Maximum agent energy

#### Simulation Parameters
- `max_simulation_steps`: Maximum simulation steps
- `simulation_time_limit`: Simulation time limit
- `checkpoint_interval`: Checkpoint saving interval
- `log_level`: Logging level (Debug, Info, Warning, Error)

## nlm::NeuronId

Unique identifier for neurons.

### Constructor
```cpp
NeuronId();
NeuronId(uint64_t value);
```

### Methods
- `uint64_t index() const`: Get neuron index
- `uint64_t getValue() const`: Get neuron value
- `void setValue(uint64_t value)`: Set neuron value
- `bool operator==(const NeuronId& other) const`: Equality comparison
- `bool operator!=(const NeuronId& other) const`: Inequality comparison

## nlm::SynapseId

Unique identifier for synapses.

### Similar to NeuronId

## nlm::RegionId

Unique identifier for brain regions.

### Similar to NeuronId

## nlm::PopulationId

Unique identifier for neuron populations.

### Similar to NeuronId

## nlm::DevelopmentalStage

Enumeration of developmental stages.

```cpp
enum DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

## nlm::FiringState

Enumeration of neuron firing states.

```cpp
enum FiringState {
    Resting,
    Active,
    Refractory,
    Inhibited
};
```

## nlm::NeuronType

Enumeration of neuron types.

```cpp
enum NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

## nlm::SynapseType

Enumeration of synapse types.

```cpp
enum SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};
```

## nlm::ActionType

Enumeration of action types.

```cpp
enum ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
};
```

## nlm::MotorCommand

Enumeration of motor commands.

```cpp
enum MotorCommand {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait
};
```

## nlm::WorldObjectType

Enumeration of world object types.

```cpp
enum WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker
};
```

# 2. Brain Classes

## nlm::Brain

Central neural simulation brain class implementing the main neural computation loop.

### Class Overview
The Brain class is the main simulation component implementing a complete artificial brain with neurons, synapses, regions, and all associated systems including plasticity, memory, neuromodulation, and cognition.

### Constructor
```cpp
Brain(std::shared_ptr<Config> config);
```

**Parameters:**
- `config`: Brain configuration

### Public Methods

#### initialize()
Initialize the brain with configuration parameters.

**Returns:**
- `true` if initialization successful, `false` otherwise

#### step(SimulationStep step, Timestamp time = 0.0)
Perform a simulation step.

**Parameters:**
- `step`: Current simulation step
- `time`: Current simulation time (optional)

#### receiveSensoryInput(const SensoryInput& input)
Inject sensory input into the brain.

**Parameters:**
- `input`: Sensory input to inject

#### produceAction()
Produce motor action based on neural activity.

**Returns:**
- Motor action based on brain state

#### reset()
Reset brain state to initial conditions.

#### save(std::string filepath)
Save brain state to file.

**Parameters:**
- `filepath`: Path where brain state should be saved

#### load(std::string filepath)
Load brain state from file.

**Parameters:**
- `filepath`: Path to file containing saved brain state

#### addRegion(std::string name = "")
Add a new neural region.

**Parameters:**
- `name`: Name for the new region (optional)

#### getRegion(RegionId id)
Get a region by ID.

**Parameters:**
- `id`: Region identifier

**Returns:**
- Pointer to the region, or nullptr if not found

#### getTotalNeuronCount() const
Get total neuron count across all regions.

**Returns:**
- Total number of neurons

#### getTotalSynapseCount() const
Get total synapse count across all regions.

**Returns:**
- Total number of synapses

#### getActiveNeuronCount() const
Get count of active neurons.

**Returns:**
- Number of active neurons

#### getFiringNeuronCount() const
Get count of currently firing neurons.

**Returns:**
- Number of firing neurons

#### getAverageFiringRate() const
Get average firing rate across all neurons.

**Returns:**
- Average firing rate in Hz

#### getTotalSpikeCount() const
Get total spike count since initialization.

**Returns:**
- Total number of spikes

#### getExcitationInhibitionRatio() const
Get excitation/inhibition balance ratio.

**Returns:**
- Excitation/inhibition ratio

#### getDevelopmentalStage() const
Get current developmental stage.

**Returns:**
- Current developmental stage

#### setDevelopmentalStage(DevelopmentalStage stage)
Set developmental stage.

**Parameters:**
- `stage`: Developmental stage to set

#### getConfig() const
Get brain configuration.

**Returns:**
- Brain configuration

#### logStatus() const
Log brain status and statistics.

#### injectCurrent(NeuronId neuron, double current)
Inject current into a specific neuron.

**Parameters:**
- `neuron`: Neuron ID
- `current`: Current to inject

#### injectCurrentToNeurons(NeuronType type, double current)
Inject current into all neurons of a specific type.

**Parameters:**
- `type`: Neuron type
- `current`: Current to inject

#### getSpikeSystem() const
Get spike system for spike analysis.

**Returns:**
- Spike system pointer

### Configuration Methods

#### configureNeuron(int64_t neuronCount, double timestep, double threshold)
Configure basic neuron parameters.

#### configurePlasticity(float stdpLtpWeight, float stdpLtdWeight, float hebbianRate)
Configure plasticity parameters.

#### configureMemory(size_t workingCapacity, size_t episodicCapacity)
Configure memory system parameters.

#### configureNeuromodulation(float dopamineBaseline, float curiosityLevel)
Configure neuromodulation parameters.

### Static Methods

#### createDefaultConfig()
Create default brain configuration.

**Returns:**
- Shared pointer to default configuration

### Usage Example
```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = nlm::Config::createDefaultConfig();
    config->set("neuron_count", 1000);
    config->set("region_count", 3);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Get statistics
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
    
    return 0;
}
```

## nlm::Neuron

Leaky Integrate-and-Fire (LIF) neuron model.

### Class Overview
Implements the LIF neuron model with membrane potential dynamics, spike generation, and refractory periods.

### Constructor
```cpp
Neuron(NeuronId id);
```

**Parameters:**
- `id`: Neuron identifier

### Methods

#### getId() const
Get neuron ID.

#### getMembranePotential() const
Get current membrane potential.

#### getThreshold() const
Get firing threshold.

#### getRefractoryTime() const
Get remaining refractory time.

#### getType() const
Get neuron type.

#### getState() const
Get current firing state.

#### injectCurrent(double current)
Inject current into neuron.

#### step(double dt)
Advance neuron state by dt time.

#### reset()
Reset neuron to resting state.

#### enablePlasticity(bool hebbian, bool stdp, bool structural)
Enable plasticity mechanisms.

#### getPlasticityEnabled() const
Check if plasticity is enabled.

#### getWeight() const
Get synaptic weight (for synapses).

#### setWeight(float weight)
Set synaptic weight.

#### isPlastic() const
Check if synapse is plastic.

### Constants

#### RESTING_POTENTIAL
Membrane resting potential (-70mV)

#### THRESHOLD
Firing threshold (-55mV)

#### CAPACITANCE
Membrane capacitance (250pF)

#### RESISTANCE
Membrane resistance (100MΩ)

## nlm::Synapse

Synaptic connection between neurons.

### Class Overview
Represents a synaptic connection with dynamic weight that can change through plasticity mechanisms.

### Constructor
```cpp
Synapse(NeuronId pre, NeuronId post, SynapseType type);
```

**Parameters:**
- `pre`: Pre-synaptic neuron ID
- `post`: Post-synaptic neuron ID
- `type`: Synapse type

### Methods

#### getPreSynapticId() const
Get pre-synaptic neuron ID.

#### getPostSynapticId() const
Get post-synaptic neuron ID.

#### getType() const
Get synapse type.

#### getWeight() const
Get current synaptic weight.

#### setWeight(float weight)
Set synaptic weight.

#### getDelay() const
Get synaptic delay.

#### setDelay(double delay)
Set synaptic delay.

#### isEnabled() const
Check if synapse is active.

#### enable(bool enable)
Enable or disable synapse.

#### enablePlasticity(bool hebbian, bool stdp, bool structural)
Enable plasticity mechanisms.

#### isPlastic() const
Check if synapse is plastic.

#### getPlasticityEnabled() const
Get plasticity enabled flags.

#### getLastUpdateTime() const
Get time since last weight update.

## nlm::NeuralRegion

Collection of neurons forming a functional unit.

### Class Overview
Groups neurons into functional regions based on their types and connectivity patterns.

### Constructor
```cpp
NeuralRegion(RegionId id, std::string name = "");
```

**Parameters:**
- `id`: Region identifier
- `name`: Region name (optional)

### Methods

#### getId() const
Get region ID.

#### getName() const
Get region name.

#### addNeuron(std::shared_ptr<Neuron> neuron)
Add neuron to region.

#### removeNeuron(NeuronId id)
Remove neuron from region.

#### getNeuron(NeuronId id)
Get neuron by ID.

#### getAllNeurons() const
Get all neurons in region.

#### getNeuronsByType(NeuronType type) const
Get neurons by type.

#### addSynapse(std::shared_ptr<Synapse> synapse)
Add synapse to region.

#### getSynapse(SynapseId id)
Get synapse by ID.

#### getSynapses() const
Get all synapses in region.

#### getNeuronCount() const
Get neuron count.

#### getSynapseCount() const
Get synapse count.

#### getActivityLevel() const
Get region activity level.

## nlm::NeuralPopulation

Group of neurons with similar properties.

### Similar to NeuralRegion

## nlm::SpikeSystem

System for spike detection and analysis.

### Class Overview
Provides spike detection, analysis, and statistics for neural activity monitoring.

### Methods

#### getMostActiveNeurons(size_t count) const
Get most active neurons.

#### getFiringRate(NeuronId neuron) const
Get firing rate of specific neuron.

#### getTotalSpikes() const
Get total spike count.

#### getActiveNeurons() const
Get all active neurons.

## nlm::DevelopmentSystem

System for brain development and maturation.

### Class Overview
Implements developmental stages and age-dependent changes in brain properties.

### Methods

#### getCurrentStage() const
Get current developmental stage.

#### advanceStage()
Advance to next developmental stage.

#### getPlasticityModulation() const
Get developmental modulation of plasticity.

## nlm::Neuromodulator

Base class for neuromodulators.

### Similar to DevelopmentSystem

## nlm::Dopamine

Dopamine neuromodulator.

### Similar to Neuromodulator

## nlm::Curiosity

Curiosity neuromodulator.

### Similar to Neuromodulator

## nlm::Novelty

Novelty detection neuromodulator.

### Similar to Neuromodulator

## nlm::PredictionError

Prediction error neuromodulator.

### Similar to Neuromodulator

## nlm::Reward

Reward neuromodulator.

### Similar to Neuromodulator

# 3. Agent Classes

## nlm::AgentBrain

Agent brain interface connecting NLM brain to world.

### Class Overview
Provides interface for agents to process sensory input and produce motor output using NLM brain.

### Constructor
```cpp
AgentBrain(std::shared_ptr<Brain> brain);
```

**Parameters:**
- `brain`: Underlying NLM brain

### Methods

#### initialize(const Environment& environment)
Initialize agent with environment.

**Parameters:**
- `environment`: Environment for agent

#### processSensoryInput(const SensoryPercept& percept)
Process sensory percept and inject into brain.

**Parameters:**
- `percept`: Sensory percept to process

#### decodeMotorCommand()
Decode brain motor activity into motor command.

**Returns:**
- Motor command based on brain state

#### applyRewardModulation(float reward, float predictedReward)
Apply reward-based neuromodulation.

**Parameters:**
- `reward`: Actual reward received
- `predictedReward`: Predicted reward

#### updateDevelopment(float timestep)
Update development system.

**Parameters:**
- `timestep`: Time step for development

#### getNeuromodulationLevel() const
Get current neuromodulation level.

**Returns:**
- Neuromodulation level

#### getCuriosityLevel() const
Get curiosity level.

**Returns:**
- Curiosity level

#### getNoveltyLevel() const
Get novelty level.

**Returns:**
- Novelty level

#### getPredictionError() const
Get prediction error.

**Returns:**
- Prediction error

#### reset()
Reset agent for new episode.

#### enableRewardModulation(bool enable)
Enable reward modulation.

**Parameters:**
- `enable`: Whether to enable reward modulation

#### enableStructuralPlasticity(bool enable)
Enable structural plasticity.

**Parameters:**
- `enable`: Whether to enable structural plasticity

#### enableDevelopment(bool enable)
Enable development.

**Parameters:**
- `enable`: Whether to enable development

#### enableCuriosity(bool enable)
Enable curiosity.

**Parameters:**
- `enable`: Whether to enable curiosity

#### isRewardModulationEnabled() const
Check if reward modulation is enabled.

**Returns:**
- `true` if reward modulation enabled

#### isStructuralPlasticityEnabled() const
Check if structural plasticity is enabled.

**Returns:**
- `true` if structural plasticity enabled

#### isDevelopmentEnabled() const
Check if development is enabled.

**Returns:**
- `true` if development enabled

#### isCuriosityEnabled() const
Check if curiosity is enabled.

**Returns:**
- `true` if curiosity enabled

### Usage Example
```cpp
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>

int main() {
    // Create brain and agent
    auto config = nlm::Config::createDefaultConfig();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create and configure world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Initialize agent
    agent->initialize(*world);
    
    // Enable learning
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run agent loop
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * 0.1);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation based on outcome
        float reward = world->getCurrentReward();
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    return 0;
}
```

## nlm::AgentBody

Agent body state.

### Class Overview
Represents the physical state of an agent including position, orientation, velocity, and energy.

### Methods

#### getX() const
Get X position.

#### getY() const
Get Y position.

#### getOrientation() const
Get orientation angle.

#### getVelocityX() const
Get X velocity.

#### getVelocityY() const
Get Y velocity.

#### getAngularVelocity() const
Get angular velocity.

#### getEnergy() const
Get energy level.

#### getHealth() const
Get health level.

#### getAge() const
Get age.

#### getIsMoving() const
Check if agent is moving.

#### getIsTurning() const
Check if agent is turning.

#### reset()
Reset agent body to initial state.

## nlm::SensoryPercept

Sensory percept data.

### Methods

#### getVision() const
Get vision data.

#### setVision(const Vision& vision)
Set vision data.

#### getVisionWidth() const
Get vision width.

#### getVisionHeight() const
Get vision height.

#### getTouch() const
Get touch data.

#### setTouch(const std::vector<float>& touch)
Set touch data.

#### getInternal() const
Get internal signals.

#### setInternal(const InternalSignals& internal)
Set internal signals.

#### getProprioception() const
Get proprioception data.

#### setProprioception(const std::vector<float>& proprioception)
Set proprioception data.

#### getAudio() const
Get audio data.

#### setAudio(const Audio& audio)
Set audio data.

#### getAllSignals() const
Get all sensory signals.

#### getTimestamp() const
Get timestamp.

#### setTimestamp(double timestamp)
Set timestamp.

## nlm::Action

Action representation for motor output.

### Methods

#### getType() const
Get action type.

#### setType(ActionType type)
Set action type.

#### getParameters() const
Get action parameters.

#### setParameters(const std::vector<float>& parameters)
Set action parameters.

#### getName() const
Get action name.

#### clone() const
Create a copy of this action.

# 4. Environment Classes

## nlm::Environment

Base class for environments.

### Similar to AgentBrain

## nlm::SimpleWorld

Simple 2D world for NLM simulation.

### Constructor
```cpp
SimpleWorld();
SimpleWorld(std::shared_ptr<Config> config);
```

### Methods

#### configure(size_t width, size_t height, size_t visionWidth, size_t visionHeight)
Configure world dimensions and agent vision.

**Parameters:**
- `width`: World width
- `height`: World height
- `visionWidth`: Agent vision width
- `visionHeight`: Agent vision height

#### reset()
Reset world to initial state.

#### update(double timestep)
Update world state.

**Parameters:**
- `timestep`: Time step for update

#### applyMotorCommand(const Action& cmd, double currentTime)
Apply motor command in world.

**Parameters:**
- `cmd`: Motor command to apply
- `currentTime`: Current simulation time

#### getSensoryPercept()
Get sensory percept from world.

**Returns:**
- Sensory percept containing vision, touch, etc.

#### getAgentBody()
Get agent body state.

**Returns:**
- Agent body state

#### addObject(const WorldObject& obj)
Add object to world.

**Parameters:**
- `obj`: Object to add

#### removeObject(float x, float y)
Remove object at position.

**Parameters:**
- `x`: X position
- `y`: Y position

#### isValidPosition(float x, float y) const
Check if position is valid.

**Parameters:**
- `x`: X position
- `y`: Y position

**Returns:**
- `true` if position is valid

#### getWidth() const
Get world width.

**Returns:**
- World width

#### getHeight() const
Get world height.

**Returns:**
- World height

#### getMaxEnergy() const
Get maximum energy.

**Returns:**
- Maximum energy

#### setMaxEnergy(float e)
Set maximum energy.

**Parameters:**
- `e`: Maximum energy

#### getEnergyDecayRate() const
Get energy decay rate.

**Returns:**
- Energy decay rate

#### setEnergyDecayRate(float r)
Set energy decay rate.

**Parameters:**
- `r`: Energy decay rate

#### getSimulationTime() const
Get simulation time.

**Returns:**
- Current simulation time

#### setRandomSeed(uint64_t seed)
Set random seed.

**Parameters:**
- `seed`: Random seed

#### getRandomSeed() const
Get random seed.

**Returns:**
- Random seed

### Usage Example
```cpp
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/agent/AgentBrain.hpp>

int main() {
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Set agent starting position
    world->setAgentStart(10, 10);
    
    // Add some objects to world
    nlm::WorldObject resource;
    resource.x = 5; resource.y = 5; resource.type = nlm::WorldObjectType::Resource; resource.value = 100.0f;
    world->addObject(resource);
    
    nlm::WorldObject hazard;
    hazard.x = 15; hazard.y = 15; hazard.type = nlm::WorldObjectType::Hazard; hazard.value = -50.0f;
    world->addObject(hazard);
    
    // Create agent
    auto brain = std::make_shared<nlm::Brain>(nlm::Config::createDefaultConfig());
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run simulation
    for (int step = 0; step < 500; ++step) {
        // Update world
        world->update(0.1);
        
        // Get percept
        auto percept = world->getSensoryPercept();
        
        // Process with agent
        agent->processSensoryInput(percept);
        
        // Run brain
        brain->step(step, step * 0.1);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Log every 50 steps
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Position (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ") "
                      << "Energy: " << world->getAgentBody().energy << std::endl;
        }
    }
    
    return 0;
}
```

## nlm::WorldObject

World object representation.

### Methods

#### getX() const
Get X position.

#### getY() const
Get Y position.

#### getRadius() const
Get radius.

#### getType() const
Get object type.

#### getValue() const
Get value.

#### getActive() const
Check if object is active.

#### setX(float x)
Set X position.

#### setY(float y)
Set Y position.

#### setRadius(float radius)
Set radius.

#### setType(WorldObjectType type)
Set object type.

#### setValue(float value)
Set value.

#### setActive(bool active)
Set active state.

# 5. Experiment Classes

## nlm::Experiment

Base class for experiments.

### Similar to VisualizationSystem

## nlm::ExperimentRunner

Runs experiments and collects data.

### Similar to UnifiedExperimentRunner

## nlm::Metrics

Experiment metrics collection.

### Similar to RuntimeInspector

# 6. Visualization Classes

## nlm::VisualizationSystem

Main visualization system class.

### Similar to VisualizationSystem

# 7. Utility Classes

## nlm::SensoryInput

Base class for sensory input.

### Methods

#### getType() const
Get sensory input type.

#### getData() const
Get raw data as vector.

#### getDimensions() const
Get dimensionality.

#### getTimestamp() const
Get timestamp.

#### setTimestamp(double timestamp)
Set timestamp.

## nlm::Vision

Vision sensory input.

### Constructor
```cpp
Vision();
Vision(size_t width, size_t height, size_t channels = 3);
```

### Methods

#### setData(const std::vector<float>& data)
Set vision data.

#### getWidth() const
Get width.

#### getHeight() const
Get height.

#### getChannels() const
Get channels.

## nlm::Audio

Audio sensory input.

### Constructor
```cpp
Audio();
Audio(size_t sampleRate, size_t numSamples);
```

### Methods

#### setData(const std::vector<float>& data)
Set audio data.

#### setSampleRate(size_t sampleRate)
Set sample rate.

#### getSampleRate() const
Get sample rate.

#### getNumSamples() const
Get number of samples.

## nlm::InternalSignals

Internal signals sensory input.

### Methods

#### addSignal(float value)
Add signal value.

#### clearSignals()
Clear all signals.

## nlm::ActionResult

Action result from world.

### Methods

#### getReward() const
Get reward.

#### getSuccess() const
Get success flag.

#### getMessage() const
Get message.

# 8. Configuration System

## nlm::ConfigSource

Enumeration of configuration sources.

```cpp
enum ConfigSource {
    Default,
    CommandLine,
    File,
    Program
};
```

## nlm::Logger

Logging system for NLM.

### Similar to RuntimeInspector

## nlm::SimulationClock

Simulation time management.

### Methods

#### SimulationClock(double timestep)
Constructor.

#### step()
Advance simulation time.

#### getCurrentTime() const
Get current time.

#### getTimestep() const
Get timestep.

# 9. Example Code

## Complete Brain Simulation Example
```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/core/Config/Config.hpp>
#include <iostream>

int main() {
    // Create configuration
    auto config = nlm::Config::createDefaultConfig();
    config->set("neuron_count", 2000);
    config->set("region_count", 5);
    config->set("connection_probability", 0.2f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(50, 50, 16, 16);
    world->reset();
    
    // Initialize agent
    agent->initialize(*world);
    
    // Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run simulation
    std::cout << "Starting NLM brain simulation..." << std::endl;
    std::cout << "Configuration: " << config->summary() << std::endl;
    std::cout << std::endl;
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process with agent
        agent->processSensoryInput(percept);
        
        // Run brain
        brain->step(step, step * 0.1);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward and apply neuromodulation
        float reward = world->getCurrentReward();
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getFiringNeuronCount() << " neurons firing, "
                      << brain->getTotalSpikeCount() << " total spikes, "
                      << "Rate: " << std::fixed << std::setprecision(3) 
                      << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Final brain status:" << std::endl;
    brain->logStatus();
    
    return 0;
}
```

## Command-Line Usage Example
```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config my_config.cfg

# Run with specific parameters
./nlm --neurons 5000 --regions 10 --timestep 0.01 --seed 12345

# Run with preset mode
./nlm --preset comprehensive

# Run with output file
./nlm --output results.txt --verbose

# Set configuration parameters via command line
./nlm --set=neuron_count=3000 --set=connection_probability=0.15 --set=stdp_ltp_weight=0.03
```

## Configuration File Example
```ini
# NLM Configuration File
# This is the default configuration for Phase 2

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## Python API Example
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

# Important Notes

1. **Memory Management**: Use `std::shared_ptr` for all NLM objects to ensure proper memory management and prevent leaks.

2. **Thread Safety**: The NLM brain is not thread-safe. Use appropriate synchronization when accessing brain state from multiple threads.

3. **Configuration Validation**: Always validate configuration parameters before creating brain instances.

4. **Error Handling**: Check return values of all NLM methods that can fail (e.g., `brain->initialize()`).

5. **Performance**: For large-scale simulations, consider using the runtime inspector and visualization system for performance monitoring.

# Quick Reference

| Class | Purpose | Main Method |
|-------|---------|-------------|
| `Brain` | Main neural simulation brain | `step()` |
| `AgentBrain` | Agent interface for brain-world interaction | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | 2D simulation environment | `update()`, `applyMotorCommand()` |
| `Config` | Configuration management | `set()`, `getOr()` |
| `VisualizationSystem` | Real-time visualization | `update()`, `render()` |
| `RuntimeInspector` | Runtime introspection and debugging | `captureSnapshot()` |

# Conclusion

The NLM C++ API provides a comprehensive framework for creating artificial developmental brains. With its modular architecture, extensive configuration options, and powerful introspection tools, NLM enables researchers to explore complex neural systems and emergent behaviors.

For more detailed information, consult the individual class documentation and the example code provided throughout this document.