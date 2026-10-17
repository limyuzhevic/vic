# NLM — API Reference Documentation

This document provides complete API reference for the NLM codebase, covering all classes, methods, and functions.

---

## Core Classes

### Brain Class

#### Creation and Initialization
- `createBrain(config)` - Creates a new brain with the given configuration
- `brain.initialize()` - Initializes the brain and all its subsystems
- `brain.reset()` - Resets the brain to its initial state

#### Simulation Control
- `brain.step(step_number)` - Runs one simulation step
- `brain.step(step_number, time)` - Runs one simulation step with timestamp

#### State Management
- `brain.save(filename)` - Saves brain state to file
- `brain.load(filename)` - Loads brain state from file

#### Statistics
- `brain.getTotalNeuronCount()` - Total number of neurons
- `brain.getTotalSynapseCount()` - Total number of synapses
- `brain.getFiringNeuronCount()` - Number of currently firing neurons
- `brain.getAverageFiringRate()` - Average firing rate across all neurons
- `brain.getExcitationInhibitionRatio()` - E/I balance ratio

#### Memory Statistics
- `brain.getWorkingMemoryStats()` - Working memory statistics
- `brain.getEpisodicMemoryCount()` - Number of episodic memory entries
- `brain.getAssociativeMemoryCount()` - Number of associative memory associations

#### Development
- `brain.getDevelopmentalStage()` - Current developmental stage
- `brain.getDevelopmentProgress()` - Development progress (0.0 to 1.0)
- `brain.getPlasticityRate()` - Current plasticity rate

#### Regions and Populations
- `brain.addRegion(name)` - Adds a new brain region
- `brain.getRegion(region_id)` - Gets region by ID
- `brain.getRegionCount()` - Total number of regions
- `brain.getPopulationCount()` - Total number of neural populations

#### Sensory and Motor
- `brain.receiveSensoryInput(percept)` - Receives sensory input
- `brain.produceAction()` - Produces motor action

### Config Class

#### Creation and Configuration
- `createDefaultConfig()` - Creates default configuration
- `config.has(key)` - Checks if configuration key exists
- `config.set(key, value)` - Sets configuration value
- `config.get(key)` - Gets configuration value

#### File I/O
- `config.loadFromFile(filename)` - Loads configuration from file
- `config.saveToFile(filename)` - Saves configuration to file

#### Validation
- `config.validate()` - Validates configuration against schema
- `config.validateSchema()` - Validates configuration schema

#### Utilities
- `config.getKeys()` - Gets all configuration keys
- `config.getKeysByPrefix(prefix)` - Gets keys by prefix
- `config.merge(other_config)` - Merges another configuration

### AgentBrain Class

#### Creation and Initialization
- `createAgentBrain(brain)` - Creates agent brain interface
- `agent.initialize(world)` - Initializes agent with world

#### Sensory Processing
- `agent.processSensoryInput(percept)` - Processes sensory input

#### Motor Decoding
- `agent.decodeMotorCommand()` - Decodes motor command from brain activity

#### Neuromodulation
- `agent.enableRewardModulation(enable)` - Enables/disables reward modulation
- `agent.applyRewardModulation(reward, predicted_reward)` - Applies reward modulation
- `agent.getCuriosityLevel()` - Gets curiosity level
- `agent.getNoveltyLevel()` - Gets novelty level
- `agent.getPredictionError()` - Gets prediction error

#### Development
- `agent.enableDevelopment(enable)` - Enables/disables development
- `agent.updateDevelopment(time)` - Updates development
- `agent.getDevelopmentalStage()` - Gets developmental stage

#### Plasticity
- `agent.enableStructuralPlasticity(enable)` - Enables/disables structural plasticity
- `agent.getNeuromodulationLevel()` - Gets neuromodulation level

### SimpleWorld Class

