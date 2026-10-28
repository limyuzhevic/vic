#!/usr/bin/env python3
"""
Test script for the improved NLM Python bindings.
This script demonstrates the new features and improvements.
"""

import sys
import traceback

try:
    import pynlm
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    print("Please install the pynlm module first.")
    sys.exit(1)

def test_basic_functionality():
    """Test basic functionality of the improved bindings."""
    print("=" * 60)
    print("Testing Basic Functionality")
    print("=" * 60)
    
    try:
        # Test module attributes
        print(f"Module version: {pynlm.__version__}")
        print(f"Module author: {pynlm.__author__}")
        print(f"Module description: {pynlm.__description__}")
        
        # Test creating a default config
        config = pynlm.create_default_config()
        print(f"Config created: {config}")
        
        # Test creating a brain
        brain = pynlm.create_brain(config)
        print(f"Brain created with {brain.get_total_neuron_count()} neurons")
        
        # Test initialization
        brain.initialize()
        print("Brain initialized successfully")
        
        # Test step
        for i in range(5):
            brain.step(i)
        
        print(f"Brain stepped 5 times, total spikes: {brain.get_total_spike_count()}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in basic functionality test: {e}")
        traceback.print_exc()
        return False

def test_enhanced_config():
    """Test enhanced configuration features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced Configuration")
    print("=" * 60)
    
    try:
        config = pynlm.create_default_config()
        
        # Test setting values
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15f)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", False)
        
        # Test getting values
        neuron_count_opt = config.get("brain.neuron_count")
        neuron_count = neuron_count_opt.value() if neuron_count_opt else None
        print(f"Neuron count: {neuron_count}")
        
        synapse_density_opt = config.get("brain.synapse_density")
        synapse_density = synapse_density_opt.value() if synapse_density_opt else None
        print(f"Synapse density: {synapse_density}")
        
        stdp_enabled_opt = config.get("plasticity.stdp.enable")
        stdp_enabled = stdp_enabled_opt.value() if stdp_enabled_opt else None
        print(f"STDP enabled: {stdp_enabled}")
        
        hebbian_enabled_opt = config.get("plasticity.hebbian.enable")
        hebbian_enabled = hebbian_enabled_opt.value() if hebbian_enabled_opt else None
        print(f"Hebbian enabled: {hebbian_enabled}")
        
        # Test validation
        print("\nTesting validation:")
        try:
            config.set("invalid.key", "test_value")
            print("Setting invalid key succeeded")
        except Exception as e:
            print(f"Expected error for invalid key: {e}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in enhanced config test: {e}")
        traceback.print_exc()
        return False

def test_enhanced_sensory_input():
    """Test enhanced sensory input features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced Sensory Input")
    print("=" * 60)
    
    try:
        # Create vision input
        vision = pynlm.Vision(10, 10, 3)
        
        # Create test data
        test_data = [i * 0.1 for i in range(300)]  # 10*10*3 = 300 values
        
        # Set data directly
        vision.set_data(test_data)
        print(f"Vision data set: {len(vision.get_data())} values")
        
        # Test vision properties
        print(f"Vision dimensions: {vision.get_dimensions()}")
        print(f"Vision width: {vision.get_width()}")
        print(f"Vision height: {vision.get_height()}")
        print(f"Vision channels: {vision.get_channels()}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in enhanced sensory input test: {e}")
        traceback.print_exc()
        return False

def test_action_enhancements():
    """Test enhanced action features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced Action Features")
    print("=" * 60)
    
    try:
        # Create actions
        move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
        print(f"Created move_forward action: {move_forward.get_name()}")
        
        # Set parameters
        move_forward.set_parameters([1.0, 0.5, 0.0])
        print(f"Action parameters: {move_forward.get_parameters()}")
        
        # Convert to dictionary and back
        action_dict = move_forward.to_dict()
        print(f"Action as dictionary: {action_dict}")
        
        # Create new action from dictionary
        action2 = pynlm.Action()
        action2.from_dict(action_dict)
        print(f"Action from dictionary: {action2.get_name()} with type {action2.get_type()}")
        
        # Test cloning
        action_clone = move_forward.clone()
        print(f"Action cloned successfully: {action_clone.get_name()}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in action enhancements test: {e}")
        traceback.print_exc()
        return False

def test_world_enhancements():
    """Test enhanced world features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced World Features")
    print("=" * 60)
    
    try:
        # Create world
        world = pynlm.create_simple_world()
        world.configure(width=20, height=20, vision_width=8, vision_height=8)
        print(f"World configured: {world.get_width()}x{world.get_height()}")
        
        # Create agent body
        agent_body = world.create_agent(10.0, 10.0)
        print(f"Agent created at position: ({agent_body.x}, {agent_body.y})")
        print(f"Agent speed: {agent_body.speed():.3f}")
        
        # Test position operations
        print(f"Agent position: {agent_body.get_position()}")
        
        # Test world update
        world.reset()
        world.update(0.1)
        print(f"World updated, simulation time: {world.get_simulation_time():.3f}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in world enhancements test: {e}")
        traceback.print_exc()
        return False

