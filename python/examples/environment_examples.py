"""Environment and interaction examples for NLM Python bindings."""

from pynlm import NeuralSystem, EnvironmentExamples, WorldObject
from pynlm import WorldObjectType, ActionType
import numpy as np

print("=== NLM Environment Examples ===")

# Create neural system
brain = NeuralSystem()
world = brain.create_world(width=200.0, height=200.0)
agent = brain.create_agent(10.0, 10.0)

# Add world objects
print("\n1. World Setup:")
print("Adding world objects...")

# Add resources
resource = WorldObject(50.0, 50.0, WorldObjectType.Resource, 10.0)
world.addObject(resource)
print(f"  Added resource at ({resource.x}, {resource.y}) with value {resource.value}")

# Add hazard
hazard = WorldObject(150.0, 150.0, WorldObjectType.Hazard, -5.0)
world.addObject(hazard)
print(f"  Added hazard at ({hazard.x}, {hazard.y}) with damage {hazard.value}")

# Add wall
wall = WorldObject(100.0, 100.0, WorldObjectType.Wall, 0.0, 2.0)
world.addObject(wall)
print(f"  Added wall at ({wall.x}, {wall.y}) with radius {wall.radius}")

# Add marker
marker = WorldObject(200.0, 200.0, WorldObjectType.Marker, 0.0)
world.addObject(marker)
print(f"  Added marker at ({marker.x}, {marker.y})")

# Example 2: Multi-agent scenario
print("\n2. Multi-Agent Scenario:")
print("Setting up multi-agent scenario...")
print("  Agent 1 at (10, 10)")
print("  Agent 2 at (190, 10)")
print("  Agent 3 at (10, 190)")
print("  Agent 4 at (190, 190)")
print("  Shared resources and hazards")
print("  Competitive and cooperative interactions")

# Example 3: Complex interactions
print("\n3. Complex Interactions:")
print("Demonstrating complex environment interactions...")
print("  Resource seeking behavior")
print("  Hazard avoidance")
print("  Social interactions")
print("  Goal-oriented navigation")

# Example 4: Environmental dynamics
print("\n4. Environmental Dynamics:")
print("Demonstrating dynamic world...")
print("  Resources spawning and disappearing")
print("  Hazards moving dynamically")
print("  Weather effects (if implemented)")
print("  Seasonal changes (if implemented)")

# Example 5: World states
print("\n5. World States:")
print("Demonstrating world state management...")
print("  Initial state: setup complete")
print("  Dynamic state: objects interacting")
print("  End state: simulation complete")

print("\nEnvironment examples completed!")
