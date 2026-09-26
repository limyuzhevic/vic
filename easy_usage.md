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

## New Pythonic Convenience Methods

NLM's Python bindings now include **factory methods** and **convenience functions** to make Python development more intuitive:

```python
import pynlm as nlm

# Factory methods for common actions
action = nlm.ActionType.MoveForward.create_action()
action = nlm.ActionType.TurnLeft.create_action(45.0)  # With parameters

# Config convenience methods
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Sensory data from arrays
vision = nlm.Vision.from_array(vision_array, width, height, channels)
audio = nlm.Audio.from_array(audio_samples, sample_rate)
internal = nlm.InternalSignals.from_dict({"reward": 0.5, "curiosity": 0.3})

# Percept helpers
percept = nlm.SensoryPercept.from_dict(data_dict)
percept.set_all_like(other_percept)
percept.has_vision()
percept.has_audio()
percept.has_internal()

# World creation helpers
world = nlm.createSimpleWorld()
world.add_resource(5.0, 3.0, value=2.0, radius=1.0)
world.add_hazard(8.0, 7.0, value=1.5, radius=0.8)
world.add_wall(10.0, 10.0, radius=0.5)
world.add_marker(15.0, 15.0, value=100.0)
agent = world.create_agent(10.0, 10.0, orientation=45.0)

# Agent brain convenience
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)
agent_brain.step_percept(percept)      # Single sensory step
agent_brain.step_and_decode(percept)   # Process + decode
agent_brain.get_system_state()         # Get agent status
agent_brain.enable_all()               # Enable all subsystems
agent_brain.disable_all()              # Disable all subsystems
```

---

## Updated Basic Usage Examples

### Example 1: Simplest Brain (with new convenience)

```python
import pynlm as nlm

# One line to create a virtual brain with convenience config
brain = nlm.createBrain(nlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (with new world helpers)

```python
import pynlm as nlm
import numpy as np

# Create everything with convenience methods
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Add world objects using new convenience methods
world.add_resource(3.0, 3.0, value=2.0, radius=0.8)      # Food source
world.add_hazard(7.0, 7.0, value=1.5, radius=0.6)       # Hazard zone
world.add_wall(5.0, 5.0, radius=0.5)                     # Wall
world.add_marker(8.0, 8.0, value=100.0)                  # Goal marker

agent = world.create_agent(5.0, 5.0, orientation=45.0)   # Agent with orientation
agent = nlm.createAgentBrain(brain)                       # Agent brain
agent.initialize(world)

# Run for 50 steps with new convenience methods
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

### Example 3: Complete Agent with Configuration Helpers

```python
def create_advanced_agent(config=None):
    """Create an advanced agent using configuration helpers."""
    
    # Use configuration helpers if provided
    if config is None:
        config = nlm.createDefaultConfig()
        # Apply convenience configuration
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("neuromod.dopamine.scale", 1.2)
    
    # Create brain and agent
    brain = nlm.createBrain(config)
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    
    # Create and configure world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Add world objects using convenience methods
    create_research_environment(world)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True, exploration_strength=0.8)
    agent.enableDevelopment(True)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent
    }

def create_research_environment(world):
    """Create a research environment with resource farm and hazards."""
    # Add research station
    world.add_marker(10.0, 10.0, value=500.0, radius=2.0)
    
    # Add resource farm
    for x in [3.0, 7.0, 11.0, 15.0]:
        for y in [3.0, 7.0, 11.0, 15.0]:
            if (x + y) % 2 == 0:
                world.add_resource(x, y, value=2.0 + (x + y) / 10.0, radius=0.8)
            else:
                world.add_hazard(x, y, value=0.5 + (x + y) / 20.0, radius=0.6)
    
    # Add escape route
    for x in [1.0, 2.0, 18.0, 19.0]:
        for y in [5.0, 10.0, 15.0]:
            world.add_resource(x, y, value=10.0, radius=0.5)

# Usage example
agent_setup = create_advanced_agent()
world = agent_setup['world']
brain = agent_setup['brain']
agent = agent_setup['agent']

print("Advanced agent created with research environment!")
```

---

## Config Class Improvements

The `Config` class now has a **dictionary-like interface** with enhanced convenience methods for better Python development:

### Dictionary-like Access

```python
import pynlm as nlm

# Create configuration
config = nlm.createDefaultConfig()

# Dictionary-like interface
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Check if key exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all keys
keys = config.getKeys()

# Clear and reset
config.clear()
config.set("brain.neuron_count", 2000)

# Save/load from files
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")

# Summary for debugging
print(config.summary())
```

### Convenience Getters

```python
# Type-safe getters with defaults
neuron_count = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
dopamine_scale = config.get("neuromod.dopamine.scale", 1.0)

# Nested access
def get_brain_setting(key, default=None):
    return config.get(f"brain.{key}", default)

def get_neuromod_setting(key, default=None):
    return config.get(f"neuromod.{key}.enable", default)

# Configuration validation
config.validate([
    ("brain.neuron_count", lambda x: x > 0 and x <= 10000),
    ("brain.synapse_density", lambda x: 0.0 <= x <= 1.0),
    ("neuromod.dopamine.scale", lambda x: x >= 0.0),
])
```

### Enhanced Error Handling

```python
# Configuration with error handling
try:
    config.set("brain.neuron_count", -100)
    print("ERROR: Neuron count must be positive!")
except ValueError as e:
    print(f"Validation error: {e}")

# Safe key access
def safe_get(key, default=None):
    try:
        return config.get(key, default)
    except KeyError:
        return default

# Batch updates
updates = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.005,
    "neuromod.dopamine.scale": 1.2,
}

for key, value in updates.items():
    config.set(key, value)
```

---

## Action Class Improvements

The `Action` class now has **factory methods for common actions**, **convenience methods**, and **serialization** support:

### Factory Methods for Common Actions

```python
import pynlm as nlm

# Create common actions using factory methods
action = nlm.ActionType.MoveForward.create_action()
action = nlm.ActionType.TurnLeft.create_action(45.0)  # With rotation
action = nlm.ActionType.Interact.create_action()
action = nlm.ActionType.Eat.create_action(5.0)        # With value
action = nlm.ActionType.Grow.create_action(0.1)       # With rate

# Custom action with parameters
custom_action = nlm.ActionType.Custom.create_action({
    "speed": 2.0,
    "direction": 90.0,
    "duration": 1.0
})
```

### Convenience Methods

```python
# Type conversion
move_action = nlm.ActionType.MoveForward.create_action()
action_dict = move_action.to_dict()  # Serialize to dictionary

action_from_dict = nlm.Action.from_dict(action_dict)  # Deserialize

# Action comparison
action1 = nlm.ActionType.MoveForward.create_action()
action2 = nlm.ActionType.MoveForward.create_action()
action3 = nlm.ActionType.TurnLeft.create_action(90.0)

print(action1 == action2)  # True
print(action1 == action3)  # False

# Action properties and metadata
action = nlm.ActionType.Eat.create_action(3.0)
print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
print(f"Action parameters: {action.getParameters()}")

# Cloning
action_copy = action.clone()
action_copy.setParameters({"value": 5.0})
```

### Serialization and Cloning

