# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. **NEW!** Using the enhanced NLM Python API with all new features

---

## Part 1: Building the C++ Project from Source

(Previous content remains the same)

---

## Part 2: Installing the Python Library

(Previous content remains the same)

---

## Part 3: Using the Enhanced Python Library

### Basic Usage

(Previous content remains the same)

### Enhanced Configuration and Feature Usage

#### NEW! Enhanced Configuration API

The enhanced configuration system provides method chaining, dictionary operations, and simulation configuration factory.

```python
import pynlm as nlm

# Basic configuration (still works)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# NEW! Enhanced configuration with method chaining
config = nlm.createConfig() \
    .withNeuronCount(1000) \
    .withLearningRate(0.001) \
    .withDevelopment(True) \
    .withCuriosity(True) \
    .withRewardModulation(True)

brain = nlm.createBrain(config)
brain.initialize()

# NEW! Dictionary operations
config_dict = config.to_dict()  # Convert to Python dict
config2 = nlm.createConfig()
config2.from_dict({"brain.neuron_count": 2000})  # Load from dict

# NEW! Simulation configuration factory
sim_config = nlm.createConfig().createSimulationConfig(
    neuronCount=500,
    regionCount=2,
    learningRate="0.001",
    enableDevelopment=True,
    enableCuriosity=True,
    enableRewardModulation=True
)
```

#### NEW! Context Managers for Automatic Resource Management

```python
import pynlm as nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# NEW! Use context manager for automatic cleanup
with nlm.brain_context(brain) as brain_ctx:
    brain_ctx.brain.step(0)
    # Automatic cleanup when exiting context

# Create world
world = nlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()

# NEW! Use world context manager
with nlm.world_context(world) as world_ctx:
    world_ctx.world.update(0.1)
    percept = world_ctx.world.getSensoryPercept()
    # Automatic cleanup when exiting context
```

#### NEW! Simulation Utilities

```python
import pynlm as nlm

# NEW! Create a foraging environment with one line of code
config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
    worldWidth=20,
    worldHeight=20,
    visionWidth=8,
    visionHeight=8,
    numFoodResources=10
)

print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Vision size: {agent.getSensoryInputSize()}")

# Run a complete episode
brain, agent = nlm.SimulationUtils.runEpisode(
    config, world, 100, enableLearning=True
)

# NEW! Measure performance
perf_metrics = nlm.SimulationUtils.measurePerformance(
    brain, world, agent, 50
)
print(f"Average firing rate: {perf_metrics.averageFiringRate:.2f} Hz")
print(f"Curiosity level: {perf_metrics.curiosityLevel:.3f}")
```

#### NEW! Enhanced Vision Processing

```python
import pynlm as nlm

# Create enhanced vision sensor
vision = nlm.Vision(8, 8, 3)

# Set vision data
data = [0.1, 0.5, 0.9] * (8 * 8 * 3)
vision.setData(data)

# Convert to Python matrix for visualization
matrix = vision.to_python_matrix()
print(f"Matrix shape: {len(matrix)} x {len(matrix[0])}")

# Detect objects
objects = vision.detectObjects()
print(f"Objects detected: {objects['object_count']}")
print(f"Edges detected: {objects['edges_count']}")

# NEW! Get individual metrics
print(f"Edge count: {vision.getEdgeCount()}")
print(f"Object count: {vision.getObjectCount()}")
```

#### NEW! Performance Monitoring

```python
import pynlm as nlm

# Create environment
config, world, agent = nlm.create_complete_environment()
brain = agent.getBrain()

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# NEW! Create performance monitor
perf_monitor = nlm.PerformanceMonitor()
perf_monitor.start_monitoring()

# Run simulation with monitoring
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.decodeMotorCommand()
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Add metrics
    perf_monitor.add_metric("firing_rate", brain.getAverageFiringRate(), "Hz")
    perf_monitor.add_metric("curiosity", agent.getCuriosityLevel(), "")

# Get performance summary
summary = perf_monitor.get_performance_summary()
print(f"Overall performance score: {summary['summary']['overall_score']:.2f}")
print(f"Performance rating: {summary['summary']['performance_rating']}")

# NEW! Run performance benchmark
elapsed, steps_per_second, total_spikes, avg_firing_rate = nlm.benchmark_performance(
    config, world, agent, 100
)
print(f"Benchmark: {steps_per_second:.1f} steps/sec, {avg_firing_rate:.2f} Hz")
```

#### NEW! Advanced Features

