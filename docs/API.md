# NLM (Neural Learning Machine) - Improved Python API Guide

## Overview

This document provides a comprehensive guide to using NLM's Python API (`pynlm`). It includes:

- **Getting Started**: Quick start for beginners
- **Advanced Usage**: Features for experienced users
- **API Reference**: Complete method documentation
- **Examples**: Real-world usage patterns
- **Troubleshooting**: Common issues and solutions

## Installation

The Python package is automatically installed when using `pip install .` or `pip install -e .`:

```bash
pip install .
```

Or install in development mode for faster iteration:

```bash
pip install -e .
```

## Quick Start (Beginner)

### Example 1: Silent Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Agent in World

```python
import pynlm

# Setup complete agent simulation
def run_complete_simulation():
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")

# Run the simulation
run_complete_simulation()
```

## Advanced Features

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

# Save configuration to file
config.saveToFile("my_config.json")

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Text-as-a-Sense Feature

```python
import pynlm

# Setup agent for text processing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Create sensory percept with text
percept = world.getSensoryPercept()
percept.setText("Hello world")  # Inject text
agent.processSensoryInput(percept)

# Brain processes text and creates associations
brain.step(0)

# Get text encoding information
print(f"Text: {percept.getText()}")
print(f"Thought trace: {agent.getThoughtTrace()}")
```

### Learning and Development

```python
import pynlm

# Configure brain for developmental learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(1000):
    # Environment interaction
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on environment interaction
    reward = 0.0  # Your reward calculation here
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Print learning statistics
    if step % 200 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Learned preference: {agent.getLearnedPreference():.3f}")

print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

### Checkpoint System

```python
import pynlm

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Run for a while
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.1, 0.0)
    agent.updateDevelopment(0.1)

# Save brain state
save_path = "brain_checkpoint.bin"
brain.save(save_path)
print(f"Brain state saved to {save_path}")

# Later, load the saved state
brain.reset()
brain.load(save_path)
print(f"Brain state loaded successfully")
print(f"Loaded neurons: {brain.getTotalNeuronCount()}")
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Error Handling

### Common Errors and Solutions

**1. "ModuleNotFoundError: No module named 'pynlm'"**

**Solution**: Ensure the package is installed:
```bash
pip install -e .
```

**2. Runtime Errors**

**Solution**: Always call `brain.initialize()` before using the brain:
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
brain.step(0)      # Now safe to use
```

**3. Segmentation Fault**

**Solution**: Check that `reset()` is called before re-running simulations:
```python
brain.reset()  # Clean state before new simulation
brain.initialize()
```

**4. Invalid Action Commands**

**Solution**: Use valid action types:
```python
# Valid actions
agent.decodeMotorCommand()  # Returns MoveForward, MoveBackward, etc.
# Avoid custom invalid actions
```

## Advanced Usage

### 1. Custom World Implementation

```python
import pynlm

class CustomWorld:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.brain = None
        self.agent = None
    
    def setup(self, width=30, height=30):
        self.world.configure(width=width, height=height, 
                           visionWidth=16, visionHeight=16)
        self.world.reset()
        self.world.setAgentStart(width/2, height/2)
        
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def run_episode(self, max_steps=1000):
        for step in range(max_steps):
            # Custom world dynamics
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process in brain
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Custom action handling
            self.handleAction(action, step)
            
            # Check termination conditions
            if self.checkTermination():
                break
        
        return self.getEpisodeStats()

# Use the custom world
world_sim = CustomWorld()
world_sim.setup()
stats = world_sim.run_episode()
```

### 2. Multi-Agent System

```python
import pynlm

class MultiAgentSystem:
    def __init__(self):
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
    
    def setup(self, num_agents=3, world_size=20):
        # Setup shared world
        self.shared_world.configure(width=world_size, height=world_size,
                                   visionWidth=8, visionHeight=8)
        self.shared_world.reset()
        
        # Create multiple agents with different behaviors
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.shared_world)
            
            # Each agent can have unique settings
            if i == 0:
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
            elif i == 1:
                agent.enableStructuralPlasticity(True)
            
            self.agents.append(agent)
    
    def run_synchronized(self, steps=500):
        for step in range(steps):
            # All agents perceive the same world state
            percept = self.shared_world.getSensoryPercept()
            
            # Each agent processes independently
            for agent in self.agents:
                agent.processSensoryInput(percept)
                agent.getBrain().step(step)
                action = agent.decodeMotorCommand()
                self.shared_world.applyMotorCommand(action, step * 0.1)
            
            # All agents receive same reward
            for agent in self.agents:
                agent.applyRewardModulation(0.0, 0.0)
            
            self.shared_world.update(0.1)
    
    def get_system_stats(self):
        stats = {
            'agents': [],
            'world': {
                'time': self.shared_world.getSimulationTime(),
                'size': (self.shared_world.getWidth(), self.shared_world.getHeight())
            }
        }
        
        for i, agent in enumerate(self.agents):
            stats['agents'].append({
                'id': i,
                'neurons': agent.getBrain().getTotalNeuronCount(),
                'spikes': agent.getBrain().getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'stage': agent.getDevelopmentalStage()
            })
        
        return stats