```python
# Serialization to different formats
action = nlm.ActionType.MoveForward.create_action()

# To dictionary (JSON serializable)
action_dict = action.to_dict()
import json
json_str = json.dumps(action_dict)

# From dictionary
action_loaded = nlm.Action.from_dict(action_dict)

# Clone with modifications
base_action = nlm.ActionType.TurnLeft.create_action(45.0)
adapted_action = base_action.clone()
adapted_action.setParameters({"rotation": 90.0})

# Batch action processing
actions = [
    nlm.ActionType.MoveForward.create_action(),
    nlm.ActionType.TurnLeft.create_action(30.0),
    nlm.ActionType.Eat.create_action(2.0),
    nlm.ActionType.Interact.create_action(),
]

for action in actions:
    print(f"Processing: {action.getName()} with params: {action.getParameters()}")
```

### Action Validation

```python
# Validate action parameters before use
def validate_action(action):
    params = action.getParameters()
    if action.getType() == nlm.ActionType.Eat:
        if "value" not in params or params["value"] < 0:
            raise ValueError("Eat action requires positive value parameter")
    elif action.getType() == nlm.ActionType.TurnLeft:
        if "rotation" not in params or not (-360 <= params["rotation"] <= 360):
            raise ValueError("Invalid rotation angle")

# Safe action creation
def create_safe_action(action_type, **params):
    try:
        action = action_type.create_action(params)
        validate_action(action)
        return action
    except Exception as e:
        print(f"Failed to create {action_type}: {e}")
        return None
```

---

## Sensory Data Improvements

The sensory input classes now include **array-like interfaces** and **factory methods** for easier data manipulation:

### Vision.from_array()

```python
import pynlm as nlm
import numpy as np

# Create vision data from numpy array
vision_array = np.random.rand(84, 84, 3).astype(np.float32)
vision = nlm.Vision.from_array(vision_array, 84, 84, 3)

# Access vision data
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()

# Get vision data as Python list
vision_data = vision.getData()

# Set vision data from different sources
vision.setData(list(vision_array.flatten()))

# Common vision operations
if vision.has_vision():
    normalized_vision = vision.get_normalized()  # Normalized to [0, 1]
    gray_vision = vision.to_grayscale()           # Convert to grayscale
    enhanced_vision = vision.apply_filter('blur')  # Apply image processing
```

### Audio.from_array()

```python
import pynlm as nlm
import numpy as np

# Create audio data from numpy array
audio_samples = np.sin(2 * np.pi * 440 * np.linspace(0, 1, 44100)).astype(np.float32)
audio = nlm.Audio.from_array(audio_samples, 44100)

# Access audio properties
num_samples = audio.getNumSamples()
sample_rate = audio.getSampleRate()

# Audio processing
if audio.has_audio():
    processed_audio = audio.apply_filter('lowpass', cutoff=1000)
    normalized_audio = audio.normalize()  # Normalize to [-1, 1]
    segments = audio.split_into_segments(duration=1.0)  # Split into 1-second segments

# Create empty audio with specific properties
empty_audio = nlm.Audio(sample_rate=22050, num_samples=22050)
```

### InternalSignals.from_dict()

```python
import pynlm as nlm

# Create internal signals from dictionary
internal_dict = {
    "reward": 0.8,
    "curiosity": 0.5,
    "novelty": 0.3,
    "prediction_error": 0.2,
    "confidence": 0.9,
    "excitement": 0.6,
    "frustration": 0.1,
    "satisfaction": 0.7,
    "surprise": 0.4,
    "boredom": 0.2,
}

internal = nlm.InternalSignals.from_dict(internal_dict)

# Access and manipulate signals
if internal.has_internal():
    reward = internal.get("reward", 0.0)
    curiosity = internal.get("curiosity", 0.0)
    confidence = internal.get("confidence", 0.0)

# Add or update signals
internal.addSignal("reward", 0.9)
internal.addSignal("new_signal", 0.5)

# Clear all signals
internal.clearSignals()

# Signal operations
if internal.has_signal("reward"):
    scaled_reward = internal.get("reward") * 2.0  # Scale reward
    internal.set("reward", scaled_reward)
```

### Sensory Data Conversion

```python
# Convert between different sensory representations
vision = nlm.Vision.from_array(np.random.rand(64, 64, 3).astype(np.float32), 64, 64, 3)
audio = nlm.Audio.from_array(np.random.randn(22050).astype(np.float32), 22050)

# Create unified percept with multiple sensors
from pynlm import SensoryPercept

percept = SensoryPercept()
percept.set_vision_like(vision.getData(), vision.getWidth(), vision.getHeight(), vision.getChannels())
percept.set_audio_like(audio.getData(), audio.getSampleRate())
percept.add_internal_like("test_signal", 0.5)

# Convert percept to dictionary for serialization
percept_dict = percept.as_dict()
```

### Sensory Data Validation

```python
# Validate sensory data before use
def validate_vision_data(vision_data, width, height, channels):
    if len(vision_data) != width * height * channels:
        raise ValueError(f"Vision data size mismatch. Expected {width * height * channels}, got {len(vision_data)}")
    if not all(0.0 <= x <= 1.0 for x in vision_data):
        raise ValueError("Vision data values should be in range [0, 1]")

def validate_audio_data(audio_data, sample_rate):
    if not audio_data:
        raise ValueError("Audio data cannot be empty")
    if sample_rate <= 0:
        raise ValueError("Sample rate must be positive")

# Safe sensory data creation
try:
    vision = nlm.Vision.from_array(np.random.rand(100, 100, 3).astype(np.float32), 100, 100, 3)
except ValueError as e:
    print(f"Vision data error: {e}")

try:
    audio = nlm.Audio.from_array(np.random.randn(44100).astype(np.float32), 44100)
except ValueError as e:
    print(f"Audio data error: {e}")
```

---

## SensoryPercept Improvements

The `SensoryPercept` class now provides **comprehensive methods** for working with sensory data, including checking for different sensory modalities:

### as_dict() and Serialization

```python
import pynlm as nlm
from pynlm import SensoryPercept

# Create a percept from world data
percept = world.getSensoryPercept()

# Convert to dictionary for serialization
percept_dict = percept.as_dict()
print(f"Vision size: {percept_dict['vision_width']}x{percept_dict['vision_height']}")
print(f"Audio samples: {len(percept_dict['audio'])}")

# Create new percept from dictionary
new_percept = SensoryPercept.from_dict(percept_dict)

# Save/load to files
import json
with open("percept.json", "w") as f:
    json.dump(percept_dict, f)

with open("percept.json", "r") as f:
    loaded_dict = json.load(f)

loaded_percept = SensoryPercept.from_dict(loaded_dict)
```

### set_all_like() Method

```python
# Copy all sensory data from another percept
source_percept = world.getSensoryPercept()
target_percept = SensoryPercept()

target_percept.set_all_like(source_percept)
print(f"Copied vision: {target_percept.has_vision()}")
print(f"Copied audio: {target_percept.has_audio()}")
print(f"Copied internal: {target_percept.has_internal()}")

# Update only specific sensory modalities
vision_data = np.random.rand(84, 84, 3).astype(np.float32)
target_percept.set_vision_like(vision_data, 84, 84, 3)
```

### has_vision(), has_audio(), has_internal()

```python
# Check what sensory data is available
percept = world.getSensoryPercept()

if percept.has_vision():
    vision_data = percept.get_vision()
    width = percept.get_vision_width()
    height = percept.get_vision_height()
    print(f"Vision data: {width}x{height}")

if percept.has_audio():
    audio_data = percept.get_audio()
    sample_rate = percept.get_audio_sample_rate()
    print(f"Audio data: {len(audio_data)} samples at {sample_rate} Hz")

if percept.has_internal():
    internal_data = percept.get_internal()
    print(f"Internal signals: {len(internal_data)} values")

# Conditional processing
if percept.has_vision() and percept.has_audio():
    combined_data = {
        'vision': percept.get_vision(),
        'audio': percept.get_audio(),
        'timestamp': percept.get_timestamp()
    }
    print("Processing combined vision and audio data")
```

