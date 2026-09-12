# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Python Integration

NLM now has comprehensive Python bindings that make it easier to use and experiment with the framework.

### Installation

```bash
# Install the NLM Python package
pip install nlm

# For development, build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e ../python
```

### Quick Start Guide

```python
import nlm

# Create a default configuration
config = nlm.createDefaultConfig()

# Create a brain with the configuration
brain = nlm.createBrain(config)

# Create a simple world
world = nlm.createSimpleWorld()

# Initialize the brain with the world
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run a basic simulation
for step in range(100):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    action = agent_brain.decodeMotorCommand()
    
    # Apply action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update world
    world.update(0.001)

print("✅ Simulation completed successfully!")
```

### Core Python Classes

NLM Python bindings provide access to all core brain components:

#### Neural Units
- **Neuron**: Individual neural units with membrane potential and firing dynamics
- **Synapse**: Connections between neurons with weight, delay, and type information
- **Region**: Brain regions containing populations and local connectivity
- **Population**: Groups of neurons with shared properties

#### Memory Systems
- **WorkingMemory**: Transient storage using persistent activity and recurrent connections
- **EpisodicMemory**: Experience storage with replay capabilities
- **AssociativeMemory**: Pattern association and spreading activation

#### Sensory Input
- **Vision**: Visual sensory input with validation and error checking
- **Audio**: Audio sensory input with sample rate and duration validation
- **InternalSignals**: Internal neuromodulatory and state signals

#### Motor Systems
- **Action**: Motor commands with type and parameter support
- **AgentBrain**: Interface connecting NLM to the world
- **AgentBody**: Physical agent state (position, orientation, energy, health)

### Advanced Python Features

#### Neural Network Visualization
```python
import nlm

brain = nlm.createBrain(nlm.createDefaultConfig())

# Get all regions and populations
for region in brain.getRegions():
    for population in region.getAllPopulations():
        # Visualize population activity
        activity = population.getActivityLevel()
        neurons = population.getNeurons()
        
        print(f"Population {population.getId()}: {activity:.2%} active")
        print(f"  Neuron types: {[n.getType() for n in neurons[:5]]}...")
```

#### Simulation Result Analysis
```python
import nlm
import matplotlib.pyplot as plt

brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation and collect statistics
firing_rates = []
active_neuron_counts = []
spike_counts = []

for step in range(1000):
    brain.step(step)
    
    total_firing_rate = brain.getAverageFiringRate()
    firing_rates.append(total_firing_rate)
    
    active_count = brain.getActiveNeuronCount()
    active_neuron_counts.append(active_count)
    
    spike_count = brain.getTotalSpikeCount()
    spike_counts.append(spike_count)

# Plot results
plt.figure(figsize=(12, 4))

plt.subplot(131)
plt.plot(firing_rates)
plt.title("Average Firing Rate")
plt.ylabel("Hz")

plt.subplot(132)
plt.plot(active_neuron_counts)
plt.title("Active Neuron Count")
plt.ylabel("Count")

plt.subplot(133)
plt.plot(spike_counts)
plt.title("Total Spike Count")
plt.ylabel("Count")

plt.tight_layout()
plt.show()
```

#### Batch Processing
```python
import nlm
import numpy as np

brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run multiple simulations with different configurations
results = []

for config_seed in range(10):
    # Create configuration with random parameters
    config = nlm.createDefaultConfig()
    
    # Run simulation
    brain.reset()
    
    for step in range(500):
        brain.step(step)
    
    # Collect results
    results.append({
        'seed': config_seed,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'region_count': brain.getRegionCount()
    })

# Analyze results
import pandas as pd
results_df = pd.DataFrame(results)

print("Simulation Results Summary:")
print(results_df.describe())

# Identify interesting patterns
interesting_simulations = results_df[
    (results_df['final_firing_rate'] > 2.0) & 
    (results_df['total_spikes'] > 1000)
]
print(f"\nFound {len(interesting_simulations)} interesting simulations")
```

### Configuration Management

Python bindings provide enhanced configuration management:

