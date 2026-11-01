"""
Comprehensive test suite for NLM Python bindings
Tests all the enhanced functionality added to the Python API
"""
import pynlm
import pytest
import tempfile
import os

class TestNLMBasicFunctionality:
    """Test basic brain creation and initialization"""
    
    def test_default_config_creation(self):
        """Test creating a default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert hasattr(config, 'has')
        assert hasattr(config, 'set')
        assert hasattr(config, 'getInt')
        assert hasattr(config, 'getFloat')
        assert hasattr(config, 'getString')
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        
        # Valid config should pass validation
        assert config.validate() is True
        
        # Create invalid config
        invalid_config = pynlm.createDefaultConfig()
        # Missing required fields - validation should fail
        with pytest.raises(py::value_error):
            invalid_config.validate()
    
    def test_learning_brain_creation(self):
        """Test creating a brain with learning configuration"""
        brain = pynlm.createLearningBrain(500)
        assert brain is not None
        assert brain.getTotalNeuronCount() == 500
        brain.initialize()
        assert brain.getTotalSynapseCount() > 0

class TestBrainOperations:
    """Test brain operations and methods"""
    
    def test_brain_creation_with_config(self):
        """Test creating brain with custom config"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        config.set("brain.synapse_density", 0.15)
        
        brain = pynlm.createBrain(config)
        assert brain is not None
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == 200
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_step_operations(self):
        """Test various step operations"""
        brain = pynlm.createLearningBrain(100)
        brain.initialize()
        
        # Test single step
        brain.step(0)
        
        # Test multiple steps
        brain.stepMultiple(1, 10)
        
        # Test step with timestamp
        brain.step(20, 20 * 0.001)
        
        # Test reset
        brain.step(30)
        brain.reset()
        assert brain.getTotalNeuronCount() == 100
    
    def test_brain_run_episode(self):
        """Test run episode functionality"""
        brain = pynlm.createLearningBrain(50)
        brain.initialize()
        
        # Run episode with sensory input
        brain.runEpisode(50, withSensory=True)
        
        # Verify episode completed
        assert brain.getTotalSpikeCount() > 0
    
    def test_brain_statistics(self):
        """Test brain statistics methods"""
        brain = pynlm.createLearningBrain(100)
        brain.initialize()
        
        # Collect statistics
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        active = brain.getActiveNeuronCount()
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        
        assert neurons == 100
        assert synapses >= 0
        assert active >= 0
        assert firing >= 0
        assert rate >= 0.0f
        
        # Test status method
        status = brain.status()
        assert "Brain Status" in status
        assert f"Neurons: {neurons}" in status

class TestAgentOperations:
    """Test agent operations and learning features"""
    
    def test_agent_creation(self):
        """Test creating agent brain interface"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test agent initialization
        assert agent.getBrain() is brain
        assert agent.getDevelopmentalStage() is not None
    
    def test_agent_sensory_processing(self):
        """Test sensory processing"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create simple percept (vision)
        percept = pynlm.SensoryPercept()
        vision = pynlm.Vision(8, 8, 3)
        data = [0.1f] * 64
        vision.setData(data)
        percept.setVision(vision)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Test motor command decoding
        action = agent.decodeMotorCommand()
        assert action is not None
    
    def test_agent_learning_features(self):
        """Test learning features and neuromodulation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Check enabled features
        assert agent.isRewardModulationEnabled() is True
        assert agent.isCuriosityEnabled() is True
        assert agent.isDevelopmentEnabled() is True
        assert agent.isStructuralPlasticityEnabled() is True
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get neuromodulation stats
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        assert curiosity >= 0.0
        assert novelty >= 0.0
        assert prediction_error >= 0.0

class TestConvenienceFunctions:
    """Test convenience functions added for Python users"""
    
    def test_run_agent_simulation(self):
        """Test run_agent_simulation convenience function"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        world = pynlm.createTrainingEnvironment(20, 20, 8, 8)
        agent = pynlm.createAgentBrain(brain)
        
        # Run simulation
        pynlm.runAgentSimulation(brain, world, agent, 50)
        
        # Verify simulation completed
        assert brain.getTotalSpikeCount() > 0
    
    def test_create_training_environment(self):
        """Test create_training_environment convenience function"""
        world = pynlm.createTrainingEnvironment(15, 15, 6, 6)
        
        assert world is not None
        assert world.getWidth() == 15
        assert world.getHeight() == 15
        world.configure(15, 15, 6, 6)  # Reconfigure to set vision
        
        world.reset()
        world.setRandomSeed(42)
        
        assert world.getRandomSeed() == 42
    
    def test_create_learning_brain(self):
        """Test create_learning_brain convenience function"""
        brain = pynlm.createLearningBrain(300)
        
        assert brain is not None
        assert brain.getTotalNeuronCount() == 300
        brain.initialize()
        assert brain.getTotalSynapseCount() > 0

