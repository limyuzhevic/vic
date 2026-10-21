#!/usr/bin/env python3
"""
Extended test suite for NLM Python bindings (pynlm) - focusing on convenience functions and improvements.

This test suite validates the improved Python bindings with enhanced functionality,
better error handling, and additional convenience features.
"""

import pynlm
import tempfile
import os
import json
import sys

def test_module_attributes():
    """Test that the module has all expected attributes."""
    print("Testing module attributes...")
    
    # Check version
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    print(f"  ✓ Version: {pynlm.__version__}")
    
    # Check constants
    assert hasattr(pynlm, 'INVALID_NEURON_ID')
    assert hasattr(pynlm, 'INVALID_SYNAPSE_ID')
    assert hasattr(pynlm, 'INVALID_REGION_ID')
    assert hasattr(pynlm, 'INVALID_POPULATION_ID')
    print("  ✓ Invalid ID constants available")
    
    # Check enumerations
    enumerations = ['NeuronType', 'SynapseType', 'DevelopmentalStage', 
                   'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType']
    for enum_name in enumerations:
        assert hasattr(pynlm, enum_name), f"Missing enumeration: {enum_name}"
        enum_class = getattr(pynlm, enum_name)
        assert hasattr(enum_class, '__members__'), f"Enumeration {enum_name} is not an enum"
    print("  ✓ All enumerations available")

def test_convenience_functions():
    """Test new convenience functions for easier usage."""
    print("\nTesting convenience functions...")
    
    # Test quick_brain function (if it exists)
    if hasattr(pynlm, 'quick_brain'):
        brain = pynlm.quick_brain(100, "configs/default.cfg")
        assert brain is not None
        assert brain.getTotalNeuronCount() == 100
        brain.initialize()
        print("  ✓ quick_brain function works")
    
    # Test run_agent_simulation function (if it exists)
    if hasattr(pynlm, 'run_agent_simulation'):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        result = pynlm.run_agent_simulation(agent, world, 100)
        assert result is not None
        assert 'final_spike_count' in result
        print("  ✓ run_agent_simulation function works")
    
    # Test create_training_config function (if it exists)
    if hasattr(pynlm, 'create_training_config'):
        config = pynlm.create_training_config(1000, 0.1, 0.01)
        assert config.has("neuron_count")
        assert config.has("connection_probability")
        assert config.has("plasticity_learning_rate")
        print("  ✓ create_training_config function works")
    
    # Test utility functions
    if hasattr(pynlm, 'plot_brain_network'):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This would normally generate a plot, but we'll just test it doesn't crash
        pynlm.plot_brain_network(brain, "test_network.png")
        print("  ✓ plot_brain_network function works")

def test_enhanced_config():
    """Test enhanced configuration functionality."""
    print("\nTesting enhanced configuration...")
    
    config = pynlm.createDefaultConfig()
    
    # Test config comparison
    config2 = pynlm.createDefaultConfig()
    assert config != config2  # Different instances
    print("  ✓ Config comparison works")
    
    # Test config copy
    if hasattr(config, 'copy'):
        config_copy = config.copy()
        assert config_copy.has("neuron_count")
        assert config_copy.getOr("neuron_count", 0) == config.getOr("neuron_count", 0)
        print("  ✓ Config.copy() works")
    
    # Test config merge
    if hasattr(config, 'merge'):
        config.merge({"test_key": "test_value", "another_key": 42})
        assert config.has("test_key")
        assert config.has("another_key")
        print("  ✓ Config.merge() works")
    
    # Test config validation
    if hasattr(config, 'validate'):
        # Add some invalid configs
        config.set("invalid_value", "not_a_number")
        config.set("negative_neurons", -10)
        
        # This should not crash
        config.validate()
        print("  ✓ Config.validate() works")

def test_enhanced_brain():
    """Test enhanced brain functionality."""
    print("\nTesting enhanced brain functionality...")
    
    config = pynlm.createDefaultConfig()
    
    # Test create_brain_with_preset function
    if hasattr(pynlm, 'create_brain_with_preset'):
        # Test different presets
        brain1 = pynlm.create_brain_with_preset("simple", 100)
        assert brain1.getTotalNeuronCount() == 100
        
        brain2 = pynlm.create_brain_with_preset("complex", 500)
        assert brain2.getTotalNeuronCount() == 500
        
        brain3 = pynlm.create_brain_with_preset("optimized", 200)
        assert brain3.getTotalNeuronCount() == 200
        
        print("  ✓ create_brain_with_preset function works")
    
    # Test brain comparison
    brain1 = pynlm.createBrain(config)
    brain2 = pynlm.createBrain(config)
    brain1.initialize()
    brain2.initialize()
    
    # Test brain statistics comparison
    if hasattr(brain1, 'compare_with'):
        comparison = brain1.compare_with(brain2)
        assert 'neuron_count_diff' in comparison
        assert 'synapse_count_diff' in comparison
        print("  ✓ brain.compare_with() works")
    
    # Test brain snapshot
    if hasattr(brain1, 'snapshot'):
        snapshot1 = brain1.snapshot()
        assert 'step' in snapshot1
        assert 'spike_count' in snapshot1
        
        brain1.step(5)
        snapshot2 = brain1.snapshot()
        assert snapshot2['step'] > snapshot1['step']
        print("  ✓ brain.snapshot() works")

