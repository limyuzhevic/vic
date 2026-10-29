"""
Integration NLM Python tests for complete system integration.

Tests for complete simulation setup and system integration.
"""

import pytest
import pynlm


class TestCompleteSimulationSetup:
    """Test complete simulation setup."""
    
    def test_complete_simulation_setup(self):
        """Test complete simulation setup with all components."""
        # Create complete setup using factory function
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Verify all components are created
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent is not None
        
        # Verify configuration
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
        assert config.has("neuron_count")
        
        # Verify brain is initialized
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
        # Verify world is configured
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
        
        # Verify agent is initialized with brain and world
        assert agent.getBrain() is not None
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_complete_simulation_functionality(self):
        """Test complete simulation functionality."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Run simulation steps
        steps = 20
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed successfully
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() >= 0.0
    
    def test_simulation_with_brain_configuration(self):
        """Test simulation with specific brain configuration."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Configure brain
        config.createBrainConfig(neuronCount=500, regionCount=2, connectionProb=0.15)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify configuration was applied
        assert brain.getTotalNeuronCount() == 500
        assert brain.getRegionCount() == 2
    
    def test_simulation_with_agent_configuration(self):
        """Test simulation with agent configuration."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Configure agent
        config.createAgentConfig()
        
        # Verify configuration
        assert config.getBool("agent.reward_modulation", False) == True
        assert config.getBool("agent.structural_plasticity", False) == True
        assert config.getBool("agent.development", False) == True
        assert config.getBool("agent.curiosity", False) == True
        assert config.getBool("agent.planning", False) == True


class TestIntegrationBenchmarks:
    """Test integration benchmarks and performance."""
    
    def test_brain_simulation_benchmark(self):
        """Test brain simulation benchmark."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Run benchmark simulation
        steps = 50
        brain_benchmark = pynlm.benchmarkSimulation(brain, steps)
        
        # Verify benchmark results
        assert brain_benchmark["steps"] == steps
        assert brain_benchmark["timeElapsed"] > 0.0
        assert brain_benchmark["spikesGenerated"] >= 0
        assert brain_benchmark["spikesPerSecond"] >= 0.0
        
        # Check that benchmark results are reasonable
        assert brain_benchmark["timeElapsed"] < 10.0  # Should complete quickly
    
    def test_brain_comparison_benchmark(self):
        """Test brain comparison benchmark."""
        # Create two different brains
        config1 = pynlm.createDefaultConfig()
        config1.createBrainConfig(neuronCount=500)
        brain1 = pynlm.createBrain(config1)
        brain1.initialize()
        
        config2 = pynlm.createDefaultConfig()
        config2.createBrainConfig(neuronCount=1000)
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        # Run comparison benchmark
        steps = 20
        comparison = pynlm.compareBrains(brain1, brain2, steps)
        
        # Verify comparison results
        assert "brain1_spikes" in comparison
        assert "brain2_spikes" in comparison
        assert "brain1_firingRate" in comparison
        assert "brain2_firingRate" in comparison
        assert "brain1_eI_ratio" in comparison
        assert "brain2_eI_ratio" in comparison
        assert "spikeRatio" in comparison
        
        # Verify comparison is reasonable
        assert comparison["brain1_spikes"] >= 0
        assert comparison["brain2_spikes"] >= 0
        assert comparison["spikeRatio"] >= 0.0


class TestIntegrationWorkflow:
    """Test integration workflow scenarios."""
    
    def test_learning_episode(self):
        """Test learning episode simulation."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Run learning episode
        steps = 30
        learning_result = agent.runLearningEpisode(steps, 0.1)
        
        # Verify learning episode completed
        assert learning_result >= 0
        assert brain.getTotalSpikeCount() >= 0
        
        # Check that agent is still functional
        assert agent.getBrain() is not None
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_world_interaction_episode(self):
        """Test agent-world interaction episode."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Set agent start position
        world.setAgentStart(10.0, 10.0)
        
        # Run interaction episode
        steps = 20
        successful_steps = 0
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            successful_steps += 1
        
        # Verify interaction episode completed
        assert successful_steps == steps
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0


class TestIntegrationValidation:
    """Test integration validation scenarios."""
    
    def test_simulation_validation(self):
        """Test simulation validation."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Validate simulation components
        assert config is not None
        assert brain is not None
        assert world is not None
        assert agent is not None
        
        # Validate brain configuration
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
        # Validate world configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        
        # Validate agent configuration
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_world_consistency(self):
        """Test agent-world consistency."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get initial state
        initial_time = world.getSimulationTime()
        initial_body = world.getAgentBody()
        initial_percept = world.getSensoryPercept()
        
        # Run simulation
        for step in range(5):
            world.update(0.1)
            
            # Check consistency
            current_time = world.getSimulationTime()
            current_body = world.getAgentBody()
            current_percept = world.getSensoryPercept()
            
            assert current_time >= initial_time
            assert current_body is not None
            assert current_percept is not None
            
            # Check that agent has proper reference
            assert agent.getBrain() is not None
            assert agent.getSensoryInputSize() > 0
            assert agent.getMotorOutputSize() > 0
    
    def test_simulation_stability(self):
        """Test simulation stability."""
        # Create setup
        config, brain, world, agent = pynlm.createCompleteSimulation()
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Run simulation multiple times with same seed
        times = []
        for run in range(3):
            # Reset for each run
            world.reset()
            
            # Run simulation
            for step in range(10):
                world.update(0.1)
            
            times.append(world.getSimulationTime())
        
        # All runs should have same final time (deterministic)
        assert len(times) == 3
        assert times[0] == times[1]
        assert times[1] == times[2]


class TestIntegrationErrorHandling:
    """Test integration error handling."""
    
    def test_invalid_configuration_handling(self):
        """Test handling of invalid configuration."""
        # Create configuration with invalid values
        config = pynlm.createDefaultConfig()
        
        # Set invalid values
        config.setString("negative_value", "-1")
        config.setFloat("invalid_range", 1000.0)
        
        # Create brain (should handle gracefully)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Brain should still be functional despite invalid config
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
    
    def test_missing_agent_configuration(self):
        """Test handling of missing agent configuration."""
        # Create minimal configuration
        config = pynlm.createDefaultConfig()
        config.createBrainConfig(neuronCount=100)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Create agent (should work with minimal config)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Verify agent works
        assert agent.getBrain() is not None
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