#### Creation and Configuration
- `createSimpleWorld()` - Creates simple world
- `world.configure(width, height, visionWidth, visionHeight)` - Configures world dimensions
- `world.reset()` - Resets world to initial state

#### World Control
- `world.update(time)` - Updates world state
- `world.setRandomSeed(seed)` - Sets random seed

#### Agent Interaction
- `world.setAgentStart(x, y)` - Sets agent starting position
- `world.applyMotorCommand(motor_cmd, current_time)` - Applies motor command

#### State Access
- `world.getSensoryPercept()` - Gets sensory percept from world
- `world.getAgentBody()` - Gets agent body

#### World Objects
- `world.addObject(obj)` - Adds object to world
- `world.removeObject(x, y)` - Removes object from world
- `world.isValidPosition(x, y)` - Checks if position is valid

#### Properties
- `world.getWidth()` - Gets world width
- `world.getHeight()` - Gets world height
- `world.getSimulationTime()` - Gets current simulation time
- `world.getMaxEnergy()` - Gets maximum energy
- `world.setMaxEnergy(energy)` - Sets maximum energy

### Error Classes

#### NLMError (Base Class)
- `what()` - Returns error message
- `logError()` - Logs error

#### ConfigError
- `ConfigError(message, key, file, line, function)` - Configuration error

#### ValidationError
- `ValidationError(message, field, type, file, line, function)` - Validation error

#### FileError
- `FileError(message, filepath, type, file, line, function)` - File error

#### InitializationError
- `InitializationError(message, component, file, line, function)` - Initialization error

### Enumerations

#### NeuronType
- `NeuronType.Excitatory` - Excitatory neuron
- `NeuronType.Inhibitory` - Inhibitory neuron
- `NeuronType.Sensory` - Sensory neuron
- `NeuronType.Motor` - Motor neuron
- `NeuronType.Modulatory` - Modulatory neuron
- `NeuronType.Internal` - Internal neuron

#### DevelopmentalStage
- `DevelopmentalStage.Initial` - Initial developmental stage
- `DevelopmentalStage.CriticalPeriod` - Critical period
- `DevelopmentalStage.Maturation` - Maturation stage
- `DevelopmentalStage.Adult` - Adult stage
- `DevelopmentalStage.Aging` - Aging stage

#### ActionType
- `ActionType.MoveForward` - Move forward action
- `ActionType.TurnLeft` - Turn left action
- `ActionType.TurnRight` - Turn right action
- `ActionType.Interact` - Interact action
- `ActionType.Eat` - Eat action

#### WorldObjectType
- `WorldObjectType.Empty` - Empty space
- `WorldObjectType.Resource` - Resource object
- `WorldObjectType.Hazard` - Hazard object
- `WorldObjectType.Wall` - Wall object
- `WorldObjectType.Marker` - Marker object

---

## Error Handling Framework

### Error Handling Macros

#### NLM_CHECK_NULL
```cpp
NLM_CHECK_NULL(ptr, field)
```
Checks if a pointer is null and throws ValidationError if it is.

#### NLM_CHECK_CONDITION
```cpp
NLM_CHECK_CONDITION(condition, message, field, type)
```
Checks a condition and throws ValidationError if false.

#### NLM_THROW_VALIDATION
```cpp
NLM_THROW_VALIDATION(message, field, type)
```
Throws ValidationError with specified message, field, and type.

#### NLM_THROW_CONFIG
```cpp
NLM_THROW_CONFIG(message, key)
```
Throws ConfigError with specified message and key.

#### NLM_THROW_FILE
```cpp
NLM_THROW_FILE(operation, filepath, message, type)
```
Throws FileError with specified operation, filepath, message, and type.

#### NLM_THROW_INITIALIZATION
```cpp
NLM_THROW_INITIALIZATION(message, component)
```
Throws InitializationError with specified message and component.

### Error Handling Functions

