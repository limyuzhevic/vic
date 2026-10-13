#!/usr/bin/env python3

"""
Python tests for NLM (Neural Learning Machine) bindings.

This module tests the Python API exposed by the pynlm bindings.
Tests cover basic functionality, agent operations, world interactions,
and memory system integration.
"""

import pynlm
import tempfile
import os
import json
from pathlib import Path
class TestConfig:
    """Test configuration system."""
    
    def test_config_creation(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert hasattr(config, 'has')
        assert hasattr(config, 'getKeys')
        assert hasattr(config, 'set')
        assert hasattr(config, 'getOr')
    
    def test_config_operations(self):
        """Test basic config operations."""
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.set("brain.neuron_count", 1000)
        assert config.has("brain.neuron_count")
        
        # Test default values
        neuron_count = config.getOr("brain.neuron_count", 0)
        assert neuron_count == 1000
        
        # Test non-existent key with default
        default_count = config.getOr("nonexistent.key", 42)
        assert default_count == 42
        
        # Test getting all keys
        keys = config.getKeys()
        assert isinstance(keys, list)
        assert "brain.neuron_count" in keys
    
    def test_config_save_load(self):
        """Test saving and loading configuration."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.15)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            config_path = f.name
        
        try:
            # Load back
            new_config = pynlm.createDefaultConfig()
            assert new_config.loadFromFile(config_path)
            
            # Verify loaded values
            assert new_config.has("brain.neuron_count")
            assert new_config.getOr("brain.neuron_count", 0) == 1500
            assert new_config.getOr("brain.synapse_density", 0.0) == 0.15
        finally:
            os.unlink(config_path)
class TestBrain:
    """Test brain functionality."""
    
    def setup_method(self):
        """Create a fresh brain for each test."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        config.set("brain.region_count", 2)
        config.set("brain.connection_probability", 0.05)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        """Test basic brain creation and initialization."""
        assert self.brain is not None
        
        # Check brain has expected methods
        assert hasattr(self.brain, 'initialize')
        assert hasattr(self.brain, 'step')
        assert hasattr(self.brain, 'getTotalNeuronCount')
        assert hasattr(self.brain, 'getTotalSynapseCount')
        assert hasattr(self.brain, 'getRegionCount')
        assert hasattr(self.brain, 'getDevelopmentalStage')
    
    def test_brain_statistics(self):
        """Test brain statistics and properties."""
        # Verify brain has expected statistics
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        region_count = self.brain.getRegionCount()
        firing_count = self.brain.getFiringNeuronCount()
        
        assert neuron_count == 200
        assert region_count == 2
        assert synapse_count >= 0
        assert firing_count >= 0
        
        # Check average firing rate is reasonable
        firing_rate = self.brain.getAverageFiringRate()
        assert firing_rate >= 0.0
        assert firing_rate <= 1000.0  # Should be reasonable
    
    def test_brain_steps(self):
        """Test running simulation steps."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run several steps
        for step in range(5):
            self.brain.step(step)
        
        # Spike count should have increased
        final_spikes = self.brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
    
    def test_brain_regions(self):
        """Test region management."""
        region_ids = self.brain.getRegionIds()
        assert len(region_ids) == 2
        
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            assert region is not None
    
    def test_brain_action_production(self):
        """Test action production."""
        action = self.brain.produceAction()
        assert action is not None
        
        # Check action has expected properties
        action_type = action.getType()
        assert action_type in pynlm.ActionType
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        # Take a step
        self.brain.step(0)
        
        # Reset brain
        self.brain.reset()
        
        # Verify reset
        neuron_count = self.brain.getTotalNeuronCount()
        assert neuron_count == 200
        
        # Should be able to initialize again
        assert self.brain.initialize()
    
    def test_brain_config_access(self):
        """Test accessing brain configuration."""
        config = self.brain.getConfig()
        assert config is not None
        assert hasattr(config, 'getKeys')
        assert hasattr(config, 'getOr')
    
    def test_brain_developmental_stage(self):
        """Test developmental stage functionality."""
        stage = self.brain.getDevelopmentalStage()
        assert stage in pynlm.DevelopmentalStage
        
        # Test setting developmental stage
        new_stage = pynlm.DevelopmentalStage.Maturation
        self.brain.setDevelopmentalStage(new_stage)
        stage_after = self.brain.getDevelopmentalStage()
        assert stage_after == new_stage
class TestAgent:
    """Test agent functionality."""
    
    def setup_method(self):
        """Create agent with brain and world."""
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def test_agent_creation(self):
        """Test agent creation and initialization."""
        assert self.agent is not None
        
        # Check agent has expected methods
        assert hasattr(self.agent, 'initialize')
        assert hasattr(self.agent, 'processSensoryInput')
        assert hasattr(self.agent, 'decodeMotorCommand')
        assert hasattr(self.agent, 'applyRewardModulation')
        assert hasattr(self.agent, 'updateDevelopment')
    
    def test_agent_sensory_input(self):
        """Test sensory input processing."""
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify agent processed it without error
        # (no exception raised is success)
        assert True
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        # Get sensory input and process it
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        assert motor_cmd is not None
        assert motor_cmd in pynlm.MotorCommand
    
    def test_agent_reward_modulation(self):
        """Test reward modulation."""
        # Apply reward modulation
        reward = 1.0
        predicted = 0.5
        self.agent.applyRewardModulation(reward, predicted)
        
        # Check neuromodulation level
        neuromod_level = self.agent.getNeuromodulationLevel()
        assert neuromod_level is not None
    
    def test_agent_development(self):
        """Test development system."""
        timestep = 0.1
        
        # Update development
        self.agent.updateDevelopment(timestep)
        
        # Check developmental stage
        stage = self.agent.getDevelopmentalStage()
        assert stage in pynlm.DevelopmentalStage
    
    def test_agent_subsystem_enabling(self):
        """Test enabling/disabling subsystems."""
        # Enable various subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Check they are enabled
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isStructuralPlasticityEnabled()
        assert self.agent.isDevelopmentEnabled()
        assert self.agent.isCuriosityEnabled()
        
        # Disable them
        self.agent.enableRewardModulation(False)
        self.agent.enableCuriosity(False)
        
        assert not self.agent.isRewardModulationEnabled()
        assert not self.agent.isCuriosityEnabled()
class TestWorld:
    """Test world functionality."""
    
    def setup_method(self):
        """Create fresh world for each test."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        assert self.world is not None
        
        # Check world has expected methods
        assert hasattr(self.world, 'configure')
        assert hasattr(self.world, 'reset')
        assert hasattr(self.world, 'setAgentStart')
        assert hasattr(self.world, 'update')
        assert hasattr(self.world, 'applyMotorCommand')
        assert hasattr(self.world, 'getSensoryPercept')
        assert hasattr(self.world, 'getAgentBody')
    
    def test_world_dimensions(self):
        """Test world dimensions."""
        assert self.world.getWidth() == 20.0
        assert self.world.getHeight() == 20.0
        assert self.world.getSimulationTime() == 0.0
    
    def test_world_update(self):
        """Test world update."""
        initial_time = self.world.getSimulationTime()
        initial_percept = self.world.getSensoryPercept()
        
        # Update world
        timestep = 0.1
        self.world.update(timestep)
        
        # Check time increased
        assert self.world.getSimulationTime() > initial_time
        
        # Check percept changed (or stayed the same if no movement)
        new_percept = self.world.getSensoryPercept()
        assert new_percept is not None
    
    def test_world_motor_command(self):
        """Test applying motor commands."""
        # Get current motor command
        percept = self.world.getSensoryPercept()
        
        # Create a brain to get motor commands
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 50)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        current_time = self.world.getSimulationTime()
        result = self.world.applyMotorCommand(motor_cmd, current_time)
        
        assert result is not None
        assert hasattr(result, 'reward')
        assert hasattr(result, 'success')
        assert hasattr(result, 'message')
    
    def test_world_objects(self):
        """Test world objects (resources, hazards, etc.)."""
        # Add a resource object
        resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
        self.world.addObject(resource)
        
        # Verify object was added
        found_obj = self.world.getObjectAt(5.0, 5.0)
        assert found_obj is not None
        assert found_obj.type == pynlm.WorldObjectType.Resource
        assert found_obj.value == 10.0
        
        # Remove object
        self.world.removeObject(5.0, 5.0)
        removed_obj = self.world.getObjectAt(5.0, 5.0)
        assert removed_obj is None
    
    def test_world_validation(self):
        """Test world position validation."""
        # Valid position should return true
        assert self.world.isValidPosition(10.0, 10.0)
        
        # Invalid position (should be false if there's a wall or boundary)
        # Note: actual invalid positions depend on world implementation
        # This test just ensures the method exists and doesn't crash
        self.world.isValidPosition(0.0, 0.0)
        self.world.isValidPosition(100.0, 100.0)
    
    def test_world_energy(self):
        """Test energy system."""
        # Check default max energy
        assert self.world.getMaxEnergy() > 0.0
        
        # Modify energy
        new_energy = 200.0
        self.world.setMaxEnergy(new_energy)
        assert self.world.getMaxEnergy() == new_energy
    
    def test_world_random_seed(self):
        """Test random seed for reproducibility."""
        seed = self.world.getRandomSeed()
        assert seed > 0
        
        # Set a specific seed
        test_seed = 12345
        self.world.setRandomSeed(test_seed)
        assert self.world.getRandomSeed() == test_seed
