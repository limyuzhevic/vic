import pytest
import sys
from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain

# Test configuration
TEST_CONFIG = {
    "brain.neuron_count": 100,
    "brain.region_count": 1,
    "brain.connection_probability": 0.05,
    "random_seed": 42,
}

class TestBasicFunctionality:
    """Test basic NLM functionality"""
    
    def setup_method(self):
        """Setup for each test"""
        self.config = createDefaultConfig()
        for key, value in TEST_CONFIG.items():
            self.config.set(key, value)
        self.brain = createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation_and_initialization(self):
        """Test that brain can be created and initialized"""
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() >= 0
        assert self.brain.getTotalSpikeCount() >= 0
        assert self.brain.getFiringNeuronCount() >= 0
        assert self.brain.getActiveNeuronCount() >= 0
    
    def test_brain_step_execution(self):
        """Test that brain can execute simulation steps"""
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        
        # Run several steps
        for step in range(50):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        final_firing = self.brain.getFiringNeuronCount()
        
        # Should have progressed (spikes may or may not increase depending on activity)
        assert final_spikes >= initial_spikes
        assert final_firing >= 0
    
    def test_configuration_operations(self):
        """Test configuration get/set operations"""
        assert self.config.has("brain.neuron_count")
        assert self.config.has("brain.region_count")
        assert self.config.has("brain.connection_probability")
        
        neuron_count = self.config.getOr[int64_t]("brain.neuron_count", 1000)
        assert neuron_count == 100  # From TEST_CONFIG
        
        region_count = self.config.getOr[int64_t]("brain.region_count", 1)
        assert region_count == 1
        
        # Test key listing
        keys = self.config.getKeys()
        assert len(keys) > 0
        
        # Test summary
        summary = self.config.summary()
        assert len(summary) > 0
    
    def test_region_operations(self):
        """Test brain region operations"""
        # Add a region
        region_id = self.brain.addRegion("test_region")
        assert region_id.value >= 0
        
        # Get the region
        region = self.brain.getRegion(region_id)
        assert region is not None
        
        # Check region count
        region_count = self.brain.getRegionCount()
        assert region_count > 0
        
        # Get all region IDs
        region_ids = self.brain.getRegionIds()
        assert len(region_ids) > 0
        
        # Get all regions
        regions = self.brain.getRegions()
        assert len(regions) > 0
    
    def test_development_system(self):
        """Test development system functionality"""
        initial_stage = self.brain.getDevelopmentalStage()
        assert initial_stage.value >= 0
        
        # Set development stage
        from pynlm import developmentalStage
        self.brain.setDevelopmentalStage(developmentalStage.CriticalPeriod)
        final_stage = self.brain.getDevelopmentalStage()
        assert final_stage.value >= 0
    
    def test_plasticity_mechanisms(self):
        """Test that plasticity mechanisms are accessible"""
        # Get plasticity systems (these might be None if not fully integrated yet)
        stdp = self.brain.getSTDP()
        hebbian = self.brain.getHebbian()
        structural = self.brain.getStructuralPlasticity()
        
        # Just verify we can access them without crashing
        # (Actual plasticity testing would require more integration)
        assert stdp is not None or hebbian is not None or structural is not None
    
    def test_world_simulation(self):
        """Test world simulation integration"""
        world = createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Set agent start position
        world.setAgentStart(5.0, 5.0)
        
        # Get agent body
        body = world.getAgentBody()
        assert body.x == 5.0
        assert body.y == 5.0
        assert body.energy > 0
        
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Check world properties
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getSimulationTime() >= 0
    
    def test_agent_brain_integration(self):
        """Test agent brain interface"""
        # Create world
        world = createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Create agent brain
        agent = createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Test agent state
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() >= 0.0
        assert agent.getNeuromodulationLevel() >= 0.0
        
        # Test agent operations
        # Create a simple percept
        from pynlm import Vision
        vision = Vision(8, 8, 3)
        # (Note: We'd need to set vision data properly)
        
        # Process sensory input (with dummy percept)
        agent.processSensoryInput(vision)
        
        # Decode motor command
        from pynlm import MotorCommand
        action = agent.decodeMotorCommand()
        assert action.value >= 0
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
    
    def test_complete_simulation_loop(self):
        """Test a complete agent-world simulation loop"""
        # Setup
        brain = self.brain
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation loop
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain computation
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = 0.0
            internal = world.getSensoryPercept().getInternal()
            if internal:
                reward = internal[0] if internal else 0.0
            
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Verify basic state
            assert brain.getTotalSpikeCount() >= 0
            assert agent.getCuriosityLevel() >= 0.0
            assert agent.getNoveltyLevel() >= 0.0
            assert world.getAgentBody().energy >= 0.0
    
    def test_error_handling_and_edge_cases(self):
        """Test error handling and edge cases"""
        # Test brain reset
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.reset()
        assert self.brain.getTotalSpikeCount() >= 0
        
        # Test step with different parameters
        self.brain.step(0)
        self.brain.step(1, 0.001)
        
        # Test that operations don't crash
        region_count = self.brain.getRegionCount()
        total_neurons = self.brain.getTotalNeuronCount()
        total_synapses = self.brain.getTotalSynapseCount()
        
        assert region_count >= 0
        assert total_neurons > 0
        assert total_synapses >= 0

class TestSystemIntegration:
    """Test system integration and utilities"""
    
    def test_system_statistics(self):
        """Test system-wide statistics"""
        from pynlm import getSystemStats
        
        stats = getSystemStats()
        assert len(stats) > 0
        assert "NLM" in stats
        assert "Phase 6" in stats
        
        # Check for key components
        key_components = ["Neural Core", "Memory Systems", "Neuromodulation"]
        for component in key_components:
            assert component in stats
    
    def test_advanced_example_setup(self):
        """Test advanced example setup"""
        from pynlm import createQuickTestSetup, createAdvancedUsageExample
        
        # Quick setup
        brain1 = createQuickTestSetup()
        assert brain1.getTotalNeuronCount() > 0
        assert brain1.getTotalSynapseCount() > 0
        
        # Advanced example
        brain2 = createAdvancedUsageExample()
        assert brain2.getTotalNeuronCount() > 0
        assert brain2.getTotalSynapseCount() > 0
    
    def test_performance_tools(self):
        """Test performance monitoring tools"""
        from pynlm import runBenchmark, analyzeBrainState, startPerformanceTimer, stopPerformanceTimer
        
        brain = createBrain(createDefaultConfig())
        brain.initialize()
        
        # Test benchmark
        profile = runBenchmark(brain, 50)
        assert "total" in profile
        assert "brain_step" in profile
        assert profile["total"] > 0
        
        # Test brain state analysis
        analyzeBrainState(brain)  # Should not raise exception
        
        # Test performance timer
        timer = startPerformanceTimer()
        time.sleep(0.001)  # Small delay
        stopPerformanceTimer(timer)  # Should not raise exception

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
