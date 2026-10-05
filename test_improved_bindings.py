#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings
Tests context managers, convenience methods, and Pythonic interfaces
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_bindings():
    """Test basic functionality of the improved bindings"""
    print("\n=== Testing Basic Bindings ===")
    
    # Test 1: Create default config
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    # Test 3: Initialize brain
    brain.initialize()
    print("✓ Initialized brain")
    
    # Test 4: Check brain stats
    stats = brain.getBrainStats()
    print(f"✓ Got brain stats (length: {len(stats)})")
    
    # Test 5: Check statistics
    neuron_count = brain.getTotalNeuronCount()
    print(f"✓ Total neurons: {neuron_count}")
    
    # Test 6: Test context manager
    with brain:
        print("✓ Used brain as context manager")
        # Brain should be initialized in context
        assert brain.getTotalNeuronCount() > 0
    
    print("✓ Context manager test passed")
    
    return brain

def test_simulation_convenience():
    """Test simulation convenience methods"""
    print("\n=== Testing Simulation Convenience Methods ===")
    
    # Create a simple simulation setup
    brain, world, agent = pynlm.createSimpleSimulation()
    print("✓ Created simple simulation setup")
    
    # Test runForSteps
    brain.runForSteps(10, verbose=False)
    print("✓ Ran simulation for 10 steps")
    
    # Test getBrainStats
    stats = brain.getBrainStats()
    print("✓ Got enhanced brain stats")
    
    # Test region iteration
    region_count = brain.getRegionCount()
    print(f"✓ Found {region_count} regions")
    
    # Test safe property access
    safe_ratio = brain.getExcitationInhibitionRatioSafe()
    print(f"✓ Safe excitation/inhibition ratio: {safe_ratio}")
    
    # Test learning progress
    progress = brain.getLearningProgress()
    print(f"✓ Learning progress: {progress}")
    
    return brain, world, agent

def test_error_handling():
    """Test enhanced error handling"""
    print("\n=== Testing Error Handling ===")
    
    # Test null checks
    try:
        pynlm.safeBrainAccess(None, "test")
        print("✗ Should have thrown error for null brain")
        return False
    except Exception as e:
        print(f"✓ Properly handled null brain access: {type(e).__name__}")
    
    # Test checkBrainHealth with invalid brain
    try:
        pynlm.checkBrainHealth(None)
        print("✗ Should have thrown error for null brain health check")
        return False
    except Exception as e:
        print(f"✓ Properly handled invalid brain health check: {type(e).__name__}")
    
    return True

def test_data_export():
    """Test data export functionality"""
    print("\n=== Testing Data Export ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 50, pynlm.ConfigSource.Default)
    config.set("region_count", 2, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuron data export
    neuron_data = brain.exportNeuronData()
    print(f"✓ Exported neuron data (length: {len(neuron_data)})")
    
    # Test synapse data export
    synapse_data = brain.exportSynapseData()
    print(f"✓ Exported synapse data (length: {len(synapse_data)})")
    
    return brain

def test_agent_integration():
    """Test agent brain integration with world"""
    print("\n=== Testing Agent Integration ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)
    world.setAgentStart(50.0, 50.0)
    print("✓ Created and configured world")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain")
    
    # Initialize with world
    agent.initialize(world)
    print("✓ Initialized agent with world")
    
    # Test agent's methods
    input_size = agent.getSensoryInputSize()
    output_size = agent.getMotorOutputSize()
    print(f"✓ Agent input size: {input_size}, output size: {output_size}")
    
    # Test agent context manager
    with agent:
        print("✓ Used agent as context manager")
        # Agent should be in valid state
        assert agent.getBrain() is not None
    
    print("✓ Agent context manager test passed")
    return brain, world, agent

def test_simulation_run():
    """Test complete simulation run"""
    print("\n=== Testing Complete Simulation Run ===")
    
    # Use the convenience function to run a simulation
    result = pynlm.runSimulation(pynlm.createBrain(pynlm.createDefaultConfig()), 50)
    print("✓ Ran complete simulation")
    
    # Test world simulation
    world = pynlm.createSimpleWorld()
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize world and agent
    world.configure(80, 80, 16, 16)
    world.setAgentStart(40.0, 40.0)
    agent.initialize(world)
    
    # Run world simulation
    pynlm.runWorldSimulation(world, agent, 20, verbose=False)
    print("✓ Ran world simulation")
    
    # Get world stats
    world_stats = pynlm.getWorldStats(world)
    print(f"✓ Got world stats (length: {len(world_stats)})")
    
    return brain, world, agent

def run_all_tests():
    """Run all tests and report results"""
    print("Testing improved NLM Python bindings")
    print("=" * 50)
    
    tests = [
        ("Basic Bindings", test_basic_bindings),
        ("Simulation Convenience", test_simulation_convenience),
        ("Error Handling", test_error_handling),
        ("Data Export", test_data_export),
        ("Agent Integration", test_agent_integration),
        ("Complete Simulation", test_simulation_run),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            test_func()
            print(f"\n✓ {test_name} PASSED")
            passed += 1
        except Exception as e:
            print(f"\n✗ {test_name} FAILED: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("All tests passed! ✓")
        return True
    else:
        print(f"Some tests failed. Please check the output above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)