# Use the multi-agent system
mas = MultiAgentSystem()
mas.setup(num_agents=3)
mas.run_synchronized(steps=300)
system_stats = mas.get_system_stats()
```

### 3. Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.step_times = []
        self.spike_counts = []
        self.memory_usage = []
    
    def start_step(self):
        self.step_start = time.time()
    
    def end_step(self):
        step_time = time.time() - self.step_start
        self.step_times.append(step_time)
        
        # Record performance metrics
        self.spike_counts.append(self.brain.getTotalSpikeCount())
    
    def get_performance_stats(self):
        if not self.step_times:
            return {}
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        max_step_time = max(self.step_times)
        min_step_time = min(self.step_times)
        
        # Calculate spikes per second
        total_time = sum(self.step_times)
        steps_per_second = len(self.step_times) / total_time if total_time > 0 else 0
        
        return {
            'avg_step_time_ms': avg_step_time * 1000,
            'max_step_time_ms': max_step_time * 1000,
            'min_step_time_ms': min_step_time * 1000,
            'steps_per_second': steps_per_second,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'current_firing_rate': self.brain.getAverageFiringRate()
        }

# Performance monitoring example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = PerformanceMonitor(brain)

# Run with monitoring
for step in range(100):
    monitor.start_step()
    brain.step(step)
    monitor.end_step()
    
    if step % 50 == 0:
        stats = monitor.get_performance_stats()
        print(f"Step {step} Performance:")
        print(f"  Average step time: {stats['avg_step_time_ms']:.2f} ms")
        print(f"  Steps per second: {stats['steps_per_second']:.1f}")
        print(f"  Firing rate: {stats['current_firing_rate']:.1f} Hz")
```

## Command-Line Interface (New Feature)

The NLM project now includes a command-line interface for running simulations:

### Installation

```bash
# The CLI is installed with pip install
nlm-cli --help
```

### Basic Usage

```bash
# Run a simple demo
nlm-cli demo --type basic --steps 100

# Run a complete agent simulation
nlm-cli demo --type agent --steps 500 --config custom_config.json

# Run a learning experiment
nlm-cli demo --type learning --steps 1000 --plasticity

# Save results
nlm-cli demo --type agent --steps 1000 --output results.json
```

### Available Commands

- `nlm-cli demo`: Run pre-built demonstration simulations
- `nlm-cli benchmark`: Performance benchmarking
- `nlm-cli experiment`: Custom experiment runner
- `nlm-cli config`: Configuration management

## Integration Examples

### Integration with External Frameworks

```python
import pynlm

class NLMEnvironment:
    """Reinforcement learning environment using NLM"""
    
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.setup()
    
    def setup(self):
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
    
    def reset(self):
        self.world.reset()
        self.agent.reset()
        return self.get_observation()
    
    def get_observation(self):
        percept = self.world.getSensoryPercept()
        # Convert to RL observation format
        return {
            'vision': percept.getVision(),
            'position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
            'velocity': (self.world.getAgentBody().velocityX, 
                        self.world.getAgentBody().velocityY)
        }
    
    def step(self, action):
        # Convert RL action to NLM action
        # Map RL action space to NLM motor commands
        action_map = {
            0: pynlm.MotorCommand.MoveForward,
            1: pynlm.MotorCommand.MoveBackward,
            2: pynlm.MotorCommand.TurnLeft,
            3: pynlm.MotorCommand.TurnRight,
            4: pynlm.MotorCommand.Interact
        }
        
        nlm_action = action_map.get(action, pynlm.MotorCommand.Wait)
        
        # Execute action
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(int(self.world.getSimulationTime()))
        world.applyMotorCommand(nlm_action, self.world.getSimulationTime())
        
        # Get next state and reward
        next_percept = self.world.getSensoryPercept()
        reward = self.calculate_reward(next_percept)
        done = self.check_done()
        
        return self.get_observation(), reward, done, {}
    
    def calculate_reward(self, percept):
        # Custom reward function
        reward = 0.0
        # Add reward based on vision, position, etc.
        return reward
    
    def check_done(self):
        # Termination conditions
        body = self.world.getAgentBody()
        return (body.health <= 0 or 
                body.energy <= 0 or
                self.world.getSimulationTime() > 1000)

# Use as RL environment
env = NLMEnvironment()
observation = env.reset()

# RL loop
for step in range(1000):
    action = agent.act(observation)  # Your RL agent
    next_obs, reward, done, info = env.step(action)
    agent.learn(observation, action, reward, next_obs, done)
    observation = next_obs
    
    if done:
        observation = env.reset()
```

