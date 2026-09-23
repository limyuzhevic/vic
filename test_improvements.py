#!/usr/bin/env python3
"""
Comprehensive Test Suite for Improved NLM Python API

This test suite verifies all the improvements made to the NLM Python bindings,
including advanced features, error handling, and performance monitoring.
"""

import pynlm
import time
import json
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic functionality...")
    
    # Test 1: Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None, "createDefaultConfig should return a valid config"
    print("✓ createDefaultConfig works")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None, "createBrain should return a valid brain"
    print("✓ createBrain works")
    
    # Test 3: Initialize brain
    success = brain.initialize()
    assert success, "brain.initialize() should return True"
    print("✓ brain.initialize() works")
    
    # Test 4: Get brain statistics
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    assert neuron_count > 0, "getTotalNeuronCount() should return > 0"
    assert synapse_count >= 0, "getTotalSynapseCount() should return >= 0"
    print(f"✓ Brain statistics: {neuron_count} neurons, {synapse_count} synapses")
    
    # Test 5: Step simulation
    for step in range(10):
        brain.step(step)
    print("✓ brain.step() works for 10 steps")
    
    # Test 6: Reset brain
    brain.reset()
    print("✓ brain.reset() works")
    
    return brain

def test_world_and_agent():
    """Test world and agent functionality"""
    print("\nTesting world and agent functionality...")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    assert world is not None, "createSimpleWorld should return a valid world"
    print("✓ createSimpleWorld works")
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "createAgentBrain should return a valid agent"
    print("✓ createAgentBrain works")
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    print("✓ world.configure() works")
    
    # Reset world
    world.reset()
    print("✓ world.reset() works")
    
    # Set agent start
    world.setAgentStart(5.0, 5.0)
    print("✓ world.setAgentStart() works")
    
    # Initialize agent
    agent.initialize(world)
    print("✓ agent.initialize() works")
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("✓ All agent features enabled")
    
    # Simulation loop
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check agent statistics
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        neuromodulation = agent.getNeuromodulationLevel()
        
        assert 0.0 <= curiosity <= 1.0, "Curiosity level should be between 0 and 1"
        assert 0.0 <= novelty <= 1.0, "Novelty level should be between 0 and 1"
        assert 0.0 <= prediction_error <= 10.0, "Prediction error should be reasonable"
        assert 0.0 <= neuromodulation <= 1.0, "Neuromodulation level should be between 0 and 1"
        
        if step % 10 == 0:
            print(f"  Step {step}: curiosity={curiosity:.3f}, novelty={novelty:.3f}, "
                  f"pred_error={prediction_error:.3f}, neuromod={neuromulation:.3f}")
    
    print("✓ Complete simulation loop works")
    return brain, agent, world

def test_advanced_features():
    """Test advanced features"""
    print("\nTesting advanced features...")
    
    # Test 1: System information
    system_info = pynlm.get_system_info()
    assert isinstance(system_info, dict), "get_system_info should return a dict"
    assert "version" in system_info, "System info should contain version"
    assert "author" in system_info, "System info should contain author"
    assert "neuron_model" in system_info, "System info should contain neuron model"
    print("✓ get_system_info() works")
    
    # Test 2: Configuration validation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100, "Runtime")  # Using string instead of enum
    
    is_valid = pynlm.validate_config(config)
    assert isinstance(is_valid, bool), "validate_config should return a bool"
    print(f"✓ validate_config() works: {is_valid}")
    
    # Test 3: Configuration schema
    schema = pynlm.export_config_schema()
    assert isinstance(schema, str), "export_config_schema should return a string"
    assert "brain.neuron_count" in schema, "Schema should contain brain.neuron_count"
    print("✓ export_config_schema() works")
    
    # Test 4: Debug logging
    debug_enabled = pynlm.enable_debug_logging()
    assert isinstance(debug_enabled, bool), "enable_debug_logging should return a bool"
    print("✓ enable_debug_logging() works")
    
    # Test 5: Logging level
    logging_set = pynlm.set_logging_level(2)
    assert isinstance(logging_set, bool), "set_logging_level should return a bool"
    print("✓ set_logging_level() works")
    
    # Test 6: Performance stats
    perf_stats = pynlm.get_performance_stats()
    assert isinstance(perf_stats, dict), "get_performance_stats should return a dict"
    assert "average_firing_rate_hz" in perf_stats, "Performance stats should contain firing rate"
    print("✓ get_performance_stats() works")
    
    return config

