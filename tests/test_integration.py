"""
Integration Tests for NLM
Comprehensive test coverage for complete simulation workflows and component interactions
"""

import pytest
import tempfile
import os
from pathlib import Path
import sys

# Add the python directory to path for imports
python_dir = Path(__file__).parent.parent / "python"
sys.path.insert(0, str(python_dir))

# Try to import the NLM Python bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Tests will be skipped.")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestCompleteSimulationWorkflows:
    """Test complete simulation workflows"""
    
    def test_complete_simulation_workflow(self):
        """Test complete simulation workflow from components to action"""
        # Create all simulation components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize all components
        brain.initialize()
        world.configure(20, 20, 8, 8)
        agent.initialize(world)
        
        # Run simulation steps
        total_spikes = 0
        for step in range(50):
            # Update world
            world.update(step * 0.1)
            
            # Get sensory perception
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Step the brain
            brain.step(step, step * 0.1)
            
            # Get spike count
            total_spikes += brain.getTotalSpikeCount()
            
            # Generate and apply motor action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
        
        # Verify simulation completed successfully
        assert brain.getTotalNeuronCount() > 0
        assert total_spikes >= 0
        assert agent.getNeuromodulationLevel() >= -1.0
        
        print("✓ Complete simulation workflow test passed")
    
    def test_end_to_end_scenario(self):
        """Test end-to-end scenario with multiple episodes"""
        # Create simulation environment
        config = pynlm.createDefaultConfig()
        config.set("learning_rate", 0.01)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(15, 15, 6, 6)
        world.setAgentStart(7, 7)
        
        # Initialize components
        brain.initialize()
        agent.initialize(world)
        
        # Run multiple episodes
        for episode in range(3):
            # Reset for new episode
            agent.reset()
            brain.reset()
            
            # Run episode steps
            episode_spikes = 0
            for step in range(20):
                # Update world
                world.update(step * 0.05)
                
                # Get perception
                percept = world.getSensoryPercept()
                
                # Process with agent
                agent.processSensoryInput(percept)
                
                # Step brain
                brain.step(step, step * 0.05)
                episode_spikes += brain.getTotalSpikeCount()
                
                # Generate action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, step * 0.05)
                
                # Apply reward modulation
                reward = 1.0 if step % 5 == 0 else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(step * 0.05)
            
            # Verify episode completed
            assert episode_spikes >= 0
            assert agent.getTotalNeuronCount() > 0
        
        print("✓ End-to-end scenario test passed")
    
    def test_component_interactions(self):
        """Test interaction between all components"""
        # Create components with specific configurations
        config = pynlm.createSimpleConfig(100, 3)
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure and initialize
        world.configure(10, 10, 4, 4)
        brain.initialize()
        agent.initialize(world)
        
        # Create sensory percept with multiple signal types
        percept = pynlm.SensoryPercept()
        
        # Add different types of sensory data
        vision_data = [0.5] * 64  # 8x8 vision
        touch_data = [0.3] * 8   # Touch sensors
        internal_data = [0.1] * 4  # Internal signals
        proprioception_data = [0.2] * 6  # Proprioception
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        
        # Process the percept
        agent.processSensoryInput(percept)
        
        # Verify agent processed all input types
        assert agent.getSensoryInputSize() == 82  # 64 + 8 + 4 + 6
        assert agent.getMotorOutputSize() == 6
        
        print("✓ Component interactions test passed")

@pytest.mark.skipif(not PYNLM_AVAILABLE, reason="pynlm module not available")
class TestErrorHandlingComplexScenarios:
    """Test error handling in complex scenarios"""
    
    def test_error_handling_scenarios(self):
        """Test error handling in various complex scenarios"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        world.configure(10, 10, 4, 4)
        agent.initialize(world)
        
        # Scenario 1: Rapid reset calls
        for i in range(10):
            agent.reset()
        
        # Scenario 2: Development with null values
        for i in range(20):
            agent.updateDevelopment(i * 0.01)
        
        # Scenario 3: Reward modulation with edge cases
        agent.applyRewardModulation(1000.0, -1000.0)
        agent.applyRewardModulation(-1000.0, 1000.0)
        
        # Scenario 4: Neuromodulation state management
        assert agent.getNeuromodulationLevel() != float('inf')
        assert agent.getNoveltyLevel() != float('inf')
        assert agent.getCuriosityLevel() != float('inf')
        
        print("✓ Error handling scenarios test passed")
    
    def test_invalid_input_handling(self):
        """Test handling of invalid or empty inputs"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        world.configure(10, 10, 4, 4)
        agent.initialize(world)
        
        # Test with empty percept
        empty_percept = pynlm.SensoryPercept()
        
        # This should not crash (error handling in C++)
        agent.processSensoryInput(empty_percept)
        
        # Test brain error handling
        agent.reset()
        
        print("✓ Invalid input handling test passed")

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
