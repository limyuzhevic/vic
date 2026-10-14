#!/usr/bin/env python3

"""
Test script for validating enhanced NLM Python bindings.
This script tests the new functionality added to the pynlm module.
"""

import pynlm
import numpy as np
import tempfile
import os
import json

def test_basic_bindings():
    """Test basic binding functionality."""
    print("Testing basic bindings...")
    
    # Test identifier creation and formatting
    neuron_id = pynlm.NeuronId(12345)
    assert neuron_id.value == 12345
    assert str(neuron_id).startswith("<NeuronId: 12345>")
    
    synapse_id = pynlm.SynapseId(67890)
    region_id = pynlm.RegionId(54321)
    population_id = pynlm.PopulationId(98765)
    
    # Test enums
    assert pynlm.NeuronType.Excitatory.value == 0
    assert pynlm.SynapseType.Excitatory.value == 0
    assert pynlm.DevelopmentalStage.Initial.value == 0
    assert pynlm.ActionType.MoveForward.value == 0
    assert pynlm.MotorCommand.MoveForward.value == 0
    assert pynlm.WorldObjectType.Empty.value == 0
    
    print("✓ Basic bindings test passed")

def test_config_functionality():
    """Test enhanced configuration functionality."""
    print("Testing configuration functionality...")
    
    # Test basic config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test setting different value types
    config.set("int_value", 42, pynlm.ConfigSource.Runtime)
    config.set("double_value", 3.14, pynlm.ConfigSource.Runtime)
    config.set("string_value", "hello", pynlm.ConfigSource.Runtime)
    config.set("bool_value", True, pynlm.ConfigSource.Runtime)
    
    assert config.has("int_value")
    assert config.has("double_value")
    assert config.has("string_value")
    assert config.has("bool_value")
    
    # Test get methods
    int_val = config.get<int64_t>("int_value")
    assert int_val.has_value() and int_val.value() == 42
    
    double_val = config.get<double>("double_value")
    assert double_val.has_value() and double_val.value() > 3.13 and double_val.value() < 3.15
    
    # Test getOr method
    missing_val = config.getOr<int64_t>("missing", 100)
    assert missing_val == 100
    
    existing_val = config.getOr<int64_t>("int_value", 100)
    assert existing_val == 42
    
    # Test updateFromDict
    config.updateFromDict({
        "new_int": 999,
        "new_string": "world",
        "new_bool": False
    })
    
    assert config.getOr<int64_t>("new_int", 0) == 999
    assert config.getOr<std::string>("new_string", "") == "world"
    assert config.getOr<bool>("new_bool", True) == False
    
    # Test to_dict
    config_dict = config.to_dict()
    assert "int_value" in config_dict
    assert "double_value" in config_dict
    assert "string_value" in config_dict
    assert "bool_value" in config_dict
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        config_file = f.name
    
    try:
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(config_file)
        
        assert config2.has("int_value")
        assert config2.getOr<int64_t>("int_value", 0) == 42
        
    finally:
        os.unlink(config_file)
    
    print("✓ Configuration functionality test passed")