### Percept Comparison and Merging

```python
# Compare two percepts for differences
def compare_percepts(percept1, percept2):
    differences = {}
    
    if percept1.has_vision() != percept2.has_vision():
        differences['vision'] = 'vision presence mismatch'
    elif percept1.has_vision() and percept2.has_vision():
        if percept1.get_vision() != percept2.get_vision():
            differences['vision'] = 'vision data mismatch'
    
    if percept1.has_audio() != percept2.has_audio():
        differences['audio'] = 'audio presence mismatch'
    elif percept1.has_audio() and percept2.has_audio():
        if percept1.get_audio() != percept2.get_audio():
            differences['audio'] = 'audio data mismatch'
    
    if percept1.has_internal() != percept2.has_internal():
        differences['internal'] = 'internal presence mismatch'
    elif percept1.has_internal() and percept2.has_internal():
        if percept1.get_internal() != percept2.get_internal():
            differences['internal'] = 'internal data mismatch'
    
    return differences

# Merge multiple percepts
percept1 = world.getSensoryPercept()
percept2 = SensoryPercept()

# Add some data to percept2
vision_data = np.random.rand(64, 64, 3).astype(np.float32)
percept2.set_vision_like(vision_data, 64, 64, 3)

# Merge percepts
merged_percept = SensoryPercept()
merged_percept.set_all_like(percept1)  # Copy all from percept1
if percept2.has_vision():
    merged_percept.set_vision_like(percept2.get_vision(),
                                 percept2.get_vision_width(),
                                 percept2.get_vision_height(),
                                 percept2.get_vision_channels())
```

### Percept Validation and Error Handling

```python
# Validate percept data
def validate_percept(percept):
    errors = []
    
    if percept.has_vision():
        vision_data = percept.get_vision()
        if not vision_data:
            errors.append("Vision data is empty")
    
    if percept.has_audio():
        audio_data = percept.get_audio()
        if not audio_data:
            errors.append("Audio data is empty")
    
    if percept.has_internal():
        internal_data = percept.get_internal()
        if not internal_data:
            errors.append("Internal data is empty")
    
    return errors

# Safe percept creation
def create_safe_percept(source):
    try:
        target = SensoryPercept()
        target.set_all_like(source)
        errors = validate_percept(target)
        if errors:
            print(f"Percept validation warnings: {errors}")
        return target
    except Exception as e:
        print(f"Failed to create percept: {e}")
        return None
```

### Advanced Percept Operations

```python
# Create specialized percepts
full_percept = SensoryPercept.from_dict({
    'vision': list(np.random.rand(84, 84, 3).flatten()),
    'vision_width': 84,
    'vision_height': 84,
    'audio': list(np.random.randn(22050)),
    'timestamp': 0.0,
})

# Extract specific data
if full_percept.has_vision():
    vision_slice = full_percept.get_vision_region(40, 40, 20, 20)
    centered_vision = full_percept.get_vision_centered(42, 42)

# Process percept with multiple sensors
if full_percept.has_vision() and full_percept.has_audio():
    # Create combined feature vector
    vision_features = full_percept.extract_vision_features()
    audio_features = full_percept.extract_audio_features()
    combined_features = vision_features + audio_features
    
    # Create neural input
    neural_input = full_percept.to_neural_input(combined_features)
```

---

## SimpleWorld Improvements

The `SimpleWorld` class now provides **convenience methods for common operations**, **world configuration helpers**, and **agent management**:

### create_agent()

```python
import pynlm as nlm

# Create agent at specific position
world = nlm.createSimpleWorld()

# Basic agent creation
agent = world.create_agent(5.0, 3.0)  # At coordinates (5.0, 3.0)
agent = world.create_agent(10.0, 10.0, orientation=45.0)  # With rotation
agent = world.create_agent(0.0, 0.0, orientation=90.0)     # At origin with heading

# Agent with custom properties
agent = world.create_agent(
    x=5.0, 
    y=5.0, 
    orientation=0.0,
    brain=brain,
    max_energy=100.0,
    max_curiosity=0.8
)
```

### add_resource() and add_hazard()

```python
# Add different types of world objects
world.add_resource(1.0, 1.0, value=5.0, radius=0.5)    # High-value resource
world.add_resource(3.0, 3.0, value=0.5, radius=1.0)    # Low-value, large resource
world.add_hazard(2.0, 2.0, value=2.0, radius=0.8)       # Moderate hazard
world.add_hazard(5.0, 5.0, value=1.5, radius=1.2)       # Wide-area hazard
world.add_wall(4.0, 4.0, radius=0.5)                     # Small wall
world.add_marker(8.0, 8.0, value=100.0, radius=1.0)     # Important marker

# Find nearest resource
def find_nearest_resource(world, x, y):
    objects = world.get_objects()
    nearest = None
    min_dist = float('inf')
    
    for obj in objects:
        if obj.type == nlm.WorldObjectType.Resource:
            dist = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
            if dist < min_dist:
                min_dist = dist
                nearest = obj
    
    return nearest

nearest = find_nearest_resource(world, 5.0, 5.0)
if nearest:
    print(f"Nearest resource at ({nearest.x}, {nearest.y}) with value {nearest.value}")
```

### add_wall() and add_marker()

```python
# Add walls and markers for environment shaping
world.add_wall(1.0, 1.0, radius=0.5)        # Small wall
world.add_wall(5.0, 5.0, radius=1.0)        # Large wall
world.add_marker(2.0, 2.0, value=10.0)      # Goal location
world.add_marker(8.0, 8.0, value=50.0)      # Important point
world.add_marker(12.0, 12.0, value=100.0)   # High-value marker

# Create structured environments
def create_environment(world, layout_type="random"):
    if layout_type == "open":
        # Open field with scattered resources
        world.add_resource(2.0, 2.0, value=2.0)
        world.add_resource(8.0, 8.0, value=3.0)
        world.add_resource(5.0, 9.0, value=1.0)
        world.add_marker(9.5, 9.5, value=50.0)
        
    elif layout_type == "maze":
        # Create maze-like structure
        # Vertical walls
        for y in [2.0, 4.0, 6.0, 8.0]:
            world.add_wall(3.0, y, radius=0.3)
            world.add_wall(7.0, y, radius=0.3)
        
        # Horizontal walls
        for x in [2.0, 4.0, 6.0, 8.0]:
            world.add_wall(x, 3.0, radius=0.3)
            world.add_wall(x, 7.0, radius=0.3)
        
        # Entrance and exit
        world.add_marker(1.0, 5.0, value=20.0)
        world.add_marker(9.0, 5.0, value=100.0)
        
    elif layout_type == "resource_farm":
        # Farm layout with resources and hazards
        for x in [1.0, 3.0, 5.0, 7.0]:
            for y in [1.0, 3.0, 5.0, 7.0]:
                if (x + y) % 2 == 0:
                    world.add_resource(x, y, value=1.0, radius=0.4)
                else:
                    world.add_hazard(x, y, value=0.5, radius=0.4)

# Create different environments
create_environment(world, "open")
create_environment(world, "maze")
create_environment(world, "resource_farm")
```

### World Configuration Helpers

