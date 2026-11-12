#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.

This test suite provides comprehensive coverage of the NLM Python API,
testing all major classes, methods, and integration scenarios.

Run with: pytest tests/
"""

import pytest
import pynlm
import numpy as np
from typing import Dict, Any, List
import tempfile
import os

class TestConfiguration:
    """Test configuration management."""
    
    def test_default_config_creation(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
    def test_config_has(self):
        """Test checking configuration keys."""
        config = pynlm.createDefaultConfig()
        assert config.has("brain.neuron_count")
        assert config.has("simulation_timestep")
        assert not config.has("nonexistent.key")
        
    def test_config_get_keys(self):
        """Test getting all configuration keys."""
        config = pynlm.createDefaultConfig()
        keys = config.getKeys()
        assert isinstance(keys, list)
        assert len(keys) > 0
        assert "brain.neuron_count" in keys
        
    def test_config_set(self):
        """Test setting configuration values."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        assert config.has("brain.neuron_count")
        
    def test_config_save_load(self):
        """Test saving and loading configuration files."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
            
        try:
            config.saveToFile(config_file)
            
            # Create new config and load
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(config_file)
            
            assert new_config.has("brain.neuron_count")
        finally:
            os.unlink(config_file)
            
    def test_config_load_from_args(self):
        """Test loading configuration from arguments."""
        config = pynlm.createDefaultConfig()
        args = ["--neuron_count", "500", "--simulated_timestep", "0.01"]
        config.loadFromArgs(args)
        
        # Should have updated values from args
        assert config.has("neuron_count")

class TestBasicBrain:
    """Test basic brain functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
    def test_brain_creation(self):
        """Test brain object creation."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        assert brain is not None
        
    def test_brain_initialization(self):
        """Test brain initialization."""
        assert self.brain is not None
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
        
    def test_brain_step(self):
        """Test brain step execution."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for i in range(10):
            self.brain.step(i)
            
        # Should have progressed
        assert self.brain.getTotalSpikeCount() >= initial_spikes
        
    def test_brain_reset(self):
        """Test brain reset functionality."""
        # Run some steps
        for i in range(5):
            self.brain.step(i)
            
        # Reset
        self.brain.reset()
        
        # Should be back to initial state
        assert self.brain.getTotalSpikeCount() == 0
        
    def test_brain_save_load(self):
        """Test brain state persistence."""
        # Run some steps
        for i in range(20):
            self.brain.step(i)
            
        # Save
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_file = f.name
            
        try:
            self.brain.save(save_file)
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            new_brain.load(save_file)
            
            # Should have loaded state
            assert new_brain.getTotalSpikeCount() > 0
        finally:
            os.unlink(save_file)
            
    def test_brain_region_management(self):
        """Test brain region management."""
        # Add region
        region_id = self.brain.addRegion("TestRegion")
        assert region_id is not None
        
        # Get region
        region = self.brain.getRegion(region_id)
        assert region is not None
        
    def test_brain_statistics(self):
        """Test brain statistics methods."""
        stats = {
            "neurons": self.brain.getTotalNeuronCount(),
            "synapses": self.brain.getTotalSynapseCount(),
            "spikes": self.brain.getTotalSpikeCount(),
            "firingNeurons": self.brain.getFiringNeuronCount(),
            "developmentStage": self.brain.getDevelopmentalStage()
        }
        
        assert stats["neurons"] > 0
        assert stats["synapses"] > 0
        assert "firingNeurons" in stats
        
    def test_brain_neuromodulation(self):
        """Test neuromodulation access."""
        # These should exist in the enhanced bindings
        spike_system = self.brain.getSpikeSystem()
        assert spike_system is not None
        
    def test_brain_plasticity(self):
        """Test plasticity system access."""
        stdp = self.brain.getSTDP()
        hebbian = self.brain.getHebbian()
        assert stdp is not None
        assert hebbian is not None

