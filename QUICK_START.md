"""
Quick Reference Guide for NLM Python Bindings

This guide provides concise documentation for the enhanced NLM Python bindings,
focusing on the most important improvements and usage patterns.

## Quick Start for Beginners

### 1. Creating a Simple Vision-Based Brain
```python
from pynlm import createSimpleWorld, createBrain, createDefaultConfig

# Create world
world = createSimpleWorld()
world.configure(128, 128, 64, 64)  # width, height, visionWidth, visionHeight

# Create brain with vision support
config = createDefaultConfig()
config.setup_vision(64, 64, 3)  # Setup vision parameters
brain = createBrain(config)

# Initialize agent
agent = createAgentBrain(brain)
agent.initialize(world)

print("Brain initialized successfully!")
```

### 2. Running a Complete Simulation
```python
from pynlm import createSimpleWorld, createBrain, createAgentBrain

# Setup environment
world = createSimpleWorld()
world.configure(100, 100, 50, 50)

# Add objects to world
world.add_resource(10, 10, 0.5, 10.0)   # Food source
world.add_hazard(90, 90, 0.5, -5.0)     # Hazard

# Create and initialize agent
config = createDefaultConfig()
config.setup_brain(neuronCount=200)
brain = createBrain(config)
agent = createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    world.update(0.01)
    
    # Print progress
    if step % 10 == 0:
        print(f"Step {step}: Pos=({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Energy={world.getAgentBody().energy:.2f}")
```

## Convenience Methods Reference

### Config Setup
```python
config = createDefaultConfig()

# Quick setup methods
config.setup_vision(width, height, channels=3)  # Vision configuration
config.setup_audio(sampleRate, numSamples)      # Audio configuration  
config.setup_brain(neuronCount=100, regionCount=1)  # Brain configuration
config.setup_performance(enableSIMD=True, enableParallel=False)  # Performance settings
```

### Sensory Processing
```python
# Vision operations
vision = Vision(64, 64, 3)
vision.setImageData(image_3d_array)  # Set from 3D RGB list
vision.resize(32, 32)                 # Resize input
vision.normalize()                     # Normalize to [0,1]

# Audio operations  
audio = Audio(44100, 1024)
audio.addNoise(0.1)                    # Add controlled noise

# Internal signals
internal = InternalSignals()
internal.addSignal(neuron_activity)
internal.addSignal(memory_content)
```

### Action Creation
```python
# Create actions using convenience methods
action = Action.create_move("forward", speed=1.0)
action = Action.create_look("left") 
action = Action.create_interact("target")

# Check action type
if action.is_movement():
    print("This is a movement action")
elif action.is_sensory():
    print("This is a sensory/action")
```

### World Object Management
```python
# Add objects with convenience methods
world.add_resource(x=10, y=10, radius=0.3, value=10.0)
world.add_hazard(x=90, y=90, radius=0.3, value=-5.0)
```

## Advanced Features for Experts

### Direct System Access
```python
from pynlm import createBrain

config = createDefaultConfig()
brain = createBrain(config)

# Access all internal systems
spike_system = brain.get_spike_system()
stdp = brain.get_stdp()
hebbian = brain.get_hebbian()
structural = brain.get_structural_plasticity()

working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()
associative_memory = brain.get_associative_memory()

prediction_system = brain.get_prediction_system()
planner = brain.get_planner()
concept_formation = brain.get_concept_formation()
attention = brain.get_attention()

development_system = brain.get_development_system()
dopamine = brain.get_dopamine()
curiosity = brain.get_curiosity()
novelty = brain.get_novelty()
prediction_error = brain.get_prediction_error()

random_generator = brain.get_random_generator()
```

### Advanced Configuration
```python
# Configure plasticity systems
stdp.set_learning_rate(0.01)
stdp.set_time_constant(20.0)

dopamine.set_strength(1.0)
dopamine.set_time_constant(100.0)

curiosity.set_sensitivity(0.5)
curiosity.set_decay_rate(0.01)
```

### Efficient Simulation Control
```python
# Run multiple steps efficiently
brain.run_steps(1000, timestep=0.001)

# Simulate complete episode with inputs
inputs = [vision_input1, vision_input2, ...]
brain.simulate_episode(inputs, timestep=0.01)

# Export and import brain state
brain.export_brain_state("trained_brain.pkl")
brain.load("trained_brain.pkl")
```

### Agent Brain Advanced Features
```python
from pynlm import createAgentBrain

agent = createAgentBrain(brain)

# Create and initialize in one step
agent = AgentBrain.create_and_initialize(brain, world)

# Run complete episode
percepts = [percept1, percept2, ...]
agent.run_episode(world, percepts, timestep=0.01)

# Get comprehensive statistics
stats = agent.get_agent_stats()
print(f"Curiosity: {stats['curiosity_level']}")
print(f"Novelty: {stats['novelty_level']}")
print(f"Neuromodulation: {stats['neuromodulation_level']}")

# Simulate action with automatic neuromodulation
result = agent.simulate_action(action)
```

## Error Handling

### Custom Exceptions
```python
from pynlm import NLMError

try:
    # Code that might raise NLMError
    brain = createBrain(config)
except NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Other error: {e}")
```

## Performance Optimization

### Configure Performance Settings
```python
config = createDefaultConfig()
config.setup_performance(enableSIMD=True, enableParallel=True)
```

### Memory Management
```python
# Use memory pools for large simulations
from nlm import MemoryPool

with MemoryPool() as pool:
    # Create many neurons efficiently
    for i in range(1000):
        neuron = pool.create_neuron(i)
        # ... use neuron ...
```

## Best Practices

### 1. Configuration Pattern
```python
# Recommended configuration pattern
config = createDefaultConfig()
config.setup_vision(64, 64, 3)
config.setup_audio(44100, 1024) 
config.setup_brain(neuronCount=200, regionCount=2)
config.setup_performance(enableSIMD=True)

brain = createBrain(config)
```

### 2. Object Reuse Pattern
```python
# Reuse objects instead of creating new ones
vision = Vision(64, 64, 3)
audio = Audio(44100, 1024)
internal = InternalSignals()

for step in range(100):
    # Update objects
    vision.setData(new_data)
    audio.setData(new_audio_data)
    internal.addSignal(new_signal)
    
    # Use in simulation
    agent.process_sensory_input(percept)
    # ...
```

### 3. Memory Management
```python
# Monitor and manage memory
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Clear unused objects
del unused_objects
import gc
gc.collect()
```

### 4. Performance Monitoring
```python
# Monitor simulation performance
import time

start_time = time.time()
brain.run_steps(1000)
end_time = time.time()

print(f"Completed 1000 steps in {end_time - start_time:.3f} seconds")
print(f"Performance: {(end_time - start_time) / 1000:.6f} seconds per step")
```

## Migration Guide

### From Old to New API

#### Config Methods
```python
# OLD
config = Config()
config.loadFromFile("config.json")
config.set("vision.width", 64)
config.set("vision.height", 64)
config.set("brain.neurons", 100)

# NEW  
config = createDefaultConfig()
config.loadFromFile("config.json")
config.setup_vision(64, 64, 3)
config.setup_brain(neuronCount=100)
```

#### Action Creation
```python
# OLD
action = Action(ActionType::MoveForward)
action.setType(ActionType::Look)

# NEW
action = Action.create_move("forward", speed=1.0)
action = Action.create_look("direction")
```

#### Sensory Processing
```python
# OLD
vision = Vision()
vision.setData(data_vector)

# NEW
vision = Vision(64, 64, 3)
vision.setImageData(3d_rgb_list)  # More Pythonic
```

## Common Patterns

### Pattern 1: Vision Processing Pipeline
```python
def process_vision_pipeline(image_3d, width, height):
    # Create vision input
    vision = Vision(width, height, 3)
    vision.setImageData(image_3d)  # Direct from 3D array
    vision.normalize()             # Preprocess
    
    # Create percept
    percept = SensoryPercept()
    percept.setVision(vision)
    
    return percept
```

### Pattern 2: Resource Gathering Agent
```python
def create_resource_gathering_agent(world, vision_size=32):
    # Create agent with resource gathering capabilities
    config = createDefaultConfig()
    config.setup_vision(vision_size, vision_size, 3)
    config.setup_brain(neuronCount=100)
    
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    return agent
```

### Pattern 3: Multi-Episode Training
```python
def train_agent(agent, world, episodes=10, steps_per_episode=100):
    for episode in range(episodes):
        # Reset for new episode
        agent.reset()
        world.reset()
        
        # Run episode
        for step in range(steps_per_episode):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            world.update(0.01)
            
        # Log episode statistics
        stats = agent.get_agent_stats()
        print(f"Episode {episode}: Curiosity={stats['curiosity_level']:.3f}, "
              f"Energy={world.getAgentBody().energy:.3f}")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Vision setup not working
```python
# Solution: Check vision setup
config = createDefaultConfig()
config.setup_vision(64, 64, 3)  # Required: width, height, channels
```

#### Issue: Agent initialization failing
```python
# Solution: Check world setup first
world = createSimpleWorld()
world.configure(100, 100, 50, 50)  # Required: width, height, visionWidth, visionHeight
```

#### Issue: High memory usage
```python
# Solution: Use memory pools
from nlm import MemoryPool
with MemoryPool() as pool:
    # Create objects within pool context
    # ...
```

#### Issue: Slow simulation performance
```python
# Solution: Enable optimizations
config = createDefaultConfig()
config.setup_performance(enableSIMD=True, enableParallel=True)
```

## Getting Help

### Resources
- **Documentation**: See `examples/basic_usage.py` and `IMPLEMENTATION_SUMMARY.md`
- **GitHub Issues**: Report bugs and request features
- **Community**: Join NLM discussions for best practices

### Tips
1. **Start with examples**: Use `examples/basic_usage.py` as a template
2. **Use convenience methods**: They provide Pythonic interfaces for common tasks
3. **Leverage type hints**: Use IDE autocompletion for method discovery
4. **Profile performance**: Use built-in performance monitoring tools
5. **Read documentation**: Comprehensive docstrings available for all methods

## API Compatibility

### Backward Compatibility
- All existing Python APIs continue to work unchanged
- No breaking changes to public interfaces
- Existing code requires no modifications

### New Features
- Enhanced convenience methods for simplified usage
- Advanced access for expert users
- Improved error handling and debugging
- Comprehensive documentation and examples

## Performance Tips

### 1. Batch Operations
```python
# Process multiple inputs efficiently
inputs = [frame1, frame2, frame3, ...]
brain.simulate_episode(inputs, timestep=0.01)
```

### 2. Memory Management
```python
# Clear unused objects regularly
del old_objects
import gc
gc.collect()
```

### 3. Configuration Optimization
```python
# Enable all performance optimizations
config = createDefaultConfig()
config.setup_performance(enableSIMD=True, enableParallel=True)
```

## Conclusion

The enhanced NLM Python bindings provide:

✅ **Beginner-Friendly**: Simplified initialization and common use cases
✅ **Expert-Powerful**: Full access to internal systems and advanced control
✅ **Backward Compatible**: Existing code continues to work
✅ **Well-Documented**: Comprehensive examples and documentation
✅ **Performance Optimized**: Built-in optimizations and monitoring

The API is now "way better for anyone to add" - making NLM accessible to beginners while providing powerful features for experts.

## Quick Reference Commands

### Create Basic Setup
```bash
from pynlm import createSimpleWorld, createBrain, createDefaultConfig

world = createSimpleWorld()
world.configure(128, 128, 64, 64)

config = createDefaultConfig()
config.setup_vision(64, 64, 3)
brain = createBrain(config)
```

### Run Simulation
```bash
agent = createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    world.update(0.01)
```

### Access Advanced Features
```bash
spike_system = brain.get_spike_system()
stdp = brain.get_stdp()
working_memory = brain.get_working_memory()

brain.run_steps(1000, timestep=0.001)
brain.export_brain_state("trained.pkl")
```

Happy coding with NLM!
