# NLM Python Bindings Reference
# ==========================

Complete reference for the low-level NLM Python bindings, focusing on the C++ bindings module `pynlm`.

## Overview

The NLM Python bindings provide direct access to the C++ NLM framework, allowing users to leverage the full power and performance of the underlying system. This module is essential for advanced users who need fine-grained control over brain parameters, simulation mechanics, and performance optimization.

### Binding Architecture

The Python bindings are organized into three main components:

1. **Core identifier types**: NeuronId, SynapseId, RegionId, PopulationId
2. **Enumerations**: NeuronType, SynapseType, DevelopmentalStage, FiringState, ActionType, MotorCommand, WorldObjectType
3. **Core classes**: Config, SensoryInput, Action, Brain, AgentBrain, SimpleWorld
4. **Convenience factory functions**: createBrain, createSimpleWorld, createAgentBrain
5. **Module-level helper functions**: create_simple_brain, create_standard_experiment, quick_test
6. **Custom exception handling**: NLMException
7. **Parameter validation helpers**

## Core Binding Classes

### Config Class

```python
class Config
```

**Description**: Configuration system for NLM parameters and simulation settings.

**Key Methods**:
```python
def __init__(self)
    Create new configuration

def set(self, key: str, value)
    Set configuration value

def get(self, key: str)
    Get configuration value

def getKeys(self) -> List[str]
    Get all configuration keys

def has(self, key: str) -> bool
    Check if configuration has key

def loadFromFile(self, filename: str)
    Load configuration from JSON file

def saveToFile(self, filename: str)
    Save configuration to JSON file
```

**Example Usage**:
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)

# Save configuration
config.saveToFile("brain_config.json")
```

### Brain Class

```python
class Brain
```

**Description**: Main neural brain simulation class implementing Leaky Integrate-and-Fire (LIF) neuron dynamics.

**Key Methods**:
```python
def __init__(self, config: Config)
    Create brain with configuration

def initialize(self)
    Initialize brain for simulation

def step(self, step_number: int, current_time: Optional[float] = None)
    Run one simulation step

def reset(self)
    Reset brain to initial state

def getTotalNeuronCount(self) -> int
    Get total neuron count

def getTotalSynapseCount(self) -> int
    Get total synapse count

def getFiringNeuronCount(self) -> int
    Get count of currently firing neurons

def getAverageFiringRate(self) -> float
    Get average firing rate across all neurons

def getTotalSpikeCount(self) -> int
    Get total spike count

def getExcitationInhibitionRatio(self) -> float
    Get E/I balance ratio

def receiveSensoryInput(self, sensory_input)
    Inject sensory input into brain

def produceAction(self)
    Produce motor action based on neural activity

def injectCurrent(self, neuron_id: int, current: float)
    Inject current into specific neuron

def save(self, filepath: str)
    Save brain state to file

def load(self, filepath: str)
    Load brain state from file
```

**Example Usage**:
```python
import pynlm

# Create brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -50.0)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    firing_rate = brain.get_average_firing_rate()
    print(f"Step {step}: Firing rate = {firing_rate:.3f}")
```

### AgentBrain Class

```python
class AgentBrain
```

**Description**: Brain interface for agent-world interaction, bridging neural processing with environmental actions.

**Key Methods**:
```python
def __init__(self, brain: Brain)
    Create agent brain interface

def initialize(self, world)
    Initialize agent with world

def processSensoryInput(self, percept)
    Process sensory input from world

def decodeMotorCommand(self)
    Decode motor command from neural activity

def applyRewardModulation(self, reward: float, predicted_reward: float)
    Apply reward-based learning

def updateDevelopment(self, timestep: float)
    Update developmental stage

def isCuriosityEnabled(self) -> bool
    Check if curiosity feature is enabled

def getCuriosityLevel(self) -> float
    Get current curiosity level
```

**Example Usage**:
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Initialize agent
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent-world interaction
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(0)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

### SimpleWorld Class

```python
class SimpleWorld
```

**Description**: Simple 2D world simulation for agent-environment interaction.

**Key Methods**:
```python
def __init__(self)
    Create new world

def configure(self, width: int, height: int, visionWidth: int, visionHeight: int)
    Configure world dimensions and sensing capabilities

def reset(self)
    Reset world to initial state

def update(self, timestep: float)
    Update world state

def setAgentStart(self, x: float, y: float)
    Set agent starting position

def applyMotorCommand(self, action, current_time: float)
    Apply motor command to world

def getSensoryPercept(self) -> SensoryInput
    Get current sensory perception

def getAgentBody(self) -> Body
    Get agent body state