```python
import pynlm as nlm

# NEW! Get environment builder
builder = nlm.advanced_features.get_environment_builder()

# Build custom environment
config, world, agent = builder \
    .with_name("Research Environment") \
    .with_world_size(25, 25) \
    .with_vision_size(10, 10) \
    .with_brain_size(2000) \
    .build()

# NEW! Get plugin manager
plugin_manager = nlm.advanced_features.get_plugin_manager()

# NEW! Create custom environment
custom_env = nlm.advanced_features.create_custom_environment(
    config={"brain.neuron_count": 1500},
    rules=[my_rule_function],
    dynamics=[my_dynamics_function],
    events=[my_event_function]
)
```

#### NEW! Error Handling

```python
import pynlm as nlm

# NEW! Try-catch with specific error types
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
    print(f"Error code: {e.error_code}")
    print(f"Key: {e.key}")
    
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
    print(f"Step: {e.step}")
    print(f"Time: {e.simulation_time}")
    
except nlm.VisionError as e:
    print(f"Vision error: {e}")
    
except nlm.PerformanceError as e:
    print(f"Performance error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Complete Agent Example (Enhanced)

```python
import pynlm as nlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced features."""
    
    # Create enhanced configuration
    config = nlm.createConfig() \
        .withNeuronCount(1500) \
        .withLearningRate(0.001) \
        .withDevelopment(True)
    
    # Create brain and initialize
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = nlm.createAgentBrain(brain)
    
    # Create and configure world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # NEW! Create performance monitor
    perf_monitor = nlm.PerformanceMonitor()
    perf_monitor.start_monitoring()
    
    # Run simulation loop
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
        
        # NEW! Add performance metrics
        perf_monitor.add_metric("firing_rate", brain.getAverageFiringRate(), "Hz")
        perf_monitor.add_metric("curiosity", agent.getCuriosityLevel(), "")
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # NEW! Get performance summary
    summary = perf_monitor.get_performance_summary()
    print(f"\nPerformance Summary:")
    print(f"  Overall score: {summary['summary']['overall_score']:.2f}")
    print(f"  Performance rating: {summary['summary']['performance_rating']}")
    print(f"  Total time: {summary['summary']['total_time']:.2f}s")
    print(f"  Steps per second: {summary['summary']['steps_per_second']:.1f}")
    
    return config, world, agent

# Run the simulation
config, world, agent = run_agent_simulation(1000)
```

### Key Classes and Methods (Enhanced)

#### Enhanced Brain Class
```python
# Creation with enhanced configuration
brain = nlm.createBrain(config)  # config created with method chaining

# All standard methods work as before
brain.initialize()
brain.step(step_number)
brain.reset()
```

#### Enhanced Config Class
```python
# Enhanced configuration creation
config = nlm.createConfig()

# Method chaining
config = config.withNeuronCount(1000).withLearningRate(0.001)

# Dictionary operations
config_dict = config.to_dict()
config2 = nlm.createConfig()
config2.from_dict({"brain.neuron_count": 2000})

# Simulation configuration
sim_config = nlm.createConfig().createSimulationConfig(neuronCount=500)
```

#### Enhanced AgentBrain Class
```python
# Creation
agent = nlm.createAgentBrain(brain)

# All standard methods work as before
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()

# Enhanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# NEW! Performance metrics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### Enhanced SimpleWorld Class
```python
# Creation
world = nlm.createSimpleWorld()

# All standard methods work as before
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.update(timestep)
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced features
world.setAgentStart(x, y)
world.getSensoryPercept()
world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, value=1.0))
```

### NEW! Enumerations (Enhanced)

```python
# All standard enumerations work as before
pynlm.NeuronType.Excitatory
pynlm.DevelopmentalStage.Initial
pynlm.ActionType.MoveForward
pynlm.WorldObjectType.Resource

# NEW! Enhanced enumerations
pynlm.PerformanceLevel.Excellent
pynlm.PerformanceLevel.Good
pynlm.PerformanceLevel.Fair
pynlm.PerformanceLevel.Poor
pynlm.PerformanceLevel.Critical
```

---

## Part 4: Configuration Options

(Previous content remains the same)

---

## Part 5: Example Scripts

### Minimal Example

(Previous content remains the same)

### Enhanced Environment Interaction

```python
import pynlm as nlm

# Setup with enhanced configuration
config = nlm.createConfig() \
    .withNeuronCount(1000) \
    .withLearningRate(0.001)

brain = nlm.createBrain(config)
brain.initialize()
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run enhanced simulation
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Part 6: Troubleshooting

(Previous content remains the same)

---

## Part 7: Further Reading

(Previous content remains the same)
