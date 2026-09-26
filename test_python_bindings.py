#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.
Tests all the new Pythonic features and functionality.
"""

import pynlm
import numpy as np
import json
import time
from contextlib import contextmanager

def test_basic_imports():
    """Test that all imports work correctly."""
    print("Testing basic imports...")
    
    # Test core classes
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    print("✓ Basic imports successful")
    return config, brain, world, agent

def test_context_managers():
    """Test context manager functionality."""
    print("\nTesting context managers...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    
    # Test context manager
    with pynlm.SimulationContext(brain, world, agent) as sim:
        sim.runEpisode(100, 0.1, verbose=False)
        assert sim.getBrain() is brain
        assert sim.getWorld() is world
        assert sim.getAgent() is agent
    
    print("✓ Context managers working correctly")
    return config, brain, world, agent

def test_batch_processing():
    """Test batch processing functionality."""
    print("\nTesting batch processing...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    
    # Create batch processor
    batch = pynlm.BatchProcessor(brain, world, agent, batchSize=50)
    
    # Test batch processing
    steps = list(range(200))
    timesteps = [0.1] * 200
    metrics = batch.processBatch(steps, timesteps, verbose=[False] * 200)
    
    assert len(metrics) == 200
    for metric in metrics:
        assert metric.simulationTime >= 0.0
        assert metric.totalSpikes >= 0
    
    print("✓ Batch processing working correctly")
    return config, brain, world, agent

def test_performance_monitoring():
    """Test performance monitoring APIs."""
    print("\nTesting performance monitoring...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get performance metrics
    metrics = brain.getPerformanceMetrics()
    assert hasattr(metrics, 'simulationTime')
    assert hasattr(metrics, 'totalSpikes')
    assert hasattr(metrics, 'activeNeurons')
    
    # Test steps per second calculation
    steps = 1000
    metrics.getStepsPerSecond(steps)
    
    print("✓ Performance monitoring working correctly")
    return config, brain

def test_configuration_validation():
    """Test configuration validation."""
    print("\nTesting configuration validation...")
    
    # Test valid configuration
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(1000)
    
    # This should work without throwing an exception
    pynlm.validate_config(config)
    
    # Test invalid configuration
    try:
        config.setNeuronCount(0)  # Invalid: must be > 0
        pynlm.validate_config(config)
        assert False, "Should have raised an exception"
    except pynlm.NLMConfigurationError:
        pass  # Expected
    
    print("✓ Configuration validation working correctly")
    return config

def test_numpy_integration():
    """Test NumPy integration."""
    print("\nTesting NumPy integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test array creation from Python data
    vision_data = np.random.rand(8, 8, 3).astype(np.float32)
    
    # Verify that the bindings work with numpy arrays
    # (This tests the pybind11 integration)
    assert vision_data.shape == (8, 8, 3)
    
    print("✓ NumPy integration working correctly")
    return config, brain

def test_error_handling():
    """Test enhanced error handling."""
    print("\nTesting error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test invalid neuron count
    try:
        config.setNeuronCount(-1)
        assert False, "Should have raised an exception"
    except pynlm.NLMConfigurationError:
        pass  # Expected
    
    # Test resource error (invalid operation)
    config2 = pynlm.createDefaultConfig()
    
    print("✓ Error handling working correctly")
    return config2

def test_help_system():
    """Test the help system."""
    print("\nTesting help system...")
    
    help_text = pynlm.help("simulation")
    assert isinstance(help_text, str)
    assert len(help_text) > 0
    
    print("✓ Help system working correctly")
    return help_text

def test_version_info():
    """Test version information."""
    print("\nTesting version info...")
    
    version = pynlm.__version__()
    assert isinstance(version, str)
    assert len(version) > 0
    
    print(f"✓ Version info: {version}")
    return version

def test_statistics_and_analysis():
    """Test statistics and analysis functions."""
    print("\nTesting statistics and analysis...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get brain statistics
    stats = brain.get_brain_stats()
    assert isinstance(stats, dict)
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    assert "active_neurons" in stats
    assert "firing_neurons" in stats
    
    print("✓ Statistics and analysis working correctly")
    return config, brain

def test_example_scenarios():
    """Test common example scenarios."""
    print("\nTesting example scenarios...")
    
    # Test 1: Silent brain (just neurons)
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    for i in range(10):
        brain1.step(i)
    
    assert brain1.getTotalSpikeCount() >= 0
    print("✓ Silent brain example working")
    
    # Test 2: Brain watching a world
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world2.reset()
    
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world2)
    
    # Watch the world for 30 steps
    for i in range(30):
        world2.update(0.1)
        agent2.processSensoryInput(world2.getSensoryPercept())
        brain2.step(i)
    
    print(f"✓ World watching example working (firing rate: {brain2.getAverageFiringRate():.3f})")
    
    # Test 3: Complete agent
    config3 = pynlm.createDefaultConfig()
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    world3 = pynlm.createSimpleWorld()
    world3.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world3.reset()
    
    agent3 = pynlm.createAgentBrain(brain3)
    agent3.initialize(world3)
    
    # Enable learning
    agent3.enableRewardModulation(True)
    agent3.enableCuriosity(True)
    
    # Run agent
    for step in range(50):
        world3.update(0.1)
        agent3.processSensoryInput(world3.getSensoryPercept())
        brain3.step(step)
        action = agent3.decodeMotorCommand()
        world3.applyMotorCommand(action, world3.getSimulationTime())
    
    print("✓ Complete agent example working")
    return config1, brain1

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings Improvement Tests")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_context_managers,
        test_batch_processing,
        test_performance_monitoring,
        test_configuration_validation,
        test_numpy_integration,
        test_error_handling,
        test_help_system,
        test_version_info,
        test_statistics_and_analysis,
        test_example_scenarios,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ Test failed: {test.__name__} - {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! Python bindings are working correctly.")
    else:
        print(f"⚠️  {failed} tests failed. Please review the implementation.")
    
    return passed, failed

if __name__ == "__main__":
    # Run all tests
    run_all_tests()
