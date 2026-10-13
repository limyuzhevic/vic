# NLM Python Usage Examples

This document provides comprehensive examples for using the NLM (Neural Learning Machine) Python bindings.

## Quick Start

### Basic Usage
```python
import pynlm

# Create a default configuration
config = pynlm.Config()

# Create a neural brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i, i * 0.001)
    
# Check learning
print(f"Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
```

### Advanced Usage with Configuration
```python
import pynlm

# Create and configure a brain with custom settings
config = pynlm.Config()
config["neuron_count"] = 1000
config["learning_rate"] = 0.01
config["random_seed"] = 42

brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation with sensory input
world = pynlm.create_simple_world()
world.configure(200, 200, 40, 40)

# Simulate agent behavior
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.initialize(world)

for step in range(1000):
    # Create sensory percept
    percept = pynlm.SensoryPercept()
    
    # Process sensory input
    agent_brain.process_sensory_input(percept)
    
    # Get motor action
    action = brain.produce_action()
    
    # Apply action in world
    world.applyMotorCommand(action, step * 0.001)
```

## Configuration Management

### Creating and Saving Configurations
```python
import pynlm
import json

# Create configuration from scratch
config = pynlm.Config()

# Set various configuration values
config["neuron_count"] = 500
config["learning_rate"] = 0.001
config["connection_probability"] = 0.15
config["enable_plasticity"] = True
config["enable_development"] = True
config["enable_curiosity"] = True

# Get configuration values
print(f"Neuron count: {config['neuron_count']}")
print(f"Learning rate: {config.get_or('learning_rate', 0.01)}")

# Check if key exists
print(f"Has random_seed: {'random_seed' in config}")

# Use default value if key doesn't exist
print(f"Max energy: {config.get_or('max_energy', 100.0)}")

# Save configuration to file
config.save_to_file("my_config.json")

# Load configuration from file
new_config = pynlm.Config()
new_config.load_from_file("my_config.json")

# Configuration validation
required_keys = ["neuron_count", "learning_rate", "connection_probability"]
type_hints = {"neuron_count": "int", "learning_rate": "float", "connection_probability": "float"}

if new_config.validate(required_keys, type_hints):
    print("Configuration is valid!")
else:
    print("Configuration has validation errors!")
```

### Configuration Operators
```python
config = pynlm.Config()

# Using operator syntax (Pythonic)
config["neuron_count"] = 100
config["learning_rate"] = 0.01
config["tags"] = ["exc", "inh", "mod"]

print(config["neuron_count"])  # Access using []

# Iterate over configuration keys
for key in config:
    print(f"{key}: {config[key]}")

# Items view for key-value pairs
for key, value in config.items():
    print(f"{key}: {value}")

# Update configuration from dictionary
config.update({
    "new_param": "value",
    "another_param": 42,
    "enabled": True
})

# Clear all configuration
config.clear()
```

## Neural Network Management

### Brain Operations
```python
import pynlm

# Create brain with default configuration
config = pynlm.Config()
brain = pynlm.create_brain(config)

# Initialize brain
brain.initialize()

# Get brain statistics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")

# Run simulation with different time steps
for step in range(100):
    brain.step(step, step * 0.001)
    
    # Get firing statistics every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: Firing rate = {brain.get_average_firing_rate():.2f}")

# Reset and run another simulation
brain.reset()
brain.initialize()
```

### Brain State Management
```python
import pynlm

# Save brain state
success = brain.save("brain_state.pkl")
if success:
    print("Brain state saved successfully!")

# Load brain state
if brain.load("brain_state.pkl"):
    print("Brain state loaded successfully!")
else:
    print("Failed to load brain state!")

# Add neural regions
region_id = brain.addRegion("sensory_cortex")
region_id = brain.addRegion("motor_cortex")
region_id = brain.addRegion("association_cortex")

# Get region information
print(f"Number of regions: {brain.get_region_count()}")
for region_id in brain.get_region_ids():
    print(f"Region ID: {region_id}")
```

