# NLM Usage Guide

**Comprehensive guide for using the NLM neural simulation framework**

## Quick Start

If you're new to NLM, start with [NLM - Easy Usage Guide for Beginners](../easy_usage.md) for a simple introduction.

This guide covers:
- Installation and setup
- Basic configuration
- Advanced usage patterns
- Common use cases

## Installation

### Prerequisites

NLM requires:
- C++ compiler with C++17 support
- Python 3.7+ with pybind11 bindings
- CMake 3.10+

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j4
```

### Python Installation

The Python bindings are built as part of the main project. After building:

```bash
# Verify Python bindings are available
python3 -c "import pynlm; print(pynlm.__version__)"
```

### Running Tests

```bash
# Run unit tests
./run_tests.sh
```

## Basic Setup

### Creating a Simple Brain

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Configuration:\n{brain.getConfig()}")
```

### Running a Silent Simulation

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
print("Running silent brain simulation...")
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total simulation completed: {brain.getTotalSpikeCount()} spikes")
```

### Creating a Complete Agent-World System

```python
import pynlm

# Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Setup agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems (optional)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Agent-world system ready for simulation")
```

## Basic Configuration

### Understanding Configuration

The NLM configuration system uses key-value pairs with dot notation for organization:

```python
# Examples of configuration keys
brain.neuron_count = 1000
brain.region[0].population[0].neuron_type = "Excitatory"
plasticity.stdp.enable = True
neuromodulation.dopamine.baseline = 0.5
development.stage_initial_plasticity = 1.0
```

### Common Configuration Categories

#### Brain Configuration
```python
# Create and configure brain
config = pynlm.createDefaultConfig()

# Set neuron count
config.set("brain.neuron_count", "2000")

# Configure regions
config.set("brain.region[0].name", "Sensory")
config.set("brain.region[0].population[0].neuron_type", "Sensory")
config.set("brain.region[1].name", "Motor")
config.set("brain.region[1].population[0].neuron_type", "Motor")
```

#### Plasticity Configuration
```python
# Enable plasticity mechanisms
config.set("plasticity.stdp.enable", "true")
config.set("plasticity.stdp.learning_rate", "0.01")
config.set("plasticity.hebbian.enable", "true")
config.set("plasticity.structural.enable", "true")
config.set("plasticity.structural.growth_rate", "0.001")
```

#### Neuromodulation Configuration
```python
# Configure neuromodulators
config.set("neuromodulation.dopamine.baseline", "0.5")
config.set("neuromodulation.curiosity.threshold", "0.1")
config.set("neuromodulation.novelty.decay_rate", "0.95")
```

#### Development Configuration
```python
# Configure developmental stages
config.set("development.stages.0.plasticity", "1.0")  # Initial
config.set("development.stages.1.plasticity", "0.5")  # CriticalPeriod
config.set("development.stages.2.plasticity", "0.3")  # Maturation
config.set("development.stages.3.plasticity", "0.2")  # Adult
config.set("development.stages.4.plasticity", "0.1")  # Aging
```

### Loading Configuration from Files

```python
import pynlm
import json

