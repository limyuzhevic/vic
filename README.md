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

## Python API Advanced Usage

This section provides advanced examples for using the NLM Python API effectively.

### 1. Creating and Configuring a Brain

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)

# Initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")
```

### 2. Simulation with World Interaction

```python
import pynlm

# Create complete agent setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=32, height=32, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all subsystems for advanced behavior
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with detailed logging
for step in range(1000):
    world.update(timestep=0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

### 3. Brain State Management

```python
import pynlm
import os

# Save brain state for checkpointing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Save checkpoint
checkpoint_file = "brain_checkpoint.bin"
brain.save(checkpoint_file)
print(f"Brain saved to {checkpoint_file}")

# Later, load from checkpoint
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.load(checkpoint_file)
print(f"Loaded brain with {brain2.getTotalNeuronCount()} neurons")
```

### 4. Configuration Management

```python
import pynlm
import json

# Create config and save to JSON
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromodulation.curiosity.enable", True)

# Save config to file
config.saveToFile("my_config.json")

# Load config from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Get config as Python dictionary
config_dict = config.getKeys()
for key in config_dict:
    value = config.get(key)
    print(f"{key}: {value}")

# Create config from JSON
json_data = {"brain.neuron_count": 2000, "plasticity.hebbian.enable": True}
config3 = pynlm.createConfigFromJson(json_data)
```

### 5. Advanced Simulation Control

```python
import pynlm

# Create sophisticated simulation setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all subsystems
brain.enableSTDP(True)
brain.enableHebbian(True)
brain.enableStructuralPlasticity(True)
brain.enableDevelopment(True)

# Performance benchmarking
print("Running performance benchmark...")
import time
start_time = time.time()

for step in range(500):
    brain.step(step)

end_time = time.time()
print(f"Benchmark completed in {end_time - start_time:.2f} seconds")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Get detailed statistics
stats = pynlm.getBrainStats(brain)
for key, value in stats.items():
    print(f"{key}: {value}")
```

### 6. Memory and Performance Monitoring

```python
import pynlm
import psutil
import os

def monitor_memory_usage():
    """Monitor memory usage of the simulation"""
    process = psutil.Process(os.getpid())
    mem_info = process.memory_info()
    return mem_info.rss / 1024 / 1024  # Return in MB

# Create brain and monitor memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

initial_memory = monitor_memory_usage()
print(f"Initial memory usage: {initial_memory:.2f} MB")

# Run simulation
for step in range(200):
    brain.step(step)
    if step % 50 == 0:
        current_memory = monitor_memory_usage()
        print(f"Step {step}: Memory usage: {current_memory:.2f} MB")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")

final_memory = monitor_memory_usage()
print(f"Final memory usage: {final_memory:.2f} MB")
print(f"Memory increase: {final_memory - initial_memory:.2f} MB")
```

### 7. Custom Neuron Types and Regions

```python
import pynlm

# Create brain with specific regions
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("region_count", 5)  # Create 5 different brain regions

brain = pynlm.createBrain(config)
brain.initialize()

# Create specialized regions
region_names = ["cortex", "hippocampus", "amygdala", "thalamus", "hypothalamus"]
region_ids = []

for i, name in enumerate(region_names):
    region_id = brain.addRegion(name)
    region_ids.append(region_id)
    print(f"Created region {name} with ID: {region_id}")

# Get region statistics for all regions
for region_id in region_ids:
    stats = pynlm.getRegionStats(brain, region_id)
    print(f"Region {region_id}:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
```

### 8. Experiment Runner with Results

```python
import pynlm
import json
from datetime import datetime

def run_experiment(experiment_name, config_overrides, steps=1000):
    """Run a complete experiment and return results"""
    print(f"Starting experiment: {experiment_name}")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    for key, value in config_overrides.items():
        config.set(key, value)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    agent.initialize(world)
    
    # Configure subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run experiment
    rewards = []
    firing_rates = []
    
    for step in range(steps):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        rewards.append(reward)
        firing_rates.append(brain.getAverageFiringRate())
        
        if step % 200 == 0:
            print(f"Progress: {step}/{steps} steps")
    
    # Collect results
    results = {
        "experiment_name": experiment_name,
        "timestamp": datetime.now().isoformat(),
        "config": config_overrides,
        "total_steps": steps,
        "final_reward": sum(rewards) / len(rewards),
        "max_reward": max(rewards),
        "min_reward": min(rewards),
        "avg_firing_rate": sum(firing_rates) / len(firing_rates),
        "final_developmental_stage": brain.getDevelopmentalStage(),
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "time_per_step_ms": 1000.0 / len(firing_rates) if len(firing_rates) > 0 else 0
    }
    
    # Save results
    with open(f"experiment_{experiment_name}.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"Experiment completed. Results saved to experiment_{experiment_name}.json")
    return results

# Example experiment
experiment_results = run_experiment(
    experiment_name="learning_behavior",
    config_overrides={
        "brain.neuron_count": 1000,
        "plasticity.stdp.enable": True,
        "neuromodulation.curiosity.enable": True,
        "development.enable": True
    },
    steps=500
)
```

## Performance Tips

### 1. Memory Optimization
```python
# Use smaller configurations for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start small
config.set("region_count", 1)

# Gradually increase complexity
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Batch Processing
```python
# Process multiple steps efficiently
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Process 1000 steps at once
for step in range(1000):
    brain.step(step)
```

### 3. Checkpointing Strategy
```python
# Use checkpoints for long-running simulations
brain.save("checkpoint_100.bin")  # Save at milestones
brain.load("checkpoint_100.bin")  # Resume from checkpoint
```

## Troubleshooting

### Common Issues and Solutions

1. **Segmentation Faults**
   - Ensure `brain.initialize()` is called before `brain.step()`
   - Check for null pointers in Python code
   - Use smaller configurations for debugging

2. **Memory Issues**
   - Monitor memory usage during long simulations
   - Use checkpointing to save intermediate states
   - Consider increasing swap space if needed

3. **Performance Problems**
   - Profile with `pynlm.benchmarkBrain()`
   - Reduce neuron count for testing
   - Check for inefficient code paths

### Debugging Commands

```python
import pynlm

# Enable debug logging
brain.logStatus()  # Print detailed brain status

# Get simulation statistics
brain.getTotalSpikeCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()

# Check configuration
config.has("key.name")
config.get("key.name")
```

## Migration Guide

### From Phase 4 to Phase 6

The Phase 6 API maintains backward compatibility while adding advanced features:

```python
# Phase 4 style (still works)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Phase 6 style with advanced features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# New advanced features
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)
agent.enableCuriosity(True)
```

## Advanced Python Integration

### 1. Using NumPy for Data Processing

```python
import pynlm
import numpy as np

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and collect data
firing_rates = []
for step in range(100):
    brain.step(step)
    firing_rates.append(brain.getAverageFiringRate())

