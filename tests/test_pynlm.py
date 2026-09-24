#!/usr/bin/env python3
"""
Python test suite for NLM Python bindings.

Tests the enhanced Python API for NLM (Neural Learning Machine) with focus on:
- Pythonic interface
- Error handling and validation
- Convenience methods
- Context managers
- Factory functions
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Please build the bindings first:")
    print("cd /path/to/nlm && mkdir -p build && cd build && cmake .. && make")
    sys.exit(1)

def test_imports():
    """Test basic imports and module metadata."""
    print("Testing imports and module metadata...")
    
    # Check module metadata
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    assert hasattr(pynlm, '__author__'), "Missing __author__ attribute"
    assert hasattr(pynlm, '__license__'), "Missing __license__ attribute"
    
    print(f"  Version: {pynlm.__version__}")
    print(f"  Author: {pynlm.__author__}")
    print(f"  License: {pynlm.__license__}")
    print("  ✓ Module metadata OK")

def test_enums():
    """Test enumerations."""
    print("\nTesting enumerations...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
    assert pynlm.NeuronType.Inhibitory == pynlm.NeuronType.Inhibitory
    assert pynlm.NeuronType.Sensory == pynlm.NeuronType.Sensory
    assert pynlm.NeuronType.Motor == pynlm.NeuronType.Motor
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward == pynlm.ActionType.MoveForward
    assert pynlm.ActionType.TurnLeft == pynlm.ActionType.TurnLeft
    assert pynlm.ActionType.Interact == pynlm.ActionType.Interact
    
    print("  ✓ Enumerations OK")

def test_ids():
    """Test ID types."""
    print("\nTesting ID types...")
    
    # Test NeuronId
    neuron_id1 = pynlm.NeuronId(42)
    neuron_id2 = pynlm.NeuronId(42)
    neuron_id3 = pynlm.NeuronId(100)
    
    assert neuron_id1.value == 42
    assert neuron_id1 == neuron_id2
    assert neuron_id1 != neuron_id3
    assert hash(neuron_id1) == hash(neuron_id2)
    
    # Test string representation
    assert "NeuronId: 42" in repr(neuron_id1)
    assert str(neuron_id1) == "42"
    
    print("  ✓ NeuronId OK")
    
    # Test SynapseId
    syn_id = pynlm.SynapseId(123)
    assert syn_id.value == 123
    print("  ✓ SynapseId OK")
    
    # Test RegionId
    region_id = pynlm.RegionId(999)
    assert region_id.value == 999
    print("  ✓ RegionId OK")

def test_config():
    """Test Config class."""
    print("\nTesting Config class...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test Pythonic set/get methods
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count") == 1000
    
    # Test get with default
    assert config.get("nonexistent_key", "default") == "default"
    
    # Test brain helper methods
    brain_config = config.brain()
    assert "neuron_count" in brain_config
    assert brain_config["neuron_count"] == 1000
    
    # Test plasticity helper
    config.set("plasticity.stdp.learning_rate", 0.001)
    plasticity_config = config.plasticity()
    assert "stdp.learning_rate" in plasticity_config
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.save_to_file(f.name)
        
        new_config = pynlm.createDefaultConfig()
        new_config.load_from_file(f.name)
        
        assert new_config.has("brain.neuron_count")
        os.unlink(f.name)
    
    print("  ✓ Config class OK")

def test_vision():
    """Test Vision class."""
    print("\nTesting Vision class...")
    
    # Test creation
    vision = pynlm.Vision(640, 480, 3)
    assert vision.width == 640
    assert vision.height == 480
    assert vision.channels == 3
    
    # Test property access
    assert hasattr(vision, 'width')
    assert hasattr(vision, 'height')
    assert hasattr(vision, 'channels')
    
    # Test data operations
    data = [0.0] * 640 * 480 * 3
    vision.set_data(data)
    retrieved_data = vision.get_data()
    assert len(retrieved_data) == len(data)
    
    # Test pixel access
    assert vision.get_pixel(0, 0, 0) == 0.0
    
    # Test clone
    vision2 = vision.clone()
    assert vision2.width == vision.width
    assert vision2.height == vision.height
    
    # Test length
    assert len(vision) == 640 * 480 * 3
    
    print("  ✓ Vision class OK")

def test_audio():
    """Test Audio class."""
    print("\nTesting Audio class...")
    
    # Test creation
    audio = pynlm.Audio(sample_rate=44100, num_samples=1024)
    assert audio.sample_rate == 44100
    assert audio.num_samples == 1024
    assert audio.duration > 0.0
    
    # Test duration calculation
    assert abs(audio.duration - 1024.0/44100.0) < 0.001
    
    # Test data operations
    data = [0.5] * 1024
    audio.set_data(data)
    retrieved = audio.get_data()
    assert len(retrieved) == len(data)
    
    # Test length
    assert len(audio) == 1024
    
    print("  ✓ Audio class OK")

def test_action():
    """Test Action class."""
    print("\nTesting Action class...")
    
    # Test creation
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.type == pynlm.ActionType.MoveForward
    assert action1.get_name() == "MoveForward"
    
    action2 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
    assert action2.type == pynlm.ActionType.MoveForward
    assert action2.get_parameters() == [1.0, 2.0]
    
    # Test type checking
    assert action1.is_same_type(pynlm.ActionType.MoveForward)
    assert not action1.is_same_type(pynlm.ActionType.TurnLeft)
    
    # Test equality
    action3 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
    assert action2 == action3
    
    # Test with parameters
    action4 = action2.with_parameters([3.0, 4.0])
    assert action4.get_parameters() == [3.0, 4.0]
    
    # Test representation
    repr(action1)
    
    print("  ✓ Action class OK")

def test_brain():
    """Test Brain class."""
    print("\nTesting Brain class...")
    
    # Create brain with quick factory
    config = pynlm.createDefaultConfig()
    config.set_brain("neuron_count", 100)
    brain = pynlm.createBrain(config)
    
    assert brain is not None
    assert brain.neuron_count == 100
    
    # Initialize brain
    brain.initialize()
    
    # Test basic operations
    brain.step(0)
    brain.step(1)
    
    # Test convenience properties
    assert hasattr(brain, 'firing_neurons')
    assert hasattr(brain, 'active_neurons')
    assert hasattr(brain, 'spike_count')
    assert hasattr(brain, 'avg_firing_rate')
    
    # Test statistics
    stats = {
        'firing': brain.firing_neurons(),
        'active': brain.active_neurons(),
        'spikes': brain.spike_count(),
        'rate': brain.avg_firing_rate(),
        'regions': brain.region_count(),
        'neurons': brain.neuron_count(),
        'synapses': brain.synapse_count()
    }
    
    # Test context manager
    with brain:
        assert brain is not None
    
    print("  ✓ Brain class OK")

def test_world():
    """Test SimpleWorld class."""
    print("\nTesting SimpleWorld class...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Configure world
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Test agent position
    world.set_agent_start(10.0, 10.0)
    
    # Test agent properties
    body = world.get_agent_body()
    assert hasattr(body, 'x')
    assert hasattr(body, 'y')
    assert hasattr(body, 'orientation')
    
    # Test position helpers
    pos = world.get_agent_position()
    assert len(pos) == 2
    
    # Test world updates
    world.update(0.1)
    
    print("  ✓ SimpleWorld class OK")

def test_agent_brain():
    """Test AgentBrain class."""
    print("\nTesting AgentBrain class...")
    
    # Create agent with quick factory
    brain, world, agent = pynlm.quick_agent(neuron_count=50, world_width=10, world_height=10)
    
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    # Initialize agent
    agent.initialize(world)
    
    # Test enable methods
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Test enable checks
    assert agent.is_reward_modulation_enabled()
    assert agent.is_curiosity_enabled()
    assert agent.is_development_enabled()
    
    # Test statistics
    stats = agent.get_stats()
    assert "curiosity" in stats
    assert "novelty" in stats
    assert "prediction_error" in stats
    
    # Test episode running
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(0)
    world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
    
    print("  ✓ AgentBrain class OK")

def test_context_managers():
    """Test context manager support."""
    print("\nTesting context managers...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test brain context manager
    with brain:
        assert brain is not None
    
    # Test with statement
    config = pynlm.createDefaultConfig()
    with config:
        assert config is not None
    
    print("  ✓ Context managers OK")

def test_error_handling():
    """Test error handling and validation."""
    print("\nTesting error handling...")
    
    # Test ValidationError
    try:
        # Create vision with invalid data
        vision = pynlm.Vision(10, 10, 3)
        wrong_data = [1.0] * 50  # Wrong size
        vision.set_data(wrong_data)
        assert False, "Should have raised ValidationError"
    except Exception as e:
        assert "ValidationError" in str(type(e).__name__)
    
    # Test config operations
    config = pynlm.createDefaultConfig()
    try:
        # Test invalid access pattern (not actually tested, just checking structure)
        pass
    except:
        pass
    
    print("  ✓ Error handling OK")

def test_factory_functions():
    """Test factory functions."""
    print("\nTesting factory functions...")
    
    # Test quick_brain
    brain = pynlm.quick_brain(neuron_count=200)
    assert brain is not None
    assert brain.neuron_count == 200
    
    # Test quick_agent
    brain, world, agent = pynlm.quick_agent()
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    print("  ✓ Factory functions OK")

def test_batch_operations():
    """Test batch operations."""
    print("\nTesting batch operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test run_steps
    brain.run_steps(start_step=0, num_steps=10)
    
    # Test run_simulation
    brain.run_simulation(num_steps=5, timestep=0.1)
    
    print("  ✓ Batch operations OK")

def test_global_constants():
    """Test global constants."""
    print("\nTesting global constants...")
    
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    
    print("  ✓ Global constants OK")

def test_representation():
    """Test string representations."""
    print("\nTesting string representations...")
    
    # Test various objects
    neuron_id = pynlm.NeuronId(42)
    repr(neuron_id)
    str(neuron_id)
    
    config = pynlm.createDefaultConfig()
    repr(config)
    
    vision = pynlm.Vision(10, 10, 3)
    repr(vision)
    
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    repr(action)
    
    print("  ✓ String representations OK")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_imports,
        test_enums,
        test_ids,
        test_config,
        test_vision,
        test_audio,
        test_action,
        test_brain,
        test_world,
        test_agent_brain,
        test_context_managers,
        test_error_handling,
        test_factory_functions,
        test_batch_operations,
        test_global_constants,
        test_representation
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ FAILED: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
