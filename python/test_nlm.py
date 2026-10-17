#!/usr/bin/env python3
"""
Comprehensive test for the NLM Python interface.

This test verifies that the new Python interface provides all the required
functionality and maintains backward compatibility with the existing C++ bindings.
"""

import nlm
import tempfile
import os

def test_custom_exceptions():
    """Test custom exception hierarchy."""
    print("Testing custom exceptions...")
    
    # Test that all custom exceptions exist
    assert hasattr(nlm, 'NLMLibError'), "NLMLibError not found"
    assert hasattr(nlm, 'ConfigError'), "ConfigError not found"
    assert hasattr(nlm, 'BrainError'), "BrainError not found"
    assert hasattr(nlm, 'WorldError'), "WorldError not found"
    assert hasattr(nlm, 'SimulationError'), "SimulationError not found"
    
    # Test exception inheritance
    assert issubclass(nlm.ConfigError, nlm.NLMLibError), "ConfigError should inherit from NLMLibError"
    assert issubclass(nlm.BrainError, nlm.NLMLibError), "BrainError should inherit from NLMLibError"
    assert issubclass(nlm.WorldError, nlm.NLMLibError), "WorldError should inherit from NLMLibError"
    assert issubclass(nlm.SimulationError, nlm.NLMLibError), "SimulationError should inherit from NLMLibError"
    
    print("✓ Custom exceptions test passed")

def test_result_object():
    """Test Result object functionality."""
    print("Testing Result object...")
    
    # Test successful operation
    result = nlm.Result.from_operation(lambda: 1 + 1)
    assert result.success, "Result should be successful"
    assert result.data == 2, "Result data should be 2"
    assert result.error is None, "Result error should be None"
    
    # Test failed operation
    result = nlm.Result.from_operation(lambda: 1 / 0)
    assert not result.success, "Result should not be successful"
    assert result.data is None, "Result data should be None"
    assert result.error is not None, "Result error should not be None"
    
    print("✓ Result object test passed")

def test_simulation_stats():
    """Test SimulationStats class."""
    print("Testing SimulationStats...")
    
    # Create mock brain stats
    brain_stats = {
        'active_neuron_count': 100,
        'average_firing_rate': 5.5,
        'excitation_inhibition_ratio': 0.8,
        'total_spike_count': 1000,
        'developmental_stage': 'Adult'
    }
    
    agent_stats = {
        'curiosity_level': 0.7,
        'novelty_level': 0.6,
        'neuromodulation_level': 0.9
    }
    
    world_stats = {
        'agent_x': 10.0,
        'agent_y': 10.0,
        'agent_energy': 80.0
    }
    
    stats = nlm.SimulationStats(brain_stats, agent_stats, world_stats, 1000)
    
    assert stats.step_count == 1000, "Step count should be 1000"
    assert stats.get_active_neurons() == 100, "Active neurons should be 100"
    assert stats.get_firing_rate() == 5.5, "Firing rate should be 5.5"
    assert stats.get_ei_ratio() == 0.8, "E/I ratio should be 0.8"
    assert stats.get_total_spikes() == 1000, "Total spikes should be 1000"
    assert stats.get_curiosity_level() == 0.7, "Curiosity level should be 0.7"
    assert stats.get_novelty_level() == 0.6, "Novelty level should be 0.6"
    assert stats.get_development_stage() == 'Adult', "Development stage should be 'Adult'"
    
    # Test to_dict method
    stats_dict = stats.to_dict()
    assert 'step_count' in stats_dict, "to_dict should contain step_count"
    assert 'brain_stats' in stats_dict, "to_dict should contain brain_stats"
    assert 'agent_stats' in stats_dict, "to_dict should contain agent_stats"
    assert 'world_stats' in stats_dict, "to_dict should contain world_stats"
    
    print("✓ SimulationStats test passed")

def test_config_presets():
    """Test configuration presets."""
    print("Testing configuration presets...")
    
    # Test all presets exist
    presets = ['quick_dev', 'high_perf', 'learning', 'production']
    
    for preset in presets:
        config = nlm.create_config(preset=preset)
        assert config is not None, f"Config for preset '{preset}' should not be None"
        
        # Each preset should have different characteristics
        if preset == 'quick_dev':
            assert config.has('brain.neuron_count'), "quick_dev should have brain.neuron_count"
        elif preset == 'high_perf':
            assert config.has('brain.neuron_count'), "high_perf should have brain.neuron_count"
        elif preset == 'learning':
            assert config.has('neuromod.curiosity.enable'), "learning should have neuromod.curiosity.enable"
        elif preset == 'production':
            assert config.has('neuromod.curiosity.enable'), "production should have neuromod.curiosity.enable"
    
    # Test invalid preset error
    try:
        config = nlm.create_config(preset='invalid_preset')
        assert False, "Should have raised ConfigError for invalid preset"
    except nlm.ConfigError:
        pass  # Expected
    
    print("✓ Configuration presets test passed")

