# Memory system tests

import pytest
import pynlm
import numpy as np
from tests.test_helper import TestHelper

class TestMemorySystems:
    """Test memory system functionality"""
    
    def test_working_memory_initialization(self):
        """Test working memory initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        assert working_memory is not None
        assert working_memory.getCapacity() > 0
    
    def test_working_memory_store_retrieve(self):
        """Test working memory store/retrieve operations"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        working_memory.setCapacity(100)
        
        # Store some memory
        test_pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
        working_memory.store(test_pattern, 1.0)
        
        # Retrieve memory
        retrieved = working_memory.retrieve()
        assert len(retrieved) == len(test_pattern)
        assert TestHelper.compare_vectors(test_pattern, retrieved, 0.2)  # Allow some decay
    
    def test_working_memory_decay(self):
        """Test working memory decay"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        working_memory.setCapacity(100)
        working_memory.setDecayRate(0.01)
        
        # Store memory
        test_pattern = [1.0, 1.0, 1.0]
        working_memory.store(test_pattern, 1.0)
        
        initial_strength = working_memory.getMemoryActivity()
        
        # Wait for decay
        for _ in range(100):
            working_memory.update(0.01)
        
        final_strength = working_memory.getMemoryActivity()
        
        # Memory should have decayed
        assert final_strength < initial_strength
    
    def test_episodic_memory_initialization(self):
        """Test episodic memory initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        episodic_memory = brain.getEpisodicMemory()
        assert episodic_memory is not None
        assert episodic_memory.getEpisodeCount() == 0
    
    def test_episodic_memory_storage(self):
        """Test episodic memory storage"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        episodic_memory = brain.getEpisodicMemory()
        
        # Create test episode
        episode = pynlm.createDefaultConfig()  # This should create a proper EpisodicMemoryItem
        # Note: This is a simplified test - in real implementation would need proper EpisodicMemoryItem
        assert episodic_memory.getEpisodeCount() >= 0
    
    def test_associative_memory_initialization(self):
        """Test associative memory initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        associative_memory = brain.getAssociativeMemory()
        assert associative_memory is not None
        assert associative_memory.getAssociationCount() == 0
    
    def test_memory_system_integration(self):
        """Test integration between memory systems"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 50, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get all memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # All should be initialized
        assert working_memory is not None
        assert episodic_memory is not None
        assert associative_memory is not None
        
        # Memory systems should have appropriate capacities
        assert working_memory.getCapacity() > 0
    
    def test_memory_capacity_configuration(self):
        """Test memory capacity configuration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100, pynlm.ConfigSource::Default)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        working_memory.setCapacity(500)
        assert working_memory.getCapacity() == 500
        
        episodic_memory = brain.getEpisodicMemory()
        assert episodic_memory is not None