def addObject(self, obj)
    Add object to world

def removeObject(self, x: float, y: float)
    Remove object at position

def isValidPosition(self, x: float, y: float) -> bool
    Check if position is valid
```

## Convenience Factory Functions

### createBrain

```python
def createBrain(config: Config) -> Brain
    Create and initialize brain
```

### createSimpleWorld

```python
def createSimpleWorld() -> SimpleWorld
    Create simple world
```

### createAgentBrain

```python
def createAgentBrain(brain: Brain) -> AgentBrain
    Create agent brain interface
```

## Enumerations

### NeuronType
```python
class NeuronType:
    EXCITATORY = 0
    INHIBITORY = 1
    SENSORY = 2
    MOTOR = 3
    MODULATORY = 4
    INTERNAL = 5
```

### DevelopmentalStage
```python
class DevelopmentalStage:
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4
```

### ActionType
```python
class ActionType:
    MOVE_FORWARD = 0
    TURN_LEFT = 1
    TURN_RIGHT = 2
    INTERACT = 3
    EAT = 4
    SLEEP = 5
```

## Custom Exception Handling

### NLMException
```python
class NLMException(Exception):
    """Base exception class for all NLM Python API errors."""
```

### ValidationError
```python
class ValidationError(NLMError):
    """Exception raised for invalid input parameters."""
```

### ConfigurationError
```python
class ConfigurationError(NLMError):
    """Exception raised for configuration errors."""
```

## Parameter Validation

### Input Validation Functions

```python
def validateNeuronCount(count: int) -> bool
    Validate neuron count

def validateComplexity(complexity: float) -> bool
    Validate brain complexity (0.0-1.0)

def validateWorldSize(width: int, height: int) -> bool
    Validate world dimensions

def validateConfig(config: Config) -> bool
    Validate complete configuration
```

## Error Handling Examples

### Validation Errors
```python
try:
    brain = create_simple_brain(neuron_count=0, complexity=1.5)  # Invalid values
except ValidationError as e:
    print(f"Validation error: {e}")
```

### Configuration Errors
```python
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "invalid")  # Wrong type
except ConfigurationError as e:
    print(f"Configuration error: {e}")
```

### Import Errors
```python
try:
    import pynlm
except ImportError as e:
    print(f"Import error: {e}")
    print("Please build NLM C++ bindings first")
```

## Performance Considerations

### Memory Management
```python
# Reuse brain objects for multiple simulations
brain = createBrain(config)

# Multiple simulation runs
for i in range(5):
    brain.initialize()
    for step in range(100):
        brain.step(step)
    brain.save(f"simulation_{i}.nlm")
    brain.reset()
```

### Simulation Efficiency
```python
# Process multiple steps efficiently
brain.initialize()
for step in range(1000):
    brain.step(step)

# Process in batches for better cache locality
batch_size = 100
for batch_start in range(0, 1000, batch_size):
    batch_end = min(batch_start + batch_size, 1000)
    for step in range(batch_start, batch_end):
        brain.step(step)
```

## Migration from High-Level to Low-Level API

### High-Level to Low-Level Migration

```python
# High-Level API
import pynlm as nlm

brain = nlm.NLMBrain(neuron_count=1000)
brain.initialize()

# Low-Level API
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Feature Mapping

| High-Level Feature | Low-Level Equivalent |
|-------------------|----------------------|
| `nlm.NLMBrain()` | `pynlm.createBrain(config)` |
| `brain.initialize()` | `brain.initialize()` |
| `brain.step(i)` | `brain.step(i)` |
| `brain.get_total_spikes()` | `brain.getTotalSpikeCount()` |
| `nlm.NLMExperiment.create_standard()` | `pynlm.createSimpleWorld()` |
| `experiment.run(num_steps)` | `world.update(0.1)` |
| `nlm.create_simple_brain()` | `pynlm.createBrain(config)` |

## Installation Notes

### Building C++ Bindings

The Python bindings require the NLM C++ libraries to be built first:

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (Release for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build Python bindings
pip install -e .
```

### Dependencies

```bash
pip install pybind11 scikit-build-core pytest numpy
```

## Version Information

- Python Bindings Version: 1.0.0
- Documentation Version: 1.0.0
- Last Updated: 2026-09-27

## Contact

For Python bindings issues or questions, please refer to the NLM documentation or support channels.

---

**Note**: The low-level Python bindings (`pynlm`) require the NLM C++ extensions to be built and installed. For most use cases, the high-level API (`nlm`) provides a more convenient and user-friendly interface while maintaining access to the same underlying functionality.