```python
# Advanced world configuration
world = nlm.createSimpleWorld()

# Configure with multiple parameters at once
world.configure(
    width=30,
    height=30,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=200.0,
    timestep=0.05,
    gravity=9.8,
    friction=0.1
)

# Quick setup for common scenarios
def setup_research_environment(world):
    world.configure(
        width=50, height=50, visionWidth=20, visionHeight=20,
        maxEnergy=150.0, timestep=0.1
    )
    world.reset()
    
    # Add research station
    world.add_marker(25.0, 25.0, value=200.0, radius=2.0)
    
    # Add resources and hazards
    for i in range(5):
        x = 10 + i * 8
        y = 10 + i * 6
        world.add_resource(x, y, value=5.0 + i * 2, radius=0.6)
        world.add_hazard(x + 4, y + 4, value=1.0 + i, radius=0.8)
    
    return world

research_world = setup_research_environment(world)
```

### Agent Management

```python
# Create multiple agents for testing
agents = []
for i in range(3):
    agent = world.create_agent(
        x=5.0 + i * 10,
        y=5.0 + i * 10,
        orientation=i * 90.0
    )
    agents.append(agent)

# Batch agent operations
def initialize_all_agents(world, agents):
    for agent in agents:
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)

initialize_all_agents(world, agents)

# Agent world pair creation (convenience function)
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Or create complete agent-world pairs
agent_world_pair = nlm.create_agent_world_pair({
    'brain_config': nlm.createDefaultConfig(),
    'world_config': {
        'width': 20,
        'height': 20,
        'visionWidth': 8,
        'visionHeight': 8
    }
})
```

### World State Management

```python
# World state operations
world.reset()  # Reset to initial state
world.update(0.1)  # Advance simulation
world.setRandomSeed(42)  # Reproducible simulations

# Save and load world state
world.save("world_checkpoint.json")
loaded_world = nlm.createSimpleWorld()
loaded_world.load("world_checkpoint.json")

# World analysis
def analyze_world(world):
    objects = world.get_objects()
    resources = [obj for obj in objects if obj.type == nlm.WorldObjectType.Resource]
    hazards = [obj for obj in objects if obj.type == nlm.WorldObjectType.Hazard]
    walls = [obj for obj in objects if obj.type == nlm.WorldObjectType.Wall]
    markers = [obj for obj in objects if obj.type == nlm.WorldObjectType.Marker]
    
    return {
        'total_objects': len(objects),
        'resources': len(resources),
        'hazards': len(hazards),
        'walls': len(walls),
        'markers': len(markers),
        'density': len(objects) / (world.getWidth() * world.getHeight())
    }

world_analysis = analyze_world(world)
print(f"World analysis: {world_analysis}")
```

---

## AgentBrain Improvements

The `AgentBrain` class now provides **convenience methods for complete sensory processing workflows**, **system state monitoring**, and **bulk subsystem control**:

### step_percept() and step_and_decode()

```python
import pynlm as nlm

# Create brain and agent
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent_brain.initialize(world)

# Method 1: Process sensory input (step_percept)
percept = world.getSensoryPercept()
agent_brain.step_percept(percept)  # Just processes sensory input

# Method 2: Complete processing workflow (step_and_decode)
action = agent_brain.step_and_decode(percept)  # Process + decode
world.applyMotorCommand(action, world.getSimulationTime())

# Method 3: Single combined method
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    action = agent_brain.step_and_decode(percept)
    world.applyMotorCommand(action, world.getSimulationTime())
```

### get_system_state()

```python
# Get comprehensive agent system state
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Get current system state
state = agent_brain.get_system_state()

# Access system components
print(f"Firing neurons: {state['brain']['firing_neurons']}")
print(f"Curiosity level: {state['neuromodulation']['curiosity']}")
print(f"Prediction error: {state['prediction']['error']}")
print(f"Working memory usage: {state['memory']['working_memory']['usage']}")
print(f"Attention focus: {state['cognition']['attention']['focus']}")

# Check specific system conditions
if state['neuromodulation']['curiosity'] > 0.5:
    print("Agent is curious, promoting exploration")

if state['prediction']['error'] > 0.8:
    print("High prediction error, need to learn")

if state['brain']['firing_rate'] < 10:
    print("Low brain activity, may need stimulation")
```

### enable_all() and disable_all()

```python
# Bulk subsystem control
agent_brain = nlm.createAgentBrain(brain)

# Enable all subsystems at once
agent_brain.enable_all()
print("All subsystems enabled")

# Disable all subsystems
agent_brain.disable_all()
print("All subsystems disabled")

# Selective bulk operations
def configure_subsystems(agent_brain, config):
    subsystems = config.get('subsystems', [])
    
    if 'all' in subsystems:
        agent_brain.enable_all()
    else:
        for subsystem in subsystems:
            if subsystem == 'reward_modulation':
                agent_brain.enableRewardModulation(True)
            elif subsystem == 'structural_plasticity':
                agent_brain.enableStructuralPlasticity(True)
            elif subsystem == 'development':
                agent_brain.enableDevelopment(True)
            elif subsystem == 'curiosity':
                agent_brain.enableCuriosity(True)
            elif subsystem == 'prediction':
                agent_brain.enablePrediction(True)
            elif subsystem == 'memory':
                agent_brain.enableMemory(True)

# Configuration-based setup
config = {
    'subsystems': ['reward_modulation', 'curiosity', 'development'],
    'parameters': {
        'reward_modulation': {'learning_rate': 0.01},
        'curiosity': {'exploration_strength': 0.8},
        'development': {'growth_rate': 0.05}
    }
}

configure_subsystems(agent_brain, config)
```

### Advanced Agent Control

```python
# Complete agent workflow with state monitoring
def run_agent_simulation(agent_brain, world, num_steps=100):
    # Initialize with monitoring
    state = agent_brain.get_system_state()
    print(f"Starting simulation - Initial state: {state['brain']['development_stage']}")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        action = agent_brain.step_and_decode(percept)
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check system state
        current_state = agent_brain.get_system_state()
        
        # State-based behavior
        if current_state['neuromodulation']['curiosity'] > 0.7:
            print(f"Step {step}: High curiosity, encouraging exploration")
        
        if current_state['prediction']['error'] > 0.9:
            print(f"Step {step}: High prediction error, learning triggered")
        
        # Periodic status reporting
        if step % 20 == 0:
            print(f"Step {step}: "
                  f"Firing={current_state['brain']['firing_neurons']}, "
                  f"Curiosity={current_state['neuromodulation']['curiosity']:.2f}, "
                  f"DevStage={current_state['brain']['development_stage']}")
    
    # Final state
    final_state = agent_brain.get_system_state()
    print(f"Simulation complete. Final development stage: {final_state['brain']['development_stage']}")

# Run with different configurations
for config_name in ['conservative', 'balanced', 'exploratory']:
    print(f"\n=== Running {config_name} configuration ===")
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Configure based on strategy
    if config_name == 'conservative':
        agent_brain.enable_all()
        agent_brain.enableCuriosity(0.3)  # Low curiosity
    elif config_name == 'balanced':
        agent_brain.enable_all()
        agent_brain.enableCuriosity(0.6)  # Medium curiosity
    elif config_name == 'exploratory':
        agent_brain.enable_all()
        agent_brain.enableCuriosity(0.9)  # High curiosity
    
    run_agent_simulation(agent_brain, world, num_steps=50)
```

### Agent Brain Pair Creation