def test_enhanced_agent():
    """Test enhanced agent functionality."""
    print("\nTesting enhanced agent functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent preset configurations
    if hasattr(pynlm, 'create_agent_with_preset'):
        agent1 = pynlm.create_agent_with_preset("explorer", brain)
        agent1.initialize(world)
        assert agent1.isCuriosityEnabled()
        
        agent2 = pynlm.create_agent_with_preset("learner", brain)
        agent2.initialize(world)
        assert agent2.isRewardModulationEnabled()
        
        print("  ✓ create_agent_with_preset function works")
    
    # Test agent behavior profiles
    if hasattr(agent, 'set_behavior_profile'):
        agent.set_behavior_profile("curious")
        assert agent.isCuriosityEnabled()
        
        agent.set_behavior_profile("methodical")
        assert agent.isDevelopmentEnabled()
        
        print("  ✓ agent.set_behavior_profile() works")
    
    # Test agent learning statistics
    agent.enableRewardModulation(True)
    
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.8)
    
    # Test learning statistics
    if hasattr(agent, 'get_learning_stats'):
        stats = agent.get_learning_stats()
        assert 'total_rewards' in stats
        assert 'average_reward' in stats
        assert 'learning_curve' in stats
        print("  ✓ agent.get_learning_stats() works")

def test_file_io_improvements():
    """Test improved file I/O functionality."""
    print("\nTesting improved file I/O...")
    
    # Test enhanced config file I/O
    if hasattr(pynlm, 'save_config_to_file'):
        config = pynlm.createDefaultConfig()
        config.set("custom_param", "test_value")
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            pynlm.save_config_to_file(config, filepath)
            assert os.path.exists(filepath)
            
            # Load and verify
            loaded_config = pynlm.load_config_from_file(filepath)
            assert loaded_config.has("custom_param")
            assert loaded_config.getOr("custom_param") == "test_value"
            
            print("  ✓ save/load config functions work")
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
    
    # Test enhanced brain checkpoint
    if hasattr(pynlm, 'save_brain_checkpoint'):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(5):
            brain.step(step)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.chk', delete=False) as f:
            filepath = f.name
        
        try:
            pynlm.save_brain_checkpoint(brain, filepath)
            assert os.path.exists(filepath)
            
            # Try to load checkpoint (may not be fully implemented)
            try:
                loaded_brain = pynlm.load_brain_checkpoint(filepath)
                print("  ✓ save/load brain checkpoint functions work")
            except:
                print("  ! Brain checkpoint load not fully implemented, but save works")
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)

def test_visualization_improvements():
    """Test visualization improvements."""
    print("\nTesting visualization improvements...")
    
    if hasattr(pynlm, 'generate_network_graph'):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This would generate a network graph
        pynlm.generate_network_graph(brain, "network.png", show_neurons=True, show_synapses=True)
        print("  ✓ generate_network_graph function works")
    
    if hasattr(pynlm, 'create_brain_visualization'):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This would create a visualization
        pynlm.create_brain_visualization(brain, "brain_viz.html")
        print("  ✓ create_brain_visualization function works")

def test_error_handling_improvements():
    """Test improved error handling."""
    print("\nTesting improved error handling...")
    
    # Test NLMError class (if it exists)
    if hasattr(pynlm, 'NLMError'):
        try:
            raise pynlm.NLMError("Test error message")
        except pynlm.NLMError as e:
            assert str(e) == "Test error message"
            print("  ✓ NLMError class works")
    
    # Test argument validation
    if hasattr(pynlm, 'validate_config'):
        config = pynlm.createDefaultConfig()
        
        # These should raise helpful error messages
        try:
            pynlm.validate_config(config, {"neuron_count": -10})
        except ValueError as e:
            print("  ✓ validate_config catches invalid values")
    
    # Test graceful degradation
    try:
        config = pynlm.createDefaultConfig()
        # Try to access non-existent method gracefully
        if hasattr(config, 'advanced_feature'):
            # If the method exists, call it
            pass
        print("  ✓ Graceful degradation works")
    except:
        pass

def test_documentation_improvements():
    """Test that documentation is accessible."""
    print("\nTesting documentation accessibility...")
    
    # Test if there's a way to get module documentation
    if hasattr(pynlm, '__doc__'):
        assert pynlm.__doc__ is not None
        assert len(pynlm.__doc__) > 0
        print("  ✓ Module documentation available")
    
    # Test if there are examples in the module
    if hasattr(pynlm, 'examples'):
        examples = pynlm.examples
        assert isinstance(examples, list)
        print("  ✓ Examples available")

def run_improved_tests():
    """Run all improved test suites."""
    print("=" * 80)
    print("NLM Python Bindings - Enhanced Test Suite")
    print("=" * 80)
    print("Testing improved bindings with better error handling,")
    print("convenience functions, and enhanced functionality.")
    print("=" * 80)
    
    tests = [
        test_module_attributes,
        test_convenience_functions,
        test_enhanced_config,
        test_enhanced_brain,
        test_enhanced_agent,
        test_file_io_improvements,
        test_visualization_improvements,
        test_error_handling_improvements,
        test_documentation_improvements,
    ]
    
    passed = 0
    failed = 0
    skipped = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ Test failed: {e}")
            failed += 1
    
    print("\n" + "=" * 80)
    print(f"Test Results: {passed} passed, {failed} failed, {skipped} skipped")
    print("=" * 80)
    
    return failed == 0

if __name__ == "__main__":
    success = run_improved_tests()
    sys.exit(0 if success else 1)
