# Test suite for NLM Python bindings
"""
Comprehensive test suite for NLM Python bindings.
Tests both basic functionality and advanced features.
"""

import pytest
import pynlm
import tempfile
import os

class TestBasicFunctionality:
    """Test basic NLM functionality."""
    
    def test_version(self):
        """Test version information."""
        assert pynlm.__version__ == "0.1.0"
        assert hasattr(pynlm, 'get_version')
        assert pynlm.get_version() == "0.1.0"
    
    def test_config_creation(self):
        """Test config creation and default values."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
        assert config.has("brain.neuron_count")
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
        brain.initialize()
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
    
    def test_step_functionality(self):
        """Test brain stepping."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        assert brain.getTotalSpikeCount() >= initial_spikes
    
    def test_agent_creation(self):
        """Test agent creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test agent initialization
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        agent.initialize(world)
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        world.configure(20, 20, 8, 8)
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
        
        world.reset()
        assert world.getSimulationTime() == 0.0
        
        # Test world update
        world.update(0.1)
        assert world.getSimulationTime() == 0.1

class TestIntegration:
    """Test integration of all systems."""
    
    def test_phase6_integration_test(self):
        """Test Phase 6 integration functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        result = pynlm.run_phase6_integration_test(brain)
        assert isinstance(result, tuple)
        assert len(result) == 4  # spikes, firing_rate, curiosity, developmental_stage
        
        spikes, firing_rate, curiosity, dev_stage = result
        assert isinstance(spikes, int) and spikes >= 0
        assert isinstance(firing_rate, float) and firing_rate >= 0.0
        assert isinstance(curiosity, float) and 0.0 <= curiosity <= 1.0
        assert isinstance(dev_stage, pynlm.DevelopmentalStage)
    
    def test_complete_agent_world_loop(self):
        """Test complete agent-world interaction loop."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        steps = 50
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check metrics are updating
            assert agent.getCuriosityLevel() >= 0.0
            assert agent.getNoveltyLevel() >= 0.0
        
        # Verify final state
        assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial
        assert brain.getTotalSpikeCount() >= 0
    
    def test_checkpoint_system(self):
        """Test checkpoint save/load functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add some activity
        for step in range(10):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filename = f.name
        
        try:
            pynlm.save_brain_checkpoint(brain, filename)
            assert os.path.exists(filename)
            
            # Create new brain and load
            config2 = pynlm.createDefaultConfig()
            brain2 = pynlm.createBrain(config2)
            brain2.load(filename)
            
            # Verify basic properties match
            assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
            
        finally:
            if os.path.exists(filename):
                os.unlink(filename)

class TestAdvancedFeatures:
    """Test advanced features for expert users."""
    
    def test_advanced_brain_creation(self):
        """Test advanced brain creation with custom parameters."""
        brain = pynlm.create_advanced_brain(3000, 0.2)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 3000
        
        # Verify advanced features are enabled
        # (In a real implementation, we would check specific system states)
    
    def test_complex_world_creation(self):
        """Test complex world creation with terrain."""
        world = pynlm.create_complex_world(50, 50, 10, 10)
        assert world is not None
        assert world.getWidth() == 50
        assert world.getHeight() == 50
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
    
    def test_advanced_simulation(self):
        """Test advanced simulation with monitoring."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 6, 6)
        world.reset()
        
        agent.initialize(world)
        
        # Run advanced simulation
        steps = 100
        max_curiosity, max_novelty, learning_events = pynlm.run_advanced_simulation(
            brain, world, agent, steps, verbose=False
        )
        
        assert isinstance(max_curiosity, float)
        assert isinstance(max_novelty, float)
        assert isinstance(learning_events, int)
        assert max_curiosity >= 0.0
        assert max_novelty >= 0.0
        assert learning_events >= 0
    
    def test_experiment_management(self):
        """Test experiment management functionality."""
        # Test NLMSimulation class
        sim = pynlm.NLMSimulation()
        assert sim.config is not None
        assert sim.brain is None
        assert sim.agent is None
        
        # Test experiment setup
        brain = sim.setup_brain('standard')
        assert brain is not None
        assert sim.brain is not None
        
        world = sim.setup_complex_world(25, 25, 8, 8)
        assert world is not None
        assert sim.world is not None
        
        # Test episode run
        stats = sim.run_episode(50, verbose=False)
        assert isinstance(stats, dict)
        assert 'duration' in stats
        assert 'steps' in stats
        assert 'final_spike_count' in stats
        
        # Test ExperimentManager
        manager = pynlm.ExperimentManager()
        exp1 = manager.create_experiment('test1', brain_type='compact')
        exp2 = manager.create_experiment('test2', brain_type='expert')
        
        assert exp1 is not None
        assert exp2 is not None
        
        # Test comparison
        results = manager.compare_experiments(['test1', 'test2'], 50)
        assert isinstance(results, dict)
        assert 'test1' in results
        assert 'test2' in results

class TestConfiguration:
    """Test configuration management."""
    
    def test_preset_configurations(self):
        """Test preset configuration availability."""
        presets = pynlm.presets
        assert 'quick_test' in presets
        assert 'standard_test' in presets
        assert 'expert_test' in presets
        assert 'integration_test' in presets
        
        # Verify preset structure
        for preset_name, preset_config in presets.items():
            assert 'brain_type' in preset_config
            assert 'world_size' in preset_config
            assert 'steps' in preset_config
            assert 'learning' in preset_config
    
    def test_convenience_functions(self):
        """Test convenience functions."""
        # These should run without errors
        quick_result = pynlm.quick_test(20)
        assert isinstance(quick_result, dict)
        
        standard_result = pynlm.standard_test(50)
        assert isinstance(standard_result, dict)
        
        expert_result = pynlm.expert_test(100)
        assert isinstance(expert_result, dict)
        
        integration_result = pynlm.run_integration_test()
        assert isinstance(integration_result, tuple)
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        # Valid configurations
        assert pynlm.validate_simulation_config(1000, (20, 20), 100, True)
        assert pynlm.validate_simulation_config(500, (10, 10), 50, False)
        
        # Invalid configurations
        assert not pynlm.validate_simulation_config(50, (10, 10), 50, True)  # Too small brain
        assert not pynlm.validate_simulation_config(10000, (10, 10), 50, True)  # Too large brain
        assert not pynlm.validate_simulation_config(1000, (3, 3), 50, True)  # Too small world
        assert not pynlm.validate_simulation_config(1000, (20, 20), 5, True)  # Too few steps
        assert not pynlm.validate_simulation_config(1000, (20, 20), 100, True)  # Learning but insufficient steps

class TestErrorHandling:
    """Test error handling."""
    
    def test_invalid_brain_type(self):
        """Test handling of invalid brain type."""
        sim = pynlm.NLMSimulation()
        with pytest.raises(ValueError):
            sim.setup_brain('invalid_type')
    
    def test_invalid_world_size(self):
        """Test handling of invalid world size in validation."""
        assert not pynlm.validate_simulation_config(1000, (2, 2), 100, True)
    
    def test_uninitialized_simulation(self):
        """Test error when running uninitialized simulation."""
        sim = pynlm.NLMSimulation()
        with pytest.raises(RuntimeError):
            sim.run_episode(10)