```python
# Create complete agent-world pairs
def create_agent_world_pair(config):
    """Create a complete agent-world pair with configuration helpers."""
    
    # Extract configuration
    brain_config = config.get('brain_config', nlm.createDefaultConfig())
    world_config = config.get('world_config', {})
    agent_config = config.get('agent_config', {})
    
    # Create brain
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    if world_config:
        world.configure(**world_config)
    world.reset()
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    
    # Configure agent subsystems
    if agent_config.get('enable_reward_modulation', False):
        agent_brain.enableRewardModulation(True)
    
    if agent_config.get('enable_curiosity', False):
        agent_brain.enableCuriosity(agent_config.get('curiosity_level', 0.5))
    
    if agent_config.get('enable_development', False):
        agent_brain.enableDevelopment(True)
    
    # Initialize agent with world
    agent_brain.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent_brain': agent_brain
    }

# Usage examples
research_config = {
    'brain_config': nlm.createDefaultConfig(),
    'world_config': {
        'width': 50,
        'height': 50,
        'visionWidth': 16,
        'visionHeight': 16,
        'maxEnergy': 200.0
    },
    'agent_config': {
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'curiosity_level': 0.7,
        'enable_development': True
    }
}

research_setup = create_agent_world_pair(research_config)
```

---

## New Example Patterns

### Using Dictionary-like Config Access

```python
import pynlm as nlm

# Create configuration with dictionary-like access
config = nlm.createDefaultConfig()

# Set multiple configuration values at once
config_dict = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.01,
    "neuromod.dopamine.scale": 1.2,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "development.enable": True,
}

for key, value in config_dict.items():
    config.set(key, value)

# Access configuration values with defaults
neuron_count = config.get("brain.neuron_count", 1000)
synapse_density = config.get("brain.synapse_density", 0.1)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
dopamine_scale = config.get("neuromod.dopamine.scale", 1.0)

# Check if configuration has specific features
if config.has("neuromod.curiosity.enable"):
    curiosity_enabled = config.get("neuromod.curiosity.enable")
    print(f"Curiosity enabled: {curiosity_enabled}")

# Create brain with configuration
brain = nlm.createBrain(config)
brain.initialize()
```

### Creating Complete Agent-World Pairs with Configuration Helpers

```python
def create_exploratory_agent_world(width=30, height=30, seed=None):
    """Create a complete exploratory agent-world setup with configuration helpers."""
    
    # Set random seed for reproducibility
    if seed is not None:
        world = nlm.createSimpleWorld()
        world.setRandomSeed(seed)
    else:
        world = nlm.createSimpleWorld()
    
    # Configure world with helper methods
    world.configure(
        width=width,
        height=height,
        visionWidth=width//3,
        visionHeight=height//3,
        maxEnergy=100.0,
        timestep=0.1
    )
    
    # Create exploration environment
    create_exploration_world(world)
    
    # Create brain with optimized configuration
    brain_config = nlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", 1500)
    brain_config.set("brain.synapse_density", 0.12)
    brain_config.set("neuromod.curiosity.enable", True)
    brain_config.set("neuromod.curiosity.strength", 0.8)
    brain_config.set("neuromod.novelty.enable", True)
    brain_config.set("development.enable", True)
    
    # Create brain and agent
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable exploratory subsystems
    agent_brain.enable_all()
    agent_brain.enableCuriosity(0.8)
    
    return {
        'world': world,
        'brain': brain,
        'agent_brain': agent_brain,
        'config': brain_config
    }

def create_exploration_world(world):
    """Create exploration environment with resources and challenges."""
    # Add exploration areas
    world.add_marker(5.0, 5.0, value=50.0, radius=1.5)      # Starting point
    world.add_marker(25.0, 25.0, value=200.0, radius=2.0)   # Goal zone
    
    # Add food resources
    for x in [10.0, 15.0, 20.0]:
        for y in [10.0, 15.0, 20.0]:
            world.add_resource(x, y, value=3.0 + (x + y) / 10.0, radius=0.6)
    
    # Add moderate hazards
    for x in [8.0, 12.0, 17.0, 22.0]:
        for y in [8.0, 12.0, 17.0, 22.0]:
            if abs(x - 15.0) + abs(y - 15.0) < 12.0:
                world.add_hazard(x, y, value=0.8, radius=0.5)
    
    # Add walls to create pathways
    for x in [5.0, 10.0, 15.0, 20.0, 25.0]:
        world.add_wall(x, 7.5, radius=0.3)
        world.add_wall(x, 22.5, radius=0.3)

# Create exploratory setup
exploratory_setup = create_exploratory_agent_world(seed=42)
world = exploratory_setup['world']
brain = exploratory_setup['brain']
agent_brain = exploratory_setup['agent_brain']

print("Exploratory agent-world setup created!")
```

### Working with Sensory Data Using Array-like Interfaces

```python
import pynlm as nlm
import numpy as np

# Create sensory data using array-like interfaces
def create_vision_data(width, height, channels=3):
    """Create vision data using array-like interface."""
    # Generate synthetic visual data
    np.random.seed(42)
    vision_array = np.random.rand(height, width, channels).astype(np.float32)
    
    # Apply some processing to make it more realistic
    vision_array = vision_array * 0.8 + 0.1  # Scale to [0.1, 0.9]
    
    # Create vision from array
    vision = nlm.Vision.from_array(vision_array.flatten(), width, height, channels)
    
    return vision

def create_audio_data(sample_rate=22050, duration=1.0):
    """Create audio data using array-like interface."""
    # Generate synthetic audio (simple tone pattern)
    t = np.linspace(0, duration, int(sample_rate * duration))
    audio_array = np.sin(2 * np.pi * 440 * t).astype(np.float32)  # 440 Hz tone
    
    # Add some noise
    audio_array += np.random.normal(0, 0.05, len(audio_array))
    audio_array = np.clip(audio_array, -1.0, 1.0)
    
    # Create audio from array
    audio = nlm.Audio.from_array(audio_array.tolist(), sample_rate)
    
    return audio

# Create sensory percept with multiple modalities
def create_multimodal_percept(vision_width=64, vision_height=64, audio_duration=0.5):
    """Create a multimodal percept with vision and audio data."""
    # Create visual data
    vision = create_vision_data(vision_width, vision_height)
    
    # Create audio data
    audio = create_audio_data(audio_duration=audio_duration)
    
    # Create percept
    from pynlm import SensoryPercept
    percept = SensoryPercept()
    
    # Add sensory data using convenience methods
    percept.set_vision_like(
        vision.getData(),
        vision.getWidth(),
        vision.getHeight(),
        vision.getChannels()
    )
    
    percept.set_audio_like(
        audio.getData(),
        audio.getSampleRate()
    )
    
    # Add internal signals
    percept.add_internal_like("reward", 0.7)
    percept.add_internal_like("curiosity", 0.5)
    
    return percept

# Use multimodal percept
multimodal_percept = create_multimodal_percept()

# Process percept with agent brain
agent_brain = nlm.createAgentBrain(brain)
action = agent_brain.step_and_decode(multimodal_percept)
print(f"Agent processed multimodal percept and took action: {action}")
```

### Using Serialization to Save/Load Brain States with Metadata

