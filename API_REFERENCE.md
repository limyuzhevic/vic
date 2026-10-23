# NLM (Neural Learning Machine) - API Reference

This document provides a comprehensive reference for the NLM Python API and C++ API.

## Python API

### Core Classes

#### Brain

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

#### Config

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

#### AgentBrain

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

#### SimpleWorld

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

#### Enumerations

##### Neuron Types

| Enum | Description |
|------|-------------|
| `pynlm.NeuronType.Excitatory` | Excitatory neuron |
| `pynlm.NeuronType.Inhibitory` | Inhibitory neuron |
| `pynlm.NeuronType.Modulatory` | Neuromodulatory neuron |
| `pynlm.NeuronType.Sensory` | Sensory neuron |
| `pynlm.NeuronType.Motor` | Motor neuron |
| `pynlm.NeuronType.Internal` | Internal neuron |

##### Developmental Stages

| Enum | Description |
|------|-------------|
| `pynlm.DevelopmentalStage.Initial` | Initial development stage |
| `pynlm.DevelopmentalStage.CriticalPeriod` | Critical period of plasticity |
| `pynlm.DevelopmentalStage.Maturation` | Maturation stage |
| `pynlm.DevelopmentalStage.Adult` | Adult stage |
| `pynlm.DevelopmentalStage.Aging` | Aging stage |

##### Action Types

| Enum | Description |
|------|-------------|
| `pynlm.ActionType.MoveForward` | Move forward |
| `pynlm.ActionType.TurnLeft` | Turn left |
| `pynlm.ActionType.TurnRight` | Turn right |
| `pynlm.ActionType.Interact` | Interact with environment |
| `pynlm.ActionType.Eat` | Eat/consume resource |
| `pynlm.ActionType.Sleep` | Sleep/rest |
| `pynlm.ActionType.Explore` | Explore environment |
| `pynlm.ActionType.Learn` | Learn from experience |

##### World Object Types

| Enum | Description |
|------|-------------|
| `pynlm.WorldObjectType.Empty` | Empty space |
| `pynlm.WorldObjectType.Resource` | Resource that can be consumed |
| `pynlm.WorldObjectType.Hazard` | Hazard that damages agent |
| `pynlm.WorldObjectType.Wall` | Wall obstacle |
| `pynlm.WorldObjectType.Marker` | Marker for navigation |

### Error Handling

NLM provides comprehensive error handling with custom exceptions:

```python
try:
    brain.initialize()
    # Your simulation code
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Advanced Usage Patterns

#### Context Manager for Brain Lifecycle

```python
import pynlm

with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)
    # Brain automatically cleaned up
```

#### Progress Callbacks for Long Simulations

```python
def progress_callback(step, total_steps, brain):
    progress = (step + 1) / total_steps * 100
    print(f"Progress: {progress:.1f}% - Step {step + 1}/{total_steps}")

# Configure brain with progress callback
brain = pynlm.createBrain(config)
brain.setProgressCallback(progress_callback, 1000)
brain.initialize()

for step in range(1000):
    brain.step(step)
```

#### Custom Configuration Schema

```python
# Create custom configuration schema
config = pynlm.createDefaultConfig()

# Define custom validation
custom_schema = {
    "brain.neuron_count": {"type": "int", "min": 100, "max": 10000},
    "learning.rate": {"type": "float", "min": 0.0, "max": 1.0},
    "brain.connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
}

config.validateSchema(custom_schema)
```

## C++ API

The C++ API provides low-level access to NLM components. For detailed C++ API documentation, refer to:

- `src/core/` - Core utilities and types
- `src/brain/` - Neural processing components
- `src/memory/` - Memory systems
- `src/neuromodulation/` - Neuromodulation systems
- `src/cognition/` - Cognitive mechanisms

### Key C++ Classes

#### Brain Class

```cpp
// Create brain with configuration
Brain brain(config);
brain.initialize();

// Simulation
brain.step(step_number);
brain.step(step_number, time);
brain.reset();

// Neural operations
brain.injectCurrent(neuron_id, current_value);
brain.getNeuronActivation(neuron_id);
brain.getRegion(region_id);
```

#### EnhancedConfig Class

The `EnhancedConfig` class provides advanced configuration management with schema validation, environment variable support, and better error handling.

```cpp
// Enhanced configuration with validation
EnhancedConfig enhanced_config;

