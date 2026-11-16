# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Advanced Usage (New Features)

### Command-Line Interface

The NLM command-line interface now supports many new options:

```bash
nlm --help                    # Show all available options
nlm --version                 # Show version information
nlm --config myconfig.cfg     # Load custom configuration
nlm --seed 123                # Set random seed for reproducibility
nlm --neuron-count 200        # Override neuron count
nlm --region-count 2          # Override region count
nlm --log-level debug         # Set detailed logging
```

### Experiment Management

NLM now includes advanced experiment management capabilities:

```python
import pynlm

# Create an experiment runner
runner = pynlm.createExperimentRunner()

# Create an experiment with metadata
experiment = runner.createExperiment(
    name="test_neural_network",
    description="Test neural network with advanced plasticity",
    tags={"type": "reinforcement_learning", "complexity": "high"}
)

# Configure advanced experiment settings
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("plasticity_learning_rate", 0.05)
config.set("enable_structural_plasticity", True)

# Run experiment with progress tracking
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleGoal()

progress = lambda progress: print(f"Progress: {progress.percentage}% (Step {progress.currentStep}/{progress.totalSteps})")

runner.runExperimentWithProgress(
    experiment=experiment,
    brain=brain,
    world=world,
    maxSteps=500,
    progressCallback=progress,
    config=config
)
```

### Enhanced Python Bindings

The Python bindings have been significantly enhanced:

```python
import pynlm

# Create brain with advanced configuration
config = pynlm.createAdvancedConfig()

# Set various parameters
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)
config.set("enable_structural_plasticity", True)
config.set("enable_reward_modulation", True)
config.set("enable_curiosity", True)
config.set("enable_prediction", True)
config.set("enable_attention", True)

# Load from file
config.loadFromFile("advanced_config.json")

# Save configuration
config.saveToFile("my_config.json")

brain = pynlm.createBrain(config)
brain.initialize()

# Use enhanced logging
brain.enableDetailedLogging(True)
brain.setLogLevel("info")

# Get comprehensive brain statistics
stats = brain.getBrainStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Synapses: {stats['total_synapses']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")

# Advanced plasticity control
brain.enableHebbianPlasticity(True, strength=0.1)
brain.enableSTDP(True, ltp_weight=0.02, ltd_weight=0.015)

# Save and load brain states
saved_path = "/tmp/my_brain_state.bin"
brain.save(saved_path)

# Load into new brain
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load(saved_path)
```

### Enhanced Logging and Monitoring

NLM now includes comprehensive logging and monitoring:

```python
import pynlm

# Configure advanced logging
logging_config = pynlm.createLoggingConfig()

# Console logging with colors
logging_config.setConsoleLogging(True)
logging_config.setLogLevel("debug")
logging_config.enableColors(True)

# File logging
logging_config.setFileLogging(True)
logging_config.setLogFile("simulation.log")
logging_config.setLogFormat("json")

# Performance monitoring
logging_config.enablePerformanceMonitoring(True)
logging_config.setPerformanceReportInterval(10.0)  # seconds

brain = pynlm.createBrain(config)
brain.initialize()
brain.configureLogging(logging_config)

# Real-time monitoring
monitor = brain.startMonitoring()
while not monitor.shouldStop():
    metrics = monitor.getCurrentMetrics()
    print(f"Step {monitor.getCurrentStep()}: "
          f"Spikes: {metrics['spike_count']}, "
          f"Firing: {metrics['firing_neurons']}, "
          f"Rate: {metrics['firing_rate']:.2f} Hz")
    brain.step(monitor.getCurrentStep())
    monitor.update()

monitor.stop()
```

### Advanced Neural Network Features

Enhanced neural network capabilities:

