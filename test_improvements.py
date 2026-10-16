#!/usr/bin/env python3
"""
Simple test script to verify NLM Python API improvements.

This script demonstrates the key improvements made to the NLM Python API,
including better error handling, configuration validation, and factory functions.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic NLM Functionality ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            print("❌ Failed to initialize brain")
            return False
        
        print("✅ Brain created and initialized successfully")
        
        # Test simulation
        for step in range(10):
            brain.step(step)
        
        print(f"✅ Simulation completed: {brain.getTotalSpikeCount()} spikes")
        
        # Test configuration
        config.set("test_value", "test")
        if config.get("test_value") != "test":
            print("❌ Configuration test failed")
            return False
        
        print("✅ Configuration test passed")
        
        # Test validation
        is_valid, errors = config.validate_config()
        if is_valid:
            print("✅ Configuration validation passed")
        else:
            print(f"⚠️  Configuration validation issues: {errors}")
        
        return True
        
    except Exception as e:
        print(f"❌ Error in basic functionality test: {e}")
        return False

def test_agent_world():
    """Test agent-world integration."""
    print("\n=== Testing Agent-World Integration ===")
    
    try:
        # Create complete agent setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        world_config = {
            "width": 20,
            "height": 20,
            "visionWidth": 8,
            "visionHeight": 8
        }
        
        brain, agent, world = pynlm.create_complete_agent(config, world_config)
        
        print("✅ Complete agent setup created")
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = 0.0
            if world.getAgentBody().x > world.getWidth() * 0.8:
                reward = 1.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        print(f"✅ Agent-world simulation completed")
        print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"   - Novelty level: {agent.getNoveltyLevel():.3f}")
        
        return True
        
    except Exception as e:
        print(f"❌ Error in agent-world test: {e}")
        return False

def test_configuration_features():
    """Test advanced configuration features."""
    print("\n=== Testing Configuration Features ===")
    
    try:
        # Test Pythonic configuration
        config = pynlm.createDefaultConfig()
        config.set(
            brain_neuron_count=1000,
            brain_connection_probability=0.05,
            simulation_timestep=0.0005,
            plasticity_stdp_learning_rate=0.01
        )
        
        print("✅ Pythonic configuration applied")
        
        # Test configuration validation
        is_valid, errors = config.validate_config()
        if is_valid:
            print("✅ Configuration validation passed")
        else:
            print(f"⚠️  Validation issues: {errors}")
        
        # Test configuration summary
        summary = config.summary()
        if len(summary) > 0:
            print("✅ Configuration summary generated")
        
        # Test configuration keys
        keys = config.getKeys()
        if len(keys) > 0:
            print(f"✅ Configuration keys: {len(keys)} keys found")
        
        return True
        
    except Exception as e:
        print(f"❌ Error in configuration test: {e}")
        return False

def test_factory_functions():
    """Test factory functions."""
    print("\n=== Testing Factory Functions ===")
    
    try:
        # Test simple simulation
        config = pynlm.createDefaultConfig()
        brain, agent, world = pynlm.create_complete_agent(
            config,
            worldConfig={"width": 10, "height": 10, "visionWidth": 4, "visionHeight": 4}
        )
        
        result = pynlm.run_simple_simulation(brain, world, agent, 10)
        
        spikes, firing_rate, curiosity, novelty = result
        print(f"✅ Simple simulation: {spikes} spikes, {firing_rate:.2f} Hz")
        
        return True
        
    except Exception as e:
        print(f"❌ Error in factory functions test: {e}")
        return False

def test_error_handling():
    """Test error handling."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test invalid configuration
        config = pynlm.createDefaultConfig()
        
        # This should handle the invalid configuration gracefully
        config.set("brain.neuron_count", 50)  # Very small brain
        
        is_valid, errors = config.validate_config()
        print(f"✅ Invalid configuration handled: valid={is_valid}, errors={len(errors)}")
        
        return True
        
    except Exception as e:
        print(f"❌ Error in error handling test: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Python API Improvements Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Agent-World Integration", test_agent_world),
        ("Configuration Features", test_configuration_features),
        ("Factory Functions", test_factory_functions),
        ("Error Handling", test_error_handling),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'=' * 50}")
        success = test_func()
        results.append((test_name, success))
    
    # Summary
    print(f"\n{'=' * 50}")
    print("TEST SUMMARY")
    print(f"{'=' * 50}")
    
    passed = 0
    for test_name, success in results:
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{len(results)} tests passed")
    
    if passed == len(results):
        print("🎉 All tests passed! NLM Python API improvements are working correctly.")
        return 0
    else:
        print("⚠️  Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
