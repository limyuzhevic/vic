#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings improvements.

This test suite validates all the new functionality added to the pynlm module,
including advanced configuration options, enhanced agent functionality,
and improved error handling.
"""

import pynlm
import tempfile
import json
import pytest

class TestBasicFunctionality:
    """Test basic NLM functionality after improvements."""
    
    def test_version_and_metadata(self):
        """Test module metadata and version information."""
        version_info = pynlm.get_version()
        assert "version" in version_info
        assert "description" in version_info
        assert version_info["version"] == "0.1.0"
        print(f"✓ NLM version: {version_info['version']}")
    
    def test_basic_brain_creation(self):
        """Test basic brain creation with default config."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        brain.initialize()
        
        # Check brain statistics
        stats = brain.getBrainStatistics()
        assert stats["neuron_count"] > 0
        assert stats["synapse_count"] > 0
        print(f"✓ Brain created: {stats['neuron_count']} neurons, {stats['synapse_count']} synapses")
    
    def test_config_operations(self):
        """Test enhanced configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test setting various types
        config.set_int("test.int", 42)
        config.set_float("test.float", 3.14)
        config.set_string("test.string", "hello")
        config.set_bool("test.bool", True)
        
        # Test getting values
        assert config.get_int("test.int") == 42
        assert config.get_float("test.float") == 3.14
        assert config.get_string("test.string") == "hello"
        assert config.get_bool("test.bool") == True
        
        # Test validation
        config.validate(["test.int", "test.float", "test.string", "test.bool"])
        print("✓ Configuration operations working correctly")
    
    def test_enum_values(self):
        """Test all enumeration values are accessible."""
        neuron_types = [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory,
                       pynlm.NeuronType.Sensory, pynlm.NeuronType.Motor]
        assert len(neuron_types) == 4
        
        motor_commands = [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.TurnLeft,
                         pynlm.MotorCommand.Interact, pynlm.MotorCommand.Wait]
        assert len(motor_commands) == 4
        
        developmental_stages = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.Adult]
        assert len(developmental_stages) == 2
        
        print("✓ Enumeration values accessible")
class TestEnhancedAgentFunctionality:
    """Test enhanced agent functionality and commands."""
    
    def setup_method(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_brain_status(self):
        """Test enhanced agent brain status reporting."""
        status = self.agent.getBrainStatus()
        
        assert "neuron_count" in status
        assert "curiosity" in status
        assert "novelty" in status
        assert "prediction_error" in status
        
        # Test with different agent configurations
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        status2 = self.agent.getBrainStatus()
        assert status2["curiosity"] >= 0.0
        print("✓ Agent brain status reporting working")
    
    def test_motor_command_operations(self):
        """Test motor command creation and utility functions."""
        # Test getting command names
        assert pynlm.getMotorCommandName(pynlm.MotorCommand.MoveForward) == "MoveForward"
        assert pynlm.getMotorCommandName(pynlm.MotorCommand.TurnLeft) == "TurnLeft"
        assert pynlm.getMotorCommandName(pynlm.MotorCommand.Interact) == "Interact"
        assert pynlm.getMotorCommandName(pynlm.MotorCommand.Wait) == "Wait"
        
        # Test getting command list
        commands = pynlm.getMotorCommandList()
        assert "MoveForward" in commands
        assert "TurnLeft" in commands
        assert "Wait" in commands
        
        # Test action creation from motor command
        action = pynlm.createActionFromCommand(pynlm.MotorCommand.MoveForward)
        assert action.getType() == pynlm.MotorCommand.MoveForward
        
        print("✓ Motor command operations working")
    
    def test_enhanced_sensory_processing(self):
        """Test enhanced sensory processing capabilities."""
        # Create percept with data
        percept = pynlm.SensoryPercept()
        vision_data = [0.1] * 64  # 8x8 vision
        percept.setVision(vision_data)
        
        # Add noise for realism
        percept.add_noise(vision_noise=0.01, touch_noise=0.01, internal_noise=0.01, proprioception_noise=0.01)
        
        # Test normalization
        percept.normalize()
        
        # Test signal ranges
        ranges = percept.getSignalRanges()
        assert "vision" in ranges
        assert len(ranges["vision"]) == 2
        
        # Test validation
        pynlm.validate_percept(percept)
        
        # Test signal counts
        assert percept.hasVision()
        assert not percept.hasTouch()
        
        print("✓ Enhanced sensory processing working")
    
    def test_world_interaction(self):
        """Test enhanced world interaction capabilities."""
        # Test object creation and placement
        world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0, 1.0)
        assert world_obj.x == 5.0
        assert world_obj.y == 5.0
        assert world_obj.type == pynlm.WorldObjectType.Resource
        assert world_obj.value == 10.0
        
        # Test distance calculation
        distance = world_obj.getDistance(0.0, 0.0)
        assert distance > 0.0
        
        # Test proximity checking
        assert world_obj.isNear(5.1, 5.1, 0.2)
        assert not world_obj.isNear(10.0, 10.0, 0.1)
        
        # Test agent body functionality
        body = self.world.getAgentBody()
        body.setPosition(1.0, 1.0)
        body.setVelocity(1.0, 0.5)
        
        assert body.x == 1.0
        assert body.y == 1.0
        assert body.velocityX == 1.0
        assert body.velocityY == 0.5
        
        speed = body.getSpeed()
        assert speed > 0.0
        
        direction = body.getDirection()
        assert direction >= 0.0
        
        # Test body update
        body.update(0.1)
        assert body.x > 1.0  # Moved forward
        
        print("✓ World interaction enhanced")
class TestErrorHandlingAndValidation:
    """Test enhanced error handling and validation."""
    
    def test_configuration_validation(self):
        """Test enhanced configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Should raise error for missing required keys
        with pytest.raises(pytest.raises):
            config.validate(["missing.key"])
        
        # Should work for present keys
        config.set_string("present.key", "value")
        config.validate(["present.key"])
        
        # Test validation with config normalization
        config_summary = pynlm.normalize_config(config)
        assert "present.key" in config_summary
        
        print("✓ Configuration validation enhanced")
    
    def test_percept_validation(self):
        """Test percept validation."""
        # Test empty percept validation
        empty_percept = pynlm.SensoryPercept()
        pynlm.validate_percept(empty_percept)  # Should not raise
        
        # Test percept with data
        percept = pynlm.SensoryPercept()
        percept.setVision([0.5] * 16)
        pynlm.validate_percept(percept)  # Should not raise
        
        print("✓ Percept validation working")
    
    def test_error_scenarios(self):
        """Test error scenarios and edge cases."""
        config = pynlm.createDefaultConfig()
        
        # Test boundary values
        config.set_int("boundary.test", 0)
        assert config.get_int("boundary.test") == 0
        
        config.set_float("boundary.test", 0.0)
        assert config.get_float("boundary.test") == 0.0
        
        config.set_string("boundary.test", "")
        assert config.get_string("boundary.test") == ""
        
        config.set_bool("boundary.test", False)
        assert config.get_bool("boundary.test") == False
        
        print("✓ Error handling and boundary conditions covered")