// Load from file with validation
enhanced_config.loadFromFile("config.json", true);

// Load from environment variables
enhanced_config.loadFromEnvironment("NLM_");

// Set values with validation
enhanced_config.setInt("brain.neuron_count", 1000);
enhanced_config.setFloat("brain.synapse_density", 0.1);

// Validate configuration
ValidationResult result = enhanced_config.validate();
if (!result.isValid) {
    std::cout << "Configuration validation failed: " << result.errorMessage << std::endl;
}

// Get values with validation
int neuron_count = enhanced_config.getInt("brain.neuron_count");
float synapse_density = enhanced_config.getFloat("brain.synapse_density");
```

#### Configuration Schemas

Define schemas for type validation and constraints:

```cpp
// Schema definition
ConfigSchema int_schema = ConfigSchema::Int(
    "brain.neuron_count", 1000, false,
    "Number of neurons", {-1000, 100000}
);

ConfigSchema float_schema = ConfigSchema::Float(
    "brain.synapse_density", 0.1, false,
    "Synapse connectivity density", {0.0f, 1.0f}
);

ConfigSchema string_schema = ConfigSchema::String(
    "brain.name", "Default Brain", false,
    "Brain name", {"Default", "Custom", "Test"}
);

// Set schemas
enhanced_config.setSchema(int_schema);
enhanced_config.setSchema(float_schema);
enhanced_config.setSchema(string_schema);
```

#### Factory Functions

Create different types of configurations:

```cpp
// Create enhanced configurations
EnhancedConfig default_config = EnhancedConfig::createDefault();
EnhancedConfig brain_config = EnhancedConfig::createBrain();
EnhancedConfig world_config = EnhancedConfig::createWorld();
EnhancedConfig neuromod_config = EnhancedConfig::createNeuromodulation();

// Merge configurations
EnhancedConfig complete_config = brain_config;
complete_config.merge(world_config);
complete_config.merge(neuromod_config);
```

#### Environment Variables

Environment variables use the pattern `NLM_PREFIX` + `KEY`:

```bash
export NLM_BRAIN_NEURON_COUNT=2000
export NLM_WORLD_WIDTH=50
export NLM_NEUROMOD_DOPAMINE_SCALE=1.5
```

#### Configuration Examples

```cpp
#include <nlm.h>

void exampleEnhancedConfiguration() {
    // Create enhanced configuration
    EnhancedConfig config = EnhancedConfig::createDefault();
    
    // Load configuration with validation
    if (config.loadFromFile("config.json")) {
        std::cout << "Configuration loaded successfully" << std::endl;
        
        // Get configuration values
        int neurons = config.getInt("brain.neuron_count");
        float density = config.getFloat("brain.synapse_density");
        std::string name = config.getString("brain.name");
        
        std::cout << "Brain name: " << name << std::endl;
        std::cout << "Neurons: " << neurons << std::endl;
        std::cout << "Synapse density: " << density << std::endl;
        
        // Validate configuration
        ValidationResult result = config.validate();
        if (!result.isValid) {
            std::cout << "Validation failed: " << result.errorMessage << std::endl;
        }
    }
    
    // Create brain with enhanced configuration
    Brain brain(config);
    brain.initialize();
}
```

#### Memory Systems

**WorkingMemory**
```cpp
WorkingMemory working_memory;
working_memory.store(neuron_id, value);
float retrieved_value = working_memory.retrieve(neuron_id);
```

**EpisodicMemory**
```cpp
EpisodicMemory episodic_memory;
EpisodicMemoryItem episode;
episode.timestamp = current_step;
episode.neurons = {neuron1, neuron2, ...};
episodic_memory.storeEpisode(episode);
```

**NeuralWorkingMemory** (Enhanced version)
```cpp
NeuralWorkingMemory neural_memory;
neural_memory.initialize(&brain);
neural_memory.store(pattern, strength);
std::vector<float> retrieved = neural_memory.retrieve();
```

#### Neuromodulation Systems

```cpp
// Reward modulation
Dopamine dopamine;
dopamine.signalReward(1.0f);
dopamine.signalRewardPredictionError(0.5f);

// Curiosity
Curiosity curiosity;
curiosity.update(exploration_level);

