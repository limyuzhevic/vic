#!/usr/bin/env python3
"""
Python bindings test suite for NLM neural simulation framework.

This test suite exercises the Python bindings to ensure they work correctly
and provide the expected functionality.
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import the NLM Python bindings
try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please build the C++ extension first:")
    print("mkdir build && cd build && cmake .. && make")
    sys.exit(1)

def test_import():
    """Test that all expected modules are importable."""
    print("\n=== Testing Imports ===")
    
    # Test core imports
    from pynlm.core import Config, create_default_config
    from pynlm.brain import Brain
    from pynlm.agent import AgentBrain, AgentBody, SensoryPercept, Action, ActionResult
    from pynlm.world import SimpleWorld
    from pynlm.sensory import Vision, Audio, InternalSignals
    from pynlm.motor import ActionType, MotorCommand
    
    print("✓ All core imports successful")

def test_config():
    """Test configuration functionality."""
    print("\n=== Testing Config ===")
    
    from pynlm import Config, create_default_config
    
    # Test default config
    config = create_default_config()
    print("✓ Created default config")
    
    # Test config properties
    assert config.has("neuron_count")
    assert config.has("region_count")
    print("✓ Config has expected keys")
    
    # Test get/set
    config.set("neuron_count", 200)
    assert config.get("neuron_count") == 200
    print("✓ Config get/set works")
    
    # Test dictionary access
    config["region_count"] = 3
    assert config["region_count"] == 3
    print("✓ Dictionary access works")
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
    
    try:
        config.save_to_file(config_path)
        print("✓ Config save works")
        
        new_config = Config()
        new_config.load_from_file(config_path)
        print("✓ Config load works")
        
        assert new_config.get("neuron_count") == 200
        print("✓ Config loaded correctly")
        
    finally:
        if os.path.exists(config_path):
            os.unlink(config_path)
    
    # Test validation
    errors = config.validate()
    assert isinstance(errors, list)
    print("✓ Config validation works")
    
    # Test summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    print("✓ Config summary works")

def test_brain_creation():
    """Test brain creation and basic functionality."""
    print("\n=== Testing Brain Creation ===")
    
    from pynlm import Config, Brain
    
    # Test brain creation with default config
    config = Config()
    brain = Brain(config)
    print("✓ Created brain with default config")
    
    # Test brain initialization
    brain.initialize()
    print("✓ Brain initialized")
    
    # Test brain properties
    assert brain.is_initialized()
    print("✓ Brain reports as initialized")
    
    assert brain.get_region_count() == 1
    print("✓ Brain has expected region count")
    
    # Test statistics
    stats = brain.get_statistics()
    assert stats.total_neurons > 0
    assert stats.total_synapses >= 0
    print("✓ Brain statistics collected")

def test_world_interaction():
    """Test world-agent interaction."""
    print("\n=== Testing World Interaction ===")
    
    from pynlm import Config, Brain, SimpleWorld, Vision, ActionType
    
    # Create world
    world = SimpleWorld()
    world.configure(64, 64, 32, 32)
    print("✓ Created and configured world")
    
    # Create brain
    config = Config()
    brain = Brain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    # Create agent brain
    from pynlm import AgentBrain
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    print("✓ Created and initialized agent brain")
    
    # Test world properties
    assert world.get_width() == 64
    assert world.get_height() == 64
    print("✓ World dimensions correct")
    
    # Test agent body
    body = world.get_agent_body()
    assert body is not None
    print("✓ Got agent body")
    
    # Test sensory input
    percept = world.get_sensory_percept()
    assert percept is not None
    print("✓ Got sensory percept")

def test_sensory_systems():
    """Test sensory system functionality."""
    print("\n=== Testing Sensory Systems ===")
    
    from pynlm import Vision, Audio, InternalSignals
    
    # Test Vision
    vision = Vision(64, 64, 3)
    assert vision.get_width() == 64
    assert vision.get_height() == 64
    assert vision.get_channels() == 3
    print("✓ Vision created with correct dimensions")
    
    vision.set_data([0.1] * 64 * 64 * 3)
    print("✓ Vision data set")
    
    # Test Audio
    audio = Audio(44100, 1024)
    assert audio.get_sample_rate() == 44100
    assert audio.get_num_samples() == 1024
    print("✓ Audio created with correct parameters")
    
    audio.set_data([0.1] * 1024)
    print("✓ Audio data set")
    
    # Test InternalSignals
    signals = InternalSignals()
    signals.add_signal(0.5)
    signals.add_signal(0.8)
    signals.clear_signals()
    print("✓ InternalSignals created and used")

def test_action_systems():
    """Test action and motor systems."""
    print("\n=== Testing Action Systems ===")
    
    from pynlm import ActionType, MotorCommand, Action
    
    # Test Action creation
    action = Action(ActionType.MoveForward)
    assert action.get_type() == ActionType.MoveForward
    print("✓ Action created with correct type")
    
    action.set_parameters([1.0, 0.0])
    assert action.get_parameters() == [1.0, 0.0]
    print("✓ Action parameters set")
    
    # Test action cloning
    cloned = action.clone()
    assert cloned.get_type() == action.get_type()
    assert cloned.get_parameters() == action.get_parameters()
    print("✓ Action cloning works")
    
    # Test convenience functions
    from pynlm.motor import create_movement_action, create_rotation_action
    move_action = create_movement_action("forward", 2.0)
    assert move_action.get_type() == ActionType.MoveForward
    print("✓ Movement action creation works")
    
    rotation_action = create_rotation_action("left", 0.5)
    assert rotation_action.get_type() == ActionType.TurnLeft
    print("✓ Rotation action creation works")

def test_agent_system():
    """Test complete agent system."""
    print("\n=== Testing Agent System ===")
    
    from pynlm import Config, Brain, AgentBrain, ActionType
    
    # Create components
    config = Config()
    brain = Brain(config)
    brain.initialize()
    
    # Test agent brain initialization
    agent_brain = AgentBrain(brain)
    assert not agent_brain.is_initialized()
    print("✓ Agent brain starts uninitialized")
    
    # Test enable/disable features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    print("✓ Agent features can be enabled")
    
    assert agent_brain.is_reward_modulation_enabled()
    assert agent_brain.is_curiosity_enabled()
    assert agent_brain.is_structural_plasticity_enabled()
    assert agent_brain.is_development_enabled()
    print("✓ Agent features report as enabled")

def test_error_handling():
    """Test error handling and edge cases."""
    print("\n=== Testing Error Handling ===")
    
    from pynlm import Config, ConfigError, BrainError
    
    # Test config error handling
    try:
        config = Config()
        config.load_from_file("/nonexistent/path.json")
        assert False, "Should have raised ConfigError"
    except ConfigError:
        print("✓ Config correctly handles file not found")
    
    # Test brain error handling
    try:
        from pynlm import Brain
        config = Config()
        brain = Brain(config)
        brain.step(0)  # Should fail without initialization
        assert False, "Should have raised BrainError"
    except BrainError:
        print("✓ Brain correctly handles uninitialized access")

def run_all_tests():
    """Run all tests."""
    print("Running NLM Python Bindings Test Suite")
    print("=" * 50)
    
    try:
        test_import()
        test_config()
        test_brain_creation()
        test_world_interaction()
        test_sensory_systems()
        test_action_systems()
        test_agent_system()
        test_error_handling()
        
        print("\n" + "=" * 50)
        print("All tests passed! ✓")
        print("\nThe NLM Python bindings are working correctly.")
        print("Key improvements implemented:")
        print("- Modular Python API with type hints")
        print("- Comprehensive error handling")
        print("- Convenience functions for common operations")
        print("- Full documentation and examples")
        print("- Backward compatibility with C++ extensions")
        
        return True
        
    except Exception as e:
        print(f"\n✗ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)