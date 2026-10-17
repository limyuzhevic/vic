# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the NLM neural simulation framework. This documentation covers both the original bindings and the enhanced API.

## Installation

### Dependencies

- Python 3.8 or higher
- NumPy (optional, recommended for advanced usage)
- Pybind11 (included in the build)

### Installation from Source

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON=ON

# Build the project
make -j4

# Install Python module
sudo make install
```

### Installation via pip (if available)

```bash
pip install pynlm
```

## Quick Start

### Basic Usage with Original API

```python
import pynlm

# Create default configuration
config = pynlm.Config()

# Create brain with configuration
brain = pynlm.Brain(config)

# Initialize brain
brain.initialize()

# Create world
world = pynlm.SimpleWorld()
world.configure(20.0, 20.0, 16, 16)

# Create agent brain
agent_brain = pynlm.AgentBrain(brain)

# Main simulation loop
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Decode motor command
    action_type = agent_brain.decodeMotorCommand()
    
    # Apply action
    action_result = world.applyMotorCommand(action_type)
    
    # Update development
    agent_brain.updateDevelopment(0.01)
    
    # Update world
    world.update(0.01)
    
    # Print statistics
    if step % 10 == 0:
        print(f"Step {step}: Neurons={brain.total_neurons()}, "
              f"Firing rate={brain.firing_rate():.2f}")
```

### Enhanced API Usage

```python
import enhanced_pynlm as nlm

# Create enhanced configuration with defaults
config = nlm.Config.create_default()

# Set custom values
config.set("random_seed", 12345)
config.set("max_energy", 200.0)
config.set("vision_width", 32)
config.set("vision_height", 32)

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Create world with custom settings
world = nlm.World()
world.configure(width=50.0, height=50.0, vision_width=64, vision_height=64)

# Add interesting objects to the world
world.add_object(10.0, 10.0, nlm.WorldObjectType.Resource, 50.0)
world.add_object(40.0, 30.0, nlm.WorldObjectType.Hazard, -30.0)

# Run simulation with statistics collection
stats = []
for step in range(200):
    # Get observations
    percept = world.getSensoryPercept()
    body_state = world.get_agent_body()
    
    # Create agent brain
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Process sensory input
    agent_brain.process_sensory_input(percept)
    
    # Choose action
    action_type = agent_brain.decode_motor_command()
    
    # Apply action
    result = world.apply_action(action_type)
    
    # Apply reward
    agent_brain.apply_reward(result["reward"])
    
    # Update development
    agent_brain.update_development(0.01)
    
    # Update world
    world.update(0.01)
    
    # Collect statistics every 20 steps
    if step % 20 == 0:
        step_stats = {
            "step": step,
            "time": world.simulation_time(),
            "neurons": brain.total_neurons(),
            "firing_rate": brain.firing_rate(),
            "energy": body_state["energy"],
            "reward": result["reward"],
            "neuromodulation": agent_brain.neuromodulation_level(),
            "curiosity": agent_brain.curiosity_level(),
            "developmental_stage": agent_brain.developmental_stage()
        }
        stats.append(step_stats)

# Print results
print("\nSimulation completed!")
for stat in stats:
    print(f"Step {stat['step']}: "
          f"Firing rate={stat['firing_rate']:.3f}, "
          f"Energy={stat['energy']:.1f}, "
          f"Reward={stat['reward']:.1f}, "
          f"Stage={stat['developmental_stage']}")
```

## Configuration

### Configuration Types

The Config class supports the following value types:

- Integers (int, int64_t)
- Floating-point numbers (double)
- Booleans (bool)
- Strings (std::string)
- Integer arrays (std::vector<int>)
- Floating-point arrays (std::vector<double>)
- String arrays (std::vector<std::string>)

### Creating and Managing Configuration

```python
import nlm

# Create empty configuration
config = nlm.Config()

# Set values
config.set("simulation_steps", 1000)
config.set("learning_rate", 0.01)
config.set("max_energy", 100.0)
config.set("enable_plasticity", True)
config.set("neuron_types", [0, 1, 0, 1])  # Excitatory, Inhibitory, etc.

# Get values with defaults
steps = config.get("simulation_steps", 500)
learning_rate = config.get("learning_rate", 0.001)

# Check if key exists
if config.has("enable_plasticity"):
    print("Plasticity is enabled:", config.get("enable_plasticity", False))

# Get all configuration keys
keys = config.getKeys()
print("Configuration keys:", keys)

