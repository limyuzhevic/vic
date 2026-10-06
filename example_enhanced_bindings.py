"""
Simple demonstration of the enhanced Pythonic features in pynlm.

This file shows the key improvements made to the Python bindings.
"""

# Import the enhanced bindings
import pynlm

# Example 1: Python Properties instead of get/set methods
print("=== Example 1: Python Properties ===")
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1000)
config.set("brain.learning_rate", 0.01)

# Using Python properties for better readability
neuron_count = config.get_int("brain.neuron_count")
learning_rate = config.get_float("brain.learning_rate")
print(f"Neuron count: {neuron_count}, Learning rate: {learning_rate}")

# Dict-like access
config["brain.weight_decay"] = 0.001
print(f"Weight decay: {config.get_float('brain.weight_decay')}")

# 'in' operator support
print(f"Has neuron_count key: {'brain.neuron_count' in config}")
print(f"Has unknown key: {'unknown' in config}")

# Example 2: Context Manager for Checkpointing
print("\n=== Example 2: Context Manager for Checkpointing ===")
brain = pynlm.create_brain(config)
brain.initialize()
brain.addRegion("input_region")
brain.addRegion("hidden_region")

print(f"Initial region count: {brain.region_count}")

# Use checkpoint context manager
with brain.checkpoint("demo_checkpoint.sav") as chk:
    # Add more regions within context
    brain.addRegion("output_region")
    brain.addRegion("extra_region")
    print(f"Region count in context: {brain.region_count}")
    # Checkpoint automatically saved on context exit

print("Checkpoint saved automatically")

# Load the checkpoint
brain.reset()
brain.initialize()
brain.load("demo_checkpoint.sav")
print(f"Region count after load: {brain.region_count}")

# Clean up
import os
if os.path.exists("demo_checkpoint.sav"):
    os.unlink("demo_checkpoint.sav")

# Example 3: Pythonic API (__len__, iteration)
print("\n=== Example 3: Pythonic API ===")
config2 = pynlm.create_default_config()
for i in range(5):
    config2.set(f"param_{i}", i * 0.1)

# Use len() for count
print(f"Number of parameters: {len(config2)}")  # Uses __len__

# Dict-like access and iteration
for key in config2:
    print(f"  {key}: {config2.get_float(key)}")

# __getitem__ for direct access
if "param_3" in config2:
    print(f"param_3 value: {config2['param_3']}")

# Example 4: Type Conversion Helpers
print("\n=== Example 4: Type Conversion Helpers ===")
# Convert Python list to C++ vector
py_list = [0.1, 0.2, 0.3, 0.4, 0.5]
vector = pynlm.to_vector(py_list)
print(f"Python list: {py_list}")
print(f"Converted vector: {vector}")

# Convert Python dict to C++ map
py_dict = {"weight": 0.5, "bias": 0.1, "threshold": 0.8}
map_result = pynlm.to_map(py_dict)
print(f"Python dict: {py_dict}")
print(f"Converted map: {map_result}")

# Example 5: Error Handling with Python Exceptions
print("\n=== Example 5: Error Handling ===")
config3 = pynlm.create_default_config()

# Validation helper
validation_result = pynlm.validate_config(config3)
print(f"Validation result: {validation_result}")

# Add invalid configuration
config3.set("brain.neuron_count", -1)

# Get validation errors
errors = pynlm.get_config_errors(config3)
print(f"Validation errors: {errors}")

# Try to validate invalid config (will raise ValueError)
try:
    pynlm.validate_config(config3)
except pynlm.ValueError as e:
    print(f"Caught ValueError as expected: {e}")

# Example 6: Convenience Methods
print("\n=== Example 6: Convenience Methods ===")
brain2 = pynlm.create_brain(config)
brain2.initialize()

# Add some regions
brain2.addRegion("region_a")
brain2.addRegion("region_b")

# Use Python properties for common access
print(f"Total neurons: {brain2.total_neuron_count}")
print(f"Total synapses: {brain2.total_synapse_count}")
print(f"Firing neurons: {brain2.firing_neuron_count}")
print(f"Region count: {brain2.region_count}")

# Use len() for region count
print(f"Number of regions (using len): {len(brain2)}")

# Example 7: Smart Brain Features
print("\n=== Example 7: Smart Brain Features ===")
smart_brain = pynlm.create_smart_brain(config)
print(f"Smart brain created: {smart_brain is not None}")

with smart_brain:
    print(f"Initialized with {smart_brain.get_neuron_count()} neurons")
    
    # Get brain state as dictionary
    state = smart_brain.get_state_dict()
    print(f"Brain state: {state}")
    
    # Run some simulation steps
    result = smart_brain.run_simulation(3)
    print(f"Simulation result: {result}")

print(f"Smart brain string representation:\n{smart_brain.to_string()}")

# Example 8: World Object Factory
print("\n=== Example 8: World Object Factory ===")
resource = pynlm.create_world_object(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
print(f"Created resource: x={resource.x}, y={resource.y}, type={resource.type}, value={resource.value}")

hazard = pynlm.create_world_object(10.0, 10.0, pynlm.WorldObjectType.Hazard, 0.5, 2.0)
print(f"Created hazard: x={hazard.x}, y={hazard.y}, radius={hazard.radius}")

# Example 9: Enhanced Sensory Input Properties
print("\n=== Example 9: Enhanced Sensory Input ===")
vision = pynlm.Vision(640, 480, 3)
print(f"Vision dimensions: {vision.shape}")  # Tuple property

audio = pynlm.Audio(44100, 1024)
print(f"Audio duration: {audio.duration:.2f} seconds")

# Example 10: Agent Body with Convenience Methods
print("\n=== Example 10: Agent Body Convenience Methods ===")
body = pynlm.AgentBody()
body.x = 10.0
body.y = 20.0
body.orientation = 0.5
body.velocityX = 5.0
body.velocityY = 3.0
body.energy = 0.8
body.health = 0.9
body.isMoving = True
body.isTurning = False

print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent speed: {body.speed():.2f}")
print(f"Agent is idle: {body.is_idle()}")

print("\n=== All Examples Completed Successfully ===")
print("\nThe enhanced bindings provide a much more Pythonic interface")
print("while maintaining full backward compatibility!")
