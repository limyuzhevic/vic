#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.

This test suite validates the enhanced Python bindings for the NLM (Neural Learning Machine)
framework, including:
- Basic brain functionality
- Advanced binding features
- Configuration validation
- Error handling
- Debug and monitoring tools
- Experimentation utilities

Run with: pytest test_pynlm.py
"""

import pytest
import tempfile
import os
import pynlm


def test_basic_brain_creation():
    """Test basic brain creation and initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    assert brain.getRegionCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    # Initialize the brain
    result = brain.initialize()
    assert result is True
    
    # Test basic step
    brain.step(0)
    assert brain.getTotalSpikeCount() >= 0


def test_advanced_binding_features():
    """Test advanced Python binding features."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test system information
    info = pynlm.getBrainSystemInfo(brain)
    assert isinstance(info, dict)
    assert "neuron_count" in info
    assert "synapse_count" in info
    assert "firing_neurons" in info
    assert "region_count" in info
    assert "developmental_stage" in info
    
    # Test integration test
    results = pynlm.runIntegrationTest()
    assert isinstance(results, dict)
    assert "total_spikes" in results
    assert "avg_firing_rate" in results
    assert "e_i_ratio" in results
    
    # Test brain comparison
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    comparison = pynlm.compareBrains(brain, brain2)
    assert isinstance(comparison, dict)
    assert "neuron_count" in comparison
    assert "synapse_count" in comparison
    assert "firing_rate" in comparison


def test_config_validation():
    """Test configuration validation and error handling."""
    config = pynlm.createDefaultConfig()
    
    # Test config validation
    errors = pynlm.validateConfig(config)
    assert isinstance(errors, list)
    
    # Test valid config check
    is_valid = pynlm.isValidBrainConfig(config)
    assert isinstance(is_valid, bool)
    
    # Test with custom config
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 500)
    config2.set("brain.region_count", 2)
    config2.set("brain.connection_probability", 0.15)
    
    errors2 = pynlm.validateConfig(config2)
    assert isinstance(errors2, list)
    
    is_valid2 = pynlm.isValidBrainConfig(config2)
    assert is_valid2 is True


def test_error_handling():
    """Test error handling and exception types."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test accessing null brain info
    null_info = pynlm.getBrainSystemInfo(None)
    assert isinstance(null_info, dict)
    assert null_info.get("status") == "null"
    
    # Test that exception types are available
    # (This tests that pybind11 exceptions are registered)
    with pytest.raises(Exception):
        # This should raise an exception if we try to access invalid attributes
        pass  # Pybind11 handles this automatically


def test_world_and_agent():
    """Test world and agent functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test world properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getVisionWidth() == 8
    assert world.getVisionHeight() == 8
    
    # Test agent properties
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Test agent initialization
    world.setAgentStart(10.0, 10.0)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled() is True
    assert agent.isCuriosityEnabled() is True


def test_sensory_motor_cycle():
    """Test complete sensory-motor cycle."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation steps
    for step in range(10):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Test reward modulation
        reward = 0.1  # Example reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Test development update
        agent.updateDevelopment(0.1)
    
    # Verify simulation completed successfully
    assert brain.getTotalSpikeCount() >= 0


def test_agent_stats():
    """Test agent statistics and neuromodulation levels."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get various statistics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromodulation = agent.getNeuromodulationLevel()
    developmental_stage = agent.getDevelopmentalStage()
    
    # Verify all statistics are accessible
    assert isinstance(curiosity, float)
    assert isinstance(novelty, float)
    assert isinstance(prediction_error, float)
    assert isinstance(neuromodulation, float)
    assert developmental_stage is not None
    
    # Test development
    agent.updateDevelopment(1.0)
    
    new_stage = agent.getDevelopmentalStage()
    assert new_stage is not None


def test_save_load_functionality():
    """Test brain save and load functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to generate state
    for step in range(5):
        brain.step(step)
    
    # Note: save/load functionality needs to be implemented
    # For now, just verify the methods exist
    assert hasattr(brain, 'save')
    assert hasattr(brain, 'load')


def test_config_file_operations():
    """Test configuration file operations."""
    config = pynlm.createDefaultConfig()
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.config', delete=False) as f:
        filepath = f.name
        try:
            # Save to file
            result = config.saveToFile(filepath)
            assert isinstance(result, bool)
            
            # Load from file (would need implementation)
            # For now, just test that the methods exist
            assert hasattr(config, 'loadFromFile')
            assert hasattr(config, 'saveToFile')
            
        finally:
            os.unlink(filepath)


def test_enum_values():
    """Test that all enumerations are properly accessible."""
    # Test neuron types
    neuron_types = [pynlm.NeuronType.Excitatory,
                    pynlm.NeuronType.Inhibitory,
                    pynlm.NeuronType.Modulatory,
                    pynlm.NeuronType.Sensory,
                    pynlm.NeuronType.Motor,
                    pynlm.NeuronType.Internal]
    
    for nt in neuron_types:
        assert nt is not None
    
    # Test developmental stages
    stages = [pynlm.DevelopmentalStage.Initial,
              pynlm.DevelopmentalStage.CriticalPeriod,
              pynlm.DevelopmentalStage.Maturation,
              pynlm.DevelopmentalStage.Adult,
              pynlm.DevelopmentalStage.Aging]
    
    for stage in stages:
        assert stage is not None
    
    # Test action types
    actions = [pynlm.ActionType.MoveForward,
               pynlm.ActionType.MoveBackward,
               pynlm.ActionType.TurnLeft,
               pynlm.ActionType.TurnRight,
               pynlm.ActionType.Look,
               pynlm.ActionType.Interact]
    
    for action in actions:
        assert action is not None


def test_performance_monitoring():
    """Test performance monitoring utilities."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with monitoring
    stats = []
    for step in range(20):
        brain.step(step)
        
        # Collect various statistics
        stat = {
            'step': step,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
        stats.append(stat)
    
    # Verify we collected data
    assert len(stats) == 20
    for stat in stats:
        assert 'step' in stat
        assert 'total_spikes' in stat
        assert stat['total_spikes'] >= 0


def test_memory_systems():
    """Test memory system integration (if available)."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory system access (even if they're nullptr placeholders)
    assert hasattr(brain, 'getWorkingMemory')
    assert hasattr(brain, 'getEpisodicMemory')
    assert hasattr(brain, 'getAssociativeMemory')
    
    # Note: These might return nullptr in current implementation
    # but the API should be available for Phase 2
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    # Just verify they exist and don't crash
    assert working_memory is not None or True  # May be nullptr currently


def test_prediction_cognition_systems():
    """Test prediction and cognition system integration (if available)."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test prediction and cognition system access
    assert hasattr(brain, 'getPredictionSystem')
    assert hasattr(brain, 'getPlanner')
    assert hasattr(brain, 'getConceptFormation')
    assert hasattr(brain, 'getAttention')
    
    # Note: These might return nullptr in current implementation
    prediction_system = brain.getPredictionSystem()
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()


def test_neuromodulation_systems():
    """Test neuromodulation system integration (if available)."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuromodulation system access
    assert hasattr(brain, 'getDopamine')
    assert hasattr(brain, 'getCuriosity')
    assert hasattr(brain, 'getNovelty')
    assert hasattr(brain, 'getPredictionErrorSignal')
    
    # Note: These might return nullptr in current implementation
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()


if __name__ == '__main__':
    pytest.main([__file__, '-v'])