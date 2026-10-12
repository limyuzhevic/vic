# NLM API Reference

## Overview

This document provides comprehensive API reference for the NLM (Neural Learning Machine) project. NLM is a brain-inspired spiking neural network simulator with multiple layers of abstraction, from low-level neuron dynamics to high-level cognitive functions.

## API Structure

NLM APIs are organized into several modules:

### 1. Core Module (`nlm_core`)
- Static library containing fundamental brain components
- **Headers**: `src/core/`, `src/brain/`, `src/dynamics/`, `src/plasticity/`, etc.

### 2. Agent Module (`nlm_agent`)
- Brain-world interface for perception and action
- **Headers**: `src/agent/`

### 3. World Module (`nlm_world`)
- Environment simulation and world management
- **Headers**: `src/world/`

### 4. Phase Modules
- Specialized experiment implementations
- **Headers**: `src/experiments/` (Phase 3, Phase 4, Phase 6)

### 5. Python Bindings
- Complete Python API via pybind11
- **Module**: `pynlm`

---

## Core C++ API

### Brain Class (`src/brain/Brain.hpp`)

The Brain class is the central coordinator of neural processing in NLM.

#### Creation and Initialization

```cpp
// Create brain with default configuration
Brain brain(cfg);

// Initialize brain (must be called before simulation)
bool Brain::initialize()

// Reset brain to initial state
void Brain::reset()
```

#### Simulation Control

```cpp
// Run one simulation step
void Brain::step(SimulationStep step, Timestamp time = 0.0)

// Get current neuron statistics
uint64_t Brain::getTotalNeuronCount() const
uint64_t Brain::getTotalSynapseCount() const
uint64_t Brain::getFiringNeuronCount() const
float Brain::getAverageFiringRate() const
```

#### Memory Systems

```cpp
// Access working memory
class WorkingMemory* Brain::getWorkingMemory() const

// Access episodic memory
class EpisodicMemory* Brain::getEpisodicMemory() const

// Access semantic memory
class SemanticMemory* Brain::getSemanticMemory() const

// Access procedural memory
class ProceduralMemory* Brain::getProceduralMemory() const
```

#### Neuromodulation Systems

```cpp
// Access dopamine system
class Dopamine* Brain::getDopamine() const

// Access curiosity system
class Curiosity* Brain::getCuriosity() const

// Access prediction error system
class PredictionError* Brain::getPredictionError() const

// Access novelty detection system
class Novelty* Brain::getNovelty() const
```

#### Cognition Systems

```cpp
// Access neural planner
class NeuralPlanner* Brain::getPlanner() const

// Access concept formation system
class ConceptFormation* Brain::getConceptFormation() const

// Access attention system
class Attention* Brain::getAttention() const

// Access development system
class DevelopmentSystem* Brain::getDevelopmentSystem() const

// Access prediction system
class PredictionSystem* Brain::getPredictionSystem() const
```

#### Persistence

```cpp
// Save brain state to file
bool Brain::save(const std::string& filepath) const

// Load brain state from file
bool Brain::load(const std::string& filepath)
```

#### Regions

```cpp
// Add a neural region
uint64_t Brain::addRegion(const std::string& name)

// Get neural region by ID
const NeuralRegion* Brain::getRegion(uint64_t region_id) const
```

### Config Class (`src/core/Config/Config.hpp`)

Configuration management system for NLM parameters.

#### Creation

```cpp
// Create default configuration
std::shared_ptr<Config> Config::createDefault()

// Create empty configuration
Config cfg
```

#### Configuration Access

```cpp
// Check if configuration parameter exists
bool Config::has(const std::string& key) const

// Get configuration keys
std::vector<std::string> Config::getKeys() const

// Get value with default
T Config::get(const std::string& key, const T& default_value) const
```

#### Configuration Modification

```cpp
// Set configuration parameter
void Config::set(const std::string& key, const T& value)

// Set nested parameter
void Config::set(const std::string& parent, const std::string& child, const T& value)
```

#### File I/O

```cpp
// Load configuration from JSON file
bool Config::loadFromFile(const std::string& filepath)

// Save configuration to JSON file
bool Config::saveToFile(const std::string& filepath)
```

### AgentBrain Class (`src/agent/AgentBrain.hpp`)

Interface between brain and world for perception and action.

#### Creation and Initialization

```cpp
// Create agent-brain interface
AgentBrain agent(brain)

// Initialize agent with world
void AgentBrain::initialize(const World& world)
```

#### Sensory Processing

```cpp
// Process sensory input from world
void AgentBrain::processSensoryInput(const SensoryPercept& percept)
```

#### Motor Decoding

```cpp
// Get motor command from brain activity
MotorCommand AgentBrain::decodeMotorCommand()
```

#### Neuromodulation