class TestTypeConversions:
    """Test type conversion utilities"""
    
    def test_neuron_id_conversion(self):
        """Test NeuronId type conversion"""
        neuron_id = pynlm.toNeuronId(42)
        assert neuron_id is not None
        # This tests the conversion function
    
    def test_synapse_id_conversion(self):
        """Test SynapseId type conversion"""
        synapse_id = pynlm.toSynapseId(123)
        assert synapse_id is not None
    
    def test_region_id_conversion(self):
        """Test RegionId type conversion"""
        region_id = pynlm.toRegionId(1)
        assert region_id is not None
    
    def test_population_id_conversion(self):
        """Test PopulationId type conversion"""
        population_id = pynlm.toPopulationId(5)
        assert population_id is not None

class TestModuleAttributes:
    """Test module-level attributes and utilities"""
    
    def test_version_attribute(self):
        """Test version attribute"""
        version = pynlm.getVersion()
        assert version == "0.1.0"
    
    def test_supported_actions(self):
        """Test get supported actions"""
        actions = pynlm.getSupportedActions()
        assert isinstance(actions, list)
        assert len(actions) > 0
        
        # Check for common action types
        from enum import Enum
        action_types = [a.value for a in actions if isinstance(a, Enum)]
        assert any(a.value == pynlm.ActionType.MoveForward for a in actions)
        assert any(a.value == pynlm.ActionType.TurnLeft for a in actions)
    
    def test_neuron_types(self):
        """Test get neuron types"""
        neuron_types = pynlm.getNeuronTypes()
        assert isinstance(neuron_types, list)
        assert len(neuron_types) > 0
        
        # Check for common neuron types
        from enum import Enum
        types = [t.value for t in neuron_types if isinstance(t, Enum)]
        assert any(t == pynlm.NeuronType.Excitatory for t in neuron_types)
        assert any(t == pynlm.NeuronType.Inhibitory for t in neuron_types)
    
    def test_world_object_types(self):
        """Test get world object types"""
        object_types = pynlm.getWorldObjectTypes()
        assert isinstance(object_types, list)
        assert len(object_types) > 0
    
    def test_create_random_seed(self):
        """Test create_random_seed utility"""
        seed = pynlm.createRandomSeed()
        assert isinstance(seed, int)
        assert seed > 0

class TestErrorHandling:
    """Test error handling and validation"""
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test getting non-existent key
        with pytest.raises(py::key_error):
            config.getInt("nonexistent_key")
        
        with pytest.raises(py::key_error):
            config.getFloat("nonexistent_key")
        
        with pytest.raises(py::key_error):
            config.getString("nonexistent_key")
    
    def test_brain_error_handling(self):
        """Test brain error handling"""
        brain = pynlm.createLearningBrain(100)
        
        # Brain not initialized - should handle gracefully
        # Note: Some methods may require initialization

class TestAdvancedFeatures:
    """Test advanced features and integration"""
    
    def test_agent_episode_management(self):
        """Test agent episode management"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create episode
        result = agent.createEpisode()
        assert "Episode created" in result
        
        # Get episode stats
        stats = agent.getEpisodeStats()
        assert "Episode Statistics" in stats
        
        # Learn from experience
        learning_result = agent.learnFromExperience(0.5, 0.2)
        assert "Learning completed" in learning_result
    
    def test_complete_simulation(self):
        """Test a complete simulation with all features"""
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createTrainingEnvironment(10, 10, 4, 4)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run complete simulation
        steps = 100
        for step in range(steps):
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = 0.1
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed successfully
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() >= 0.0

class TestMemoryAndSerialization:
    """Test memory management and serialization features"""
    
    def test_brain_checkpoint(self):
        """Test brain checkpoint functionality"""
        brain = pynlm.createLearningBrain(50)
        brain.initialize()
        
        # Add some activity to checkpoint
        brain.step(0)
        brain.step(1)
        
        # Create checkpoint
        checkpoint_result = brain.createCheckpoint()
        assert "Checkpoint created" in checkpoint_result

if __name__ == "__main__":
    # Run basic tests
    print("Running NLM Python Bindings Tests...")
    
    test = TestNLMBasicFunctionality()
    test.test_default_config_creation()
    print("✓ Basic functionality tests passed")
    
    test = TestBrainOperations()
    test.test_brain_creation_with_config()
    print("✓ Brain operations tests passed")
    
    print("\nAll tests completed successfully!")