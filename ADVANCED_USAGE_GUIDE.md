# Advanced NLM Usage Guide

## Quick Reference

### Core API Functions
```python
import pynlm

# Configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Brain creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Agent interface
agent = pynlm.createAgentBrain(brain)

# World setup
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

## Simulation Patterns

### 1. Standard Agent Loop
```python
def standard_agent_loop(brain, world, agent, num_steps):
    """Standard agent-environment interaction loop."""
    for step in range(num_steps):
        # Environment dynamics
        world.update(0.1)
        
        # Sensory perception
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Action execution
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = get_reward(action, percept)  # Environment provides this
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.1)
```

### 2. Exploration Pattern
```python
def exploration_pattern(brain, world, agent, exploration_steps):
    """Agent actively explores environment using curiosity."""
    for step in range(exploration_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Curiously choose actions
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.3:
            # Occasionally take random actions for exploration
            action = random_explore_action()
        else:
            action = agent.decodeMotorCommand()
            
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)  # No immediate reward for exploration
```

### 3. Learning Pattern
```python
def learning_pattern(brain, world, agent, learning_steps):
    """Agent learns from successful experiences."""
    best_reward = -float('inf')
    best_action = None
    
    for step in range(learning_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get environmental reward
        reward = get_environment_reward(action, percept)
        
        # Track best actions
        if reward > best_reward:
            best_reward = reward
            best_action = action
            
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}: Best action {best_action} with reward {best_reward}")
```

## Advanced Configuration

### Brain Configuration Options
```python
def configure_advanced_brain(config):
    """Configure brain with advanced settings for complex tasks."""
    # Network size and topology
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    
    # Neuron parameters
    config.set("brain.v_thresh", -45.0)
    config.set("brain.v_rest", -65.0)
    config.set("brain.tau_mem", 20.0)
    config.set("brain.tau_ref", 2.0)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.adult_plasticity", 0.2)
    
    return config
```

### World Configuration
```python
def configure_complex_world(world):
    """Configure world with complex dynamics and objects."""
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12
    )
    
    # Set interesting starting positions
    world.setAgentStart(15.0, 15.0)
    
    # Add resources and hazards
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Hazard, value=-5.0))
    
    # Configure world properties
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.95)
    world.setRandomSeed(42)
