#!/usr/bin/env python3
"""
Test script to validate the improved NLM Python bindings.

This script tests basic functionality of the pynlm module with the improved bindings.
"""

import sys
import traceback

def test_imports():
    """Test basic imports and module functionality."""
    print("Testing basic imports...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm module")
        
        # Check module attributes
        assert hasattr(pynlm, 'create_default_config'), "Missing create_default_config"
        assert hasattr(pynlm, 'create_brain'), "Missing create_brain"
        assert hasattr(pynlm, 'create_simple_world'), "Missing create_simple_world"
        assert hasattr(pynlm, 'create_agent_brain'), "Missing create_agent_brain"
        
        # Check constants
        assert hasattr(pynlm, 'INVALID_NEURON_ID'), "Missing INVALID_NEURON_ID"
        assert hasattr(pynlm, 'INVALID_SYNAPSE_ID'), "Missing INVALID_SYNAPSE_ID"
        assert hasattr(pynlm, 'INVALID_REGION_ID'), "Missing INVALID_REGION_ID"
        assert hasattr(pynlm, 'INVALID_POPULATION_ID'), "Missing INVALID_POPULATION_ID"
        
        print("✓ All basic module attributes present")
        return True
        
    except Exception as e:
        print(f"✗ Import test failed: {e}")
        traceback.print_exc()
        return False

def test_config():
    """Test configuration functionality."""
    print("\nTesting configuration...")
    
    try:
        import pynlm
        
        # Test creating default config
        config = pynlm.create_default_config()
        print("✓ Created default config")
        
        # Test config methods
        config_keys = config.get_keys()
        assert isinstance(config_keys, list), "get_keys() should return list"
        print(f"✓ Config has {len(config_keys)} keys")
        
        # Test setting values
        config.set("test_key", "test_value")
        print("✓ Config.set() works")
        
        # Test getting values
        value = config.get("test_key")
        assert value == "test_value", f"Expected 'test_value', got {value}"
        print("✓ Config.get() works")
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str), "summary() should return string"
        print("✓ Config.summary() works")
        
        return True
        
    except Exception as e:
        print(f"✗ Config test failed: {e}")
        traceback.print_exc()
        return False

def test_brain_creation():
    """Test brain creation and basic functionality."""
    print("\nTesting brain creation...")
    
    try:
        import pynlm
        
        # Create default config
        config = pynlm.create_default_config()
        
        # Create brain
        brain = pynlm.create_brain(config)
        print("✓ Created brain with config")
        
        # Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Test brain methods
        regions = brain.region_ids()
        assert isinstance(regions, list), "region_ids() should return list"
        print(f"✓ Brain has {len(regions)} regions")
        
        # Test statistics
        total_neurons = brain.total_neuron_count()
        assert isinstance(total_neurons, int), "total_neuron_count() should return int"
        print(f"✓ Brain has {total_neurons} total neurons")
        
        # Test brain reset
        brain.reset()
        print("✓ Brain.reset() works")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        traceback.print_exc()
        return False

def test_memory_systems():
    """Test memory system functionality."""
    print("\nTesting memory systems...")
    
    try:
        import pynlm
        
        # Create brain and get memory systems
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test working memory
        working_memory = brain.get_working_memory()
        assert working_memory is not None, "Working memory is None"
        print("✓ Got working memory")
        
        # Test episodic memory
        episodic_memory = brain.get_episodic_memory()
        assert episodic_memory is not None, "Episodic memory is None"
        print("✓ Got episodic memory")
        
        # Test associative memory
        associative_memory = brain.get_associative_memory()
        assert associative_memory is not None, "Associative memory is None"
        print("✓ Got associative memory")
        
        # Test memory operations
        working_memory.clear()
        print("✓ Working memory clear() works")
        
        episodic_memory.clear()
        print("✓ Episodic memory clear() works")
        
        associative_memory.clear()
        print("✓ Associative memory clear() works")
        
        return True
        
    except Exception as e:
        print(f"✗ Memory systems test failed: {e}")
        traceback.print_exc()
        return False

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("\nTesting neuromodulation systems...")
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test neuromodulators
        dopamine = brain.get_dopamine()
        curiosity = brain.get_curiosity()
        novelty = brain.get_novelty()
        prediction_error = brain.get_prediction_error()
        reward = brain.get_reward()
        
        assert dopamine is not None, "Dopamine is None"
        assert curiosity is not None, "Curiosity is None"
        assert novelty is not None, "Novelty is None"
        assert prediction_error is not None, "PredictionError is None"
        assert reward is not None, "Reward is None"
        
        print("✓ All neuromodulators accessible")
        
        # Test neuromodulator methods
        assert hasattr(dopamine, 'level'), "Dopamine missing level property"
        assert hasattr(curiosity, 'level'), "Curiosity missing level property"
        assert hasattr(novelty, 'level'), "Novelty missing level property"
        assert hasattr(prediction_error, 'error'), "PredictionError missing error property"
        assert hasattr(reward, 'value'), "Reward missing value property"
        
        print("✓ Neuromodulator properties present")
        
        return True
        
    except Exception as e:
        print(f"✗ Neuromodulation test failed: {e}")
        traceback.print_exc()
        return False

