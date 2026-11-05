"""
Hello World with NLM (Neural Learning Machine)

This is a simple tutorial to get started with the NLM Python bindings.
It demonstrates basic concepts of creating and using a brain simulation.

Prerequisites:
- Install NLM: pip install pynlm
- Install NumPy: pip install numpy

Let's start with a basic "Hello World" example that creates a simple brain
and runs a few simulation steps.
"""

"""
Example 1: Basic Brain Creation and Initialization
"""

print("=== NLM Hello World Tutorial ===")
print("Creating a simple neural network simulation...\n")

# Import the NLM bindings
from pynlm import Brain, Config

print("1. Creating configuration...")
config = Config()
config.set("neuron_count", 100)  # 100 neurons
config.set("region_count", 2)    # 2 brain regions  
config.set("connection_probability", 0.1)  # 10% connectivity

print("   Configuration created successfully")
print(f"   - Neurons: {config.get('neuron_count')}")
print(f"   - Regions: {config.get('region_count')}")
print(f"   - Connection probability: {config.get('connection_probability')}")
print()

print("2. Creating and initializing the brain...")
brain = Brain(config)

success = brain.initialize()
if success:
    print("   ✓ Brain initialized successfully")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Number of regions: {brain.getRegionCount()}")
else:
    print("   ✗ Failed to initialize brain")
    exit(1)

print()

print("3. Running simulation steps...")
for step in range(10):
    brain.step()
    print(f"   Step {step + 1}/10")

print()
print("✓ Basic simulation completed successfully!")
print()

"""
Example 2: Working with Sensory Input
"""

print("=== Example 2: Sensory Input ===")
print("Demonstrating how to work with sensory input...\n")

# Import sensory modules
from pynlm import Vision, Audio, SensoryInput

print("1. Creating Vision sensory input...")
# Create a 224x224 RGB image (common in computer vision)
# In real usage, you'd load actual image data here
vision = Vision(224, 224, 3)

# Set some example data (simulating an image)
import numpy as np
example_data = np.random.rand(224 * 224 * 3).astype(np.float32)
vision.setData(example_data)

print("   ✓ Vision created and data set")
print(f"   - Width: {vision.getWidth()}")
print(f"   - Height: {vision.getHeight()}")
print(f"   - Channels: {vision.getChannels()}")
print(f"   - Dimensions: {vision.getDimensions()}")
print()

print("2. Creating Audio sensory input...")
audio = Audio(sampleRate=44100, numSamples=1024)

# Set example audio data
example_audio = np.random.rand(1024).astype(np.float32)
audio.setData(example_audio)

print("   ✓ Audio created and data set")
print(f"   - Sample rate: {audio.getSampleRate()}")
print(f"   - Num samples: {audio.getNumSamples()}")
print(f"   - Type: {audio.getType()}")
print()

"""
Example 3: Actions and Motor Control
"""

print("=== Example 3: Actions and Motor Control ===")
print("Demonstrating how to create and use actions...\n")

from pynlm import Action, ActionType

print("1. Creating basic actions...")

# Create different types of actions
action1 = Action(ActionType.MoveForward)
print("   ✓ MoveForward action created")

action2 = Action(ActionType.TurnLeft)
print("   ✓ TurnLeft action created")

action3 = Action(ActionType.Interact)
print("   ✓ Interact action created")

# Create actions with parameters
action4 = Action(ActionType.Custom)
action4.setParameter("force", 5.0)
action4.setParameter("duration", 2.0)
print("   ✓ Custom action with parameters created")

print()

print("2. Executing actions (simulated)...")
# In a real scenario, you'd send these actions to an agent
print("   - Action 1: MoveForward")
print("   - Action 2: TurnLeft") 
print("   - Action 3: Interact")
print("   - Action 4: Custom (force=5.0, duration=2.0)")
print()

print("✓ All action examples completed!")
print()

"""
Example 4: Configuration Management
"""

print("=== Example 4: Configuration Management ===")
print("Demonstrating configuration options...\n")

from pynlm import Config

print("1. Creating configuration...")
config = Config()
print("   ✓ Configuration created")

print()
print("2. Setting various configuration options...")

# Basic brain parameters
config.set("neuron_count", 500)
config.set("region_count", 3)
config.set("connection_probability", 0.05)

# Plasticity parameters
config.set("plasticity_learning_rate", 0.01)
config.set("stdp_ltp_weight", 0.01)
config.set("stdp_ltd_weight", 0.012)

# Neuromodulation parameters
config.set("dopamine_baseline", 0.0)

# Simulation parameters
config.set("simulation_timestep", 0.001)
config.set("max_simulation_steps", 10000)

print("   ✓ Configuration options set")

print()
print("3. Saving configuration...")
# Save configuration to file
import os
config.saveToFile("example_config.cfg")
print("   ✓ Configuration saved to example_config.cfg")

print()
print("4. Loading configuration...")
# Load configuration from file
loaded_config = Config()
loaded_config.loadFromFile("example_config.cfg")
print("   ✓ Configuration loaded from file")

print()

"""
Example 5: Getting Started with Advanced Features
"""

print("=== Example 5: Getting Started with Advanced Features ===")
print("Introduction to advanced NLM features...\n")

print("1. Available advanced features:")
print("   - Multithreading for large-scale simulations")
print("   - SIMD (Single Instruction Multiple Data) optimizations")
print("   - Sparse connectivity for memory efficiency")
print("   - Event-driven processing")
print("   - Checkpointing and replay")
print("   - Developmental stages")
print("   - Neuromodulation (dopamine, curiosity, etc.)")
print("   - Memory systems (working, episodic, associative)")
print("   - Prediction and planning")
print("   - Social learning and imitation")
print()

print("2. Performance considerations:")
print("   - For small simulations: Basic settings")
print("   - For medium simulations (1k-10k neurons): Enable multithreading")
print("   - For large simulations (10k+ neurons): Enable sparse connectivity")
print("   - For long simulations: Enable checkpointing")
print()

print("3. Next steps:")
print("   - Read the HOW_TO_USE.md for detailed documentation")
print("   - Check out the experiments directory for example simulations")
print("   - Join the NLM community for support and discussion")
print("   - Contribute to the project!")
print()

print("=== Tutorial Complete! ===")
print("You now have a basic understanding of how to use NLM.")
print("Remember to check the documentation for more advanced features.")
print()

# Cleanup example files
if os.path.exists("example_config.cfg"):
    os.remove("example_config.cfg")
    print("Cleaned up example_config.cfg")

print("Happy brain simulation! 🌠")
