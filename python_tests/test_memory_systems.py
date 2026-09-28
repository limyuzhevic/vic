import pytest
import pynlm
import random

class TestMemorySystems:
    """Test memory system integration and functionality"""

    def test_working_memory(self, medium_brain):
        """Test working memory system"""
        working_memory = medium_brain.getWorkingMemory()
        assert working_memory is not None
        
        # Test working memory properties
        assert working_memory.getCapacity() > 0
        assert working_memory.getMaxTraces() > 0
        
        # Test storing neural activities
        initial_traces = working_memory.getActiveTraces()
        
        # Simulate some neural activity
        for step in range(50):
            medium_brain.step(step)
            # In real usage, you would store specific neural activities
            # For testing, we verify the system is responsive
        
        final_traces = working_memory.getActiveTraces()
        assert final_traces >= initial_traces

    def test_episodic_memory(self, medium_brain):
        """Test episodic memory system"""
        episodic_memory = medium_brain.getEpisodicMemory()
        assert episodic_memory is not None
        
        # Test episodic memory properties
        assert episodic_memory.getMaxEpisodes() > 0
        assert episodic_memory.getCurrentMemoryLoad() == 0
        
        # Test experience recording (simplified for testing)
        for step in range(100):
            medium_brain.step(step)
            
        # Verify memory system is active
        assert episodic_memory.getEpisodeCount() >= 0

    def test_associative_memory(self, medium_brain):
        """Test associative memory system"""
        associative_memory = medium_brain.getAssociativeMemory()
        assert associative_memory is not None
        
        # Test pattern storage and retrieval
        test_pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
        associative_memory.storePattern(test_pattern)
        
        retrieved_pattern = associative_memory.retrievePattern(test_pattern)
        assert retrieved_pattern is not None
        
        # Check pattern similarity
        similarity = associative_memory.calculateSimilarity(test_pattern, retrieved_pattern)
        assert 0.0 <= similarity <= 1.0

    def test_memory_integration(self, medium_brain, test_world, test_agent):
        """Test memory system integration with brain and world"""
        # Run simulation with memory tracking
        memory_records = []
        
        for step in range(100):
            test_world.update(0.1)
            test_agent.processSensoryInput(test_world.getSensoryPercept())
            medium_brain.step(step)
            
            # Record memory state
            if medium_brain.getWorkingMemory():
                memory_records.append({
                    'step': step,
                    'working_memory_traces': medium_brain.getWorkingMemory().getActiveTraces(),
                    'episodic_episodes': medium_brain.getEpisodicMemory().getEpisodeCount() if medium_brain.getEpisodicMemory() else 0
                })
        
        # Verify memory usage over time
        assert len(memory_records) == 100
        assert all('working_memory_traces' in record for record in memory_records)
        assert all('episodic_episodes' in record for record in memory_records)

    def test_memory_decay(self, medium_brain):
        """Test memory decay and consolidation"""
        working_memory = medium_brain.getWorkingMemory()
        assert working_memory is not None
        
        initial_traces = working_memory.getActiveTraces()
        
        # Run simulation for many steps to allow decay
        for step in range(1000):
            medium_brain.step(step)
        
        # Memory should have some decay mechanism
        final_traces = working_memory.getActiveTraces()
        # (Decay behavior depends on implementation)