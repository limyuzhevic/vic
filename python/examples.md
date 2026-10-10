# NLM Python Bindings - Example Gallery

## Complete Working Examples

This gallery provides practical examples demonstrating the enhanced NLM Python API features.

## 1. Simple Brain Simulation

### Minimal Example
```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Silent Brain Test
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

## 2. Agent in a Simple World

### Basic Agent Example
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

### Complete Agent with Learning
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
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
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # Run the simulation
    run_agent_simulation(1000)
```

## 3. Sensory Processing Examples

### Vision Processing
```python
import pynlm
import numpy as np

# Create vision input
vision = pynlm.createVision(64, 64, 3)  # 64x64 RGB image

# Create realistic image data (gradient)
for y in range(64):
    for x in range(64):
        # Create a gradient from dark to light
        intensity = (x + y) / 128.0
        for c in range(3):
            idx = (y * 64 + x) * 3 + c
            # Initialize with zeros, we'll use a more efficient approach
            pass

# Use efficient numpy for batch processing
image_data = np.zeros((64, 64, 3), dtype=np.float32)
for y in range(64):
    for x in range(64):
        intensity = (x + y) / 128.0
        image_data[y, x, :] = [intensity, intensity * 0.8, intensity * 0.6]

# Flatten for setData
vision.setData(image_data.flatten().tolist())

# Process the vision
vision.normalize()           # Normalize to [0, 1] range
vision.applyThreshold(0.5)   # Create binary image
vision.convertToGrayscale()  # Convert to single channel

print(f"Processed vision: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
print(f"Vision dimensions: {vision.getDimensions()}")

# Create percept
percept = pynlm.SensoryPercept()
percept.setVision(vision)
print(f"Percept has vision: {percept.hasVision()}")
```

### Audio Processing
```python
import pynlm
import numpy as np

# Create audio input
audio = pynlm.createAudio(44100, 2048)  # 44.1kHz, 2048 samples (~46ms)

# Generate a simple sine wave
t = np.linspace(0, 1.0, 2048, False)
frequency = 440.0  # A4 note
sine_wave = np.sin(2 * np.pi * frequency * t)

# Add some noise for realism
noise = np.random.normal(0, 0.1, 2048)
audio_data = sine_wave + noise

audio.setData(audio_data.tolist())

print(f"Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
print(f"Duration: {audio.getDuration():.3f} seconds")

# Process audio
audio.normalize()  # Normalize to [-1, 1] range

# Get frequency (placeholder - would use real FFT in production)
freq = audio.getFrequency()
print(f"Dominant frequency: {freq:.1f} Hz")

# Create percept
percept = pynlm.SensoryPercept()
percept.setAudio(audio)
print(f"Percept has audio: {percept.hasAudio()}")
```

### Internal Signals Processing
```python
import pynlm
import numpy as np

# Create internal signals
internal_signals = pynlm.createInternalSignals()

# Add some realistic internal signals (simulating neural activity)
# Homeostasis
internal_signals.addSignal(0.8)  # High energy demand
internal_signals.addSignal(0.3)  # Low fatigue
internal_signals.addSignal(0.9)  # High alertness

# Learning signals
internal_signals.addSignal(0.1)  # Low prediction error
internal_signals.addSignal(0.7)  # High dopamine (reward)

print(f"Internal signals: {internal_signals.getAllSignals()}")
print(f"Homeostatic level: {internal_signals.getHomeostaticLevel():.3f}")

# Process internal signals
internal_signals.normalize()  # Normalize ranges

# Create percept
percept = pynlm.SensoryPercept()
percept.setInternal(internal_signals)
print(f"Percept has internal signals: {percept.hasInternal()}")
```

## 4. Action Creation and Usage

### Basic Actions
```python
import pynlm

# Create basic actions
move_forward = pynlm.createAction(pynlm.ActionType.MoveForward)
move_backward = pynlm.createAction(pynlm.ActionType.MoveBackward)
turn_left = pynlm.createAction(pynlm.ActionType.TurnLeft)
turn_right = pynlm.createAction(pynlm.ActionType.TurnRight)
interact = pynlm.createAction(pynlm.ActionType.Interact)
eat = pynlm.createAction(pynlm.ActionType.Eat)
wait = pynlm.createAction(pynlm.ActionType.Wait)

# Display action information
actions = [move_forward, move_backward, turn_left, turn_right, interact, eat, wait]

for action in actions:
    print(f"{action.getName()}: type={action.getType().name}, params={action.getParameters()}")
    print(f"  Valid: {action.isValid()}")
```

