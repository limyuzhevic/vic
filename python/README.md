# NLM Python API Reference

## Overview

The NLM (Neural Learning Machine) Python API provides complete bindings for the artificial developmental brain framework, including all Phase 6 integrated systems.

**Key Features:**
- **Full Integration**: All memory, cognition, prediction, and neuromodulation systems
- **High Performance**: Access to SIMD, multithreading, sparse connectivity optimizations
- **Complete API**: Every C++ system exposed with full Python documentation
- **Ready to Use**: No compilation or configuration required

## Installation

```bash
pip install pynlm
```

The Python bindings include:
- Neural brain simulation with integrated memory systems
- Agent-world interaction with sensory processing
- Complete developmental lifecycle
- Advanced cognitive capabilities (planning, concept formation)
- Real-time prediction and learning

## Core Classes

### Brain
Central neural simulation brain with fully integrated systems:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Step through simulation
for step in range(1000):
    brain.step(step)

# Access integrated systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
planner = brain.getPlanner()
prediction_system = brain.getPredictionSystem()
```

**Integrated Systems:**
- **Memory**: WorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
- **Cognition**: NeuralPlanner, ConceptFormation, attention
- **Prediction**: PredictionSystem, NeuralPrediction
- **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
- **Development**: DevelopmentSystem with full stage transitions
- **Plasticity**: STDP, Hebbian, StructuralPlasticity

### AgentBrain
Bridge between brain and world environment:

```python
agent_brain = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

agent_brain.initialize(world)

# Sensory-motor loop
for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**Key Features:**
- Automatic sensory processing and motor decoding
- Integrated reward modulation and learning
- Curiosity-driven exploration behavior
- Developmental stage management

### SimpleWorld
2D simulation environment:

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(5, 5)

# Add objects
for obj in objects:
    world.addObject(obj)

# Run simulation
for step in range(100):
    world.update(0.1)  # Update physics
    percept = world.getSensoryPercept()
```

**Capabilities:**
- Physics simulation and collision detection
- Comprehensive sensory processing (vision, touch, internal)
- Object manipulation and interaction
- Energy, health, and state management

## Memory Systems

### WorkingMemory
Active information maintenance with neural traces:

```python
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain)

# Store neural activation
working_memory.storeToNeuron(neuron_id, 0.75)
active_traces = working_memory.getActiveTraces()  # Number of active traces
```

**Features:**
- Persistent neural activity
- Competitive selection
- Memory trace decay
- Capacity management

### NeuralEpisodicMemory
Experience storage and retrieval with pattern completion:

```python
episodic_memory = brain.getEpisodicMemory()
episodic_memory.initialize(brain)

# Store experience
episode = EpisodicMemoryItem(
    timestamp=current_step,
    reward=reward,
    action=action,
    sensory_state=sensory_data,
    resulting_state=next_sensory_state,
    active_neurons=list_of_active_neurons,
    neuron_activations=activation_levels
)
episodic_memory.storeEpisode(episode)

# Retrieve similar experiences
retrieved = episodic_memory.retrieveSimilar(current_sensory_state, max_results=5)
for ep in retrieved:
    print(f"Episode at step {ep.timestamp} with reward {ep.reward}")
```

**Retrieval Capabilities:**
- Pattern-based similarity matching
- Temporal filtering (by time range)
- Spatial location retrieval
- Action-specific retrieval
- Replay for consolidation

### NeuralAssociativeMemory
Pattern association and spreading activation:

```python
associative_memory = brain.getAssociativeMemory()
associative_memory.initialize(brain)

# Form associations
associative_memory.associate(state_a, state_b, strength=0.8)
associative_memory.associateFromExperience(episode)

# Retrieve through association
retrieved_patterns = associative_memory.retrieve(query_pattern, max_results=3)
for pattern in retrieved_patterns:
    print(f"Associated pattern with mean value: {sum(pattern)/len(pattern):.3f}")
```

**Features:**
- Bidirectional pattern associations
- Activation spreading
- Strength-based retrieval
- Experience-derived associations

## Cognition Systems

### NeuralPlanner
Predictive action planning with multi-step evaluation:

```python
planner = brain.getPlanner()
planner.initialize(brain)
planner.setPlanningDepth(5)  # Look 5 steps ahead