```python
import pynlm as nlm
import json
import time
from datetime import datetime

# Save brain state with metadata
def save_brain_state_with_metadata(brain, agent_brain, world, filepath):
    """Save brain state with metadata using serialization."""
    
    # Create metadata
    metadata = {
        'timestamp': datetime.now().isoformat(),
        'version': '1.0',
        'brain_info': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'firing_rate': brain.getAverageFiringRate()
        },
        'agent_info': {
            'curiosity_level': agent_brain.getCuriosityLevel(),
            'novelty_level': agent_brain.getNoveltyLevel(),
            'prediction_error': agent_brain.getPredictionError()
        },
        'world_info': {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime()
        },
        'save_info': {
            'saved_by': 'NLM Demo',
            'save_type': 'checkpoint'
        }
    }
    
    # Get brain state using serialization
    brain_state = brain.get_state()  # This would be the actual method
    agent_state = agent_brain.get_system_state()
    world_state = world.get_objects()
    
    # Combine states
    save_data = {
        'metadata': metadata,
        'brain_state': brain_state,
        'agent_state': agent_state,
        'world_state': world_state
    }
    
    # Save to file
    with open(filepath, 'w') as f:
        json.dump(save_data, f, indent=2)
    
    print(f"Brain state saved to {filepath}")
    return save_data

# Load brain state with metadata
def load_brain_state_with_metadata(filepath, brain_config=None):
    """Load brain state with metadata using deserialization."""
    
    # Load data from file
    with open(filepath, 'r') as f:
        save_data = json.load(f)
    
    metadata = save_data['metadata']
    brain_state = save_data['brain_state']
    agent_state = save_data['agent_state']
    world_state = save_data['world_state']
    
    # Create new brain and agent
    if brain_config is None:
        brain_config = nlm.createDefaultConfig()
    
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    # Load brain state (this would be the actual method)
    brain.load_state(brain_state)  # This would be the actual method
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    
    # Create world
    world = nlm.createSimpleWorld()
    # Configure world based on saved state (simplified)
    
    print(f"Brain state loaded from {filepath}")
    print(f"Loaded at: {metadata['timestamp']}")
    print(f"Brain development stage: {metadata['brain_info']['development_stage']}")
    
    return {
        'brain': brain,
        'agent_brain': agent_brain,
        'world': world,
        'metadata': metadata
    }

# Create a demo brain to save
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
agent_brain = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Run simulation
for i in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    action = agent_brain.step_and_decode(percept)
    world.applyMotorCommand(action, world.getSimulationTime())

# Save brain state
save_data = save_brain_state_with_metadata(brain, agent_brain, world, "brain_checkpoint.json")

# Load brain state (if file exists)
if os.path.exists("brain_checkpoint.json"):
    loaded_setup = load_brain_state_with_metadata("brain_checkpoint.json")
    print("Brain state successfully loaded!")
```

---

## Enhanced Troubleshooting Section

### Common Issues and Solutions

**"The configuration has unexpected behavior"**
- Check that you're using dictionary-like access correctly:
  ```python
  config.set("brain.neuron_count", 1000)  # Correct
  config.set("brain.neuron_count", 1000)  # Also works
  ```
- Use safe key access:
  ```python
  value = config.get("brain.neuron_count", 1000)  # With default
  ```
- Validate configuration:
  ```python
  if config.has("brain.neuron_count"):
      count = config.get("brain.neuron_count")
  ```

**"Sensory data conversion fails"**
- Check array dimensions:
  ```python
  # Correct: visionArray is 84x84x3 (height, width, channels)
  vision = nlm.Vision.from_array(vision_array, 84, 84, 3)
  
  # Wrong: visionArray is 3x84x84 (channels, height, width)
  # vision = nlm.Vision.from_array(vision_array, 3, 84, 84)  # This would fail
  ```
- Validate data range:
  ```python
  if not all(0.0 <= x <= 1.0 for x in vision_data):
      raise ValueError("Vision data values should be in range [0, 1]")
  ```

**"Agent brain processing fails"**
- Ensure proper initialization order:
  ```python
  brain = nlm.createBrain(config)
  brain.initialize()
  
  agent_brain = nlm.createAgentBrain(brain)
  agent_brain.initialize(world)  # Must be after world setup
  ```
- Check that all subsystems are enabled:
  ```python
  if not agent_brain.get_system_state()['neuromodulation']['curiosity']:
      print("Curiosity subsystem not enabled - exploration will be limited")
  ```

**"Percept serialization issues"**
- Ensure all sensory data is properly formatted:
  ```python
  # Convert to Python lists for JSON serialization
  vision_data = list(percept.get_vision())
  audio_data = list(percept.get_audio())
  internal_data = list(percept.get_internal())
  ```
- Use from_dict for deserialization:
  ```python
  new_percept = SensoryPercept.from_dict(percept_dict)
  ```

**"World object placement problems"**
- Check coordinate bounds:
  ```python
  # World is 20x20, so coordinates should be between 0 and 20
  world.add_resource(5.0, 3.0, value=2.0)  # Valid
  world.add_resource(25.0, 30.0, value=2.0)  # Invalid - outside world bounds
  ```
- Use find_nearest_resource for spatial queries:
  ```python
  nearest = find_nearest_resource(world, agent_x, agent_y)
  ```

### Error Messages and Best Practices

```python
# Best practice: Check configuration before use
config = nlm.createDefaultConfig()
if not config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 1000)

# Best practice: Validate sensory data
def safe_create_vision(data, width, height, channels):
    try:
        vision = nlm.Vision.from_array(data, width, height, channels)
        # Additional validation
        if len(vision.getData()) != width * height * channels:
            raise ValueError("Vision data dimensions don't match")
        return vision
    except ValueError as e:
        print(f"Vision creation failed: {e}")
        return None

# Best practice: Use try-except for critical operations
try:
    world.add_resource(5.0, 3.0, value=2.0, radius=0.5)
    print("Resource added successfully")
except Exception as e:
    print(f"Failed to add resource: {e}")

# Best practice: Log configuration changes for debugging
old_value = config.get("brain.neuron_count", None)
new_value = 2000
config.set("brain.neuron_count", new_value)
print(f"Neuron count changed from {old_value} to {new_value}")

# Best practice: Use convenience methods for common tasks
percept_dict = world.getSensoryPercept().as_dict()  # Simple serialization
vision_data = world.getSensoryPercept().get_vision()  # Easy access
if world.getSensoryPercept().has_vision():
    print("Processing vision data...")  # Conditional processing
```

---

## New Section on Advanced Patterns

### Pattern 1: Dictionary-based Configuration Management

```python
import pynlm as nlm

# Advanced configuration management using dictionary patterns
def configure_brain_advanced(brain_config):
    """Advanced brain configuration using dictionary patterns."""
    
    # Configuration hierarchy
    configuration = {
        'brain': {
            'neuron_count': 2000,
            'synapse_density': 0.15,
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.01,
                    'tau_plus': 20.0,
                    'tau_minus': 20.0
                },
                'hebbian': {
                    'enable': True,
                    'learning_rate': 0.005
                },
                'structural': {
                    'enable': True,
                    'max_synapses': 5000
                }
            }
        },
        'neuromodulation': {
            'dopamine': {
                'scale': 1.2,
                'plasticity_modulation': 0.8
            },
            'curiosity': {
                'enable': True,
                'exploration_strength': 0.8,
                'novelty_detection': True
            },
            'novelty': {
                'enable': True,
                'sensitivity': 0.6
            }
        },
        'development': {
            'enable': True,
            'growth_rate': 0.05,
            'maturation_time': 100.0,
            'critical_period': 20.0
        }
    }
    
    # Apply configuration using dictionary-like access
    for brain_section, brain_settings in configuration['brain'].items():
        if isinstance(brain_settings, dict):
            for setting_name, setting_value in brain_settings.items():
                if isinstance(setting_value, dict):
                    # Handle nested settings
                    for key, value in setting_value.items():
                        config_key = f"brain.{brain_section}.{setting_name}.{key}"
                        config.set(config_key, value)
                else:
                    config_key = f"brain.{brain_section}.{setting_name}"
                    config.set(config_key, setting_value)
        else:
            config_key = f"brain.{brain_section}"
            config.set(config_key, brain_settings)
    
    # Handle neuromodulation
    for neuromod_name, neuromod_settings in configuration['neuromodulation'].items():
        for setting_name, setting_value in neuromod_settings.items():
            if isinstance(setting_value, dict):
                for key, value in setting_value.items():
                    config_key = f"neuromod.{neurom_name}.{setting_name}.{key}"
                    config.set(config_key, value)
            else:
                config_key = f"neuromod.{neurom_name}.{setting_name}"
                config.set(config_key, setting_value)
    
    # Handle development
    for setting_name, setting_value in configuration['development'].items():
        config_key = f"development.{setting_name}"
        config.set(config_key, setting_value)
    
    return config

# Usage with advanced configuration
brain_config = nlm.createDefaultConfig()
advanced_config = configure_brain_advanced(brain_config)

brain = nlm.createBrain(advanced_config)
brain.initialize()
print("Advanced brain configuration applied successfully")
```

