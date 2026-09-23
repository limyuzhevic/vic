# NLM Python Bindings Documentation

## Overview

The NLM Python bindings provide a comprehensive Python API for interacting with the NLM (Neural Learning Machine) neural simulation system. This documentation covers all aspects of the Python bindings, including enhanced features, advanced usage patterns, and integration with the NLM system.

## Key Features

### 1. Enhanced Python API
- **Type Hints**: Comprehensive type hints for better IDE support
- **Docstrings**: Detailed docstrings for all functions and classes
- **Error Handling**: Robust error handling with clear error messages
- **Performance Optimization**: Performance-optimized Python bindings

### 2. Advanced Features
- **Async Support**: Asynchronous Python bindings for high-performance applications
- **Context Managers**: Context managers for resource management
- **Configuration Management**: Advanced configuration management
- **Event System**: Event-driven architecture for simulation control

### 3. Integration Features
- **Numpy Support**: Integration with NumPy arrays for neural data
- **Pandas Support**: Integration with Pandas DataFrames for data analysis
- **JSON Serialization**: JSON serialization/deserialization for configuration
- **Parallel Processing**: Parallel processing capabilities

### 4. Developer Experience
- **Autocompletion**: Full IDE autocompletion support
- **Introspection**: Python introspection for type information
- **Testing**: Comprehensive testing with Python test frameworks
- **Documentation**: Auto-generated documentation with Sphinx

## Installation

### Requirements

```bash
# Install Python dependencies
pip install numpy scipy pandas matplotlib
pip install pybind11 scikit-build-core pytest

# Install NLM Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### Build from Source

```bash
# Clone NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build Python bindings
pip install .

