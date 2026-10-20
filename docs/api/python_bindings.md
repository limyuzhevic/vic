# Python Bindings API Reference

Comprehensive documentation for the NLM Python bindings. This is the core API that provides access to all NLM functionality through Python.

## Overview

The NLM Python bindings expose the complete NLM neural simulation framework through a user-friendly Python interface. The bindings provide access to:

- **Core brain functionality** - Neural processing, memory, and learning
- **Neuromodulation systems** - Dopamine, curiosity, novelty detection
- **Development system** - Brain maturation and critical periods
- **Advanced utilities** - Simulation control, benchmarking, and validation
- **Error handling** - Comprehensive exception management

## Module Structure

```python
import pynlm as nlm

# Core components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# World simulation
world = nlm.createSimpleWorld(width, height, visionWidth, visionHeight)
world.reset()
world.update(timestep)

# Agent interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Helper functions
stats = nlm.getBrainStats(brain)
nlm.runEpisode(agent, world, maxSteps)
```

## Core Classes

### Config Class

The `Config` class manages configuration parameters for NLM components.

```python
# Create configuration
config = nlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Set configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# File operations
config.saveToFile("my_config.json")
new_config = nlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Validation
is_valid = nlm.validateConfig(config)
if not is_valid:
    errors = nlm.getConfigValidationError(config)
    print(f"Configuration errors: {errors}")
```

#### Config Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createDefaultConfig()` | Create config with default values | None | Config |
| `has(key)` | Check if key exists | `key: str` | bool |
| `get(key)` | Get value for key | `key: str` | Any |
| `getOr(key, default)` | Get value or return default | `key: str, default: Any` | Any |
| `set(key, value)` | Set value for key | `key: str, value: Any` | None |
| `loadFromFile(filepath)` | Load from JSON file | `filepath: str` | None |
| `saveToFile(filepath)` | Save to JSON file | `filepath: str` | None |
| `clear()` | Clear all configuration | None | None |
| `summary()` | Get human-readable summary | None | str |

### Brain Class

The `Brain` class represents the core neural processing unit.

```python
# Create and initialize brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Simulation
for step in range(1000):
    brain.step(step)
    brain.step(step, timestamp)  # With time
    
# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
brain.reset()

# Sensory/motor
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()
ratio = brain.getExcitationInhibitionRatio()
```

#### Brain Methods