```python
# Create advanced brain with custom architecture
brain_config = pynlm.createAdvancedBrainConfig()

# Custom neuron types
brain_config.addNeuronType("excitatory", threshold=-55.0, reset=-70.0)
brain_config.addNeuronType("inhibitory", threshold=-40.0, reset=-65.0)
brain_config.addNeuronType("modulatory", threshold=-50.0, reset=-70.0)

# Custom synapse types
brain_config.addSynapseType("excitatory", weight_range=(0.0, 1.0), delay=1)
brain_config.addSynapseType("inhibitory", weight_range=(-1.0, 0.0), delay=2)
brain_config.addSynapseType("electrical", weight_range=(-1.0, 1.0), delay=0)

# Advanced plasticity rules
brain_config.setPlasticityRule("stdp", {
    "ltp_weight": 0.02,
    "ltd_weight": 0.015,
    "time_constant": 20.0
})

brain_config.setPlasticityRule("hebbian", {
    "learning_rate": 0.01,
    "eligibility_trace": True
})

brain_config.setPlasticityRule("reward_modulated", {
    "prediction_error": True,
    "dopamine_gain": 1.0
})

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Advanced learning scenarios
brain.enableStructuredLearning(True)
brain.setLearningEnvironment("grid_world")
brain.configureRewardFunction(lambda r: r * 0.1)
```

### Multi-Agent and Social Learning

Support for multiple interacting agents:

```python
# Create social learning environment
social_env = pynlm.createSocialEnvironment(num_agents=5)

# Create multiple brains with different architectures
agents = []
for i in range(5):
    agent_config = pynlm.createSocialLearningConfig()
    agent_config.set("agent_id", i)
    agent_config.set("social_influence", 0.5 + i * 0.1)
    agent_config.set("imitation_rate", 0.3)
    agent_config.set("cooperation_threshold", 0.7)
    
    brain = pynlm.createBrain(agent_config)
    agent = pynlm.createSocialAgent(brain, social_env)
    agents.append(agent)

# Run social learning simulation
for episode in range(100):
    # Reset environment
    social_env.reset()
    
    # Run episode with all agents
    for step in range(50):
        # Each agent observes the environment
        for agent in agents:
            percept = social_env.getObservation(agent.getId())
            agent.processObservation(percept)
        
        # Each agent chooses an action
        actions = [agent.chooseAction() for agent in agents]
        
        # Apply actions and get rewards
        rewards = social_env.step(actions)
        
        # Each agent learns from experience
        for i, agent in enumerate(agents):
            agent.learnFromExperience(rewards[i])
        
        # Check if episode ended
        if social_env.isEpisodeDone():
            break

# Analyze social learning results
social_stats = social_env.getSocialStats()
print(f"Cooperation rate: {social_stats['cooperation_rate']:.2f}")
print(f"Learning speed: {social_stats['learning_speed']:.2f}")
print(f"Emergent_behavior: {social_stats['emergent_behavior']}")
```

### Real-Time Control and API

WebSocket and API support for real-time control:

```python
import asyncio
import websockets

# Create NLM server with real-time control
server = pynlm.createRealTimeServer(host="localhost", port=8888)

# Register control functions
@s server.register_command("initialize")
async def initialize_brain(command):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    server.setVariable("brain", brain)
    return {"status": "initialized"}

@s server.register_command("step")
async def step_brain(command):
    brain = server.getVariable("brain")
    step = command.get("step", 0)
    brain.step(step)
    return {
        "spike_count": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate()
    }

@s server.register_command("inject_current")
async def inject_current(command):
    brain = server.getVariable("brain")
    neuron_id = command.get("neuron_id")
    current = command.get("current", 10.0)
    brain.injectCurrent(neuron_id, current)
    return {"status": "current_injected"}

# Start server
await server.start()
print("NLM server started on ws://localhost:8888")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
brain.enableRewardModulation(True)      # Learn from rewards
brain.enableStructuralPlasticity(True)  # Grow new connections
brain.enableDevelopment(True)           # Brain matures over time
brain.enableCuriosity(True)             # Explore new things
brain.enablePrediction(True)            # Predict future events
brain.enableAttention(True)             # Focus on important stimuli
```

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.

## New Advanced Features

### 1. Enhanced Configuration System

NLM now includes a comprehensive configuration system:

```python
# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Set various parameters
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("simulation_timestep", 0.001)
config.set("enable_structural_plasticity", True)
config.set("enable_reward_modulation", True)
config.set("enable_curiosity", True)
config.set("enable_prediction", True)
config.set("enable_attention", True)

# Load from file
config.loadFromFile("advanced_config.json")

# Save configuration
config.saveToFile("my_config.json")
```

### 2. Advanced Experiment Management

Create and manage complex experiments with metadata and tracking:

```python
# Create experiment runner
runner = pynlm.createExperimentRunner()

# Create experiment with full metadata
metadata = {
    "name": "brain_development_study",
    "description": "Study of brain development with different plasticity rules",
    "author": "Dr. Scientist",
    "version": "1.0",
    "tags": {"type": "developmental", "plasticity": "advanced"},
    "parameters": {"learning_rate": 0.01, "time_constant": 20.0}
}

experiment = runner.createExperiment(metadata)

# Run experiment with advanced configuration
experiment_config = {
    "max_steps": 1000,
    "timestep": 0.001,
    "random_seed": 123,
    "enable_logging": True,
    "progress_reporting": True,
    "parallel_threads": 4,
    "save_intermediate_results": True
}

results = runner.runExperiment(experiment, experiment_config)
```

### 3. Enhanced Python Bindings

The Python bindings now include:

- **Advanced configuration access**: Get, set, and validate configuration values
- **Enhanced sensory processing**: Multiple sensory modalities with preprocessing
- **Advanced motor control**: Complex action generation with parameter tuning
- **Performance monitoring**: Real-time metrics collection and analysis
- **Error handling**: Comprehensive error reporting and recovery

```python
# Use enhanced configuration
config = pynlm.createBrainConfig()
config.set("neuron_count", 500)
config.set("plasticity_rules", {"stdp": True, "hebbian": True, "reward": True})
config.set("environment", "grid_world")
config.set("observation_mode", "full_vision")

brain = pynlm.createBrain(config)

# Enhanced sensory processing
vision = pynlm.createVisionInput(width=32, height=32, channels=3)
vision.setData(some_image_data)
vision.normalize()  # Automatic preprocessing
vision.applyAugmentation()  # Data augmentation

# Advanced motor control
action = pynlm.createAction(
    action_type=pynlm.ActionType.MoveForward,
    parameters={"speed": 0.5, "duration": 1.0}
)

# Performance monitoring
brain.enablePerformanceMonitoring(True)
metrics = brain.getPerformanceMetrics()
print(f"FPS: {metrics['fps']:.2f}")
print(f"Memory usage: {metrics['memory_mb']:.2f} MB")
```

### 4. Advanced Logging and Monitoring

Comprehensive logging system with multiple output formats:

```python
# Configure advanced logging
logging_config = pynlm.createLoggingConfig()

# Console logging with colors
logging_config.setConsoleLogging(True)
logging_config.setLogLevel("debug")
logging_config.enableColors(True)

# File logging
logging_config.setFileLogging(True)
logging_config.setLogFile("simulation.log")
logging_config.setLogFormat("json")

# Performance monitoring
logging_config.enablePerformanceMonitoring(True)
logging_config.setPerformanceReportInterval(10.0)  # seconds

brain = pynlm.createBrain(config)
brain.initialize()
brain.configureLogging(logging_config)

# Real-time monitoring
monitor = brain.startMonitoring()
while not monitor.shouldStop():
    metrics = monitor.getCurrentMetrics()
    print(f"Step {monitor.getCurrentStep()}: "
          f"Spikes: {metrics['spike_count']}, "
          f"Firing: {metrics['firing_neurons']}, "
          f"Rate: {metrics['firing_rate']:.2f} Hz")
    brain.step(monitor.getCurrentStep())
    monitor.update()

monitor.stop()
```

### 5. Advanced Neural Network Features

Enhanced neural network capabilities:

```python
# Create advanced brain with custom architecture
brain_config = pynlm.createAdvancedBrainConfig()

# Custom neuron types
brain_config.addNeuronType("excitatory", threshold=-55.0, reset=-70.0)
brain_config.addNeuronType("inhibitory", threshold=-40.0, reset=-65.0)
brain_config.addNeuronType("modulatory", threshold=-50.0, reset=-70.0)

# Custom synapse types
brain_config.addSynapseType("excitatory", weight_range=(0.0, 1.0), delay=1)
brain_config.addSynapseType("inhibitory", weight_range=(-1.0, 0.0), delay=2)
brain_config.addSynapseType("electrical", weight_range=(-1.0, 1.0), delay=0)

# Advanced plasticity rules
brain_config.setPlasticityRule("stdp", {
    "ltp_weight": 0.02,
    "ltd_weight": 0.015,
    "time_constant": 20.0
})

brain_config.setPlasticityRule("hebbian", {
    "learning_rate": 0.01,
    "eligibility_trace": True
})

brain_config.setPlasticityRule("reward_modulated", {
    "prediction_error": True,
    "dopamine_gain": 1.0
})

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Advanced learning scenarios
brain.enableStructuredLearning(True)
brain.setLearningEnvironment("grid_world")
brain.configureRewardFunction(lambda r: r * 0.1)
```

### 6. Multi-Agent and Social Learning

Support for multiple interacting agents:

```python
# Create social learning environment
social_env = pynlm.createSocialEnvironment(num_agents=5)

# Create multiple brains with different architectures
agents = []
for i in range(5):
    agent_config = pynlm.createSocialLearningConfig()
    agent_config.set("agent_id", i)
    agent_config.set("social_influence", 0.5 + i * 0.1)
    agent_config.set("imitation_rate", 0.3)
    agent_config.set("cooperation_threshold", 0.7)
    
    brain = pynlm.createBrain(agent_config)
    agent = pynlm.createSocialAgent(brain, social_env)
    agents.append(agent)

# Run social learning simulation
for episode in range(100):
    # Reset environment
    social_env.reset()
    
    # Run episode with all agents
    for step in range(50):
        # Each agent observes the environment
        for agent in agents:
            percept = social_env.getObservation(agent.getId())
            agent.processObservation(percept)
        
        # Each agent chooses an action
        actions = [agent.chooseAction() for agent in agents]
        
        # Apply actions and get rewards
        rewards = social_env.step(actions)
        
        # Each agent learns from experience
        for i, agent in enumerate(agents):
            agent.learnFromExperience(rewards[i])
        
        # Check if episode ended
        if social_env.isEpisodeDone():
            break

# Analyze social learning results
social_stats = social_env.getSocialStats()
print(f"Cooperation rate: {social_stats['cooperation_rate']:.2f}")
print(f"Learning speed: {social_stats['learning_speed']:.2f}")
print(f"Emergent_behavior: {social_stats['emergent_behavior']}")
```

### 7. Real-Time Control and API

WebSocket and API support for real-time control:

```python
import asyncio
import websockets

# Create NLM server with real-time control
server = pynlm.createRealTimeServer(host="localhost", port=8888)

# Register control functions
@s server.register_command("initialize")
async def initialize_brain(command):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    server.setVariable("brain", brain)
    return {"status": "initialized"}

@s server.register_command("step")
async def step_brain(command):
    brain = server.getVariable("brain")
    step = command.get("step", 0)
    brain.step(step)
    return {
        "spike_count": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate()
    }

@s server.register_command("inject_current")
async def inject_current(command):
    brain = server.getVariable("brain")
    neuron_id = command.get("neuron_id")
    current = command.get("current", 10.0)
    brain.injectCurrent(neuron_id, current)
    return {"status": "current_injected"}

# Start server
await server.start()
print("NLM server started on ws://localhost:8888")
```

---

## Performance Optimization

### 1. SIMD Vectorization

NLM now supports SIMD (Single Instruction, Multiple Data) operations:

```python
# Enable SIMD optimization
brain_config = pynlm.createBrainConfig()
brain_config.enableSIMD(True)
brain_config.setSIMDLevel("avx512")

# SIMD-accelerated operations
brain = pynlm.createBrain(brain_config)
brain.initialize()

# Performance metrics
perf = brain.getPerformanceMetrics()
print(f"SIMD speedup: {perf['simd_speedup']:.2f}x")
print(f"FLOPs/s: {perf['flops_per_second']:.2e}")
```

