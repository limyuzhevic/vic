# Comprehensive NLM Documentation Suite

This documentation provides complete guidance for using the NLM (Neural Learning Machine) system:

- [Quick Start Guide](QUICK_START.md) - Get up and running in 5 minutes
- [Python API Reference](PYTHON_API.md) - Complete Python bindings documentation
- [C++ Architecture](CPP_ARCHITECTURE.md) - Deep dive into the C++ implementation
- [Advanced Usage](ADVANCED_USAGE.md) - For power users and researchers
- [Performance Guide](PERFORMANCE.md) - Optimizing NLM for speed and scale
- [Troubleshooting](TROUBLESHOOTING.md) - Common issues and solutions
- [Future Roadmap](ROADMAP.md) - Planned features and enhancements

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a brain-inspired neural system capable of developing and adapting through interaction with an environment.

### Key Features

- **Brain-like Architecture**: Implements spiking neural computation with LIF neurons
- **Integrated Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and prediction error
- **Development**: Adaptive neural development with structural plasticity
- **Cognition**: Prediction systems, neural planners, and concept formation
- **Full Integration**: All Phase 6 systems work together cohesively

## Quick Start Guide

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 simulation steps
for step in range(100):
    brain.step(step)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
```

### Example 2: Complete Agent with World Interaction

```python
import pynlm

# Setup the complete system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run the agent
for step in range(500):
    world.update(0.1)  # Update world physics
    percept = world.getSensoryPercept()  # What agent sees
    agent.processSensoryInput(percept)  # Brain processes
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
```

## Python API Reference

### Core Classes

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step_number)
brain.receiveSensoryInput(input_data)
action = brain.produceAction()
brain.save(filepath)
brain.load(filepath)
```

#### Config
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("random_seed", 42)
value = config.getOr("parameter", default_value)
```

#### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(100, 100, 16, 16)  # width, height, visionWidth, visionHeight
world.update(timestep)
percept = world.getSensoryPercept()
world.applyMotorCommand(action, time)
```

### Agent Features

#### Enable/Disable Features
```python
# Learning capabilities
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

#### Neuromodulation Levels
```python
# Get current neuromodulation levels
dopamine = agent.getNeuromodulationLevel()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()

print(f"Dopamine: {dopamine:.3f}")
print(f"Curiosity: {curiosity:.3f}")
print(f"Novelty: {novelty:.3f}")
print(f"Prediction Error: {prediction_error:.3f}")
```

#### Development Stages
```python
stage = agent.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")

# Update development
agent.updateDevelopment(timestep)
```

## Advanced Usage Patterns

### Running Experiments

```python
def run_learning_experiment():
    # Create agent with enhanced settings
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("random_seed", 12345)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run with detailed logging
    for step in range(1000):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action selection with learning
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with prediction error
        reward = calculate_reward(world, action)
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log key metrics every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    return agent
```

### Batch Processing Multiple Brains

```python
def batch_experiment(config_template, num_runs, steps_per_run):
    results = []
    
    for run in range(num_runs):
        print(f"Running batch iteration {run + 1}/{num_runs}")
        
        # Create unique config for this run
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", config_template.neuron_count)
        config.set("random_seed", run + 1000)  # Different seed each run
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_run):
            brain.step(step)
        
        # Record results
        result = {
            'run': run,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'neuron_count': brain.getTotalNeuronCount()
        }
        results.append(result)
        
        # Save checkpoint every 10 runs
        if (run + 1) % 10 == 0:
            brain.save(f"checkpoints/batch_run_{run + 1}.bin")
    
    return results
