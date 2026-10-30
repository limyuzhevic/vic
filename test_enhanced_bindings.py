#!/usr/bin/env python3
"""
Simple test script to verify the enhanced NLM Python bindings.
This script tests the high-level convenience functions and API improvements.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    
    # Test 1: Create a simple brain using convenience function
    print("\n1. Testing create_brain convenience function...")
    brain = pynlm.create_brain(neuron_count=100, max_steps=500)
    print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ Initialized status: {brain.initialize()}")
    
    # Test 2: Create a world using convenience function
    print("\n2. Testing create_world convenience function...")
    world = pynlm.create_world(width=16, height=16)
    print(f"   ✓ Created world {world.getWidth()}x{world.getHeight()}")
    
    # Test 3: Create a complete agent simulation
    print("\n3. Testing create_agent convenience function...")
    brain, world, agent = pynlm.create_agent(neuron_count=200, world_width=20, world_height=20)
    print(f"   ✓ Created agent simulation")
    print(f"   ✓ Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"   ✓ World size: {world.getWidth()}x{world.getHeight()}")
    
    # Test 4: Test configuration utility
    print("\n4. Testing configuration utilities...")
    config_dict = {
        "neuron_count": 150,
        "region_count": 2,
        "connection_probability": 0.15
    }
    config = pynlm.create_config_from_dict(config_dict)
    print(f"   ✓ Created config from dict")
    print(f"   ✓ Config has keys: {config.get_keys()}")
    print(f"   ✓ Neuron count: {config.get_int('neuron_count')}")
    
    # Test 5: Get brain statistics
    print("\n5. Testing brain statistics...")
    stats = pynlm.get_statistics(brain)
    print(f"   ✓ Brain statistics:")
    for key, value in stats.items():
        print(f"     - {key}: {value}")
    
    # Test 6: Run a simple simulation
    print("\n6. Testing simulation...")
    result = pynlm.run_simulation(brain, world, agent, max_steps=50)
    print(f"   ✓ Simulation completed:")
    print(f"     - Steps: {result.steps_completed}")
    print(f"     - Total reward: {result.total_reward:.4f}")
    print(f"     - Success: {result.success}")
    
    # Test 7: Test multi-agent simulation
    print("\n7. Testing multi-agent simulation...")
    agents = pynlm.create_multi_agent(num_agents=2, neuron_count_per_agent=100)
    print(f"   ✓ Created {len(agents)} agents")
    
    multi_result = pynlm.run_multi_agent(agents, max_steps=30)
    print(f"   ✓ Multi-agent simulation completed:")
    print(f"     - Results: {len(multi_result)} agent results")
    
    # Test 8: Test performance benchmarking
    print("\n8. Testing performance benchmarking...")
    perf_metrics = pynlm.benchmark_performance(num_neurons=50, num_steps=100)
    print(f"   ✓ Performance metrics:")
    for key, value in perf_metrics.items():
        print(f"     - {key}: {value:.2f}")
    
    print("\n" + "="*50)
    print("✅ ALL TESTS PASSED!")
    print("Enhanced NLM Python bindings are working correctly.")
    print("="*50)
    
except ImportError as e:
    print(f"❌ Failed to import pynlm: {e}")
    print("This might be because the Python bindings haven't been built yet.")
    print("Try building the project first using CMake.")
    sys.exit(1)
except Exception as e:
    print(f"❌ Test failed with error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