### 2. Memory Pooling

Efficient memory management with object pooling:

```python
# Enable memory pooling
brain_config = pynlm.createBrainConfig()
brain_config.enableMemoryPooling(True)
brain_config.setPoolSize("neuron", 1000)
brain_config.setPoolSize("synapse", 10000)

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Memory statistics
stats = brain.getMemoryStats()
print(f"Memory usage: {stats['memory_mb']:.2f} MB")
print(f"Allocation count: {stats['allocation_count']}")
print(f"Deallocation count: {stats['deallocation_count']}")
```

### 3. Parallel Processing

Multi-core processing capabilities:

```python
# Enable parallel processing
brain_config = pynlm.createBrainConfig()
brain_config.enableParallelProcessing(True)
brain_config.setParallelThreads(4)
brain_config.setCPUAffinity([0, 1, 2, 3])  # Use specific CPU cores

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Parallel processing metrics
perf = brain.getPerformanceMetrics()
print(f"Parallel efficiency: {perf['parallel_efficiency']:.2%}")
print(f"Speedup factor: {perf['speedup_factor']:.2f}x")
```

---

## Scientific Validation

NLM includes validation tests for scientific accuracy:

### 1. LIF Neuron Dynamics Validation

```python
# Test LIF neuron properties
neuron = pynlm.createNeuron()
neuron.setParameters(threshold=-55.0, resting_potential=-70.0, 
                     time_constant=20.0, capacitance=1.0)

# Validate membrane potential dynamics
for step in range(100):
    v = neuron.getMembranePotential()
    if step % 20 == 0:
        neuron.injectCurrent(10.0)  # Inject current
    neuron.step(0.001)  # Simulation timestep
    
    # Check for reasonable behavior
    assert -80.0 <= v <= 50.0, f"Membrane potential out of range: {v}"
    assert neuron.getFiringRate() >= 0.0, f"Negative firing rate: {neuron.getFiringRate()}"
```

### 2. Plasticity Rule Validation

```python
# Test STDP rule
brain_config = pynlm.createBrainConfig()
brain_config.set("neuron_count", 100)
brain_config.set("enable_stdp", True)

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Enable STDP and test potentiation/depression
synapse = brain.getSynapse(1)  # Get specific synapse
synapse.enablePlasticity(True, True, False)  # STDP only

# Test correlated pre-post activity
pre_neuron = synapse.getSourceNeuron()
post_neuron = synapse.getDestinationNeuron()

# Potentiation trial
for step in range(50):
    brain.injectCurrent(pre_neuron, 20.0)
    brain.step(step)
    brain.injectCurrent(post_neuron, 20.0)
    brain.step(step + 1)

initial_weight = synapse.getWeight()
for step in range(100):
    brain.injectCurrent(pre_neuron, 20.0)
    brain.step(step)
    brain.injectCurrent(post_neuron, 20.0)
    brain.step(step + 1)

final_weight = synapse.getWeight()
assert final_weight > initial_weight, "STDP potentiation failed"

# Depression trial (anti-correlation)
for step in range(100):
    brain.injectCurrent(pre_neuron, 20.0)
    brain.step(step)
    brain.injectCurrent(post_neuron, -20.0)  # Negative current
    brain.step(step + 1)

# Weight should decrease
assert synapse.getWeight() < final_weight, "STDP depression failed"
```

### 3. Network Stability Validation

```python
# Test network stability
brain_config = pynlm.createBrainConfig()
brain_config.set("neuron_count", 200)
brain_config.set("connection_probability", 0.1)

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Run simulation and check for stability
spike_counts = []
for step in range(500):
    brain.step(step)
    spike_counts.append(brain.getTotalSpikeCount())

# Check for stability (should not explode or go to zero)
avg_spikes = sum(spike_counts) / len(spike_counts)
max_spikes = max(spike_counts)
min_spikes = min(spike_counts)

print(f"Average spikes per step: {avg_spikes:.2f}")
print(f"Max spikes per step: {max_spikes}")
print(f"Min spikes per step: {min_spikes}")

# Stability criteria
assert avg_spikes > 0, "Network went silent"
assert max_spikes < avg_spikes * 10, "Network exploded"
assert min_spikes > 0, "Network completely inactive"
```

