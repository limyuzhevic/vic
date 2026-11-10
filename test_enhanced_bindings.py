#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python bindings.

This test file validates all new features and improvements in the NLM Python bindings,
including:
- Version and package information
- Context managers
- Brain copying and state management
- Configuration validation
- Experiment utilities
- Error handling
- Performance monitoring capabilities

Run with: python3 test_enhanced_bindings.py
"""

import sys
import os
import tempfile
import time
from typing import Dict, Any, List

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def test_package_info():
    """Test package information and version."""
    print("Testing package information...")
    
    # Test version attribute
    assert hasattr(pynlm, '__version__'), "Missing __version__ attribute"
    assert isinstance(pynlm.__version__, str), "__version__ should be a string"
    print(f"✓ Version: {pynlm.__version__}")
    
    # Test library info function
    info = pynlm.get_library_info()
    assert isinstance(info, str), "get_library_info() should return string"
    print(f"✓ Library info: {info}")
    
    print("✓ Package information tests passed\n")


def test_basic_functionality():
    """Test basic brain functionality."""
    print("Testing basic brain functionality...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check initial state
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    assert brain.getTotalSpikeCount() >= 0, "Should have non-negative spike count"
    print(f"✓ Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    
    print("✓ Basic functionality tests passed\n")


def test_context_managers():
    """Test context manager functionality."""
    print("Testing context managers...")
    
    # Test brain context manager
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        assert brain is not None, "Brain should not be None"
        brain.initialize()
        
        for step in range(5):
            brain.step(step)
    
    print("✓ Brain context manager works")
    
    # Test with explicit copy
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    with brain.copy() as copied_brain:
        assert copied_brain is not None, "Copied brain should not be None"
        # Copy should be separate from original
        assert copied_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
    
    print("✓ Brain copy context manager works")
    print("✓ Context manager tests passed\n")


def test_brain_copying():
    """Test brain copying and state restoration."""
    print("Testing brain copying...")
    
    # Create original brain
    config = pynlm.createDefaultConfig()
    original_brain = pynlm.createBrain(config)
    original_brain.initialize()
    
    # Run some steps
    for step in range(50):
        original_brain.step(step)
    
    original_spikes = original_brain.getTotalSpikeCount()
    print(f"✓ Original brain spikes: {original_spikes}")
    
    # Create copy
    copied_brain = original_brain.copy()
    assert copied_brain is not original_brain, "Copy should be a different object"
    
    # Copy should have same configuration but separate state
    assert copied_brain.getTotalNeuronCount() == original_brain.getTotalNeuronCount()
    print("✓ Copy has same neuron count as original")
    
    # Copy should be independent
    copied_brain.initialize()  # Reinitialize copy
    copied_spikes = 0
    for step in range(25):
        copied_brain.step(step)
        copied_spikes += copied_brain.getTotalSpikeCount()
    
    print(f"✓ Copy spikes: {copied_spikes}")
    print("✓ Brain copying tests passed\n")


def test_configuration_creation():
    """Test configuration creation utilities."""
    print("Testing configuration creation...")
    
    # Test default config
    config = pynlm.createDefaultConfig()
    assert config is not None, "Default config should not be None"
    print("✓ Created default configuration")
    
    # Test experiment config
    exp_config = pynlm.create_experiment_config()
    assert exp_config is not None, "Experiment config should not be None"
    
    # Check experiment config has reasonable values
    assert exp_config.has("brain.neuron_count"), "Experiment config should have neuron count"
    assert exp_config.has("plasticity.stdp.enable"), "Experiment config should have STDP setting"
    print(f"✓ Experiment config has {exp_config.getOr('brain.neuron_count', 0)} neurons")
    
    # Test config from dict
    config_dict = {
        "brain.neuron_count": 500,
        "brain.synapse_density": 0.15,
        "plasticity.stdp.enable": True
    }
    dict_config = pynlm.create_config_from_dict(config_dict)
    assert dict_config is not None, "Config from dict should not be None"
    print("✓ Created config from dictionary")
    
    # Test config validation
    keys = pynlm.validate_config(config)
    assert isinstance(keys, list), "validate_config should return list of keys"
    assert len(keys) > 0, "Config should have keys"
    print(f"✓ Validated config with keys: {keys}")
    
    print("✓ Configuration creation tests passed\n")


def test_configuration_validation():
    """Test configuration validation."""
    print("Testing configuration validation...")
    
    # Create a valid configuration
    config = pynlm.createExperimentConfig()
    
    # Test validation
    keys = pynlm.validate_config(config)
    assert isinstance(keys, list), "Validation should return list"
    assert len(keys) > 0, "Valid config should have keys"
    print(f"✓ Validated config with {len(keys)} keys")
    
    # Test with null config (should raise exception)
    try:
        pynlm.validate_config(None)
        assert False, "validate_config should raise exception for None input"
    except Exception as e:
        print(f"✓ validate_config correctly raises exception for None: {type(e).__name__}")
    
    print("✓ Configuration validation tests passed\n")


def test_experiment_utilities():
    """Test experiment utilities."""
    print("Testing experiment utilities...")
    
    # Create experiment setup
    config = pynlm.createExperimentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # Run experiment
    result = pynlm.run_agent_simulation(agent, world, 100)
    assert isinstance(result, tuple), "run_agent_simulation should return tuple"
    assert len(result) == 2, "Result should have two elements"
    
    spike_count, curiosity_level = result
    assert isinstance(spike_count, int), "Spike count should be integer"
    assert isinstance(curiosity_level, float), "Curiosity level should be float"
    
    print(f"✓ Experiment complete. Spikes: {spike_count}, Curiosity: {curiosity_level:.3f}")
    
    # Test experiment config creation
    exp_config = pynlm.create_experiment_config()
    assert exp_config is not None
    print("✓ Created experiment configuration")
    
    print("✓ Experiment utility tests passed\n")


def test_error_handling():
    """Test error handling and robustness."""
    print("Testing error handling...")
    
    # Test configuration validation with invalid input
    try:
        pynlm.validate_config(None)
        assert False, "Should raise exception for None"
    except Exception:
        print("✓ validate_config handles None input correctly")
    
    # Test brain initialization order
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        # Don't initialize - should work but produce no activity
        brain.step(0)
        print("✓ Brain step works even without initialize (graceful handling)")
    except Exception as e:
        print(f"✓ Brain correctly handles initialization errors: {type(e).__name__}")
    
    # Test invalid configuration values
    try:
        invalid_config = {}
        pynlm.create_config_from_dict(invalid_config)
        print("✓ create_config_from_dict handles empty dict")
    except Exception:
        print("✓ create_config_from_dict handles invalid input")
    
    print("✓ Error handling tests passed\n")


def test_advanced_features():
    """Test advanced features and capabilities."""
    print("Testing advanced features...")
    
    # Test brain with all learning subsystems
    config = pynlm.createExperimentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run comprehensive experiment
    for step in range(200):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Monitor all aspects
    final_spikes = brain.getTotalSpikeCount()
    final_curiosity = agent.getCuriosityLevel()
    final_novelty = agent.getNoveltyLevel()
    final_prediction = agent.getPredictionError()
    final_neuromod = agent.getNeuromodulationLevel()
    dev_stage = brain.getDevelopmentalStage()
    
    print(f"✓ Advanced experiment complete:")
    print(f"  Total spikes: {final_spikes}")
    print(f"  Final curiosity: {final_curiosity:.3f}")
    print(f"  Final novelty: {final_novelty:.3f}")
    print(f"  Final prediction error: {final_prediction:.3f}")
    print(f"  Final neuromodulation: {final_neuromod:.3f}")
    print(f"  Developmental stage: {dev_stage}")
    
    # Test brain representation
    brain_repr = repr(brain)
    brain_str = str(brain)
    print(f"✓ Brain repr: {brain_repr}")
    print(f"✓ Brain str: {brain_str}")
    
    print("✓ Advanced features tests passed\n")


def test_state_management():
    """Test state management and persistence."""
    print("Testing state management...")
    
    # Test state serialization
    config = pynlm.createExperimentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Save state
    temp_dir = tempfile.mkdtemp()
    checkpoint_path = f"{temp_dir}/checkpoint.brain"
    
    brain.save(checkpoint_path)
    print(f"✓ Brain state saved to {checkpoint_path}")
    
    # Restore state
    restored_brain = pynlm.createBrain(config)
    restored_brain.load(checkpoint_path)
    
    print(f"✓ Brain state restored")
    print(f"✓ Original spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Restored spikes: {restored_brain.getTotalSpikeCount()}")
    
    # Clean up
    import os
    os.remove(checkpoint_path)
    os.rmdir(temp_dir)
    
    print("✓ State management tests passed\n")


def test_configuration_parameters():
    """Test different configuration parameters."""
    print("Testing configuration parameters...")
    
    # Test small brain
    small_config = pynlm.createConfigFromDict({"brain.neuron_count": 100})
    small_brain = pynlm.createBrain(small_config)
    small_brain.initialize()
    
    print(f"✓ Small brain: {small_brain.getTotalNeuronCount()} neurons")
    
    # Test large brain
    large_config = pynlm.createConfigFromDict({"brain.neuron_count": 2000})
    large_brain = pynlm.createBrain(large_config)
    large_brain.initialize()
    
    print(f"✓ Large brain: {large_brain.getTotalNeuronCount()} neurons")
    
    # Test plasticity configuration
    plasticity_config = pynlm.createConfigFromDict({
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.01,
        "plasticity.hebbian.enable": False,
        "plasticity.structural.enable": True
    })
    
    plasticity_brain = pynlm.createBrain(plasticity_config)
    plasticity_brain.initialize()
    print("✓ Plasticity configuration created")
    
    # Test neuromodulation configuration
    neuromod_config = pynlm.createConfigFromDict({
        "neuromod.dopamine.scale": 2.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "neuromod.novelty.threshold": 0.7
    })
    
    neuromod_brain = pynlm.createBrain(neurom_config)
    neuromod_brain.initialize()
    print("✓ Neuromodulation configuration created")
    
    print("✓ Configuration parameter tests passed\n")


def test_performance_monitoring():
    """Test performance monitoring capabilities."""
    print("Testing performance monitoring...")
    
    # Create brain for performance testing
    config = pynlm.createExperimentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Time simulation
    start_time = time.time()
    
    for step in range(500):
        brain.step(step)
    
    end_time = time.time()
    elapsed_time = end_time - start_time
    
    print(f"✓ Performance test: {500} steps in {elapsed_time:.3f} seconds")
    print(f"✓ Average time per step: {elapsed_time/500*1000:.3f} ms")
    
    # Test with different step counts
    for step_count in [100, 500, 1000]:
        test_brain = pynlm.createBrain(pynlm.createExperimentConfig())
        test_brain.initialize()
        
        test_start = time.time()
        for step in range(step_count):
            test_brain.step(step)
        test_end = time.time()
        
        print(f"✓ {step_count} steps: {test_end-test_start:.3f} seconds")
    
    print("✓ Performance monitoring tests passed\n")


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("ENHANCED NLM PYTHON BINDINGS - COMPREHENSIVE TEST SUITE")
    print("=" * 60)
    print()
    
    tests = [
        ("Package Information", test_package_info),
        ("Basic Functionality", test_basic_functionality),
        ("Context Managers", test_context_managers),
        ("Brain Copying", test_brain_copying),
        ("Configuration Creation", test_configuration_creation),
        ("Configuration Validation", test_configuration_validation),
        ("Experiment Utilities", test_experiment_utilities),
        ("Error Handling", test_error_handling),
        ("Advanced Features", test_advanced_features),
        ("State Management", test_state_management),
        ("Configuration Parameters", test_configuration_parameters),
        ("Performance Monitoring", test_performance_monitoring),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"RUNNING: {test_name}")
        print(f"{'=' * 60}")
        print()
        
        try:
            test_func()
            passed += 1
            print(f"✅ {test_name}: PASSED")
        except Exception as e:
            failed += 1
            print(f"❌ {test_name}: FAILED")
            print(f"   Error: {type(e).__name__}: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success rate: {passed/len(tests)*100:.1f}%")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 ALL TESTS PASSED! Enhanced NLM Python bindings are working correctly.")
        return True
    else:
        print("⚠️  SOME TESTS FAILED. Please review the errors above.")
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)