# Create default config and save as JSON
config = pynlm.createDefaultConfig()
config.saveToFile("my_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
success = config2.loadFromFile("my_config.json")

if success:
    print("Configuration loaded successfully")
    brain = pynlm.createBrain(config2)
else:
    print("Failed to load configuration")

# Create configuration from command line arguments
import sys
config3 = pynlm.createDefaultConfig()
success = config3.loadFromArgs(len(sys.argv), sys.argv)
```

## Advanced Usage Patterns

### Simulation Loop Pattern

```python
def run_simulation(brain, world, agent, num_steps):
    """Complete simulation loop pattern"""
    for step in range(num_steps):
        # 1. Update world environment
        world.update(0.1)  # 100ms timestep
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain what was perceived
        agent.processSensoryInput(percept)
        
        # 4. Brain processes information
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Apply learning signals
        if step % 10 == 0:  # Apply reward every 10 steps
            reward = calculate_reward(world, action)
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
        
        # Optional: Update development
        if step % 100 == 0:  # Update development every 100 steps
            agent.updateDevelopment(0.1)

def calculate_reward(world, action):
    """Example reward function"""
    agent_body = world.getAgentBody()
    if action.getType() == pynlm.ActionType.Eat:
        # Reward for eating
        return 1.0
    elif action.getType() == pynlm.ActionType.Interact:
        # Reward for interacting
        return 0.5
    else:
        # Small penalty for movement
        return -0.01
```

### Monitoring and Logging Pattern

```python
def run_monitored_simulation(brain, world, agent, num_steps):
    """Simulation with comprehensive monitoring"""
    stats = []
    
    for step in range(num_steps):
        # Update simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics
        if step % 10 == 0:
            stat = {
                'step': step,
                'time': world.getSimulationTime(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'developmental_stage': int(agent.getDevelopmentalStage()),
                'enabled_systems': agent.getEnablements()
            }
            stats.append(stat)
            print(f"Step {step}: {stat['firing_neurons']} neurons firing")
    
    return stats

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

stats = run_monitored_simulation(brain, world, agent, 1000)
```

### Developmental Simulation Pattern

```python
def run_developmental_simulation(brain, world, agent, total_steps):
    """Simulation that tracks developmental progression"""
    developmental_stats = {}
    
    for step in range(total_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record developmental stage changes
        current_stage = agent.getDevelopmentalStage()
        if current_stage != developmental_stats.get('last_stage', current_stage):
            developmental_stats[f'stage_change_step_{current_stage}'] = step
            developmental_stats['last_stage'] = current_stage
            
            # Log stage change
            stage_names = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
            print(f"Development stage changed to: {stage_names[current_stage]} at step {step}")
        
        # Update development periodically
        if step % 100 == 0:
            agent.updateDevelopment(0.1)
    
    return developmental_stats
```

### Learning and Adaptation Pattern

```python
def run_learning_simulation(brain, world, agent, num_steps):
    """Simulation with learning enabled"""
    exploration_count = 0
    reward_history = []
    
    for step in range(num_steps):
        # Enable exploration based on curiosity
        if agent.getCuriosityLevel() > 0.5 and exploration_count < 5:
            # Force exploration action
            action = pynlm.createAction(pynlm.ActionType.TurnLeft)
            world.applyMotorCommand(action, world.getSimulationTime())
            exploration_count += 1
            print(f"Exploration step {step}: curiosity = {agent.getCuriosityLevel():.3f}")
        
        # Normal simulation loop
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_task_reward(world, agent, step)
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        reward_history.append(reward)
        
        # Record learning metrics
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            neuromod = agent.getNeuromodulationLevel()
            
            print(f"Step {step}: curiosity={curiosity:.3f}, novelty={novelty:.3f}, neuromod={neurom:.3f}")
    
    return reward_history

# Helper function for task rewards
def calculate_task_reward(world, agent, step):
    """Example task-specific reward function"""
    # Simple maze navigation reward
    action = agent.decodeMotorCommand()
    agent_body = world.getAgentBody()
    
    # Positive reward for reaching goal
    if world.isGoalPosition(agent_body.x, agent_body.y):
        return 10.0
    
    # Negative reward for hitting walls
    if world.isHazardPosition(agent_body.x, agent_body.y):
        return -5.0
    
    # Small positive reward for exploration
    if agent.getNoveltyLevel() > 0.3:
        return 0.1
    
    # Small negative reward for inactivity
    if agent.getCuriosityLevel() < 0.1:
        return -0.01
    
    # Default small negative reward
    return -0.001
```

## Common Use Cases and Examples

### 1. Simple Maze Navigation

```python
import pynlm

def run_maze_agent():
    """Agent navigating a simple maze"""
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=5, visionHeight=5)
    world.reset()
    
    # Add obstacles
    for x in range(5, 10):
        world.addObject(pynlm.WorldObject(x, 7, pynlm.WorldObjectType.Wall))
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if reached goal
        agent_body = world.getAgentBody()
        if world.isGoalPosition(agent_body.x, agent_body.y):
            print(f"Goal reached at step {step}!")
            break
    
    return brain, world, agent

# Run maze navigation
brain, world, agent = run_maze_agent()
```

### 2. Resource Collection Task

```python
import pynlm

def run_resource_collection():
    """Agent collecting resources in environment"""
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    # Add resources
    resources = []
    for i in range(5):
        x = 2 + i * 1.5
        y = 2
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(resource)
        resources.append((x, y))
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    total_resources = 0
    
    # Run simulation
    for step in range(300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for resource collection
        if result.success and action.getType() == pynlm.ActionType.Eat:
            agent_body = world.getAgentBody()
            # Check if we're on a resource
            for i, (rx, ry) in enumerate(resources):
                if abs(agent_body.x - rx) < 0.5 and abs(agent_body.y - ry) < 0.5:
                    total_resources += 1
                    print(f"Resource collected at step {step}!")
                    break
    
    print(f"Total resources collected: {total_resources}")
    return brain, world, agent

brain, world, agent = run_resource_collection()
```

### 3. Pattern Learning Task

```python
import pynlm

def run_pattern_learning():
    """Agent learning visual patterns"""
    # Create brain with larger capacity for learning
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with pattern sequence
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Create agent with all learning enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Pattern sequence (simple visual patterns)
    patterns = [
        [1, 1, 1, 0, 0, 0],  # Horizontal line
        [1, 0, 1, 0, 1, 0],  # Vertical line
        [1, 0, 0, 1, 0, 0],  # Diagonal
        [0, 1, 1, 1, 0, 0],  # Another horizontal
    ]
    
    pattern_index = 0
    pattern_correct_count = 0
    total_steps = 0
    
    # Run simulation
    for step in range(500):
        # Present pattern based on step
        current_pattern = patterns[pattern_index % len(patterns)]
        
        # Create visual percept based on pattern
        percept = pynlm.Vision(6, 1, 1)  # 6x1 grayscale image
        for i, pixel in enumerate(current_pattern):
            percept.setData([pixel] * 6 * 1)  # Fill with pattern
        
        # Update world with pattern
        world.setSensoryPercept(percept)
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reinforcement based on curiosity
        novelty = agent.getNoveltyLevel()
        agent.applyRewardModulation(novelty, 0.0)
        
        # Track pattern learning (very simplified)
        if step % 100 == 0 and step > 0:
            pattern_index += 1
            print(f"Pattern progression: {pattern_index}")
        
        total_steps += 1
    
    print(f"Pattern learning completed. Total steps: {total_steps}")
    return brain, world, agent

brain, world, agent = run_pattern_learning()
```

### 4. Performance Benchmark

```python
import pynlm
import time

def benchmark_performance():
    """Benchmark brain simulation performance"""
    config = pynlm.createDefaultConfig()
    
    # Test with different brain sizes
    sizes = [100, 500, 1000]
    results = {}
    
    for size in sizes:
        print(f"\nBenchmarking with {size} neurons...")
        
        # Create configuration for specific size
        config.set("brain.neuron_count", str(size))
        
        # Setup brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Measure simulation time
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Calculate performance metrics
        spikes_per_second = brain.getTotalSpikeCount() / (sim_time / 100)  # Per 100 steps
        neurons_per_second = size / sim_time
        
        results[size] = {
            'init_time': init_time,
            'sim_time': sim_time,
            'spikes_per_second': spikes_per_second,
            'neurons_per_second': neurons_per_second,
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        print(f"  Initialization: {init_time:.4f}s")
        print(f"  Simulation: {sim_time:.4f}s")
        print(f"  Performance: {neurons_per_second:.0f} neurons/sec")
        print(f"  Activity: {spikes_per_second:.0f} spikes/sec")
    
    return results

# Run benchmark
performance_results = benchmark_performance()
```

## Configuration Files

### Example Configuration Files

#### Basic Configuration (`basic_config.json`)
```json
{
    "brain": {
        "neuron_count": 1000,
        "region": [
            {
                "name": "Sensory",
                "population": [
                    {
                        "neuron_type": "Sensory",
                        "count": 200,
                        "excitation": 1.0
                    }
                ]
            },
            {
                "name": "Motor",
                "population": [
                    {
                        "neuron_type": "Motor",
                        "count": 200,
                        "excitation": 1.0
                    }
                ]
            }
        ]
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        }
    }
}
```

#### Advanced Configuration (`advanced_config.json`)
```json
{
    "brain": {
        "neuron_count": 5000,
        "region": [
            {
                "name": "Sensory",
                "population": [
                    {
                        "neuron_type": "Sensory",
                        "count": 1000,
                        "excitation": 1.0
                    }
                ]
            },
            {
                "name": "WorkingMemory",
                "population": [
                    {
                        "neuron_type": "Excitatory",
                        "count": 800,
                        "excitation": 1.0
                    }
                ]
            },
            {
                "name": "Motor",
                "population": [
                    {
                        "neuron_type": "Motor",
                        "count": 1000,
                        "excitation": 1.0
                    }
                ]
            }
        ]
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        },
        "structural": {
            "enable": true,
            "growth_rate": 0.001,
            "pruning_rate": 0.0005
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.5,
            "learning_rate": 0.02
        },
        "curiosity": {
            "threshold": 0.1,
            "decay_rate": 0.95
        },
        "novelty": {
            "decay_rate": 0.98
        }
    },
    "development": {
        "stages": [
            {"plasticity": 1.0},
            {"plasticity": 0.5},
            {"plasticity": 0.3},
            {"plasticity": 0.2},
            {"plasticity": 0.1}
        ]
    }
}
```

### Loading Configuration

```python
import pynlm
import json

# Save example configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "2000")
config.set("plasticity.stdp.enable", "true")
config.saveToFile("example_config.json")

# Load and use configuration
loaded_config = pynlm.createDefaultConfig()
if loaded_config.loadFromFile("example_config.json"):
    brain = pynlm.createBrain(loaded_config)
    brain.initialize()
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem:** Brain appears inactive with no firing neurons.

**Solutions:**
1. Verify `brain.initialize()` was called
2. Check if brain has neurons configured
3. Ensure sensory input is being provided
4. Try running more simulation steps
5. Check configuration parameters

**Debug code:**
```python
# Debug brain initialization and activity
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "100")
brain = pynlm.createBrain(config)
print(f"Config before init: {config.summary()}")

brain.initialize()
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### "The agent isn't moving"

**Problem:** Agent doesn't produce motor commands.

**Solutions:**
1. Verify `agent.initialize(world)` was called
2. Check if agent has motor neurons
3. Ensure agent's brain is receiving sensory input
4. Check agent enablements (reward modulation, etc.)
5. Verify action decoding is working

**Debug code:**
```python
# Debug agent movement
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check agent configuration
print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
print(f"Enabled systems: {agent.getEnablements()}")

# Test simulation
for step in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Step {step}: Action = {action.getType()}, Position = ({agent_body.x}, {agent_body.y})")
```

#### "Everything is 0 or NaN"

**Problem:** All values are zero or NaN (invalid).

**Solutions:**
1. Brains need time to "warm up" - try more steps
2. Ensure neurons have inputs to fire - check sensory connections
3. Verify configuration values are valid numbers
4. Check for division by zero in custom code
5. Ensure random seed is set for deterministic behavior

**Debug code:**
```python
# Debug value issues
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "50")  # Start small
config.set("plasticity.stdp.learning_rate", "0.01")  # Valid learning rate
config.set("neuromodulation.dopamine.baseline", "0.5")  # Valid baseline

brain = pynlm.createBrain(config)
brain.initialize()

# Warm up the brain
for step in range(50):
    brain.step(step)
    stats = brain.getStatistics()
    print(f"Step {step}: {stats}")
    
    # Check for invalid values
    for key, value in stats.items():
        if value != value:  # NaN check
            print(f"  WARNING: {key} is NaN")
```

#### "Configuration loading fails"

**Problem:** Configuration file fails to load.

**Solutions:**
1. Verify file exists and is readable
2. Check JSON syntax in configuration file
3. Ensure required fields are present
4. Check file permissions
5. Try loading with default configuration first

**Debug code:**
```python
import pynlm
import os

config_path = "my_config.json"

if not os.path.exists(config_path):
    print(f"Error: Configuration file {config_path} not found")
    # Create basic config as fallback
    config = pynlm.createDefaultConfig()
    config.saveToFile(config_path)
    print(f"Created basic config at {config_path}")

config = pynlm.createDefaultConfig()
if not config.loadFromFile(config_path):
    print(f"Error: Failed to load configuration from {config_path}")
    print("Using default configuration")
    config = pynlm.createDefaultConfig()
else:
    print(f"Successfully loaded configuration from {config_path}")
```

### Performance Issues

#### "Simulation is too slow"

**Solutions:**
1. Reduce brain size for debugging
2. Disable unnecessary plasticity mechanisms
3. Reduce simulation frequency
4. Check for performance bottlenecks in custom code
5. Consider using smaller timesteps

#### "Memory usage is high"

**Solutions:**
1. Reduce brain and neuron counts
2. Disable episodic memory if not needed
3. Clear unused references
4. Monitor memory usage with system tools

## Advanced Configuration Techniques

### Dynamic Configuration

```python
import pynlm

def dynamic_config_simulation():
    """Simulation with dynamic configuration changes"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Start with small brain
    config.set("brain.neuron_count", "100")
    
    for step in range(200):
        brain.step(step)
        
        # Dynamically adjust brain size at certain steps
        if step == 50:
            config.set("brain.neuron_count", "500")
            # Note: neuron count changes take effect on next initialization
            # For runtime changes, use specialized brain APIs
        
        if step == 100:
            config.set("brain.neuron_count", "1000")
    
    return brain

brain = dynamic_config_simulation()
```

### Configuration Validation

```python
def validate_config(config):
    """Validate configuration before use"""
    errors = []
    
    # Check required fields
    if not config.has("brain.neuron_count"):
        errors.append("Missing brain.neuron_count")
    
    # Check value ranges
    neuron_count = config.getInt("brain.neuron_count")
    if neuron_count < 10 or neuron_count > 100000:
        errors.append(f"Invalid neuron_count: {neuron_count}")
    
    # Check plasticity settings
    if config.getBool("plasticity.stdp.enable"):
        learning_rate = config.getFloat("plasticity.stdp.learning_rate", 0.0)
        if learning_rate <= 0 or learning_rate > 1:
            errors.append(f"Invalid STDP learning rate: {learning_rate}")
    
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    print("Configuration is valid")
    return True

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", "1000")
config.set("plasticity.stdp.enable", "true")
config.set("plasticity.stdp.learning_rate", "0.01")

validate_config(config)
```

## Summary

This Usage Guide provides comprehensive documentation for using the NLM framework:

1. **Installation**: Building and setting up NLM
2. **Basic Setup**: Creating brains and agents
3. **Configuration**: Understanding and managing configuration
4. **Advanced Patterns**: Common simulation patterns
5. **Use Cases**: Practical examples and implementations
6. **Troubleshooting**: Common issues and solutions
7. **Advanced Techniques**: Dynamic configuration and validation

For more detailed information, refer to:
- [API Reference](API_REFERENCE.md) - Complete API documentation
- [Tutorial](TUTORIAL.md) - Step-by-step tutorials
- [Examples](EXAMPLES.md) - Complete example scripts
- [Architecture](ARCHITECTURE.md) - System architecture overview