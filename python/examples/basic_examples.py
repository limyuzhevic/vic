"""Basic usage examples for NLM Python bindings."""

from pynlm import NeuralSystem, SensorData
import numpy as np

print("=== NLM Basic Examples ===")

# Example 1: Create neural system and run simple episode
print("\n1. Basic Neural System Usage:")
brain = NeuralSystem()
print(f"Created brain with {brain.brain.getTotalNeuronCount()} neurons")

# Create world
world = brain.create_world(width=100.0, height=100.0, vision_width=32, vision_height=32)
print(f"Created world: {world.getWidth()}x{world.getHeight()}")

# Create agent
agent = brain.create_agent(0.0, 0.0)
print(f"Created agent at (0, 0)")

# Run episode
print("\nRunning episode...")
episode, total_reward = brain.run_episode(max_steps=10)
print(f"Episode completed with total reward: {total_reward}")

# Get metrics
metrics = brain.get_metrics()
print(f"Brain metrics: {metrics.total_neurons} neurons, {metrics.firing_rate:.2f} firing rate")

# Example 2: Sensor data with NumPy integration
print("\n2. Sensor Data with NumPy:")
vision_data = np.random.rand(32, 32, 3).astype(np.float32)
sensor_data = SensorData.from_vision(vision_data)
print(f"Created sensor data with vision shape: {sensor_data.vision.shape}")

# Example 3: Context managers
print("\n3. Context Managers:")
print("Using BrainSession context manager...")
with NeuralSystem() as brain_session:
    print("  Brain session initialized")
    # Brain is automatically reset when exiting context

print("Example completed successfully!")
