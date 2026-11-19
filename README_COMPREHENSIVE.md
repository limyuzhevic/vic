# NLM (Neural Learning Machine) - Comprehensive Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Concepts

NLM simulates brain-like computation using:

- **Neurons** - Computational units that process and transmit signals
- **Synapses** - Connections between neurons that can strengthen or weaken
- **Plasticity** - Learning mechanisms that modify connections over time
- **Neuromodulation** - Chemical signals that influence learning and behavior
- **Memory** - Systems for storing and retrieving information
- **Development** - Stages of maturation affecting learning capacity

## Getting Started (The Complete Guide)

This is a comprehensive guide covering all aspects of NLM, from simple introduction to advanced usage.

### Part 1: Quick Start - Complete Examples

#### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Let it think for 10 steps
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Example 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
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
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
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
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Part 2: Core Concepts and Architecture

#### The NLM Brain Loop

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

#### Key Systems

1. **Memory Systems**
   - Working memory with persistent activity and competition
   - Episodic memory with experience encoding and replay
   - Associative memory with Hebbian pattern associations

2. **Neuromodulation**
   - Dopamine affects neural excitability and plasticity
   - Curiosity drives exploration behavior
   - Novelty detection integrated with sensory processing

3. **Prediction System**
   - Prediction system integrated into brain loop
   - Prediction error signals affect learning
   - Confidence tracking implemented

4. **Cognition Systems**
   - Neural planner with action sequence evaluation
   - Concept formation from experience patterns
   - Attention with competitive selection dynamics

5. **Development System**
   - Developmental stages affect plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

### Part 3: Python API Reference

#### Core Classes

##### Config Class

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Set configuration (auto-converts types)
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# Load from JSON string
config.loadJson('{"brain.neuron_count": 500}')

# Export to JSON
json_str = config.toJson()

# Validate configuration
missing = config.validate()
```

##### Brain Class

```python
# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Development: {brain.getDevelopmentalStage()}")

# Advanced operations
brain.stepMultiple(1000)  # Run many steps efficiently
brain.runSimulation(100, verbose=True)  # Run with progress reporting
brain.injectCurrent(42, 5.0)  # Inject current into neuron

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Access subsystems
spike_system = brain.getSpikeSystem()
stdp_system = brain.getSTDP()
```

##### AgentBrain Class

```python
# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command with details
cmd, activities = agent.decodeWithDetails()

# Apply learning
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(0.1)

# Get statistics
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Concepts: {agent.getConceptCount()}")

# Social learning
agent.observeOtherAgent(MotorCommand.MoveForward, before_vision, after_vision)

# Text encoding
text_neurons = agent.encodeText("hello world")
```

##### SimpleWorld Class

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    action = world.applyMotorCommand(cmd, world.getSimulationTime())

# World properties
print(f"Width: {world.getWidth()}, Height: {world.getHeight()}")
print(f"Time: {world.getSimulationTime()}")
print(f"Energy: {world.getAgentBody().energy}")
```

#### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Rest

# World object types
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Part 4: Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Part 5: Advanced Usage Patterns

#### Pattern 1: Training a Brain

```python
def train_brain(config, world, agent, steps, log_interval=100):
    """Train an agent over many episodes."""
    brain = agent.getBrain()
    
    for episode in range(steps):
        brain.reset()
        agent.reset()
        
        for step in range(100):
            # Run simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Log progress
        if episode % log_interval == 0:
            print(f"Episode {episode}: "
                  f"Reward: {agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Concepts: {agent.getConceptCount()}")
        
        # Occasionally save checkpoint
        if episode % 1000 == 0:
            brain.save(f"checkpoint_ep{episode}.bin")
            print(f"Saved checkpoint at episode {episode}")
    
    return brain.getLearnedPreference()
```

#### Pattern 2: Experiment with Different Configurations

```python
def experiment_configurations(base_config, variations, world, num_steps=500):
    """Compare different brain configurations."""
    results = {}
    
    for name, variation in variations.items():
        print(f"Testing configuration: {name}")
        
        # Create config from base with variation
        config = pynlm.createDefaultConfig()
        config.loadJson(base_config)  # Start with base
        config.loadJson(variation)   # Apply variation
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Run simulation
        run_simulation(world, agent, num_steps)
        
        # Record results
        results[name] = {
            'final_reward': agent.getNeuromodulationLevel(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_concepts': agent.getConceptCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'learned_preference': brain.getLearnedPreference()
        }
        
        print(f"  Results: {results[name]}")
    
    return results
```

#### Pattern 3: Continuous Learning and Adaptation

```python
def continuous_learning_loop(world, agent, num_episodes=1000):
    """Continuous learning with experience replay."""
    brain = agent.getBrain()
    episode_count = 0
    
    while episode_count < num_episodes:
        # Run episode
        brain.reset()
        agent.reset()
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
        
        episode_count += 1
        
        # Periodic checkpoint and replay
        if episode_count % 100 == 0:
            brain.save(f"checkpoint_{episode_count}.bin")
            print(f"Completed {episode_count} episodes, "
                  f"learned preference: {brain.getLearnedPreference():.3f}")
    
    return brain
```

### Part 6: Troubleshooting and Common Issues

#### "My brain isn't doing anything"