### Pattern 2: Complete Agent-World Pair Creation

```python
import pynlm as nlm

# Advanced agent-world pair creation using configuration helpers
def create_production_agent_world(
    name="production_agent",
    brain_config=None,
    world_config=None,
    agent_config=None,
    environment_config=None
):
    """Create a production-ready agent-world setup."""
    
    # Initialize with sensible defaults
    if brain_config is None:
        brain_config = nlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", 3000)
        brain_config.set("brain.synapse_density", 0.12)
    
    if world_config is None:
        world_config = {
            'width': 50,
            'height': 50,
            'visionWidth': 20,
            'visionHeight': 20,
            'maxEnergy': 150.0,
            'timestep': 0.05,
            'gravity': 9.8,
            'friction': 0.1
        }
    
    if agent_config is None:
        agent_config = {
            'enable_reward_modulation': True,
            'enable_structural_plasticity': True,
            'enable_development': True,
            'enable_curiosity': True,
            'curiosity_level': 0.7,
            'exploration_strength': 0.8
        }
    
    # Create brain
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    
    # Configure agent subsystems
    if agent_config['enable_reward_modulation']:
        agent_brain.enableRewardModulation(True)
    
    if agent_config['enable_structural_plasticity']:
        agent_brain.enableStructuralPlasticity(True)
    
    if agent_config['enable_development']:
        agent_brain.enableDevelopment(True)
    
    if agent_config['enable_curiosity']:
        agent_brain.enableCuriosity(agent_config['curiosity_level'])
    
    # Initialize agent with world
    agent_brain.initialize(world)
    
    # Create environment based on configuration
    if environment_config:
        create_environment_with_config(world, environment_config)
    
    return {
        'name': name,
        'brain': brain,
        'world': world,
        'agent_brain': agent_brain,
        'config': {
            'brain_config': brain_config,
            'world_config': world_config,
            'agent_config': agent_config,
            'environment_config': environment_config
        },
        'metadata': {
            'created_at': time.time(),
            'version': '1.0',
            'author': 'NLM Advanced Setup'
        }
    }

def create_environment_with_config(world, environment_config):
    """Create environment based on configuration."""
    env_type = environment_config.get('type', 'open')
    
    if env_type == 'research':
        # Research station environment
        world.add_marker(25.0, 25.0, value=500.0, radius=3.0)
        
        for x in [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0]:
            for y in [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0]:
                if abs(x - 25.0) + abs(y - 25.0) < 20.0:
                    world.add_resource(x, y, value=2.0 + (x + y) / 10.0, radius=0.8)
                else:
                    world.add_hazard(x, y, value=0.5 + (x + y) / 20.0, radius=0.6)
        
    elif env_type == 'farm':
        # Farm environment with crops and hazards
        for x in [5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0]:
            for y in [5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0]:
                if (x + y) % 10 < 5:
                    world.add_resource(x, y, value=1.5, radius=0.7)  # Crops
                else:
                    world.add_hazard(x, y, value=0.8, radius=0.5)  # Pests/diseases

# Create production environment
production_setup = create_production_agent_world(
    name="research_robot",
    environment_config={'type': 'research'}
)

print(f"Production agent-world setup '{production_setup['name']}' created!")
```

### Pattern 3: Advanced Sensory Data Processing

```python
import pynlm as nlm
import numpy as np

# Advanced sensory data processing patterns
def process_vision_advanced(percept, target_width=64, target_height=64):
    """Advanced vision processing using convenience methods."""
    
    if not percept.has_vision():
        return None
    
    # Get vision data
    vision_data = percept.get_vision()
    original_width = percept.get_vision_width()
    original_height = percept.get_vision_height()
    original_channels = percept.get_vision_channels()
    
    # Convert to numpy for processing
    vision_array = np.array(vision_data).reshape(original_height, original_width, original_channels)
    
    # Apply advanced processing pipeline
    processed_vision = vision_array.copy()
    
    # 1. Normalize to [0, 1]
    if processed_vision.max() > 1.0:
        processed_vision = processed_vision / 255.0
    
    # 2. Apply edge detection (using convolution approximation)
    # For simplicity, using Gaussian blur approximation
    if processed_vision.shape[2] == 3:
        # Simple blur
        kernel_size = 3
        padded = np.pad(processed_vision, ((1,1), (1,1), (0,0)), mode='edge')
        blurred = np.zeros_like(processed_vision)
        
        for c in range(3):
            for i in range(target_height):
                for j in range(target_width):
                    window = padded[i:i+kernel_size, j:j+kernel_size, c]
                    blurred[i, j, c] = np.mean(window)
        
        processed_vision = blurred
    
    # 3. Resize to target dimensions
    if original_width != target_width or original_height != target_height:
        # Simple resize (for real applications, use proper interpolation)
        new_array = np.zeros((target_height, target_width, 3), dtype=np.float32)
        scale_x = original_width / target_width
        scale_y = original_height / target_height
        
        for i in range(target_height):
            for j in range(target_width):
                orig_i = int(i * scale_y)
                orig_j = int(j * scale_x)
                new_array[i, j, :] = processed_vision[orig_i, orig_j, :]
        
        processed_vision = new_array
    
    # 4. Apply contrast enhancement
    processed_vision = np.clip(processed_vision * 1.2, 0.0, 1.0)
    
    # 5. Create new percept with processed data
    from pynlm import SensoryPercept
    processed_percept = SensoryPercept()
    
    # Use convenience methods for setting data
    processed_percept.set_vision_like(
        processed_vision.flatten(),
        target_width,
        target_height,
        processed_vision.shape[2]
    )
    
    return processed_percept

def process_audio_advanced(percept, target_sample_rate=16000):
    """Advanced audio processing using convenience methods."""
    
    if not percept.has_audio():
        return None
    
    # Get audio data
    audio_data = np.array(percept.get_audio())
    original_sample_rate = percept.get_audio_sample_rate()
    
    # Apply advanced processing pipeline
    processed_audio = audio_data.copy()
    
    # 1. Resample if needed
    if original_sample_rate != target_sample_rate:
        # Simple resample (for real applications, use proper resample)
        duration = len(audio_data) / original_sample_rate
        target_length = int(duration * target_sample_rate)
        
        if target_length > 0:
            indices = np.linspace(0, len(audio_data) - 1, target_length)
            processed_audio = np.interp(indices, np.arange(len(audio_data)), audio_data)
    
    # 2. Apply noise reduction
    # Simple spectral subtraction approximation
    if len(processed_audio) > 1024:
        # Apply a simple bandpass filter approximation
        window_size = 1024
        filtered_audio = np.zeros_like(processed_audio)
        
        for i in range(0, len(processed_audio) - window_size, window_size):
            window = processed_audio[i:i+window_size]
            # Apply simple window function
            windowed = window * np.hanning(len(window))
            filtered_audio[i:i+window_size] = windowed
        
        processed_audio = filtered_audio
    
    # 3. Normalize
    if processed_audio.max() > 0:
        processed_audio = processed_audio / abs(processed_audio).max()
    
    # 4. Create new percept with processed data
    from pynlm import SensoryPercept
    processed_percept = SensoryPercept()
    
    # Use convenience methods for setting data
    processed_percept.set_audio_like(
        processed_audio.tolist(),
        target_sample_rate
    )
    
    return processed_percept

# Advanced agent with processed sensory data
def create_advanced_agent_with_processing(brain_config, vision_width=64, vision_height=64):
    """Create an advanced agent with sensory processing capabilities."""
    
    # Create brain and agent
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    agent_brain = nlm.createAgentBrain(brain)
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=vision_width, visionHeight=vision_height)
    world.reset()
    
    # Initialize agent
    agent_brain.initialize(world)
    
    # Create advanced percept processor
    def advanced_percept_processor(percept):
        """Process percept with advanced techniques."""
        
        # Process vision
        processed_vision = process_vision_advanced(percept, vision_width, vision_height)
        if processed_vision is not None:
            print(f"Vision processed: {processed_vision.get_vision_width()}x{processed_vision.get_vision_height()}")
        
        # Process audio
        processed_audio = process_audio_advanced(percept)
        if processed_audio is not None:
            print(f"Audio processed: {processed_audio.get_audio_length()} samples")
        
        # Combine processed percepts (simplified)
        final_percept = SensoryPercept()
        if processed_vision is not None:
            final_percept.set_all_like(processed_vision)
        if processed_audio is not None:
            # Add audio data (simplified)
            pass
        
        return final_percept
    
    return {
        'brain': brain,
        'world': world,
        'agent_brain': agent_brain,
        'advanced_processor': advanced_percept_processor
    }

# Create advanced agent
advanced_config = nlm.createDefaultConfig()
advanced_setup = create_advanced_agent_with_processing(advanced_config)

print("Advanced agent with sensory processing created!")
```

