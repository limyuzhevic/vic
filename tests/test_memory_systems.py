"""Memory System Tests

Test that memory systems are accessible via Python bindings.
"""

import pytest
import pynlm
import numpy as np


class TestMemorySystems:
    """Test memory system functionality through Python bindings."""
    
    def test_working_memory_access(self):
        """Test working memory system access."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Access brain regions
        brain.addRegion("working_memory")
        brain.addRegion("episodic_memory")
        brain.addRegion("semantic_memory")
        brain.addRegion("procedural_memory")
        
        # Test region management
        region_count = brain.getRegionCount()
        assert region_count == 4
        
        region_ids = brain.getRegionIds()
        assert len(region_ids) == 4
        
        # Test getting specific regions
        if region_ids:
            region = brain.getRegion(region_ids[0])
            assert region is not None
        
        # Test neural population statistics (working memory related)
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        
        # These should be 0 until we add neurons
        assert total_neurons == 0
        assert total_synapses == 0
    
    def test_neuromodulation_systems(self):
        """Test neuromodulation system access."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get neuromodulation statistics
        avg_firing_rate = brain.getAverageFiringRate()
        excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
        total_spike_count = brain.getTotalSpikeCount()
        
        # These are float values
        assert isinstance(avg_firing_rate, float)
        assert isinstance(excitation_inhibition_ratio, float)
        assert isinstance(total_spike_count, (int, float))
        
        # Test developmental stage affects neuromodulation
        stage = brain.getDevelopmentalStage()
        assert stage in [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging,
        ]
        
        # Test setting developmental stage
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        stage = brain.getDevelopmentalStage()
        assert stage == pynlm.DevelopmentalStage.CriticalPeriod
    
    def test_memory_system_constants(self):
        """Test memory-related constants and invalid IDs."""
        # Test invalid neuron ID access
        invalid_neuron_id = pynlm.INVALID_NEURON_ID
        assert invalid_neuron_id.value == 0
        
        # Test neuron ID methods
        assert invalid_neuron_id.index() == 0
        
        # Test equality operations
        neuron_id1 = pynlm.NeuronId()
        neuron_id2 = pynlm.NeuronId()
        assert neuron_id1 == neuron_id2  # Both should be zero
        
        # Test hash (should not crash)
        hash(neuron_id1)
        
        # Test invalid synapse ID
        invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
        assert invalid_synapse_id.value == 0
        
        # Test invalid region ID
        invalid_region_id = pynlm.INVALID_REGION_ID
        assert invalid_region_id.value == 0
        
        # Test invalid population ID
        invalid_pop_id = pynlm.INVALID_POPULATION_ID
        assert invalid_pop_id.value == 0
    
    def test_memory_system_types(self):
        """Test memory system related type information."""
        # Test neuron types
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Modulatory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal,
        ]
        
        # Test synapse types
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
            pynlm.SynapseType.Electrical,
            pynlm.SynapseType.GapJunction,
        ]
        
        # Test that all types are accessible
        for nt in neuron_types:
            assert nt is not None
            
        for st in synapse_types:
            assert st is not None
    
    def test_memory_system_stats(self):
        """Test memory system statistics and metrics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test various brain statistics
        stats = {
            'total_neuron_count': brain.getTotalNeuronCount(),
            'total_synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'region_count': brain.getRegionCount(),
        }
        
        # Verify all stats are present and have appropriate types
        assert isinstance(stats['total_neuron_count'], (int, np.integer))
        assert isinstance(stats['total_synapse_count'], (int, np.integer))
        assert isinstance(stats['active_neuron_count'], (int, np.integer))
        assert isinstance(stats['firing_neuron_count'], (int, np.integer))
        assert isinstance(stats['average_firing_rate'], (float, np.floating))
        assert isinstance(stats['excitation_inhibition_ratio'], (float, np.floating))
        assert isinstance(stats['total_spike_count'], (int, float, np.integer, np.floating))
        assert isinstance(stats['region_count'], (int, np.integer))
        
        # Verify ratios are within reasonable bounds
        if stats['average_firing_rate'] is not None:
            assert stats['average_firing_rate'] >= 0.0
        
        if stats['excitation_inhibition_ratio'] is not None:
            assert stats['excitation_inhibition_ratio'] >= 0.0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
