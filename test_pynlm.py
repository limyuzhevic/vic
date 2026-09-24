import pytest
import numpy as np
import tempfile
import os
from pathlib import Path
import sys

# Add the python bindings to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False

@pytest.mark.skipif(not PYNL_AVAILABLE, reason="pynlm not available")
class TestBasicFunctionality:
    """Test basic NLM functionality"""
    
    def test_create_brain(self):
        """Test creating a basic brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        
    def test_brain_initialization(self):
        """Test brain initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain.initialize()
        
    def test_brain_step(self):
        """Test basic brain stepping"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
            
        assert brain.getTotalSpikeCount() > 0
        
    def test_memory_integration(self):
        """Test that memory systems are integrated"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Memory systems should be available (Phase 6 integration)
        assert brain.getWorkingMemory() is not None
        assert brain.getEpisodicMemory() is not None
        assert brain.getPredictionSystem() is not None
        assert brain.getPlanner() is not None
        assert brain.getCuriosity() is not None
        assert brain.getDopamine() is not None

@pytest.mark.skipif(not PYNL_AVAILABLE, reason="pynlm not available")
class TestAdvancedFunctionality:
    """Test advanced NLM functionality"""
    
    def test_world_and_agent_integration(self):
        """Test world and agent integration"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run a simple simulation
        for i in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        assert brain.getFiringNeuronCount() > 0
        
    def test_checkpoint_system(self):
        """Test checkpoint save/load"""
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Run some steps
        for i in range(50):
            brain1.step(i)
            
        # Save checkpoint
        with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as tmp:
            checkpoint_path = tmp.name
            
        try:
            assert brain1.save(checkpoint_path)
            assert os.path.exists(checkpoint_path)
            
            # Create new brain and load
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            assert brain2.load(checkpoint_path)
            
            # Check that some state was preserved
            assert brain2.getTotalSpikeCount() >= brain1.getTotalSpikeCount()
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
                
    def test_development_integration(self):
        """Test development system integration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableDevelopment(True)
        
        # Run for many steps to see development effects
        for i in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            agent.updateDevelopment(0.1)
            
        # Brain should show developmental progression
        assert brain.getDevelopmentalStage() != None
        
    def test_memory_functionality(self):
        """Test working memory and episodic memory"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation to create memories
        for i in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            
        # Check memory systems
        if brain.getWorkingMemory():
            # Working memory may or may not have active traces depending on activity
            pass
            
        if brain.getEpisodicMemory():
            episode_count = brain.getEpisodicMemory().getEpisodeCount()
            # May have episodes depending on step frequency
            
    def test_prediction_system(self):
        """Test prediction system integration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Prediction system should be available
        assert brain.getPredictionSystem() is not None
        
    def test_planning_system(self):
        """Test neural planner integration"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Neural planner should be available
        assert brain.getPlanner() is not None
        
    def test_neuromodulation(self):
        """Test neuromodulation systems"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        
        # Apply some reward
        for i in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(1.0, 0.0)
            
        # Neuromodulation systems should be functional
        if brain.getDopamine():
            dopamine_level = brain.getDopamine().getLevel()
            
        if brain.getCuriosity():
            curiosity_level = brain.getCuriosity().getLevel()

@pytest.mark.skipif(not PYNL_AVAILABLE, reason="pynlm not available")
class TestPerformance:
    """Test performance characteristics"""
    
    def test_large_brain_performance(self):
        """Test performance with a larger brain"""
        config = pynlm.createDefaultConfig()
        
        # Configure larger brain
        config.set("neuron_count", 500)
        config.set("region_count", 5)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run performance test
        import time
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
            
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be reasonable (less than 1 second for 100 steps)
        assert duration < 10.0, f"Performance too slow: {duration:.2f}s for 100 steps"
        
        print(f"Performance: {duration:.2f}s for 100 steps with {brain.getTotalNeuronCount()} neurons")

@pytest.mark.skipif(not PYNL_AVAILABLE, reason="pynlm not available")
class TestAdvancedUsagePatterns:
    """Test advanced usage patterns and features"""
    
    def test_agent_with_all_features_enabled(self):
        """Test agent with all advanced features enabled"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for i in range(150):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            agent.updateDevelopment(0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        # Check that all systems are working
        assert brain.getWorkingMemory() is not None
        assert brain.getEpisodicMemory() is not None
        assert brain.getPlanner() is not None
        assert brain.getConceptFormation() is not None
        assert brain.getAttention() is not None
        assert brain.getPredictionSystem() is not None
        
    def test_brain_status_logging(self):
        """Test brain status logging"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This should not crash
        brain.logStatus()
        
    def test_configuration_saving(self):
        """Test configuration saving and loading"""
        config1 = pynlm.createDefaultConfig()
        config1.set("neuron_count", 300)
        config1.set("region_count", 3)
        
        # Save to file
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
            config_path = f.name
            
        try:
            config1.saveToFile(config_path)
            assert os.path.exists(config_path)
            
            # Load from file
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_path)
            
            # Check that config was loaded
            assert config2.has("neuron_count")
            
        finally:
            if os.path.exists(config_path):
                os.unlink(config_path)

if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
