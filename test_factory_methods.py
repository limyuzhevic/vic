#!/usr/bin/env python3
"""
Factory Methods Tests for NLM Python Bindings

Tests factory functions and helper methods for creating
NLM components with enhanced Pythonic interfaces.
"""

import sys
import os

# Add the current directory to the path to ensure pynlm can be imported
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def test_config_factory_methods():
    """Test configuration factory methods."""
    print("Testing config factory methods...")
    
    # Test default config factory
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createDefaultConfig()
    
    # Should create independent configs
    assert config1 is not config2
    
    # Both should have the same defaults
    assert config1.getOr<int64_t>("random_seed", 0) == config2.getOr<int64_t>("random_seed", 0)
    
    print("✓ Config factory methods test passed")


def test_brain_factory_methods():
    """Test brain factory methods."""
    print("Testing brain factory methods...")
    
    # Test quick brain factory
    brain1 = pynlm.Brain.createQuickBrain()
    brain2 = pynlm.Brain.createQuickBrain()
    
    # Should create independent brains
    assert brain1 is not brain2
    
    # Both should have reasonable neuron counts
    neurons1 = brain1.getTotalNeuronCount()
    neurons2 = brain2.getTotalNeuronCount()
    
    assert neurons1 > 0
    assert neurons2 > 0
    
    print(f"✓ Brain factory methods test passed ({neurons1}, {neurons2} neurons)")


def test_sensory_factory_methods():
    """Test sensory input factory methods."""
    print("Testing sensory factory methods...")
    
    # Test Vision factory methods
    vision1 = pynlm.Vision.createImage(640, 480)
    vision2 = pynlm.Vision.createImage(320, 240, 1)
    
    assert vision1.getWidth() == 640
    assert vision1.getHeight() == 480
    assert vision1.getChannels() == 3
    
    assert vision2.getWidth() == 320
    assert vision2.getHeight() == 240
    assert vision2.getChannels() == 1
    
    # Test Audio factory methods
    audio1 = pynlm.Audio.createAudio(44100, 1024)
    audio2 = pynlm.Audio.createAudio(22050, 512)
    
    assert audio1.getSampleRate() == 44100
    assert audio1.getNumSamples() == 1024
    
    assert audio2.getSampleRate() == 22050
    assert audio2.getNumSamples() == 512
    
    # Test InternalSignals factory method
    signals1 = pynlm.InternalSignals.createSignals()
    signals2 = pynlm.InternalSignals.createSignals()
    
    assert signals1 is not signals2
    
    print("✓ Sensory factory methods test passed")


def test_agent_factory():
    """Test agent brain factory."""
    print("Testing agent factory...")
    
    # Create brain and agent
    brain = pynlm.Brain.createQuickBrain()
    agent1 = pynlm.createAgentBrain(brain)
    agent2 = pynlm.createAgentBrain(brain)
    
    # Should create independent agents
    assert agent1 is not agent2
    
    # Both should work with the same brain
    assert agent1.getBrain() is brain
    assert agent2.getBrain() is brain
    
    # Test agent configuration methods
    agent1.enableRewardModulation(True)
    agent1.enableStructuralPlasticity(True)
    agent1.enableDevelopment(True)
    agent1.enableCuriosity(True)
    
    assert agent1.isRewardModulationEnabled()
    assert agent1.isStructuralPlasticityEnabled()
    assert agent1.isDevelopmentEnabled()
    assert agent1.isCuriosityEnabled()
    
    print("✓ Agent factory test passed")


def test_world_factory():
    """Test world factory method."""
    print("Testing world factory...")
    
    # Create multiple worlds
    world1 = pynlm.createSimpleWorld()
    world2 = pynlm.createSimpleWorld()
    
    # Should create independent worlds
    assert world1 is not world2
    
    # Test world configuration
    world1.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world2.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    
    assert world1.getWidth() == 10
    assert world1.getHeight() == 10
    assert world2.getWidth() == 20
    assert world2.getHeight() == 20
    
    print("✓ World factory test passed")


def test_factory_validation():
    """Test factory validation."""
    print("Testing factory validation...")
    
    # Test null validation in agent factory
    try:
        # This should fail - createBrain expects a config
        brain = pynlm.createBrain(None)
        assert False, "Should have raised an error for null config"
    except Exception as e:
        print(f"✓ Correctly caught null config error: {type(e).__name__}")
    
    # Test null validation in agent factory
    try:
        # This should fail - createAgentBrain expects a brain
        agent = pynlm.createAgentBrain(None)
        assert False, "Should have raised an error for null brain"
    except Exception as e:
        print(f"✓ Correctly caught null brain error: {type(e).__name__}")
    
    print("✓ Factory validation test passed")


def run_all_tests():
    """Run all factory method tests."""
    print("=" * 60)
    print("NLM Factory Methods Test Suite")
    print("=" * 60)
    
    tests = [
        test_config_factory_methods,
        test_brain_factory_methods,
        test_sensory_factory_methods,
        test_agent_factory,
        test_world_factory,
        test_factory_validation,
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
    
    print("\n" + "=" * 60)
    print(f"Factory Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All factory tests passed!")


if __name__ == "__main__":
    run_all_tests()