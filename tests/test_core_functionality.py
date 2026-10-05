import pytest
from pynlm import createBrain, createDefaultConfig

def test_basic_brain_creation():
    """Test basic brain creation and initialization"""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Check basic properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getTotalSpikeCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    
    # Check that we can step the brain
    initial_spikes = brain.getTotalSpikeCount()
    for step in range(10):
        brain.step(step)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes

def test_configuration():
    """Test configuration management"""
    config = createDefaultConfig()
    
    # Test has method
    assert config.has("brain.neuron_count")
    assert config.has("brain.region_count")
    assert config.has("brain.connection_probability")
    
    # Test getOr method
    neuron_count = config.getOr[int64_t]("brain.neuron_count", 1000)
    assert neuron_count > 0
    
    region_count = config.getOr[int64_t]("brain.region_count", 1)
    assert region_count > 0
    
    # Test keys
    keys = config.getKeys()
    assert len(keys) > 0
    
    # Test summary
    summary = config.summary()
    assert len(summary) > 0

def test_region_operations():
    """Test brain region operations"""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Add a region
    region_id = brain.addRegion("test_region")
    assert region_id.value >= 0
    
    # Get region
    region = brain.getRegion(region_id)
    assert region is not None
    
    # Check region count
    assert brain.getRegionCount() > 0
    
    # Get region IDs
    region_ids = brain.getRegionIds()
    assert len(region_ids) > 0

def test_development_system():
    """Test development system"""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage.value >= 0
    
    # Set developmental stage
    brain.setDevelopmentalStage(stage)
    assert brain.getDevelopmentalStage().value >= 0

def test_system_stats():
    """Test system statistics"""
    from pynlm import getSystemStats
    
    stats = getSystemStats()
    assert len(stats) > 0
    assert "NLM" in stats
    assert "Phase 6" in stats

def test_error_handling():
    """Test error handling"""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Test reset
    brain.reset()
    assert brain.getTotalSpikeCount() >= 0
    
    # Test step with different parameters
    brain.step(0)
    brain.step(1, 0.001)
    
    # Test region operations
    region_count = brain.getRegionCount()
    total_neurons = brain.getTotalNeuronCount()
    
    assert region_count >= 0
    assert total_neurons > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
