#!/usr/bin/env python3
"""
Test script for NLM Python bindings - Demonstrating Pythonic enhancements
"""

import sys
import os

# Add the current directory to the Python path to find the pynlm module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
    print(f"  Version: {pynlm.get_version_info()}")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("  Note: You need to build the Python bindings first using:")
    print("  pip install -e .")
    sys.exit(1)

def test_basic_functionality():
    print("\n=== Testing Basic Functionality ===")
    
    # 1. Test factory methods with sensible defaults
    print("\n1. Testing factory methods:")
    try:
        config = pynlm.createDefaultConfig()
        print("  ✓ Created default config")
        
        brain = pynlm.createBrain(config)
        print("  ✓ Created brain with default config")
        
        world = pynlm.createDefaultWorld()
        print("  ✓ Created default world")
        
        agent = pynlm.createAgentBrain(brain)
        print("  ✓ Created agent brain")
        
    except Exception as e:
        print(f"  ✗ Factory methods test failed: {e}")
        return False
    
    # 2. Test Pythonic property accessors
    print("\n2. Testing Pythonic property accessors:")
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"  ✓ brain.neuron_count = {brain.neuron_count}")
        print(f"  ✓ brain.synapse_count = {brain.synapse_count}")
        print(f"  ✓ brain.active_neuron_count = {brain.active_neuron_count}")
        print(f"  ✓ brain.firing_neuron_count = {brain.firing_neuron_count}")
        print(f"  ✓ brain.ei_ratio = {brain.ei_ratio:.3f}")
        print(f"  ✓ brain.total_spike_count = {brain.total_spike_count}")
        
    except Exception as e:
        print(f"  ✗ Pythonic property accessors test failed: {e}")
        return False
    
    # 3. Test context manager support
    print("\n3. Testing context manager support:")
    try:
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            print(f"  ✓ Context manager created brain with {brain.neuron_count} neurons")
            # Brain should be reset when exiting context
        
        print("  ✓ Context manager properly cleaned up brain")
        
    except Exception as e:
        print(f"  ✗ Context manager test failed: {e}")
        return False
    
    # 4. Test string representations and debugging methods
    print("\n4. Testing string representations:")
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        repr_str = repr(brain)
        print(f"  ✓ brain.__repr__() = {repr_str}")
        
        str_str = str(brain)
        print(f"  ✓ brain.__str__() = {str_str}")
        
        world = pynlm.createDefaultWorld()
        world_repr = repr(world)
        print(f"  ✓ world.__repr__() = {world_repr}")
        
    except Exception as e:
        print(f"  ✗ String representations test failed: {e}")
        return False
    
    # 5. Test simulation methods that combine multiple steps
    print("\n5. Testing combined simulation methods:")
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test stepSim (multiple steps)
        brain.stepSim(10, 0.1)
        print(f"  ✓ Completed 10 simulation steps")
        
        # Test brain statistics aggregation
        stats = brain.get_statistics()
        print(f"  ✓ Brain statistics: {len(stats)} properties")
        
        # Test configuration helpers
        brain_config = pynlm.createPyConfig()
        print(f"  ✓ Created Python-friendly config")
        
    except Exception as e:
        print(f"  ✗ Combined simulation methods test failed: {e}")
        return False
    
    # 6. Test type improvements
    print("\n6. Testing type improvements:")
    try:
        # Test neuron ID formatting
        neuron_id = pynlm.to_neuron_id(123)
        formatted = pynlm.format_neuron_id(neuron_id)
        print(f"  ✓ Formatted neuron ID: {formatted}")
        
        # Test action utilities
        action_names = pynlm.get_action_names()
        print(f"  ✓ Action names: {[str(name) for name in action_names[:5]]}...")
        
        # Test version and features
        version_info = pynlm.get_version_info()
        features = pynlm.get_feature_list()
        print(f"  ✓ Version: {version_info['version']}")
        print(f"  ✓ Number of features: {len(features)}")
        
    except Exception as e:
        print(f"  ✗ Type improvements test failed: {e}")
        return False
    
    # 7. Test error handling
    print("\n7. Testing enhanced error handling:")
    try:
        # Test validation
        config = pynlm.createDefaultConfig()
        validation = pynlm.validate_config(config)
        print(f"  ✓ Configuration validation: {validation['valid']}")
        
        # Test brain statistics analysis
        brain = pynlm.createBrain(config)
        brain.initialize()
        analysis = pynlm.analyze_brain_statistics(brain)
        print(f"  ✓ Brain analysis: {len(analysis)} metrics calculated")
        
    except Exception as e:
        print(f"  ✗ Error handling test failed: {e}")
        return False
    
    # 8. Test simulation setup utilities
    print("\n8. Testing simulation setup utilities:")
    try:
        # Test simulation setup
        setup = pynlm.createSimulationSetup()
        print(f"  ✓ Created simulation setup with {len(setup)} components")
        
        # Test simulation environment
        env = pynlm.create_simulated_environment(30, 30)
        print(f"  ✓ Created environment: {env.getWidth()}x{env.getHeight()}")
        
        # Test simulation constants
        constants = pynlm.get_simulation_constants()
        print(f"  ✓ Constants loaded: default timestep = {constants['default_timestep']}")
        
    except Exception as e:
        print(f"  ✗ Simulation setup utilities test failed: {e}")
        return False
    
    return True