```

## Monitoring and Debugging

### Brain Statistics
```python
def monitor_brain(brain, step):
    """Monitor brain statistics during simulation."""
    print(f"Step {step}:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### Agent Monitoring
```python
def monitor_agent(agent, step):
    """Monitor agent state and learning progress."""
    print(f"Agent State - Step {step}:")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Learned preference: {agent.getLearnedPreference():.3f}")
    print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Check which subsystems are enabled
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
```

## Performance Optimization

### Efficient Simulation
```python
def optimize_simulation(brain, world, agent, target_fps=30):
    """Optimize simulation for specific performance targets."""
    timestep = 1.0 / target_fps
    
    # Pre-allocate if possible (for large simulations)
    if brain.getTotalNeuronCount() > 1000:
        print("Large simulation detected - optimizing memory usage")
        # Could implement memory pooling here
    
    # Performance monitoring
    import time
    step_times = []
    
    for step in range(target_fps * 60):  # Run for 60 seconds
        start_time = time.time()
        
        world.update(timestep)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        step_times.append(end_time - start_time)
        
        # Check if we're meeting performance targets
        avg_step_time = sum(step_times[-100:]) / len(step_times[-100:])
        if avg_step_time > timestep:
            print(f"Performance warning: Average step time {avg_step_time:.4f} exceeds timestep {timestep}")
```

### Memory Management
```python
def memory_efficient_simulation(brain, world, agent, max_memory_mb=1000):
    """Run simulation with memory efficiency checks."""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss / 1024 / 1024
    
    for step in range(10000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor memory usage
        current_memory = process.memory_info().rss / 1024 / 1024
        memory_increase = current_memory - initial_memory
        
        if memory_increase > max_memory_mb:
            print(f"Memory limit exceeded: {memory_increase:.1f} MB")
            break
            
        if step % 1000 == 0:
            print(f"Step {step}: Memory usage {memory_increase:.1f} MB")
```

## Advanced Features

### Learning and Adaptation
```python
def adaptive_learning(brain, world, agent, adaptation_steps):
    """Agent adapts its behavior based on performance feedback."""
    performance_history = []
    adaptation_threshold = 0.1
    
    for step in range(adaptation_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and track performance
        reward = get_reward(action, percept)
        performance_history.append(reward)
        
        # Adapt based on recent performance
        if len(performance_history) >= 100:
            recent_performance = sum(performance_history[-100:]) / 100
            if abs(recent_performance - 0.0) > adaptation_threshold:
                # Adjust curiosity to explore more or less
                current_curiosity = agent.getCuriosityLevel()
                if recent_performance < 0:  # Poor performance
                    agent.enableCuriosity(True)  # Explore more
                else:  # Good performance
                    agent.enableCuriosity(False)  # Exploit more
        
        agent.applyRewardModulation(reward, 0.0)
```

### Multi-Agent Coordination
```python
def multi_agent_coordination(brain1, brain2, world1, world2, steps):
    """Coordinate two agents working together."""
    # Create two agents that can communicate
    agent1 = pynlm.createAgentBrain(brain1)
    agent2 = pynlm.createAgentBrain(brain2)
    
    # Initialize with shared world (or separate worlds)
    agent1.initialize(world1)
    agent2.initialize(world2)
    
    # Enable social learning
    # (Would require implementing social learning between agents)
    
    for step in range(steps):
        # Both agents perceive and act
        percept1 = world1.getSensoryPercept()
        percept2 = world2.getSensoryPercept()
        
        agent1.processSensoryInput(percept1)
        agent2.processSensoryInput(percept2)
        
        brain1.step(step)
        brain2.step(step)
        
        action1 = agent1.decodeMotorCommand()
        action2 = agent2.decodeMotorCommand()
        
        world1.applyMotorCommand(action1, world1.getSimulationTime())
        world2.applyMotorCommand(action2, world2.getSimulationTime())
        
        # Agents can observe each other's actions
        agent1.observeOtherAgent(action2, get_vision_state(world2))
        agent2.observeOtherAgent(action1, get_vision_state(world1))
        
        # Joint learning from coordination
        reward1 = get_joint_reward(action1, action2, percept1, percept2)
        reward2 = get_joint_reward(action2, action1, percept2, percept1)
        
        agent1.applyRewardModulation(reward1, 0.0)
        agent2.applyRewardModulation(reward2, 0.0)
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Brain initialization fails**
```python
# Solution: Always initialize brain before use
brain = pynlm.createBrain(config)
brain.initialize()  # This is crucial!
```

**Issue 2: Agent actions are all the same**
```python
# Solution: Enable curiosity and development
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

**Issue 3: No learning occurring**
```python
# Solution: Check if reward modulation is enabled
if agent.isRewardModulationEnabled():
    print("Reward modulation enabled")
else:
    agent.enableRewardModulation(True)
```

**Issue 4: Performance issues**
```python
# Solution: Profile and optimize
import cProfile, pstats
import io
from pstats import SortKey

pr = cProfile.Profile()
pr.enable()

# Run your simulation
run_simulation(brain, world, agent, 1000)

pr.disable()
s = io.StringIO()
sortby = SortKey.CUMULATIVE
ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
ps.print_stats()
print(s.getvalue())
```

## Migration Guide

### From Previous Versions

If you're migrating from an older version of NLM, here are the key API changes:

**Configuration Loading**
```python
# Old way (if it worked)
config = pynlm.createDefaultConfig()
config.loadFromArgs(["--brain.neuron_count=1000"])

# New way
config = pynlm.createDefaultConfig()
# Configuration is set automatically via constructor parameters
```

**Brain Initialization**
```python
# Old way (might not have worked)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# New way (documented in easy_usage.md)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

**Agent Methods**
```python
# All agent methods are now properly documented:
agent.initialize(world)                    # ✅ Available
agent.getSensoryInputSize()                # ✅ Available
agent.getMotorOutputSize()                 # ✅ Available
agent.processSensoryInput(percept)         # ✅ Available
agent.decodeMotorCommand()                 # ✅ Available
agent.applyRewardModulation(reward, pred)  # ✅ Available
agent.updateDevelopment(timestep)          # ✅ Available
```

## API Summary

### Configuration Class
- `createDefaultConfig()`: Create default configuration
- `config.set(key, value)`: Set configuration value
- `config.has(key)`: Check if key exists
- `config.getOr(key, default)`: Get value with default

### Brain Class
- `createBrain(config)`: Create brain with configuration
- `brain.initialize()`: Initialize brain
- `brain.step(step, time)`: Run simulation step
- `brain.getTotalNeuronCount()`: Get neuron count
- `brain.getTotalSynapseCount()`: Get synapse count
- `brain.getFiringNeuronCount()`: Get firing neuron count
- `brain.getAverageFiringRate()`: Get average firing rate
- And many more statistics and control methods

### AgentBrain Class
- `createAgentBrain(brain)`: Create agent interface
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process perception
- `agent.decodeMotorCommand()`: Get motor action
- `agent.applyRewardModulation(reward, predicted)`: Learn from reward
- `agent.updateDevelopment(timestep)`: Update development
- And many more monitoring and control methods

### SimpleWorld Class
- `createSimpleWorld()`: Create world
- `world.configure(width, height, visionWidth, visionHeight)`: Configure dimensions
- `world.reset()`: Reset world state
- `world.update(timestep)`: Update world
- `world.applyMotorCommand(action, time)`: Execute action
- `world.getSensoryPercept()`: Get current perception

This comprehensive guide provides everything needed to get started with NLM and go all the way to advanced usage and optimization.