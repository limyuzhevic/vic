#!/usr/bin/env python3
"""
Error Handling Tests for NLM Python Bindings

Tests error handling, validation, and edge cases in the
NLM Python bindings.
"""

import sys
import os

# Add the current directory to the path to ensure pynlm can be imported
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def test_null_config_validation():
    """Test validation of null config in brain creation."""
    print("Testing null config validation...")
    
    # This should raise an exception due to validation
    try:
        brain = pynlm.createBrain(None)
        print("✗ Expected exception for null config but none was raised")
        return False
    except (TypeError, ValueError, Exception) as e:
        print(f"✓ Correctly caught null config error: {type(e).__name__}")
        return True


def test_null_brain_validation():
    """Test validation of null brain in agent creation."""
    print("Testing null brain validation...")
    
    try:
        agent = pynlm.createAgentBrain(None)
        print("✗ Expected exception for null brain but none was raised")
        return False
    except (TypeError, ValueError, Exception) as e:
        print(f"✓ Correctly caught null brain error: {type(e).__name__}")
        return True


def test_config_parameter_validation():
    """Test configuration parameter validation."""
    print("Testing config parameter validation...")
    
    config = pynlm.createDefaultConfig()
    
    # Test setting valid values
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    assert config.has("test_int")
    assert config.has("test_float")
    assert config.has("test_string")
    assert config.has("test_bool")
    
    # Test getting values
    int_val = config.getOr<int>("test_int", 0)
    assert int_val == 42
    
    float_val = config.getOr<double>("test_float", 0.0)
    assert abs(float_val - 3.14) < 0.001
    
    string_val = config.getOr<std::string>("test_string", "")
    assert string_val == "hello"
    
    bool_val = config.getOr<bool>("test_bool", False)
    assert bool_val == True
    
    print("✓ Config parameter validation test passed")
    return True


def test_missing_key_handling():
    """Test handling of missing configuration keys."""
    print("Testing missing key handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test getting missing keys with defaults
    missing_int = config.getOr<int>("missing_key", 999)
    assert missing_int == 999
    
    missing_string = config.getOr<std::string>("missing_key", "default")
    assert missing_string == "default"
    
    missing_bool = config.getOr<bool>("missing_key", False)
    assert missing_bool == False
    
    print("✓ Missing key handling test passed")
    return True


def test_sensory_input_validation():
    """Test sensory input validation."""
    print("Testing sensory input validation...")
    
    # Test Vision validation
    vision = pynlm.Vision.createImage(640, 480)
    assert vision.getWidth() == 640
    assert vision.getHeight() == 480
    
    # Test Audio validation
    audio = pynlm.Audio.createAudio(44100, 1024)
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    
    # Test InternalSignals validation
    signals = pynlm.InternalSignals.createSignals()
    signals.addSignal(1.0)
    signals.addSignal(2.0)
    
    data = signals.getData()
    assert len(data) == 2
    assert data[0] == 1.0
    assert data[1] == 2.0
    
    print("✓ Sensory input validation test passed")
    return True


def test_brain_initialization_validation():
    """Test brain initialization validation."""
    print("Testing brain initialization validation...")
    
    brain = pynlm.Brain.createQuickBrain()
    
    # Test that brain is initialized
    neurons = brain.getTotalNeuronCount()
    assert neurons > 0
    
    # Test stepping without explicit initialization
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    after_spikes = brain.getTotalSpikeCount()
    
    # Should not crash
    print(f"✓ Brain initialization validation test passed")
    print(f"  Neurons: {neurons}, Spikes before/after: {initial_spikes}/{after_spikes}")
    return True


def test_context_manager_validation():
    """Test context manager validation."""
    print("Testing context manager validation...")
    
    # Test Vision context manager
    with pynlm.Vision.createImage(100, 100) as vision:
        assert vision.getWidth() == 100
        assert vision.getHeight() == 100
    
    # Test Audio context manager
    with pynlm.Audio.createAudio(8000, 512) as audio:
        assert audio.getSampleRate() == 8000
    
    # Test InternalSignals context manager
    with pynlm.InternalSignals.createSignals() as signals:
        signals.addSignal(3.14)
        assert len(signals.getData()) == 1
    
    print("✓ Context manager validation test passed")
    return True


def test_factory_method_validation():
    """Test factory method validation."""
    print("Testing factory method validation...")
    
    # Test that factory methods create valid objects
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    brain = pynlm.Brain.createQuickBrain()
    assert brain is not None
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    vision = pynlm.Vision.createImage(100, 100)
    assert vision is not None
    
    audio = pynlm.Audio.createAudio(8000, 512)
    assert audio is not None
    
    signals = pynlm.InternalSignals.createSignals()
    assert signals is not None
    
    print("✓ Factory method validation test passed")
    return True


def run_all_tests():
    """Run all error handling tests."""
    print("=" * 60)
    print("NLM Error Handling Test Suite")
    print("=" * 60)
    
    tests = [
        test_null_config_validation,
        test_null_brain_validation,
        test_config_parameter_validation,
        test_missing_key_handling,
        test_sensory_input_validation,
        test_brain_initialization_validation,
        test_context_manager_validation,
        test_factory_method_validation,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Error Handling Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All error handling tests passed!")


if __name__ == "__main__":
    run_all_tests()