```

### Visualization and Monitoring

```python
def monitor_brain_activity(brain, output_file=None):
    """Monitor and log brain activity over time"""
    import time
    
    monitor_data = {
        'timestamps': [],
        'spike_counts': [],
        'firing_rates': [],
        'active_neuron_counts': [],
        'e_i_ratios': [],
        'steps': []
    }
    
    start_time = time.time()
    
    for step in range(1000):
        brain.step(step)
        
        # Record metrics
        monitor_data['timestamps'].append(time.time() - start_time)
        monitor_data['spike_counts'].append(brain.getTotalSpikeCount())
        monitor_data['firing_rates'].append(brain.getAverageFiringRate())
        monitor_data['active_neuron_counts'].append(brain.getActiveNeuronCount())
        monitor_data['e_i_ratios'].append(brain.getExcitationInhibitionRatio())
        monitor_data['steps'].append(step)
        
        # Log every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Spikes={monitor_data['spike_counts'][-1]}, "
                  f"Firing Rate={monitor_data['firing_rates'][-1]:.2f}")
    
    # Save monitoring data
    if output_file:
        import json
        with open(output_file, 'w') as f:
            json.dump(monitor_data, f, indent=2)
        print(f"Monitoring data saved to {output_file}")
    
    return monitor_data
```

## C++ Architecture Deep Dive

NLM's architecture is organized into several key components:

### Core Neural Components

#### Brain.cpp - Main Brain Controller
- Manages all neural systems integration
- Handles memory systems, prediction, cognition
- Manages neuromodulation and development
- Provides checkpointing and persistence

#### Neuron.cpp - Individual Neuron Implementation
- Leaky Integrate-and-Fire (LIF) dynamics
- Membrane potential integration and spike generation
- Synaptic input processing
- Refractory period management

#### Synapse.cpp - Synaptic Connections
- Event-driven spike propagation with delays
- Short-term plasticity (STP) mechanisms
- Eligibility traces for reward-modulated learning
- STDP (Spike-Timing-Dependent Plasticity)

### System Integration Components

#### AgentBrain.cpp - Brain-World Interface
- Sensory transduction from world to brain
- Motor command decoding from brain to world
- Neuromodulation signal processing
- Development state management

#### Development.cpp - Neural Development
- Synaptogenesis (formation of new connections)
- Pruning (elimination of weak connections)
- Maturation (structural and functional maturation)
- Critical period management

#### Neuromodulation.cpp - Learning Signals
- Dopamine reward prediction error
- Curiosity-driven exploration
- Novelty detection
- Prediction error signaling

### Memory Systems

#### Working Memory - NeuralWorkingMemory.cpp
- Short-term active information maintenance
- Competitive selection of representations
- Capacity management

#### Episodic Memory - NeuralEpisodicMemory.cpp
- Experience storage and replay
- Memory consolidation
- Sleep/rest cycle integration

## Performance Guide

### Optimization Strategies

#### 1. Memory Pooling
```cpp
// Use memory pools for frequent allocations
#include "src/performance/MemoryPool.hpp"

// Create specialized pools for different data types
MemoryPool<NeuronState> neuronPool(1000);
MemoryPool<SynapseHandle> synapsePool(5000);
```

#### 2. Event-Driven Processing
```cpp
// Use event queues for efficient spike propagation
#include "src/performance/EventQueue.hpp"

EventQueue<SpikeEvent> spikeQueue;
EventQueue<DelayedSpikeEvent> delayedQueue;
```

#### 3. SIMD Vectorization
```cpp
// Utilize SIMD for parallel neuron operations
#include "src/performance/SIMDVectorization.hpp"

SIMDVector<float> potentials;
potentials = SIMDVector<float>::load_from_array(neuron_potentials);
```

#### 4. Sparse Connectivity
```cpp
// Use sparse matrices for connection storage
#include "src/performance/SparseConnectivity.hpp"

SparseConnectivity<float> connectivity;
connectivity.addConnection(source, target, weight);
```

### Profiling Recommendations

```bash
# Run with performance profiling
./nlm_phase6_demo --profile --output=performance.prof

# Memory leak detection
valgrind --leak-check=full ./nlm_phase6_demo

# CPU profiling
perf record -g ./nlm_phase6_demo
perf report
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain isn't doing anything" Error

**Problem**: Neurons aren't firing or showing activity.

**Solutions**:
1. **Check initialization**: Ensure `brain.initialize()` was called
2. **Verify configuration**: Check neuron count, connection probability
3. **Inject current**: Manually inject current into sensory neurons
4. **Check thresholds**: Verify neuron thresholds are set correctly