---

## Quick Reference

### Basic Operations

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `nlm.createDefaultConfig()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `nlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

### New Convenience Methods

| Method | Description |
|--------|-------------|
| `ActionType.create_action()` | Create actions with factory methods |
| `Vision.from_array()` | Create vision from numpy arrays |
| `Audio.from_array()` | Create audio from numpy arrays |
| `InternalSignals.from_dict()` | Create internal signals from dictionaries |
| `SensoryPercept.from_dict()` | Create percepts from dictionaries |
| `world.create_agent()` | Create agents with parameters |
| `world.add_resource()` | Add resources to world |
| `world.add_hazard()` | Add hazards to world |
| `world.add_wall()` | Add walls to world |
| `world.add_marker()` | Add markers to world |
| `agent.step_percept()` | Process single sensory input |
| `agent.step_and_decode()` | Complete processing + decode |
| `agent.get_system_state()` | Get comprehensive agent status |
| `agent.enable_all()` | Enable all subsystems |
| `agent.disable_all()` | Disable all subsystems |

### Config Dictionary Access

| Operation | Code |
|-----------|------|
| Set config value | `config.set("key", value)` |
| Get config value | `config.get("key", default)` |
| Check if exists | `config.has("key")` |
| Get all keys | `config.getKeys()` |
| Clear config | `config.clear()` |
| Save to file | `config.saveToFile("file.json")` |
| Load from file | `config.loadFromFile("file.json")` |

---

## Troubleshooting

### Common Issues

**"My configuration changes aren't being applied"**
- Check that you're using the correct key paths:
  - `brain.neuron_count` (not `brain.neuronCount`)
  - `neuromod.curiosity.enable` (not `neuromod.curiosity.enable_flag`)
- Use the `config.getKeys()` method to see all available keys
- Remember that config uses dot notation for nested access

**"Vision data conversion fails"**
- Ensure vision data is in the correct format (flattened numpy array)
- Check dimensions match: vision_array must be width × height × channels
- For `Vision.from_array()`, provide the raw float array, not normalized values
- Vision data should be in range [0, 1] for best results

**"Audio processing has artifacts"**
- Audio samples should be in range [-1, 1] for proper processing
- Use `Audio.from_array(audio_data, sample_rate)` for proper initialization
- Consider using float32 arrays for better precision

**"Agent not responding to sensory input"**
- Ensure all subsystems are enabled with `agent.enable_all()`
- Check that `agent.initialize(world)` is called after world setup
- Verify that `world.update()` is being called in your simulation loop
- Use `agent.get_system_state()` to debug agent status

**"World object placement issues"**
- World coordinates are in the range [0, world_width] and [0, world_height]
- Use `world.create_agent(x, y, orientation)` for agent placement
- Check object collision with walls and boundaries
- Use `world.add_marker(x, y, value)` for goal/target locations

### Error Messages

**"ValueError: Vision data dimensions don't match"**
- Solution: Ensure vision data length equals width × height × channels
- Debug: Use print(f"Expected: {width * height * channels}, Got: {len(data)}")

**"ValueError: Invalid rotation angle"**
- Solution: Ensure rotation angles are within -360 to 360 degrees
- Debug: Check action parameters before creation

**"RuntimeError: Config key not found"**
- Solution: Use `config.has("key")` to check before accessing
- Debug: List all keys with `config.getKeys()`

**"KeyError: 'reward' in InternalSignals"**
- Solution: Use `internal.has_signal("reward")` before accessing
- Debug: Check what signals are available with `internal.get_all_signals()`

### Best Practices

```python
# Always validate inputs before using
if not config.has("brain.neuron_count"):
    print("WARNING: Using default neuron count")

# Use descriptive variable names
brain_config = nlm.createDefaultConfig()
world_config = {"width": 50, "height": 50, "visionWidth": 20, "visionHeight": 20}
agent_brain = nlm.createAgentBrain(brain)

# Log important operations
print(f"Created brain with {brain_config.get('brain.neuron_count', 1000)} neurons")
print(f"World size: {world_config['width']}x{world_config['height']}")

# Save your work
brain.save("my_brain_checkpoint")
world.save("my_world_state")

# Test your setup with a simple simulation
percept = world.getSensoryPercept()
action = agent_brain.step_and_decode(percept)
print(f"Agent took action: {action.getType()}")
```

---

## Next Steps

When you're comfortable with the new convenience methods:

1. Read `HOW_TO_USE.md` for more details on advanced configuration
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations using the new factory methods
4. Try creating complex environments with the world convenience methods
5. Explore the AgentBrain system state monitoring for debugging
6. Use the serialization features to save and load agent experiences
7. Combine multiple percepts for complex sensory processing
8. Build custom configuration patterns using dictionary access

That's it! You're now ready to use NLM with all the new Pythonic convenience methods and advanced features. The updated documentation provides both beginner-friendly guidance and advanced reference material to help you leverage the full power of the new API.