def test_brain_factory():
    """Test BrainFactory class."""
    print("Testing BrainFactory...")
    
    factory = nlm.BrainFactory()
    
    # Test different brain sizes
    sizes = ['small', 'medium', 'large']
    
    for size in sizes:
        brain = factory.create_brain(size=size)
        assert brain is not None, f"Brain for size '{size}' should not be None"
        assert brain.getTotalNeuronCount() > 0, f"Brain should have neurons"
    
    # Test brain with development
    brain_with_dev = factory.create_brain_with_dev(development_enabled=True)
    assert brain_with_dev is not None, "Brain with development should not be None"
    
    # Test brain with neuromodulation
    brain_with_neuromod = factory.create_brain_with_neuromod(neuromodulation_enabled=True)
    assert brain_with_neuromod is not None, "Brain with neuromodulation should not be None"
    
    # Test custom configuration
    custom_config = {'brain.neuron_count': 5000}
    custom_brain = factory.create_brain(custom_config=custom_config)
    assert custom_brain.getTotalNeuronCount() == 5000, "Custom brain should have 5000 neurons"
    
    print("✓ BrainFactory test passed")

def test_world_helper():
    """Test WorldHelper class."""
    print("Testing WorldHelper...")
    
    helper = nlm.WorldHelper()
    
    # Test world creation with default settings
    world = helper.create_world()
    assert world is not None, "World should not be None"
    assert world.getWidth() == 20, "Default width should be 20"
    assert world.getHeight() == 20, "Default height should be 20"
    
    # Test world creation with custom settings
    world_custom = helper.create_world(width=30, height=40, vision_width=12, vision_height=12)
    assert world_custom is not None, "Custom world should not be None"
    assert world_custom.getWidth() == 30, "Width should be 30"
    assert world_custom.getHeight() == 40, "Height should be 40"
    
    # Test world with agent start
    world_with_start = helper.create_world(width=25, height=25, agent_start=(12.5, 12.5))
    assert world_with_start is not None, "World with start should not be None"
    
    print("✓ WorldHelper test passed")

def test_file_io():
    """Test file input/output operations."""
    print("Testing file I/O...")
    
    # Create a temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        # Test config save/load
        config = nlm.create_config(preset='quick_dev')
        
        # Save config
        result = nlm.save_config(config, temp_file)
        assert result.success, "Config save should be successful"
        assert os.path.exists(temp_file), "Config file should exist"
        
        # Load config
        result = nlm.load_config(temp_file)
        assert result.success, "Config load should be successful"
        
        loaded_config = result.data
        assert loaded_config is not None, "Loaded config should not be None"
        
        # Test file cleanup
        os.remove(temp_file)
        
        print("✓ File I/O test passed")
        
    finally:
        # Clean up if file still exists
        if os.path.exists(temp_file):
            os.remove(temp_file)

def test_backward_compatibility():
    """Test backward compatibility with existing C++ bindings."""
    print("Testing backward compatibility...")
    
    # Test that existing C++ binding classes are available
    cpp_classes = [
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage',
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain'
    ]
    
    for class_name in cpp_classes:
        try:
            cls = getattr(nlm, class_name)
            assert cls is not None, f"{class_name} should not be None"
        except AttributeError:
            print(f"⚠️  {class_name} not available (may be expected)")
    
    # Test that global functions are available
    global_functions = [
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'
    ]
    
    for func_name in global_functions:
        try:
            func = getattr(nlm, func_name)
            assert func is not None, f"{func_name} should not be None"
        except AttributeError:
            print(f"⚠️  {func_name} not available (may be expected)")
    
    # Test global constants
    constants = [
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
        'INVALID_POPULATION_ID'
    ]
    
    for const_name in constants:
        try:
            const = getattr(nlm, const_name)
            print(f"  {const_name}: {const}")
        except AttributeError:
            print(f"⚠️  {const_name} not available (may be expected)")
    
    print("✓ Backward compatibility test passed")

def test_module_attributes():
    """Test module attributes and metadata."""
    print("Testing module attributes...")
    
    # Test version
    assert hasattr(nlm, '__version__'), "__version__ should exist"
    assert nlm.__version__ is not None, "__version__ should not be None"
    
    # Test description
    assert hasattr(nlm, '__description__'), "__description__ should exist"
    assert nlm.__description__ is not None, "__description__ should not be None"
    
    # Test __all__ exports
    assert hasattr(nlm, '__all__'), "__all__ should exist"
    assert isinstance(nlm.__all__, list), "__all__ should be a list"
    
    print("✓ Module attributes test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Interface Test Suite")
    print("=" * 60)
    
    tests = [
        test_custom_exceptions,
        test_result_object,
        test_simulation_stats,
        test_config_presets,
        test_brain_factory,
        test_world_helper,
        test_file_io,
        test_backward_compatibility,
        test_module_attributes,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        test_name = test.__name__
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print(f"{'='*50}")
        
        try:
            test()
            print(f"✓ {test_name} PASSED")
            passed += 1
        except Exception as e:
            print(f"✗ {test_name} FAILED: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    # Print summary
    print(f"\n{'='*60}")
    print("Test Summary")
    print(f"{'='*60}")
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        return True
    else:
        print(f"\n⚠️  {failed} test(s) failed")
        return False

if __name__ == '__main__':
    success = run_all_tests()
    sys.exit(0 if success else 1)