- **Symptom**: Brain appears idle, no motor actions, low neuron activity
- **Causes**:
  1. **Forgot to initialize**: Always call `brain.initialize()` before `brain.step()`
  2. **Too few steps**: Brains need time to "warm up" - try more steps
  3. **No sensory input**: Connect brain to world via agent

- **Solution**:
  ```python
  # Ensure proper initialization
  brain = pynlm.createBrain(config)
  brain.initialize()  # CRITICAL
  
  # Run enough steps for activity
  for i in range(1000):
      brain.step(i)
  ```

#### "The agent isn't moving"

- **Symptom**: Agent stays in place, never takes actions
- **Causes**:
  1. **Missing world updates**: Need to call `world.update()`
  2. **No motor decoding**: Agent might not decode motor commands properly
  3. **Inhibition too strong**: Learning might suppress all actions

- **Solution**:
  ```python
  # Ensure complete loop
  for step in range(100):
      world.update(0.1)  # CRITICAL: update world
      agent.processSensoryInput(world.getSensoryPercept())
      brain.step(step)
      motor_cmd = agent.decodeMotorCommand()  # Critical decoding
      world.applyMotorCommand(motor_cmd, world.getSimulationTime())  # CRITICAL: apply action
  ```

#### "Everything is 0 or None"

- **Symptom**: All sensor readings, neural activity, rewards are zero
- **Causes**:
  1. **Brains need warm-up time**: Initial activity is often low
  2. **Insufficient stimulation**: No sensory input or reward signals
  3. **Configuration issues**: Default settings might not create activity

- **Solution**:
  ```python
  # Allow time for activity to build
  for i in range(200):
      brain.step(i)
  
  # Add sensory stimulation
  config.set("brain.initial_weight_mean", 0.8)
  brain = pynlm.createBrain(config)
  brain.initialize()
  
  # Enable learning to create dynamics
  agent.enableRewardModulation(True)
  agent.enableCuriosity(True)
  ```

#### "ImportError: No module named 'pynlm'"

- **Symptom**: Cannot import the NLM Python bindings
- **Causes**:
  1. **Module not installed**: Need to build and install the Python bindings
  2. **Wrong environment**: Using wrong Python environment
  3. **Build issues**: C++ bindings failed to compile

- **Solution**:
  ```bash
  # Install from source (development)
  pip install -e .
  
  # Or install wheel (release)
  pip install pynlm
  
  # Verify installation
  python -c "import pynlm; print(pynlm.__version__)"
  ```

### Part 7: Performance and Optimization

#### Efficient Simulation

```python
import time

def benchmark_simulation(num_steps=5000):
    """Benchmark simulation performance."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    
    # Use optimized batch processing
    for i in range(num_steps):
        brain.step(i)
    
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_second = num_steps / duration
    
    print(f"Simulation: {num_steps} steps in {duration:.2f}s")
    print(f"Performance: {steps_per_second:.0f} steps/second")
    print(f"Neurons created: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return duration, steps_per_second

# Run benchmark
benchmark_simulation(5000)
```

#### Memory Management

```python
def simulate_with_memory_monitoring(world, agent, num_episodes=100):
    """Monitor memory usage during long simulations."""
    brain = agent.getBrain()
    import tracemalloc
    
    tracemalloc.start()
    
    for episode in range(num_episodes):
        # Run episode
        brain.reset()
        agent.reset()
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(world.getSensoryPercept().getInternal()[0], 0.0)
        
        # Periodic memory checkpoint
        if episode % 50 == 0:
            current, peak = tracemalloc.get_traced_memory()
            print(f"Episode {episode}: "
                  f"Current memory: {current / 1024 / 1024:.1f} MB, "
                  f"Peak: {peak / 1024 / 1024:.1f} MB")
    
    tracemalloc.stop()
```

### Part 8: Additional Resources

#### More Information

- **Architecture Documentation**: `docs/ARCHITECTURE.md` - System architecture overview
- **Scientific Background**: `docs/SCIENCE.md` - Scientific foundations and research
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions and protocols
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Final integration audit report

#### Getting Help

**For questions about:
- **Basic usage**: Refer to `easy_usage.md`
- **Advanced API**: Refer to `HOW_TO_USE.md`
- **Building**: Refer to `README.md`
- **Development**: GitHub repository at `https://github.com/nlm-project/nlm`

#### Contributing

NLM is an open-source research project. Contributions are welcome for:
- **Bug fixes and improvements**
- **New neural mechanisms**
- **Documentation and tutorials**
- **Performance optimizations**
- **Python API enhancements**

See the repository's CONTRIBUTING.md for guidelines.

## Summary

This comprehensive documentation serves as a complete reference for using NLM:

1. **Quick Start**: Copy-paste examples for immediate use
2. **Core Concepts**: Understanding the brain architecture and operation
3. **Python API**: Complete reference for all classes and methods
4. **Configuration**: Detailed parameter explanations
5. **Advanced Usage**: Patterns for complex applications
6. **Troubleshooting**: Solutions to common problems
7. **Performance**: Optimization strategies and benchmarks

The Python bindings have been significantly enhanced with:
- **Convenience methods** for common operations
- **Better error handling** and type checking
- **Advanced configuration utilities**
- **Performance optimizations** for batch operations
- **Comprehensive documentation** with examples

This makes NLM more accessible to beginners while providing powerful capabilities for advanced researchers and developers.