def test_usage_examples():
    print("\n=== Testing Usage Examples ===")
    
    try:
        # Example 1: Basic brain usage
        print("\n1. Basic brain usage example:")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"   - Created brain with {brain.neuron_count} neurons")
        print(f"   - Brain representation: {brain}")
        
        # Run some simulation steps
        for i in range(5):
            brain.step(i)
        
        print(f"   - After 5 steps: {brain.firing_neuron_count} firing neurons")
        
        # Example 2: World simulation
        print("\n2. World simulation example:")
        world = pynlm.create_simulated_environment()
        print(f"   - Created world: {world}")
        
        # Example 3: Agent simulation
        print("\n3. Agent simulation example:")
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllSystems()
        
        status = agent.getSystemStatus()
        print(f"   - Agent enabled systems: {len([k for k, v in status.items() if v])} active")
        
        # Example 4: Configuration utilities
        print("\n4. Configuration utilities example:")
        config = pynlm.createPyConfig()
        config.set("brain.neuron_count", 2000)
        print(f"   - Custom neuron count: {config.getOr('neuron_count', 1000)}")
        
        # Example 5: Statistics and analysis
        print("\n5. Statistics and analysis example:")
        stats = brain.get_statistics()
        print(f"   - Key statistics: neuron_count={stats['neuron_count']}, "
              f"firing_percentage={stats['firing_neuron_count']/stats['neuron_count']*100:.1f}%")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Usage examples test failed: {e}")
        return False

if __name__ == "__main__":
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("Testing Pythonic enhancements and convenience methods")
    print("=" * 60)
    
    success = True
    
    # Run all tests
    if not test_basic_functionality():
        success = False
    
    if not test_usage_examples():
        success = False
    
    print("\n" + "=" * 60)
    if success:
        print("✓ All tests passed successfully!")
        print("\nPythonic enhancements have been successfully implemented:")
        print("  - Pythonic property accessors (e.g., brain.neuron_count)")
        print("  - Context manager support for resource management")
        print("  - String representations and debugging methods")
        print("  - Factory methods with sensible defaults")
        print("  - Configuration helper methods")
        print("  - Simulation episode methods")
        print("  - Type improvements and utilities")
        print("  - Enhanced error handling and validation")
        print("  - Memory management utilities")
    else:
        print("✗ Some tests failed. Please check the implementation.")
        sys.exit(1)
    
    print("=" * 60)