| Category | Method | Description | Parameters | Returns |
|----------|--------|-------------|------------|---------|
| **Creation** | `createBrain(config)` | Create brain with config | `config: Config` | Brain |
| | `createDefaultConfig()` | Create default config | None | Config |
| **Initialization** | `initialize()` | Initialize brain state | None | None |
| | `reset()` | Reset to initial state | None | None |
| **Simulation** | `step(step)` | Run simulation step | `step: int` | None |
| | `step(step, time)` | Run step with timestamp | `step: int, time: float` | None |
| **State** | `save(filepath)` | Save brain state | `filepath: str` | None |
| | `load(filepath)` | Load brain state | `filepath: str` | None |
| **I/O** | `receiveSensoryInput(input)` | Inject sensory data | `input: SensoryInput` | None |
| | `produceAction()` | Generate motor command | None | Action |
| **Statistics** | `getTotalNeuronCount()` | Total neurons | None | int |
| | `getTotalSynapseCount()` | Total synapses | None | int |
| | `getFiringNeuronCount()` | Currently firing neurons | None | int |
| | `getAverageFiringRate()` | Average firing rate (Hz) | None | float |
| | `getExcitationInhibitionRatio()` | E/I balance ratio | None | float |
| **Development** | `getDevelopmentalStage()` | Current developmental stage | None | DevelopmentalStage |
| | `setDevelopmentalStage(stage)` | Set developmental stage | `stage: DevelopmentalStage` | None |
| **Memory Access** | `getWorkingMemory()` | Get working memory | None | WorkingMemory |
| | `getEpisodicMemory()` | Get episodic memory | None | EpisodicMemory |
| | `getAssociativeMemory()` | Get associative memory | None | AssociativeMemory |
| | `getConceptFormation()` | Get concept formation | None | ConceptFormation |
| | `getSpatialRepresentation()` | Get spatial representation | None | SpatialRepresentation |
| | `getTemporalRelation()` | Get temporal relations | None | TemporalRelation |
| **Plasticity Access** | `getSTDP()` | Get STDP plasticity | None | STDP |
| | `getHebbian()` | Get Hebbian plasticity | None | Hebbian |
| | `getStructuralPlasticity()` | Get structural plasticity | None | StructuralPlasticity |
| **Neuromodulation Access** | `getDopamine()` | Get dopamine neuromodulator | None | Dopamine |
| | `getCuriosity()` | Get curiosity system | None | Curiosity |
| | `getNovelty()` | Get novelty detector | None | Novelty |
| | `getPredictionErrorSignal()` | Get prediction error system | None | PredictionError |
| | `getAcetylcholine()` | Get acetylcholine neuromodulator | None | Acetylcholine |
| | `getNorepinephrine()` | Get norepinephrine neuromodulator | None | Norepinephrine |
| | `getSerotonin()` | Get serotonin neuromodulator | None | Serotonin |
| **Development Access** | `getDevelopmentSystem()` | Get development system | None | DevelopmentSystem |
| **Prediction Access** | `getPredictionSystem()` | Get prediction system | None | PredictionSystem |
| | `getNeuralPrediction()` | Get neural prediction | None | NeuralPrediction |
| | `getActionConsequencePredictor()` | Get action consequence predictor | None | ActionConsequencePredictor |
| **Cognitive Access** | `getPlanner()` | Get neural planner | None | NeuralPlanner |
| | `getAttention()` | Get attentional selection | None | AttentionalSelection |
| | `getSelfModel()` | Get self model | None | SelfModel |
| | `getSocialLearning()` | Get social learning | None | SocialLearning |
| **Concept Access** | `getConceptFormation()` | Get concept formation | None | ConceptFormation |
| **Region Management** | `addRegion(name)` | Add brain region | `name: str` | int |
| | `getRegion(region_id)` | Get brain region | `region_id: int` | Region |

### AgentBrain Class

The `AgentBrain` class provides the interface between brain and world.

```python
# Create agent brain
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
stage = agent.getDevelopmentalStage()

# Subsystem control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Statistics
level = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
error = agent.getPredictionError()
neuromod = agent.getNeuromodulationLevel()
```

#### AgentBrain Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createAgentBrain(brain)` | Create agent brain interface | `brain: Brain` | AgentBrain |
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | None |
| `processSensoryInput(percept)` | Process sensory input | `percept: SensoryPercept` | None |
| `decodeMotorCommand()` | Decode motor command | None | Action |
| `applyRewardModulation(reward, predicted)` | Apply reward modulation | `reward: float, predicted: float` | None |
| `updateDevelopment(timestep)` | Update development | `timestep: float` | None |
| `getDevelopmentalStage()` | Get developmental stage | None | DevelopmentalStage |
| `enableRewardModulation(enable)` | Enable/disable reward modulation | `enable: bool` | None |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `enable: bool` | None |
| `enableDevelopment(enable)` | Enable/disable development | `enable: bool` | None |
| `enableCuriosity(enable)` | Enable/disable curiosity | `enable: bool` | None |
| `getCuriosityLevel()` | Get curiosity level | None | float |
| `getNoveltyLevel()` | Get novelty level | None | float |
| `getPredictionError()` | Get prediction error | None | float |
| `getNeuromodulationLevel()` | Get neuromodulation level | None | float |

### SimpleWorld Class

The `SimpleWorld` class represents the simulation environment.

```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Control
world.update(timestep)
world.applyMotorCommand(motor_cmd, time)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(WorldObject(x, y, WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)

# Properties
width = world.getWidth()
height = world.getHeight()
time = world.getSimulationTime()
max_energy = world.getMaxEnergy()
```

