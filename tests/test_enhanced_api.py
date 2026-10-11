"""Test script for enhanced NLM Python bindings."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import pynlm.utils

def test_basic_functionality():
    """Test basic functionality of enhanced bindings."""
    print("Testing basic functionality...")
    
    # Create configuration
    config = pynlm.Config()
    config.set("simulation_max_steps", 100)
    
    # Create brain
    brain = pynlm.Brain(config)
    
    # Initialize brain
    assert brain.initialize(), "Brain initialization should succeed"
    
    # Test Pythonic properties
    assert hasattr(brain, 'spike_count'), "Brain should have spike_count property"
    assert hasattr(brain, 'synapse_count'), "Brain should have synapse_count property"
    assert hasattr(brain, 'active_neurons'), "Brain should have active_neurons property"
    assert hasattr(brain, 'firing_neurons'), "Brain should have firing_neurons property"
    assert hasattr(brain, 'ei_ratio'), "Brain should have ei_ratio property"
    assert hasattr(brain, 'region_names'), "Brain should have region_names property"
    
    print("  ✓ Basic functionality tests passed")

def test_error_handling():
    """Test error handling features."""
    print("Testing error handling...")
    
    # Create brain
    brain = pynlm.Brain()
    
    # Test error classes exist
    assert hasattr(pynlm, 'PynlmError'), "PynlmError should exist"
    assert hasattr(pynlm, 'InvalidConfigurationError'), "InvalidConfigurationError should exist"
    assert hasattr(pynlm, 'NotInitializedError'), "NotInitializedError should exist"
    assert hasattr(pynlm, 'InvalidInputError'), "InvalidInputError should exist"
    
    # Test error handling context
    assert hasattr(pynlm.utils, 'ErrorHandlingContext'), "ErrorHandlingContext should exist"
    
    print("  ✓ Error handling tests passed")

def test_advanced_features():
    """Test advanced features."""
    print("Testing advanced features...")
    
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Test step_batch
    assert hasattr(brain, 'step_batch'), "Brain should have step_batch method"
    steps = 10
    result = brain.step_batch(steps)
    assert result == steps, f"step_batch should return number of steps processed, got {result}"
    
    # Test get_summary
    assert hasattr(brain, 'get_summary'), "Brain should have get_summary method"
    summary = brain.get_summary()
    assert isinstance(summary, dict), "get_summary should return a dictionary"
    assert 'spike_count' in summary, "Summary should contain spike_count"
    assert 'active_neurons' in summary, "Summary should contain active_neurons"
    
    # Test save/load (convenience wrappers)
    assert hasattr(brain, 'save_to_file'), "Brain should have save_to_file method"
    assert hasattr(brain, 'load_from_file'), "Brain should have load_from_file method"
    
    print("  ✓ Advanced features tests passed")

def test_agent_features():
    """Test agent-specific features."""
    print("Testing agent features...")
    
    brain = pynlm.Brain()
    brain.initialize()
    
    # Create agent brain
    agent_brain = pynlm.create_agent_brain(brain)
    
    # Test Pythonic properties
    assert hasattr(agent_brain, 'reward_modulation_enabled'), "Agent should have reward_modulation_enabled property"
    assert hasattr(agent_brain, 'curiosity_enabled'), "Agent should have curiosity_enabled property"
    
    # Test enable methods
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    
    assert agent_brain.reward_modulation_enabled, "Reward modulation should be enabled"
    assert agent_brain.curiosity_enabled, "Curiosity should be enabled"
    
    # Test process_sensory_batch
    assert hasattr(agent_brain, 'process_sensory_batch'), "Agent should have process_sensory_batch method"
    
    print("  ✓ Agent features tests passed")

def test_world_features():
    """Test world-specific features."""
    print("Testing world features...")
    
    world = pynlm.SimpleWorld()
    
    # Test configure_advanced
    assert hasattr(world, 'configure_advanced'), "World should have configure_advanced method"
    
    # Test configure_advanced with various options
    config = {
        "max_energy": 200.0,
        "energy_decay_rate": 0.005,
        "random_seed": 12345,
        "agent_start_x": 0.0,
        "agent_start_y": 0.0
    }
    
    result = world.configure_advanced(**config)
    assert result is True, "configure_advanced should return True"
    
    # Test save/load convenience wrappers
    assert hasattr(world, 'save_to_file'), "World should have save_to_file method"
    assert hasattr(world, 'load_from_file'), "World should have load_from_file method"
    
    print("  ✓ World features tests passed")

def test_utility_functions():
    """Test utility functions."""
    print("Testing utility functions...")
    
    # Test get_version_info
    version_info = pynlm.utils.get_version_info()
    assert isinstance(version_info, dict), "get_version_info should return a dictionary"
    assert 'version' in version_info, "Version info should contain version"
    assert 'python_version' in version_info, "Version info should contain python_version"
    
    # Test validate_brain (with brain)
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    assert pynlm.utils.validate_brain(brain), "validate_brain should return True for valid brain"
    
    # Test get_memory_usage
    memory_info = pynlm.utils.get_memory_usage()
    assert isinstance(memory_info, dict), "get_memory_usage should return a dictionary"
    assert 'rss_mb' in memory_info, "Memory info should contain rss_mb"
    assert 'vms_mb' in memory_info, "Memory info should contain vms_mb"
    
    print("  ✓ Utility functions tests passed")

def test_error_context_manager():
    """Test error handling context manager."""
    print("Testing error context manager...")
    
    context = pynlm.utils.ErrorHandlingContext()
    
    # Test context manager
    with context:
        assert context.last_error is None, "Last error should be None before exception"
    
    print("  ✓ Error context manager tests passed")

def test_example_script():
    """Test the example script."""
    print("Testing example script...")
    
    # Import and run the example script in a subprocess
    import subprocess
    import tempfile
    
    # Create a temporary directory for testing
    with tempfile.TemporaryDirectory() as temp_dir:
        example_path = os.path.join(os.path.dirname(__file__), 'examples', 'basic_usage.py')
        
        # Run the example script
        result = subprocess.run(
            [sys.executable, example_path],
            cwd=temp_dir,
            capture_output=True,
            text=True,
            timeout=30
        )
        
        # The example should run without crashing (even if some features are mocked)
        assert result.returncode == 0, f"Example script should run successfully, got return code {result.returncode}"
        
        print("  ✓ Example script tests passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_error_handling,
        test_advanced_features,
        test_agent_features,
        test_world_features,
        test_utility_functions,
        test_error_context_manager,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ {test.__name__} failed: {e}")
            failed += 1
    
    # Test example script separately
    try:
        test_example_script()
        passed += 1
    except Exception as e:
        print(f"  ✗ test_example_script failed: {e}")
        failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! ✓")
        return True
    else:
        print(f"{failed} tests failed ✗")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
