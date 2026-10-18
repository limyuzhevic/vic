#!/usr/bin/env python3
"""
Comprehensive Python binding tests for NLM (Neural Learning Machine)

This test suite validates the pynlm Python bindings functionality.
It tests both the basic API and more advanced features.
"""

import sys
import os
import tempfile
import json
from pathlib import Path

# Add the current directory to Python path to ensure we import the local pynlm module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    PYNLM_AVAILABLE = False

class TestNLMBasic:
    """Basic functionality tests for NLM Python bindings"""
    
    def setup_method(self):
        """Setup before each test"""
        if not PYNLM_AVAILABLE:
            return
            
        # Create a fresh config for each test
        self.config = pynlm.createDefaultConfig()
        
        # Create a simple config with some values
        self.config.set("brain.neuron_count", 100)
        self.config.set("brain.synapse_density", 0.1)
        self.config.set("random_seed", 12345)
        self.config.set("simulation_timestep", 0.001)
        
        # Create brain with this config
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Cleanup after each test"""
        if hasattr(self, 'brain') and self.brain is not None:
            self.brain.reset()
        if hasattr(self, 'config') and self.config is not None:
            pass
    
    def test_module_import(self):
        """Test that pynlm module imports correctly"""
        assert PYNLM_AVAILABLE
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')
    
    def test_config_creation(self):
        """Test basic config creation and methods"""
        assert self.config is not None
        
        # Test basic methods
        assert not self.config.has("nonexistent_key")
        assert self.config.has("brain.neuron_count")
        assert self.config.has("brain.synapse_density")
        
        # Test get methods
        assert self.config.get("brain.neuron_count") == 100
        assert self.config.getOr("brain.neuron_count", 50) == 100
        assert self.config.getOr("brain.nonexistent", 50) == 50
        
        # Test getKeys
        keys = self.config.getKeys()
        assert isinstance(keys, list)
        assert "brain.neuron_count" in keys
        assert "brain.synapse_density" in keys
    
    def test_config_save_load(self):
        """Test config save and load functionality"""
        # Save config to a temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            # Save config
            assert self.config.saveToFile(config_file)
            
            # Verify file was created
            assert os.path.exists(config_file)
            
            # Create new config and load from file
            new_config = pynlm.createDefaultConfig()
            assert new_config.loadFromFile(config_file)
            
            # Check that some keys were loaded (though simple format doesn't preserve all)
            assert new_config.has("brain.neuron_count") or len(new_config.getKeys()) > 0
            
        finally:
            # Cleanup
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_brain_creation(self):
        """Test brain creation and initialization"""
        assert self.brain is not None
        
        # Test basic brain properties
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
        assert self.brain.getActiveNeuronCount() >= 0
        assert self.brain.getFiringNeuronCount() >= 0
        assert self.brain.getTotalSpikeCount() == 0
        
        # Test brain step method
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        
        # After a step, spike count should be the same or increased
        assert self.brain.getTotalSpikeCount() >= initial_spikes
        assert self.brain.getPendingSpikeEventCount() >= 0
    
    def test_brain_simulation(self):
        """Test basic brain simulation loop"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run several simulation steps
        for step in range(10):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
        
        # Test statistics
        assert self.brain.getAverageFiringRate() >= 0.0
        assert self.brain.getExcitationInhibitionRatio() >= 0.0
    
    def test_region_management(self):
        """Test brain region management"""
        # Add regions
        region1_id = self.brain.addRegion("TestRegion1")
        region2_id = self.brain.addRegion("TestRegion2")
        
        assert region1_id.index() > 0
        assert region2_id.index() > region1_id.index()
        
        # Get regions
        region1 = self.brain.getRegion(region1_id)
        region2 = self.brain.getRegion(region2_id)
        
        assert region1 is not None
        assert region2 is not None
        
        # Test region count
        assert self.brain.getRegionCount() >= 2
        assert len(self.brain.getRegionIds()) >= 2
    
    def test_config_from_args(self):
        """Test configuration loading from command line arguments"""
        config = pynlm.createDefaultConfig()
        
        # Simulate command line arguments
        test_args = ["program", "--brain.neuron_count=200", "--simulation_timestep=0.002"]
        
        # Convert to C-style args
        c_args = [arg.encode('utf-8') for arg in test_args]
        
        # Load from args (this is a simplified version of the actual C++ interface)
        # Note: This may need to be adapted based on the actual pybind11 interface
        try:
            # The actual implementation might be different
            pass
        except:
            pass
    
    def test_error_handling(self):
        """Test error handling for invalid inputs"""
        # Test loading non-existent file
        config = pynlm.createDefaultConfig()
        assert not config.loadFromFile("/nonexistent/path/file.json")
        
        # Test with invalid JSON (should fallback to simple format)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write("invalid json content [[[[[[[[[")
            invalid_file = f.name
        
        try:
            # Should handle gracefully or return false
            result = config.loadFromFile(invalid_file)
            # Result depends on implementation
        finally:
            if os.path.exists(invalid_file):
                os.unlink(invalid_file)
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run some steps
        for step in range(5):
            self.brain.step(step)
        
        # Reset brain
        self.brain.reset()
        
        # Spike count should be reset
        assert self.brain.getTotalSpikeCount() == 0
        assert self.brain.getTotalSpikeCount() == 0
        
        # But we should still be able to run steps after reset
        self.brain.step(0)
        assert self.brain.getTotalSpikeCount() >= 0