#### SimpleWorld Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createSimpleWorld()` | Create empty world | None | SimpleWorld |
| `configure(width, height, visionWidth, visionHeight)` | Configure world | `width: int, height: int, visionWidth: int, visionHeight: int` | None |
| `reset()` | Reset world state | None | None |
| `update(timestep)` | Update world by timestep | `timestep: float` | None |
| `setRandomSeed(seed)` | Set random seed | `seed: int` | None |
| `setAgentStart(x, y)` | Set agent starting position | `x: float, y: float` | None |
| `applyMotorCommand(cmd, time)` | Apply motor command | `cmd: Action, time: float` | None |
| `getSensoryPercept()` | Get sensory percept | None | SensoryPercept |
| `getAgentBody()` | Get agent body | None | Body |
| `addObject(object)` | Add world object | `object: WorldObject` | None |
| `removeObject(x, y)` | Remove object at position | `x: float, y: float` | None |
| `isValidPosition(x, y)` | Check if position valid | `x: float, y: float` | bool |
| `getWidth()` | Get world width | None | int |
| `getHeight()` | Get world height | None | int |
| `getSimulationTime()` | Get simulation time | None | float |
| `getMaxEnergy()` | Get maximum energy | None | float |
| `setMaxEnergy(value)` | Set maximum energy | `value: float` | None |

## Enumerations

### NeuronType

```python
# Neuron types
pynlm.NeuronType.Excitatory   # Excitatory neurons
pynlm.NeuronType.Inhibitory   # Inhibitory neurons
pynlm.NeuronType.Sensory      # Sensory neurons
pynlm.NeuronType.Motor        # Motor neurons
pynlm.NeuronType.Modulatory   # Modulatory neurons
pynlm.NeuronType.Internal     # Internal neurons
```

### DevelopmentalStage

```python
# Developmental stages
pynlm.DevelopmentalStage.Initial      # Initial stage
pynlm.DevelopmentalStage.CriticalPeriod # Critical learning period
pynlm.DevelopmentalStage.Maturation    # Maturation toward adult
pynlm.DevelopmentalStage.Adult        # Adult stage
pynlm.DevelopmentalStage.Aging        # Aging stage
```

### ActionType

```python
# Action types
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.TurnRight    # Turn right
pynlm.ActionType.Interact     # Interact with object
pynlm.ActionType.Eat         # Consume resource
pynlm.ActionType.Sleep       # Rest/consolidation
# ... and more
```

### WorldObjectType

```python
# World object types
pynlm.WorldObjectType.Empty    # Empty space
pynlm.WorldObjectType.Resource # Consumable resource
pynlm.WorldObjectType.Hazard   # Dangerous object
pynlm.WorldObjectType.Wall     # Impassable barrier
pynlm.WorldObjectType.Marker   # Target/location marker
```

## Helper Functions

### Factory Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createDefaultConfig()` | Create config with defaults | None | Config |
| `createBrain(config)` | Create and initialize brain | `config: Config` | Brain |
| `createSimpleWorld(width, height, visionWidth, visionHeight)` | Create configured world | `width: int, height: int, visionWidth: int, visionHeight: int` | SimpleWorld |
| `createAgentBrain(brain)` | Create agent brain interface | `brain: Brain` | AgentBrain |
| `buildNeuralNetwork(config, name)` | Build complete neural network | `config: Config, name: str` | Brain |
| `getBrainStats(brain)` | Get comprehensive brain statistics | `brain: Brain` | dict |
| `validateConfig(config)` | Validate configuration | `config: Config` | bool |
| `getConfigValidationError(config)` | Get validation errors | `config: Config` | str |
| `exportBrainState(brain, filepath)` | Save brain state | `brain: Brain, filepath: str` | None |
| `importBrainState(brain, filepath)` | Load brain state | `brain: Brain, filepath: str` | None |
| `runEpisode(agentBrain, world, maxSteps)` | Run complete simulation episode | `agentBrain: AgentBrain, world: SimpleWorld, maxSteps: int` | float |
| `resetEnvironment(world, agentBrain)` | Reset simulation environment | `world: SimpleWorld, agentBrain: AgentBrain` | None |
| `benchmarkStep(brain, steps)` | Benchmark brain performance | `brain: Brain, steps: int` | dict |