#### ErrorHandling Namespace
- `checkCondition()` - Checks condition and throws error if false
- `convertFileError()` - Converts to FileError
- `convertConfigError()` - Converts to ConfigError
- `convertInitializationError()` - Converts to InitializationError

### Result Type

```cpp
template<typename T>
class Result {
public:
    bool isSuccess() const;
    T get() const;
    const Error& getError() const;
    static Result success(T value);
    static Result failure(Error error);
}
```

---

## Configuration System

### Configuration Sources

1. **Default** - Built-in default values
2. **File** - Configuration files (.cfg, JSON)
3. **Environment** - Environment variables (NLM_ prefix)
4. **CommandLine** - Command line arguments
5. **Runtime** - Programmatic configuration
6. **Preset** - Built-in presets
7. **Template** - Configuration templates

### Configuration Precedence

1. Environment Variables > Command Line > Configuration File > Programmatic > Default

### Built-in Presets

- `basic` - Basic configuration
- `advanced` - Advanced configuration
- `debug` - Debug configuration
- `production` - Production configuration

### Configuration Templates

- `default` - Default template
- `debug` - Debug template
- `simple` - Simple template

---

## Command Line Interface

### Main Executable

#### Available Commands

- `--run-phase-6, -p6` - Run Phase 6 experiments
- `--validate-config, -v` - Validate configuration
- `--export-config, -e` - Export configuration
- `--info, -i` - Show configuration information
- `--compare-configs` - Compare configurations
- `--list-presets, -lp` - List presets
- `--list-templates, -lt` - List templates
- `--help, -h` - Show help

#### Configuration Loading Options

- `--config <file>` - Load configuration from file
- `--preset <name>` - Load from preset
- `--template <name> [overrides]` - Load from template
- `--schema <file>` - Load schema
- `--env-prefix <prefix>` - Set environment prefix
- `--no-env` - Disable environment loading

---

## Performance Optimizations

### SIMD Vectorization
- `brain.enableSIMD(true)` - Enable SIMD optimizations

### Parallel Processing
- `brain.enableParallelProcessing(true)` - Enable parallel processing

### Memory Pools
- `brain.enableMemoryPools(true)` - Enable memory pools

### Event Queues
- `brain.enableEventQueues(true)` - Enable event queues

---

## Build and Installation

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installing Python Bindings

```bash
pip install .
pip install -e .  # Development mode
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

---

## Integration Examples

### Complete Agent Simulation

```python
import pynlm

def run_simulation(config_file=None, num_steps=1000):
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    if config_file:
        config.loadFromFile(config_file)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, agent, world

# Run simulation
brain, agent, world = run_simulation("config.json", 1000)
```

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
try:
    config.validate()
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration error: {e}")

# Export configuration
config.saveToFile("exported_config.json")
```

### Error Handling

```cpp
#include "core/Error/ErrorHandling.hpp"
#include "core/Config/Config.hpp"

void example() {
    nlm::Config config;
    
    try {
        config.loadFromFile("config.json");
        config.validate();
    } catch (const nlm::ConfigError& e) {
        NLM_THROW_CONFIG("Failed to load configuration", e.getKey());
    }
    
    // Use NLM error handling macros
    NLM_CHECK_CONDITION(config.has("brain.neuron_count"), 
                      "Neuron count not specified", 
                      "brain.neuron_count", 
                      nlm::ValidationError::ErrorType::MissingRequired);
    
    int neuron_count = config.getInt("brain.neuron_count");
    NLM_CHECK_CONDITION(neuron_count > 0,
                      "Neuron count must be positive",
                      "brain.neuron_count",
                      nlm::ValidationError::ErrorType::InvalidRange);
}
```

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMConfigurationReference.md** - Configuration documentation
3. **NLMExamples.md** - Additional examples
4. **NLMMigrationGuide.md** - Migration guide
5. **NLMBestPractices.md** - Best practices
6. **NLMTroubleshooting.md** - Troubleshooting guide

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*