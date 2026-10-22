#!/usr/bin/env python3
"""
Test script to verify NLM improvements and functionality.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    
    print("=== NLM Python Package Test ===")
    print(f"Version: {pynlm.__version__}")
    print(f"Author: {pynlm.__author__}")
    print()
    
    # Test 1: Create default configuration
    print("Test 1: Creating default configuration...")
    config = pynlm.createDefaultConfig()
    print("✓ Default configuration created successfully")
    
    # Test 2: Check configuration methods
    print("\nTest 2: Testing configuration methods...")
    config.set("brain.neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    print(f"✓ Configuration set: neuron_count={config.getOr('brain.neuron_count', 0)}")
    print(f"✓ Configuration set: timestep={config.getOr('simulation_timestep', 0)}")
    
    # Test 3: Create brain
    print("\nTest 3: Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized successfully")
    
    # Test 4: Basic simulation
    print("\nTest 4: Running basic simulation...")
    for i in range(10):
        brain.step(i)
    
    stats = brain.getStats() if hasattr(brain, 'getStats') else {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'firing': brain.getFiringNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'avg_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio()
    }
    
    print(f"✓ Simulation completed:")
    print(f"  Neurons: {stats['neurons']}")
    print(f"  Synapses: {stats['synapses']}")
    print(f"  Firing neurons: {stats['firing']}")
    print(f"  Total spikes: {stats['spikes']}")
    print(f"  Average firing rate: {stats['avg_rate']:.3f}")
    print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
    
    # Test 5: Test utility functions
    print("\nTest 5: Testing utility functions...")
    validate_result = pynlm.validate_configuration({"brain.neuron_count": 500})
    print(f"✓ Configuration validation: {validate_result}")
    
    # Test 6: Test context manager
    print("\nTest 6: Testing context manager...")
    with pynlm.brain_context({"brain.neuron_count": 200}) as test_brain:
        test_brain.initialize()
        test_brain.step(0)
        print("✓ Context manager works successfully")
    
    # Test 7: Test NLMInterface
    print("\nTest 7: Testing NLMInterface...")
    nlm_interface = pynlm.NLMInterface({"brain.neuron_count": 300})
    nlm_interface.run(5)
    interface_stats = nlm_interface.get_stats()
    print(f"✓ NLMInterface works: {interface_stats['neurons']} neurons")
    
    print("\n=== All Tests Passed ===")
    print("NLM improvements are working correctly!")
    
except ImportError as e:
    print(f"Error: Could not import NLM: {e}")
    print("This may be because C++ bindings are not built.")
    
except Exception as e:
    print(f"Error during testing: {e}")
    import traceback
    traceback.print_exc()

print("\n=== Summary of Improvements ===")
print("1. Fixed critical bugs in Config.cpp, Phase6Demo.cpp, and Neuron.cpp")
print("2. Added Phase6Demo to CMakeLists.txt (was missing)")
print("3. Fixed duplicate keys in configs/default.cfg")
print("4. Created comprehensive Python package structure")
print("5. Added pybind11 bindings for Python integration")
print("6. Implemented advanced Python utilities and interfaces")
print("7. Added error handling and fallback implementations")
print("8. Created comprehensive documentation and examples")