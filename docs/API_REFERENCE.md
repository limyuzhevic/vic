# NLM API Reference

## Overview

This document provides comprehensive API documentation for NLM (Neural Learning Machine), an experimental artificial developmental brain project. The API is organized around core brain components that implement spiking neural dynamics, learning, memory, and cognition.

## Table of Contents

- [Core Brain Components](#core-brain-components)
- [Neuron Layer](#neuron-layer)
- [Synaptic Connectivity](#synaptic-connectivity)
- [Plasticity Systems](#plasticity-systems)
- [Memory Systems](#memory-systems)
- [Prediction Systems](#prediction-systems)
- [Cognitive Systems](#cognitive-systems)
- [Neuromodulation](#neuromodulation)
- [Environment and World Systems](#environment-and-world-systems)
- [Configuration](#configuration)
- [Experiments](#experiments)

## Core Brain Components

### Brain
The main brain coordinator that manages neurons, regions, and processing loops.

```cpp
Brain(const std::shared_ptr<Config>& config)
Brain::initialize()
Brain::step(SimulationStep step, double time = 0.0)
Brain::reset()
Brain::getTotalNeuronCount() const
Brain::getTotalSynapseCount() const
Brain::getFiringNeuronCount() const
Brain::getTotalSpikeCount() const
Brain::getAverageFiringRate() const
Brain::getDevelopmentalStage() const
```

**Purpose:** Coordinates all brain regions and implements the main processing loop. Manages neuron dynamics, plasticity, memory systems, and cognitive functions.

**Key Features:**
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
- Structural plasticity (synaptogenesis, pruning)
- Integration of prediction, memory, and cognitive systems

### Config
Configuration management system for all NLM parameters.

```cpp
Config()
Config::loadFromFile(const std::string& filepath)
Config::loadFromArgs(int argc, char** argv)
Config::set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)
Config::get<T>(const std::string& key) const
Config::getOr<T>(const std::string& key, const T& defaultValue) const
Config::has(const std::string& key) const
```

**Purpose:** Centralized configuration management with type safety, validation, and support for multiple sources (files, command line, runtime).

**Key Features:**
- Type-safe parameter storage (int, double, string, bool, arrays)
- Parameter validation with custom validators
- Support for default, file, command line, and runtime sources
- Section-based configuration organization
- Required parameter checking

## Neuron Layer

### Neuron
Individual neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.

```cpp
Neuron(NeuronId id)
Neuron::setType(NeuronType type)
Neuron::setMembranePotential(MembranePotential potential)
Neuron::addToMembranePotential(MembranePotential delta)
Neuron::setThreshold(MembranePotential threshold)
Neuron::setFiringState(FiringState state)
Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt)
Neuron::step(Timestamp currentTime)
Neuron::initializeRandom(RandomGenerator& rng)
Neuron::recordSpike(Timestamp timestamp)
```

**Purpose:** Biological-inspired neuron with LIF dynamics, refractory periods, and spike-frequency adaptation.

**Key Features:**
- Leaky Integrate-and-Fire membrane dynamics
- Refractory period handling
- Spike-frequency adaptation
- Multiple neuron types (excitatory, inhibitory, sensory, motor, etc.)
- Spike history tracking

### NeuronState
Internal state structure for neuron properties and dynamics.

## Synaptic Connectivity

### Synapse
Synaptic connection between neurons with delays and short-term plasticity.

```cpp
Synapse(SynapseId id, NeuronId source, NeuronId destination)
Synapse::getWeight() const
Synapse::setWeight(SynapticWeight weight)
Synapse::addToWeight(SynapticWeight delta)
Synapse::getDelay() const
Synapse::setDelay(Delay delay)
Synapse::getType() const
Synapse::setType(SynapseType type)
Synapse::recordPreSpike(Timestamp timestamp)
Synapse::recordPostSpike(Timestamp timestamp)
Synapse::step(Timestamp currentTime)
```

**Purpose:** Implements synaptic transmission with realistic dynamics including delays, weight changes, and short-term plasticity.

**Key Features:**
- Synaptic delays (1-100+ steps)
- Weight dynamics with bounds (-1.0 to +1.0 typical)
- Short-term facilitation and depression
- Eligibility traces for reward-modulated learning
- Spike history for STDP learning

### NeuralRegion
Group of neurons with shared properties and regional organization.

```cpp
RegionId regionId;
RegionId populationId;
std::vector<Neuron*> neurons;
std::vector<Synapse*> synapses;
```

**Purpose:** Represents brain regions with functionally specialized neurons and intra-regional connectivity.

## Plasticity Systems

### PlasticityRule (Abstract Base)
Abstract base class for all plasticity mechanisms.

```cpp
virtual void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes, TimestepDuration dt) = 0;
virtual const char* getName() const = 0;
```

### STDP (Spike-Timing-Dependent Plasticity)
Hebbian learning that depends on precise spike timing.

```cpp
STDP()
void STDP::setLearningRate(float rate)
float STDP::getLearningRate() const
void STDP::setTimeConstant(float tau)
float STDP::getTimeConstant() const
```

**Learning Rule:** LTP when pre fires before post (within ~20ms), LTD when post fires before pre.

### Hebbian (Covariance-based)
Simple correlation-based learning rule.

```cpp
Hebbian()
void Hebbian::setLearningRate(float rate)
float Hebbian::getLearningRate() const
```

**Learning Rule:** Δw = η × (coactivity - baseline) - implements "fire together, wire together".

### StructuralPlasticity
Dynamic synapse formation and elimination.

```cpp
StructuralPlasticity()
void StructuralPlasticity::setSynaptogenesisRate(float rate)
void StructuralPlasticity::setPruningRate(float rate)
```

**Key Features:**
- Probabilistic synapse formation based on activity
- Activity-dependent pruning of weak synapses
- Developmental modulation of plasticity rates

## Memory Systems

### NeuralWorkingMemory
Persistent neural activity for temporary information maintenance.

```cpp
NeuralWorkingMemory()
void NeuralWorkingMemory::storePattern(const std::vector<float>& pattern)
std::vector<float> NeuralWorkingMemory::retrievePattern(size_t index) const
void NeuralWorkingMemory::update()  // Called every timestep
```

**Mechanism:** Recurrent connections with competitive dynamics for selective retention.

### NeuralEpisodicMemory
Experience encoding with pattern completion and replay.

```cpp
NeuralEpisodicMemory()
void NeuralEpisodicMemory::encodeExperience(const std::vector<float>& percept)
std::vector<float> NeuralEpisodicMemory::recallExperience(const std::vector<float>& cue)
void NeuralEpisodicMemory::replay()  // During consolidation
```

**Key Features:**
- Neural pattern storage without symbolic representations
- Temporal and spatial indexing
- Pattern completion from partial cues

### NeuralAssociativeMemory
Experience-based associations between patterns.

```cpp
NeuralAssociativeMemory()
void NeuralAssociativeMemory::storeAssociation(const std::vector<float>& key, 
                                               const std::vector<float>& value)
std::vector<float> NeuralAssociativeMemory::retrieveValue(const std::vector<float>& key)
```

### ConceptFormation
Pattern discovery and prototype formation without labels.

```cpp
ConceptFormation()
void ConceptFormation::processPattern(const std::vector<float>& pattern)
std::vector<float> ConceptFormation::getPrototype(size_t conceptId) const
```

## Prediction Systems

### NeuralPrediction
Temporal sequence learning and prediction.

```cpp
NeuralPrediction()
void NeuralPrediction::learnSequence(const std::vector<std::vector<float>>& sequence)
std::vector<float> NeuralPrediction::predictNext(const std::vector<float>& state) const
float NeuralPrediction::getPredictionConfidence(const std::vector<float>& state) const
```

### ActionConsequencePredictor
Association between actions and their effects.

```cpp
ActionConsequencePredictor()
void ActionConsequencePredictor::learnActionEffect(const Action& action, 
                                                   const std::vector<float>& result)
std::vector<float> ActionConsequencePredictor::predictOutcome(const Action& action) const
```

## Cognitive Systems

### AttentionalSelection
Competitive selection of neural representations.

```cpp
AttentionalSelection()
void AttentionalSelection::applySelection(const std::vector<float>& activations)
size_t AttentionalSelection::getSelectedIndex() const
```

**Mechanism:** Lateral inhibition with top-down and bottom-up biases - NOT Transformer attention.

### NeuralPlanner
Multi-step action sequence evaluation.

```cpp
NeuralPlanner()
void NeuralPlanner::updatePlannerState(const std::vector<float>& currentState)
Action NeuralPlanner::selectAction() const
```

### SelfModel
Sensorimotor self-awareness and agency detection.

```cpp
SelfModel()
void SelfModel::learnForwardModel(const Action& action, 
                                  const std::vector<float>& sensoryResult)
bool SelfModel::isSelfGenerated(const std::vector<float>& change) const
```

### SocialLearning
Observation and imitation of other agents.

```cpp
SocialLearning()
void SocialLearning::observeAction(const AgentAction& action, 
                                   const std::vector<float>& outcome)
void SocialLearning::imitate(const AgentAction& action) const
```

## Neuromodulation

### Neuromodulator (Abstract Base)
Abstract base class for all neuromodulators.

```cpp
virtual void update(TimestepDuration dt) = 0;
virtual void signalReward(float reward) = 0;
virtual void signalRewardPredictionError(float error) = 0;
virtual float getLevel() const = 0;
virtual const char* getName() const = 0;
```

### Dopamine
Reward prediction error signaling.

```cpp
Dopamine()
void Dopamine::signalReward(float reward)
void Dopamine::signalRewardPredictionError(float error)
float Dopamine::getPlasticityFactor() const
```

### Novelty
Novelty detection for exploration.

```cpp
Novelty()
void Novelty::update(const std::vector<float>& currentState)
float Novelty::getLevel() const
```

### Curiosity
Exploration motivation based on novelty/error.

```cpp
Curiosity()
void Curiosity::update(float predictionError)
float Curiosity::getDriveLevel() const
```

## Environment and World Systems

### Environment (Abstract)
Abstract base class for all environments.

```cpp
virtual void reset() = 0;
virtual void update(TimestepDuration dt) = 0;
virtual std::vector<float> getSensoryPercept() const = 0;
virtual Action applyMotorCommand(const Action& action, Timestamp time) = 0;
```

### SimpleWorld
2D environment with objects, vision, and proprioception.

```cpp
SimpleWorld()
void SimpleWorld::configure(int width, int height, 
                            int visionWidth, int visionHeight)
void SimpleWorld::reset()
void SimpleWorld::update(TimestepDuration dt)
std::vector<float> SimpleWorld::getSensoryPercept() const
Action SimpleWorld::applyMotorCommand(const Action& action, Timestamp time)
```

### SensoryInput (Abstract)
Abstract base for sensory modalities.

```cpp
virtual void update(const Environment& env) = 0
virtual std::vector<float> getData() const = 0
virtual SensorType getSensorType() const = 0
```

### Vision
Visual sensory input processing.

```cpp
Vision()
void Vision::setFOV(float fov)
void Vision::setResolution(int width, int height)
```

### MotorSystem
Neural-to-action conversion.

```cpp
MotorSystem()
Action MotorSystem::decode(std::vector<float> neuralActivity) const
```

## Configuration Parameters

### Core Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep (seconds) |
| `neuron_count` | int64_t | 1000 | Total number of neurons |
| `region_count` | int64_t | 1 | Number of brain regions |
| `connection_probability` | float | 0.05 | Probability of connection between neurons |

### Neuron Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron.membrane_capacitance` | float | 1.0f | Membrane capacitance (nF) |
| `neuron.time_constant` | float | 20.0f | Membrane time constant (ms) |
| `neuron.resting_potential_default` | float | -70.0f | Resting potential (mV) |
| `neuron.threshold_default` | float | -55.0f | Default firing threshold (mV) |
| `neuron.refractory_period_default` | int | 5 | Default refractory period (steps) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau` | float | 20.0f | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01f | Hebbian learning rate |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0f | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Experiments

### Experiment (Abstract)
Abstract base class for all experiment types.

```cpp
virtual void setup() = 0
virtual void run(SimulationStep step) = 0
virtual void analyze() = 0
```

### ExperimentRunner
Manages experiment execution and metrics collection.

```cpp
ExperimentRunner()
void ExperimentRunner::addExperiment(std::unique_ptr<Experiment> experiment)
void ExperimentRunner::run(SimulationStep steps)
void ExperimentRunner::saveResults(const std::string& filename) const
```

## Data Types

### NeuronId, SynapseId, RegionId, PopulationId
Strongly-typed identifiers for brain components.

### NeuronType
Enumeration of neuron functional types.

```cpp
enum class NeuronType {
    Excitatory,      // Positive feedback connections
    Inhibitory,      // Negative feedback connections
    Sensory,         // External input processing
    Motor,           // Action output generation
    Modulatory,      // Neuromodulation signaling
    Internal         // Internal brain processing
};
```

### DevelopmentalStage
Brain development stages.

```cpp
enum class DevelopmentalStage {
    Initial,         // Embryonic stage
    CriticalPeriod,  // High plasticity phase
    Maturation,      // Specialization phase
    Adult,           // Stable function
    Aging            // Decline phase
};
```

## Using the API

### Basic Brain Setup

```cpp
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 1000, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world and agent interface
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Create agent interface
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        world->update(0.1);  // Update environment
        agent->processSensoryInput(world->getSensoryPercept());  // Process input
        brain->step(step);  // Neural computation
        auto action = agent->decodeMotorCommand();  // Select action
        world->applyMotorCommand(action, step);  // Execute action
    }
    
    return 0;
}
```

### Configuration File

```
# Simple configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1

# Neuron parameters
neuron.time_constant = 20.0f
neuron.resting_potential_default = -70.0f
neuron.threshold_default = -55.0f

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.01f

# Neuromodulation parameters
neuromod.dopamine.scale = 1.0f
neuromod.curiosity.enable = true
```

## Error Handling

The NLM API uses exceptions for error conditions:

```cpp
try {
    brain->initialize();
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

Common error codes:
- "Configuration validation failed for key: [key]"
- "Required configuration parameter missing: [key]"
- "Failed to initialize brain!"
- "Load configuration from file failed"

## Performance Considerations

### Memory Management
- Use shared_ptr for brain and world objects
- Synapse and neuron objects are managed internally
- Configure neuron_count appropriately for available memory

### Simulation Speed
- Event-driven spike propagation is efficient
- Use smaller region_count for faster initialization
- Disable unused plasticity mechanisms when not needed

### Parallel Processing
- Some components support parallel processing
- Check documentation for specific parallelism options
- Profile to identify bottlenecks

## Advanced Usage

### Multiple Regions

```cpp
// Create brain with multiple regions
config->set("region_count", 3, ConfigSource::Default);

// Configure regional properties
config->set("region.1.neuron_count", 300, ConfigSource::Default);
config->set("region.2.neuron_count", 300, ConfigSource::Default);
config->set("region.3.neuron_count", 400, ConfigSource::Default);
```

### Custom Plasticity Rules

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, TimestepDuration dt) override {
        // Custom learning rule implementation
    }
    
    const char* getName() const override { return "Custom"; }
};

// Enable custom plasticity
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(false, false, false);
    }
}
// Custom plasticity would be enabled through configuration or registration
```

### Checkpointing

```cpp
// Save brain state
brain->save("checkpoint.bin");

// Load brain state
brain->reset();
brain->initialize();
brain->load("checkpoint.bin");
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [HOW_TO_USE.md](HOW_TO_USE.md) - Python API usage
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization

## API Changes

### Versioning
NLM uses semantic versioning. Major version changes may include breaking API changes. Check CHANGELOG.md for detailed changes.

### Deprecated APIs
Deprecated methods are marked with `// DEPRECATED` comments and have reduced efficiency. Use newer APIs instead.

### Experimental APIs
Some APIs are marked experimental and may change without notice. Use only in development code.

## License

See LICENSE file for copyright and license information.
