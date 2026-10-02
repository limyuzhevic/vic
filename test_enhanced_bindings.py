#!/usr/bin/env python3
"""
Quick Test Script for Enhanced NLM Python Bindings

This script tests the new enhanced Python bindings to ensure they work correctly.
Run with: python test_enhanced_bindings.py
"""

import pynlm
import sys
import time

def test_basic_functionality():
    """Test basic enhanced functionality"""
    print("=== Testing Basic Enhanced Functionality ===")
    
    try:
        # Test version attributes
        print(f"✓ Version: {pynlm.__version__}")
        print(f"✓ Description: {pynlm.__description__}")
        print(f"✓ Author: {pynlm.__author__}")
        
        # Test configuration creation
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created with {len(config.getKeys())} keys")
        
        # Test Pythonic configuration
        config["test.int"] = 42
        config["test.float"] = 3.14
        config["test.bool"] = True
        config["test.string"] = "hello"
        
        print(f"✓ Pythonic configuration works")
        print(f"  - int: {config.get_int('test.int')}")
        print(f"  - float: {config.get_float('test.float')}")
        print(f"  - bool: {config.get_bool('test.bool')}")
        print(f"  - string: {config.get_string('test.string')}")
        
        # Test dictionary updates
        updates = {"key1": "value1", "key2": 123, "key3": True}
        config.update(updates)
        print(f"✓ Dictionary updates work")
        
        # Test save_dict
        config_dict = config.save_dict()
        print(f"✓ save_dict works: {len(config_dict)} items")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        return False

def test_brain_enhancements():
    """Test enhanced brain functionality"""
    print("\n=== Testing Brain Enhancements ===")
    
    try:
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Brain created and initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Test simulate() convenience method
        brain.simulate(steps=50, timestep=0.1, verbose=False)
        print(f"✓ simulate() convenience method works")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Test get_statistics()
        stats = brain.get_statistics()
        print(f"✓ get_statistics() works")
        print(f"  - Keys in stats: {len(stats)}")
        
        # Test __repr__ method
        repr_str = repr(brain)
        print(f"✓ __repr__ works: {repr_str}")
        
        # Test visualize_brain()
        result = brain.visualize_brain()
        print(f"✓ visualize_brain() works: {result}")
        
        # Test context manager
        with pynlm.BrainContext(pynlm.createBrain(config)) as test_brain:
            test_brain.initialize()
            test_brain.simulate(steps=25)
            print(f"✓ Context manager works")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain enhancements test failed: {e}")
        return False

def test_agent_enhancements():
    """Test enhanced agent functionality"""
    print("\n=== Testing Agent Enhancements ===")
    
    try:
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        agent.initialize(world)
        print(f"✓ Agent initialized")
        
        # Test setup_learning()
        agent.setup_learning(reward=True, curiosity=True, development=True)
        print(f"✓ setup_learning() works")
        
        # Test get_agent_stats()
        stats = agent.get_agent_stats()
        print(f"✓ get_agent_stats() works")
        print(f"  - Curiosity: {stats['curiosity_level']}")
        print(f"  - Novelty: {stats['novelty_level']}")
        print(f"  - Development stage: {stats['developmental_stage']}")
        
        # Test __repr__ method
        repr_str = repr(agent)
        print(f"✓ Agent __repr__ works: {repr_str}")
        
        # Test create_episode()
        world.reset()
        result = agent.create_episode(world, steps=20, verbose=False)
        print(f"✓ create_episode() works: {result} steps")
        
        return True
        
    except Exception as e:
        print(f"✗ Agent enhancements test failed: {e}")
        return False

def test_error_handling():
    """Test error handling"""
    print("\n=== Testing Error Handling ===")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test safe access with defaults
        value = config.get_string("nonexistent.key", "default")
        print(f"✓ Safe access with default works: {value}")
        
        # Test KeyError handling
        try:
            invalid_value = config["invalid.key"]
            print(f"✗ Should have raised KeyError")
            return False
        except KeyError:
            print(f"✓ KeyError raised as expected")
        
        # Test type checking
        config["test.int"] = 42
        if isinstance(config.get_int("test.int"), int):
            print(f"✓ Type checking works")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def test_performance_monitoring():
    """Test performance monitoring features"""
    print("\n=== Testing Performance Monitoring ===")
    
    try:
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test timing
        start_time = time.time()
        brain.simulate(steps=100, timestep=0.05)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = 100 / duration
        
        print(f"✓ Performance monitoring works")
        print(f"  - Simulation time: {duration:.2f} seconds")
        print(f"  - Speed: {steps_per_second:.0f} steps/second")
        
        # Test statistics
        stats = brain.get_statistics()
        efficiency = stats['firing_neurons'] / stats['total_neurons'] * 100
        
        print(f"  - Network efficiency: {efficiency:.1f}%")
        print(f"  - Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance monitoring test failed: {e}")
        return False

def test_experimental_setup():
    """Test experimental setup"""
    print("\n=== Testing Experimental Setup ===")
    
    try:
        # Create experimental configuration
        config = pynlm.createDefaultConfig()
        
        experimental_config = {
            "brain.neuron_count": 500,
            "plasticity.stdp.enable": True,
            "neuromod.curiosity.enable": True,
            "debug.enabled": True,
            "debug.log_level": "INFO"
        }
        
        config.update(experimental_config)
        print(f"✓ Experimental configuration created")
        
        # Create brain with experimental setup
        brain = pynlm.createBrain(config)
        
        # Use context manager for controlled experiment
        with pynlm.BrainContext(brain) as experimental_brain:
            experimental_brain.initialize()
            experimental_brain.simulate(steps=75, timestep=0.02, verbose=False)
            
            # Get experimental results
            stats = experimental_brain.get_statistics()
            print(f"✓ Experimental setup works")
            print(f"  - Neurons: {stats['total_neurons']}")
            print(f"  - Total spikes: {stats['total_spikes']}")
            print(f"  - Development stage: {stats['developmental_stage']}")
        
        return True
        
    except Exception as e:
        print(f"✗ Experimental setup test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("Enhanced NLM Python Bindings - Quick Test Suite")
    print("=" * 50)
    print()
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Brain Enhancements", test_brain_enhancements),
        ("Agent Enhancements", test_agent_enhancements),
        ("Error Handling", test_error_handling),
        ("Performance Monitoring", test_performance_monitoring),
        ("Experimental Setup", test_experimental_setup),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        if test_func():
            passed += 1
        else:
            print(f"  Test '{test_name}' failed")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("✓ All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print(f"✗ {total - passed} tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    exit(main())