```python
import nlm

# Create configuration
config = nlm.createDefaultConfig()

# Load from file
config.loadFromFile("my_config.json")

# Set values with validation
config.set("neural.regionCount", 20)
config.setFloat("neural.simulationTimestep", 0.0005)

# Get values with defaults
region_count = config.getInt("neural.regionCount", 10)
sim_timestep = config.getFloat("neural.simulationTimestep", 0.001)

# Validate configuration
if config.validate():
    print("✅ Configuration is valid")
else:
    print("❌ Configuration has errors")

# Save configuration
config.saveToFile("my_updated_config.json")
```

### Error Handling and Validation

Python bindings include comprehensive error checking:

```python
import nlm

brain = nlm.createBrain(nlm.createDefaultConfig())

# Error handling with try-catch
try:
    # This will raise RuntimeError if data is invalid
    brain.getWorkingMemory().store([0.1, 0.2, 0.3])
    print("✅ Memory operation successful")
except nlm.RuntimeError as e:
    print(f"❌ Error: {e}")

# Data validation
vision = nlm.Vision(84, 84)
try:
    vision.setData([0.1] * (84 * 84 * 3))  # Valid data
    vision.validateImage()
    print("✅ Vision data is valid")
except nlm.RuntimeError as e:
    print(f"❌ Invalid vision data: {e}")

# Type checking
action = nlm.Action(nlm.ActionType.MoveForward)
print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
```

### Performance Tuning Tips

1. **Memory Efficiency**
   - Use `consolidate()` on episodic memory to prevent memory growth
   - Set appropriate working memory capacities based on your use case
   - Monitor memory usage with `getActiveTraces()`

2. **Simulation Performance**
   - Use smaller timesteps for stability (0.001-0.01 seconds)
   - Batch processing of multiple simulations
   - Profile with timeit for bottlenecks

3. **Data Handling**
   - Pre-allocate numpy arrays for large sensory data
   - Use efficient data structures for memory systems
   - Consider using `pickle` for checkpoint saving

4. **Configuration Optimization**
   - Cache configuration objects for repeated use
   - Use default configurations when possible
   - Validate configurations before simulation

### Advanced Usage Examples

#### Custom Neuron Types
```python
import nlm

# Create brain and add custom neuron types
brain = nlm.createBrain(nlm.createDefaultConfig())

# Add region and modify neuron types
region = brain.getRegion(brain.addRegion("CustomRegion"))
population = region.addPopulation(50, nlm.NeuronType.Sensory)

# Customize neuron properties for each population
for i in range(population.getSize()):
    neuron = population.getNeuron(i)
    if neuron:
        neuron.setThreshold(-40.0)  # Higher threshold
        neuron.setFiringRate(2.0)   # Higher firing rate
```

#### Memory Replay for Training
```python
import nlm

brain = nlm.createBrain(nlm.createDefaultConfig())
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain)

# Store experience patterns
experiences = [
    [0.1, 0.5, 0.3, 0.8],
    [0.2, 0.6, 0.4, 0.7],
    [0.15, 0.55, 0.35, 0.75]
]

# Store in working memory with strength
for i, exp in enumerate(experiences):
    working_memory.store(exp, strength=float(i + 1))

# Run competition to select winning patterns
working_memory.runCompetition()

# Retrieve and reinforce winning patterns
for neuron_id in working_memory.getMemoryNeurons():
    working_memory.strengthenMemory(1.5)  # Increase strength
```

#### Sensorimotor Loop with Memory
```python
import nlm

brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

agent.initialize(world)

# Continuous sensorimotor loop
for episode in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through agent brain
    agent.processSensoryInput(percept)
    
    # Store in working memory
    sensory_pattern = percept.getAllSignals()
    brain.getWorkingMemory().store(sensory_pattern, strength=1.0)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update world
    world.update(0.001)
    
    # Occasionally replay important memories
    if episode % 50 == 0:
        brain.getEpisodicMemory().replaySequence([0])
```

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/                           # Python bindings
│   ├── bindings.cpp
│   ├── CMakeLists.txt
│   └── python_docs.py               # This documentation file
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

For questions about Python usage:
- Check the documentation in `python_docs.py`
- Use the examples and quick start guides
- Review the Python API reference

For issues and feedback:
- Report issues at: https://github.com/Kilo-Org/kilocode/issues

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
