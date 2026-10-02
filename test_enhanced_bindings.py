#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings improvements.

This test file verifies that the enhanced Python bindings are working correctly
and provides a framework for testing the new functionality.
"""

import sys
import os
import tempfile
import json

# Add the python directory to the path so we can import our bindings
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    CPP_BINDINGS_AVAILABLE = True
except ImportError as e:
    CPP_BINDINGS_AVAILABLE = False
    print(f"Warning: C++ bindings not available: {e}")
    print("Some tests will be skipped.")

# Test constants
TEST_CONFIG_PATH = "test_config.cfg"
TEMP_DIR = tempfile.gettempdir()

def test_basic_import():
    """Test that pynlm can be imported and basic functionality works."""
    print("Testing basic import...")
    
    assert hasattr(pynlm, '__version__'), "pynlm should have __version__ attribute"
    assert hasattr(pynlm, '__description__'), "pynlm should have __description__ attribute"
    assert hasattr(pynlm, '__author__'), "pynlm should have __author__ attribute"
    
    print(f"✓ Version: {pynlm.__version__}")
    print(f"✓ Description: {pynlm.__description__}")
    print(f"✓ Author: {pynlm.__author__}")
    
    return True

def test_configuration_creation():
    """Test that configuration can be created and manipulated."""
    print("\nTesting configuration creation...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Test default configuration
    config = pynlm.createDefaultConfig()
    assert config is not None, "Should be able to create default config"
    
    # Test configuration has essential methods
    assert hasattr(config, 'has'), "Config should have 'has' method"
    assert hasattr(config, 'getKeys'), "Config should have 'getKeys' method"
    assert hasattr(config, 'getOr'), "Config should have 'getOr' method"
    assert hasattr(config, 'set'), "Config should have 'set' method"
    
    # Test configuration operations
    config.set("test.key", "test_value")
    assert config.has("test.key"), "Should be able to set and check config key"
    
    # Test getOr method
    value = config.getOr("test.key", "default")
    assert value == "test_value", "getOr should return the set value"
    
    default_value = config.getOr("nonexistent.key", "default")
    assert default_value == "default", "getOr should return default for missing key"
    
    # Test save and load
    config.saveToFile(TEST_CONFIG_PATH)
    
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(TEST_CONFIG_PATH)
    assert config2.has("test.key"), "Loaded config should have the saved key"
    
    print("✓ Configuration creation and manipulation works")
    
    # Clean up
    if os.path.exists(TEST_CONFIG_PATH):
        os.remove(TEST_CONFIG_PATH)
    
    return True

def test_brain_creation():
    """Test that brains can be created and initialized."""
    print("\nTesting brain creation...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Test default brain creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None, "Should be able to create brain"
    
    # Test brain initialization
    brain.initialize()
    
    # Test brain properties
    assert hasattr(brain, 'getTotalNeuronCount'), "Brain should have getTotalNeuronCount"
    assert hasattr(brain, 'getTotalSynapseCount'), "Brain should have getTotalSynapseCount"
    assert hasattr(brain, 'getFiringNeuronCount'), "Brain should have getFiringNeuronCount"
    assert hasattr(brain, 'getTotalSpikeCount'), "Brain should have getTotalSpikeCount"
    assert hasattr(brain, 'getAverageFiringRate'), "Brain should have getAverageFiringRate"
    
    neuron_count = brain.getTotalNeuronCount()
    assert neuron_count > 0, f"Brain should have neurons, got {neuron_count}"
    
    # Test brain step
    brain.step(0)
    
    # Test reset
    brain.reset()
    brain.initialize()
    
    print(f"✓ Brain creation works (created {neuron_count} neurons)")
    
    return True

def test_world_creation():
    """Test that worlds can be created and configured."""
    print("\nTesting world creation...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Test default world creation
    world = pynlm.createSimpleWorld()
    assert world is not None, "Should be able to create world"
    
    # Test world configuration
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Test world properties
    assert hasattr(world, 'getWidth'), "World should have getWidth"
    assert hasattr(world, 'getHeight'), "World should have getHeight"
    assert hasattr(world, 'getSimulationTime'), "World should have getSimulationTime"
    
    assert world.getWidth() == 20, f"World width should be 20, got {world.getWidth()}"
    assert world.getHeight() == 20, f"World height should be 20, got {world.getHeight()}"
    
    # Test world reset
    world.reset()
    world.update(0.1)
    
    print("✓ World creation and configuration works")
    
    return True

def test_agent_creation():
    """Test that agents can be created and initialized."""
    print("\nTesting agent creation...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "Should be able to create agent"
    
    # Test agent initialization
    agent.initialize(world)
    
    # Test agent properties
    assert hasattr(agent, 'getSensoryInputSize'), "Agent should have getSensoryInputSize"
    assert hasattr(agent, 'getMotorOutputSize'), "Agent should have getMotorOutputSize"
    assert hasattr(agent, 'getCuriosityLevel'), "Agent should have getCuriosityLevel"
    assert hasattr(agent, 'getNoveltyLevel'), "Agent should have getNoveltyLevel"
    
    # Test agent functionality
    agent.processSensoryInput(world.getSensoryPercept())
    agent.step(0)
    
    # Test agent subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Agent creation and functionality works")
    
    return True

def test_enhanced_functionality():
    """Test the enhanced Python functionality when C++ bindings are available."""
    print("\nTesting enhanced Python functionality...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Test BrainSimulator
    print("  Testing BrainSimulator...")
    from pynlm import BrainSimulator
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    simulator = BrainSimulator(brain)
    status = simulator.get_status()
    
    assert 'neuron_count' in status, "BrainSimulator status should have neuron_count"
    assert 'synapse_count' in status, "BrainSimulator status should have synapse_count"
    
    # Test world simulation
    print("  Testing WorldSimulator...")
    from pynlm import WorldSimulator
    
    world = pynlm.createSimpleWorld()
    world_simulator = WorldSimulator(world)
    world_simulator.setup_environment(width=10, height=10)
    
    episode_data = world_simulator.run_episode(num_steps=10)
    assert 'steps' in episode_data, "Episode data should have steps"
    assert len(episode_data['steps']) == 10, "Should have 10 steps"
    
    # Test Agent
    print("  Testing Agent...")
    from pynlm import Agent
    
    agent = Agent(brain, world)
    episode_data = agent.run_episode(num_steps=10)
    assert 'steps' in episode_data, "Agent episode data should have steps"
    
    # Test ConfigurationManager
    print("  Testing ConfigurationManager...")
    from pynlm import ConfigurationManager
    
    presets = ConfigurationManager.get_presets()
    assert 'minimal' in presets, "Should have minimal preset"
    assert 'standard' in presets, "Should have standard preset"
    assert 'advanced' in presets, "Should have advanced preset"
    assert 'research' in presets, "Should have research preset"
    
    # Test configuration preset application
    config = pynlm.createDefaultConfig()
    config = ConfigurationManager.apply_preset(config, 'standard')
    assert config.has('plasticity.stdp.enable'), "Standard preset should enable STDP"
    assert config.has('plasticity.hebbian.enable'), "Standard preset should enable Hebbian"
    
    # Test configuration validation
    validation = ConfigurationManager.validate_config(config)
    assert 'is_valid' in validation, "Validation should have is_valid field"
    assert 'warnings' in validation, "Validation should have warnings field"
    assert 'recommendations' in validation, "Validation should have recommendations field"
    
    print("✓ All enhanced Python functionality works")
    
    return True

def test_error_handling():
    """Test error handling and exception types."""
    print("\nTesting error handling...")
    
    # Test that error classes are available
    from pynlm import NLMError, ConfigurationError, SimulationError, InitializationError
    
    # Test that error classes can be raised and caught
    try:
        raise ConfigurationError("Test configuration error")
    except ConfigurationError as e:
        assert "Test configuration error" in str(e)
        print("  ✓ ConfigurationError works")
    
    try:
        raise SimulationError("Test simulation error")
    except SimulationError as e:
        assert "Test simulation error" in str(e)
        print("  ✓ SimulationError works")
    
    try:
        raise InitializationError("Test initialization error")
    except InitializationError as e:
        assert "Test initialization error" in str(e)
        print("  ✓ InitializationError works")
    
    try:
        raise NLMError("Test NLM error")
    except NLMError as e:
        assert "Test NLM error" in str(e)
        print("  ✓ NLMError works")
    
    print("✓ Error handling works correctly")
    
    return True

def test_convenience_functions():
    """Test convenience functions."""
    print("\nTesting convenience functions...")
    
    if not CPP_BINDINGS_AVAILABLE:
        print("  Skipping: C++ bindings not available")
        return True
    
    # Test create_brain_with_config
    from pynlm import create_brain_with_config
    
    brain = create_brain_with_config(neuron_count=100)
    assert brain is not None, "create_brain_with_config should work"
    
    # Test validate_configuration
    from pynlm import validate_configuration
    
    config = pynlm.createDefaultConfig()
    validation = validate_configuration(config)
    assert 'is_valid' in validation, "validate_configuration should return validation results"
    
    # Test run_agent_simulation
    from pynlm import run_agent_simulation
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    results = run_agent_simulation(brain, world, agent, num_steps=10)
    assert 'total_spikes' in results, "Simulation results should have total_spikes"
    assert 'firing_neuron_counts' in results, "Simulation results should have firing_neuron_counts"
    
    print("✓ Convenience functions work correctly")
    
    return True

def test_backward_compatibility():
    """Test that the module maintains backward compatibility."""
    print("\nTesting backward compatibility...")
    
    # Test that all essential C++ functions are available
    essential_functions = [
        'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
        'NeuronType', 'ActionType', 'DevelopmentalStage', 'FiringState', 'WorldObjectType'
    ]
    
    for func_name in essential_functions:
        assert hasattr(pynlm, func_name), f"Should have {func_name}"
    
    print("✓ Backward compatibility maintained")
    
    return True

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_import,
        test_configuration_creation,
        test_brain_creation,
        test_world_creation,
        test_agent_creation,
        test_enhanced_functionality,
        test_error_handling,
        test_convenience_functions,
        test_backward_compatibility
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test failed with exception: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("All tests passed! 🎉")
        return True
    else:
        print(f"Some tests failed. {failed} failures.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