# Plan action sequence
current_state = get_brain_state()
planned_sequence = planner.planAction(current_state, max_depth=5)
quality = planner.evaluateActionSequence(planned_sequence)
print(f"Planned sequence quality: {quality:.3f}")
```

**Capabilities:**
- Multi-step action prediction
- Consequence evaluation
- Goal-directed planning
- Dynamic replanning

### ConceptFormation
Pattern discovery and categorization without labels:

```python
concept_formation = brain.getConceptFormation()
concept_formation.initialize(brain)

# Learn from patterns
concept_formation.processPattern(pattern_a, is_positive=True)
concept_formation.processPattern(pattern_b, is_positive=False)

# Get learned concepts
concepts = concept_formation.getConcepts()
for concept_id in concepts:
    stability = concept_formation.getConceptStability(concept_id)
    print(f"Concept {concept_id} stability: {stability:.3f}")
```

**Features:**
- Unsupervised learning
- Stability measurement
- Dynamic updating
- Pattern abstraction

## Prediction Systems

### PredictionSystem
Temporal sequence prediction and error computation:

```python
prediction_system = brain.getPredictionSystem()
prediction_system.initialize(brain)

# Get current prediction
current_state = get_sensory_state()
prediction = prediction_system.predictNextState(current_state)

# Get prediction error
error = prediction_system.getPredictionError()
print(f"Prediction error: {error:.3f}")

# Update with actual outcome
prediction_system.updatePrediction(actual_next_state)

# Get confidence for future steps
for step in range(1, 6):
    confidence = prediction_system.getConfidence(step)
    print(f"Step {step} confidence: {confidence:.3f}")
```

**Features:**
- Multi-step ahead prediction
- Error-based learning
- Confidence scoring
- History tracking

### NeuralPrediction
Neural mechanism for prediction with configurable predictors:

```python
neural_prediction = brain.getNeuralPrediction()
neural_prediction.initialize(brain)

# Set custom prediction function
def my_predictor(state):
    # Custom prediction logic
    return predict_next_state(state)

neural_prediction.setPredictor(my_predictor)

# Use for prediction
next_state = neural_prediction.predict(current_state)
neural_prediction.update(next_state)
```

## Neuromodulation Systems

### Dopamine
Reward signaling and reinforcement learning:

```python
dopamine = brain.getDopamine()
dopamine.initialize(brain)

# Apply reward-based learning
agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Access current level
current_level = dopamine.getLevel()
print(f"Dopamine level: {current_level:.3f}")

# Modify level
dopamine.setLevel(0.3)

# Get plasticity factor
plasticity_factor = dopamine.getPlasticityFactor()
print(f"Plasticity factor: {plasticity_factor:.3f}")
```

### Curiosity
Exploration motivation based on novelty and prediction error:

```python
curiosity = brain.getCuriosity()
curiosity.initialize(brain)

current_level = curiosity.getLevel()
print(f"Curiosity level: {current_level:.3f}")

curiosity.setLevel(0.7)
```

### Novelty
Novelty detection from sensory changes:

```python
novelty = brain.getNovelty()
novelty.initialize(brain)

# Novelty level updates automatically based on sensory changes
level = novelty.getLevel()
```

### PredictionError
Learning error signals for prediction updates:

```python
pred_error = brain.getPredictionErrorSignal()
pred_error.initialize(brain)

# Error level updates during learning
error_level = pred_error.getLevel()
pred_error.setLevel(-0.2)  # Negative error = worse than expected
```

## Development System

### DevelopmentSystem
Brain maturation and stage transitions:

```python
development = brain.getDevelopmentSystem()
development.initialize(brain)

# Current developmental stage
current_stage = development.getCurrentStage()
print(f"Current stage: {current_stage}")

# Update development with time
brain.step(step, current_time)
development.update(brain, rng, timestep)

# Stage-specific properties
synaptogenesis_rate = development.getSynaptogenesisRate()
pruning_rate = development.getPruningRate()
maturation_level = development.getMaturationLevel()
```

**Stages:**
- **Initial**: High plasticity, rapid learning
- **CriticalPeriod**: Focused development
- **Maturation**: Stabilizing capabilities
- **Adult**: Refined, stable performance
- **Aging**: Gradual plasticity reduction

## Performance Infrastructure

### MemoryPool
Efficient lock-free memory allocation:

```python
from pynlm import MemoryPool