# Process with NumPy
firing_rates_array = np.array(firing_rates)
print(f"Mean firing rate: {firing_rates_array.mean():.3f}")
print(f"Firing rate std: {firing_rates_array.std():.3f}")
print(f"Max firing rate: {firing_rates_array.max():.3f}")

# Statistical analysis
percentiles = np.percentile(firing_rates_array, [25, 50, 75])
print(f"Percentiles: {percentiles}")
```

### 2. Visualization Integration

```python
import pynlm
import matplotlib.pyplot as plt

# Create visualization data
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect time series data
steps = 200
time_series = []
for step in range(steps):
    brain.step(step)
    time_series.append({
        'step': step,
        'firing_rate': brain.getAverageFiringRate(),
        'spike_count': brain.getTotalSpikeCount(),
        'active_neurons': brain.getActiveNeuronCount()
    })

# Create plots
steps_list = [t['step'] for t in time_series]
firing_rates = [t['firing_rate'] for t in time_series]

plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(steps_list, firing_rates)
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate')

plt.subplot(2, 2, 2)
plt.hist(firing_rates, bins=20, alpha=0.7)
plt.title('Firing Rate Distribution')
plt.xlabel('Firing Rate')
plt.ylabel('Frequency')

plt.subplot(2, 2, 3)
spike_counts = [t['spike_count'] for t in time_series]
plt.plot(steps_list, spike_counts)
plt.title('Cumulative Spike Count')
plt.xlabel('Step')
plt.ylabel('Total Spikes')

plt.tight_layout()
plt.show()
```

### 3. Parallel Processing

```python
import pynlm
from multiprocessing import Pool
import time

def run_simulation_batch(config_dict, steps):
    """Run a simulation in a separate process"""
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'steps': steps
    }

# Run multiple simulations in parallel
if __name__ == '__main__':
    configs = [
        {"brain.neuron_count": 100, "plasticity.stdp.enable": True},
        {"brain.neuron_count": 200, "plasticity.stdp.enable": True},
        {"brain.neuron_count": 300, "plasticity.stdp.enable": True},
        {"brain.neuron_count": 400, "plasticity.stdp.enable": True},
        {"brain.neuron_count": 500, "plasticity.stdp.enable": True},
    ]
    
    start_time = time.time()
    
    with Pool(processes=4) as pool:
        results = pool.starmap(run_simulation_batch, 
                              [(config, 100) for config in configs])
    
    end_time = time.time()
    
    print(f"Parallel simulations completed in {end_time - start_time:.2f} seconds")
    for i, result in enumerate(results):
        print(f"Config {i+1}: {result}")
```

## Next Steps

1. **Explore the Documentation**: Read the ARCHITECTURE.md, SCIENCE.md, and EXPERIMENTS.md files for detailed scientific background
2. **Run Phase 6 Demos**: Use the `nlm_phase6_demo` executable to see integrated systems in action
3. **Contribute**: Add your own experiments and publish them as new examples
4. **Feedback**: Report issues and suggest improvements on the GitHub repository

## Quick Reference

### Core Classes
- `pynlm.createDefaultConfig()` - Create configuration
- `pynlm.createBrain(config)` - Create brain
- `pynlm.createAgentBrain(brain)` - Create agent interface
- `pynlm.createSimpleWorld()` - Create world
- `pynlm.createAction(type, parameters)` - Create action

### Key Methods
- `initialize()` - Initialize brain
- `step(step_number)` - Run simulation step
- `save(filepath)` - Save checkpoint
- `load(filepath)` - Load checkpoint
- `processSensoryInput(percept)` - Process input
- `decodeMotorCommand()` - Get action

### Configuration Keys
- `brain.neuron_count` - Number of neurons
- `brain.synapse_density` - Connection density
- `plasticity.stdp.enable` - Enable STDP
- `neuromodulation.curiosity.enable` - Enable curiosity
- `development.enable` - Enable development

### Common Patterns
1. **Agent-Wall Interaction**: Use `AgentBrain` to connect brain to world
2. **Checkpointing**: Save and load brain state for long simulations
3. **Performance Monitoring**: Use `benchmarkBrain()` for timing analysis
4. **Configuration Management**: Use JSON files for experiment parameters

This comprehensive guide should help you get the most out of the NLM Phase 6 Python API. The examples cover everything from basic usage to advanced research applications. Feel free to explore the code and experiment with different configurations to discover new possibilities!

## License

MIT

## Authors

Research project — See docs for scientific background.