**Debug Script**:
```python
import pynlm

def debug_brain(brain):
    # Check basic stats
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Total Spikes: {brain.getTotalSpikeCount()}")
    
    # Try injecting current
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: Firing={brain.getFiringNeuronCount()}")
    
    # Check neuron states
    for i, region in enumerate(brain.getRegions()):
        print(f"Region {i} ({region.getName()}):")
        for pop in region.getPopulations():
            print(f"  Population {pop.getId()}: {pop.getNeuronCount()} neurons")
```

#### "The agent isn't moving" Error

**Problem**: Motor neurons aren't producing actions.

**Solutions**:
1. **Check motor neuron groups**: Verify motorForward_, motorTurnLeft_, etc.
2. **Check action selection**: Verify `decodeMotorCommand()` is being called
3. **Check neural activity**: Verify motor neurons have sufficient activity
4. **Check world integration**: Ensure agent's world reference is set

**Debug Script**:
```python
def debug_agent(agent, world):
    brain = agent.getBrain()
    
    # Check motor neuron groups
    print("Motor neuron groups:")
    print(f"  Forward: {len(agent.motorForward_)}")
    print(f"  Backward: {len(agent.motorBackward_)}")
    print(f"  Turn Left: {len(agent.motorTurnLeft_)}")
    print(f"  Turn Right: {len(agent.motorTurnRight_)}")
    print(f"  Interact: {len(agent.motorInteract_)}")
    print(f"  Wait: {len(agent.motorWait_)}")
    
    # Check brain motor neurons
    for region in brain.getRegions():
        for pop in region.getPopulations():
            if pop.getNeuronType() == pynlm.NeuronType.Motor:
                print(f"  Brain motor population {pop.getId()}: {pop.getNeuronCount()} neurons")
    
    # Try a manual motor command
    print("\nTesting manual action selection...")
    action = agent.decodeMotorCommand()
    print(f"Selected action: {action}")
    
    if world:
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f"World agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

#### Memory Issues

**Problem**: High memory usage or memory leaks.

**Solutions**:
1. **Use memory pools**: Implement `src/performance/MemoryPool.hpp`
2. **Monitor allocations**: Use `valgrind` or similar tools
3. **Clear unused data**: Explicitly clear containers when no longer needed
4. **Use smart pointers**: Prefer `std::unique_ptr` over raw pointers

### Getting Help

1. **Check documentation**: Review the docs/ folder for detailed guides
2. **Run demos**: `./nlm_phase6_demo` shows integration capabilities
3. **Read source code**: Comments in the code often explain the "why"
4. **GitHub Issues**: Report bugs and feature requests

## Future Roadmap

### Phase 8: Advanced Learning
- **Meta-learning**: System can learn how to learn
- **Transfer learning**: Knowledge transfer between tasks
- **Meta-cognition**: System can think about its own thinking

### Phase 9: Multi-Modal Integration
- **Multi-sensory input**: Beyond vision and motor
- **Language processing**: Text comprehension and generation
- **Abstract reasoning**: Logical and mathematical thinking

### Phase 10: Conscious Experience
- **Subjectivity**: First-person perspective modeling
- **Emotional states**: Affect-based decision making
- **Self-awareness**: Recognition of self vs. environment

### Technical Enhancements
- **Hardware acceleration**: GPU support for neural computation
- **Distributed computing**: Scale to massive brain models
- **Real-time interaction**: Millisecond-level responsiveness

## Conclusion

NLM represents a significant step toward creating brain-like computational systems. While Phase 6 achieves remarkable integration of all major systems, the journey toward true artificial general intelligence continues.

The key achievement of NLM is demonstrating that:

1. **Integration is possible**: All major brain systems can work together
2. **Development matters**: Neural development leads to more capable systems
3. **Learning is organic**: Natural learning rules emerge from neural dynamics
4. **Emergence works**: Complex cognition emerges from simple components

NLM is not a final product, but a research platform for exploring how brain-like systems can think, learn, and develop.
