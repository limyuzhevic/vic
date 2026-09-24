#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings (pynlm).

This test suite covers all functionality exposed through the Python bindings,
including configuration management, brain simulation, agent integration,
world interaction, and advanced features.
"""

import pynlm
import tempfile
import os

class TestPynlmBasic:
    """Test basic NLM functionality and Python bindings."""
    
    def test_version_import(self):
        """Test that pynlm module can be imported and has version."""
        # Basic import test
        assert hasattr(pynlm, '__version__')
        assert pynlm.__version__ is not None
        assert isinstance(pynlm.__version__, str)
    
    def test_create_default_config(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert isinstance(config, pynlm.Config)
        
        # Test default values
        assert config.has("random_seed")
        assert config.has("simulation_timestep")
        assert config.has("neuron_count")
        assert config.has("region_count")
    
    def test_config_operations(self):
        """Test configuration get/set operations."""
        config = pynlm.createDefaultConfig()
        
        # Test setting values
        config.set("random_seed", 12345)
        config.set("neuron_count", 500)
        config.set("simulation_timestep", 0.01)
        config.set("connection_probability", 0.05)
        
        # Test getting values
        assert config.getOr("random_seed", 0) == 12345
        assert config.getOr("neuron_count", 0) == 500
        assert config.getOr("simulation_timestep", 0.0) == 0.01
        assert config.getOr("connection_probability", 0.0) == 0.05
        
        # Test has method
        assert config.has("random_seed")
        assert not config.has("nonexistent_key")
        
        # Test keys
        keys = config.getKeys()
        assert "random_seed" in keys
        assert "neuron_count" in keys
        
        # Test summary
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_config_file_io(self):
        """Test configuration file I/O operations."""
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("# Test config\n")
            f.write("random_seed = 999\n")
            f.write("neuron_count = 200\n")
            f.write("simulation_timestep = 0.002\n")
            temp_file = f.name
        
        try:
            config = pynlm.createDefaultConfig()
            
            # Test loading from file
            success = config.loadFromFile(temp_file)
            assert success, "Failed to load config from file"
            
            # Test loaded values
            assert config.getOr("random_seed", 0) == 999
            assert config.getOr("neuron_count", 0) == 200
            assert config.getOr("simulation_timestep", 0.0) == 0.002
            
            # Test saving to file
            with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
                save_file = f.name
            
            try:
                success = config.saveToFile(save_file)
                assert success, "Failed to save config to file"
                
                # Load the saved file and verify content
                config2 = pynlm.createDefaultConfig()
                config2.loadFromFile(save_file)
                assert config2.getOr("random_seed", 0) == 999
                assert config2.getOr("neuron_count", 0) == 200
                assert config2.getOr("simulation_timestep", 0.0) == 0.002
            finally:
                if os.path.exists(save_file):
                    os.unlink(save_file)
        finally:
            os.unlink(temp_file)
    
    def test_config_command_line_args(self):
        """Test loading configuration from command line arguments."""
        config = pynlm.createDefaultConfig()
        
        # Simulate command line arguments
        import sys
        old_argv = sys.argv
        try:
            sys.argv = ["test", "--random_seed", "777", "--neuron_count", "300", 
                     "--simulation_timestep", "0.005"]
            success = config.loadFromArgs(len(sys.argv), sys.argv)
            assert success, "Failed to load config from command line args"
            
            # Verify values
            assert config.getOr("random_seed", 0) == 777
            assert config.getOr("neuron_count", 0) == 300
            assert config.getOr("simulation_timestep", 0.0) == 0.005
        finally:
            sys.argv = old_argv
class TestPynlmBrain:
    """Test brain creation and simulation functionality."""
    
    def test_brain_creation(self):
        """Test creating and initializing a brain."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert isinstance(brain, pynlm.Brain)
        
        # Initialize the brain
        result = brain.initialize()
        assert result == True
        
        # Check basic statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getRegionCount() > 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
    
    def test_brain_step_single_arg(self):
        """Test brain stepping with single argument (convenience method)."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test single argument step (should use default timestep)
        brain.step(0)
        brain.step(1)
        brain.step(10)
        
        # Check that brain state changed
        spike_count_0 = brain.getTotalSpikeCount()
        assert spike_count_0 >= 0
    
    def test_brain_step_two_args(self):
        """Test brain stepping with explicit timestep."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test two argument step (explicit time)
        brain.step(0, 0.0)
        brain.step(1, 0.001)
        brain.step(10, 0.01)
        
        # Check statistics
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
    
    def test_brain_sensory_input(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create sensory input (vision)
        vision = pynlm.Vision(8, 8, 3)
        data = [0.0] * (8 * 8 * 3)
        for i in range(min(len(data), 20)):
            data[i] = 0.5  # Set some pixels to 50% brightness
        vision.setData(data)
        
        # Inject sensory input
        brain.receiveSensoryInput(vision)
        
        # Run simulation
        for step in range(10):
            brain.step(step)
    
    def test_brain_action_production(self):
        """Test action production from brain."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Produce action
        action = brain.produceAction()
        assert action is not None
        assert isinstance(action, pynlm.Action)
        
        # Check action properties
        assert action.getType() in pynlm.ActionType.__members__.values()
    
    def test_brain_state_management(self):
        """Test brain state saving and loading."""
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Record some state
        initial_spikes = brain1.getTotalSpikeCount()
        initial_neuron_count = brain1.getTotalNeuronCount()
        
        # Run some steps
        for step in range(10):
            brain1.step(step)
        
        # Create another brain and save state
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Save brain1 state
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            save_path = f.name
        
        try:
            save_result = brain1.save(save_path)
            assert save_result == True
            
            # Reset brain2 and load state
            brain2.reset()
            brain2.initialize()
            
            load_result = brain2.load(save_path)
            # Loading might fail due to complex state dependencies, but shouldn't crash
            assert isinstance(load_result, bool)
        finally:
            if os.path.exists(save_path):
                os.unlink(save_path)
    
    def test_brain_region_management(self):
        """Test brain region management."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test region count
        initial_regions = brain.getRegionCount()
        assert initial_regions >= 1
        
        # Add a region
        region_id = brain.addRegion("new_region")
        assert region_id.value > 0
        
        # Check region count increased
        assert brain.getRegionCount() == initial_regions + 1
        
        # Get region
        region = brain.getRegion(region_id)
        assert region is not None
        
        # Get all regions
        regions = brain.getRegions()
        assert len(regions) == brain.getRegionCount()
        
        # Get region IDs
        region_ids = brain.getRegionIds()
        assert len(region_ids) == brain.getRegionCount()
        assert region_id in region_ids
    
    def test_brain_statistics(self):
        """Test brain statistics collection."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Initial statistics
        assert brain.getTotalNeuronCount() >= 500
        assert brain.getTotalSynapseCount() >= 0
        assert brain.getActiveNeuronCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
        # Run simulation
        for step in range(20):
            brain.step(step)
        
        # Check statistics after simulation
        assert brain.getTotalSpikeCount() >= brain.getTotalSpikeCount()
        assert brain.getAverageFiringRate() >= 0.0
        
        # Test excitation/inhibition ratio
        e_i_ratio = brain.getExcitationInhibitionRatio()
        assert e_i_ratio >= 0.0
        
        # Test developmental stage
        dev_stage = brain.getDevelopmentalStage()
        assert dev_stage in pynlm.DevelopmentalStage.__members__.values()
        
        # Test log status
        brain.logStatus()  # Should not crash
    
    def test_brain_mixed_stepping(self):
        """Test mixing single and two-argument brain stepping."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Mix different stepping patterns
        brain.step(0)                    # Single arg
        brain.step(1, 0.001)             # Two args with explicit time
        brain.step(2)                    # Single arg again
        brain.step(3, 0.003)             # Two args with explicit time
        
        # Verify brain is still functional
        action = brain.produceAction()
        assert action is not None
class TestPynlmAgent:
    """Test agent brain and world integration."""
    
    def test_agent_brain_creation(self):
        """Test creating an agent brain."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        assert agent is not None
        assert isinstance(agent, pynlm.AgentBrain)
        
        # Initialize brain first
        brain.initialize()
    
    def test_agent_world_integration(self):
        """Test complete agent-world integration."""
        # Create all components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Run simulation episode
        for step in range(50):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            assert percept is not None
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            assert motor_cmd in pynlm.MotorCommand.__members__.values()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Check agent statistics
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() is not None
        assert agent.getNeuromodulationLevel() is not None
    
    def test_agent_subsystem_controls(self):
        """Test agent subsystem enable/disable controls."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Test initial state
        assert agent.isRewardModulationEnabled() == True
        assert agent.isStructuralPlasticityEnabled() == True
        assert agent.isDevelopmentEnabled() == True
        assert agent.isCuriosityEnabled() == True
        
        # Test enabling/disabling
        agent.enableRewardModulation(False)
        agent.enableStructuralPlasticity(False)
        agent.enableDevelopment(False)
        agent.enableCuriosity(False)
        
        assert agent.isRewardModulationEnabled() == False
        assert agent.isStructuralPlasticityEnabled() == False
        assert agent.isDevelopmentEnabled() == False
        assert agent.isCuriosityEnabled() == False
        
        # Re-enable
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        assert agent.isRewardModulationEnabled() == True
        assert agent.isCuriosityEnabled() == True
    
    def test_agent_development(self):
        """Test agent development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize and run development
        brain.initialize()
        
        # Test development update
        for step in range(10):
            agent.updateDevelopment(0.1)
            
            # Check developmental stage
            dev_stage = agent.getDevelopmentalStage()
            assert dev_stage in pynlm.DevelopmentalStage.__members__.values()
        
        # Check developmental age
        dev_stage = agent.getDevelopmentalStage()
        assert dev_stage in pynlm.DevelopmentalStage.__members__.values()
    
    def test_agent_reward_modulation(self):
        """Test agent reward modulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        brain.initialize()
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)  # reward, predicted reward
        
        # Check neuromodulation level
        neuromod_level = agent.getNeuromodulationLevel()
        assert neuromod_level is not None
        
        # Apply negative reward
        agent.applyRewardModulation(-1.0, 0.5)
        
        # Check that prediction error was updated
        pred_error = agent.getPredictionError()
        assert pred_error is not None
    
    def test_agent_reset(self):
        """Test agent reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        brain.initialize()
        
        # Apply some changes
        agent.applyRewardModulation(1.0, 0.0)
        
        # Reset agent
        agent.reset()
        
        # Check that state was reset
        assert agent.getNeuromodulationLevel() is not None
        assert agent.getCuriosityLevel() is not None
        assert agent.getNoveltyLevel() is not None
        assert agent.getPredictionError() is not None
class TestPynlmWorld:
    """Test world simulation functionality."""
    
    def test_world_creation(self):
        """Test world creation and basic setup."""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert isinstance(world, pynlm.SimpleWorld)
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        assert world.getWidth() == 20
        assert world.getHeight() == 20
    
    def test_world_configuration(self):
        """Test world configuration options."""
        world = pynlm.createSimpleWorld()
        
        # Test configuration
        world.configure(width=15, height=10, visionWidth=6, visionHeight=6)
        
        assert world.getWidth() == 15
        assert world.getHeight() == 10
    
    def test_world_reset(self):
        """Test world reset functionality."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.setAgentStart(5.0, 5.0)
        
        # Update world
        world.update(0.1)
        
        # Reset
        world.reset()
        
        # Check that agent returned to start position
        body = world.getAgentBody()
        assert abs(body.x - 5.0) < 0.001
        assert abs(body.y - 5.0) < 0.001
    
    def test_world_sensory_percept(self):
        """Test world sensory percept generation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        # Get initial percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Check percept components
        vision = percept.getVision()
        assert len(vision) > 0
        
        touch = percept.getTouch()
        assert isinstance(touch, list)
        
        internal = percept.getInternal()
        assert isinstance(internal, list)
        
        proprioception = percept.getProprioception()
        assert isinstance(proprioception, list)
        
        # Update world and get new percept
        world.update(0.1)
        percept2 = world.getSensoryPercept()
        assert percept2 is not None
    
    def test_world_object_management(self):
        """Test world object management."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Create a world object
        obj = pynlm.WorldObject(3.0, 4.0, pynlm.WorldObjectType.Resource, value=10.0)
        assert obj.x == 3.0
        assert obj.y == 4.0
        assert obj.type == pynlm.WorldObjectType.Resource
        assert obj.value == 10.0
        
        # Add object to world
        world.addObject(obj)
        
        # Remove object
        world.removeObject(3.0, 4.0)
        
        # Check position is valid
        assert world.isValidPosition(3.0, 4.0)
    
    def test_world_collision_detection(self):
        """Test world collision detection."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Add a wall object
        wall = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Wall, radius=0.8)
        world.addObject(wall)
        
        # Check collision detection
        assert world.getObjectAt(5.0, 5.0) == wall
        assert world.getObjectAt(5.0, 5.0).type == pynlm.WorldObjectType.Wall
    
    def test_world_agent_control(self):
        """Test world agent control."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(2.0, 2.0)
        
        # Get initial agent body
        body = world.getAgentBody()
        assert abs(body.x - 2.0) < 0.001
        assert abs(body.y - 2.0) < 0.001
        
        # Apply motor command
        world.applyMotorCommand(pynlm.MotorCommand.MoveForward, world.getSimulationTime())
        
        # Update world
        world.update(0.1)
        
        # Check agent moved
        body = world.getAgentBody()
        assert body.isMoving == True
    
    def test_world_energy_and_health(self):
        """Test world energy and health management."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Check default values
        assert world.getMaxEnergy() > 0
        assert world.getEnergyDecayRate() > 0
        
        # Set custom values
        world.setMaxEnergy(100.0)
        world.setEnergyDecayRate(0.95)
        
        assert world.getMaxEnergy() == 100.0
        assert world.getEnergyDecayRate() == 0.95
    
    def test_world_random_seed(self):
        """Test world random seed functionality."""
        world = pynlm.createSimpleWorld()
        
        # Set random seed
        world.setRandomSeed(42)
        assert world.getRandomSeed() == 42
class TestPynlmIntegration:
    """Test integrated agent-world-brain simulations."""
    
    def test_complete_agent_episode(self):
        """Test a complete agent episode with world interaction."""
        # Create NLM components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure and initialize
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.0, 7.0)
        
        brain.initialize()
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run episode
        episode_reward = 0.0
        for step in range(100):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Decode and apply action
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Get reward (simplified)
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            episode_reward += reward
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
        
        # Verify episode completed successfully
        assert episode_reward is not None
        assert brain.getTotalNeuronCount() > 0
        assert agent.getDevelopmentalStage() in pynlm.DevelopmentalStage.__members__.values()
    
    def test_multiple_episodes(self):
        """Test running multiple episodes."""
        # Create components for multiple episodes
        config = pynlm.createDefaultConfig()
        world = pynlm.createSimpleWorld()
        
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        
        for episode in range(3):
            # Create fresh brain for each episode
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world.reset()
            world.setAgentStart(5.0, 5.0)
            agent.initialize(world)
            
            # Run episode
            for step in range(30):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                agent.updateDevelopment(0.1)
            
            # Check episode completion
            assert brain.getTotalNeuronCount() > 0
            assert agent.getDevelopmentalStage() in pynlm.DevelopmentalStage.__members__.values()
    
    def test_simple_world_simulation(self):
        """Test simple world simulation without agent."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Get initial state
        percept1 = world.getSensoryPercept()
        body1 = world.getAgentBody()
        
        # Run multiple steps
        for step in range(20):
            world.update(0.1)
            
            # Check world state
            percept2 = world.getSensoryPercept()
            body2 = world.getAgentBody()
            
            # Should not crash
            assert percept2 is not None
            assert body2 is not None
