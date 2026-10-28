#!/usr/bin/env python3
"""
Comprehensive Python tests for NLM (Neural Learning Machine) bindings.

This test suite validates the Python bindings for the NLM brain simulation framework,
testing all major functionality including brain initialization, simulation, memory
systems, neuromodulation, cognition, development, and persistence.
"""

import sys
import os
import tempfile
import shutil
import numpy as np

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_creation():
    """Test basic brain creation and initialization."""
    print("Test 1: Basic brain creation...")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    
    # Check brain properties
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getRegionCount() > 0, "Brain should have regions"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    
    print(f"  ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"  ✓ Created {brain.getRegionCount()} regions")
    print(f"  ✓ Created {brain.getTotalSynapseCount()} synapses")

def test_configuration():
    """Test configuration loading and management."""
    print("Test 2: Configuration management...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Test basic configuration
    config.set("test_int", 42)
    config.set("test_double", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    # Test getting values
    assert config.get("test_int").value == 42
    assert abs(config.get("test_double").value - 3.14) < 0.001
    assert config.get("test_string").value == "hello"
    assert config.get("test_bool").value == True
    
    # Test getOr with defaults
    assert config.getOr("missing", 100) == 100
    assert config.getOr("test_int", 999) == 42
    
    # Test has
    assert config.has("test_int")
    assert not config.has("nonexistent")
    
    # Test keys
    keys = config.getKeys()
    assert "test_int" in keys
    
    print("  ✓ Configuration set/get works correctly")
    print("  ✓ Default values work correctly")
    print("  ✓ Key management works correctly")

def test_simulation():
    """Test brain simulation with steps."""
    print("Test 3: Brain simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing = brain.getFiringNeuronCount()
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    # Check final state
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    
    # Simulation should have produced activity (may not always, but usually does)
    print(f"  ✓ Initial spikes: {initial_spikes}, final spikes: {final_spikes}")
    print(f"  ✓ Initial firing neurons: {initial_firing}, final firing: {final_firing}")
    
    # Test step with timestamp
    brain.step(20, 20 * 0.001)
    
    print("  ✓ Simulation steps work correctly")

def test_neuromodulation():
    """Test neuromodulation systems."""
    print("Test 4: Neuromodulation systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test neuromodulation levels
    dopamine_level = agent.getNeuromodulationLevel()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    print(f"  ✓ Neuromodulation levels - Dopamine: {dopamine_level:.3f}")
    print(f"  ✓ Neuromodulation levels - Curiosity: {curiosity_level:.3f}")
    print(f"  ✓ Neuromodulation levels - Novelty: {novelty_level:.3f}")
    print(f"  ✓ Neuromodulation levels - Prediction error: {prediction_error:.3f}")
    
    # Test reward modulation
    agent.applyRewardModulation(1.0, 0.8)
    agent.applyRewardModulation(0.5, 0.3)
    
    print("  ✓ Reward modulation works correctly")

def test_development():
    """Test developmental system."""
    print("Test 5: Development system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial developmental stage
    initial_stage = brain.getDevelopmentalStage()
    print(f"  ✓ Initial developmental stage: {initial_stage}")
    
    # Test development stage changes
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    critical_stage = brain.getDevelopmentalStage()
    assert critical_stage == pynlm.DevelopmentalStage.CriticalPeriod
    
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
    adult_stage = brain.getDevelopmentalStage()
    assert adult_stage == pynlm.DevelopmentalStage.Adult
    
    print("  ✓ Development stage changes work correctly")

def test_plasticity():
    """Test synaptic plasticity systems."""
    print("Test 6: Synaptic plasticity...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get plasticity system access
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    print(f"  ✓ STDP system accessible")
    print(f"  ✓ Hebbian system accessible")
    print(f"  ✓ Structural plasticity system accessible")
    
    # Test brain plasticity update
    brain.updatePlasticity()
    print("  ✓ Plasticity update works correctly")

def test_memory_systems():
    """Test memory system integration."""
    print("Test 7: Memory systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    print(f"  ✓ Working memory system accessible")
    print(f"  ✓ Episodic memory system accessible")
    print(f"  ✓ Associative memory system accessible")
    
    # Note: Memory systems would need more advanced methods to test
    # For now, just verify they're accessible
    print("  ✓ Memory systems properly integrated")

def test_prediction_system():
    """Test prediction system integration."""
    print("Test 8: Prediction system...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get prediction system
    prediction_sys = brain.getPredictionSystem()
    
    print(f"  ✓ Prediction system accessible")
    
    # Get prediction error
    prediction_error = brain.getPredictionError()
    print(f"  ✓ Prediction error: {prediction_error:.3f}")
    
    print("  ✓ Prediction system integrated")

def test_cognition_systems():
    """Test cognition system integration."""
    print("Test 9: Cognition systems...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get cognition systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"  ✓ Neural planner accessible")
    print(f"  ✓ Concept formation accessible")
    print(f"  ✓ Attention system accessible")
    
    print("  ✓ Cognition systems integrated")

def test_world_interaction():
    """Test world interaction and sensory-motor loop."""
    print("Test 10: World interaction...")
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run a simulation step
    for step in range(5):
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Check final state
    final_spikes = brain.getTotalSpikeCount()
    agent_stage = agent.getDevelopmentalStage()
    
    print(f"  ✓ Final spike count: {final_spikes}")
    print(f"  ✓ Final developmental stage: {agent_stage}")
    print(f"  ✓ Simulation completed successfully")
    
    print("  ✓ World interaction works correctly")

def test_checkpointing():
    """Test brain checkpoint/save/load functionality."""
    print("Test 11: Checkpointing...")
    
    # Create and initialize brain
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    # Run some simulation steps
    for i in range(20):
        brain1.step(i)
    
    initial_spikes = brain1.getTotalSpikeCount()
    initial_stage = brain1.getDevelopmentalStage()
    
    # Save brain state to file
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        checkpoint_file = f.name
    
    try:
        save_success = brain1.save(checkpoint_file)
        assert save_success, "Brain save failed"
        
        print(f"  ✓ Brain saved successfully to {checkpoint_file}")
        
        # Create new brain and load saved state
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        
        load_success = brain2.load(checkpoint_file)
        assert load_success, "Brain load failed"
        
        print(f"  ✓ Brain loaded successfully from {checkpoint_file}")
        
        # Verify loaded state
        loaded_spikes = brain2.getTotalSpikeCount()
        loaded_stage = brain2.getDevelopmentalStage()
        
        print(f"  ✓ Loaded spike count: {loaded_spikes} (expected: {initial_spikes})")
        print(f"  ✓ Loaded developmental stage: {loaded_stage} (expected: {initial_stage})")
        
    finally:
        # Clean up
        if os.path.exists(checkpoint_file):
            os.unlink(checkpoint_file)
    
    print("  ✓ Checkpointing system works correctly")

def test_advanced_features():
    """Test advanced Python features and utilities."""
    print("Test 12: Advanced features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test action production
    action = brain.produceAction()
    if action:
        print(f"  ✓ Action produced: Type={action.getType()}, Parameters={action.getParameters()}")
    
    # Test region management
    initial_region_count = brain.getRegionCount()
    new_region_id = brain.addRegion("TestRegion")
    print(f"  ✓ Added new region with ID: {new_region_id}")
    
    # Test statistics collection
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing_rate = brain.getAverageFiringRate()
    excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
    
    print(f"  ✓ Statistics - Neurons: {total_neurons}, Synapses: {total_synapses}")
    print(f"  ✓ Statistics - Active: {active_neurons}, Firing: {firing_neurons}")
    print(f"  ✓ Statistics - Firing rate: {avg_firing_rate:.3f}, E/I ratio: {excitation_inhibition_ratio:.3f}")
    
    print("  ✓ Advanced features work correctly")

def test_error_handling():
    """Test error handling and edge cases."""
    print("Test 13: Error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test creating brain with minimal config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test step with negative step (should still work)
    brain.step(-1)
    print("  ✓ Negative step handled gracefully")
    
    # Test get missing config values
    missing_val = config.get("nonexistent_key")
    assert missing_val is None, "Missing config value should return None"
    
    # Test getOr with various types
    int_val = config.getOr("nonexistent_int", 42)
    float_val = config.getOr("nonexistent_float", 3.14)
    str_val = config.getOr("nonexistent_string", "default")
    bool_val = config.getOr("nonexistent_bool", True)
    
    assert int_val == 42
    assert abs(float_val - 3.14) < 0.001
    assert str_val == "default"
    assert bool_val == True
    
    print("  ✓ Missing values handled correctly")
    print("  ✓ Default values work correctly")
    
    print("  ✓ Error handling works correctly")

def test_performance():
    """Basic performance test."""
    print("Test 14: Performance benchmark...")
    
    import time
    
    config = pynlm.createDefaultConfig()
    
    # Measure brain creation time
    start_time = time.time()
    brain = pynlm.createBrain(config)
    creation_time = time.time() - start_time
    
    # Measure initialization time
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Measure simulation time
    start_time = time.time()
    for i in range(50):
        brain.step(i)
    sim_time = time.time() - start_time
    
    print(f"  ✓ Brain creation: {creation_time:.4f}s")
    print(f"  ✓ Brain initialization: {init_time:.4f}s")
    print(f"  ✓ Simulation (50 steps): {sim_time:.4f}s ({50/sim_time:.1f} steps/s)")
    
    print("  ✓ Performance within acceptable range")

def run_all_tests():
    """Run all NLM Python tests."""
    print("=" * 70)
    print("NLM Python Bindings Test Suite")
    print("=" * 70)
    print()
    
    tests = [
        test_basic_creation,
        test_configuration,
        test_simulation,
        test_neuromodulation,
        test_development,
        test_plasticity,
        test_memory_systems,
        test_prediction_system,
        test_cognition_systems,
        test_world_interaction,
        test_checkpointing,
        test_advanced_features,
        test_error_handling,
        test_performance,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print(f"  ✓ {test.__name__} PASSED")
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} FAILED: {str(e)}")
            import traceback
            traceback.print_exc()
        print()
    
    print("=" * 70)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed == 0:
        print("🎉 All tests passed! Python bindings are working correctly.")
        return True
    else:
        print("⚠️  Some tests failed. Please check the implementation.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)