# Save configuration
config.saveToFile("my_config.txt")

# Load configuration
config.loadFromFile("my_config.txt")

# Load from command line arguments
config.loadFromArgs(["--simulation_steps", "2000", "--learning_rate", "0.05"])
```

### Advanced Configuration Management

```python
import enhanced_pynlm as nlm

# Create configuration with defaults
config = nlm.Config.create_default()

# Modify defaults
config.set("random_seed", 42)
config.set("development_enabled", True)
config.set("curiosity_enabled", True)
config.set("prediction_enabled", True)

# Load from file (supports JSON, YAML, or simple key=value format)
config.load_file("experiment_config.json")

# Load from Python argument list
config.load_args(["--random_seed", "12345", "--max_energy", "200"])

# Get summary
print("Configuration summary:")
print(config.summary())

# Clear and start fresh
config.clear()
```

## Brain Simulation

### Brain Operations

```python
import nlm

# Create brain with configuration
config = nlm.Config()
brain = nlm.Brain(config)

# Initialize the brain (must be called before simulation)
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Failed to initialize brain")

# Get brain statistics
print(f"Total neurons: {brain.total_neurons()}")
print(f"Total synapses: {brain.total_synapses()}")
print(f"Active neurons: {brain.active_neurons()}")
print(f"Firing neurons: {brain.firing_neurons()}")
print(f"Firing rate: {brain.firing_rate():.3f}")
print(f"Excitation/Inhibition ratio: {brain.excitation_inhibition_ratio():.3f}")
print(f"Total spikes: {brain.total_spikes()}")

# Add neural regions
region_id = brain.add_region("sensory_cortex")
region_id2 = brain.add_region("motor_cortex")

# Get region information
region = brain.getRegion(region_id)

# Set developmental stage
brain.set_developmental_stage("Adult")
stage = brain.get_developmental_stage()

# Save and load brain state
brain.save("brain_checkpoint.nml")
if brain.load("saved_brain.nml"):
    print("Brain state loaded successfully")

# Perform simulation steps
for step in range(100):
    brain.step(step, 0.01)  # step with time
    if step % 10 == 0:
        print(f"Step {step}: Firing rate = {brain.firing_rate():.3f}")
```

### Enhanced Brain API

```python
import enhanced_pynlm as nlm

# Create brain with enhanced configuration
config = nlm.Config.create_default()
config.set("random_seed", 12345)
config.set("development_enabled", True)

brain = nlm.Brain(config)
brain.initialize()

# Advanced statistics collection
print("=== Brain Statistics ===")
print(f"Total neurons: {brain.total_neurons()}")
print(f"Total synapses: {brain.total_synapses()}")
print(f"Firing rate: {brain.firing_rate():.3f}")
print(f"Developmental stage: {brain.developmental_stage()}")

# Simulation with detailed monitoring
for step in range(200):
    brain.step(step)
    
    if step % 20 == 0:
        print(f"\nStep {step}:")
        print(f"  Time: {step * 0.01:.2f}s")
        print(f"  Firing rate: {brain.firing_rate():.3f}")
        print(f"  Active neurons: {brain.active_neurons()}")
        print(f"  Total spikes: {brain.total_spikes()}")

# Save brain state
brain.save("enhanced_brain_state.json")
```

## World Simulation

### Basic World Operations

```python
import nlm

# Create and configure world
world = nlm.SimpleWorld()
world.configure(width=20.0, height=20.0, vision_width=16, vision_height=16)

# Set agent starting position
world.set_agent_start(10.0, 10.0)

# Reset world to initial state
world.reset()

# Main simulation loop
for timestep in range(100):
    # Get current sensory percept
    percept = world.getSensoryPercept()
    
    # Get agent body state
    body = world.getAgentBody()
    
    # Check for objects at agent position
    agent_x, agent_y = body.x, body.y
    if world.is_valid_position(agent_x, agent_y):
        obj = world.getObjectAt(agent_x, agent_y)
        if obj:
            print(f"Agent at ({agent_x:.1f}, {agent_y:.1f}): "
                  f"Type={obj.type}, Value={obj.value}")
    
    # Update world state
    world.update(0.01)
    
    # Print progress
    if timestep % 20 == 0:
        print(f"Timestep {timestep}: Position=({body.x:.1f}, {body.y:.1f}), "
              f"Energy={body.energy:.1f}, Time={world.simulation_time():.2f}s")
