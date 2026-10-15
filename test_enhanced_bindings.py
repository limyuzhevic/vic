#!/usr/bin/env python3

# Test script for NLM Enhanced Python Bindings
# This script verifies all the enhanced features work correctly

import sys
import traceback

def test_version_info():
    """Test version and module information"""
    print("=== Testing Version Information ===")
    try:
        import pynlm
        
        # Check version attributes
        assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
        assert hasattr(pynlm, '__author__'), "Missing __author__ attribute"
        assert hasattr(pynlm, '__license__'), "Missing __license__ attribute"
        assert hasattr(pynlm, '__description__'), "Missing __description__ attribute"
        
        print(f"✓ Version: {pynlm.__version__}")
        print(f"✓ Author: {pynlm.__author__}")
        print(f"✓ License: {pynlm.__license__}")
        print(f"✓ Description: {pynlm.__description__}")
        
        # Check invalid constants
        assert hasattr(pynlm, 'INVALID_NEURON_ID'), "Missing INVALID_NEURON_ID"
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID'), "Missing INVALID_SYNAPSE_ID"
        assert hasattr(pynlm, 'INVALID_REGION_ID'), "Missing INVALID_REGION_ID"
        assert hasattr(pynlm, 'INVALID_POPULATION_ID'), "Missing INVALID_POPULATION_ID"
        
        print("✓ All invalid constants present")
        return True
        
    except Exception as e:
        print(f"✗ Version info test failed: {e}")
        traceback.print_exc()
        return False