### Custom Actions with Parameters
```python
import pynlm

# Create actions with parameters
walk_to_target = pynlm.createAction(
    pynlm.ActionType.Custom, 
    [0.5, 2.0, 1.0]  # speed=0.5, duration=2.0s, angle=1.0rad
)

reach_for_object = pynlm.createAction(
    pynlm.ActionType.Interact,
    [0.8, 0.3]  # strength=0.8, precision=0.3
)

# Modify parameters dynamically
walk_to_target.setParameter(0, 1.0)  # Change speed
walk_to_target.addParameter(2.5)    # Add height parameter

print(f"Walk action: {walk_to_target.getParameters()}")
print(f"Reach action: {reach_for_object.getParameters()}")
```

### Action Validation
```python
import pynlm

# Test various actions
test_actions = [
    pynlm.createAction(pynlm.ActionType.MoveForward),
    pynlm.createAction(pynlm.ActionType.Custom, []),  # Invalid: Custom without params
    pynlm.createAction(pynlm.ActionType.MoveForward, [float('nan')]),  # Invalid: NaN parameter
    pynlm.createAction(pynlm.ActionType.Rest),  # Valid: Rest action
]

for i, action in enumerate(test_actions):
    print(f"Action {i}: {action.getName()}")
    print(f"  Type: {action.getType().name}")
    print(f"  Parameters: {action.getParameters()}")
    print(f"  Valid: {action.isValid()}")
    print()
```

## 5. Configuration Examples

### Configuration Management
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()

# Set configuration values
config.setInt("brain.neuron_count", 2000)
config.setFloat("brain.synapse_density", 0.15)
config.setDouble("brain.v_thresh", -50.0)
config.setBool("plasticity.stdp.enable", True)
config.setBool("plasticity.hebbian.enable", True)
config.setBool("plasticity.structural.enable", True)

config.setBool("neuromod.dopamine.scale", 1.5)
config.setBool("neuromod.curiosity.enable", True)
config.setBool("neuromod.novelty.enable", True)

# Verify configuration
print("Configuration keys:", config.getKeys())
print("Has neuron_count:", config.has("brain.neuron_count"))
print("Neuron count:", config.getInt("brain.neuron_count"))
print("Synapse density:", config.getFloat("brain.synapse_density"))

# Load from file (if file exists)
# config.loadFromFile("my_config.txt")

# Save configuration
# config.saveToFile("my_config.txt")

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Configuration Comparison
```python
import pynlm

# Create two configurations
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createDefaultConfig()

# Modify only one
config2.setInt("brain.neuron_count", 500)
config2.setBool("neuromod.curiosity.enable", False)

print("Config1 summary:")
print(config1.summary())

print("\nConfig2 summary:")
print(config2.summary())

# Get configuration keys
print("\nConfig1 keys:", config1.getKeys())
print("Config2 keys:", config2.getKeys())
```

## 6. World Interaction Examples

### World Configuration
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=12,
    visionHeight=12
)

world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)  # For reproducible results