```

### Enhanced World API

```python
import enhanced_pynlm as nlm

# Create world with enhanced configuration
world = nlm.World()
world.configure(width=50.0, height=50.0, vision_width=64, vision_height=64)

# Set random seed for reproducible environments
world.set_random_seed(42)

# Reset and add interesting objects
world.reset()
world.add_object(5.0, 5.0, nlm.WorldObjectType.Resource, 50.0, 0.8)
world.add_object(15.0, 10.0, nlm.WorldObjectType.Hazard, -30.0, 0.5)
world.add_object(30.0, 35.0, nlm.WorldObjectType.Resource, 75.0, 1.0)
world.add_object(40.0, 25.0, nlm.WorldObjectType.Marker, 0.0, 0.3)

# Set agent start
world.set_agent_start(25.0, 25.0)

# Run simulation with detailed observation
for step in range(100):
    # Get complete observation
    percept = world.get_sensory_percept()
    body = world.get_agent_body()
    
    # Create action space for agent
    actions = [nlm.MotorCommand.MoveForward, nlm.MotorCommand.TurnLeft, 
               nlm.MotorCommand.TurnRight, nlm.MotorCommand.Interact, 
               nlm.MotorCommand.Wait]
    
    # Process observation (simplified - would need proper implementation)
    # agent_brain.process_sensory_input(percept)
    
    # Choose and apply action
    action = nlm.MotorCommand.Wait  # Would be decided by brain
    result = world.apply_action(action)
    
    # Print observation
    if step % 10 == 0:
        print(f"\nStep {step}:")
        print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  Agent orientation: {body.orientation:.1f} rad")
        print(f"  Agent energy: {body.energy:.1f}")
        print(f"  Action result: Reward={result['reward']}, "
              f"Success={result['success']}")
        if 'vision' in percept:
            print(f"  Vision size: {len(percept['vision'])} pixels")
    
    # Update world
    world.update(0.01)
```

## AgentBrain Interface

### Basic AgentBrain Usage

```python
import nlm

# Create brain and agent brain
config = nlm.Config()
brain = nlm.Brain(config)
agent_brain = nlm.AgentBrain(brain)

# Initialize agent brain with world
world = nlm.SimpleWorld()
agent_brain.initialize(world)

# Get sensory and motor dimensions
input_size = agent_brain.getSensoryInputSize()
output_size = agent_brain.getMotorOutputSize()
print(f"Expected sensory input size: {input_size}")
print(f"Expected motor output size: {output_size}")

# Process sensory input (simplified example)
# percept = world.getSensoryPercept()
# agent_brain.processSensoryInput(percept)

# Get motor command (would be computed by brain)
# motor_command = agent_brain.decodeMotorCommand()
# print(f"Motor command: {motor_command}")

# Apply reward
agent_brain.applyRewardModulation(reward=10.0, predicted_reward=5.0)

# Update development
agent_brain.updateDevelopment(0.01)

# Get neuromodulation levels
print(f"Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent_brain.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent_brain.getPredictionError():.3f}")
print(f"Developmental stage: {agent_brain.getDevelopmentalStage()}")

# Configure neuromodulation systems
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Reset agent
agent_brain.reset()
```

### Enhanced AgentBrain API

```python
import enhanced_pynlm as nlm

# Create brain and enhanced agent brain
config = nlm.Config.create_default()
config.set("random_seed", 12345)
config.set("development_enabled", True)
config.set("reward_modulation_enabled", True)
config.set("curiosity_enabled", True)

brain = nlm.Brain(config)
brain.initialize()

# Create world and agent brain
world = nlm.World()
world.configure(width=30.0, height=30.0, vision_width=32, vision_height=32)
world.reset()

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

# Configure all neuromodulation systems
agent_brain.enable_reward_modulation(True)
agent_brain.enable_structural_plasticity(True)
agent_brain.enable_development(True)
agent_brain.enable_curiosity(True)