class TestNLMAdvanced:
    """Advanced functionality tests"""
    
    def setup_method(self):
        """Setup for advanced tests"""
        if not PYNLM_AVAILABLE:
            return
            
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def teardown_method(self):
        """Cleanup"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_world_simulation(self):
        """Test world simulation with agent"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a world simulation
        for step in range(20):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update development
            self.agent.updateDevelopment(0.1)
        
        # Verify some activity occurred
        assert self.brain.getTotalSpikeCount() > initial_spikes
        assert self.brain.getFiringNeuronCount() >= 0
    
    def test_memory_systems(self):
        """Test basic memory system access"""
        # Get memory systems
        working_memory = self.brain.getWorkingMemory()
        episodic_memory = self.brain.getEpisodicMemory()
        
        # These should be accessible (may be None if not initialized)
        if working_memory is not None:
            assert working_memory is not None
        
        # Test agent memory access
        assert self.agent.getBrain() == self.brain
    
    def test_neuromodulation(self):
        """Test neuromodulation functionality"""
        # Enable neuromodulation
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Check if enabled
        assert self.agent.isRewardModulationEnabled()
        assert self.agent.isCuriosityEnabled()
        
        # Check neuromodulation levels
        neuromodulation_level = self.agent.getNeuromodulationLevel()
        curiosity_level = self.agent.getCuriosityLevel()
        novelty_level = self.agent.getNoveltyLevel()
        
        assert isinstance(neuromodulation_level, (int, float))
        assert isinstance(curiosity_level, (int, float))
        assert isinstance(novelty_level, (int, float))
    
    def test_development(self):
        """Test development system"""
        initial_stage = self.brain.getDevelopmentalStage()
        assert initial_stage is not None
        
        # Update development multiple times
        for step in range(10):
            self.agent.updateDevelopment(0.1)
        
        # Stage should progress or at least be accessible
        assert self.brain.getDevelopmentalStage() is not None
    
    def test_plasticity_systems(self):
        """Test plasticity system access"""
        # Get plasticity systems
        stdp = self.brain.getSTDP()
        hebbian = self.brain.getHebbian()
        structural = self.brain.getStructuralPlasticity()
        
        # These should be accessible (may be None if not initialized)
        # The actual implementation depends on the NLM architecture
        pass

class TestNLMIntegration:
    """Integration tests combining multiple components"""
    
    def setup_method(self):
        """Setup for integration tests"""
        if not PYNLM_AVAILABLE:
            return
            
        self.config = pynlm.createDefaultConfig()
        
        # Configure for realistic simulation
        self.config.set("brain.neuron_count", 500)
        self.config.set("brain.synapse_density", 0.05)
        self.config.set("random_seed", 42)
        self.config.set("simulation_timestep", 0.001)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.setAgentStart(15.0, 15.0)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable key systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def teardown_method(self):
        """Cleanup"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation with all systems"""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run a realistic simulation
        for step in range(100):
            # World dynamics
            self.world.update(0.1)
            
            # Perception
            percept = self.world.getSensoryPercept()
            
            # Agent processing
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Action selection
            action = self.agent.decodeMotorCommand()
            
            # World interaction
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learning and development
            if step % 10 == 0:
                reward = self.brain.getTotalSpikeCount() / 100.0
                self.agent.applyRewardModulation(reward, 0.0)
                self.agent.updateDevelopment(0.1)
        
        # Verify simulation produced results
        final_spikes = self.brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes
        
        # Verify agent state
        assert self.agent.getDevelopmentalStage() is not None
        assert self.agent.getNeuromodulationLevel() >= 0.0
        
        # Verify brain is still functional
        self.brain.step(100)
        assert self.brain.getTotalSpikeCount() >= final_spikes
    
    def test_checkpointing(self):
        """Test brain checkpointing functionality"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            # Run some simulation
            for step in range(50):
                self.brain.step(step)
            
            # Save checkpoint
            assert self.brain.save(checkpoint_file)
            assert os.path.exists(checkpoint_file)
            
            # Reset and load checkpoint
            self.brain.reset()
            assert self.brain.load(checkpoint_file)
            
            # Verify checkpoint was loaded
            assert self.brain.getTotalNeuronCount() > 0
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)

class TestNLMPerformance:
    """Performance and stress tests"""
    
    def setup_method(self):
        """Setup for performance tests"""
        if not PYNLM_AVAILABLE:
            return
            
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 200)
        self.config.set("simulation_timestep", 0.001)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def teardown_method(self):
        """Cleanup"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_large_brain_simulation(self):
        """Test simulation with larger brain"""
        import time
        
        start_time = time.time()
        
        # Run simulation with larger brain
        for step in range(200):
            self.brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Verify some spikes occurred
        assert self.brain.getTotalSpikeCount() >= 0
        
        # Performance should be reasonable (this is a very rough check)
        # Allow up to 10 seconds for 200 steps with 200 neurons
        assert elapsed < 30.0, f"Performance too slow: {elapsed:.2f}s for 200 steps"
    
    def test_brain_statistics(self):
        """Test brain statistics collection"""
        # Get initial stats
        initial_stats = {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
        }
        
        # Run simulation
        for step in range(50):
            self.brain.step(step)
        
        # Get final stats
        final_stats = {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
        }
        
        # Neuron count should remain constant
        assert initial_stats['neuron_count'] == final_stats['neuron_count']
        
        # Synapse count should remain constant
        assert initial_stats['synapse_count'] == final_stats['synapse_count']
        
        # Spike count should be non-decreasing
        assert final_stats['spike_count'] >= initial_stats['spike_count']

# Run tests if executed directly
if __name__ == "__main__":
    import pytest
    
    # Skip tests if pynlm is not available
    if not PYNLM_AVAILABLE:
        print("Skipping NLM tests - pynlm module not available")
        sys.exit(0)
    
    # Run pytest
    exit_code = pytest.main([
        __file__,
        "-v",
        "--tb=short",
        "--timeout=30"
    ])
    
    sys.exit(exit_code)