### Statistics and Monitoring

```python
# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)
print(f"Brain has {stats['totalNeurons']} neurons")
print(f"Firing rate: {stats['averageFiringRate']:.2f} Hz")
print(f"Memory usage: {stats['memoryUsage']:.1f}%")

# Benchmark performance
results = pynlm.benchmarkStep(brain, 1000)
print(f"Average step time: {results['avgTimePerStepMs']:.2f} ms")
print(f"Steps per second: {results['stepsPerSecond']:.0f}")
```

## Neuromodulation System Classes

### Reward Class

```python
# Create reward system
reward = pynlm.Reward()
reward.setValue(1.5)
print(f"Reward value: {reward.getValue()}")
reward.add(0.5)
print(f"Reward after add: {reward.getValue()}")
reward.reset()

# Compute reward from observation
computed = reward.computeReward(observation)
```

#### Reward Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Reward()` | Create reward system | None | Reward |
| `getValue()` | Get current reward value | None | float |
| `setValue(value)` | Set reward value | `value: float` | None |
| `add(delta)` | Add to reward | `delta: float` | None |
| `reset()` | Reset reward to zero | None | None |
| `computeReward(observation)` | Compute from observation | `observation: Any` | float |

### Neuromodulator Base Class

Abstract base class for all neuromodulators:

```python
# Abstract base class
class Neuromodulator:
    def getLevel(self) -> float: ...
    def setLevel(self, level: float) -> None: ...
    def initialize(self, brain: Brain) -> None: ...
    def update(self, *args, **kwargs) -> None: ...
```

### Dopamine

Dopamine neuromodulator for reward and reinforcement:

```python
# Create dopamine system
dopamine = pynlm.Dopamine()
dopamine.initialize(brain)

# Signal reward
dopamine.signalReward(reward_value)
print(f"Dopamine level: {dopamine.getLevel():.3f}")
```

### Acetylcholine

Acetylcholine neuromodulator for attention and memory:

```python
acetylcholine = pynlm.Acetylcholine()
```

### Norepinephrine

Norepinephrine neuromodulator for arousal and vigilance:

```python
norepinephrine = pynlm.Norepinephrine()
```

### Serotonin

Serotonin neuromodulator for mood and impulsivity:

```python
serotonin = pynlm.Serotonin()
```

## Curiosity System

Curiosity system for exploration motivation:

```python
# Create curiosity system
curiosity = pynlm.Curiosity()
curiosity.initialize(brain)

# Update with novelty and prediction error
for step in range(100):
    novelty = detect_novelty()
    prediction_error = compute_prediction_error()
    curiosity.update(novelty, prediction_error, 0.1)
    
    print(f"Curiosity level: {curiosity.getLevel():.3f}")
    print(f"Exploration drive: {curiosity.getExplorationDrive():.3f}")
```

## Novelty Detection

Novelty detection system:

```python
# Create novelty detector
novelty = pynlm.Novelty()
novelty.initialize(brain)

# Detect novelty
current = get_current_observation()
previous = get_previous_observation()
newness = novelty.detectNovelty(current, previous)
novelty.setLevel(newness)
```

## Prediction Error System

Prediction error system for learning:

```python
# Create prediction error system
prediction_error = pynlm.PredictionError()
prediction_error.initialize(brain)

# Compute error
predicted = brain.predict()
actual = brain.observe()
error = prediction_error.computeError(predicted, actual)
print(f"Prediction error: {error:.3f}")
print(f"Error magnitude: {prediction_error.getMagnitude():.3f}")
```

## Development System

Development system for brain maturation:

```python
# Create development system
dev_system = pynlm.DevelopmentSystem()
print(f"Current stage: {dev_system.getStageName()}")
print(f"Plasticity modifier: {dev_system.getPlasticityModifier():.3f}")
print(f"In critical period: {dev_system.isCriticalPeriod()}")

# Set stage
dev_system.setStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Advance stage
dev_system.advanceStage()

# Update development
dev_system.update(brain, timestep)
```

