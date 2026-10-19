#!/usr/bin/env python3

"""
Comprehensive test suite for NLM Python bindings improvements.

This script tests all the new features and improvements made to the Python bindings.
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build/python'))

try:
    from pynlm import (
        createBrain, createDefaultConfig, createSimpleWorld,
        createOptimizedConfig, createSimpleVisionInput, createSimpleAudioInput,
        validateConfig, formatConfig, check, safeExecute, NLMError,
        createPerformanceMonitor, SimulationHelper,
        debugBrainState, generateNetworkData, show_docs,
        Vision, Audio, Config, Brain, SimpleWorld
    )
except ImportError as e:
    print(f"Import error: {e}")
    print("Make sure the Python bindings have been built.")
    sys.exit(1)

def test_basic_import():
    """Test that all imports work correctly."""
    print("✓ Basic imports successful")

def test_config_creation():
    """Test configuration creation and manipulation."""
    print("Testing configuration creation...")
    
    # Create default config
    config = createDefaultConfig()
    assert isinstance(config, Config)
    
    # Set values
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Get values
    assert config.getOr("test_int", 0) == 42
    assert abs(config.getOr("test_double", 0.0) - 3.14) < 0.001
    assert config.getOr("test_string", "") == "hello"
    assert config.getOr("test_bool", False) == True
    
    # Check existence
    assert config.has("test_int")
    assert not config.has("nonexistent")
    
    # Test summary
    summary = config.summary()
    assert "test_int" in summary
    
    print("✓ Configuration creation and manipulation successful")

def test_config_optimization():
    """Test configuration optimization from environment variables."""
    print("Testing configuration optimization...")
    
    # Create optimized config with environment variables
    env_vars = {
        "NLM_RANDOM_SEED": "12345",
        "NLM_NEURON_COUNT": "2000"
    }
    
    config = createOptimizedConfig(env_vars)
    assert config.getOr("random_seed", 0) == 12345
    assert config.getOr("neuron_count", 0) == 2000
    
    print("✓ Configuration optimization successful")

def test_config_validation():
    """Test configuration validation."""
    print("Testing configuration validation...")
    
    config = createDefaultConfig()
    
    # Test valid configuration
    errors = validateConfig(config)
    assert len(errors) == 0, f"Valid config should have no errors: {errors}"
    
    # Test invalid configuration
    config.set("neuron_count", -5)
    errors = validateConfig(config)
    assert len(errors) > 0, "Invalid config should have errors"
    
    config.set("simulation_timestep", -0.001)
    errors = validateConfig(config)
    assert len(errors) > 0, "Invalid timestep should have errors"
    
    print("✓ Configuration validation successful")

def test_config_formatting():
    """Test configuration formatting."""
    print("Testing configuration formatting...")
    
    config = createDefaultConfig()
    formatted = formatConfig(config)
    
    assert "NLM Configuration Summary:" in formatted
    assert "random_seed:" in formatted
    assert "simulation_timestep:" in formatted
    
    print("✓ Configuration formatting successful")

def test_vision_input():
    """Test vision input creation and NumPy integration."""
    print("Testing vision input...")
    
    # Create vision input
    vision = createSimpleVisionInput(32, 32, 3)
    assert isinstance(vision, Vision)
    
    # Test properties
    assert vision.getWidth() == 32
    assert vision.getHeight() == 32
    assert vision.getChannels() == 3
    
    # Test data
    data = vision.getData()
    assert len(data) == 32 * 32 * 3
    
    # Test NumPy conversion
    numpy_array = vision.to_numpy()
    assert numpy_array.shape == (32, 32, 3)
    
    print("✓ Vision input testing successful")

def test_audio_input():
    """Test audio input creation."""
    print("Testing audio input...")
    
    # Create audio input
    audio = createSimpleAudioInput(44100, 44100)
    assert isinstance(audio, Audio)
    
    # Test properties
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 44100
    
    # Test data
    data = audio.getData()
    assert len(data) == 44100
    
    # Test NumPy conversion
    numpy_array = audio.to_numpy()
    assert numpy_array.shape == (44100,)
    
    print("✓ Audio input testing successful")

def test_error_handling():
    """Test error handling utilities."""
    print("Testing error handling...")
    
    # Test check function
    try:
        check(True, "This should not raise an error")
        print("✓ check() with True works")
    except Exception as e:
        print(f"✗ check() with True raised error: {e}")
    
    # Test check function with false condition
    try:
        check(False, "This error should be raised")
        print("✗ check() with False should have raised an error")
    except NLMError as e:
        print("✓ check() with False correctly raises NLMError")
    
    # Test safeExecute
    try:
        result = safeExecute(lambda: 42 + 8, "Test operation")
        assert result == 50
        print("✓ safeExecute works with successful function")
    except Exception as e:
        print(f"✗ safeExecute failed: {e}")
    
    try:
        safeExecute(lambda: 1 / 0, "Division by zero")
        print("✗ safeExecute should have caught division by zero")
    except NLMError:
        print("✓ safeExecute correctly catches exceptions")
    
    print("✓ Error handling testing successful")

def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("Testing performance monitoring...")
    
    from pynlm import createPerformanceMonitor
    
    monitor = createPerformanceMonitor()
    assert monitor is not None
    
    # Test basic properties
    monitor.start()
    import time
    time.sleep(0.01)
    monitor.stop()
    
    assert monitor.getTotalTime() > 0
    assert monitor.getStepCount() == 0  # Not incremented yet
    
    # Test step increment
    monitor.incrementSteps(5)
    assert monitor.getStepCount() == 5
    
    # Test average time
    avg_time = monitor.getAverageTimePerStep()
    assert avg_time >= 0
    
    # Test reset
    monitor.reset()
    assert monitor.getStepCount() == 0
    assert monitor.getTotalTime() == 0
    
    print("✓ Performance monitoring testing successful")

def test_simulation_helper():
    """Test simulation helper (requires brain to be created)."""
    print("Testing simulation helper...")
    
    # Create a brain and world
    config = createDefaultConfig()
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(100, 100, 20, 20)
    
    # Create simulation helper
    helper = brain.createSimulationHelper()
    assert helper is not None
    
    # Test that helper has brain reference
    assert helper.getBrain() is brain
    
    print("✓ Simulation helper testing successful")

def test_debugging_utilities():
    """Test debugging utilities."""
    print("Testing debugging utilities...")
    
    # Create brain for debugging
    config = createDefaultConfig()
    brain = createBrain(config)
    
    # Test debug brain state
    debug_str = debugBrainState(brain)
    assert "Brain State:" in debug_str
    assert "Neurons:" in debug_str
    
    # Test network data generation
    network_data = generateNetworkData(brain)
    assert isinstance(network_data, dict)
    assert "region_sizes" in network_data
    assert "neuron_types" in network_data
    
    print("✓ Debugging utilities testing successful")

def test_help_system():
    """Test help system."""
    print("Testing help system...")
    
    docs = show_docs()
    assert "Available Classes:" in docs
    assert "Brain:" in docs
    assert "Config:" in docs
    
    print("✓ Help system testing successful")

def test_documentation():
    """Test comprehensive documentation."""
    print("Testing documentation...")
    
    from pynlm import show_docs
    
    docs = show_docs()
    
    # Check for key sections
    sections = [
        "Available Classes",
        "Brain:",
        "Config:",
        "SensoryInput:",
        "Action:",
        "World:",
        "AgentBrain:",
        "Available Functions:",
        "createDefaultConfig",
        "createBrain",
        "Error Handling:",
        "Performance Monitoring:"
    ]
    
    for section in sections:
        assert section in docs, f"Documentation missing section: {section}"
    
    print("✓ Documentation testing successful")

def main():
    """Run all tests."""
    print("Running NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        test_basic_import,
        test_config_creation,
        test_config_optimization,
        test_config_validation,
        test_config_formatting,
        test_vision_input,
        test_audio_input,
        test_error_handling,
        test_performance_monitoring,
        test_simulation_helper,
        test_debugging_utilities,
        test_help_system,
        test_documentation,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        print(f"\n{failed} tests failed. Please check the output above for details.")
        return 1
    else:
        print("\nAll tests passed! 🎉")
        return 0

if __name__ == "__main__":
    sys.exit(main())