// Novelty detection
Novelty novelty;
novelty.update(sensory_input);
```

### Build and Installation

#### Building with CMake

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Python Installation

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Install in development mode
pip install -e .
```

#### Custom Build Options

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Performance Considerations

#### Memory Management

NLM provides several memory management optimizations:

1. **Memory Pools**: Objects are allocated from memory pools to reduce fragmentation
2. **Event Queues**: Asynchronous operations use event queues for efficient processing
3. **SIMD Support**: Mathematical operations use SIMD instructions where available

#### Parallel Processing

The project supports parallel processing for certain operations:

- Neural simulation can use multiple threads
- Memory operations can be parallelized
- Some operations use OpenMP for performance

#### Memory Usage

| Component | Typical Memory Usage |
|-----------|---------------------|
| Brain (1000 neurons) | ~50-100 MB |
| World (20x20 grid) | ~10-20 MB |
| Configuration | ~1-5 MB |
| AgentBrain interface | ~5-10 MB |

#### Simulation Speed

- **C++ core**: 10,000+ steps/second per 1000 neurons
- **Python bindings**: 1,000-2,000 steps/second per 1000 neurons
- **Performance optimizations**: SIMD, memory pools, event-driven simulation

### Examples and Templates

#### Complete Agent Simulation Template

```python
import pynlm
import time

def run_complete_simulation(config_file=None, num_steps=1000):
    """Run a complete agent simulation with all subsystems."""
    
    # 1. Create configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems for Phase 6 integration
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation with detailed logging
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward from world
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.3f}")
    
    end_time = time.time()
    
    # Final statistics
    print(f"\nSimulation complete in {end_time - start_time:.2f} seconds")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Example usage
if __name__ == "__main__":
    run_complete_simulation(num_steps=1000)
```

#### Memory System Usage Example

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable working memory
agent.enableWorkingMemory(True)

# Run simulation to fill working memory
for step in range(100):
    brain.step(step)
    
    # Access working memory contents
    working_memory = agent.getWorkingMemory()
    if working_memory:
        recent_activities = working_memory.getRecentActivities(10)
        print(f"Step {step}: Working memory contains {len(recent_activities)} items")
```

#### Neuromodulation Usage Example

```python
import pynlm

# Create simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Run simulation to observe neuromodulation effects
for step in range(500):
    # Apply simulated reward
    reward = 0.1 if step % 50 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Get neuromodulation levels
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    neuromodulation_level = agent.getNeuromodulationLevel()
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {curiosity_level:.3f}")
        print(f"  Novelty: {novelty_level:.3f}")
        print(f"  Neuromodulation: {neuromodulation_level:.3f}")
```

#### Development System Usage Example

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check initial developmental stage
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation to allow development to progress
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        current_stage = brain.getDevelopmentalStage()
        plasticity_rate = brain.getPlasticityRate()
        print(f"Step {step}:")
        print(f"  Stage: {current_stage}")
        print(f"  Plasticity rate: {plasticity_rate:.3f}")
```

## Version Information

### Semantic Versioning

NLM follows semantic versioning (MAJOR.MINOR.PATCH):

- **MAJOR version**: When making incompatible API changes
- **MINOR version**: When adding functionality in a backward-compatible manner
- **PATCH version**: When fixing bugs in a backward-compatible manner

### Version History

- **Phase 6.0.0**: Final integration (current)
- **Phase 5.0.0**: Performance optimizations
- **Phase 4.0.0**: Advanced cognition systems
- **Phase 3.0.0**: World interaction
- **Phase 2.0.0**: Basic learning mechanisms
- **Phase 1.0.0**: Core components

### Compatibility

- **Python**: 3.8+
- **C++**: C++20
- **Dependencies**: pybind11 2.11+, scikit-build-core 0.5+

## License

MIT License. See `LICENSE` file for details.

## Support

For issues, questions, or suggestions:

1. Check the documentation in `docs/`
2. Visit the project repository
3. Report issues through the project issue tracker
4. Consult the troubleshooting section in `HOW_TO_USE.md`

## Acknowledgments

This project builds on decades of research in:

- Neural network simulation
- Computational neuroscience
- Artificial developmental systems
- Neuromodulation and reinforcement learning
- Cognitive architecture

Special thanks to the open-source community for pybind11, scikit-build-core, and other essential tools.