# Run episode with learning
for episode in range(5):
    print(f"\n=== Episode {episode + 1} ===")
    
    # Get initial state
    percept = world.get_sensory_percept()
    body = world.get_agent_body()
    
    # Reset agent and world
    agent_brain.reset()
    world.reset()
    
    # Episode loop
    for step in range(50):
        # Process sensory input (would need proper implementation)
        # agent_brain.process_sensory_input(percept)
        
        # Get action from brain
        action = agent_brain.decode_motor_command()
        
        # Apply action
        result = world.apply_action(action)
        
        # Apply reward
        agent_brain.apply_reward(result["reward"])
        
        # Update development
        agent_brain.update_development(0.01)
        
        # Update world
        world.update(0.01)
        
        # Print episode progress
        if step % 10 == 0:
            print(f"  Step {step}: "
                  f"Pos=({body.x:.1f},{body.y:.1f}), "
                  f"Energy={body.energy:.1f}, "
                  f"Reward={result['reward']:.1f}, "
                  f"Curiosity={agent_brain.curiosity_level():.3f}, "
                  f"Novelty={agent_brain.novelty_level():.3f}")
        
        # Get next observation (simplified)
        # percept = world.get_sensory_percept()
    
    print(f"Episode completed: Final curiosity={agent_brain.curiosity_level():.3f}")
```

## Experiment Management

### Running Experiments

```python
import enhanced_pynlm as nlm
import json
from datetime import datetime

def run_learning_experiment(config_overrides=None):
    """Run a complete learning experiment with enhanced tracking"""
    
    # Create experiment configuration
    config = nlm.Config.create_default()
    
    # Apply overrides
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create simulation objects
    brain = nlm.Brain(config)
    world = nlm.World()
    world.configure(width=20.0, height=20.0, vision_width=16, vision_height=16)
    world.reset()
    
    agent_brain = nlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable all advanced features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    agent_brain.enable_curiosity(True)
    
    # Run experiment
    experiment_data = {
        "start_time": datetime.now().isoformat(),
        "config": dict(zip(config.keys(), [config.get(k) for k in config.keys()])),
        "episodes": []
    }
    
    for episode in range(10):
        episode_data = {
            "episode": episode,
            "steps": [],
            "total_reward": 0.0
        }
        
        # Reset for new episode
        agent_brain.reset()
        world.reset()
        
        # Run episode
        for step in range(100):
            # Get observations
            percept = world.get_sensory_percept()
            body = world.get_agent_body()
            
            # Process with agent brain (simplified)
            # agent_brain.process_sensory_input(percept)
            
            # Get action
            action = agent_brain.decode_motor_command()
            
            # Apply action
            result = world.apply_action(action)
            
            # Apply reward
            agent_brain.apply_reward(result["reward"])
            
            # Update development
            agent_brain.update_development(0.01)
            
            # Update world
            world.update(0.01)
            
            # Record step data
            step_data = {
                "step": step,
                "time": world.simulation_time(),
                "position": {"x": body.x, "y": body.y},
                "energy": body.energy,
                "action": action,
                "reward": result["reward"],
                "neuromodulation": agent_brain.neuromodulation_level(),
                "curiosity": agent_brain.curiosity_level(),
                "developmental_stage": agent_brain.developmental_stage()
            }
            episode_data["steps"].append(step_data)
            episode_data["total_reward"] += result["reward"]
        
        experiment_data["episodes"].append(episode_data)
    
    # Save experiment data
    experiment_data["end_time"] = datetime.now().isoformat()
    with open(f"experiment_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json", "w") as f:
        json.dump(experiment_data, f, indent=2)
    
    print(f"Experiment completed! Data saved to file.")
    print(f"Total episodes: {len(experiment_data['episodes'])}")
    print(f"Average total reward per episode: {sum(e['total_reward'] for e in experiment_data['episodes']) / len(experiment_data['episodes']):.2f}")
    
    return experiment_data

# Run experiment with custom configuration
experiment_config = {
    "random_seed": 12345,
    "max_energy": 150.0,
    "development_enabled": True,
    "reward_modulation_enabled": True,
    "curiosity_enabled": True
}

run_learning_experiment(experiment_config)
```

## Advanced Features

### Neural Network Construction

```python
import enhanced_pynlm as nlm

# Create brain with specific configuration
config = nlm.Config.create_default()
config.set("random_seed", 42)

brain = nlm.Brain(config)

# Create neural regions
print("Creating neural regions...")
sensory_region = brain.add_region("sensory_cortex")
motor_region = brain.add_region("motor_cortex")
memory_region = brain.add_region("working_memory")
cognition_region = brain.add_region("cognition")

print(f"Created {brain.getRegionCount()} regions")
print(f"Region IDs: {brain.getRegionIds()}")

# Initialize brain
brain.initialize()