# Create specialized pools for different object types
neuron_pool = MemoryPool("Neuron", block_size=1024, initial_blocks=4)
synapse_pool = MemoryPool("Synapse", block_size=2048, initial_blocks=2)

# Allocate and deallocate
neuron = neuron_pool.allocate()
# ... use neuron ...
neuron_pool.deallocate(neuron)

# Check status
print(f"Allocated: {neuron_pool.allocatedCount()}")
print(f"Available: {neuron_pool.availableCount()}")
```

**Features:**
- Lock-free allocation/deallocation
- Pre-allocated memory blocks
- Cache-line aligned for performance
- Thread-safe operations

### OptimizedBrain
High-performance simulation with advanced optimizations:

```python
# The OptimizedBrain is automatically used when available
optimized_brain = brain  # Cast if needed

# Enable optimizations
optimized_brain.enableEventDriven(True)
optimized_brain.enableMultithreading(True)
optimized_brain.enableSIMD(True)
optimized_brain.enableSparseConnectivity(True)

# Get performance statistics
stats = optimized_brain.getPerformanceStats()
print(f"Neurons per second: {stats.neuronsPerSecond:.0f}")
print(f"Firing neurons: {stats.firingNeurons}")
print(f"Memory usage: {stats.memoryUsageMB:.1f} MB")
```

**Optimizations:**
- **Event-Driven**: Only active neurons consume computation
- **SIMD Vectorization**: Parallel neural state updates
- **Sparse Connectivity**: Efficient synapse representation
- **Multithreading**: Parallel population processing
- **Structure-of-Arrays**: Better cache utilization

## Configuration Management

### Config
Centralized configuration system:

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Load from file
config.loadFromFile("my_config.json")

# Load from command line
# config.loadFromArgs(len(sys.argv), sys.argv)

# Get configuration values
neuron_count = config.getOr("neuron_count", 1000)
simulation_time = config.getOr("simulation_timestep", 0.001)

# Save configuration
config.saveToFile("output_config.json")

# Configuration summary
print(config.summary())
```

**Configuration Categories:**
- **Simulation**: timestep, neuron count, region count
- **Plasticity**: STDP parameters, learning rates
- **Development**: Synaptogenesis and pruning rates
- **Neuromodulation**: Baseline levels and thresholds
- **Environment**: World dimensions and physics
- **Logging**: Log levels and output destinations

## Experimentation and Testing

### LearningExperiment
Convenient wrapper for learning experiments:

```python
from pynlm import createLearningExperiment

brain = pynlm.createBrain(config)
experiment = createLearningExperiment(brain, seed=42)

# Record initial state
experiment.recordInitialState()

# Run learning simulation
for step in range(1000):
    brain.step(step)

# Record final state and compute statistics
experiment.recordFinalState()
experiment.computeStatistics()
```

### Test Functions
Built-in test functions for system validation:

```python
# Basic connectivity test
runBasicConnectivityTest(brain)

# Plasticity learning experiment
runPlasticityExperiment(brain)

# STDP verification test
runStdpVerification(brain)
```

## Advanced Usage

### Custom Predictors
```python
def custom_predictor(state):
    """Custom prediction logic for NeuralPrediction"""
    # Implement your own prediction algorithm
    return predicted_next_state

neural_prediction = brain.getNeuralPrediction()
neural_prediction.setPredictor(custom_predictor)
```

### Memory Management
```python
# Clear and reset systems
brain.getWorkingMemory().clear()
brain.getEpisodicMemory().clear()
brain.getAssociativeMemory().clear()

# Reset entire brain for new episode
brain.reset()
```

### Error Handling
```python
try:
    # Your code here
    world.update(0.1)
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
except pynlm.OutOfRange as e:
    print(f"Out of range error: {e}")
```

## Performance Tips

1. **Enable Optimizations**: `optimized_brain.enableEventDriven(True)`
2. **Adjust Thread Count**: `optimized_brain.setThreadCount(4)`
3. **Monitor Performance**: Use `getPerformanceStats()` regularly
4. **Configure Memory Pools**: Pre-allocate based on expected load
5. **Use Sparse Connectivity**: For large-scale networks

## Migration from C++

### Equivalent Operations

| C++ Operation | Python Equivalent |
|---------------|------------------|
| `brain.step(step)` | `brain.step(step)` |
| `brain.getTotalNeuronCount()` | `brain.getTotalNeuronCount()` |
| `brain.getTotalSynapseCount()` | `brain.getTotalSynapseCount()` |
| `brain.getWorkingMemory()` | `brain.getWorkingMemory()` |
| `agent.enableRewardModulation(True)` | `agent.enableRewardModulation(True)` |