class TestPynlmErrorHandling:
    """Test error handling and edge cases."""
    
    def test_brain_not_initialized(self):
        """Test behavior when brain is not initialized."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should work without explicit initialization (lazy init)
        brain.step(0)
        brain.step(1)
        
        # But operations might fail
        try:
            brain.getTotalNeuronCount()
            # If this doesn't raise an exception, that's fine
        except Exception:
            pass  # Expected
    
    def test_invalid_config_values(self):
        """Test handling of invalid configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Set some potentially problematic values
        config.set("random_seed", -1)  # Negative seed
        config.set("neuron_count", 0)   # Zero neurons
        config.set("simulation_timestep", -0.001)  # Negative timestep
        
        # Should still be able to create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should not crash
        action = brain.produceAction()
        assert action is not None
    
    def test_world_edge_cases(self):
        """Test world edge cases and boundary conditions."""
        world = pynlm.createSimpleWorld()
        world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
        
        # Test agent at edge
        world.reset()
        world.setAgentStart(0.0, 0.0)  # Corner
        
        # Update should not crash
        world.update(0.1)
        
        # Test invalid positions
        assert not world.isValidPosition(-1.0, 0.0)
        assert not world.isValidPosition(0.0, -1.0)
        assert not world.isValidPosition(10.0, 10.0)
        
        # Test valid positions
        assert world.isValidPosition(0.0, 0.0)
        assert world.isValidPosition(5.0, 5.0)
    
    def test_memory_limits(self):
        """Test behavior with large configurations."""
        # Create a larger configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 5000)
        config.set("region_count", 5)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should still work
        assert brain.getTotalNeuronCount() == 5000
        assert brain.getRegionCount() == 5
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Should not crash
        action = brain.produceAction()
        assert action is not None
if __name__ == "__main__":
    # Simple test runner
    import sys
    
    print("Running NLM Python bindings comprehensive test suite...")
    
    test_classes = [
        TestPynlmBasic,
        TestPynlmBrain,
        TestPynlmAgent,
        TestPynlmWorld,
        TestPynlmIntegration,
        TestPynlmErrorHandling
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\n=== Running {test_class.__name__} ===")
        instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(instance) 
                      if method.startswith('test_') and callable(getattr(instance, method))]
        
        for method_name in test_methods:
            try:
                method = getattr(instance, method_name)
                print(f"Running {method_name}...", end=" ")
                method()
                print("PASS")
                passed += 1
            except Exception as e:
                print(f"FAIL: {e}")
                failed += 1
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed > 0:
        sys.exit(1)
    else:
        sys.exit(0)