# Get brain statistics
print("\nBrain Statistics:")
print(f"  Total neurons: {brain.total_neurons()}")
print(f"  Total synapses: {brain.total_synapses()}")
print(f"  Firing rate: {brain.firing_rate():.3f}")
print(f"  Developmental stage: {brain.developmental_stage()}")

# Save brain state
brain.save("neural_network.json")
print("\nNeural network saved to 'neural_network.json'")
```

### World Environment Configuration

```python
import enhanced_pynlm as nlm

# Create world with specific configuration
world = nlm.World()
world.configure(width=100.0, height=100.0, vision_width=64, vision_height=64)

# Set random seed for reproducible environments
world.set_random_seed(12345)

# Add various objects to the environment
world.reset()

# Add resources (positive reward)
world.add_object(10.0, 10.0, nlm.WorldObjectType.Resource, 50.0, 0.8)
world.add_object(30.0, 20.0, nlm.WorldObjectType.Resource, 75.0, 1.0)
world.add_object(50.0, 30.0, nlm.WorldObjectType.Resource, 25.0, 0.5)

# Add hazards (negative reward)
world.add_object(70.0, 15.0, nlm.WorldObjectType.Hazard, -30.0, 0.6)
world.add_object(90.0, 40.0, nlm.WorldObjectType.Hazard, -50.0, 0.7)

# Add walls (boundaries)
world.add_object(0.0, 0.0, nlm.WorldObjectType.Wall, 0.0, 2.0)  # Corner
world.add_object(50.0, 0.0, nlm.WorldObjectType.Wall, 0.0, 2.0)

# Add markers (special navigation points)
world.add_object(25.0, 25.0, nlm.WorldObjectType.Marker, 0.0, 0.3)
world.add_object(75.0, 75.0, nlm.WorldObjectType.Marker, 0.0, 0.3)

# Set agent starting position
world.set_agent_start(5.0, 5.0)

print("World environment configured:")
print(f"  Dimensions: {world.width()} x {world.height()}")
print(f"  Vision: {world.width()} x {world.height()}")
print(f"  Number of objects: {world.getObjectCount()}")

# Print object locations
print("\nObject locations:")
for obj in world.get_objects():
    type_name = {0: "Empty", 1: "Resource", 2: "Hazard", 
                 3: "Wall", 4: "Marker"}[int(obj.type)]
    print(f"  ({obj.x:.1f}, {obj.y:.1f}): {type_name} (value={obj.value})")
```

## Performance Optimization

### Configuration for Performance

```python
import enhanced_pynlm as nlm

# Create high-performance configuration
config = nlm.Config.create_default()

# Performance optimizations
config.set("random_seed", 12345)
config.set("max_energy", 200.0)  # Higher energy for longer simulations
config.set("vision_width", 32)   # Smaller vision for faster processing
config.set("vision_height", 32)
config.set("development_enabled", True)
config.set("prediction_enabled", True)   # Enable prediction for efficiency
config.set("planning_enabled", True)     # Enable planning for decision making
config.set("attention_enabled", True)    # Enable attention mechanisms

# Create brain with performance configuration
brain = nlm.Brain(config)
brain.initialize()

print("High-performance brain configuration created")
print(f"  Vision resolution: {32}x{32}")
print(f"  Maximum energy: {config.get('max_energy')}")
print(f"  Random seed: {config.get('random_seed')}")
```

### Memory Management

```python
import enhanced_pynlm as nlm

# Configure memory-efficient simulation
config = nlm.Config.create_default()

# Memory optimizations
config.set("random_seed", 42)
config.set("max_energy", 100.0)  # Lower energy for shorter simulations
config.set("development_enabled", False)  # Disable development to save memory
config.set("prediction_enabled", True)    # Enable prediction with less memory
config.set("planning_enabled", False)     # Disable planning to save memory
config.set("attention_enabled", False)    # Disable attention to save memory

# Create world with memory optimizations
world = nlm.World()
world.configure(width=15.0, height=15.0, vision_width=8, vision_height=8)  # Small world
world.set_random_seed(config.get("random_seed"))

print("Memory-optimized configuration created")
print(f"  World size: {world.width()}x{world.height()}")
print(f"  Vision resolution: {8}x{8}")
print(f"  Development enabled: {config.get('development_enabled')}")
```

## Error Handling and Debugging

### Exception Handling

```python
import enhanced_pynlm as nlm
import traceback

# Create configuration
config = nlm.Config.create_default()