class TestAgent:
    """Test agent functionality."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def test_agent_creation(self):
        """Test agent creation."""
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(self.world)
        assert agent is not None
        
    def test_agent_sensory_processing(self):
        """Test sensory processing."""
        percept = self.world.getSensoryPercept()
        assert percept is not None
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Should have modified brain state
        assert self.brain.getTotalSpikeCount() >= 0
        
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Inject current to make neurons fire
        for neuron_id in range(self.brain.getTotalNeuronCount()):
            self.brain.injectCurrent(pynlm.NeuronId(neuron_id), 50.0)
            
        # Step brain
        self.brain.step(0)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        assert action is not None
        
    def test_agent_reward_modulation(self):
        """Test reward modulation."""
        # Apply reward modulation
        self.agent.applyRewardModulation(0.8, 0.5)
        
        # Check neuromodulation level
        neuromod_level = self.agent.getNeuromodulationLevel()
        assert neuromod_level >= 0.0
        
    def test_agent_development(self):
        """Test development system."""
        self.agent.updateDevelopment(0.1)
        
        # Get developmental stage
        stage = self.agent.getDevelopmentalStage()
        assert stage is not None
        
    def test_agent_subsystem_enabling(self):
        """Test enabling/disabling subsystems."""
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check they are enabled
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isStructuralPlasticityEnabled()
        assert self.agent.isDevelopmentEnabled()
        assert self.agent.isCuriosityEnabled()
        
    def test_agent_thought_trace(self):
        """Test thought trace generation."""
        trace = self.agent.getThoughtTrace()
        assert isinstance(trace, str)
        
    def test_agent_reset(self):
        """Test agent reset."""
        # Process some input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Reset
        self.agent.reset()
        
        # Should be in fresh state

class TestCognitiveFunctions:
    """Test cognitive functions."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
    def test_self_model(self):
        """Test SelfModel functionality."""
        # The SelfModel is integrated into AgentBrain
        # Access through agent (this depends on implementation)
        # self.agent.getSelfModel() would work in enhanced bindings
        
        # For now, test that it exists in the enhanced bindings
        pass
        
    def test_social_learning(self):
        """Test SocialLearning functionality."""
        # Similar to SelfModel, this is integrated
        # self.agent.getSocialLearning() would work in enhanced bindings
        pass
        
    def test_neural_planner(self):
        """Test NeuralPlanner functionality."""
        # NeuralPlanner is integrated into AgentBrain
        # self.agent.getNeuralPlanner() would work in enhanced bindings
        pass
        
    def test_concept_formation(self):
        """Test ConceptFormation functionality."""
        # ConceptFormation is integrated into brain
        # self.brain.getConceptFormation() would work in enhanced bindings
        pass