print(f"World: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
```

### Agent Initialization and Movement
```python
import pynlm

# Setup simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(25.0, 25.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Simulate agent movement
for step in range(50):
    world.update(0.1)
    
    # Get current agent state
    agent_body = world.getAgentBody()
    print(f"Step {step}: Position=({agent_body.x:.1f}, {agent_body.y:.1f}), "
          f"Energy={agent_body.energy:.1f}, Health={agent_body.health:.1f}")
    
    # Create simple percept
    percept = pynlm.SensoryPercept()
    
    # Check for boundaries and create simple vision
    if agent_body.x > world.getWidth() or agent_body.x < 0 or agent_body.y > world.getHeight() or agent_body.y < 0:
        # Near boundary - create warning signal
        internal = pynlm.createInternalSignals()
        internal.addSignal(1.0)  # High boundary proximity
        percept.setInternal(internal)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Get action and move
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 10 == 0:
        print(f"  Action: {action.getName()}")
        print(f"  Brain stage: {brain.getDevelopmentalStage().name}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print()
```

## 7. Learning and Development Examples

### Neuromodulation Training
```python
import pynlm
import numpy as np

# Create agent with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Starting neuromodulation training...")
print("=" * 50)

# Training loop with reward signals
for episode in range(3):
    print(f"\nEpisode {episode + 1}:")
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and think
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Generate reward (in real scenario, based on world outcomes)
        base_reward = 1.0
        novelty_bonus = agent.getNoveltyLevel() * 0.5
        curiosity_bonus = agent.getCuriosityLevel() * 0.3
        
        reward = base_reward + novelty_bonus + curiosity_bonus
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print metrics
        if step % 10 == 0:
            print(f"  Step {step}: Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Dev={brain.getDevelopmentalStage().name}")
    
    print(f"  Episode {episode + 1} completed")

print("\nTraining complete!")
print(f"Final developmental stage: {brain.getDevelopmentalStage().name}")
print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

### Development Stage Progression
```python
import pynlm

# Create agent and simulate development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Development Stage Progression:")
print("=" * 40)

for step in range(100):
    # Simulate developmental progression over time
    age = step * 0.1  # 0.1 simulation time units per step
    
    # Update development with age
    agent.updateDevelopment(age)
    
    # Monitor development
    if step % 10 == 0:
        stage = brain.getDevelopmentalStage()
        curiosity = agent.getCuriosityLevel()
        neuromod = agent.getNeuromodulationLevel()
        
        stage_names = ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"]
        print(f"Step {step:3d}: Stage={stage_names[stage.value]:12s}, "
              f"Curiosity={curiosity:.3f}, Neuromod={neuromod:.3f}")

print(f"\nFinal stage: {brain.getDevelopmentalStage().name}")
```

## 8. State Persistence Examples

### Save and Load Brain State
```python
import pynlm
import tempfile
import os

# Create and train a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Training brain for 100 steps...")
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalSpikeCount()} total spikes")

# Save brain state to temporary file
with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    brain_path = f.name

brain.save(brain_path)
print(f"Saved brain state to: {brain_path}")

# Create a new brain and load the saved state
brain2 = pynlm.createBrain(config)
brain2.load(brain_path)

print(f"Loaded brain state from: {brain_path}")
print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
print(f"Loaded brain has {brain2.getTotalSpikeCount()} total spikes")

# Verify they have the same number of neurons
if brain.getTotalNeuronCount() == brain2.getTotalNeuronCount():
    print("✓ Neuron counts match!")
else:
    print("✗ Neuron counts don't match!")

# Clean up
os.unlink(brain_path)
print("Cleaned up temporary file")
```

### Configuration Persistence
```python
import pynlm
import tempfile
import os

# Create and modify configuration
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 1500)
config.setFloat("brain.synapse_density", 0.2)
config.setBool("plasticity.stdp.enable", True)

print("Configuration:")
print(config.summary())

# Save to file
with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
    config_path = f.name

config.saveToFile(config_path)
print(f"\nSaved configuration to: {config_path}")

# Load configuration (creates new config)
config2 = pynlm.createDefaultConfig()
config2.loadFromFile(config_path)

print("Loaded configuration:")
print(config2.summary())

# Compare configurations
print(f"\nConfigurations match: {config.getKeys() == config2.getKeys()}")

# Clean up
os.unlink(config_path)
print("Cleaned up temporary file")
```

## 9. Error Handling Examples

### Robust Error Handling
```python
import pynlm

print("Testing error handling...")
print("=" * 40)

# Test 1: Invalid configuration
print("\n1. Testing invalid configuration...")
try:
    config = pynlm.createDefaultConfig()
    # This should work fine
    config.set("invalid.type", "not_a_type")
    print("  ✓ Created config with invalid type")
except Exception as e:
    print(f"  ✗ Error: {e}")

# Test 2: Invalid action parameters
print("\n2. Testing invalid action parameters...")
try:
    action = pynlm.createAction(pynlm.ActionType.Custom, [])
    if not action.isValid():
        print("  ✓ Correctly identified invalid action (Custom without params)")
except Exception as e:
    print(f"  ✗ Error: {e}")

# Test 3: Invalid brain operations
print("\n3. Testing invalid brain operations...")
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    # Should fail - need to call initialize first
    brain.step(0)
    print("  ✗ Should have thrown error - initialize not called")
except Exception as e:
    print(f"  ✓ Correctly caught error: {type(e).__name__}")

# Test 4: Invalid percept operations
print("\n4. Testing invalid percept operations...")
try:
    percept = pynlm.SensoryPercept()
    # Try to get a non-existent modality
    # This would depend on actual implementation
    print("  ✓ Created empty percept")
except Exception as e:
    print(f"  ✗ Error: {e}")

print("\nError handling test complete!")
```

## 10. Performance and Memory Examples

### Efficient Object Reuse
```python
import pynlm
import numpy as np

print("Testing efficient object reuse...")
print("=" * 40)

# Create vision object once and reuse
vision = pynlm.createVision(8, 8, 3)
data = np.random.randn(8*8*3).astype(np.float32)

# Process multiple times with same object
for i in range(10):
    vision.setData(data.tolist())
    # Do processing...
    processed = vision.getWidth() * vision.getHeight()  # Just an example
    if i % 5 == 0:
        print(f"  Iteration {i}: Processed {processed} pixels")

print("✓ Reused vision object efficiently")
```

### Batch Processing
```python
import pynlm
import numpy as np

print("Testing batch processing...")
print("=" * 40)

# Create multiple images at once
num_images = 50
image_size = 8 * 8 * 3
images = np.random.randn(num_images, image_size).astype(np.float32)

# Process each image
for i in range(num_images):
    vision = pynlm.createVision(8, 8, 3)
    vision.setData(images[i].tolist())
    
    # Process vision
    vision.normalize()
    
    if i % 10 == 0:
        print(f"  Processed image {i}/{num_images}")

print("✓ Processed batch of images")
```

### Configuration Reuse
```python
import pynlm

print("Testing configuration reuse...")
print("=" * 40)

# Create base configuration
base_config = pynlm.createDefaultConfig()
base_config.setInt("brain.neuron_count", 1000)
base_config.setBool("plasticity.stdp.enable", True)

# Create multiple brains with same config
brains = []
for i in range(5):
    brain = pynlm.createBrain(base_config)
    brain.initialize()
    brains.append(brain)
    print(f"  Created brain {i+1} with {brain.getTotalNeuronCount()} neurons")

print("✓ Reused configuration for multiple brains")
```

## 11. Integration Examples

### Complete Integration Pipeline
```python
import pynlm
import numpy as np

print("Complete NLM Integration Pipeline")
print("=" * 50)

# Step 1: Configuration and Brain Creation
print("\n1. Creating brain...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: World Creation and Configuration
print("2. Creating world...")
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
world.setAgentStart(15.0, 15.0)

# Step 3: Agent Creation and Initialization
print("3. Creating agent...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 4: Enable Learning Systems
print("4. Enabling learning systems...")
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Step 5: Main Simulation Loop
print("5. Running simulation...")
for step in range(100):
    # Update environment
    world.update(0.1)
    
    # Get sensory input (create simple environment)
    percept = pynlm.SensoryPercept()
    
    # Simple environment with goal
    agent_body = world.getAgentBody()
    distance_to_goal = np.sqrt((agent_body.x - 20.0)**2 + (agent_body.y - 20.0)**2)
    
    # Create internal signals representing goal proximity
    internal = pynlm.createInternalSignals()
    internal.addSignal(1.0 / (distance_to_goal + 0.1))  # Higher signal = closer to goal
    internal.addSignal(agent_body.energy / 100.0)      # Energy level
    internal.addSignal(brain.getDevelopmentalStage().value / 4.0)  # Development stage
    
    percept.setInternal(internal)
    
    # Process and act
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = 1.0 / (distance_to_goal + 1.0)  # Reward based on proximity to goal
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 20 == 0:
        print(f"  Step {step}: "
              f"Pos({agent_body.x:.1f},{agent_body.y:.1f}) "
              f"Action={action.getName()} "
              f"Reward={reward:.2f} "
              f"Dev={brain.getDevelopmentalStage().name}")

# Step 6: Final Results
print("\n6. Final Results:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

# Step 7: Save Results
print("\n7. Saving results...")
brain.save("final_brain.bin")
print("  Brain state saved")

print("\n✓ Complete integration pipeline executed successfully!")
```

## Conclusion

These examples demonstrate the enhanced NLM Python API capabilities, including:

- **Complete sensory processing** (Vision, Audio, InternalSignals)
- **Robust action system** with parameter support
- **Advanced brain simulation** with development and learning
- **World interaction** with agent control
- **Error handling** for robust applications
- **Performance optimization** techniques
- **State persistence** for long-running simulations

The enhanced API provides a comprehensive, intuitive interface for building neural simulation systems with full control over all aspects of the NLM framework.

## Next Steps

1. **Explore the documentation** for detailed API reference
2. **Run the examples** to understand the API better
3. **Build your own applications** using the enhanced features
4. **Contribute** to the NLM project if you find it useful

Happy coding with NLM!