def test_basic_functionality():
    """Test basic functionality"""
    print("\n=== Testing Basic Functionality ===")
    try:
        import pynlm
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Created default configuration")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Check brain properties
        assert brain.neuron_count > 0, "Neuron count should be positive"
        assert brain.synapse_count >= 0, "Synapse count should be non-negative"
        assert brain.active_neurons >= 0, "Active neurons should be non-negative"
        assert brain.firing_neurons >= 0, "Firing neurons should be non-negative"
        assert brain.average_firing_rate >= 0, "Average firing rate should be non-negative"
        
        print(f"✓ Brain properties: {brain.neuron_count} neurons, {brain.synapse_count} synapses")
        
        # Test enhanced simulation
        spike_count = brain.simulate(10, 0.1)
        print(f"✓ Enhanced simulation completed: {spike_count} spikes")
        
        # Test status retrieval
        status = brain.get_status()
        assert isinstance(status, str), "Status should be string"
        assert len(status) > 0, "Status should not be empty"
        print("✓ Status retrieval successful")
        
        # Test Pythonic properties
        assert hasattr(brain, 'neuron_count'), "Brain should have neuron_count property"
        assert hasattr(brain, 'synapse_count'), "Brain should have synapse_count property"
        assert hasattr(brain, 'active_neurons'), "Brain should have active_neurons property"
        assert hasattr(brain, 'firing_neurons'), "Brain should have firing_neurons property"
        assert hasattr(brain, 'average_firing_rate'), "Brain should have average_firing_rate property"
        assert hasattr(brain, 'development_stage'), "Brain should have development_stage property"
        
        print("✓ All Pythonic properties present")
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_agent():
    """Test enhanced agent functionality"""
    print("\n=== Testing Enhanced Agent ===")
    try:
        import pynlm
        
        # Setup components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable enhanced subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Enabled enhanced agent subsystems")
        
        # Check subsystem status
        assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
        assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
        assert agent.isDevelopmentEnabled(), "Development should be enabled"
        assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
        
        print("✓ All subsystems enabled correctly")
        
        # Test enhanced episode
        print("Running enhanced episode...")
        
        for step in range(10):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            # Print enhanced stats
            if step % 5 == 0:
                print(f"  Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}, "
                      f"Dev Stage={agent.getDevelopmentalStage()}")
        
        # Test enhanced agent statistics
        stats = agent.get_episode_stats()
        assert isinstance(stats, str), "Episode stats should be string"
        assert len(stats) > 0, "Episode stats should not be empty"
        
        print("✓ Enhanced episode completed successfully")
        print("✓ Enhanced episode statistics retrieved")
        
        # Check agent Pythonic properties
        assert hasattr(agent, 'brain'), "Agent should have brain property"
        
        print("✓ All agent tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Enhanced agent test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and exception translation"""
    print("\n=== Testing Error Handling ===")
    try:
        import pynlm
        
        # Test basic error handling
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test normal operations (should not raise exceptions)
        brain.step(0)
        spike_count = brain.simulate(5, 0.1)
        status = brain.get_status()
        
        print("✓ Normal operations completed without errors")
        
        # Test exception types (these should be available)
        assert hasattr(pynlm, 'NLMException'), "Should have NLMException"
        assert hasattr(pynlm, 'BrainInitializationException'), "Should have BrainInitializationException"
        assert hasattr(pynlm, 'SimulationStepException'), "Should have SimulationStepException"
        assert hasattr(pynlm, 'ConfigurationException'), "Should have ConfigurationException"
        
        print("✓ All custom exception types available")
        
        # Test that RuntimeError is still registered
        assert hasattr(pynlm, 'RuntimeError'), "Should have RuntimeError"
        
        print("✓ RuntimeError exception type available")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_configuration():
    """Test configuration management"""
    print("\n=== Testing Configuration ===")
    try:
        import pynlm
        
        # Create and modify configuration
        config = pynlm.createDefaultConfig()
        
        # Check initial state
        assert not config.has("test_key"), "Should not have test key initially"
        print("✓ Initial configuration check passed")
        
        # Test configuration methods
        config.set("int_value", 42, nlm.ConfigSource.Runtime)
        assert config.has("int_value"), "Should have int_value after setting"
        
        int_val = config.get<int>("int_value")
        assert int_val.has_value(), "Should get int value"
        assert *int_val == 42, "Int value should match"
        
        config.set("double_value", 3.14, nlm.ConfigSource.Runtime)
        double_val = config.get<double>("double_value")
        assert double_val.has_value(), "Should get double value"
        
        config.set("string_value", "hello", nlm.ConfigSource.Runtime)
        string_val = config.get<std::string>("string_value")
        assert string_val.has_value(), "Should get string value"
        assert *string_val == "hello", "String value should match"
        
        config.set("bool_value", true, nlm.ConfigSource.Runtime)
        bool_val = config.get<bool>("bool_value")
        assert bool_val.has_value(), "Should get bool value"
        assert *bool_val == true, "Bool value should match"
        
        # Test getOr method
        int_val2 = config.getOr<int>("missing", 100)
        assert int_val2 == 100, "Should return default for missing key"
        
        int_val3 = config.getOr<int>("int_value", 100)
        assert int_val3 == 42, "Should return existing value"
        
        # Test remove method
        config.set("remove_test", 123, nlm.ConfigSource.Runtime)
        assert config.has("remove_test"), "Should have remove_test before removal"
        
        config.remove("remove_test")
        assert not config.has("remove_test"), "Should not have remove_test after removal"
        
        # Test getKeys method
        keys = config.getKeys()
        assert keys.size() > 0, "Should have some keys"
        
        print("✓ All configuration tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        traceback.print_exc()
        return False

def test_world_and_objects():
    """Test world and object functionality"""
    print("\n=== Testing World and Objects ===")
    try:
        import pynlm
        
        # Create world
        world = pynlm.createSimpleWorld()
        print("✓ Created simple world")
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        print("✓ Configured world")
        
        # Reset world
        world.reset()
        print("✓ Reset world")
        
        # Set agent start
        world.setAgentStart(5.0, 5.0)
        print("✓ Set agent start position")
        
        # Check world properties
        assert world.getWidth() == 10, "Width should be 10"
        assert world.getHeight() == 10, "Height should be 10"
        assert world.getVisionWidth() == 4, "Vision width should be 4"
        assert world.getVisionHeight() == 4, "Vision height should be 4"
        
        print("✓ World properties correct")
        
        # Test world update
        world.update(0.1)
        print("✓ World update completed")
        
        # Get agent body
        body = world.getAgentBody()
        assert hasattr(body, 'x'), "Agent body should have x coordinate"
        assert hasattr(body, 'y'), "Agent body should have y coordinate"
        
        print("✓ Agent body retrieved")
        
        # Test world objects
        obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, value=5.0)
        assert obj.x == 1.0, "Object x should be 1.0"
        assert obj.y == 2.0, "Object y should be 2.0"
        assert obj.type == pynlm.WorldObjectType.Resource, "Object type should be Resource"
        assert obj.value == 5.0, "Object value should be 5.0"
        
        world.addObject(obj)
        print("✓ World object added")
        
        # Test valid position
        assert world.isValidPosition(3.0, 3.0), "Should be valid position"
        assert not world.isValidPosition(15.0, 15.0), "Should not be valid position"
        
        print("✓ Position validation working")
        
        return True
        
    except Exception as e:
        print(f"✗ World and objects test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("NLM Enhanced Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        test_version_info,
        test_basic_functionality,
        test_enhanced_agent,
        test_error_handling,
        test_configuration,
        test_world_and_objects
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test {test.__name__} crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
