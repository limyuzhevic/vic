#!/usr/bin/env python3
"""
Demonstration script for the improved NLM Python bindings.

This script shows the new convenience functions and improved API
that make it easier to work with the NLM (Neural Learning Machine)
neural simulation framework.
"""

import sys
import time

# Add the current directory to the Python path
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d5b6e053-2dc7-47dc-9896-62313df0ca80')

def test_basic_bindings():
    """Test basic bindings functionality."""
    print("Testing basic NLM bindings...")
    
    try:
        # Import the NLM module
        import pynlm as nlm
        
        print("✓ Successfully imported pynlm")
        
        # Test creating a default configuration
        config = nlm.create_default_config(
            neuron_count=50,
            region_count=2,
            connection_probability=0.1
        )
        print("✓ Created default configuration")
        
        # Test configuration properties
        print(f"  - Configuration has 'neuron_count': {config.has('neuron_count')}")
        print(f"  - Configuration has 'region_count': {config.has('region_count')}")
        
        # Test creating a brain with the configuration
        brain = nlm.createBrain(config)
        print("✓ Created brain with configuration")
        
        # Test brain initialization
        success = brain.initialize()
        print(f"✓ Brain initialization: {'Success' if success else 'Failed'}")
        
        # Test creating a world
        world = nlm.createSimpleWorld()
        print("✓ Created simple world")
        
        # Test creating an agent brain
        agent_brain = nlm.createAgentBrain(brain)
        print("✓ Created agent brain")
        
        # Test global constants
        print(f"✓ Invalid neuron ID: {nlm.INVALID_NEURON_ID}")
        print(f"✓ Invalid synapse ID: {nlm.INVALID_SYNAPSE_ID}")
        print(f"✓ Invalid region ID: {nlm.INVALID_REGION_ID}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in basic bindings test: {e}")
        return False

def test_convenience_functions():
    """Test the new convenience functions."""
    print("\nTesting convenience functions...")
    
    try:
        import pynlm as nlm
        
        # Test create_world_with_agent
        world = nlm.create_world_with_agent(
            width=100.0,
            height=100.0,
            vision_width=32.0,
            vision_height=32.0
        )
        print("✓ Created world with agent using convenience function")
        
        # Test collect_brain_metrics
        config = nlm.create_default_config(20, 1)
        brain = nlm.createBrain(config)
        brain.initialize()
        
        metrics = nlm.collect_brain_metrics(brain)
        print("✓ Collected brain metrics:")
        for key, value in metrics.items():
            print(f"  - {key}: {value}")
        
        # Test get_brain_summary
        summary = nlm.get_brain_summary(brain)
        print("✓ Brain summary:")
        for key, value in summary.items():
            print(f"  - {key}: {value}")
        
        # Test validate_config
        validation = nlm.validate_config(config)
        print(f"✓ Configuration validation: {'Passed' if not validation else 'Failed'}")
        
        # Test config_to_dict
        config_dict = nlm.config_to_dict(config)
        print(f"✓ Converted config to dictionary with {len(config_dict)} keys")
        
        # Test setup_python_logging
        nlm.setup_python_logging()
        print("✓ Configured Python logging")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in convenience functions test: {e}")
        return False

def test_context_manager():
    """Test the context manager functionality."""
    print("\nTesting context manager...")
    
    try:
        import pynlm as nlm
        
        # Create components
        config = nlm.create_default_config(10, 1)
        brain = nlm.createBrain(config)
        world = nlm.create_simple_world()
        
        # Use context manager
        with nlm.BrainSimulation(brain, world) as sim:
            retrieved_brain = sim.get_brain()
            retrieved_world = sim.get_world()
            
            print("✓ Context manager successfully retrieved brain and world")
            print(f"  - Brain initialized: {retrieved_brain.initialize()}")
            print(f"  - World configured: {retrieved_world.getWidth() == 100.0}")
        
        print("✓ Context manager exited cleanly")
        return True
        
    except Exception as e:
        print(f"✗ Error in context manager test: {e}")
        return False

def test_brain_simulation():
    """Test brain simulation with convenience functions."""
    print("\nTesting brain simulation...")
    
    try:
        import pynlm as nlm
        
        # Create world with agent
        world = nlm.create_world_with_agent(100.0, 100.0, 32.0, 32.0)
        
        # Create brain with agent
        config = nlm.create_default_config(30, 2)
        brain = nlm.createBrain(config)
        agent_brain = nlm.createAgentBrain(brain)
        
        print("✓ Created brain and agent brain")
        
        # Run a simple episode with callback
        step_count = 0
        def step_callback(step):
            nonlocal step_count
            step_count = step
            if step % 5 == 0:
                print(f"  Step {step}: Running simulation...")
        
        print("✓ Running simple episode (5 steps with callback)...")
        nlm.run_simple_episode(
            brain,
            world,
            max_steps=5,
            on_step=step_callback
        )
        
        print(f"✓ Episode completed with {step_count} steps")
        
        # Test visualization
        print("✓ Visualizing brain state...")
        nlm.visualizeBrain(brain)
        
        return True
        
    except Exception as e:
        print(f"✗ Error in brain simulation test: {e}")
        return False

def test_error_handling():
    """Test improved error handling."""
    print("\nTesting error handling...")
    
    try:
        import pynlm as nlm
        
        # Test configuration validation error
        try:
            config = nlm.create_default_config()
            # Remove required keys to test validation
            config.clear()
            nlm.validate_config(config)
            print("✗ Should have raised validation error")
            return False
        except nlm.ConfigError as e:
            print(f"✓ Configuration validation correctly raised error: {e}")
        
        # Test null brain/world error
        try:
            nlm.run_simple_episode(None, None)
            print("✗ Should have raised brain/world error")
            return False
        except nlm.BrainError as e:
            print(f"✓ Brain/world validation correctly raised error: {e}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling test: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings - Improved Functionality Demo")
    print("=" * 60)
    
    tests = [
        ("Basic Bindings", test_basic_bindings),
        ("Convenience Functions", test_convenience_functions),
        ("Context Manager", test_context_manager),
        ("Brain Simulation", test_brain_simulation),
        ("Error Handling", test_error_handling),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'-' * 40}")
        print(f"Running: {test_name}")
        print(f"{'-' * 40}")
        
        if test_func():
            passed += 1
            print(f"✓ {test_name} PASSED")
        else:
            failed += 1
            print(f"✗ {test_name} FAILED")
    
    print(f"\n{'=' * 60}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'=' * 60}")
    
    if failed == 0:
        print("🎉 All tests passed! The improved Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())