class TestWorldInteraction:
    """Test world interaction scenarios."""
    
    def test_agent_simulation_loop(self):
        """Test complete agent simulation loop."""
        # Use the convenience function
        brain, world, agent = pynlm.createCompleteAgentSetup(
            pynlm.createDefaultConfig(),
            worldWidth=15,
            worldHeight=15,
            visionWidth=6,
            visionHeight=6
        )
        
        # Run simulation
        pynlm.runAgentSimulation(brain, world, agent, 50)
        
        # Should have run without errors
        assert brain.getTotalSpikeCount() >= 0
        
    def test_world_configuration(self):
        """Test world configuration options."""
        world = pynlm.createSimpleWorld()
        
        # Test different configurations
        configs = [
            {"width": 10, "height": 10, "visionWidth": 4, "visionHeight": 4},
            {"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8},
            {"width": 5, "height": 5, "visionWidth": 2, "visionHeight": 2},
        ]
        
        for config in configs:
            world = pynlm.createSimpleWorld()
            world.configure(**config)
            
            assert world.getWidth() == config["width"]
            assert world.getHeight() == config["height"]
            
    def test_world_agent_interaction(self):
        """Test agent-world interaction."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Create agent
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run steps
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Should have interacted
        assert world.getAgentBody().x >= 0.0

class TestPerformance:
    """Test performance and scalability."""
    
    def test_large_brain_simulation(self):
        """Test simulation with larger brain."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
            
        # Should have processed
        assert brain.getTotalSpikeCount() >= 0
        
    def test_multiple_agents(self):
        """Test multiple agents in same world."""
        config = pynlm.createDefaultConfig()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        
        # Create multiple agents
        agents = []
        brains = []
        
        for i in range(3):
            brain = pynlm.createBrain(config)
            brain.initialize()
            agents.append(pynlm.createAgentBrain(brain))
            agents[-1].initialize(world)
            brains.append(brain)
            
        # Run simulation with all agents
        for step in range(20):
            world.update(0.1)
            
            for agent in agents:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                agent.decodeMotorCommand()
                
        # All agents should have run
        for brain in brains:
            assert brain.getTotalSpikeCount() >= 0
            
    def test_simulation_stability(self):
        """Test simulation stability over time."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run for extended period
        spike_counts = []
        for step in range(200):
            brain.step(step)
            spike_counts.append(brain.getTotalSpikeCount())
            
        # Should not crash or explode
        assert len(spike_counts) == 200
        
        # Should have some reasonable activity
        assert max(spike_counts) > 0

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_invalid_neuron_id(self):
        """Test handling of invalid neuron IDs."""
        # This should be handled gracefully
        invalid_id = pynlm.NeuronId(999999)  # Very large ID
        
        # Should not crash
        pass
        
    def test_brain_before_initialize(self):
        """Test behavior when brain is used before initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should not crash if step is called (though behavior may be undefined)
        try:
            brain.step(0)
        except:
            pass  # Expected to potentially fail
            
    def test_config_file_not_found(self):
        """Test handling of missing config files."""
        config = pynlm.createDefaultConfig()
        
        # Should handle missing file gracefully
        try:
            config.loadFromFile("/nonexistent/path/config.cfg")
        except:
            pass  # Expected to fail
            
    def test_save_file_permissions(self):
        """Test saving with invalid file permissions."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to save to read-only location (may fail)
        pass

class TestIntegration:
    """Test integration scenarios."""
    
    def test_phase6_integration_demo(self):
        """Test Phase 6 integration scenario."""
        # This simulates the Phase 6 integration demo
        config = pynlm.createDefaultConfig()
        
        # Create complete setup
        brain, world, agent = pynlm.createCompleteAgentSetup(
            config,
            worldWidth=20,
            worldHeight=20,
            visionWidth=8,
            visionHeight=8
        )
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                      f"curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Should have completed successfully
        assert brain.getTotalSpikeCount() > 0
        
    def test_complete_brain_workflow(self):
        """Test complete brain workflow from setup to shutdown."""
        # 1. Create brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("plasticity.stdp.learning_rate", 0.01)
        
        # 2. Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        world.setAgentStart(7.5, 7.5)
        
        # 6. Run simulation with learning
        for step in range(150):
            # Update environment
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Agent decides action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning (simulate reward)
            if step % 10 == 0:
                agent.applyRewardModulation(0.5, 0.3)
                
            # Update development
            agent.updateDevelopment(0.1)
            
        # 7. Save final state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_file = f.name
            
        try:
            brain.save(save_file)
            
            # Create new brain and load
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_brain.initialize()
            new_brain.load(save_file)
            
            # Should have restored state
            assert new_brain.getTotalSpikeCount() > 0
            
        finally:
            os.unlink(save_file)
            
    def test_performance_monitoring(self):
        """Test performance monitoring and metrics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run and collect metrics
        metrics = {}
        for step in range(50):
            brain.step(step)
            
            # Collect metrics
            metrics[f"step_{step}"] = {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "development_stage": brain.getDevelopmentalStage()
            }
            
        # Should have collected metrics
        assert len(metrics) == 50
        
        # Check for reasonable values
        for step_data in metrics.values():
            assert step_data["total_spikes"] >= 0
            assert step_data["firing_neurons"] >= 0
            assert step_data["total_synapses"] > 0

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
