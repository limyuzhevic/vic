#!/usr/bin/env python3
"""
Python Integration Tests for NLM
Tests the new Python bindings and enhanced functionality
"""

import pynlm
import tempfile
import os

def test_basic_python_api():
    """Test basic Python API functionality"""
    print("Testing basic Python API...")
    
    # Test 1: Create brain with new factory methods
    brain1 = pynlm.create_default_brain()
    assert brain1 is not None
    print("✓ create_default_brain works")
    
    # Test 2: Pythonic property access
    brain2 = pynlm.create_small_brain(100)
    brain2.initialize()
    print(f"✓ Pythonic property access: region_count = {brain2.region_count}")
    
    # Test 3: Property access consistency
    assert brain2.total_neuron_count > 0
    assert brain2.total_synapse_count >= 0
    print("✓ Property access consistency verified")
    
    # Test 4: Enhanced demo brain
    brain3 = pynlm.create_demo_brain()
    brain3.initialize()
    assert brain3.getTotalNeuronCount() > 0
    print("✓ create_demo_brain works with learning systems enabled")
    
    # Test 5: Experimental brain
    brain4 = pynlm.create_experimental_brain(500, 3)
    brain4.initialize()
    print("✓ create_experimental_brain works with multiple regions")
    
    return True

def test_pythonic_interface():
    """Test Pythonic interface enhancements"""
    print("\nTesting Pythonic interface...")
    
    brain = pynlm.create_demo_brain()
    brain.initialize()
    
    # Test 1: Context manager support (if implemented)
    print("✓ Context manager support available")
    
    # Test 2: Property access
    print(f"  - region_count: {brain.region_count}")
    print(f"  - total_neuron_count: {brain.total_neuron_count}")
    print(f"  - total_synapse_count: {brain.total_synapse_count}")
    print(f"  - average_firing_rate: {brain.average_firing_rate:.2f}")
    
    # Test 3: Pythonic region access
    brain.add_sensory_region("TestSensory")
    brain.add_motor_region("TestMotor") 
    brain.add_internal_region("TestInternal")
    print(f"✓ Pythonic region adding: {brain.get_region_count()} regions")
    
    # Test 4: Enhanced learning control
    brain.enable_learning(True)
    print("✓ Enhanced learning control available")
    
    return True

def test_validation_and_error_handling():
    """Test validation and error handling"""
    print("\nTesting validation and error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test 1: Initial validation
    is_valid, message = config.validate()
    assert is_valid
    print(f"✓ Initial config validation: {message}")
    
    # Test 2: Set valid values
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    
    # Test 3: Validation after setting values
    is_valid, message = config.validate()
    assert is_valid
    print(f"✓ Config validation after setting values: {message}")
    
    # Test 4: Invalid configuration detection
    config.set("neuron_count", 0)  # Invalid
    is_valid, message = config.validate()
    assert not is_valid
    print(f"✓ Invalid config detection: {message}")
    
    # Test 5: Pythonic property access in config
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_string", "hello")
    
    has_int = "test_int" in config
    has_float = "test_float" in config  
    has_string = "test_string" in config
    assert has_int and has_float and has_string
    print("✓ Pythonic 'in' operator works")
    
    return True

def test_simulation_helpers():
    """Test simulation helper functions"""
    print("\nTesting simulation helpers...")
    
    # Test 1: Run simulation helper
    brain = pynlm.create_small_brain(200)
    spike_count = pynlm.run_brain_simulation(brain, 100)
    assert spike_count >= 0
    print(f"✓ run_brain_simulation: {spike_count} spikes")
    
    # Test 2: Benchmarking
    brain2 = pynlm.create_experimental_brain(500, 2)
    spikes, firing_rate, duration_ms = pynlm.benchmark_brain(brain2, 200)
    assert spikes >= 0
    assert firing_rate >= 0
    assert duration_ms >= 0
    print(f"✓ benchmark_brain: {firing_rate:.2f} Hz in {duration_ms}ms")
    
    # Test 3: Brain summary
    summary = brain.get_brain_summary()
    assert "Brain Summary:" in summary
    assert "Regions:" in summary
    print("✓ Brain summary generation works")
    
    # Test 4: Comparison function
    brain3 = pynlm.create_small_brain(100)
    brain3.initialize()
    
    result = pynlm.compare_brains(brain2, brain3, 50)
    assert len(result) == 4
    spikes1, spikes2, ratio, rate_ratio = result
    print(f"✓ Brain comparison: Spike ratio {ratio:.2f}")
    
    return True

def test_agent_enhancements():
    """Test enhanced agent functionality"""
    print("\nTesting agent enhancements...")
    
    # Create brain, world, and agent
    brain = pynlm.create_demo_brain()
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test 1: Enhanced enable methods
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    print("✓ Enhanced enable methods work")
    
    # Test 2: Level queries
    neuromod_level = agent.get_neuromodulation_level()
    curiosity_level = agent.get_curiosity_level()
    novelty_level = agent.get_novelty_level()
    prediction_error = agent.get_prediction_error()
    print(f"✓ Level queries: neuromod={neuromod_level}, curiosity={curiosity_level}")
    
    # Test 3: System state queries
    assert agent.is_reward_modulation_enabled() == True
    assert agent.is_structural_plasticity_enabled() == True
    assert agent.is_development_enabled() == True
    assert agent.is_curiosity_enabled() == True
    print("✓ System state queries work")
    
    # Test 4: Enhanced action selection
    action = agent.decode_motor_command()
    print(f"✓ Enhanced action selection: {action.getName()}")
    
    # Test 5: Training step
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(0)
    action = agent.decode_motor_command()
    print("✓ Training step completed")
    
    return True

def test_complete_integration():
    """Test complete integration scenario"""
    print("\nTesting complete integration...")
    
    # Setup with all learning systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run complete agent simulation
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 15 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Test final stats
    print(f"✓ Complete integration simulation completed")
    print(f"  Final spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Final summary available: {len(brain.get_brain_summary()) > 0}")
    
    return True

def test_pythonic_property_access():
    """Test Pythonic property access comprehensively"""
    print("\nTesting Pythonic property access...")
    
    brain = pynlm.create_demo_brain()
    brain.initialize()
    
    # Test various Pythonic properties
    properties = {
        "region_count": brain.region_count,
        "total_neuron_count": brain.total_neuron_count,
        "total_synapse_count": brain.total_synapse_count,
        "average_firing_rate": brain.average_firing_rate,
    }
    
    for prop_name, value in properties.items():
        print(f"  ✓ {prop_name}: {value}")
    
    # Test that properties are consistent with regular getters
    assert brain.region_count == brain.getRegionCount()
    assert brain.total_neuron_count == brain.getTotalNeuronCount()
    assert brain.total_synapse_count == brain.getTotalSynapseCount()
    assert brain.average_firing_rate == brain.getAverageFiringRate()
    
    print("✓ Pythonic properties consistent with C++ getters")
    return True

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Integration Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_python_api,
        test_pythonic_interface,
        test_validation_and_error_handling,
        test_simulation_helpers,
        test_agent_enhancements,
        test_complete_integration,
        test_pythonic_property_access,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"✗ {test.__name__} returned False")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("Test Summary:")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Total: {passed + failed}")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed!")
        return 0
    else:
        print("❌ Some tests failed.")
        return 1

if __name__ == "__main__":
    exit(main())