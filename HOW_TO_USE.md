# NLM (Neural Learning Machine) - Improved Documentation

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with enhanced Python bindings. This document provides comprehensive guidance on using NLM, including information about recent improvements and enhanced features.

**Key Improvements:**
- Enhanced error handling with custom exceptions
- Pythonic convenience methods and validation
- Memory safety with smart pointers
- Advanced utility functions
- Improved documentation and examples
- Better type safety and parameter validation

## Quick Start

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Simulation with Enhanced Features

```python
import pynlm

def run_enhanced_simulation(num_steps=1000):
    """Run a complete agent simulation with all new features."""
    
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
    
    # 5. Initialize agent with world (includes validation)
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run enhanced simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Validate percept before processing
        if not percept.isValid():
            print(f"Warning: Invalid percept at step {step}")
            continue
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation with validation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        if std::isnan(reward):
            reward = 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print enhanced progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Action: {motor_cmd}")
    
    return brain, world, agent

# Run the enhanced simulation
brain, world, agent = run_enhanced_simulation(1000)
```

## Enhanced AgentBrain Interface

### Key Improvements

The AgentBrain class now includes:

1. **Memory Safety**: Uses `std::shared_ptr` instead of raw pointers
2. **Enhanced Validation**: Comprehensive input and state validation
3. **Debug Utilities**: Access to internal neuron groups and statistics
4. **Better Error Handling**: Custom exceptions for different error types
5. **Convenience Methods**: Simplified access to common operations

### Pythonic Interface

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization with validation
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)  # Automatically validates world and brain

# Check if agent is functional
if agent.isValid() and agent.is_functional():
    print("Agent is ready for simulation!")

# Access internal neuron groups for debugging
motor_neurons = agent.getMotorForwardNeurons()
sensory_neurons = agent.getSensoryVisionNeurons()
print(f"Motor neurons: {len(motor_neurons)}")
print(f"Sensory neurons: {len(sensory_neurons)}")

# Get comprehensive statistics
stats = agent.get_brain_stats()
print(f"Statistics: {stats}")
```

## Enhanced SensoryPercept Class

The SensoryPercept class now provides advanced utility functions:

### Advanced Analysis

```python
import pynlm

# Create a percept
percept = pynlm.SensoryPercept()

# Enhanced processing with validation
if percept.validate():
    # Statistical analysis
    print(f"Vision mean: {percept.getVisionMean():.3f}")
    print(f"Vision std dev: {percept.getVisionStdDev():.3f}")
    print(f"Touch mean: {percept.getTouchMean():.3f}")
    print(f"Max signal: {percept.getMaxSignal():.3f}")
    print(f"Min signal: {percept.getMinSignal():.3f}")
    
    # High activation detection
    high_vision = percept.getHighActivationVision(0.8)
    print(f"High activation vision pixels: {len(high_vision)}")
    
    # Normalization
    normalized = percept.getNormalizedVision()
    print(f"Normalized vision sample: {normalized[:5]}")
    
    # Clone for modification
    cloned = percept.clone()
    print(f"Cloned successfully: {cloned.toString()}")
else:
    print("Percept validation failed!")
```

## Error Handling and Validation

### Custom Exceptions

NLM now provides custom exceptions for better error handling:

```python
import pynlm

try:
    # This will raise a PyConfigurationError if config is null
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # This will raise a PyBrainError if agent is invalid
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Try to decode command with invalid agent
    if not agent.isValid():
        raise PyBrainError("Agent is not valid for operation")
        
except pynlm.PyConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.PyBrainError as e:
    print(f"Brain error: {e}")