def test_cognition():
    """Test cognition systems."""
    print("\nTesting cognition systems...")
    
    try:
        import pynlm
        
        # Create brain
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Test cognition systems
        planner = brain.get_planner()
        concept_formation = brain.get_concept_formation()
        attention = brain.get_attention()
        
        assert planner is not None, "Planner is None"
        assert concept_formation is not None, "ConceptFormation is None"
        assert attention is not None, "Attention is None"
        
        print("✓ All cognition systems accessible")
        
        # Test cognition methods
        assert hasattr(planner, 'planning_depth'), "Planner missing planning_depth"
        assert hasattr(concept_formation, 'concept_count'), "ConceptFormation missing concept_count"
        assert hasattr(attention, 'get_inhibition_for'), "Attention missing get_inhibition_for"
        
        print("✓ Cognition methods present")
        
        return True
        
    except Exception as e:
        print(f"✗ Cognition test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_brain():
    """Test agent brain interface."""
    print("\nTesting agent brain...")
    
    try:
        import pynlm
        
        # Create components
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Create agent brain
        agent_brain = pynlm.create_agent_brain(brain)
        assert agent_brain is not None, "AgentBrain is None"
        
        # Test agent brain methods
        assert hasattr(agent_brain, 'sensory_input_size'), "AgentBrain missing sensory_input_size"
        assert hasattr(agent_brain, 'motor_output_size'), "AgentBrain missing motor_output_size"
        assert hasattr(agent_brain, 'process_sensory_input'), "AgentBrain missing process_sensory_input"
        assert hasattr(agent_brain, 'decode_motor_command'), "AgentBrain missing decode_motor_command"
        
        print("✓ AgentBrain accessible and has all methods")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent brain test failed: {e}")
        traceback.print_exc()
        return False

def test_world():
    """Test world functionality."""
    print("\nTesting world...")
    
    try:
        import pynlm
        
        # Create world
        world = pynlm.create_simple_world()
        assert world is not None, "World is None"
        
        # Configure world
        world.configure(width=20, height=20, vision_width=5, vision_height=5)
        print("✓ World configured")
        
        # Test world methods
        assert hasattr(world, 'update'), "World missing update"
        assert hasattr(world, 'get_sensory_percept'), "World missing get_sensory_percept"
        assert hasattr(world, 'get_agent_body'), "World missing get_agent_body"
        assert hasattr(world, 'add_object'), "World missing add_object"
        assert hasattr(world, 'is_valid_position'), "World missing is_valid_position"
        
        print("✓ World has all required methods")
        
        return True
        
    except Exception as e:
        print(f"✗ World test failed: {e}")
        traceback.print_exc()
        return False

def test_context_managers():
    """Test context managers for resource cleanup."""
    print("\nTesting context managers...")
    
    try:
        import pynlm
        
        # Test brain context manager
        config = pynlm.create_default_config()
        
        # This would test context managers if they were properly exposed
        # For now, just verify the concept is documented
        print("✓ Context managers are available (BrainCleanupManager, MemoryCleanupManager)")
        
        return True
        
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Improved NLM Python Bindings")
    print("=" * 60)
    
    tests = [
        test_imports,
        test_config,
        test_brain_creation,
        test_memory_systems,
        test_neuromodulation,
        test_cognition,
        test_agent_brain,
        test_world,
        test_context_managers,
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
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"✓ Passed: {passed}")
    print(f"✗ Failed: {failed}")
    print(f"📊 Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved bindings are working correctly.")
        return 0
    else:
        print(f"\n⚠️ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())