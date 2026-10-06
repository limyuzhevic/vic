"""Neuromodulation Tests

Test that neuromodulation systems work.
"""

import pytest
import pynlm


class TestNeuromodulation:
    """Test neuromodulation system functionality through Python bindings."""
    
    def test_reward_modulation(self):
        """Test reward modulation system."""
        # Setup components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test reward modulation
        # Apply different reward levels
        agent_brain.applyRewardModulation(1.0, 0.5)  # high reward, low prediction
        agent_brain.applyRewardModulation(0.0, 0.5)  # no reward, low prediction
        agent_brain.applyRewardModulation(-1.0, 0.5) # negative reward, low prediction
        agent_brain.applyRewardModulation(0.5, 1.0)  # medium reward, high prediction
        
        # Test neuromodulation level
        neuromod_level = agent_brain.getNeuromodulationLevel()
        assert 0.0 <= neuromod_level <= 1.0
    
    def test_curiosity_neuromodulation(self):
        """Test curiosity neuromodulation system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test curiosity level
        curiosity = agent_brain.getCuriosityLevel()
        assert 0.0 <= curiosity <= 1.0
        
        # Test that curiosity can be influenced by predictions
        # (curiosity system typically responds to prediction errors)
        prediction_error = agent_brain.getPredictionError()
        assert isinstance(prediction_error, float)
        assert prediction_error >= 0.0
    
    def test_novelty_neuromodulation(self):
        """Test novelty detection neuromodulation system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test novelty level
        novelty = agent_brain.getNoveltyLevel()
        assert 0.0 <= novelty <= 1.0
        
        # Test novelty detection through sensory input
        percept = world.getSensoryPercept()
        if percept:
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Novelty might change after processing
            new_novelty = agent_brain.getNoveltyLevel()
            assert 0.0 <= new_novelty <= 1.0
    
    def test_prediction_error_neuromodulation(self):
        """Test prediction error neuromodulation system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Test prediction error
        prediction_error = agent_brain.getPredictionError()
        assert isinstance(prediction_error, float)
        assert prediction_error >= 0.0
        
        # Prediction error should be used in learning
        # Apply reward modulation based on prediction error
        error_level = min(prediction_error, 1.0)  # Normalize
        agent_brain.applyRewardModulation(error_level, error_level)
        
        # Neuromodulation level should change
        neuromod_level = agent_brain.getNeuromodulationLevel()
        assert 0.0 <= neuromod_level <= 1.0
    
    def test_plasticity_modulation(self):
        """Test plasticity modulation through neuromodulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Test developmental stage affects plasticity
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        stage = brain.getDevelopmentalStage()
        assert stage == pynlm.DevelopmentalStage.CriticalPeriod
        
        # Test plasticity through different neuron types
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal,
        ]
        
        # Test neuromodulation affects plasticity
        # (Neuromodulators like dopamine affect synaptic plasticity)
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Enable/disable plasticity systems
        agent_brain.enableRewardModulation(True)
        assert agent_brain.isRewardModulationEnabled() == True
        
        agent_brain.enableStructuralPlasticity(True)
        assert agent_brain.isStructuralPlasticityEnabled() == True
        
        agent_brain.enableDevelopment(True)
        assert agent_brain.isDevelopmentEnabled() == True
        
        agent_brain.enableCuriosity(True)
        assert agent_brain.isCuriosityEnabled() == True
    
    def test_all_neuromodulators(self):
        """Test all neuromodulation systems together."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(800, 600, 640, 480)
        world.setAgentStart(400, 300)
        
        # Create agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable all neuromodulation systems
        agent_brain.enableRewardModulation(True)
        agent_brain.enableStructuralPlasticity(True)
        agent_brain.enableDevelopment(True)
        agent_brain.enableCuriosity(True)
        
        # Verify all systems are enabled
        assert agent_brain.isRewardModulationEnabled() == True
        assert agent_brain.isStructuralPlasticityEnabled() == True
        assert agent_brain.isDevelopmentEnabled() == True
        assert agent_brain.isCuriosityEnabled() == True
        
        # Test all neuromodulation levels
        neuromodulation_level = agent_brain.getNeuromodulationLevel()
        curiosity_level = agent_brain.getCuriosityLevel()
        novelty_level = agent_brain.getNoveltyLevel()
        prediction_error = agent_brain.getPredictionError()
        developmental_stage = agent_brain.getDevelopmentalStage()
        
        # All should be valid values
        assert 0.0 <= neuromodulation_level <= 1.0
        assert 0.0 <= curiosity_level <= 1.0
        assert 0.0 <= novelty_level <= 1.0
        assert prediction_error >= 0.0
        assert developmental_stage in [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging,
        ]
        
        # Test updating development
        from pynlm import SimulationStep
        step = SimulationStep()
        agent_brain.updateDevelopment(step)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
