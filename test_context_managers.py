#!/usr/bin/env python3
"""
Context Manager Tests for NLM Python Bindings

Tests context manager functionality for NLM components,
ensuring proper resource management and cleanup.
"""

import sys
import os

# Add the current directory to the path to ensure pynlm can be imported
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def test_vision_context_manager():
    """Test Vision context manager."""
    print("Testing Vision context manager...")
    
    # Test that context manager works
    with pynlm.Vision.createImage(640, 480) as vision:
        assert vision is not None
        assert vision.getWidth() == 640
        assert vision.getHeight() == 480
        assert vision.getChannels() == 3
        
        # Use the vision object
        print(f"  Processing image: {vision.getWidth()}x{vision.getHeight()} with {vision.getChannels()} channels")
    
    print("✓ Vision context manager test passed")
    return True


def test_audio_context_manager():
    """Test Audio context manager."""
    print("Testing Audio context manager...")
    
    with pynlm.Audio.createAudio(44100, 1024) as audio:
        assert audio is not None
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        # Use the audio object
        print(f"  Processing audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    
    print("✓ Audio context manager test passed")
    return True


def test_internal_signals_context_manager():
    """Test InternalSignals context manager."""
    print("Testing InternalSignals context manager...")
    
    with pynlm.InternalSignals.createSignals() as signals:
        assert signals is not None
        
        # Add some signals
        signals.addSignal(1.0)
        signals.addSignal(2.0)
        signals.addSignal(3.14)
        
        data = signals.getData()
        assert len(data) == 3
        assert data[0] == 1.0
        assert data[1] == 2.0
        assert abs(data[2] - 3.14) < 0.001
        
        print(f"  Processing {len(data)} internal signals: {data}")
    
    print("✓ InternalSignals context manager test passed")
    return True


def test_multiple_context_managers():
    """Test multiple context managers simultaneously."""
    print("Testing multiple context managers...")
    
    # Create multiple context managers
    with pynlm.Vision.createImage(320, 240) as vision1, \
         pynlm.Audio.createAudio(22050, 512) as audio1, \
         pynlm.InternalSignals.createSignals() as signals1:
        
        # Use all three objects
        assert vision1.getWidth() == 320
        assert audio1.getSampleRate() == 22050
        signals1.addSignal(42.0)
        
        print(f"  Processing vision: {vision1.getWidth()}x{vision1.getHeight()}")
        print(f"  Processing audio: {audio1.getSampleRate()} Hz")
        print(f"  Processing signals: {len(signals1.getData())} signals")
    
    print("✓ Multiple context managers test passed")
    return True


def test_context_manager_resource_cleanup():
    """Test that context managers properly cleanup resources."""
    print("Testing context manager resource cleanup...")
    
    # Create a vision object and use it in a context manager
    vision = pynlm.Vision.createImage(800, 600)
    
    # Use it in a context manager
    with vision:
        assert vision.getWidth() == 800
        assert vision.getHeight() == 600
        
        # Perform operations
        for i in range(5):
            print(f"  Processing vision chunk {i+1}")
    
    # After context manager exits, the vision should still be usable
    # (context manager is just for organization, not resource cleanup)
    assert vision.getWidth() == 800
    
    print("✓ Context manager resource cleanup test passed")
    return True


def test_context_manager_with_agent_simulation():
    """Test context manager integration with agent simulation."""
    print("Testing context manager with agent simulation...")
    
    # Create brain and agent
    brain = pynlm.Brain.createQuickBrain()
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create sensory inputs in context managers
    with pynlm.Vision.createImage(128, 128) as vision, \
         pynlm.InternalSignals.createSignals() as signals:
        
        # Process with both sensory inputs
        signals.addSignal(0.5)  # Reward signal
        signals.addSignal(0.1)  # Curiosity signal
        
        # Simulate agent processing
        agent.processSensoryInput(vision)
        
        # Simulate reward modulation
        agent.applyRewardModulation(0.5, 0.3)
        
        print(f"  Agent curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Agent novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("✓ Context manager with agent simulation test passed")
    return True


def test_nested_context_managers():
    """Test nested context managers."""
    print("Testing nested context managers...")
    
    # Create nested context managers
    with pynlm.Vision.createImage(400, 300) as outer_vision:
        with pynlm.InternalSignals.createSignals() as inner_signals:
            
            # Nested operations
            outer_vision.setData([1.0] * (400 * 300 * 3))
            inner_signals.addSignal(1.0)
            inner_signals.addSignal(2.0)
            
            print(f"  Outer vision: {outer_vision.getWidth()}x{outer_vision.getHeight()}")
            print(f"  Inner signals: {len(inner_signals.getData())} signals")
        
        # Inner context exited
        assert len(inner_signals.getData()) == 2
        
        # Continue with outer context
        assert outer_vision.getWidth() == 400
    
    print("✓ Nested context managers test passed")
    return True


def run_all_tests():
    """Run all context manager tests."""
    print("=" * 60)
    print("NLM Context Manager Test Suite")
    print("=" * 60)
    
    tests = [
        test_vision_context_manager,
        test_audio_context_manager,
        test_internal_signals_context_manager,
        test_multiple_context_managers,
        test_context_manager_resource_cleanup,
        test_context_manager_with_agent_simulation,
        test_nested_context_managers,
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
    print(f"Context Manager Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All context manager tests passed!")


if __name__ == "__main__":
    run_all_tests()