```cpp
// Apply reward modulation
void AgentBrain::applyRewardModulation(float reward, float predicted_reward)
```

#### Development

```cpp
// Update developmental stage
void AgentBrain::updateDevelopment(float timestep)

// Get current developmental stage
DevelopmentalStage AgentBrain::getDevelopmentalStage() const
```

#### Subsystem Control

```cpp
// Enable/disable learning features
void AgentBrain::enableRewardModulation(bool enable)
void AgentBrain::enableStructuralPlasticity(bool enable)
void AgentBrain::enableDevelopment(bool enable)
void AgentBrain::enableCuriosity(bool enable)
```

#### Statistics

```cpp
// Get neuromodulation level
float AgentBrain::getNeuromodulationLevel() const

// Get curiosity level
float AgentBrain::getCuriosityLevel() const

// Get novelty level
float AgentBrain::getNoveltyLevel() const

// Get prediction error
float AgentBrain::getPredictionError() const
```

### SimpleWorld Class (`src/world/SimpleWorld.hpp`)

2D grid-based world simulation environment.

#### Creation and Configuration

```cpp
// Create simple world
SimpleWorld world

// Configure world dimensions
void SimpleWorld::configure(uint32_t width, uint32_t height, 
                           uint32_t vision_width, uint32_t vision_height)
```

#### World Control

```cpp
// Reset world to initial state
void SimpleWorld::reset()

// Update world by given timestep
void SimpleWorld::update(float timestep)

// Set random seed for reproducible simulations
void SimpleWorld::setRandomSeed(uint64_t seed)
```

#### Agent Interaction

```cpp
// Set agent starting position
void SimpleWorld::setAgentStart(float x, float y)

// Apply motor command to world
void SimpleWorld::applyMotorCommand(const MotorCommand& cmd, float time)
```

#### State Access

```cpp
// Get current sensory percept
SensoryPercept SimpleWorld::getSensoryPercept()

// Get agent body representation
const AgentBody& SimpleWorld::getAgentBody() const
```

#### World Objects

```cpp
// Add object to world
void SimpleWorld::addObject(const WorldObject& obj)

// Remove object from world
void SimpleWorld::removeObject(float x, float y)

// Check if position is valid
bool SimpleWorld::isValidPosition(float x, float y) const
```

#### World Properties

```cpp
// Get world dimensions
uint32_t SimpleWorld::getWidth() const
uint32_t SimpleWorld::getHeight() const

// Get simulation time
float SimpleWorld::getSimulationTime() const

// Get maximum energy
float SimpleWorld::getMaxEnergy() const

// Set maximum energy
void SimpleWorld::setMaxEnergy(float energy)
```

---

## Python API

The complete NLM API is available through the `pynlm` Python module, which provides Pythonic interfaces to all C++ APIs.

### Module Overview

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### Core Python Classes

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Convenience Functions

#### High-Level Factory Functions

```python
# Create complete agent setup in one function
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### Common Operations

```python
# Run complete agent simulation
def run_agent_simulation(steps=1000):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return brain, world, agent

# Get quick statistics
stats = {
    'neurons': brain.getTotalNeuronCount(),
    'firing': brain.getFiringNeuronCount(),
    'spikes': brain.getTotalSpikeCount(),
    'rate': brain.getAverageFiringRate(),
    'curiosity': agent.getCuriosityLevel(),
    'novelty': agent.getNoveltyLevel()
}
```

### Configuration Parameters

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Phase-Specific APIs

### Phase 3 API (`nlm_phase3`)

Phase 3 focuses on world interaction and basic learning.

```cpp
// Create Phase 3 experiment
Phase3Experiment phase3;

// Run Phase 3 experiments
phase3.runExperiment(config);

// Get Phase 3 metrics
float Phase3Experiment::getRewardPredictionError() const
float Phase3Experiment::getLearningRate() const
```

### Phase 4 API (`nlm_phase4`)

Phase 4 adds cognitive functions like memory, prediction, and planning.

```cpp
// Create Phase 4 experiment
Phase4Experiment phase4;

// Run Phase 4 experiments with cognitive functions
phase4.runExperiment(config);

// Get Phase 4 cognitive metrics
float Phase4Experiment::getMemoryRetention() const
float Phase4Experiment::getPredictionAccuracy() const
float Phase4Experiment::getPlanningAccuracy() const
```

### Phase 6 API (`nlm_phase6_demo`)

Phase 6 is the final integration test verifying all brain systems are connected.

```cpp
// Run Phase 6 integration demo
nlm_phase6_demo