except pynlm.PyWorldError as e:
    print(f"World error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Input Validation

All major operations now include comprehensive validation:

1. **Configuration Validation**: Checks for null pointers and invalid parameters
2. **Brain Validation**: Validates brain state before operations
3. **World Validation**: Validates world parameters before simulation
4. **Percept Validation**: Validates sensory data for integrity
5. **Action Validation**: Validates actions before execution

## Advanced Usage Patterns

### Pattern 1: Enhanced Simulation with Statistics

```python
def run_statistics_simulation(steps=1000):
    """Run simulation with comprehensive statistics collection."""
    
    import pynlm
    import time
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Statistics collection
    stats = {
        'steps': steps,
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'dev_stages': [],
        'action_counts': {}
    }
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        if percept.isValid():
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
            
            # Collect statistics
            stats['firing_rates'].append(brain.getFiringNeuronCount())
            stats['curiosity_levels'].append(agent.getCuriosityLevel())
            stats['novelty_levels'].append(agent.getNoveltyLevel())
            stats['dev_stages'].append(brain.getDevelopmentalStage())
            stats['action_counts'][str(action)] = stats['action_counts'].get(str(action), 0) + 1
    
    end_time = time.time()
    stats['duration'] = end_time - start_time
    
    return stats

# Run simulation
results = run_statistics_simulation(500)
print(f"Simulation completed in {results['duration']:.2f} seconds")
print(f"Average firing rate: {sum(results['firing_rates'])/len(results['firing_rates']):.1f}")
```

### Pattern 2: Agent Training with Enhanced Features

```python
class EnhancedAgentTrainer:
    """Enhanced agent trainer with advanced features and validation."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.agent = None
        self.world = None
        self.training_stats = {}
    
    def setup_training(self, world_width=20, world_height=20):
        """Setup the training environment with enhanced validation."""
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=world_width, height=world_height, 
                           visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(world_width/2, world_height/2)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Enhanced initialization with validation
        if not self.agent.isValid():
            raise pynlm.PyBrainError("Cannot initialize invalid agent")
        
        self.agent.initialize(self.world)
        
        # Enable advanced learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        return True
    
    def train_episode(self, steps=100, reward_function=None):
        """Run a single training episode with enhanced features."""
        if not self.setup_training():
            return None
        
        episode_stats = {
            'steps': steps,
            'total_reward': 0.0,
            'action_sequence': [],
            'novelty_sum': 0.0,
            'curiosity_sum': 0.0
        }
        
        for step in range(steps):
            # World update
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Validate and process
            if not percept.isValid():
                continue
                
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            episode_stats['action_sequence'].append(action)
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward if provided
            reward = reward_function(step, action) if reward_function else 0.1
            predicted_reward = 0.0
            
            # Apply reward modulation with validation
            if not std::isnan(reward):
                self.agent.applyRewardModulation(reward, predicted_reward)
                episode_stats['total_reward'] += reward
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect statistics
            episode_stats['novelty_sum'] += self.agent.getNoveltyLevel()
            episode_stats['curiosity_sum'] += self.agent.getCuriosityLevel()
        
        return episode_stats
    
    def evaluate_agent(self, episodes=10):
        """Evaluate agent performance with detailed statistics."""
        all_stats = []
        
        for episode in range(episodes):
            stats = self.train_episode(steps=200)
            if stats:
                all_stats.append(stats)
        
        if not all_stats:
            return None
        
        # Calculate averages
        avg_reward = sum(s['total_reward'] for s in all_stats) / len(all_stats)
        avg_novelty = sum(s['novelty_sum'] for s in all_stats) / len(all_stats)
        avg_curiosity = sum(s['curiosity_sum'] for s in all_stats) / len(all_stats)
        
        return {
            'episodes': episodes,
            'average_reward': avg_reward,
            'average_novelty': avg_novelty,
            'average_curiosity': avg_curiosity,
            'action_distribution': self._calculate_action_distribution(all_stats)
        }
    
    def _calculate_action_distribution(self, all_stats):
        """Calculate action distribution from all episodes."""
        action_counts = {}
        total_actions = 0
        
        for stats in all_stats:
            for action in stats['action_sequence']:
                action_str = str(action)
                action_counts[action_str] = action_counts.get(action_str, 0) + 1
                total_actions += 1
        
        # Convert to percentages
        distribution = {}
        for action, count in action_counts.items():
            distribution[action] = (count / total_actions) * 100
        
        return distribution

# Use the enhanced trainer
trainer = EnhancedAgentTrainer()
results = trainer.evaluate_agent(episodes=5)
print(f"Training results: {results}")
```

## Installation and Building

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: ninja or make

### Build Instructions

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Installation

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

## API Reference

### Configuration

- `createDefaultConfig()`: Create default configuration
- `config.set(key, value)`: Set configuration value
- `config.get(key, default)`: Get configuration value
- `config.has(key)`: Check if key exists

### Core Classes

- `createBrain(config)`: Create neural brain
- `brain.initialize()`: Initialize brain
- `brain.step(step)`: Perform simulation step
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

### Agent Interface

- `createAgentBrain(brain)`: Create agent brain interface
- `agent.initialize(world)`: Initialize agent with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command
- `agent.applyRewardModulation(reward, predicted)`: Apply reward modulation
- `agent.updateDevelopment(timestep)`: Update development

### World Interface

- `createSimpleWorld()`: Create simple world
- `world.configure(width, height, visionWidth, visionHeight)`: Configure world
- `world.update(timestep)`: Update world
- `world.applyMotorCommand(cmd, time)`: Apply motor command
- `world.getSensoryPercept()`: Get sensory percept

### Enhanced Utilities

- `SensoryPercept.clone()`: Clone percept
- `SensoryPercept.validate()`: Validate percept
- `AgentBrain.isValid()`: Check if agent is valid
- `AgentBrain.get_brain_stats()`: Get brain statistics
- `AgentBrain.set_sensor_novelty_decay()`: Set novelty decay

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if all learning features are enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent has valid sensory input

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if percept validation is passing

**"Null pointer errors"**
- Always check if objects are valid before using them
- Use `isValid()` methods before operations
- Ensure proper initialization order

**"Performance issues"**
- Use smaller brain sizes for testing
- Consider disabling non-essential features
- Profile with `pynlm.example_simple_simulation()`

## Examples

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Advanced Example with All Features

```python
import pynlm

def advanced_example():
    """Demonstrate all advanced features."""
    
    # Setup with validation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run with validation and statistics
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        if percept.isValid():
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
    
    # Get final statistics
    stats = agent.get_brain_stats()
    print(f"Final stats: {stats}")
    
    return brain, world, agent

# Run advanced example
brain, world, agent = advanced_example()
```

## Version Information

- **NLM Version**: 1.0.0-enhanced
- **Python Bindings Version**: 1.0.0-enhanced
- **Last Updated**: October 2024
- **Build Type**: Release with enhanced features

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](README.md) - Project overview and basic usage

## Contact and Support

For questions, issues, or feature requests:
- Check the documentation first
- Report issues in the GitHub repository
- Join the discussion forums for community support

This documentation covers NLM version 1.0.0 with all enhanced features. For information about earlier versions, please refer to the original documentation.