# Try to create brain with invalid configuration
try:
    # This would fail if required configuration values are missing
    config.set("invalid_key", "invalid_value")
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    print("Brain created successfully despite invalid config")
    
except Exception as e:
    print(f"Error creating brain: {e}")
    traceback.print_exc()

# Handle invalid action types
try:
    world = nlm.World()
    result = world.apply_action(999)  # Invalid action type
    
except Exception as e:
    print(f"Error applying action: {e}")
    traceback.print_exc()

# Handle configuration errors
try:
    config.load_file("nonexistent_file.txt")
    
except Exception as e:
    print(f"Error loading config file: {e}")
```

### Logging and Debugging

```python
import enhanced_pynlm as nlm

# Enable logging
nlm.enable_debug_logging(True)

# Create and run simulation with logging
config = nlm.Config.create_default()
config.set("random_seed", 42)

brain = nlm.Brain(config)
world = nlm.World()
world.configure(width=20.0, height=20.0, vision_width=16, vision_height=16)
world.reset()

# Run simulation with logging
for step in range(10):
    brain.step(step)
    world.update(0.01)
    
    if step % 2 == 0:
        # Log simulation state
        nlm.log_simulation_state({
            "step": step,
            "brain_neurons": brain.total_neurons(),
            "world_position": {"x": world.getAgentBody().x, "y": world.getAgentBody().y}
        })

# Get debug logs
logs = nlm.get_debug_logs()
print(f"Number of log entries: {len(logs)}")
for log_entry in logs[-5:]:  # Last 5 entries
    print(f"  {log_entry}")
```

## Migration Guide

### From Original API to Enhanced API

This guide helps you migrate from the original pynlm API to the enhanced enhanced_pynlm API.

#### 1. Basic Brain Creation

**Original:**
```python
import pynlm
config = pynlm.Config()
brain = pynlm.Brain(config)
```

**Enhanced:**
```python
import enhanced_pynlm as nlm
config = nlm.Config.create_default()
brain = nlm.Brain(config)
```

#### 2. Configuration Management

**Original:**
```python
config.set("key", "value")
value = config.get("key")
```

**Enhanced:**
```python
config.set("key", "value")
value = config.get("key", "default")
```

#### 3. Brain Statistics

**Original:**
```python
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

**Enhanced:**
```python
print(f"Neurons: {brain.total_neurons()}")
```

#### 4. World Configuration

**Original:**
```python
world = pynlm.SimpleWorld()
world.configure(20.0, 20.0, 16, 16)
```

**Enhanced:**
```python
world = nlm.World()
world.configure(width=20.0, height=20.0, vision_width=16, vision_height=16)
```

#### 5. AgentBrain

**Original:**
```python
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)
```

**Enhanced:**
```python
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)
```

#### 6. Action Application

**Original:**
```python
result = world.applyMotorCommand(action_type)
```

**Enhanced:**
```python
result = world.apply_action(action_type)
```

## Examples

### Example 1: Simple Simulation

```python
import enhanced_pynlm as nlm

# Create simple simulation
config = nlm.Config.create_default()
brain = nlm.Brain(config)
world = nlm.World()
world.configure(20.0, 20.0, 16, 16)
world.reset()

# Run simulation
for step in range(100):
    brain.step(step)
    world.update(0.01)
    
    if step % 20 == 0:
        print(f"Step {step}: Firing rate = {brain.firing_rate():.3f}")

print("Simulation completed!")
```

### Example 2: Learning with Curiosity

```python
import enhanced_pynlm as nlm

# Create learning simulation
config = nlm.Config.create_default()
config.set("curiosity_enabled", True)
config.set("reward_modulation_enabled", True)

brain = nlm.Brain(config)
world = nlm.World()
world.configure(30.0, 30.0, 32, 32)
world.reset()

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)
agent_brain.enable_curiosity(True)

# Run learning episode
for step in range(200):
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process input
    # agent_brain.process_sensory_input(percept)
    
    # Choose action
    action = agent_brain.decode_motor_command()
    
    # Apply action and reward
    result = world.apply_action(action)
    agent_brain.apply_reward(result["reward"])
    
    # Update development
    agent_brain.update_development(0.01)
    world.update(0.01)
    
    # Print curiosity-driven behavior
    if step % 20 == 0:
        curiosity = agent_brain.curiosity_level()
        print(f"Step {step}: Curiosity={curiosity:.3f}, "
              f"Energy={world.get_agent_body().energy:.1f}")
```