## Exception Classes

The Python bindings provide comprehensive exception handling:

### NLMError

Base exception for all NLM errors:

```python
try:
    brain = pynlm.createBrain(config)
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
```

### InvalidParameterError

Error for invalid parameters:

```python
try:
    brain = pynlm.createBrain(invalid_config)
except pynlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")
```

### InitializationError

Error for initialization failures:

```python
try:
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Initialization failed: {e}")
```

### OperationError

Error for operation failures:

```python
try:
    brain.save("invalid_path.brain")
except pynlm.OperationError as e:
    print(f"Operation failed: {e}")
```

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

# Create configuration with basic settings
config = nlm.createDefaultConfig()

# Customize parameters
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.synapse_density", 0.05)        # Fewer connections
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("plasticity.hebbian.enable", True)    # Enable Hebbian
config.set("neuromodulation.curiosity.enable", True)  # Enable curiosity
config.set("development.initialStage", "CriticalPeriod")

# Validate configuration
if nlm.validateConfig(config):
    brain = nlm.createBrain(config)
    brain.initialize()
    print("Configuration valid and brain created successfully!")
else:
    print(f"Configuration errors: {nlm.getConfigValidationError(config)}")
```

### Example 2: Advanced Configuration

```python
# Create advanced configuration
config = nlm.createDefaultConfig()

# Learning system configuration
config.set("brain.neuron_count", 5000)
config.set("brain.learning_rate", 0.01)
config.set("brain.momentum", 0.9)
config.set("brain.discount_factor", 0.95)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.exploration_weight", 0.8)
config.set("neuromod.novelty.threshold", 0.3)

# Development configuration
config.set("development.critical_period_length", 0.3)
config.set("development.maturation_rate", 0.05)
config.set("development.plasticity_decay", 0.01)

# Performance configuration
config.set("simulation.max_memory", 1000.0)
config.set("simulation.max_steps", 10000)
config.set("simulation.timestep", 0.1)

# Save configuration
config.saveToFile("advanced_config.json")

# Load and use
new_config = nlm.createDefaultConfig()
new_config.loadFromFile("advanced_config.json")
brain = nlm.createBrain(new_config)
```

### Example 3: Complete Simulation

```python
import pynlm