## API Reference

### Core Classes

#### Brain
- `createBrain(config)`: Create a new brain
- `brain.initialize()`: Initialize the brain
- `brain.step(step)`: Perform a simulation step
- `brain.reset()`: Reset brain state
- `brain.getTotalNeuronCount()`: Get total neuron count
- `brain.getTotalSynapseCount()`: Get total synapse count
- `brain.getFiringNeuronCount()`: Get currently firing neurons
- `brain.getAverageFiringRate()`: Get average firing rate
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

#### AgentBrain
- `createAgentBrain(brain)`: Create agent interface
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command
- `agent.applyRewardModulation(reward, predicted)`: Apply reward
- `agent.enableRewardModulation(bool)`: Enable/disable reward modulation
- `agent.enableCuriosity(bool)`: Enable/disable curiosity

#### SimpleWorld
- `createSimpleWorld()`: Create a world instance
- `world.configure(width, height, visionWidth, visionHeight)`: Configure world
- `world.reset()`: Reset world state
- `world.update(timestep)`: Update world
- `world.applyMotorCommand(action, time)`: Apply action
- `world.getSensoryPercept()`: Get sensory percept
- `world.getAgentBody()`: Get agent body state

### Configuration

#### Config
- `createDefaultConfig()`: Create default configuration
- `config.set(key, value, source)`: Set configuration value
- `config.getOr(key, default)`: Get configuration with default
- `config.has(key)`: Check if key exists
- `config.saveToFile(filepath)`: Save to JSON file
- `config.loadFromFile(filepath)`: Load from JSON file

### Enums

#### NeuronType
- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

#### ActionType
- `MoveForward`: Move forward action
- `MoveBackward`: Move backward action
- `TurnLeft`: Turn left action
- `TurnRight`: Turn right action
- `Interact`: Interact action

#### DevelopmentalStage
- `Initial`: Initial development stage
- `CriticalPeriod`: Critical period
- `Maturation`: Maturation stage
- `Adult`: Adult stage

## Troubleshooting

### Common Issues and Solutions

| Issue | Solution |
|-------|----------|
| "My brain isn't doing anything" | Call `brain.initialize()` before using. Always reset between simulations. |
| "The agent isn't moving" | Ensure `world.applyMotorCommand()` is being called in the loop. |
| "Everything is 0" | Brains need time to "warm up" - try more steps. Ensure sensory input is connected. |
| "ImportError: No module named 'pynlm'" | Run `pip install -e .` to install the package. |
| "Segmentation fault" | Check that `initialize()` is called before `step()`. Ensure `reset()` is called before re-running. |

### Getting Help

1. **Documentation**: See `docs/ARCHITECTURE.md` for system architecture
2. **Examples**: Check the `examples/` directory for more usage patterns
3. **Community**: Visit [NLM GitHub](https://github.com/nlm-project/nlm) for discussions
4. **Issues**: Report bugs at [NLM Issues](https://github.com/nlm-project/nlm/issues)

## Contributing

We welcome contributions to improve the NLM Python API! Here are some ways to get involved:

1. **Report Issues**: Create issues for bugs or feature requests
2. **Fix Bugs**: Fix identified issues in the codebase
3. **Add Examples**: Share new usage examples and patterns
4. **Improve Documentation**: Enhance documentation and comments
5. **Performance Improvements**: Optimize Python bindings and performance
6. **New Features**: Suggest and implement new API features

### Guidelines

- Follow PEP 8 style guidelines
- Write comprehensive docstrings and comments
- Add tests for new functionality
- Maintain backward compatibility
- Use meaningful variable and function names

## License

MIT License - See `LICENSE` file for details.

## Acknowledgements

This improved documentation and API enhances the original NLM Python bindings with:

- Better error handling and validation
- More comprehensive examples and tutorials
- Advanced usage patterns and integrations
- Performance monitoring tools
- Command-line interface
- Multi-agent system support
- Reinforcement learning environment integration
- Enhanced configuration options

The NLM Python API now provides a more robust, user-friendly, and feature-rich interface for working with the Neural Learning Machine framework.