### Example 3: Complex Environment

```python
import enhanced_pynlm as nlm

# Create complex environment
world = nlm.World()
world.configure(width=50.0, height=50.0, vision_width=64, vision_height=64)
world.set_random_seed(12345)
world.reset()

# Add complex environment features
# Resources at different locations
world.add_object(10.0, 10.0, nlm.WorldObjectType.Resource, 50.0, 0.8)
world.add_object(30.0, 20.0, nlm.WorldObjectType.Resource, 25.0, 0.5)
world.add_object(40.0, 40.0, nlm.WorldObjectType.Resource, 75.0, 1.0)

# Hazards
world.add_object(70.0, 15.0, nlm.WorldObjectType.Hazard, -30.0, 0.6)
world.add_object(20.0, 35.0, nlm.WorldObjectType.Hazard, -50.0, 0.7)

# Navigation markers
world.add_object(25.0, 25.0, nlm.WorldObjectType.Marker, 0.0, 0.3)
world.add_object(60.0, 60.0, nlm.WorldObjectType.Marker, 0.0, 0.3)

# Walls
for x in [0.0, 50.0]:
    for y in range(0, 51, 5):
        world.add_object(x, y, nlm.WorldObjectType.Wall, 0.0, 0.2)

# Create brain and agent
config = nlm.Config.create_default()
brain = nlm.Brain(config)
brain.initialize()

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

# Run exploration
print("Starting exploration in complex environment...")
for step in range(100):
    # Get observations
    percept = world.get_sensory_percept()
    body = world.get_agent_body()
    
    # Process and act (simplified)
    action = agent_brain.decode_motor_command()
    result = world.apply_action(action)
    agent_brain.apply_reward(result["reward"])
    agent_brain.update_development(0.01)
    world.update(0.01)
    
    # Print exploration statistics
    if step % 10 == 0:
        print(f"Step {step}: "
              f"Pos=({body.x:.1f},{body.y:.1f}), "
              f"Energy={body.energy:.1f}, "
              f"Curiosity={agent_brain.curiosity_level():.3f}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Configuration file not found"

**Problem:** Config.loadFromFile() fails because file doesn't exist.

**Solution:**
```python
import os
if not os.path.exists("config.txt"):
    # Create default config
    config = nlm.Config.create_default()
    config.save_to_file("config.txt")
    config.load_from_file("config.txt")
else:
    config.load_from_file("config.txt")
```

#### Issue 2: "Brain not initialized"

**Problem:** Brain.initialize() returns false or wasn't called.

**Solution:**
```python
brain = nlm.Brain(config)
if not brain.initialize():
    print("Failed to initialize brain")
    # Check configuration
    print("Config summary:", config.summary())
    raise RuntimeError("Brain initialization failed")
```

#### Issue 3: "Invalid action type"

**Problem:** Applying invalid motor command.

**Solution:**
```python
# Use enum values
try:
    action = nlm.MotorCommand.MoveForward
    result = world.apply_action(action)
except ValueError:
    print(f"Invalid action type")
```

#### Issue 4: Memory usage too high

**Problem:** Simulation running out of memory.

**Solution:**
```python
# Optimize configuration
config = nlm.Config.create_default()
config.set("development_enabled", False)
config.set("prediction_enabled", False)
config.set("planning_enabled", False)
config.set("vision_width", 16)
config.set("vision_height", 16)
```

#### Issue 5: Non-reproducible results

**Problem:** Simulation produces different results each run.

**Solution:**
```python
config = nlm.Config.create_default()
config.set("random_seed", 12345)  # Fixed seed

world.set_random_seed(12345)  # Consistent world generation
brain = nlm.Brain(config)
```

### Getting Help

If you encounter issues:

1. **Check the documentation:** Review this documentation for common patterns
2. **Use debugging:** Enable debug logging and examine logs
3. **Reproduce the issue:** Create a minimal example that demonstrates the problem
4. **Check configuration:** Verify all required configuration values are set
5. **Review examples:** Look at the example code in this documentation
6. **File issues:** Check if your issue is already documented in the project

## License

This documentation is licensed under the MIT License.

For more information about the NLM project, visit: https://github.com/nlm-project/nlm

## Acknowledgements

This enhanced Python API builds upon the original pynlm bindings and incorporates numerous improvements for better usability and functionality.

Special thanks to the original NLM development team for creating this powerful neural simulation framework.