// Check integration status
bool Phase6IntegrationResult::memoryWorkingMemoryIntegrated
bool Phase6IntegrationResult::memoryEpisodicMemoryIntegrated
bool Phase6IntegrationResult::neuromodulationIntegrated
bool Phase6IntegrationResult::predictionIntegrated
bool Phase6IntegrationResult::developmentIntegrated
```

---

## Enumerations

### NeuronType (`src/brain/Neuron.hpp`)

```cpp
enum class NeuronType {
    Excitatory,    // Excitatory neurons
    Inhibitory,    // Inhibitory neurons
    Sensory,       // Sensory neurons
    Motor,         // Motor neurons
    Modulatory,    // Neuromodulatory neurons
    Internal       // Internal neurons
};
```

### DevelopmentalStage (`src/development/DevelopmentSystem.hpp`)

```cpp
enum class DevelopmentalStage {
    Initial,      // Initial developmental stage
    CriticalPeriod,  // Critical period for learning
    Maturation,   // Maturation stage
    Adult,        // Adult stage
    Aging         // Aging stage
};
```

### ActionType (`src/motor/MotorSystem.hpp`)

```cpp
enum class ActionType {
    MoveForward,   // Move forward
    TurnLeft,      // Turn left
    TurnRight,     // Turn right
    Interact,      // Interact with object
    Eat,          // Eat food
    Drink,        // Drink water
    Sleep,        // Sleep/rest
    // ... more action types
};
```

### WorldObjectType (`src/world/SimpleWorld.hpp`)

```cpp
enum class WorldObjectType {
    Empty,        // Empty space
    Resource,     // Resource (food, water, etc.)
    Hazard,       // Hazard (danger, obstacle)
    Wall,         // Wall/obstacle
    Marker,       // Marker/landmark
    // ... more object types
};
```

---

## Error Handling

### Error Codes

```cpp
enum class NLMError {
    SUCCESS = 0,           // Operation successful
    INIT_FAILED,           // Initialization failed
    INVALID_CONFIG,        // Invalid configuration
    MEMORY_ERROR,          // Memory allocation failed
    FILE_ERROR,            // File I/O error
    SIMULATION_ERROR,      // Simulation error
    NOT_IMPLEMENTED,        // Feature not implemented
    UNKNOWN_ERROR          // Unknown error
};
```

### Error Handling

```cpp
// Check for errors
NLMError Brain::initialize()

// Handle specific errors
if (error == NLMError::INIT_FAILED) {
    std::cerr << "Failed to initialize brain" << std::endl;
    return 1;
}

// Get error messages
std::string NLMError::toString(NLMError error)
```

---

## Performance Considerations

### Memory Management

- All C++ classes use stack allocation where possible
- Dynamic allocation is optimized with memory pools
- Configuration and large objects use shared_ptr for memory safety

### Thread Safety

- Brain::step() is NOT thread-safe
- Config operations are thread-safe
- File I/O operations are thread-safe

### Performance Tips

```cpp
// Use shared configuration to reduce memory usage
auto config = std::make_shared<Config>();

// Reuse brain objects instead of creating new ones
Brain brain1(config);
brain1.initialize();
// ... use brain1 ...
brain1.reset();  // Reset for reuse instead of new Brain(config)

// Minimize Python-GIL overhead
# Use numpy arrays for large numerical data
# Avoid Python loops where possible in performance-critical sections
```

---

## Migration Guide

### From NLM v0.1 to v0.2

1. **Configuration**: JSON configuration files are now the preferred format
2. **API Changes**: Some methods have been renamed for consistency
3. **Error Handling**: New error codes and exception handling

### Python Migration

```python
# Old way
brain = pynlm.Brain(config)
brain.initialize()

# New way (same)
brain = pynlm.createBrain(config)
brain.initialize()
```

### C++ Migration

```cpp
// Old way
Brain brain(cfg);
brain.initialize()

// New way (same)
Brain brain(cfg);
brain.initialize()
```

---

## Examples

### Complete Agent Example

```python
import pynlm

def run_complete_simulation(steps=1000):
    """Run a complete NLM agent simulation with all features."""
    
    # Setup configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_complete_simulation(1000)
print("Simulation completed successfully!")
```

### C++ Integration Example

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"

int main() {
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 500);
    cfg->set("synapse_density", 0.1);
    
    // Create brain
    Brain brain(cfg);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Create world
    SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    // Run simulation
    for (uint64_t step = 0; step < 1000; ++step) {
        // Update world
        world.update(0.1f);
        
        // Get sensory input
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step, step * 0.001f);
        
        // Get action
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action
        world.applyMotorCommand(cmd, world.getSimulationTime());
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}
```

---

## References

1. [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
2. [SCIENCE.md](docs/SCIENCE.md) - Scientific background
3. [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
4. [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit results

This API reference provides comprehensive documentation for using NLM, from basic usage to advanced integration. The system is designed to be both powerful and accessible, with clear APIs and extensive documentation.