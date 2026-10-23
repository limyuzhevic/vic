#!/usr/bin/env python3
"""Test script to verify Python bindings and identify improvements needed."""

import sys
import traceback

print("Testing NLM Python Bindings...")
print("=" * 50)

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {pynlm.__version__ if hasattr(pynlm, '__version__') else 'Unknown'}")
except Exception as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

# Test basic configuration
try:
    config = pynlm.createDefaultConfig()
    print("✓ Successfully created default config")
    config.set("brain.neuron_count", 100)
    print(f"✓ Set neuron_count to {config.getOr('brain.neuron_count', 0)}")
except Exception as e:
    print(f"✗ Failed to test config: {e}")
    traceback.print_exc()

# Test Brain creation
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Successfully created and initialized brain")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Synapse count: {brain.getTotalSynapseCount()}")
except Exception as e:
    print(f"✗ Failed to test brain: {e}")
    traceback.print_exc()

# Test world creation
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    print("✓ Successfully created and configured world")
    print(f"  Dimensions: {world.getWidth()}x{world.getHeight()}")
except Exception as e:
    print(f"✗ Failed to test world: {e}")
    traceback.print_exc()

# Test agent creation
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    print("✓ Successfully created agent brain")
    
    # Test some agent methods
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
except Exception as e:
    print(f"✗ Failed to test agent: {e}")
    traceback.print_exc()

# Test enum values
try:
    print("\nTesting enum values:")
    neuron_types = [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory, pynlm.NeuronType.Sensory]
    print(f"  Neuron types: {[str(t) for t in neuron_types]}")
    
    # Test motor command enum
    try:
        print(f"  MotorCommand.MoveForward: {pynlm.MotorCommand.MoveForward}")
    except AttributeError:
        print("  ✗ MotorCommand.MoveForward not found")
    
    # Test action type enum  
    try:
        print(f"  ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
    except AttributeError:
        print("  ✗ ActionType.MoveForward not found")
        
except Exception as e:
    print(f"✗ Failed to test enums: {e}")

print("\n" + "=" * 50)
print("Test completed.")
print("\nIMPROVEMENTS NEEDED:")
print("1. Fix enum mapping inconsistencies")
print("2. Add missing enum values (MotorCommand, ActionType)")
print("3. Improve Action class Python bindings")
print("4. Add better error handling and documentation")
print("5. Add performance profiling capabilities")
