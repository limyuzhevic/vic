#!/usr/bin/env python3
"""
Quick Test Script for Enhanced NLM Python Bindings

This script tests the enhanced NLM Python bindings to ensure they work correctly.
"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    # Test 1: Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Ran simulation with {brain.getTotalSpikeCount()} spikes")
    
    # Test 3: Check development stage
    print(f"✓ Development stage: {brain.getDevelopmentalStage()}")
    
    return True

def test_enhanced_config():
    """Test enhanced configuration system"""
    print("\nTesting enhanced configuration system...")
    
    # Test 1: Create configuration from preset
    config = pynlm.EnhancedConfig.createFromPreset("basic")
    print(f"✓ Created basic configuration with {config.get('neuron_count')} neurons")
    
    # Test 2: Validate configuration
    if config.isValid():
        print("✓ Basic configuration is valid")
    else:
        print("✗ Basic configuration has errors:")
        for error in config.getValidationErrors():
            print(f"  - {error}")
        return False
    
    # Test 3: Create research configuration
    config2 = pynlm.EnhancedConfig.createFromPreset("research")
    print(f"✓ Created research configuration with {config2.get('neuron_count')} neurons")
    
    # Test 4: Validate research configuration
    if config2.isValid():
        print("✓ Research configuration is valid")
    else:
        print("✗ Research configuration has errors:")
        for error in config2.getValidationErrors():
            print(f"  - {error}")
        return False
    
    # Test 5: Create invalid configuration
    config3 = pynlm.EnhancedConfig.createFromPreset("basic")
    config3.set("neuron_count", -1)  # Invalid
    
    try:
        pynlm.validateConfig(config3)
        print("✗ Invalid configuration should have raised exception")
        return False
    except Exception:
        print("✓ Invalid configuration correctly raised exception")
    
    return True

def test_context_managers():
    """Test context managers"""
    print("\nTesting context managers...")
    
    # Test 1: Agent simulation context manager
    try:
        with pynlm.agent_simulation(preset="basic") as sim:
            sim.runSteps(10)
            print("✓ Agent simulation context manager works")
            
            stats = sim.getStatistics()
            print(f"  Statistics: {stats['neurons']} neurons, {stats['total_spikes']} spikes")
    except Exception as e:
        print(f"✗ Agent simulation context manager failed: {e}")
        return False
    
    # Test 2: World simulation context manager
    try:
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        
        with pynlm.worldSimulation(world) as world_sim:
            world_sim.runSteps(5)
            print("✓ World simulation context manager works")
            print(f"  World time: {world_sim.getTime()}")
    except Exception as e:
        print(f"✗ World simulation context manager failed: {e}")
        return False
    
    return True

def test_simulation_utilities():
    """Test simulation utilities"""
    print("\nTesting simulation utilities...")
    
    # Test 1: Run agent episode
    try:
        results = pynlm.SimulationUtils.runAgentEpisode(max_steps=20)
        print(f"✓ Run agent episode: total_reward={results['total_reward']:.2f}, "
              f"steps={results['steps_completed']}")
    except Exception as e:
        print(f"✗ Run agent episode failed: {e}")
        return False
    
    # Test 2: Batch run episodes
    try:
        results = pynlm.SimulationUtils.batchRunEpisodes(3, 10)
        print(f"✓ Batch run episodes: {len(results)} episodes completed")
        
        total_reward = sum(r["total_reward"] for r in results)
        print(f"  Total reward across episodes: {total_reward:.2f}")
    except Exception as e:
        print(f"✗ Batch run episodes failed: {e}")
        return False
    
    return True

def test_validation_utilities():
    """Test validation utilities"""
    print("\nTesting validation utilities...")
    
    # Test 1: Validate basic configuration
    config = pynlm.EnhancedConfig.createFromPreset("basic")
    
    # Use validateConfig (should raise exception on error)
    try:
        pynlm.validateConfig(config)
        print("✓ validateConfig works correctly")
    except Exception as e:
        print(f"✗ validateConfig failed: {e}")
        return False
    
    # Test 2: Get validation errors
    errors = pynlm.getConfigValidationErrors(config)
    if not errors:
        print("✓ getConfigValidationErrors works correctly")
    else:
        print(f"✗ getConfigValidationErrors returned errors: {errors}")
        return False
    
    return True

def test_configuration_diffing():
    """Test configuration diffing"""
    print("\nTesting configuration diffing...")
    
    # Create two different configurations
    config1 = pynlm.EnhancedConfig.createFromPreset("basic")
    config2 = pynlm.EnhancedConfig.createFromPreset("research")
    
    # Get diff
    diff = config1.diff(config2)
    print(f"✓ Configuration diffing works: {len(diff)} differences")
    
    # Test human-readable diff
    human_diff = pynlm.ConfigValidator.diffConfigHuman(config1.toJSON(), config2.toJSON())
    print(f"✓ Human-readable diff: {len(human_diff)} lines")
    
    return True

def test_all_features():
    """Test all enhanced features"""
    print("=" * 60)
    print("Testing All Enhanced NLM Python Bindings Features")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Configuration", test_enhanced_config),
        ("Context Managers", test_context_managers),
        ("Simulation Utilities", test_simulation_utilities),
        ("Validation Utilities", test_validation_utilities),
        ("Configuration Diffing", test_configuration_diffing),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} ERROR: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! Enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the implementation.")
        return False

if __name__ == "__main__":
    print("Enhanced NLM Python Bindings - Quick Test")
    print("=" * 60)
    
    success = test_all_features()
    
    if success:
        print("\n✅ Quick test completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Quick test failed!")
        sys.exit(1)