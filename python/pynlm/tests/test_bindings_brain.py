#!/usr/bin/env python3
"""
Brain operations test suite for NLM Python bindings.
Tests brain initialization, stepping, and Pythonic API features.
"""

import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestBrainInitialization:
    """Test brain initialization and basic setup."""
    
    def test_brain_creation_with_config(self):
        """Test creating brain with configuration."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        
        assert brain is not None
        assert isinstance(brain, nlm.Brain)
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        
        assert brain.initialize()
        
        # Verify brain is properly initialized
        region_count = brain.getRegionCount()
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        
        assert region_count > 0
        assert neuron_count > 0
        assert synapse_count >= 0
        
        # Check other statistics
        assert brain.getActiveNeuronCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getExcitationInhibitionRatio() >= 0.0
        assert brain.getTotalSpikeCount() >= 0
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        config = nlm.create_simulation_config()
        brain = nlm.Brain(config)
        
        # Initialize and run a step
        assert brain.initialize()
        brain.step(0)
        
        # Reset should work without error
        brain.reset()
        
        # After reset, stats should be back to initial state
        assert brain.getTotalNeuronCount() == config.getOr("neuron_count", 1000)

class TestBrainSimulation:
    """Test brain simulation steps and operations."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        self.brain = nlm.Brain(config)
        assert self.brain.initialize()
    
    def test_single_step(self):
        """Test single simulation step."""
        # Record initial state
        initial_neurons = self.brain.getTotalNeuronCount()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Take a step
        self.brain.step(0)
        
        # State should have changed (at least neuron count should be same)
        assert self.brain.getTotalNeuronCount() == initial_neurons
        # Spike count should have changed or stayed the same
        assert self.brain.getTotalSpikeCount() >= initial_spikes
    
    def test_multiple_steps(self):
        """Test multiple simulation steps."""
        steps = 10
        
        for step in range(steps):
            self.brain.step(step)
            
            # After each step, basic checks
            assert self.brain.getTotalNeuronCount() > 0
            assert self.brain.getAverageFiringRate() >= 0.0
        
        # Verify final state
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
    
    def test_step_with_time(self):
        """Test brain step with timestamp."""
        timestamp = 1.0
        self.brain.step(0, timestamp)
        
        # Should complete without error
        assert self.brain.getTotalNeuronCount() > 0

class TestBrainRegions:
    """Test brain region management."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        config.update(region_count=3)  # Override to 3 regions
        self.brain = nlm.Brain(config)
        assert self.brain.initialize()
    
    def test_region_management(self):
        """Test region creation and management."""
        initial_regions = self.brain.getRegionCount()
        assert initial_regions == 3
        
        # Get region IDs
        region_ids = self.brain.getRegionIds()
        assert len(region_ids) == 3
        
        # Get each region
        for region_id in region_ids:
            region = self.brain.getRegion(region_id)
            assert region is not None
        
        # Get all regions
        regions = self.brain.getRegions()
        assert len(regions) == 3
        
        # Add a new region
        new_region_id = self.brain.addRegion("custom_region")
        assert new_region_id != 0  # Should be a valid region ID
        
        # Verify new region was added
        assert self.brain.getRegionCount() == 4
        new_region = self.brain.getRegion(new_region_id)
        assert new_region is not None
    
    def test_region_operations(self):
        """Test region-specific operations."""
        brain = self.brain
        
        # Get regions
        regions = brain.getRegions()
        assert len(regions) > 0
        
        # Test region access
        for region in regions:
            assert region is not None

class TestBrainStatistics:
    """Test brain statistics and metrics."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        self.brain = nlm.Brain(config)
        assert self.brain.initialize()
    
    def test_neuron_statistics(self):
        """Test neuron-related statistics."""
        brain = self.brain
        
        # Basic statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() >= 0
        assert brain.getActiveNeuronCount() >= 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getExcitationInhibitionRatio() >= 0.0
        assert brain.getTotalSpikeCount() >= 0
    
    def test_developmental_stage(self):
        """Test developmental stage management."""
        brain = self.brain
        
        # Get current stage
        stage = brain.getDevelopmentalStage()
        assert stage in [
            nlm.DevelopmentalStage.Initial,
            nlm.DevelopmentalStage.CriticalPeriod,
            nlm.DevelopmentalStage.Maturation,
            nlm.DevelopmentalStage.Adult,
            nlm.DevelopmentalStage.Aging
        ]
        
        # Set different stages
        for test_stage in [
            nlm.DevelopmentalStage.CriticalPeriod,
            nlm.DevelopmentalStage.Maturation,
            nlm.DevelopmentalStage.Adult
        ]:
            brain.setDevelopmentalStage(test_stage)
            assert brain.getDevelopmentalStage() == test_stage
    
    def test_brain_save_load(self):
        """Test brain state save and load."""
        brain = self.brain
        
        # Record initial state
        initial_neurons = brain.getTotalNeuronCount()
        
        # Save to temporary file
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
            temp_file = f.name
        
        try:
            # Save
            assert brain.save(temp_file)
            assert os.path.exists(temp_file)
            assert os.path.getsize(temp_file) > 0
            
            # Create new brain with same config
            config = brain.getConfig()
            brain2 = nlm.Brain(config)
            
            # Load into new brain
            assert brain2.load(temp_file)
            
            # Verify loaded state
            assert brain2.getTotalNeuronCount() == initial_neurons
            
        finally:
            if os.path.exists(temp_file):
                os.unlink(temp_file)

