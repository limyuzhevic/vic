#!/usr/bin/env python3
"""Test script for improved NLM Python bindings."""

import pynlm
import tempfile
import os

def test_config_validation():
    """Test configuration validation and new methods."""
    print("Testing Config validation and new methods...")
    
    # Test 1: Basic config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ Config creation works")
    
    # Test 2: Set and get with new methods
    config.setIfNotSet("test_key", "test_value")
    assert config.has("test_key")
    
    # Try setting again - should not override
    config.setIfNotSet("test_key", "new_value")
    val = config.get("test_key")
    assert val == "test_value"
    print("✓ setIfNotSet works correctly")
    
    # Test 3: Config validation
    valid_config = pynlm.createDefaultConfig()
    pynlm.validateConfig(valid_config)
    print("✓ Config validation works")
    
    # Test 4: Value type inference
    config.set("number", 42)
    val = config.get("number")
    assert val == 42
    
    config.set("float_val", 3.14)
    val = config.get("float_val")
    assert abs(val - 3.14) < 0.01
    
    config.set("bool_val", True)
    val = config.get("bool_val")
    assert val == True
    print("✓ Value type inference works")
    
    # Test 5: Error handling
    try:
        config.get("nonexistent_key")
        assert False, "Should have raised KeyError"
    except KeyError:
        print("✓ KeyError raised for missing key")
    
    print("Config tests passed!\n")

def test_brain_convenience_methods():
    """Test brain's convenience methods."""
    print("Testing Brain convenience methods...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Convenience methods
    neuron_count = brain.getNeuronCount()  # Alias for getTotalNeuronCount()
    total_neuron_count = brain.getTotalNeuronCount()
    assert neuron_count == total_neuron_count
    print(f"✓ getNeuronCount() works: {neuron_count} neurons")
    
    # Test 2: Spike rate calculation
    spike_rate = brain.getSpikeRate()
    assert isinstance(spike_rate, float)
    assert spike_rate >= 0
    print(f"✓ getSpikeRate() works: {spike_rate}")
    
    # Test 3: Other statistics
    active_count = brain.getActiveNeuronCount()
    firing_count = brain.getFiringNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    
    assert active_count >= 0
    assert firing_count >= 0
    assert total_synapses >= 0
    print(f"✓ Statistics work: active={active_count}, firing={firing_count}, synapses={total_synapses}")
    
    # Test 4: Context manager
    with brain:
        # Inside context manager
        brain.step(0)
        step_neuron_count = brain.getNeuronCount()
        assert step_neuron_count == neuron_count
    print("✓ Context manager works")
    
    print("Brain convenience tests passed!\n")

def test_world_context_manager():
    """Test world context manager."""
    print("Testing World context manager...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    # Test context manager
    with world:
        world.reset()
        assert world.getWidth() == 10
        assert world.getHeight() == 10
    
    print("✓ World context manager works")
    print("World tests passed!\n")

def test_memory_system_access():
    """Test memory system access."""
    print("Testing memory system access...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system access
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    # Note: These may return nullptr if not implemented in C++, but the bindings should not crash
    print(f"✓ Memory systems accessible: working={working_memory is not None}, "
          f"episodic={episodic_memory is not None}, associative={associative_memory is not None}")
    
    print("Memory system tests passed!\n")

def test_prediction_system_access():
    """Test prediction system access."""
    print("Testing prediction system access...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test prediction system access
    prediction_system = brain.getPredictionSystem()
    
    # Note: This may return nullptr if not implemented, but bindings should handle it
    print(f"✓ Prediction system accessible: {prediction_system is not None}")
    
    print("Prediction system tests passed!\n")

def test_neuromodulation_access():
    """Test neuromodulation system access."""
    print("Testing neuromodulation system access...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuromodulation access
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    print(f"✓ Neuromodulation systems accessible: dopamine={dopamine is not None}, "
          f"curiosity={curiosity is not None}, novelty={novelty is not None}, "
          f"prediction_error={prediction_error is not None}")
    
    print("Neuromodulation tests passed!\n")

def test_development_stage():
    """Test developmental stage methods."""
    print("Testing developmental stage methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    print(f"✓ Developmental stage: {stage}")
    
    # Test setting developmental stage
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    new_stage = brain.getDevelopmentalStage()
    assert new_stage == pynlm.DevelopmentalStage.Maturation
    print(f"✓ Can set developmental stage: {new_stage}")
    
    # Test AgentBrain developmental methods
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent_stage = agent.getDevelopmentalStage()
    neuromod_level = agent.getNeuromodulationLevel()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_err = agent.getPredictionError()
    
    print(f"✓ AgentBrain development: stage={agent_stage}, neuromod={neuromod_level}, "
          f"curiosity={curiosity_level}, novelty={novelty_level}, error={prediction_err}")
    
    print("Development stage tests passed!\n")

def test_agent_brain_methods():
    """Test AgentBrain convenience methods."""
    print("Testing AgentBrain methods...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test size methods
    input_size = agent.getSensoryInputSize()
    output_size = agent.getMotorOutputSize()
    
    assert input_size > 0
    assert output_size > 0
    print(f"✓ AgentBrain sizes: input={input_size}, output={output_size}")
    
    # Test neuromodulation flags
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    print("✓ AgentBrain neuromodulation flags work")
    
    print("AgentBrain tests passed!\n")

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test with negative step (should be handled gracefully)
    try:
        brain.step(-1)
        print("✓ Negative step handled gracefully")
    except Exception as e:
        print(f"✓ Exception raised for negative step: {type(e).__name__}")
    
    # Test with invalid neuron type (if applicable)
    print("✓ Error handling tests completed")
    
    print("Error handling tests passed!\n")

def test_version_info():
    """Test version and utility functions."""
    print("Testing version and utility functions...")
    
    version = pynlm.getVersion()
    assert version == "1.0.0"
    print(f"✓ Version: {version}")
    
    print("Version tests passed!\n")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing Improved NLM Python Bindings")
    print("=" * 60)
    print()
    
    try:
        test_config_validation()
        test_brain_convenience_methods()
        test_world_context_manager()
        test_memory_system_access()
        test_prediction_system_access()
        test_neuromodulation_access()
        test_development_stage()
        test_agent_brain_methods()
        test_error_handling()
        test_version_info()
        
        print("=" * 60)
        print("All tests passed! ✓")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())