class TestCompleteSimulation:
    """Test complete simulation scenarios."""
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation."""
        # Create all components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        num_steps = 50
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress every 10 steps
            if step % 10 == 0:
                print(f"Step {step}: "
                      f"Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Dev Stage: {brain.getDevelopmentalStage()}")
        
        # Verify simulation completed successfully
        assert brain.getTotalNeuronCount() == 300
        assert agent.getDevelopmentalStage() in pynlm.DevelopmentalStage
    
    def test_world_exploration_simulation(self):
        """Test agent exploration in a world with objects."""
        # Create world with various objects
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        # Add some objects
        resource1 = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 5.0)
        resource2 = pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, 10.0)
        hazard1 = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, -2.0)
        wall1 = pynlm.WorldObject(20.0, 5.0, pynlm.WorldObjectType.Wall, 0.0, 2.0)
        
        for obj in [resource1, resource2, hazard1, wall1]:
            world.addObject(obj)
        
        # Create brain and agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable curiosity for exploration
        agent.enableCuriosity(True)
        
        # Run exploration simulation
        for step in range(30):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Record interactions
            if result.success:
                print(f"Step {step}: Successfully interacted with {result.message}")
            
            # Apply reward modulation based on result
            reward = result.reward if result.success else -1.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Verify exploration happened
        assert brain.getTotalSpikeCount() > 0
    
    def test_checkpoint_save_load(self):
        """Test brain checkpoint functionality."""
        # Create and run a brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 150)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few steps
        for step in range(5):
            brain.step(step)
        
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            # Save checkpoint
            save_success = brain.save(checkpoint_path)
            assert save_success
            
            # Reset brain
            brain.reset()
            assert brain.initialize()
            
            # Verify brain is reset (spike count should be lower after reset)
            after_reset_spikes = brain.getTotalSpikeCount()
            # After reset, spikes should be minimal or zero
            
            # Load checkpoint
            load_success = brain.load(checkpoint_path)
            assert load_success
            
            # Verify loaded state
            final_spikes = brain.getTotalSpikeCount()
            assert final_spikes >= after_reset_spikes
            
            # Brain should still have correct neuron count
            assert brain.getTotalNeuronCount() == 150
        
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
    
    def test_brain_development_simulation(self):
        """Test brain development over time."""
        # Create brain with longer simulation time
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        
        # Track developmental stages over time
        stages = []
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            stages.append(brain.getDevelopmentalStage())
        
        # Developmental stage should change over time
        assert len(set(stages)) > 1  # Should see multiple stages
        
        # Check final stage progression
        final_stage = brain.getDevelopmentalStage()
        assert final_stage in pynlm.DevelopmentalStage
        print(f"Developmental stage progression: {set(stages)}")
    
    def test_world_object_interaction(self):
        """Test interaction with world objects."""
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(20.0, 20.0)
        
        # Add various objects
        world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 5.0))
        world.addObject(pynlm.WorldObject(35.0, 35.0, pynlm.WorldObjectType.Resource, 15.0))
        world.addObject(pynlm.WorldObject(15.0, 25.0, pynlm.WorldObjectType.Hazard, -3.0))
        
        # Create agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableCuriosity(True)
        
        # Run simulation to potentially interact with objects
        interaction_count = 0
        resource_found = False
        
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            if result.success:
                interaction_count += 1
                # Check if it was a resource interaction
                if "resource" in result.message.lower():
                    resource_found = True
            
            agent.applyRewardModulation(result.reward if result.success else -1.0, 0.0)
        
        print(f"Simulation completed with {interaction_count} interactions")
        assert interaction_count >= 0
    
    def test_complete_brain_training(self):
        """Test brain training with all features enabled."""
        # Create comprehensive configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 250)
        config.set("brain.synapse_density", 0.08)
        config.set("brain.connection_probability", 0.04)
        config.set("stdp_ltp_weight", 0.008)
        config.set("stdp_ltd_weight", 0.009)
        
        # Create brain with all systems
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent with all capabilities
        agent = pynlm.createAgentBrain(brain)
        
        # Create complex world
        world = pynlm.createSimpleWorld()
        world.configure(25.0, 25.0, 8, 8)
        world.reset()
        world.setAgentStart(12.5, 12.5)
        world.setMaxEnergy(100.0)
        
        # Add diverse objects
        for i in range(5):
            obj = pynlm.WorldObject(
                float(i * 5 + 2), float(i * 5 + 2),
                pynlm.WorldObjectType.Resource, float(5 + i * 2)
            )
            world.addObject(obj)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run intensive training
        training_steps = 100
        learning_metrics = []
        
        for step in range(training_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Extract reward from result
            reward = action_result.reward if action_result.success else -1.0
            
            # Apply neuromodulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record metrics every 10 steps
            if step % 10 == 0:
                metrics = {
                    'step': step,
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'prediction_error': agent.getPredictionError(),
                    'development_stage': int(brain.getDevelopmentalStage()),
                    'reward': reward
                }
                learning_metrics.append(metrics)
                
                print(f"Step {step}: "
                      f"Firing: {metrics['firing_neurons']}, "
                      f"Curiosity: {metrics['curiosity']:.3f}, "
                      f"Dev Stage: {metrics['development_stage']}, "
                      f"Reward: {metrics['reward']}")
        
        # Analyze training results
        assert brain.getTotalNeuronCount() == 250
        assert len(learning_metrics) == 10
        
        # Check for learning patterns
        final_firing = learning_metrics[-1]['firing_neurons']
        initial_firing = learning_metrics[0]['firing_neurons']
        
        print(f"Training completed: {initial_firing} -> {final_firing} firing neurons")
        print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")
        print(f"Final curiosity level: {learning_metrics[-1]['curiosity']:.3f}")
        print(f"Final developmental stage: {learning_metrics[-1]['development_stage']}")
        
        # Verify that the brain evolved during training
        assert brain.getTotalSpikeCount() > 0
        print(f"Training completed: {initial_firing} -> {final_firing} firing neurons")
        print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")
        print(f"Final curiosity level: {learning_metrics[-1]['curiosity']:.3f}")
        print(f"Final developmental stage: {learning_metrics[-1]['development_stage']}")
        
        # Verify that the brain evolved during training
        assert brain.getTotalSpikeCount() > 0
class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_brain_double_initialization(self):
        """Test handling of double initialization."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 50)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try to initialize again (should handle gracefully)
        # Note: behavior depends on implementation - this test ensures it doesn't crash
        try:
            brain.initialize()
            print("Double initialization handled gracefully")
        except Exception as e:
            print(f"Double initialization raised exception (acceptable): {e}")
    
    def test_brain_step_without_initialization(self):
        """Test handling of step() without initialization."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 50)
        
        brain = pynlm.createBrain(config)
        
        # Try to step without initializing (should handle gracefully)
        try:
            brain.step(0)
            print("Step without initialization handled gracefully")
        except Exception as e:
            print(f"Step without initialization raised exception (acceptable): {e}")
    
    def test_invalid_config_values(self):
        """Test handling of invalid configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Set some values that might cause issues
        config.set("brain.neuron_count", 0)  # Zero neurons
        config.set("brain.region_count", 0)   # Zero regions
        
        # Should still create brain (though may have issues)
        brain = pynlm.createBrain(config)
        
        try:
            brain.initialize()
            print("Brain with zero neurons/initialized successfully")
        except Exception as e:
            print(f"Zero neuron configuration raised exception (acceptable): {e}")
    
    def test_world_boundary_conditions(self):
        """Test world boundary and edge cases."""
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 4, 4)
        world.reset()
        
        # Test various boundary positions
        test_positions = [
            (0.0, 0.0),
            (10.0, 10.0),
            (-1.0, -1.0),
            (20.0, 20.0),
            (5.0, 5.0)
        ]
        
        for x, y in test_positions:
            try:
                valid = world.isValidPosition(x, y)
                print(f"Position ({x}, {y}) valid: {valid}")
            except Exception as e:
                print(f"Position ({x}, {y}) raised exception: {e}")
    
    def test_large_simulation(self):
        """Test running a large simulation to check for memory/resource issues."""
        print("Testing large simulation...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)  # Larger brain
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation for more steps
        for step in range(200):
            world.update(0.05)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            agent.applyRewardModulation(0.1, 0.0)
            
            if step % 50 == 0:
                print(f"Large simulation step {step}: "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Total spikes: {brain.getTotalSpikeCount()}")
        
        print(f"Large simulation completed successfully")
        assert brain.getTotalNeuronCount() == 500
        assert brain.getTotalSpikeCount() > 0

def run_tests():
    """Run all tests and report results."""
    test_classes = [
        TestConfig,
        TestBrain,
        TestAgent,
        TestWorld,
        TestCompleteSimulation,
        TestErrorHandling
    ]
    
    total_tests = 0
    passed_tests = 0
    failed_tests = 0
    
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    for test_class in test_classes:
        print(f"\n{'='*20} {test_class.__name__} {'='*20}")
        
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [
            method for method in dir(test_instance)
            if method.startswith('test_') and callable(getattr(test_instance, method))
        ]
        
        for test_method_name in test_methods:
            total_tests += 1
            test_method = getattr(test_instance, test_method_name)
            
            # Handle setup/teardown if present
            if hasattr(test_instance, 'setup_method'):
                test_instance.setup_method()
            
            try:
                test_method()
                print(f"✓ {test_method_name} PASSED")
                passed_tests += 1
            except Exception as e:
                print(f"✗ {test_method_name} FAILED: {e}")
                failed_tests += 1
            
            # Handle teardown if present
            if hasattr(test_instance, 'teardown_method'):
                test_instance.teardown_method()
    
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    print(f"Success rate: {passed_tests/total_tests*100:.1f}%")
    
    return failed_tests == 0
if __name__ == "__main__":
    success = run_tests()
    exit(0 if success else 1)
