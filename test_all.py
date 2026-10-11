#!/usr/bin/env python3
"""Simple test runner for enhanced NLM Python bindings."""
import sys
import os
import traceback

# Import pynlm at module level
import pynlm

def run_test(test_name, test_func):
    """Run a single test and return success status."""
    print(f"\n{'='*70}")
    print(f"Running: {test_name}")
    print(f"{'='*70}")
    
    try:
        test_func()
        print(f"✓ {test_name} PASSED")
        return True
    except Exception as e:
        print(f"✗ {test_name} FAILED")
        print(f"Error: {e}")
        traceback.print_exc()
        return False

def test_basic_functionality():
    """Test basic functionality."""
    from pynlm import Brain, Config
    
    # Test config creation
    config = Config()
    assert config is not None
    assert config.get("simulation_max_steps") == 1000
    
    # Test brain creation and initialization
    brain = Brain(config)
    assert brain.initialize() is True
    
    # Test property access
    assert hasattr(brain, 'spike_count')
    assert hasattr(brain, 'synapse_count')
    assert hasattr(brain, 'active_neurons')
    assert hasattr(brain, 'firing_neurons')
    assert hasattr(brain, 'ei_ratio')
    assert hasattr(brain, 'region_names')
    
    # Test step execution
    result = brain.step(0, 0.0)
    assert result is True
    assert brain.spike_count == 1
    
    print("  ✓ Basic functionality tests passed")

def test_pythonic_features():
    """Test pythonic features."""
    from pynlm import Brain, AgentBrain, SimpleWorld
    
    brain = Brain()
    brain.initialize()
    
    # Test pythonic properties
    assert hasattr(brain, 'spike_count')
    assert isinstance(brain.spike_count, int)
    
    assert hasattr(brain, 'synapse_count')
    assert isinstance(brain.synapse_count, int)
    
    assert hasattr(brain, 'active_neurons')
    assert isinstance(brain.active_neurons, int)
    
    assert hasattr(brain, 'firing_neurons')
    assert isinstance(brain.firing_neurons, int)
    
    assert hasattr(brain, 'ei_ratio')
    assert isinstance(brain.ei_ratio, float)
    
    # Test agent features
    agent = AgentBrain(brain)
    agent.initialize(SimpleWorld())
    
    agent.enable_reward_modulation(True)
    assert agent.reward_modulation_enabled is True
    
    agent.enable_curiosity(True)
    assert agent.curiosity_enabled is True
    
    # Test batch operations
    result = brain.step_batch(10)
    assert result == 10
    
    print("  ✓ Pythonic features tests passed")

def test_error_handling():
    """Test error handling."""
    from pynlm import Brain
    
    brain = Brain()
    
    # Test error classes exist
    assert hasattr(pynlm, 'PynlmError')
    assert hasattr(pynlm, 'InvalidConfigurationError')
    assert hasattr(pynlm, 'NotInitializedError')
    assert hasattr(pynlm, 'InvalidInputError')
    
    # Test NotInitializedError
    try:
        brain.step()
        assert False, "Should have raised NotInitializedError"
    except pynlm.NotInitializedError:
        pass
    
    # Test InvalidInputError
    try:
        brain.save("")
        assert False, "Should have raised InvalidInputError"
    except pynlm.InvalidInputError:
        pass
    
    print("  ✓ Error handling tests passed")

def test_advanced_features():
    """Test advanced features."""
    import pynlm
    import pynlm.utils as utils
    from pynlm import Brain, NeuronType, SynapseType
    
    brain = Brain()
    brain.initialize()
    
    # Test type definitions
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.SynapseType.Inhibitory == 1
    
    # Test constants
    assert pynlm.PI > 3.0
    assert pynlm.EPSILON == 1e-10
    
    # Test utility functions
    version_info = utils.get_version_info()
    assert isinstance(version_info, dict)
    
    assert utils.validate_brain(brain) is True
    
    # Test advanced brain features
    summary = brain.get_summary()
    assert isinstance(summary, dict)
    assert 'spike_count' in summary
    
    print("  ✓ Advanced features tests passed")

def test_integration():
    """Test integration features."""
    from pynlm import Brain, AgentBrain, SimpleWorld
    
    # Create all components
    brain = Brain()
    world = SimpleWorld()
    agent = AgentBrain(brain)
    
    # Initialize components
    brain.initialize()
    agent.initialize(world)
    
    # Configure agent
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run integration loop
    for i in range(5):
        brain.step(i, i * 0.1)
        world.update(i * 0.1)
    
    # Verify integration
    assert brain.spike_count == 5
    assert agent.reward_modulation_enabled is True
    
    print("  ✓ Integration tests passed")

def test_performance():
    """Test performance features."""
    import pynlm.utils as utils
    from pynlm import Brain
    import time
    
    brain = Brain()
    brain.initialize()
    
    # Test batch processing
    start_time = time.time()
    result = brain.step_batch(100)
    end_time = time.time()
    
    assert result == 100
    elapsed = end_time - start_time
    assert elapsed < 1.0
    
    # Test benchmarking
    benchmark_result = utils.benchmark_brain(brain, steps=10)
    assert isinstance(benchmark_result, dict)
    assert 'avg_step_time_ms' in benchmark_result
    
    # Test memory usage
    memory_info = utils.get_memory_usage()
    assert isinstance(memory_info, dict)
    assert 'rss_mb' in memory_info
    
    print("  ✓ Performance tests passed")

def test_edge_cases():
    """Test edge cases."""
    import pynlm
    from pynlm import Brain, NeuronId
    
    # Initialize brain first
    brain = Brain()
    brain.initialize()
    
    # Test invalid inputs
    try:
        brain.inject_current(NeuronId(-1), 1.0)
        assert False, "Should have raised InvalidInputError"
    except pynlm.InvalidInputError:
        pass  # Expected
    
    # Test state validation
    brain.reset()
    assert brain.spike_count == 0
    
    print("  ✓ Edge cases tests passed")

def test_memory_systems():
    """Test memory systems."""
    from pynlm import Brain
    
    brain = Brain()
    brain.initialize()
    
    # Test memory system access
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()
    
    assert working_memory is None
    assert episodic_memory is None
    assert associative_memory is None
    
    # Test memory operations
    brain.log_status()
    
    print("  ✓ Memory systems tests passed")

def run_all_tests():
    """Run all tests."""
    print("Enhanced NLM Python Bindings - Test Suite")
    print("=" * 70)
    
    tests = [
        ("Basic Functionality Tests", test_basic_functionality),
        ("Pythonic Features Tests", test_pythonic_features),
        ("Error Handling Tests", test_error_handling),
        ("Advanced Features Tests", test_advanced_features),
        ("Integration Tests", test_integration),
        ("Performance Tests", test_performance),
        ("Edge Cases Tests", test_edge_cases),
        ("Memory Systems Tests", test_memory_systems),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        if run_test(test_name, test_func):
            passed += 1
        else:
            failed += 1
    
    print(f"\n{'='*70}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'='*70}")
    
    if failed == 0:
        print("🎉 All tests passed! Enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print(f"❌ {failed} test(s) failed.")
        return False

if __name__ == "__main__":
    try:
        success = run_all_tests()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\nTest execution failed with exception: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)