### Data Types

| C++ Type | Python Type |
|---------|-------------|
| `NeuronId` | `int` |
| `RegionId` | `int` |
| `SimulationStep` | `int` |
| `Timestamp` | `float` |
| `MembranePotential` | `float` |
| `ActionType` | `str` or `int` |

## Examples

### Simple Agent in World
```python
import pynlm

# Create brain and environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(5, 5)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    # World physics
    world.update(0.1)
    
    # Sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Motor action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning from experience
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0)

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

### Memory System Demonstration
```python
import pynlm

# Create brain with memory systems
brain = pynlm.createBrain()
brain.initialize()

# Demonstrate episodic memory
episodic_memory = brain.getEpisodicMemory()

# Create and store experience
episode = EpisodicMemoryItem(
    timestamp=100,
    reward=1.0,
    action=pynlm.ActionType.Interact,
    sensory_state=[0.1, 0.5, 0.9, 0.2],
    resulting_sensory_state=[0.2, 0.6, 1.0, 0.3],
    active_neurons=[101, 102, 103],
    neuron_activations=[0.8, 0.6, 0.9]
)

episodic_memory.storeEpisode(episode)

# Retrieve similar experiences
retrieved = episodic_memory.retrieveSimilar(
    [0.15, 0.55, 0.95, 0.25],  # Similar pattern
    maxResults=3
)

print(f"Found {len(retrieved)} similar episodes")
for i, ep in enumerate(retrieved):
    print(f"  Episode {i}: timestamp={ep.timestamp}, reward={ep.reward}")
```

## Error Codes and Exceptions

The Python API raises the following exceptions:

- **RuntimeError**: General runtime errors
- **InvalidArgument**: Invalid function parameters
- **OutOfRange**: Index or value out of valid range
- **MemoryError**: Memory allocation failures

```python
try:
    # Your code
    result = some_function(param)
except pynlm.RuntimeError as e:
    print(f"Error: {e}")
except pynlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
```

## Type Conversion

### Automatic Conversions
- Python lists → C++ vectors
- Python floats → C++ MembranePotential
- Python ints → C++ SimulationStep, NeuronId, etc.

### Custom Conversions
```python
# For custom classes, you can register converters
pybind11::detail::any_converter<YourClass>::register()

# Or use py::arg for explicit conversion
class MyClass:
    def __init__(self, value):
        self.value = value

py::class_<MyClass>(m, "MyClass")
    .def(py::init<int>(), py::arg("value"))
    .def("getValue", &MyClass::getValue);
```

## Performance Considerations

### Memory Management
- Use memory pools for frequent allocations
- Reuse objects when possible
- Monitor memory usage with `getPerformanceStats()`

### Threading
- Enable multithreading for large networks
- Consider thread count based on CPU cores
- Be aware of race conditions with shared data

### SIMD and Vectorization
- Automatically enabled when available
- Can be controlled with `enableSIMD()`
- Best for homogeneous operations

### Sparse Connectivity
- Enable for networks with low connection density
- Reduces memory usage significantly
- May have overhead for dense networks

## Future Enhancements

### Planned Features
- **Distributed Computing**: Scale across multiple machines
- **GPU Acceleration**: Offload computation to GPU
- **Hardware Integration**: Direct sensor/actuator support
- **Cloud Deployment**: Serverless simulation environments
- **Visualization API**: Real-time rendering and analysis

### Contribution Guidelines
1. Fork the repository
2. Implement new features
3. Add comprehensive tests
4. Update documentation
5. Submit pull requests

## License

This software is licensed under the MIT License. See LICENSE file for details.

## Acknowledgements

This framework builds on decades of neuroscience research and implements concepts from:

- **Neuronal Dynamics**: LIF models, spike timing-dependent plasticity
- **Cognitive Architecture**: Memory, attention, planning
- **Developmental Psychology**: Stage-based learning and maturation
- **Systems Neuroscience**: Embodied, embedded cognition

The implementation follows the NLM philosophy of creating artificial developmental brains that learn from experience rather than using pre-trained models.

---

*This documentation covers the complete NLM Python API as of Phase 6 Final Integration.*

For more information or to contribute, visit the NLM repository at GitHub.