#!/usr/bin/env python3
"""
Test script to verify the NLM Python bindings work correctly.
"""

import sys
import os

# Add the workspace root to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

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

# Test 2: Check if we have the basic API
try:
    # Check for basic functions
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    
    print("✓ Basic API functions available")
    
except Exception as e:
    print(f"✗ Basic API test failed: {e}")
    sys.exit(1)

# Test 3: Create a default configuration
try:
    config = pynlm.createDefaultConfig()
    print("✓ Created default configuration")
    print(f"  Config keys: {len(config.getKeys())}")
    
except Exception as e:
    print(f"✗ Failed to create configuration: {e}")
    sys.exit(1)

# Test 4: Create and initialize a brain
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
except Exception as e:
    print(f"✗ Failed to create brain: {e}")
    sys.exit(1)

# Test 5: Run simulation steps
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

# Test 6: Test convenience functions
try:
    # Test quick_test
    brain2 = pynlm.quick_test()
    print("✓ Successfully ran quick_test()")
    print(f"  Brain created with {brain2.getTotalNeuronCount()} neurons")
    
    # Test create_simulation
    sim = pynlm.create_simulation()
    print("✓ Successfully ran create_simulation()")
    print(f"  Created simulation with {sim['brain'].getTotalNeuronCount()} neurons")
    
    # Test run_agent_episode
    results = pynlm.run_agent_episode(steps=5, enable_features=False)
    print("✓ Successfully ran run_agent_episode()")
    print(f"  Completed {results['steps_run']} steps")
    
except Exception as e:
    print(f"✗ Failed to run convenience functions: {e}")
    sys.exit(1)

# Test 7: Test enumerations
try:
    # Test that enumerations are available
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    
    print("✓ Enumerations available")
    print(f"  NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
    print(f"  ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
    print(f"  DevelopmentalStage.Initial: {pynlm.DevelopmentalStage.Initial}")
    
except Exception as e:
    print(f"✗ Failed to test enumerations: {e}")
    sys.exit(1)

# Test 8: Test constants
try:
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("✓ Constants available")
    print(f"  INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
    
except Exception as e:
    print(f"✗ Failed to test constants: {e}")
    sys.exit(1)

print("\n" + "=" * 60)
print("All tests passed successfully!")
print("The NLM Python bindings are working correctly.")
print("\nKey features verified:")
print("  ✓ Module import and metadata")
print("  ✓ Configuration creation")
print("  ✓ Brain creation and initialization")
print("  ✓ Simulation execution")
print("  ✓ Convenience functions")
print("  ✓ Enumerations")
print("  ✓ Constants")
print("\nThe NLM Python bindings are ready for use!")
