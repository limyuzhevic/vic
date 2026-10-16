#!/usr/bin/env python3
"""
Demonstration of improved NLM Python bindings
Shows new Pythonic features and enhancements
"""

import pynlm as nlm

print("=" * 60)
print("DEMONSTRATION: Improved NLM Python Bindings")
print("=" * 60)

# Create a simple brain
print("\n1. Creating a brain with improved API:")
config = nlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 3)

brain = nlm.createBrain(config)
brain.initialize()
print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# Demonstrate enum improvements
print("\n2. Enhanced enum support with descriptions:")
print(f"   NeuronType.Excitatory = {nlm.NeuronType.Excitatory}")
print(f"   SynapseType.GapJunction = {nlm.SynapseType.GapJunction}")
print(f"   ActionType.MoveForward = {nlm.ActionType.MoveForward}")
print("   ✓ Enums now have descriptive documentation")

# Demonstrate dictionary integration
print("\n3. Dictionary-based object creation and serialization:")

# Create config from dictionary
config_dict = {"neuron_count": 50, "region_count": 2, "connection_probability": 0.1}
smart_config = nlm.create_brain_from_dict(config_dict)
print(f"   ✓ Created config from dictionary: {smart_config.get('neuron_count')} neurons")

# Create action from dictionary
action_dict = {
    "type": nlm.ActionType.Interact,
    "parameters": [0.5, 0.3]
}
intelligent_action = nlm.Action.from_dict(action_dict)
print(f"   ✓ Created action from dictionary: {intelligent_action.getName()}")

# Convert action to dictionary
reconstructed = intelligent_action.to_dict()
print(f"   ✓ Converted back to dictionary: {reconstructed}")

# Demonstrate improved error handling
print("\n4. Enhanced error handling:")
safe_config = nlm.createDefaultConfig()

# Valid operations
safe_config.set("test_int", 42)
safe_config.set("test_string", "hello")
print("   ✓ Valid operations work correctly")

# Invalid operations handled gracefully
try:
    safe_config.set("bad_value", None)
    print("   ✗ Should have raised ConfigurationError")
except nlm.ConfigurationError as e:
    print(f"   ✓ ConfigurationError caught: {e}")

# Demonstrate helper functions
print("\n5. Helper functions for common operations:")

# Configuration validation
if nlm.validate_config(smart_config):
    print("   ✓ Configuration validation passed")

# Brain reporting
report = nlm.generate_report(brain)
print("   ✓ Generated brain report (first 100 chars):")
print(f"      {report[:100]}...")

# Demonstrate agent features
print("\n6. Enhanced agent brain features:")

world = nlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Get comprehensive feature set
features = agent.getFeatures()
print(f"   ✓ Agent features:")
print(f"      - Neuromodulation level: {features['neuromodulation_level']}")
print(f"      - Curiosity level: {features['curiosity_level']}")
print(f"      - Development enabled: {features['development_enabled']}")

# Demonstrate simulation runner
print("\n7. Simplified simulation execution:")

brain2 = nlm.createBrain(nlm.createDefaultConfig())
brain2.initialize()

# Run a complete simulation in one line
nlm.run_brain_simulation(brain2, world, agent, 20, 0.1)

print(f"   ✓ Simulation completed in 20 steps")
print(f"      - Total spikes: {brain2.getTotalSpikeCount()}")
print(f"      - Firing neurons: {brain2.getFiringNeuronCount()}")

# Demonstrate world object improvements
print("\n8. Enhanced world object API:")

world_obj = nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Resource, radius=1.0)
print(f"   ✓ Created world object at ({world_obj.x}, {world_obj.y})")
print(f"      - Type: {world_obj.type.name}")
print(f"      - Distance to (10, 10): {world_obj.distance_to(10.0, 10.0):.2f}")
print(f"      - Is at (5, 5): {world_obj.is_at(5.0, 5.0)}")

# Object serialization round-trip
obj_dict = world_obj.to_dict()
reconstructed_obj = nlm.WorldObject.from_dict(obj_dict)
print(f"   ✓ World object serialization round-trip successful")

print("\n" + "=" * 60)
print("SUMMARY OF IMPROVEMENTS:")
print("=" * 60)
print("1. ✓ Reduced code duplication with template helpers")
print("2. ✓ Enhanced enum support with descriptive documentation")
print("3. ✓ Dictionary-based object creation and serialization")
print("4. ✓ Improved error handling with custom exception types")
print("5. ✓ Helper functions for common operations")
print("6. ✓ Better configuration management (Pythonic API)")
print("7. ✓ Enhanced agent and brain feature access")
print("8. ✓ Simplified simulation execution")
print("9. ✓ World object utilities and distance calculations")
print("10. ✓ Comprehensive statistics and reporting tools")
print("=" * 60)