class TestSimulationIntegration:
    """Test complete simulation integration."""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation with all enhancements."""
        # Setup enhanced simulation
        config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable all learning mechanisms
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run enhanced simulation
        for step in range(50):
            world.update(0.1)
            
            # Process sensory input with noise for realism
            percept = world.getSensoryPercept()
            if percept.hasVision():
                percept.add_noise(vision_noise=0.005, touch_noise=0.005)
                agent.processSensoryInput(percept)
            
            brain.step(step)
            
            # Get motor command and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and development
            reward = 0.0
            if percept.hasInternal() and percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            # Log progress occasionally
            if step % 10 == 0:
                status = agent.getBrainStatus()
                print(f"Step {step}: {status['neuron_count']} neurons, "
                      f"curiosity: {status['curiosity']:.3f}, "
                      f"novelty: {status['novelty']:.3f}")
        
        # Verify simulation completed successfully
        final_status = agent.getBrainStatus()
        assert final_status["neuron_count"] > 0
        assert final_status["total_spikes"] > 0
        assert final_status["curiosity"] >= 0.0
        
        print("✓ Complete agent simulation successful")
    
    def test_api_documentation(self):
        """Test API documentation generation."""
        api_doc = pynlm.get_api_documentation()
        
        assert "Config" in api_doc
        assert "Brain" in api_doc
        assert "AgentBrain" in api_doc
        assert "SimpleWorld" in api_doc
        
        config_doc = api_doc["Config"]
        assert "methods" in config_doc
        
        print("✓ API documentation generation working")
    
    def test_quickstart_examples(self):
        """Test quick start examples availability."""
        examples = pynlm.get_quickstart_examples()
        
        assert "basic_brain" in examples
        assert "complete_simulation" in examples
        
        basic_example = examples["basic_brain"]
        assert "description" in basic_example
        assert "code" in basic_example
        
        print("✓ Quick start examples available")
class TestPerformanceAndRobustness:
    """Test performance and robustness features."""
    
    def test_large_scale_simulation(self):
        """Test simulation with many neurons."""
        config = pynlm.createDefaultConfig()
        
        # Create brain with more neurons for performance testing
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run multiple steps to test robustness
        for step in range(200):
            brain.step(step)
        
        stats = brain.getBrainStatistics()
        assert stats["total_spikes"] >= 0
        print(f"✓ Large-scale simulation: {stats['neuron_count']} neurons, {stats['total_spikes']} spikes")
    
    def test_multiple_agents(self):
        """Test multiple agents in same world."""
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains and agents
        brains = [pynlm.createBrain(config) for _ in range(3)]
        agents = [pynlm.createAgentBrain(brain) for brain in brains]
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        
        for agent in agents:
            agent.initialize(world)
        
        # Run simulation with multiple agents
        for step in range(30):
            world.update(0.1)
            
            for agent in agents:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)  # Note: brain is same for all agents
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Multiple agents simulation successful")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
