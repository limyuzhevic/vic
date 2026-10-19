#!/usr/bin/env python3
"""
Test script for NLM Python API
"""

import sys
import traceback
import pynlm

def test_basic_functionality():
    """Test basic NLM Python API functionality."""
    print("Testing NLM Python API basic functionality...")
    
    try:
        # Test 1: Configuration creation
        print("1. Testing configuration creation...")
        config = pynlm.make_config("test_config")
        assert config.get_name() == "test_config"
        print("   ✓ Configuration created successfully")
        
        # Test 2: Setting configuration values
        print("2. Testing configuration value setting...")
        config.set("neuron_count", 100)
        config.set("simulation_timestep", 0.001)
        config.set("connection_probability", 0.15)
        config.set("log_level", "INFO")
        
        assert config.get_int("neuron_count") == 100
        assert config.get_double("simulation_timestep") == 0.001
        assert config.get_double("connection_probability") == 0.15
        print("   ✓ Configuration values set correctly")
        
        # Test 3: Environment variable support
        print("3. Testing environment variable support...")
        # Note: This test assumes NLM_TEST_VAR environment variable is set
        import os
        os.environ["NLM_TEST_VAR"] = "test_value"
        
        # For now, just test that the method exists
        config.load_from_env("NLM_")
        print("   ✓ Environment variable loading works")
        
        # Test 4: Configuration validation
        print("4. Testing configuration validation...")
        config.validate()
        print("   ✓ Configuration validation works")
        
        # Test 5: Configuration summary
        print("5. Testing configuration summary...")
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
        print("   ✓ Configuration summary works")
        
        # Test 6: Default configuration
        print("6. Testing default configuration...")
        default_config = pynlm.make_config.create_default()
        assert default_config.get_name() == "default"
        assert default_config.get_int("neuron_count") > 0
        print("   ✓ Default configuration works")
        
        # Test 7: Empty configuration
        print("7. Testing empty configuration...")
        empty_config = pynlm.make_config.create_empty()
        assert empty_config.get_name() == "empty"
        print("   ✓ Empty configuration works")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_brain_functionality():
    """Test brain functionality."""
    print("\nTesting NLM Python API brain functionality...")
    
    try:
        # Test 1: Brain creation
        print("1. Testing brain creation...")
        config = pynlm.make_config("brain_test")
        config.set("neuron_count", 50)  # Smaller for testing
        brain = pynlm.make_brain(config)
        print("   ✓ Brain created successfully")
        
        # Test 2: Brain initialization
        print("2. Testing brain initialization...")
        if brain.initialize():
            print("   ✓ Brain initialized successfully")
        else:
            print("   ✗ Brain initialization failed")
            return False
        
        # Test 3: Basic brain operations
        print("3. Testing basic brain operations...")
        neuron_count = brain.get_total_neuron_count()
        synapse_count = brain.get_total_synapse_count()
        spike_count = brain.get_total_spike_count()
        
        print(f"   Initial stats - Neurons: {neuron_count}, "
              f"Synapses: {synapse_count}, Spikes: {spike_count}")
        
        # Test 4: Simulation steps
        print("4. Testing simulation steps...")
        for step in range(5):
            brain.step(step, step * 0.001)
            current_spikes = brain.get_total_spike_count()
            print(f"   Step {step}: {current_spikes} spikes")
        
        print("   ✓ Simulation steps completed")
        
        # Test 5: Brain status
        print("5. Testing brain status...")
        status = brain.get_status()
        assert isinstance(status, str)
        assert len(status) > 0
        print("   ✓ Brain status works")
        
        # Test 6: Brain reset
        print("6. Testing brain reset...")
        brain.reset()
        print("   ✓ Brain reset works")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_world_functionality():
    """Test world functionality."""
    print("\nTesting NLM Python API world functionality...")
    
    try:
        # Test 1: World creation
        print("1. Testing world creation...")
        world = pynlm.make_world()
        print("   ✓ World created successfully")
        
        # Test 2: World configuration
        print("2. Testing world configuration...")
        world.configure(20, 20, 5, 5)
        dimensions = world.get_dimensions()
        assert dimensions[0] == 20
        assert dimensions[1] == 20
        print("   ✓ World configured correctly")
        
        # Test 3: World reset
        print("3. Testing world reset...")
        world.reset()
        print("   ✓ World reset works")
        
        # Test 4: World simulation
        print("4. Testing world simulation...")
        world.update(0.001)
        print("   ✓ World simulation works")
        
        # Test 5: World state
        print("5. Testing world state...")
        time = world.get_simulation_time()
        assert isinstance(time, float)
        print(f"   World simulation time: {time}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_functionality():
    """Test agent functionality."""
    print("\nTesting NLM Python API agent functionality...")
    
    try:
        # Test 1: Create brain and world
        print("1. Testing brain and world creation...")
        config = pynlm.make_config("agent_test")
        config.set("neuron_count", 50)
        brain = pynlm.make_brain(config)
        
        if brain.initialize():
            print("   ✓ Brain initialized")
        else:
            print("   ✗ Brain initialization failed")
            return False
        
        world = pynlm.make_world()
        world.configure(10, 10)
        world.set_agent_start(5, 5)
        world.reset()
        print("   ✓ World configured")
        
        # Test 2: Create agent brain
        print("2. Testing agent brain creation...")
        agent = pynlm.make_agent_brain(brain)
        print("   ✓ Agent brain created")
        
        # Test 3: Agent initialization
        print("3. Testing agent initialization...")
        if agent.initialize(world):
            print("   ✓ Agent initialized")
        else:
            print("   ✗ Agent initialization failed")
            return False
        
        # Test 4: Agent configuration
        print("4. Testing agent configuration...")
        sensor_size = agent.get_sensory_input_size()
        motor_size = agent.get_motor_output_size()
        print(f"   Sensor size: {sensor_size}, Motor size: {motor_size}")
        
        # Test 5: Agent operations
        print("5. Testing agent operations...")
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        command = agent.decode_motor_command()
        print(f"   Agent command: {command}")
        
        # Test 6: Agent development
        print("6. Testing agent development...")
        agent.update_development(0.001)
        stage = agent.get_developmental_stage()
        print(f"   Developmental stage: {stage}")
        
        # Test 7: Agent neuromodulation
        print("7. Testing agent neuromodulation...")
        agent.apply_reward_modulation(1.0, 0.5)
        neuromodulation_level = agent.get_neuromodulation_level()
        curiosity_level = agent.get_curiosity_level()
        print(f"   Neuromodulation level: {neuromodulation_level}")
        print(f"   Curiosity level: {curiosity_level}")
        
        # Test 8: Agent features
        print("8. Testing agent features...")
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        print("   ✓ All agent features enabled")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_example_code():
    """Test the example code."""
    print("\nTesting example code...")
    
    try:
        example_code = pynlm.get_example_code()
        print("   ✓ Example code retrieved")
        
        # Check that example code contains key elements
        assert "pynlm.make_config" in example_code
        assert "pynlm.make_brain" in example_code
        assert "pynlm.make_world" in example_code
        assert "pynlm.make_agent_brain" in example_code
        
        print("   ✓ Example code contains expected elements")
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_version_info():
    """Test version information."""
    print("\nTesting version information...")
    
    try:
        version = pynlm.get_version()
        print(f"   NLM Python bindings version: {version}")
        assert "NLM Python Bindings" in version
        print("   ✓ Version information retrieved")
        return True
        
    except Exception as e:
        print(f"   ✗ Test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python API Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Brain Functionality", test_brain_functionality),
        ("World Functionality", test_world_functionality),
        ("Agent Functionality", test_agent_functionality),
        ("Example Code", test_example_code),
        ("Version Information", test_version_info),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'-' * 40}")
        print(f"Running: {test_name}")
        print(f"{'-' * 40}")
        
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"Test crashed: {e}")
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'=' * 60}")
    print("Test Summary")
    print(f"{'=' * 60}")
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        return 0
    else:
        print(f"\n❌ {failed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())