# Or build with scikit-build
pip install build
python -m build
pip install dist/*.whl
```

### Installation Verification

```python
import pynlm

# Check version
print(f"NLM version: {pynlm.__version__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Clean up
del brain, config
```

## Python API

### Core Classes

#### Brain Class
```python
class Brain:
    """Represents a neural brain with neurons and synapses."""
    
    def __init__(self, config: Dict[str, Any] = None):
        """Initialize a new brain with optional configuration."""
    
    def initialize(self) -> None:
        """Initialize the brain for simulation."""
    
    def step(self, step_number: int) -> None:
        """Process one simulation step."""
    
    def reset(self) -> None:
        """Reset the brain to initial state."""
    
    def getTotalNeuronCount(self) -> int:
        """Get total number of neurons in the brain."""
    
    def getTotalSynapseCount(self) -> int:
        """Get total number of synapses in the brain."""
    
    def getFiringNeuronCount(self) -> int:
        """Get number of currently firing neurons."""
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate of neurons."""
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation-inhibition balance ratio."""
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
    
    def save(self, filename: str) -> None:
        """Save brain state to file."""
    
    def load(self, filename: str) -> None:
        """Load brain state from file."""
    
    def addRegion(self, name: str) -> int:
        """Add a brain region."""
    
    def getRegion(self, region_id: int) -> Region:
        """Get brain region by ID."""
```

#### Config Class
```python
class Config:
    """Configuration management for NLM brain."""
    
    def __init__(self):
        """Initialize empty configuration."""
    
    @staticmethod
    def createDefaultConfig() -> 'Config':
        """Create default configuration."""
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value."""
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys."""
    
    def loadFromFile(self, filename: str) -> bool:
        """Load configuration from file."""
    
    def saveToFile(self, filename: str) -> bool:
        """Save configuration to file."""
    
    def clear(self) -> None:
        """Clear all configuration values."""
```

#### AgentBrain Class
```python
class AgentBrain:
    """Agent brain interface for world interaction."""
    
    def __init__(self, brain: Brain):
        """Initialize agent brain interface."""
    
    def initialize(self, world: 'World') -> None:
        """Initialize agent brain with world."""
    
    def processSensoryInput(self, percept: 'SensoryInput') -> None:
        """Process sensory input from world."""
    
    def decodeMotorCommand(self) -> 'Action':
        """Decode motor command from brain activity."""
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward modulation for learning."""
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update brain development."""
    
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable/disable development."""
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
    
    def getNeuromodulationLevel(self) -> float:
        """Get neuromodulation level."""
```

#### SimpleWorld Class
```python
class SimpleWorld:
    """Simple world for agent simulation."""
    
    def __init__(self):
        """Initialize empty world."""
    
    @staticmethod
    def createSimpleWorld() -> 'SimpleWorld':
        """Create default simple world."""
    
    def configure(self, width: int, height: int, 
                  visionWidth: int = 8, visionHeight: int = 8,
                  visionAngle: float = 90.0) -> None:
        """Configure world dimensions and agent vision."""
    
    def reset(self) -> None:
        """Reset world to initial state."""
    
    def update(self, timestep: float) -> None:
        """Update world state."""
    
    def setAgentStart(self, x: float, y: float) -> None:
        """Set agent starting position."""
    
    def applyMotorCommand(self, action: 'Action', current_time: float) -> None:
        """Apply motor command to world."""
    
    def getSensoryPercept(self) -> 'SensoryInput':
        """Get sensory percept from world."""
    
    def getAgentBody(self) -> 'Body':
        """Get agent body state."""
    
    def getSimulationTime(self) -> float:
        """Get current simulation time."">
    
    def getWidth(self) -> int:
        """Get world width."""
    
    def getHeight(self) -> int:
        """Get world height."""
    
    def getVisionWidth(self) -> int:
        """Get vision width."""
    
    def getVisionHeight(self) -> int:
        """Get vision height."">
    
    def getMaxEnergy(self) -> float:
        """Get maximum energy."""
    
    def setMaxEnergy(self, energy: float) -> None:
        """Set maximum energy."""
    
    def setRandomSeed(self, seed: int) -> None:
        """Set random seed."""
    
    def addObject(self, obj: 'WorldObject') -> None:
        """Add object to world."""
    
    def removeObject(self, x: float, y: float) -> bool:
        """Remove object from world."""
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid."""
```

### Enumerations

#### NeuronType
```python
class NeuronType(Enum):
    """Types of neurons."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    MODULATORY = "modulatory"
    INTERNAL = "internal"
```

#### DevelopmentalStage
```python
class DevelopmentalStage(Enum):
    """Developmental stages of brain."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
```

#### ActionType
```python
class ActionType(Enum):
    """Types of actions."""
    MOVE_FORWARD = "move_forward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    INTERACT = "interact"
    EAT = "eat"
    SLEEP = "sleep"
```

#### WorldObjectType
```python
class WorldObjectType(Enum):
    """Types of world objects."""
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
```

### Functions

#### Core Functions
```python
import pynlm

# Brain creation and initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# World creation and configuration
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Agent creation and initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Complete simulation loop
run_agent_simulation(brain, world, agent, 1000)
```

#### Configuration Functions
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.learning_rate", 0.01)
config.set("brain.enable_curiosity", True)

# Get configuration values
neuron_count = config.getInt("brain.neuron_count")
learning_rate = config.getDouble("brain.learning_rate")
enable_curiosity = config.getBool("brain.enable_curiosity")

# File I/O
config.loadFromFile("config.json")
config.saveToFile("output_config.json")
```

#### World Creation Functions
```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent start
world.setAgentStart(10.0, 10.0)

# Reset world
world.reset()

# Update world
world.update(0.1)

# Get percept
percept = world.getSensoryPercept()

# Apply action
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Python Features

### 1. NumPy Integration
```python
import numpy as np
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
brain = pynlm.createBrain(config)
brain.initialize()

# Get neural data as numpy arrays
neuron_activities = brain.getNeuronActivities()
synaptic_weights = brain.getSynapticWeights()

# Process with NumPy
average_activity = np.mean(neuron_activities)
std_activity = np.std(neuron_activities)

# Update brain with numpy data
brain.updateNeuronActivities(neuron_activities)
```

### 2. Pandas Integration
```python
import pandas as pd
import pynlm

# Create simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation and collect data
simulation_data = []
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect data
    simulation_data.append({
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'reward': percept.getReward() if hasattr(percept, 'getReward') else 0.0
    })

# Convert to DataFrame
df = pd.DataFrame(simulation_data)

# Analyze data
print(df.describe())
print(f"Average curiosity: {df['curiosity'].mean()}")
print(f"Max firing neurons: {df['firing_neurons'].max()}")

# Save to file
df.to_csv('simulation_data.csv', index=False)
```

### 3. JSON Integration
```python
import json
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.learning_rate", 0.01)

# Convert to JSON
config_json = config.toJSON()
print(json.dumps(config_json, indent=2))

# Save to file
config.saveToFile("config.json")

# Load from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("config.json")

# Merge configurations
merged_config = mergeConfigs(config, new_config)
```

### 4. Async Support
```python
import asyncio
import pynlm

async def runAsyncSimulation(brain, world, agent, steps):
    """Run simulation asynchronously."""
    tasks = []
    
    for step in range(steps):
        # Create async task for each step
        task = asyncio.create_task(processStepAsync(brain, world, agent, step))
        tasks.append(task)
        
        # Wait for step to complete before starting next
        await task
    
    return tasks

async def processStepAsync(brain, world, agent, step):
    """Process single simulation step asynchronously."""
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    return step

# Run async simulation
async def main():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run async simulation
    tasks = await runAsyncSimulation(brain, world, agent, 1000)
    
    print(f"Simulation completed with {len(tasks)} steps")

# Run main
asyncio.run(main())
```

### 5. Context Managers
```python
import pynlm

# Use brain as context manager
def useBrainAsContextManager():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    with pynlm.BrainContext(config) as brain:
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Brain is automatically cleaned up
    return brain

# Use world as context manager
def useWorldAsContextManager():
    world = pynlm.SimpleWorldContext()
    world.configure(width=20, height=20)
    
    with world:
        world.reset()
        world.update(0.1)
        
        # Do work with world
        percept = world.getSensoryPercept()
    
    # World is automatically cleaned up
    return world
```

### 6. Event System
```python
import pynlm
from typing import Callable

class SimulationEvent:
    """Event for simulation events."""
    
    def __init__(self, event_type: str, data: dict):
        self.type = event_type
        self.data = data
        self.timestamp = time.time()

class EventManager:
    """Manage simulation events."""
    
    def __init__(self):
        self.listeners = {}
    
    def add_listener(self, event_type: str, callback: Callable):
        """Add event listener."""
        if event_type not in self.listeners:
            self.listeners[event_type] = []
        self.listeners[event_type].append(callback)
    
    def remove_listener(self, event_type: str, callback: Callable):
        """Remove event listener."""
        if event_type in self.listeners:
            self.listeners[event_type].remove(callback)
    
    def emit(self, event: SimulationEvent):
        """Emit event to listeners."""
        if event.type in self.listeners:
            for callback in self.listeners[event.type]:
                callback(event)

# Simulation with event system
def runSimulationWithEvents(brain, world, agent):
    event_manager = EventManager()
    
    # Add event listeners
    def onStepCompleted(event):
        print(f"Step {event.data['step']} completed")
    
    def onLearningEvent(event):
        print(f"Learning event: {event.data}")
    
    def onMemoryEvent(event):
        print(f"Memory event: {event.data}")
    
    event_manager.add_listener("step_completed", onStepCompleted)
    event_manager.add_listener("learning", onLearningEvent)
    event_manager.add_listener("memory", onMemoryEvent)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Emit events
        event_manager.emit(SimulationEvent("step_completed", {"step": step}))
        
        if agent.getCuriosityLevel() > 0.5:
            event_manager.emit(SimulationEvent("learning", {"curiosity": agent.getCuriosityLevel()}))
        
        if brain.getTotalSynapseCount() > 1000:
            event_manager.emit(SimulationEvent("memory", {"synapses": brain.getTotalSynapseCount()}))
```

### 7. Parallel Processing
```python
import concurrent.futures
import pynlm
from threading import Lock

class ParallelSimulation:
    """Parallel simulation processor."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.lock = Lock()
    
    def processStep(self, step):
        """Process single simulation step."""
        with self.lock:
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        return step
    
    def runParallel(self, steps, num_workers=4):
        """Run simulation in parallel."""
        with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
            # Submit all steps
            futures = [executor.submit(self.processStep, step) for step in range(steps)]
            
            # Wait for all steps to complete
            for future in concurrent.futures.as_completed(futures):
                try:
                    future.result()
                except Exception as e:
                    print(f"Error in parallel processing: {e}")
    
    def runWithProgress(self, steps, num_workers=4):
        """Run simulation with progress monitoring."""
        import tqdm
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
            # Submit all steps with progress bar
            futures = []
            with tqdm.tqdm(total=steps) as pbar:
                for step in range(steps):
                    future = executor.submit(self.processStep, step)
                    futures.append(future)
                    pbar.update(1)
                
                # Wait for all steps to complete
                for future in concurrent.futures.as_completed(futures):
                    try:
                        future.result()
                    except Exception as e:
                        print(f"Error in parallel processing: {e}")
```

## API Reference

### Core Functions

#### pynlm.createBrain()
```python
pynlm.createBrain(config: Dict[str, Any] = None) -> Brain
```

Create a new brain with optional configuration.

**Parameters:**
- `config`: Dictionary with brain configuration

**Returns:**
- `Brain`: New brain instance

**Example:**
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create brain with custom configuration
config = {
    "brain.neuron_count": 1000,
    "brain.learning_rate": 0.01
}
brain = pynlm.createBrain(config)
```

#### pynlm.createDefaultConfig()
```python
pynlm.createDefaultConfig() -> Config
```

Create default brain configuration.

**Returns:**
- `Config`: Default configuration

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
```

#### pynlm.createSimpleWorld()
```python
pynlm.createSimpleWorld() -> SimpleWorld
```

Create default simple world.

**Returns:**
- `SimpleWorld`: New world instance

**Example:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### pynlm.createAgentBrain()
```python
pynlm.createAgentBrain(brain: Brain) -> AgentBrain
```

Create agent brain interface.

**Parameters:**
- `brain`: Brain instance

**Returns:**
- `AgentBrain`: Agent brain instance

**Example:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
```

### Brain Methods

#### Basic Operations
```python
brain.initialize() -> None
brain.step(step_number: int) -> None
brain.reset() -> None
brain.save(filename: str) -> None
brain.load(filename: str) -> None
```

#### Statistics
```python
brain.getTotalNeuronCount() -> int
brain.getTotalSynapseCount() -> int
brain.getFiringNeuronCount() -> int
brain.getAverageFiringRate() -> float
brain.getExcitationInhibitionRatio() -> float
brain.getDevelopmentalStage() -> DevelopmentalStage
```

#### Regions
```python
brain.addRegion(name: str) -> int
brain.getRegion(region_id: int) -> Region
```

### Config Methods

#### Configuration
```python
config.set(key: str, value: Any) -> None
config.get(key: str, default: Any = None) -> Any
config.has(key: str) -> bool
config.getKeys() -> List[str]
```

#### File I/O
```python
config.loadFromFile(filename: str) -> bool
config.saveToFile(filename: str) -> bool
config.toJSON() -> Dict[str, Any]
config.fromJSON(json_dict: Dict[str, Any]) -> None
```

#### Utilities
```python
config.clear() -> None
config.merge(other: Config) -> None
config.copy() -> Config
config.isEmpty() -> bool
```

### AgentBrain Methods

#### Brain Control
```python
agent.initialize(world: SimpleWorld) -> None
agent.processSensoryInput(percept: SensoryInput) -> None
agent.decodeMotorCommand() -> Action
```

#### Learning
```python
agent.applyRewardModulation(reward: float, predicted_reward: float) -> None
agent.updateDevelopment(timestep: float) -> None
```

#### Subsystem Control
```python
agent.enableRewardModulation(enable: bool) -> None
agent.enableStructuralPlasticity(enable: bool) -> None
agent.enableDevelopment(enable: bool) -> None
agent.enableCuriosity(enable: bool) -> None
```

#### Statistics
```python
agent.getCuriosityLevel() -> float
agent.getNoveltyLevel() -> float
agent.getPredictionError() -> float
agent.getNeuromodulationLevel() -> float
```

### SimpleWorld Methods

#### World Control
```python
world.configure(width: int, height: int, visionWidth: int = 8, visionHeight: int = 8, visionAngle: float = 90.0) -> None
world.reset() -> None
world.update(timestep: float) -> None
world.setAgentStart(x: float, y: float) -> None
world.applyMotorCommand(action: Action, current_time: float) -> None
```

#### World Information
```python
world.getSensoryPercept() -> SensoryInput
world.getAgentBody() -> Body
world.getSimulationTime() -> float
world.getWidth() -> int
world.getHeight() -> int
world.getVisionWidth() -> int
world.getVisionHeight() -> int
world.getMaxEnergy() -> float
```

#### World Objects
```python
world.addObject(obj: WorldObject) -> None
world.removeObject(x: float, y: float) -> bool
world.isValidPosition(x: float, y: float) -> bool
```

### Enumerations

#### NeuronType
```python
class NeuronType(Enum):
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    MODULATORY = "modulatory"
    INTERNAL = "internal"
```

#### DevelopmentalStage
```python
class DevelopmentalStage(Enum):
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
```

#### ActionType
```python
class ActionType(Enum):
    MOVE_FORWARD = "move_forward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    INTERACT = "interact"
    EAT = "eat"
    SLEEP = "sleep"
```

#### WorldObjectType
```python
class WorldObjectType(Enum):
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
```

## Error Handling

### Exception Types

```python
class PynlmError(Exception):
    """Base exception for NLM errors."""
    pass

class BrainError(PynlmError):
    """Exception raised for brain errors."""
    pass

class ConfigError(PynlmError):
    """Exception raised for configuration errors."""
    pass

class WorldError(PynlmError):
    """Exception raised for world errors."""
    pass

class AgentError(PynlmError):
    """Exception raised for agent errors."""
    pass
```

### Error Handling Examples

```python
import pynlm

def runSimulation():
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
    except pynlm.BrainError as e:
        print(f"Brain error: {e}")
        return False
    except pynlm.ConfigError as e:
        print(f"Configuration error: {e}")
        return False
    except pynlm.PynlmError as e:
        print(f"NLM error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False
    
    return True

def safeBrainOperation():
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Safe operation
        neuron_count = brain.getTotalNeuronCount()
        
        # Clean up
        del brain
        
        return neuron_count
        
    except pynlm.BrainError:
        print("Brain operation failed")
        raise
    except Exception as e:
        print(f"Operation failed: {e}")
        raise
```

## Performance Optimization

### 1. Memory Management
```python
import pynlm
import gc

def optimizeMemory():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Clear references
    del brain, config
    gc.collect()  # Force garbage collection
    
    return True

# Alternative: Use context managers
with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
```

### 2. Caching
```python
import pynlm
from functools import lru_cache

@lru_cache(maxsize=128)
def getCachedNeuronCount(brain_id):
    """Get neuron count with caching."""
    brain = pynlm.getBrain(brain_id)
    return brain.getTotalNeuronCount()

@lru_cache(maxsize=64)
def getCachedSynapseCount(brain_id):
    """Get synapse count with caching."""
    brain = pynlm.getBrain(brain_id)
    return brain.getTotalSynapseCount()
```

### 3. Parallel Processing
```python
import pynlm
import concurrent.futures

def runParallelSimulation(brain, world, agent, steps, num_workers=4):
    """Run simulation in parallel."""
    with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
        # Submit all steps
        futures = []
        for step in range(steps):
            future = executor.submit(processStep, brain, world, agent, step)
            futures.append(future)
        
        # Wait for all steps to complete
        for future in concurrent.futures.as_completed(futures):
            future.result()  # Wait for completion
    
    return True

def processStep(brain, world, agent, step):
    """Process single simulation step."""
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    return step
```

### 4. Profiling
```python
import pynlm
import cProfile
import pstats

def profileSimulation():
    """Profile simulation performance."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with profiling
    profiler = cProfile.Profile()
    profiler.enable()
    
    for step in range(1000):
        brain.step(step)
    
    profiler.disable()
    
    # Generate profiling report
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumulative')
    stats.print_stats()
    
    return True

# Alternative: Use pynlm's built-in profiling
profiler = pynlm.createProfiler()
profiler.start()

# Run simulation
for step in range(1000):
    brain.step(step)

profiler.stop()
report = profiler.generateReport()
```

## Integration Examples

### 1. Integration with Game Engine
```python
import pynlm
import pygame

class NLMSimulation:
    """NLM simulation for game engine integration."""
    
    def __init__(self):
        # Initialize NLM
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Initialize world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=800, height=600, visionWidth=16, visionHeight=12)
        self.world.setAgentStart(400, 300)
        
        # Initialize agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Game state
        self.running = True
        self.clock = pygame.time.Clock()
        self.step_time = 0.1
    
    def run(self):
        """Run simulation loop."""
        while self.running:
            # Process events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
            
            # Update simulation
            self.update()
            
            # Render
            self.render()
            
            # Control frame rate
            self.clock.tick(60)
        
        # Cleanup
        self.cleanup()
    
    def update(self):
        """Update simulation state."""
        # Update world
        self.world.update(self.step_time)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process in agent
        self.agent.processSensoryInput(percept)
        
        # Run brain
        self.brain.step(0)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Print statistics periodically
        if int(self.world.getSimulationTime() / self.step_time) % 100 == 0:
            print(f"Step: {self.world.getSimulationTime():.1f}, "
                  f"Firing: {self.brain.getFiringNeuronCount()}, "
                  f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
    
    def render(self):
        """Render simulation."""
        # Clear screen
        screen.fill((0, 0, 0))
        
        # Draw world
        self.drawWorld()
        
        # Draw agent
        self.drawAgent()
        
        # Draw UI
        self.drawUI()
        
        # Update display
        pygame.display.flip()
    
    def drawWorld(self):
        """Draw world."""
        # Draw world grid
        for x in range(0, self.world.getWidth(), self.world.getVisionWidth()):
            pygame.draw.line(screen, (50, 50, 50), (x, 0), (x, self.world.getHeight()))
        
        for y in range(0, self.world.getHeight(), self.world.getVisionHeight()):
            pygame.draw.line(screen, (50, 50, 50), (0, y), (self.world.getWidth(), y))
    
    def drawAgent(self):
        """Draw agent."""
        body = self.world.getAgentBody()
        pygame.draw.circle(screen, (0, 255, 0), 
                          (int(body.x), int(body.y)), body.radius)
    
    def drawUI(self):
        """Draw UI."""
        # Draw statistics
        font = pygame.font.Font(None, 30)
        
        text = font.render(f"Step: {self.world.getSimulationTime():.1f}", True, (255, 255, 255))
        screen.blit(text, (10, 10))
        
        text = font.render(f"Firing: {self.brain.getFiringNeuronCount()}", True, (255, 255, 255))
        screen.blit(text, (10, 50))
        
        text = font.render(f"Curiosity: {self.agent.getCuriosityLevel():.3f}", True, (255, 255, 255))
        screen.blit(text, (10, 90))
        
        text = font.render(f"Novelty: {self.agent.getNoveltyLevel():.3f}", True, (255, 255, 255))
        screen.blit(text, (10, 130))
    
    def cleanup(self):
        """Cleanup simulation."""
        # Cleanup NLM components
        del self.agent, self.brain, self.world
        import gc
        gc.collect()

# Run simulation
if __name__ == "__main__":
    simulation = NLMSimulation()
    simulation.run()
```

### 2. Integration with Web Framework
```python
import pynlm
import json
from http.server import BaseHTTPRequestHandler, HTTPServer

class NLMWebAPI:
    """Web API for NLM simulation."""
    
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        self.server = HTTPServer(('localhost', 8080), self.RequestHandler)
    
    class RequestHandler(BaseHTTPRequestHandler):
        def __init__(self, *args, api_ref=None, **kwargs):
            self.api = api_ref
            super().__init__(*args, **kwargs)
        
        def do_GET(self):
            if self.path == '/status':
                self.send_status_response()
            elif self.path == '/step':
                self.send_step_response()
            elif self.path == '/config':
                self.send_config_response()
            elif self.path == '/reset':
                self.send_reset_response()
            else:
                self.send_error(404)
        
        def do_POST(self):
            if self.path == '/config':
                self.receive_config()
            else:
                self.send_error(404)
        
        def send_status_response(self):
            """Send simulation status."""
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            
            status = {
                'step': self.api.world.getSimulationTime(),
                'firing_neurons': self.api.brain.getFiringNeuronCount(),
                'curiosity': self.api.agent.getCuriosityLevel(),
                'novelty': self.api.agent.getNoveltyLevel()
            }
            
            self.wfile.write(json.dumps(status).encode())
        
        def send_step_response(self):
            """Send simulation step."""
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            
            # Take one step
            self.api.world.update(0.1)
            percept = self.api.world.getSensoryPercept()
            self.api.agent.processSensoryInput(percept)
            self.api.brain.step(0)
            action = self.api.agent.decodeMotorCommand()
            self.api.world.applyMotorCommand(action, self.api.world.getSimulationTime())
            
            step_data = {
                'step': self.api.world.getSimulationTime(),
                'firing_neurons': self.api.brain.getFiringNeuronCount(),
                'curiosity': self.api.agent.getCuriosityLevel(),
                'novelty': self.api.agent.getNoveltyLevel(),
                'action': action.type.value if hasattr(action, 'type') else None
            }
            
            self.wfile.write(json.dumps(step_data).encode())
        
        def send_config_response(self):
            """Send current configuration."""
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            
            config = {
                'neuron_count': self.api.brain.getTotalNeuronCount(),
                'synapse_count': self.api.brain.getTotalSynapseCount(),
                'learning_rate': self.api.agent.getCuriosityLevel()  # Placeholder
            }
            
            self.wfile.write(json.dumps(config).encode())
        
        def send_reset_response(self):
            """Reset simulation."""
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            
            # Reset simulation
            self.api.world.reset()
            self.api.agent = pynlm.createAgentBrain(self.api.brain)
            self.api.agent.initialize(self.api.world)
            
            response = {'status': 'reset', 'success': True}
            self.wfile.write(json.dumps(response).encode())
        
        def receive_config(self):
            """Receive configuration."""
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            
            config = json.loads(post_data.decode())
            
            # Update configuration
            if 'neuron_count' in config:
                self.api.brain = pynlm.createBrain(pynlm.createDefaultConfig())
                self.api.brain.initialize()
                self.api.agent = pynlm.createAgentBrain(self.api.brain)
                self.api.agent.initialize(self.api.world)
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            
            response = {'status': 'config_updated', 'success': True}
            self.wfile.write(json.dumps(response).encode())
        
        def log_message(self, format, *args):
            """Log messages."""
            pass  # Disable logging

# Run web server
if __name__ == "__main__":
    api = NLMWebAPI()
    print("Starting NLM Web API server on http://localhost:8080")
    api.server.serve_forever()
```

### 3. Integration with Data Science
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler

class NLMDatasetGenerator:
    """Generate NLM simulation datasets for machine learning."""
    
    def __init__(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        self.data = []
    
    def generate_dataset(self, steps, interval=10):
        """Generate simulation dataset."""
        for step in range(steps):
            # Update simulation
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process in agent
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect data at intervals
            if step % interval == 0:
                self.collectData(step)
            
            # Print progress
            if step % 1000 == 0:
                print(f"Generated {step} steps")
        
        return pd.DataFrame(self.data)
    
    def collectData(self, step):
        """Collect simulation data."""
        data_point = {
            'step': step,
            'simulation_time': self.world.getSimulationTime(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel()
        }
        
        self.data.append(data_point)
    
    def analyzeData(self, df):
        """Analyze simulation data."""
        print("Dataset Analysis:")
        print(f"Shape: {df.shape}")
        print(f"\nDescriptive Statistics:")
        print(df.describe())
        
        print(f"\nCorrelation Matrix:")
        print(df.corr())
        
        # Plot time series
        plt.figure(figsize=(12, 10))
        
        plt.subplot(3, 1, 1)
        plt.plot(df['simulation_time'], df['firing_neurons'])
        plt.title('Firing Neurons over Time')
        plt.xlabel('Time')
        plt.ylabel('Firing Neurons')
        
        plt.subplot(3, 1, 2)
        plt.plot(df['simulation_time'], df['curiosity'])
        plt.title('Curiosity over Time')
        plt.xlabel('Time')
        plt.ylabel('Curiosity')
        
        plt.subplot(3, 1, 3)
        plt.plot(df['simulation_time'], df['novelty'])
        plt.title('Novelty over Time')
        plt.xlabel('Time')
        plt.ylabel('Novelty')
        
        plt.tight_layout()
        plt.show()
        
        return df
    
    def prepareFeatures(self, df):
        """Prepare features for machine learning."""
        # Select features
        features = df[['firing_neurons', 'curiosity', 'novelty', 
                      'prediction_error', 'neuromodulation']]
        
        # Normalize features
        scaler = StandardScaler()
        features_scaled = scaler.fit_transform(features)
        
        # Add time feature
        features_scaled = np.column_stack([
            features_scaled,
            df['simulation_time'].values / df['simulation_time'].max()
        ])
        
        return features_scaled, scaler

# Generate and analyze dataset
if __name__ == "__main__":
    dataset_generator = NLMDatasetGenerator()
    
    print("Generating NLM simulation dataset...")
    df = dataset_generator.generate_dataset(steps=100000, interval=10)
    
    print("\nDataset generated successfully!")
    print(f"Total data points: {len(df)}")
    
    # Analyze data
    df_analyzed = dataset_generator.analyzeData(df)
    
    # Prepare features for machine learning
    X, scaler = dataset_generator.prepareFeatures(df)
    print(f"\nPrepared features for machine learning: {X.shape}")
```

## Advanced Usage Examples

### 1. Custom Brain Implementation
```python
import pynlm

class CustomBrain(pynlm.Brain):
    """Custom brain implementation."""
    
    def __init__(self, config=None):
        super().__init__(config)
        self.custom_neurons = []
        self.custom_plasticity = True
    
    def initialize(self):
        """Initialize custom brain."""
        super().initialize()
        
        # Add custom initialization
        self.setupCustomNeurons()
        self.configureCustomPlasticity()
    
    def setupCustomNeurons(self):
        """Setup custom neurons."""
        for i in range(self.getTotalNeuronCount()):
            if i % 10 == 0:  # Every 10th neuron is custom
                self.custom_neurons.append({
                    'id': i,
                    'type': 'custom',
                    'properties': {'plasticity': self.custom_plasticity}
                })
    
    def step(self, step_number):
        """Process custom step."""
        super().step(step_number)
        
        # Apply custom processing
        self.processCustomNeurons(step_number)
        self.applyCustomLearning(step_number)
    
    def processCustomNeurons(self, step_number):
        """Process custom neurons."""
        for neuron in self.custom_neurons:
            # Custom processing logic
            if neuron['type'] == 'custom':
                # Apply custom dynamics
                self.customDynamics(neuron, step_number)
    
    def customDynamics(self, neuron, step_number):
        """Apply custom neural dynamics."""
        # Custom dynamics implementation
        neuron['activity'] = np.tanh(neuron['activity'])
        neuron['plasticity'] *= 1.1 - neuron['activity']
    
    def applyCustomLearning(self, step_number):
        """Apply custom learning."""
        # Custom learning logic
        if step_number % 100 == 0:
            # Periodic learning update
            self.updateCustomPlasticity()
    
    def updateCustomPlasticity(self):
        """Update custom plasticity."""
        for neuron in self.custom_neurons:
            neuron['plasticity'] *= 0.95

# Use custom brain
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    brain = CustomBrain(config)
    
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}: Custom neurons {len(brain.custom_neurons)}")
```

### 2. Multi-Agent System
```python
import pynlm
from threading import Thread
import time

class MultiAgentSystem:
    """Multi-agent NLM system."""
    
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=100, height=100)
        
        # Create agents
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            # Customize agent configuration
            config.set("brain.neuron_count", 200)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            self.agents.append({
                'id': i,
                'brain': brain,
                'agent': agent,
                'active': True
            })
        
        self.running = True
    
    def runAgents(self):
        """Run all agents in parallel."""
        threads = []
        
        for agent_data in self.agents:
            if agent_data['active']:
                thread = Thread(target=self.runAgent, args=(agent_data,))
                threads.append(thread)
                thread.start()
        
        # Wait for all agents to complete
        for thread in threads:
            thread.join()
    
    def runAgent(self, agent_data):
        """Run single agent."""
        agent = agent_data['agent']
        brain = agent_data['brain']
        
        while agent_data['active'] and self.running:
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(0)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print agent status
            print(f"Agent {agent_data['id']}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
            
            # Simulate agent thinking time
            time.sleep(0.05)
    
    def stopAgents(self):
        """Stop all agents."""
        self.running = False
        
        for agent_data in self.agents:
            agent_data['active'] = False
    
    def getSystemStatus(self):
        """Get system status."""
        status = {
            'running': self.running,
            'agents': [],
            'world_stats': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'time': self.world.getSimulationTime()
            }
        }
        
        for agent_data in self.agents:
            agent_status = {
                'id': agent_data['id'],
                'active': agent_data['active'],
                'brain_stats': {
                    'neurons': agent_data['brain'].getTotalNeuronCount(),
                    'firing': agent_data['brain'].getFiringNeuronCount()
                },
                'agent_stats': {
                    'curiosity': agent_data['agent'].getCuriosityLevel(),
                    'novelty': agent_data['agent'].getNoveltyLevel()
                }
            }
            status['agents'].append(agent_status)
        
        return status

# Use multi-agent system
if __name__ == "__main__":
    system = MultiAgentSystem(num_agents=3)
    
    try:
        print("Starting multi-agent system...")
        system.runAgents()
        
    finally:
        print("Stopping multi-agent system...")
        system.stopAgents()
    
    # Print system status
    status = system.getSystemStatus()
    print(f"\nSystem Status: {status}")
```

### 3. Hybrid C++/Python Integration
```python
import pynlm
import ctypes
import os

class HybridNLM:
    """Hybrid C++/Python NLM system."""
    
    def __init__(self, config=None):
        # Initialize Python brain
        if config is None:
            config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Initialize world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        
        # Initialize agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Load C++ library for performance-critical operations
        self.loadCppLibrary()
    
    def loadCppLibrary(self):
        """Load C++ performance library."""
        # Find and load C++ library
        lib_path = os.path.join(os.path.dirname(__file__), 'build', 'lib', 'nlm_performance.so')
        
        if os.path.exists(lib_path):
            self.cpp_lib = ctypes.CDLL(lib_path)
            print(f"Loaded C++ library: {lib_path}")
        else:
            print("C++ library not found, using Python implementation")
            self.cpp_lib = None
    
    def hybridStep(self, step_number):
        """Perform hybrid C++/Python step."""
        if self.cpp_lib is not None:
            # Use C++ implementation for performance-critical operations
            self.cpp_lib.processStep(step_number)
        
        # Use Python implementation for higher-level logic
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(step_number)
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def runHybridSimulation(self, steps):
        """Run hybrid simulation."""
        print(f"Running hybrid simulation with {steps} steps")
        
        for step in range(steps):
            # Perform hybrid step
            self.hybridStep(step)
            
            # Print progress
            if step % 1000 == 0:
                print(f"Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
        
        return True

# Use hybrid NLM
if __name__ == "__main__":
    hybrid_nlm = HybridNLM()
    
    try:
        # Run hybrid simulation
        hybrid_nlm.runHybridSimulation(10000)
        
    except Exception as e:
        print(f"Error in hybrid simulation: {e}")
        raise
```

## Performance Benchmarks

### Basic Performance
```python
import pynlm
import time

def benchmarkBasicPerformance():
    """Benchmark basic performance."""
    print("Benchmarking basic performance...")
    
    # Test with different brain sizes
    sizes = [100, 500, 1000, 5000]
    
    for size in sizes:
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time initialization
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Time simulation
        start_time = time.time()
        for step in range(1000):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Calculate metrics
        neurons_per_second = size / init_time
        steps_per_second = 1000 / sim_time
        
        print(f"Size {size}:")
        print(f"  Initialization: {init_time:.4f}s ({neurons_per_second:.0f} neurons/s)")
        print(f"  Simulation: {sim_time:.4f}s ({steps_per_second:.0f} steps/s)")
        
        del brain

if __name__ == "__main__":
    benchmarkBasicPerformance()
```

### Memory Usage Benchmark
```python
import pynlm
import psutil
import os

def benchmarkMemoryUsage():
    """Benchmark memory usage."""
    print("Benchmarking memory usage...")
    
    process = psutil.Process(os.getpid())
    
    # Test with different brain sizes
    sizes = [100, 500, 1000, 5000]
    
    for size in sizes:
        # Get initial memory
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory after brain creation
        brain_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Get memory after simulation
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Calculate memory metrics
        brain_memory_overhead = brain_memory - initial_memory
        simulation_memory_overhead = final_memory - brain_memory
        memory_per_neuron = brain_memory_overhead / size
        memory_per_step = simulation_memory_overhead / 1000
        
        print(f"Size {size}:")
        print(f"  Brain memory overhead: {brain_memory_overhead:.2f} MB ({memory_per_neuron:.4f} MB/neuron)")
        print(f"  Simulation memory overhead: {simulation_memory_overhead:.2f} MB ({memory_per_step:.4f} MB/step)")
        
        del brain

if __name__ == "__main__":
    benchmarkMemoryUsage()
```

### Scalability Benchmark
```python
import pynlm
import time
import numpy as np

def benchmarkScalability():
    """Benchmark scalability."""
    print("Benchmarking scalability...")
    
    # Test with different brain sizes
    sizes = [100, 500, 1000, 5000, 10000]
    
    throughputs = []
    
    for size in sizes:
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        for step in range(1000):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Calculate throughput
        steps_per_second = 1000 / sim_time
        neurons_per_second = size / sim_time
        
        throughputs.append(steps_per_second)
        
        print(f"Size {size}: {steps_per_second:.0f} steps/s ({neurons_per_second:.0f} neurons/s)")
        
        del brain
    
    # Analyze scalability
    throughput_array = np.array(throughputs)
    
    # Calculate scalability
    scalability = []
    for i in range(1, len(throughputs)):
        size_ratio = sizes[i] / sizes[i-1]
        throughput_ratio = throughputs[i] / throughputs[i-1]
        efficiency = throughput_ratio / size_ratio * 100
        scalability.append(efficiency)
    
    print(f"\nScalability Analysis:")
    for i, efficiency in enumerate(scalability):
        print(f"  Size ratio {sizes[i+1]}/{sizes[i]}: {efficiency:.1f}% efficiency")
    
    avg_efficiency = np.mean(scalability)
    print(f"  Average scalability efficiency: {avg_efficiency:.1f}%")

if __name__ == "__main__":
    benchmarkScalability()
```

## Testing

### Unit Tests
```python
import unittest
import pynlm

class TestBrain(unittest.TestCase):
    """Test brain functionality."""
    
    def setUp(self):
        """Set up test."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test."""
        del self.brain
    
    def testBrainCreation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
    
    def testBrainInitialization(self):
        """Test brain initialization."""
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
    
    def testBrainStep(self):
        """Test brain step."""
        initial_firing = self.brain.getFiringNeuronCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_firing = self.brain.getFiringNeuronCount()
        self.assertGreaterEqual(final_firing, initial_firing)
    
    def testBrainStats(self):
        """Test brain statistics."""
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        
        self.assertGreater(neuron_count, 0)
        self.assertGreater(synapse_count, 0)
        self.assertLess(synapse_count, neuron_count * 10)  # Reasonable synapse count

class TestConfig(unittest.TestCase):
    """Test configuration functionality."""
    
    def setUp(self):
        """Set up test."""
        self.config = pynlm.createDefaultConfig()
    
    def testConfigCreation(self):
        """Test configuration creation."""
        self.assertIsNotNone(self.config)
    
    def testConfigSetGet(self):
        """Test configuration set/get."""
        self.config.set("test_key", "test_value")
        self.assertEqual(self.config.get("test_key"), "test_value")
    
    def testConfigGetInt(self):
        """Test configuration get int."""
        self.config.set("test_int", 42)
        self.assertEqual(self.config.getInt("test_int"), 42)
    
    def testConfigGetDouble(self):
        """Test configuration get double."""
        self.config.set("test_double", 3.14)
        self.assertEqual(self.config.getDouble("test_double"), 3.14)
    
    def testConfigGetBool(self):
        """Test configuration get bool."""
        self.config.set("test_bool", True)
        self.assertTrue(self.config.getBool("test_bool"))
    
    def testConfigGetString(self):
        """Test configuration get string."""
        self.config.set("test_string", "hello")
        self.assertEqual(self.config.getString("test_string"), "hello")
    
    def testConfigHasKey(self):
        """Test configuration has key."""
        self.assertFalse(self.config.has("nonexistent_key"))
        self.config.set("existing_key", "value")
        self.assertTrue(self.config.has("existing_key"))

class TestAgent(unittest.TestCase):
    """Test agent functionality."""
    
    def setUp(self):
        """Set up test."""
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test."""
        del self.agent, self.brain, self.world
    
    def testAgentCreation(self):
        """Test agent creation."""
        self.assertIsNotNone(self.agent)
    
    def testAgentInitialization(self):
        """Test agent initialization."""
        self.assertIsNotNone(self.agent)
    
    def testAgentSensoryProcessing(self):
        """Test agent sensory processing."""
        # Update world to get percept
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify processing
        self.assertTrue(True)  # If no exception, processing succeeded
    
    def testAgentMotorDecoding(self):
        """Test agent motor decoding."""
        # Update world to get action
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(0)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        
        # Verify action
        self.assertIsNotNone(action)
        self.assertGreater(action.x, -1000)
        self.assertGreater(action.y, -1000)
        self.assertGreater(action.z, -1000)
    
    def testAgentLearning(self):
        """Test agent learning."""
        # Update world
        self.world.update(0.1)
        
        # Get percept and process
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(0)
        
        # Apply reward modulation
        reward = percept.getReward() if hasattr(percept, 'getReward') else 0.0
        predicted_reward = 0.0
        
        initial_curiosity = self.agent.getCuriosityLevel()
        
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Verify learning
        final_curiosity = self.agent.getCuriosityLevel()
        self.assertGreaterEqual(final_curiosity, initial_curiosity)

if __name__ == "__main__":
    unittest.main()
```

## Performance Monitoring

### Performance Profiler
```python
import pynlm
import time

class PerformanceProfiler:
    """Performance profiler for NLM."""
    
    def __init__(self, name="performance_profile"):
        self.name = name
        self.metrics = {}
        self.start_time = None
    
    def start(self):
        """Start profiling."""
        self.start_time = time.time()
        self.metrics['start_time'] = self.start_time
    
    def stop(self):
        """Stop profiling."""
        end_time = time.time()
        self.metrics['end_time'] = end_time
        
        if self.start_time:
            self.metrics['duration'] = end_time - self.start_time
    
    def recordMetric(self, name, value):
        """Record performance metric."""
        if 'metrics' not in self.metrics:
            self.metrics['metrics'] = {}
        self.metrics['metrics'][name] = value
    
    def generateReport(self):
        """Generate performance report."""
        report = {
            'name': self.name,
            'duration': self.metrics.get('duration', 0),
            'start_time': self.metrics.get('start_time'),
            'end_time': self.metrics.get('end_time'),
            'metrics': self.metrics.get('metrics', {})
        }
        
        return report
    
    def saveReport(self, filename):
        """Save performance report to file."""
        import json
        
        report = self.generateReport()
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
        
        return True

# Use performance profiler
def profileSimulation():
    """Profile simulation performance."""
    profiler = PerformanceProfiler("nlm_simulation")
    
    # Create NLM
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Start profiling
    profiler.start()
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        profiler.recordMetric(f'step_{step}', brain.getFiringNeuronCount())
    
    # Stop profiling
    profiler.stop()
    
    # Generate report
    report = profiler.generateReport()
    print(f"Simulation completed in {report['duration']:.2f}s")
    print(f"Average firing neurons: {np.mean(list(report['metrics'].values())):.0f}")
    
    # Save report
    profiler.saveReport("simulation_profile.json")
    
    return report

if __name__ == "__main__":
    import numpy as np
    profileSimulation()
```

### Memory Monitor
```python
import pynlm
import psutil
import os

class MemoryMonitor:
    """Memory monitor for NLM."""
    
    def __init__(self, brain=None, name="memory_monitor"):
        self.name = name
        self.brain = brain
        self.process = psutil.Process(os.getpid())
        self.memory_samples = []
        self.start_time = time.time()
    
    def start(self):
        """Start monitoring."""
        self.start_time = time.time()
        self.collectMemorySample()
    
    def stop(self):
        """Stop monitoring."""
        self.collectMemorySample()
    
    def collectMemorySample(self):
        """Collect memory sample."""
        memory_info = self.process.memory_info()
        sample = {
            'timestamp': time.time() - self.start_time,
            'rss': memory_info.rss,
            'vms': memory_info.vms,
            'shared': memory_info.shared,
            'text': memory_info.text,
            'data': memory_info.data,
            'dirty': memory_info.dirty
        }
        
        self.memory_samples.append(sample)
    
    def getMemoryReport(self):
        """Get memory report."""
        if not self.memory_samples:
            return None
        
        rss_values = [s['rss'] for s in self.memory_samples]
        
        report = {
            'name': self.name,
            'start_time': self.start_time,
            'end_time': time.time(),
            'samples': len(self.memory_samples),
            'memory_stats': {
                'rss': {
                    'min': min(rss_values),
                    'max': max(rss_values),
                    'avg': sum(rss_values) / len(rss_values),
                    'std': np.std(rss_values)
                }
            }
        }
        
        return report
    
    def saveReport(self, filename):
        """Save memory report to file."""
        import json
        
        report = self.getMemoryReport()
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
        
        return True

# Use memory monitor
def monitorMemoryUsage():
    """Monitor memory usage during simulation."""
    monitor = MemoryMonitor(name="nlm_memory_monitoring")
    
    # Create NLM
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Start monitoring
    monitor.start()
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect memory sample
        monitor.collectMemorySample()
        
        if step % 100 == 0:
            print(f"Step {step}: Memory usage: {monitor.process.memory_info().rss / 1024 / 1024:.2f} MB")
    
    # Stop monitoring
    monitor.stop()
    
    # Generate report
    report = monitor.getMemoryReport()
    if report:
        print(f"\nMemory Report:")
        print(f"  Duration: {report['end_time'] - report['start_time']:.2f}s")
        print(f"  Samples: {report['samples']}")
        print(f"  RSS: {report['memory_stats']['rss']['avg'] / 1024 / 1024:.2f} ± {report['memory_stats']['rss']['std'] / 1024 / 1024:.2f} MB")
    
    # Save report
    monitor.saveReport("memory_report.json")
    
    return report

if __name__ == "__main__":
    import time
    import numpy as np
    monitorMemoryUsage()
```

## Conclusion

The NLM Python bindings provide a comprehensive and powerful Python API for interacting with the NLM neural simulation system. This documentation covers:

1. **Installation and Setup**: Complete guide for installing and setting up the Python bindings
2. **Core API**: Complete API reference for all NLM classes and functions
3. **Advanced Features**: NumPy integration, Pandas support, async operations, context managers, event system, and parallel processing
4. **Integration Examples**: Integration with game engines, web frameworks, data science tools, and hybrid C++/Python systems
5. **Performance Optimization**: Memory management, caching, parallel processing, and profiling
6. **Testing**: Comprehensive unit tests and performance benchmarks
7. **Error Handling**: Robust error handling with clear error messages
8. **Performance Monitoring**: Performance profiling, memory monitoring, and reporting

The Python bindings are designed for performance, flexibility, and ease of use, making them suitable for everything from simple demonstrations to complex, large-scale neural simulations.
