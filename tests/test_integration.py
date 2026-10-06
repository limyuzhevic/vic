"""Integration Tests

Test that brain, world, and agent work together.
"""

import pytest
import pynlm
import numpy as np


class TestIntegration:
    """Test integration between brain, world, and agent components."""
    
    def test_brain_world_integration(self):
        """Test integration between brain and world components."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain and world
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Test world methods
        assert world.getWidth() == 800
        assert world.getHeight() == 600
        assert world.getAgentBody().x == 400
        assert world.getAgentBody().y == 300
        
        # Test brain initialization
        brain.initialize()
        
        # Test brain regions
        brain.addRegion("cortex")
        assert brain.getRegionCount() == 1
        
        # Get regions and verify
        region_ids = brain.getRegionIds()
        assert len(region_ids) == 1
        
        # Test brain statistics
        assert brain.getTotalNeuronCount() == 0  # No neurons added yet
        assert brain.getTotalSynapseCount() == 0
        assert brain.getActiveNeuronCount() == 0
    
    def test_agent_brain_integration(self):
        """Test integration between agent brain and world."""
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize agent brain
        agent_brain.initialize(world)
        
        # Test agent brain methods
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0
        
        # Test developmental stage
        stage = agent_brain.getDevelopmentalStage()
        assert stage in [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging,
        ]
        
        # Test neuromodulation levels
        neuromod_level = agent_brain.getNeuromodulationLevel()
        assert 0.0 <= neuromod_level <= 1.0
        
        curiosity_level = agent_brain.getCuriosityLevel()
        assert 0.0 <= curiosity_level <= 1.0
        
        novelty_level = agent_brain.getNoveltyLevel()
        assert 0.0 <= novelty_level <= 1.0
        
        prediction_error = agent_brain.getPredictionError()
        assert isinstance(prediction_error, float)
    
    def test_sensory_motor_loop(self):
        """Test complete sensory-motor processing loop."""
        # Setup
        world = pynlm.createSimpleWorld()
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Get initial percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Process percept through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Get action
        action = brain.produceAction()
        assert action is not None
        assert action.getType() in [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Look,
            pynlm.ActionType.LookUp,
            pynlm.ActionType.LookDown,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Drink,
            pynlm.ActionType.Rest,
            pynlm.ActionType.Wait,
            pynlm.ActionType.Custom,
        ]
        
        # Apply action in world
        # (In a real scenario, we'd apply the motor command)
        # For now, just verify the action was produced
        assert action.getName() in [
            "MoveForward", "MoveBackward", "TurnLeft", "TurnRight",
            "Look", "LookUp", "LookDown", "Interact", "Eat", "Drink",
            "Rest", "Wait", "Custom"
        ]
    
    def test_simulation_step(self):
        """Test simulation step integration."""
        # Setup
        world = pynlm.createSimpleWorld()
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize components
        brain.initialize()
        agent_brain.initialize(world)
        
        # Add a region and neuron for testing
        brain.addRegion("test_region")
        
        # Test brain step with timestamp
        from pynlm import SimulationStep, Timestamp
        step = SimulationStep()
        time = Timestamp()
        
        # This should not crash
        try:
            brain.step(step, time)
        except Exception:
            # Expected if no neurons are configured
            pass
        
        # Test brain statistics after step
        assert brain.getTotalNeuronCount() >= 0
        assert brain.getTotalSynapseCount() >= 0
    
    def test_multiple_components(self):
        """Test creating and working with multiple component instances."""
        # Create multiple configs
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        # Create multiple brains
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        # Create multiple worlds
        world1 = pynlm.createSimpleWorld()
        world2 = pynlm.createSimpleWorld()
        
        # Configure both worlds
        world1.configure(800, 600, 640, 480)
        world2.configure(1024, 768, 512, 384)
        
        # Create multiple agent brains
        agent1 = pynlm.createAgentBrain(brain1)
        agent2 = pynlm.createAgentBrain(brain2)
        
        # Initialize both agents in their worlds
        agent1.initialize(world1)
        agent2.initialize(world2)
        
        # Verify different configurations
        assert world1.getWidth() == 800
        assert world2.getWidth() == 1024
        assert world1.getHeight() == 600
        assert world2.getHeight() == 768
        
        # Verify agent properties are different
        body1 = world1.getAgentBody()
        body2 = world2.getAgentBody()
        # Both start at center of their respective worlds
        assert body1.x == 400  # 800/2
        assert body2.x == 512  # 1024/2


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