## Sensorimotor Integration

### Working with Sensory Inputs
```python
import pynlm

# Create different types of sensory inputs
vision = pynlm.Vision()
vision.configure(640, 480, 3)

# Set vision data (simulated camera input)
vision_data = [0.1 * i for i in range(640 * 480 * 3)]
vision.setData(vision_data)

#audio = pynlm.Audio()
#audio.configure(44100, 1024)

# Set audio data
sample_rate = 44100
num_samples = 1024
audio_data = [0.5 * (i % 2) for i in range(num_samples)]
audio.setData(audio_data)

# Create internal signals (brain state monitoring)
internal = pynlm.InternalSignals()
internal.addSignal(0.8)
internal.addSignal(-0.3)
internal.addSignal(1.2)

# Get sensory percepts
percept = pynlm.SensoryPercept()
percept.setVision(vision)
# percept.setAudio(audio)
percept.setInternal(internal)

print(f"Vision width: {percept.get_vision_width()}")
print(f"Vision height: {percept.get_vision_height()}")
```

### Working with Motor Actions
```python
import pynlm

# Create motor actions
action = pynlm.Action(pynlm.ActionType.MoveForward)
action.setParameters([1.0, 0.5, 0.0])  # parameters: speed, rotation, intensity

print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
print(f"Action parameters: {action.getParameters()}")

# Clone action
action_copy = action.clone()

# Create action with custom type and parameters
look_action = pynlm.Action(pynlm.ActionType.Look, [30.0, 15.0])  # horizontal, vertical angles
```

## Agent Brain Integration

### Using AgentBrain
```python
import pynlm

# Create brain and world
config = pynlm.Config()
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world()

# Create agent brain
agent_brain = pynlm.create_agent_brain(brain)

# Initialize agent
agent_brain.initialize(world)

# Set agent starting position
world.setAgentStart(50, 50)

# Simulate agent behavior
for step in range(500):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Get motor action
    action = brain.produceAction()
    
    # Apply action in world
    world.applyMotorCommand(action, step * 0.001)
    
    # Update agent body
    body = world.getAgentBody()
    body.update()
    
    # Log progress periodically
    if step % 100 == 0:
        print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
              f"energy: {body.energy:.1f}")
```

## World Management

### World Configuration and Operations
```python
import pynlm

# Create and configure world
world = pynlm.create_simple_world()
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(12345)

print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Vision size: {world.getVisionWidth()} x {world.getVisionHeight()}")

# Add game objects
import pynlm

# Create resource object
resource = pynlm.WorldObject(30, 40, pynlm.WorldObjectType.Resource, 10.0, 0.5)
world.addObject(resource)

# Create hazard object
hazard = pynlm.WorldObject(70, 60, pynlm.WorldObjectType.Hazard, 5.0, 0.3)
world.addObject(hazard)

# Check valid positions
valid_positions = []
for x in range(0, 100, 5):
    for y in range(0, 100, 5):
        if world.isValidPosition(x, y):
            valid_positions.append((x, y))

print(f"Number of valid positions: {len(valid_positions)}")

# Get agent state
agent_body = world.getAgentBody()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
print(f"Agent orientation: {agent_body.orientation}")
print(f"Agent energy: {agent_body.energy}")

# Reset world
world.reset()
```

## Development and Plasticity

### Working with Development
```python
import pynlm

# Create brain and enable development
config = pynlm.Config()
brain = pynlm.create_brain(config)
brain.initialize()

# Enable developmental features
agent_brain = pynlm.create_agent_brain(brain)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Get developmental information
print(f"Current developmental stage: {agent_brain.get_developmental_stage()}")
print(f"Neuromodulation level: {agent_brain.get_neuromodulation_level()}")
print(f"Curiosity level: {agent_brain.get_curiosity_level()}")
print(f"Novelty level: {agent_brain.get_novelty_level()}")

# Update development
for step in range(100):
    agent_brain.updateDevelopment(0.001)
    
    if step % 20 == 0:
        print(f"Step {step}: Stage = {agent_brain.get_developmental_stage()}")
```