---

## Performance Benchmarking

NLM includes comprehensive benchmarking capabilities:

```python
# Create benchmark suite
benchmark = pynlm.createBenchmarkSuite()

# Add different configurations to benchmark
benchmark.addConfiguration({
    "name": "small_network",
    "neuron_count": 100,
    "timestep": 0.001,
    "simulation_steps": 100
})

benchmark.addConfiguration({
    "name": "medium_network",
    "neuron_count": 500,
    "timestep": 0.001,
    "simulation_steps": 500
})

benchmark.addConfiguration({
    "name": "large_network",
    "neuron_count": 1000,
    "timestep": 0.001,
    "simulation_steps": 1000
})

# Run benchmarks
results = benchmark.runAllBenchmarks()

# Print results
for config_name, metrics in results.items():
    print(f"{config_name}:")
    print(f"  Simulation time: {metrics['simulation_time']:.2f}s")
    print(f"  Throughput: {metrics['throughput']:.2f} spikes/s")
    print(f"  Memory efficiency: {metrics['memory_efficiency']:.2f} MB/spike")
```

---

## Installation and Setup

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher
- pip (for Python packages)

### Installation Steps

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install Python dependencies
pip install -r requirements.txt

# Build the C++ extensions
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install the Python package
pip install -e ..

# Test installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Alternative: Using Docker

```bash
# Build Docker image
docker build -t nlm:latest .

# Run container
docker run -it nlm:latest

# Interactive shell
docker exec -it <container_id> /bin/bash
```

---

## Getting Help

### Documentation

- **Main documentation**: `README.md`
- **Python usage guide**: `easy_usage.md`
- **Architecture documentation**: `docs/ARCHITECTURE.md`
- **API reference**: `docs/API.md`
- **Scientific background**: `docs/SCIENCE.md`

### Online Resources

- **GitHub repository**: https://github.com/nlm-project/nlm
- **Documentation website**: https://nlm.readthedocs.io
- **Issue tracker**: https://github.com/nlm-project/nlm/issues
- **Discussion forum**: https://github.com/nlm-project/nlm/discussions

### Support

For technical support and questions:

1. **GitHub**: Create an issue with detailed description
2. **Documentation**: Check the FAQ and examples
3. **Community**: Join discussions and ask questions

### Troubleshooting Common Issues

#### "My brain isn't doing anything"
- **Solution**: Ensure you call `brain.initialize()` before starting simulation
- **Debug**: Add logging with `brain.enableLogging(True)`

#### "The agent isn't moving"
- **Solution**: Ensure you call `brain.step()` regularly
- **Debug**: Check sensory input with `agent.getSensoryInput()`

#### "Everything is 0"
- **Solution**: Brains need time to "warm up" - run more steps
- **Debug**: Check neuron parameters and connectivity

#### "Performance issues"
- **Solution**: Optimize configuration parameters
- **Debug**: Use profiling tools

### Advanced Support

For advanced users and researchers:

1. **API documentation**: Detailed API reference in `docs/API.md`
2. **Research support**: Contact the development team
3. **Performance analysis**: Use benchmarking tools
4. **Integration**: Custom integration with other frameworks

---

## Conclusion

NLM represents the next generation of artificial neural network simulators with:

- **Realistic neural dynamics**: LIF neurons with biological plausibility
- **Advanced learning mechanisms**: Multiple plasticity rules
- **Comprehensive memory systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Prediction and planning**: Anticipatory and planning capabilities
- **Enhanced simulation**: Parallel processing, memory optimization
- **Scientific validation**: Comprehensive testing and validation
- **Production-ready**: Robust error handling and performance monitoring

Whether you're a beginner exploring neural networks or a researcher developing advanced brain models, NLM provides the tools and features you need to build sophisticated neural systems that learn, adapt, and solve complex problems.

Ready to start your neural exploration? Begin with the simple examples above and gradually explore the advanced features as you become more familiar with NLM!
