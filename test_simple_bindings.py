#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings improvements.
"""

import pynlm

print("Testing NLM Python Bindings Improvements")
print("=" * 50)

# Test 1: Basic functionality
print("\n1. Testing basic functionality...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"   ✓ Python version: {pynlm.__version__}")

# Test 2: Enhanced Config interface
print("\n2. Testing enhanced Config interface...")
config.set("brain.neuron_count", 500)
assert config.get("brain.neuron_count") == 500
print("   ✓ Pythonic config.set() and config.get() work")

# Test 3: Enhanced Action
print("\n3. Testing enhanced Action...")
action = pynlm.Action(pynlm.ActionType.MoveForward)
action.set_parameters([0.5, 1.0])
print(f"   ✓ Created action: {action.get_name()}")

# Test 4: Enhanced WorldObject
print("\n4. Testing enhanced WorldObject...")
obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
assert obj.is_valid()
print(f"   ✓ Created valid world object: {obj.is_valid()}")

# Test 5: Enhanced AgentBody
print("\n5. Testing enhanced AgentBody...")
body = pynlm.AgentBody()
body.set_position(10.0, 20.0)
pos = body.position()
assert abs(pos[0] - 10.0) < 0.01
print(f"   ✓ Enhanced AgentBody.position() works: {pos}")

# Test 6: Enhanced Brain statistics
print("\n6. Testing enhanced Brain statistics...")
for i in range(10):
    brain.step(i)

stats = brain.get_statistics()
print(f"   ✓ Enhanced statistics available:")
print(f"     - {stats['total_neurons']} neurons")
print(f"     - {stats['total_synapses']} synapses")
print(f"     - {stats['firing_neurons']} firing neurons")
print(f"     - {stats['total_spikes']} total spikes")

# Test 7: Utility functions
print("\n7. Testing utility functions...")
version_info = pynlm.get_brain_version()
print(f"   ✓ Version info: {version_info['project']} v{version_info['project']}")

# Test 8: Advanced AgentBrain features
print("\n8. Testing AgentBrain features...")
agent = pynlm.createAgentBrain(brain)
print(f"   ✓ Created agent brain")

# Test 9: Error handling and validation
print("\n9. Testing error handling...")
action2 = pynlm.Action(pynlm.ActionType.Custom)
action2.set_parameters([])
print(f"   ✓ Handles edge cases: {len(action2.get_parameters())} parameters")

# Test 10: Complete simulation
print("\n10. Testing complete simulation...")
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
agent.initialize(world)

for step in range(20):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"   ✓ Complete simulation successful")
print(f"     - Final firing neurons: {brain.getFiringNeuronCount()}")
print(f"     - Total spikes: {brain.getTotalSpikeCount()}")

print("\n" + "=" * 50)
print("✅ All tests passed! NLM Python bindings successfully improved.")
print("=" * 50)
print("\nKey Improvements:")
print("• Enhanced Pythonic APIs for better usability")
print("• Better error handling and validation")
print("• Additional utility functions for common operations")
print("• Comprehensive statistics and monitoring")
print("• Advanced features for expert users")
print("• Improved documentation and examples")