### Plasticity Management
```python
import pynlm

# Enable plasticity in agent brain
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)

# Apply reward modulation
agent_brain.applyRewardModulation(1.0, 0.8)  # reward, predicted_reward

# Get plasticity information
print(f"Reward modulation enabled: {agent_brain.is_reward_modulation_enabled()}")
print(f"Structural plasticity enabled: {agent_brain.is_structural_plasticity_enabled()}")
print(f"Current neuromodulation level: {agent_brain.get_neuromodulation_level()}")
```

## Error Handling and Validation

### NLMError Usage
```python
import pynlm

try:
    config = pynlm.Config()
    config.set("invalid_type", "not_an_int")
    
    # This will raise NLMError due to type mismatch
    count = config.get_with_validation("invalid_type", "int")
    print(f"Count: {count}")
    
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")

# File operation error handling
try:
    config = pynlm.Config()
    config.load_from_file("nonexistent.json")
except pynlm.NLMError as e:
    print(f"File loading error: {e}")
```

## Performance Tips

### Efficient Configuration Access
```python
import pynlm

config = pynlm.Config()

# Batch operations for better performance
batch_updates = {
    "neuron_count": 1000,
    "learning_rate": 0.001,
    "connection_probability": 0.15,
    "max_energy": 100.0
}
config.update(batch_updates)

# Use get_keys() once and iterate for multiple accesses
keys = config.get_keys()
for key in keys:
    value = config[key]
    # Process value...

# Clear configuration when done
config.clear()
```

## Complete Working Example

### Multi-Agent Simulation
```python
import pynlm

def create_agent(config, agent_id):
    """Create and initialize an agent."""
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent_brain = pynlm.create_agent_brain(brain)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    return agent_brain

def simulate_agents(num_agents, steps):
    """Run a multi-agent simulation."""
    config = pynlm.Config()
    config["neuron_count"] = 500
    config["learning_rate"] = 0.001
    
    agents = [create_agent(config, i) for i in range(num_agents)]
    world = pynlm.create_simple_world()
    world.configure(200, 200, 40, 40)
    
    for step in range(steps):
        # Each agent acts
        for i, agent_brain in enumerate(agents):
            # Create unique sensory input for each agent
            percept = pynlm.SensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Get action
            brain = agent_brain.getBrain()
            action = brain.produceAction()
            
            # Apply action (simplified)
            if action.getType() == pynlm.ActionType.MoveForward:
                # Move agent forward
                pass
        
        # Log progress
        if step % 100 == 0:
            avg_curiosity = sum(agent.get_curiosity_level() for agent in agents) / num_agents
            print(f"Step {step}: Average curiosity = {avg_curiosity:.3f}")
    
    return agents

# Run simulation
agents = simulate_agents(5, 1000)
print("Simulation completed!")
```

## Installation and Setup

### Installation
```bash
pip install pynlm
```

### Building from Source
```bash
# Build the native extensions
cmake -B build -S .
cmake --build build

# Install Python bindings
pip install -e .
```

### Requirements
- Python 3.8 or higher
- CMake 3.16 or higher
- pybind11
- numpy (optional, for array operations)

## Troubleshooting

### Common Issues

1. **Module not found**
   ```python
   # Install the package
   pip install pynlm
   ```

2. **Build errors**
   ```bash
   # Ensure you have CMake and pybind11 installed
   pip install pybind11 cmake
   ```

3. **Import errors**
   ```python
   # Try importing with verbose mode
   import pynlm
   import sys
   print(sys.path)
   ```

4. **Configuration file errors**
   ```python
   # Use try-catch for file operations
   try:
       config.load_from_file("config.json")
   except pynlm.NLMError as e:
       print(f"Error loading config: {e}")
   ```

### Debugging Tips
- Use logging to track program execution
- Check for type mismatches in configuration
- Validate configuration before use
- Use try-catch blocks for file operations
- Monitor memory usage for long simulations
```