class TestPythonicBrainFeatures:
    """Test Pythonic brain features."""
    
    def setup_method(self):
        """Setup for each test method."""
        config = nlm.create_simulation_config()
        self.brain = nlm.Brain(config)
        assert self.brain.initialize()
    
    def test_run_method(self):
        """Test run method (simulated)."""
        # The run method would simulate running for a duration
        # For testing, we just verify it exists and has the right signature
        assert hasattr(self.brain, 'run')
        
        # In a real test, we would call it and verify it runs
        # For now, we just test the interface exists
        pass
    
    def test_get_neurons_by_type(self):
        """Test getNeuronsByType method."""
        # Verify method exists
        assert hasattr(self.brain, 'getNeuronsByType')
        
        # Call with different neuron types
        neurons_exc = self.brain.getNeuronsByType(nlm.NeuronType.Excitatory)
        assert isinstance(neurons_exc, list)
        
        neurons_inh = self.brain.getNeuronsByType(nlm.NeuronType.Inhibitory)
        assert isinstance(neurons_inh, list)
        
        # Test with filters
        neurons_filtered = self.brain.getNeuronsByType(
            nlm.NeuronType.Excitatory,
            start=0,
            count=10
        )
        assert isinstance(neurons_filtered, list)
    
    def test_get_connections(self):
        """Test getConnections method."""
        # Verify method exists
        assert hasattr(self.brain, 'getConnections')
        
        # Get all connections
        connections = self.brain.getConnections()
        assert isinstance(connections, list)
        
        # Get filtered connections
        connections_filtered = self.brain.getConnections(
            source=nlm.INVALID_REGION_ID,
            target=nlm.INVALID_REGION_ID
        )
        assert isinstance(connections_filtered, list)
    
    def test_benchmark(self):
        """Test benchmark method."""
        # Verify method exists
        assert hasattr(self.brain, 'benchmark')
        
        # Run benchmark
        results = self.brain.benchmark(steps=5, warmup=2)
        
        # Verify results
        assert isinstance(results, dict)
        assert "total_time_seconds" in results
        assert "average_time_per_step_seconds" in results
        assert "steps_per_second" in results
        assert "steps" in results
        
        assert results["steps"] == 5
        assert results["total_time_seconds"] >= 0
        assert results["average_time_per_step_seconds"] >= 0
        assert results["steps_per_second"] > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v"])