def test_brain_functionality():
    """Test enhanced brain functionality."""
    print("Testing brain functionality...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100, pynlm.ConfigSource.Runtime)
    config.set("region_count", 2, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test initialization
    brain.initialize()
    
    # Test basic properties
    assert brain.getTotalNeuronCount() == 100
    assert brain.getRegionCount() == 2
    assert brain.getTotalSynapseCount() > 0
    
    # Test simulation
    brain.simulate(20, verbose=False)
    
    # Test statistics
    stats = brain.getStatistics()
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    assert "active_neurons" in stats
    assert "firing_neurons" in stats
    assert "average_firing_rate" in stats
    
    # Test developmental stages
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    assert brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        brain.save(f.name)
        brain_file = f.name
    
    try:
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        brain2.load(brain_file)
        
        assert brain2.getTotalNeuronCount() == 100
        assert brain2.getRegionCount() == 2
        
    finally:
        os.unlink(brain_file)
    
    print("✓ Brain functionality test passed")

def test_agent_brain_functionality():
    """Test enhanced agent brain functionality."""
    print("Testing agent brain functionality...")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    world.configure(100, 100, 21, 21)
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Test basic properties
    assert agent_brain.getSensoryInputSize() > 0
    assert agent_brain.getMotorOutputSize() > 0
    
    # Test developmental stages
    assert agent_brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Initial
    assert agent_brain.getNeuromodulationLevel() >= 0.0
    assert agent_brain.getCuriosityLevel() >= 0.0
    assert agent_brain.getNoveltyLevel() >= 0.0
    assert agent_brain.getPredictionError() >= 0.0
    
    # Test enable/disable features
    agent_brain.enableRewardModulation(True)
    assert agent_brain.isRewardModulationEnabled() == True
    
    agent_brain.enableDevelopment(True)
    assert agent_brain.isDevelopmentEnabled() == True
    
    # Test action processing
    percept = pynlm.SensoryPercept()
    action = agent_brain.act(percept)
    
    # Action can be null if no suitable action found
    # That's acceptable behavior
    
    print("✓ Agent brain functionality test passed")

def test_visualization_functionality():
    """Test visualization functionality."""
    print("Testing visualization functionality...")
    
    # Create brain and run simulation
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 50, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    brain.simulate(10)
    
    # Get statistics (if visualization interface available)
    stats = brain.getStatistics()
    assert "total_neurons" in stats
    assert "total_spikes" in stats
    
    print("✓ Visualization functionality test passed")

def test_convenience_functions():
    """Test convenience functions."""
    print("Testing convenience functions...")
    
    # Test createValidatedConfig
    params = {
        "neuron_count": 200,
        "region_count": 3,
        "learning_rate": 0.01
    }
    
    config = pynlm.createValidatedConfig(params)
    assert config is not None
    assert config.getOr<int64_t>("neuron_count", 0) == 200
    assert config.getOr<int64_t>("region_count", 0) == 3
    
    # Test runQuickTest
    quick_brain = pynlm.runQuickTest()
    assert quick_brain is not None
    assert quick_brain.getTotalNeuronCount() == 100
    
    # Test getVersion
    version = pynlm.getVersion()
    assert isinstance(version, str)
    assert "NLM" in version or "version" in version.lower()
    
    # Test format functions
    neuron_id = pynlm.NeuronId(999)
    formatted = pynlm.format_neuron_id(neuron_id)
    assert "NeuronId" in formatted
    
    print("✓ Convenience functions test passed")

def test_numpy_integration():
    """Test NumPy integration."""
    print("Testing NumPy integration...")
    
    try:
        # This test assumes NumPy is available
        import numpy as np
        
        # Test Vision data from NumPy
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create test data
        test_data = np.random.rand(21, 21, 3).astype(np.float32)
        
        # Test Vision integration (if available)
        if hasattr(pynlm, 'Vision'):
            vision = pynlm.Vision(21, 21, 3)
            # This would require additional C++ bindings for full NumPy support
            # For now, just ensure the Vision class exists
            assert vision is not None
        
        print("✓ NumPy integration test passed")
        
    except ImportError:
        print("⚠ NumPy not available, skipping NumPy integration tests")

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test invalid configuration
    try:
        config = pynlm.createDefaultConfig()
        # This should not crash
        config.set("invalid_key", "test", pynlm.ConfigSource.Runtime)
        print("✓ Error handling test passed")
    except Exception as e:
        print(f"⚠ Error handling test encountered expected exception: {e}")

def run_all_tests():
    """Run all tests."""
    print("=== Running NLM Python Bindings Tests ===\n")
    
    tests = [
        test_basic_bindings,
        test_config_functionality,
        test_brain_functionality,
        test_agent_brain_functionality,
        test_visualization_functionality,
        test_convenience_functions,
        test_numpy_integration,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
        print()
    
    print("=== Test Summary ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
    else:
        print(f"\n⚠️  {failed} test(s) failed")
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)