def test_brain_enhancements():
    """Test enhanced brain features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced Brain Features")
    print("=" * 60)
    
    try:
        # Create brain with default config
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test statistics
        stats = brain.get_statistics()
        print("Brain statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
        
        # Test config operations
        brain.set_config_value("brain.neuron_count", 3000)
        
        print(f"Updated neuron count: {brain.get_total_neuron_count()}")
        
        # Test step with sensory input
        vision = pynlm.Vision(8, 8, 3)
        vision.set_data([i * 0.1 for i in range(192)])  # 8*8*3 = 192
        
        brain.step(0)
        action = brain.produce_action()
        print(f"Produced action type: {action.get_type()}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in brain enhancements test: {e}")
        traceback.print_exc()
        return False

def test_agent_enhancements():
    """Test enhanced agent features."""
    print("\n" + "=" * 60)
    print("Testing Enhanced Agent Features")
    print("=" * 60)
    
    try:
        # Create brain and world
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(width=30, height=30, vision_width=8, vision_height=8)
        world.reset()
        
        # Create agent
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Test behavior statistics
        behavior_stats = agent.get_behavior_stats()
        print("Agent behavior statistics:")
        for key, value in behavior_stats.items():
            print(f"  {key}: {value}")
        
        # Test simple episode
        steps = 20
        curiosity_level = agent.create_simple_episode(world, steps)
        print(f"Created {steps}-step episode, final curiosity level: {curiosity_level:.3f}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in agent enhancements test: {e}")
        traceback.print_exc()
        return False

def test_context_manager():
    """Test context manager feature."""
    print("\n" + "=" * 60)
    print("Testing Context Manager")
    print("=" * 60)
    
    try:
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Test context manager
        with pynlm.create_brain_context(brain) as brain_ctx:
            print(f"Brain context initialized: {brain_ctx.is_initialized()}")
            
            # Perform operations
            for i in range(10):
                brain_ctx.get_brain().step(i)
            
            print(f"Total spikes after context operations: {brain_ctx.get_brain().get_total_spike_count()}")
        
        # Brain should be reset after context exit
        print(f"Brain after context: {brain.get_total_spike_count()} spikes (should be 0)")
        
        return True
        
    except Exception as e:
        print(f"ERROR in context manager test: {e}")
        traceback.print_exc()
        return False

def test_constants_and_helpers():
    """Test constants and helper functions."""
    print("\n" + "=" * 60)
    print("Testing Constants and Helper Functions")
    print("=" * 60)
    
    try:
        # Test constants
        constants = pynlm.get_constants()
        print("Constants:")
        for key, value in constants.items():
            print(f"  {key}: {value}")
        
        # Test helper functions
        pynlm.validate_positive(10, "Test value")
        print("Validation passed for positive value")
        
        pynlm.validate_range(5.0, 0.0, 10.0, "Test range")
        print("Validation passed for range")
        
        pynlm.validate_non_empty("test", "Test string")
        print("Validation passed for non-empty string")
        
        # Test error handling
        try:
            pynlm.validate_positive(-5, "Test negative")
            print("ERROR: Should have thrown validation error")
            return False
        except pynlm.ValidationError:
            print("Correctly caught validation error for negative value")
        
        try:
            pynlm.validate_range(15.0, 0.0, 10.0, "Test out of range")
            print("ERROR: Should have thrown validation error")
            return False
        except pynlm.ValidationError:
            print("Correctly caught validation error for out-of-range value")
        
        try:
            pynlm.validate_non_empty("", "Test empty")
            print("ERROR: Should have thrown validation error")
            return False
        except pynlm.ValidationError:
            print("Correctly caught validation error for empty string")
        
        return True
        
    except Exception as e:
        print(f"ERROR in constants and helpers test: {e}")
        traceback.print_exc()
        return False

def test_module_functionality():
    """Test module-level convenience functions."""
    print("\n" + "=" * 60)
    print("Testing Module-Level Convenience Functions")
    print("=" * 60)
    
    try:
        # Test create_brain_with_config
        brain = pynlm.create_brain_with_defaults(500, 0.05)
        brain.initialize()
        print(f"Brain with defaults: {brain.get_total_neuron_count()} neurons")
        
        # Test run_simulation
        spike_count = pynlm.run_simulation(brain, 100)
        print(f"Simulation completed: {spike_count} spikes")
        
        # Test create_simple_agent_episode
        config = pynlm.create_default_config()
        brain2 = pynlm.create_brain(config)
        world = pynlm.create_simple_world()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain2)
        agent.initialize(world)
        
        curiosity_level = pynlm.create_simple_agent_episode(agent, world, 50)
        print(f"Simple agent episode completed: curiosity level = {curiosity_level:.3f}")
        
        return True
        
    except Exception as e:
        print(f"ERROR in module functionality test: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("Testing Improved NLM Python Bindings")
    print("This script demonstrates the new features and improvements.")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Configuration", test_enhanced_config),
        ("Enhanced Sensory Input", test_enhanced_sensory_input),
        ("Action Enhancements", test_action_enhancements),
        ("World Enhancements", test_world_enhancements),
        ("Brain Enhancements", test_brain_enhancements),
        ("Agent Enhancements", test_agent_enhancements),
        ("Context Manager", test_context_manager),
        ("Constants and Helpers", test_constants_and_helpers),
        ("Module Functionality", test_module_functionality),
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
            print(f"✗ {test_name} FAILED with exception: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success rate: {passed/len(tests)*100:.1f}%")
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved bindings are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())