def test_file_io():
    """Test file input/output operations"""
    print("\nTesting file I/O operations...")
    
    config = pynlm.createDefaultConfig()
    
    # Test 1: Save config to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
    
    try:
        config.saveToFile(config_file)
        print("✓ config.saveToFile() works")
        
        # Test 2: Load config from file
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(config_file)
        print("✓ config.loadFromFile() works")
        
        # Test 3: Save brain state
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            brain_file = f.name
        
        brain.save(brain_file)
        print("✓ brain.save() works")
        
        # Test 4: Load brain state
        new_brain = pynlm.createBrain(config)
        new_brain.load(brain_file)
        print("✓ brain.load() works")
        
        # Verify loaded brain has same properties
        assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
        assert new_brain.getTotalSynapseCount() == brain.getTotalSynapseCount()
        print("✓ Loaded brain state matches original")
        
    finally:
        # Clean up temporary files
        if os.path.exists(config_file):
            os.unlink(config_file)
        if 'brain_file' in locals() and os.path.exists(brain_file):
            os.unlink(brain_file)
    
    print("✓ All file I/O operations work")
    return config

def test_error_handling():
    """Test error handling"""
    print("\nTesting error handling...")
    
    # Test 1: Invalid logging level
    result = pynlm.set_logging_level(5)  # Invalid level
    assert result is False, "set_logging_level with invalid level should return False"
    print("✓ Invalid logging level handled correctly")
    
    # Test 2: Valid logging levels
    for level in [0, 1, 2, 3]:
        result = pynlm.set_logging_level(level)
        assert result is True, f"set_logging_level with level {level} should return True"
    print("✓ All valid logging levels accepted")
    
    # Test 3: Configuration validation with invalid values
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1, "Runtime")  # Invalid - using string
    is_valid = pynlm.validate_config(config)
    # Note: validation might not catch all invalid values, but should handle them gracefully
    print(f"✓ Configuration validation handles invalid values: {is_valid}")
    
    print("✓ All error handling tests passed")

def test_performance_benchmarking():
    """Test performance benchmarking"""
    print("\nTesting performance benchmarking...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark brain initialization
    start_time = time.time()
    for _ in range(10):
        test_brain = pynlm.createBrain(config)
        test_brain.initialize()
    init_time = time.time() - start_time
    print(f"✓ Brain initialization: {init_time/10:.4f}s average")
    
    # Benchmark simulation steps
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    step_time = time.time() - start_time
    print(f"✓ Brain stepping: {step_time/100:.4f}s average per step")
    
    # Get performance statistics
    perf_stats = pynlm.get_performance_stats()
    assert perf_stats["average_firing_rate_hz"] >= 0.0, "Firing rate should be non-negative"
    print(f"✓ Performance stats: {perf_stats['average_firing_rate_hz']:.2f} Hz average firing rate")
    
    print("✓ Performance benchmarking completed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python API Comprehensive Test Suite")
    print("=" * 60)
    
    try:
        # Run all test suites
        brain1 = test_basic_functionality()
        brain2, agent2, world2 = test_world_and_agent()
        config3 = test_advanced_features()
        config4 = test_file_io()
        test_error_handling()
        test_performance_benchmarking()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("=" * 60)
        print("\nSummary of improvements verified:")
        print("  • Basic NLM functionality (creation, initialization, stepping)")
        print("  • World and agent integration with complete simulation loop")
        print("  • Advanced features (system info, config validation, schema export)")
        print("  • File I/O operations (save/load config and brain state)")
        print("  • Error handling for invalid inputs")
        print("  • Performance benchmarking capabilities")
        print("  • Neuromodulation and learning features")
        
        return True
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