def run_agent_simulation(world_size=100, num_steps=1000):
    """Run a complete agent simulation with learning."""
    
    # 1. Create and configure brain
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.exploration_weight", 0.5)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = nlm.createSimpleWorld(world_size, world_size, 20, 20)
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=20,
        visionHeight=20
    )
    world.reset()
    world.setAgentStart(world_size/2, world_size/2)
    
    # 3. Create agent brain
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Create neuromodulators
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    # 6. Run simulation
    total_reward = 0.0
    print(f"Running simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply neuromodulation
        if dopamine:
            dopamine.signalReward(reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Accumulate reward
        total_reward += reward
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Reward: {reward:.3f}, Total: {total_reward:.3f}")
            print(f"  Brain firing: {brain.getFiringNeuronCount()}")
            if curiosity:
                print(f"  Curiosity: {curiosity.getLevel():.3f}")
    
    # 7. Show results
    print(f"\nSimulation complete!")
    print(f"Total reward: {total_reward:.3f}")
    print(f"Average reward per step: {total_reward/num_steps:.3f}")
    print(f"Final curiosity level: {curiosity.getLevel() if curiosity else 0:.3f}")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    
    # 8. Save brain state
    nlm.exportBrainState(brain, "simulation_checkpoint.brain")
    
    return brain, world, agent, total_reward

# Run simulation
brain, world, agent, reward = run_agent_simulation(200, 500)
```

## Performance Tips

### Memory Management

```python
# Use configuration to control memory usage
config = nlm.getDefaultConfigTemplate()
config.set("simulation.max_memory", 500.0)  # Limit memory

brain = nlm.createBrain(config)

# Monitor memory usage
stats = nlm.getBrainStats(brain)
if "memoryUsage" in stats:
    print(f"Memory usage: {stats['memoryUsage']:.1f}%")
```

### Performance Optimization

```python
# Create optimized configuration
config = nlm.createDefaultConfig()
config.set("brain.learningRate", 0.01)           # Faster learning
config.set("brain.connectionProbability", 0.05)  # Sparse connectivity

brain = nlm.createBrain(config)

# Run performance benchmark
results = nlm.benchmarkStep(brain, 500)
print(f"Performance: {results['stepsPerSecond']:.1f} steps/second")
print(f"Average step time: {results['avgTimePerStepMs']:.2f} ms")
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### Issue 1: Module Not Found
```bash
pip install pynlm
```

#### Issue 2: Build Failures
```bash
# Check compiler
cpp --version

# Check CMake version
cmake --version

# Build with verbose output
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

#### Issue 3: Missing Dependencies
```bash
pip install pybind11 numpy pytest
```

#### Issue 4: Runtime Errors
```python
import pynlm as nlm

try:
    # Your code here
    brain = nlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    
except nlm.NLMError as e:
    print(f"NLM Error: {e}")
except nlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")
except nlm.InitializationError as e:
    print(f"Initialization failed: {e}")
except nlm.OperationError as e:
    print(f"Operation failed: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Safe Programming Patterns

```python
import pynlm as nlm

def safe_brain_operation(brain):
    """Safe brain operation with comprehensive error handling."""
    
    # Check for null pointers
    if brain is None:
        raise nlm.InvalidParameterError("brain", "cannot be null")
    
    # Check configuration validity
    config = brain.getConfig()
    if not nlm.validateConfig(config):
        error_msg = nlm.getConfigValidationError(config)
        raise nlm.InvalidParameterError("config", f"invalid: {error_msg}")
    
    # Check optional systems
    try:
        dev_system = brain.getDevelopmentSystem()
        if dev_system is not None:
            stage = dev_system.getStage()
            print(f"Development stage: {stage}")
        else:
            print("Warning: Development system not available")
    except Exception as e:
        print(f"Warning: Could not access development system: {e}")
    
    try:
        dopamine = brain.getDopamine()
        if dopamine is not None:
            print(f"Dopamine level: {dopamine.getLevel():.3f}")
        else:
            print("Warning: Dopamine system not available")
    except Exception as e:
        print(f"Warning: Could not access dopamine system: {e}")
    
    # Perform safe operations
    neurons = brain.getTotalNeuronCount()
    print(f"Brain has {neurons} neurons")
    
    # Try to save state (may fail due to path issues)
    try:
        brain.save("checkpoint.brain")
        print("Brain state saved successfully")
    except nlm.OperationError as e:
        print(f"Could not save brain state: {e}")
    
    return neurons

# Use the safe function
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

result = safe_brain_operation(brain)
print(f"Operation completed safely, result: {result}")
```

## Migration Guide

### From Old API to New API

#### Configuration
```python
# Old way
config = Config()
config.set("neuronCount", 100)

# New way
config = nlm.createDefaultConfig()
config.set("brain.neuronCount", 100)
```

#### Brain Creation
```python
# Old way
brain = Brain(config)

# New way
brain = nlm.createBrain(config)
```

#### Error Handling
```python
# Old way
try:
    brain.initialize()
except:
    print("Error")

# New way
try:
    brain = nlm.createBrain(config)
except nlm.InitializationError as e:
    print(f"Initialization failed: {e}")
```

## Supported Platforms

### Operating Systems
- Linux (all distributions)
- macOS (Intel and Apple Silicon)
- Windows (with Visual Studio 2019+)

### Python Versions
- Python 3.8
- Python 3.9
- Python 3.10
- Python 3.11
- Python 3.12

### Architecture Support
- x86_64 (64-bit)
- ARM64 (Apple Silicon, some Raspberry Pi)

## License

MIT License

For commercial use and support inquiries, contact the NLM team.
