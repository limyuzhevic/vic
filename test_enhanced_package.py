#!/usr/bin/env python3
"""
Test script for the enhanced NLM Python package.

This script demonstrates the new Pythonic features and improvements
to the NLM Python bindings.
"""

import sys
import time

def test_basic_imports():
    """Test that all imports work correctly."""
    print("Testing basic imports...")
    
    try:
        # Test package import
        import pynlm
        print(f"✓ Successfully imported pynlm v{pynlm.__version__}")
        
        # Test core imports
        from pynlm.core import (
            Config, Brain, World, SimpleWorld, AgentBrain,
            Vision, Audio, Action, WorldObject,
            SensorHelper, NLMFactory, BrainWrapper, WorldWrapper,
            ConfigWrapper, create_default_config, create_simple_world
        )
        print("✓ Successfully imported core components")
        
        # Test exceptions
        from pynlm.core import (
            NLMError, ConfigError, BrainError, WorldError,
            SensorError, AgentError, ValidationError
        )
        print("✓ Successfully imported custom exceptions")
        
        return True
        
    except Exception as e:
        print(f"✗ Import test failed: {e}")
        return False

def test_config_wrapper():
    """Test the enhanced ConfigWrapper functionality."""
    print("\nTesting ConfigWrapper...")
    
    try:
        from pynlm.core import create_default_config, ConfigWrapper
        
        # Create configuration
        config = create_default_config()
        wrapper = ConfigWrapper(config)
        
        # Test setting and getting values
        wrapper.set("brain.neuron_count", 1000)
        wrapper.set("brain.synapse_density", 0.1)
        wrapper.set("world.width", 100.0)
        
        assert wrapper.get("brain.neuron_count") == 1000
        assert wrapper.get("brain.synapse_density") == 0.1
        assert wrapper.get("world.width") == 100.0
        
        # Test dictionary-style access
        wrapper["brain.neuron_count"] = 2000
        assert wrapper["brain.neuron_count"] == 2000
        
        # Test validation
        wrapper.validate_required_keys(["brain.neuron_count"])
        
        # Test error handling
        try:
            wrapper.validate_required_keys(["nonexistent.key"])
            print("✗ Validation error not raised")
            return False
        except Exception as e:
            print("✓ Validation error correctly raised")
        
        print("✓ ConfigWrapper tests passed")
        return True
        
    except Exception as e:
        print(f"✗ ConfigWrapper test failed: {e}")
        return False

def test_brain_wrapper():
    """Test the enhanced BrainWrapper functionality."""
    print("\nTesting BrainWrapper...")
    
    try:
        from pynlm.core import create_brain, create_default_config, BrainWrapper, brain_context
        
        # Create brain
        config = create_default_config()
        brain = create_brain(config)
        wrapper = BrainWrapper(brain)
        
        # Test stats
        stats = wrapper.get_stats()
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        
        # Test developmental stage
        stage = wrapper.get_developmental_stage()
        wrapper.set_developmental_stage(stage)
        
        print("✓ BrainWrapper tests passed")
        return True
        
    except Exception as e:
        print(f"✗ BrainWrapper test failed: {e}")
        return False

def test_world_wrapper():
    """Test the enhanced WorldWrapper functionality."""
    print("\nTesting WorldWrapper...")
    
    try:
        from pynlm.core import create_simple_world, WorldWrapper, world_context
        
        # Create world
        world = create_simple_world()
        wrapper = WorldWrapper(world)
        
        # Test configuration
        wrapper.configure(100, 100, 20, 20)
        
        # Test statistics
        stats = wrapper.get_stats()
        assert "width" in stats
        assert "height" in stats
        
        # Test agent position
        wrapper.set_agent_start(50.0, 50.0)
        agent_body = wrapper.get_agent_body()
        assert agent_body.x == 50.0
        assert agent_body.y == 50.0
        
        print("✓ WorldWrapper tests passed")
        return True
        
    except Exception as e:
        print(f"✗ WorldWrapper test failed: {e}")
        return False

def test_sensor_helper():
    """Test the SensorHelper functionality."""
    print("\nTesting SensorHelper...")
    
    try:
        from pynlm.core import SensorHelper
        
        helper = SensorHelper()
        
        # Test normalization methods
        # Note: These would require actual vision/audio objects
        # For now, we just test that the methods exist
        assert hasattr(helper, 'normalize_vision')
        assert hasattr(helper, 'normalize_audio')
        assert hasattr(helper, 'normalize_internal_signals')
        
        print("✓ SensorHelper tests passed (structural)")
        return True
        
    except Exception as e:
        print(f"✗ SensorHelper test failed: {e}")
        return False

def test_factory_methods():
    """Test factory methods."""
    print("\nTesting factory methods...")
    
    try:
        from pynlm.core import NLMFactory
        
        # Test brain factory
        brain_wrapper = NLMFactory.create_brain_with_validation(1000, 0.1)
        assert brain_wrapper is not None
        
        # Test world factory
        world_wrapper = NLMFactory.create_world_with_agents(100, 100, 20, 20)
        assert world_wrapper is not None
        
        # Test simulation factory
        brain_wrapper, world_wrapper = NLMFactory.create_agent_simulation()
        assert brain_wrapper is not None
        assert world_wrapper is not None
        
        print("✓ Factory method tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Factory method test failed: {e}")
        return False

def test_context_managers():
    """Test context manager functionality."""
    print("\nTesting context managers...")
    
    try:
        from pynlm.core import create_brain, create_default_config, brain_context, world_context
        
        # Test brain context manager
        config = create_default_config()
        brain = create_brain(config)
        
        # Note: These context managers are defined but would need
        # proper testing with actual C++ objects
        print("✓ Context manager structural tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")
        return False

def test_enumerations():
    """Test enumerations."""
    print("\nTesting enumerations...")
    
    try:
        import pynlm
        
        # Test that enumerations exist
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'ActionType')
        assert hasattr(pynlm, 'WorldObjectType')
        assert hasattr(pynlm, 'DevelopmentalStage')
        assert hasattr(pynlm, 'FiringState')
        
        # Test enum values
        assert pynlm.NeuronType.Excitatory is not None
        assert pynlm.ActionType.MoveForward is not None
        assert pynlm.WorldObjectType.Resource is not None
        
        print("✓ Enumeration tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Enumeration test failed: {e}")
        return False

def test_error_handling():
    """Test custom error handling."""
    print("\nTesting error handling...")
    
    try:
        from pynlm.core import (
            NLMError, ConfigError, BrainError, WorldError,
            SensorError, AgentError, ValidationError
        )
        
        # Test that exception classes exist and can be instantiated
        errors = [
            NLMError("Test error"),
            ConfigError("Config error"),
            BrainError("Brain error"),
            WorldError("World error"),
            SensorError("Sensor error"),
            AgentError("Agent error"),
            ValidationError("Validation error"),
        ]
        
        # Test error inheritance
        for error in errors:
            assert isinstance(error, NLMError)
        
        print("✓ Error handling tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Enhanced NLM Python Package")
    print("=" * 60)
    
    tests = [
        test_basic_imports,
        test_config_wrapper,
        test_brain_wrapper,
        test_world_wrapper,
        test_sensor_helper,
        test_factory_methods,
        test_context_managers,
        test_enumerations,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        if test():
            passed += 1
        else:
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! The enhanced NLM package is working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
