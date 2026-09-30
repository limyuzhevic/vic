#!/usr/bin/env python3
"""
Quick test script for the enhanced NLM Python interface.
This script runs basic functionality tests to verify the improvements.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_functionality():
    """Test basic NLM Python interface functionality."""
    print("Testing NLM Python Interface...")
    print("=" * 40)
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    
    try:
        # Test configuration creation
        config = pynlm.create_default_config()
        print(f"✓ Created default config: {config}")
        
        # Test configuration values
        assert config.get("neuron_count") > 0
        print("✓ Configuration validation passed")
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        return False
    
    try:
        # Test brain creation
        brain = pynlm.create_brain({"neuron_count": 50})
        print(f"✓ Created brain with {brain.get_stats()['total_neurons']} neurons")
        
        # Test brain initialization
        result = brain.initialize()
        if result:
            print("✓ Brain initialization succeeded")
        else:
            print("✗ Brain initialization failed")
            return False
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        return False
    
    try:
        # Test world creation
        world = pynlm.create_simple_world(width=10, height=10)
        print(f"✓ Created world {world.getWidth()}x{world.getHeight()}")
    except Exception as e:
        print(f"✗ World creation test failed: {e}")
        return False
    
    try:
        # Test agent creation
        brain = pynlm.create_brain()
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent(brain, world)
        print(f"✓ Created agent with {agent.get_sensory_input_size()} sensory inputs")
    except Exception as e:
        print(f"✗ Agent creation test failed: {e}")
        return False
    
    try:
        # Test configuration templates
        templates = pynlm.get_available_templates()
        print(f"✓ Available templates: {templates}")
        
        # Test loading template
        config = pynlm.load_config_template('simple')
        assert config['neuron_count'] > 0
        print("✓ Configuration template loading works")
    except Exception as e:
        print(f"✗ Configuration template test failed: {e}")
        return False
    
    try:
        # Test feature enablement
        brain = pynlm.create_brain()
        world = pynlm.create_simple_world()
        agent = pynlm.create_agent(brain, world)
        
        agent.enable_feature('reward_modulation', True)
        agent.enable_feature('curiosity', True)
        
        assert agent.is_feature_enabled('reward_modulation')
        assert agent.is_feature_enabled('curiosity')
        print("✓ Feature enablement works")
    except Exception as e:
        print(f"✗ Feature test failed: {e}")
        return False
    
    try:
        # Test statistics
        brain = pynlm.create_brain()
        brain.initialize()
        stats = brain.get_stats()
        
        required_keys = ['total_neurons', 'total_synapses', 'total_spikes']
        for key in required_keys:
            assert key in stats
        print("✓ Statistics collection works")
    except Exception as e:
        print(f"✗ Statistics test failed: {e}")
        return False
    
    try:
        # Test step execution
        brain = pynlm.create_brain({"neuron_count": 20})
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        stats = brain.get_stats()
        assert stats['total_spikes'] > 0
        print(f"✓ Step execution works (generated {stats['total_spikes']} spikes)")
    except Exception as e:
        print(f"✗ Step execution test failed: {e}")
        return False
    
    try:
        # Test world interaction
        brain = pynlm.create_brain({"neuron_count": 30})
        world = pynlm.create_simple_world(width=8, height=8)
        agent = pynlm.create_agent(brain, world)
        
        for step in range(20):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory(percept)
            brain.step(step)
            action = agent.decode_action()
            world.apply_action(action, world.get_simulation_time())
        
        print("✓ World interaction simulation works")
    except Exception as e:
        print(f"✗ World interaction test failed: {e}")
        return False
    
    return True


def test_examples():
    """Test that example modules can be imported and run."""
    print("\nTesting Example Modules...")
    print("=" * 40)
    
    try:
        import pynlm.brain_examples
        print("✓ Successfully imported brain_examples")
    except ImportError as e:
        print(f"✗ Failed to import brain_examples: {e}")
        return False
    
    try:
        import pynlm.agent_examples
        print("✓ Successfully imported agent_examples")
    except ImportError as e:
        print(f"✗ Failed to import agent_examples: {e}")
        return False
    
    try:
        import pynlm.learning_examples
        print("✓ Successfully imported learning_examples")
    except ImportError as e:
        print(f"✗ Failed to import learning_examples: {e}")
        return False
    
    return True


def test_documentation():
    """Test that documentation is accessible."""
    print("\nTesting Documentation...")
    print("=" * 40)
    
    try:
        import pynlm
        print(f"✓ Version: {pynlm.__version__}")
        print(f"✓ Author: {pynlm.__author__}")
        print(f"✓ Description: {pynlm.__description__}")
    except Exception as e:
        print(f"✗ Documentation test failed: {e}")
        return False
    
    return True


def main():
    """Run all tests."""
    print("Enhanced NLM Python Interface - Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Example Modules", test_examples),
        ("Documentation", test_documentation)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{test_name}:")
        result = test_func()
        results.append((test_name, result))
    
    # Summary
    print("\n" + "=" * 50)
    print("Test Summary:")
    print("=" * 50)
    
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for test_name, result in results:
        status = "PASS" if result else "FAIL"
        print(f"  {test_name}: {status}")
    
    print(f"\nOverall: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! The enhanced NLM Python interface is working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1


if __name__ == "__main__":
    sys.exit(main())
