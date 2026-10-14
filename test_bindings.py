#!/usr/bin/env python3
"""
NLM Python Bindings - Quick Test Script

This script performs a quick test of the NLM Python bindings to verify
that they are working correctly.
"""

import sys
import os

# Add the workspace root to the Python path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

print("Testing NLM Python Bindings")
print("=" * 60)

# Test 1: Import the main module
try:
    import pynlm
    print("✓ Successfully imported pynlm module")
    print(f"  Version: {pynlm.__version__}")
    print(f"  Author: {pynlm.__author__}")
    print(f"  Description: {pynlm.__description__}")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("\nTrying to import from python package...")
    
    # Try to import from the python package
    try:
        from python import pynlm as python_pynlm
        print("✓ Successfully imported from python package")
        print(f"  Version: {python_pynlm.__version__}")
        
        # Use the python package version
        pynlm = python_pynlm
        
    except ImportError as e2:
        print(f"✗ Failed to import from python package: {e2}")
        print("\nThe NLM Python bindings are not available.")
        print("Please build the Python bindings first.")
        sys.exit(1)

# Test 2: Create a default configuration
try:
    config = pynlm.createDefaultConfig()
    print("✓ Created default configuration")
    print(f"  Config keys: {len(config.getKeys())}")
    
except Exception as e:
    print(f"✗ Failed to create configuration: {e}")
    sys.exit(1)

# Test 3: Create and initialize a brain
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
except Exception as e:
    print(f"✗ Failed to create brain: {e}")
    sys.exit(1)

# Test 4: Run simulation steps
try:
    initial_spikes = brain.getTotalSpikeCount()
    
    for step in range(5):
        brain.step(step)
    
    final_spikes = brain.getTotalSpikeCount()
    print("✓ Successfully ran simulation steps")
    print(f"  Steps run: 5")
    print(f"  Spike change: {final_spikes - initial_spikes}")
    
except Exception as e:
    print(f"✗ Failed to run simulation steps: {e}")
    sys.exit(1)

# Test 5: Create and configure a world
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ Successfully created and configured world")
    print(f"  Dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getAgentBody().getVisionWidth()}x{world.getAgentBody().getVisionHeight()}")
    
except Exception as e:
    print(f"✗ Failed to create world: {e}")
    sys.exit(1)

# Test 6: Create an agent brain
try:
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Successfully created and initialized agent brain")
    
except Exception as e:
    print(f"✗ Failed to create agent brain: {e}")
    sys.exit(1)

# Test 7: Test agent features
try:
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("✓ Successfully enabled agent features")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    
except Exception as e:
    print(f"✗ Failed to test agent features: {e}")
    sys.exit(1)

# Test 8: Test enumeration access
try:
    # Test that enumerations are available
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    
    print("✓ Successfully accessed enumerations")
    print(f"  NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
    print(f"  ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
    print(f"  DevelopmentalStage.Initial: {pynlm.DevelopmentalStage.Initial}")
    
except Exception as e:
    print(f"✗ Failed to access enumerations: {e}")
    sys.exit(1)

# Test 9: Test constants access
try:
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("✓ Successfully accessed constants")
    print(f"  INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
    print(f"  INVALID_SYNAPSE_ID: {pynlm.INVALID_SYNAPSE_ID}")
    
except Exception as e:
    print(f"✗ Failed to access constants: {e}")
    sys.exit(1)

# Test 10: Test convenience functions
try:
    # Test quick_test
    brain2 = pynlm.quick_test()
    print("✓ Successfully ran quick_test()")
    print(f"  Brain created with {brain2.getTotalNeuronCount()} neurons")
    
except Exception as e:
    print(f"✗ Failed to run convenience functions: {e}")
    sys.exit(1)

print("\n" + "=" * 60)
print("All tests passed successfully!")
print("The NLM Python bindings are working correctly.")
print("\nKey features verified:")
print("  ✓ Module import and metadata")
print("  ✓ Configuration creation")
print("  ✓ Brain creation and initialization")
print("  ✓ Simulation execution")
print("  ✓ World creation and configuration")
print("  ✓ Agent brain creation and initialization")
print("  ✓ Agent feature management")
print("  ✓ Enumeration access")
print("  ✓ Constants access")
print("  ✓ Convenience functions")
print("\nThe NLM Python bindings are ready for use!")
