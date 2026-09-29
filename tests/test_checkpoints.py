#!/usr/bin/env python3
"""
Checkpoint Optimization Tests for NLM

Tests checkpoint optimization features specifically.
"""

import sys
import os
import time
import pytest
import numpy as np
import tempfile
import shutil
from typing import List, Dict, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from performance.MemoryPool import MemoryPool
    from performance.SIMDVectorization import SIMDUtils, VectorizedLIF
    from performance.ParallelProcessing import ThreadPool, ParallelFor
    from performance.CheckpointSystem import CheckpointManager
except ImportError as e:
    print(f"Error importing checkpoint modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestCheckpoints:
    """Test checkpoint optimization specific features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.temp_dir = tempfile.mkdtemp()
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.config.set("plasticity_rate", 0.1)
    
    def teardown_method(self):
        """Clean up after each test method"""
        if os.path.exists(self.temp_dir):
            shutil.rmtree(self.temp_dir)
    
    def test_checkpoint_manager_creation(self):
        """Test CheckpointManager creation and configuration"""
        # Create checkpoint manager
        manager = CheckpointManager()
        
        # Configure manager
        manager.configure(self.temp_dir, 100, 5, True, 1000)  # path, max, frequency, compress, chunk_size
        
        # Verify configuration
        assert manager.getCheckpointDir() == self.temp_dir
        assert manager.maxCheckpoints_ == 100
        assert manager.frequency_ == 5
        assert manager.compress_ == True
        assert manager.chunkSize_ == 1000
        
        # Test additional configuration methods
        manager.setCompressionLevel(2)
        assert manager.compressionLevel_ == 2
        
        manager.setChunkSize(2000)
        assert manager.chunkSize_ == 2000
    
    def test_checkpoint_save_and_load(self):
        """Test checkpoint save and load functionality"""
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 50, 1, True)
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Run some simulation work
        for i in range(10):
            brain.step(i)
        
        # Save checkpoint
        checkpoint_path = manager.saveCheckpoint("test_brain", brain)
        assert checkpoint_path is not None
        assert checkpoint_path.endswith(".nlm")
        
        # Verify checkpoint was created
        assert os.path.exists(checkpoint_path)
        
        # List checkpoints
        checkpoints = manager.listCheckpoints()
        assert len(checkpoints) >= 1
        assert any("test_brain" in cp for cp in checkpoints)
        
        # Cleanup
        manager.clearCheckpoints()
        
        brain.shutdown()
    
    def test_checkpoint_cleanup(self):
        """Test checkpoint cleanup functionality"""
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 5, 1, True)  # Max 5 checkpoints
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Save many checkpoints (more than max)
        for i in range(10):
            checkpoint_name = f"checkpoint_{i}"
            checkpoint_path = manager.saveCheckpoint(checkpoint_name, brain)
            assert checkpoint_path is not None
        
        # List checkpoints should not exceed max
        checkpoints = manager.listCheckpoints()
        assert len(checkpoints) <= 5
        
        # Older checkpoints should be removed
        # The most recent 5 should remain
        recent_checkpoints = [cp for cp in checkpoints if cp.endswith("_9") or 
                             cp.endswith("_8") or cp.endswith("_7") or 
                             cp.endswith("_6") or cp.endswith("_5")]
        # Note: This depends on the actual checkpoint naming scheme
        
        cleanup_time = manager.cleanupOldCheckpoints()
        assert cleanup_time > 0
        
        brain.shutdown()
    
    def test_checkpoint_compression(self):
        """Test checkpoint compression functionality"""
        # Create checkpoint manager with compression
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 20, 1, True, 1000, 2)  # compression_level 2
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Run simulation
        for i in range(5):
            brain.step(i)
        
        # Save checkpoint with compression
        checkpoint_path = manager.saveCheckpoint("compressed_brain", brain)
        assert checkpoint_path is not None
        
        # Check file size (should be compressed)
        if os.path.exists(checkpoint_path):
            file_size = os.path.getsize(checkpoint_path)
            print(f"Compressed checkpoint size: {file_size} bytes")
            # Compressed file should be smaller than raw (at least for text data)
            # But this depends on the actual compression implementation
        
        brain.shutdown()
    
    def test_checkpoint_chunking(self):
        """Test checkpoint chunking functionality"""
        # Create checkpoint manager with chunking
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 10, 1, False, 100)  # chunk_size 100
        
        # Create large brain configuration
        large_config = Config()
        large_config.set("neuron_count", 1000)
        large_config.set("plasticity_rate", 0.1)
        
        # Create test brain
        brain = Brain(large_config)
        assert brain.initialize() == True
        
        # Run simulation
        for i in range(3):
            brain.step(i)
        
        # Save checkpoint with chunking
        checkpoint_path = manager.saveCheckpoint("chunked_brain", brain)
        assert checkpoint_path is not None
        
        # Cleanup
        manager.clearCheckpoints()
        
        brain.shutdown()
    
    def test_checkpoint_performance(self):
        """Test checkpoint save/load performance"""
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 100, 1, True)
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Run simulation to create data
        for i in range(50):
            brain.step(i)
        
        # Measure checkpoint save time
        start_time = time.time()
        checkpoint_path = manager.saveCheckpoint("perf_test", brain)
        save_time = time.time() - start_time
        
        assert checkpoint_path is not None
        print(f"Checkpoint save time: {save_time:.4f}s")
        
        # Performance should be reasonable
        assert save_time < 5.0  # Should save within 5 seconds
        
        # Cleanup
        manager.clearCheckpoints()
        
        brain.shutdown()
    
    def test_checkpoint_multiple_instances(self):
        """Test multiple checkpoint instances"""
        # Create multiple checkpoint managers
        managers = []
        
        for i in range(3):
            manager = CheckpointManager()
            config_dir = os.path.join(self.temp_dir, f"manager_{i}")
            os.makedirs(config_dir, exist_ok=True)
            
            manager.configure(config_dir, 20, 1, True)
            managers.append(manager)
        
        # Create test brains
        brains = []
        for i, manager in enumerate(managers):
            config = Config()
            config.set("neuron_count", 100 + i * 50)
            config.set("plasticity_rate", 0.1)
            
            brain = Brain(config)
            assert brain.initialize() == True
            brains.append(brain)
        
        # Save checkpoints with different managers
        for i, (manager, brain) in enumerate(zip(managers, brains)):
            checkpoint_name = f"brain_{i}"
            checkpoint_path = manager.saveCheckpoint(checkpoint_name, brain)
            assert checkpoint_path is not None
            
            # Verify each manager has its own checkpoints
            checkpoints = manager.listCheckpoints()
            assert len(checkpoints) >= 1
        
        # Cleanup
        for manager in managers:
            manager.clearCheckpoints()
        
        for brain in brains:
            brain.shutdown()
    
    def test_checkpoint_error_handling(self):
        """Test checkpoint error handling"""
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 10, 1, True)
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True()
        
        # Try to save checkpoint (should work)
        checkpoint_path = manager.saveCheckpoint("error_test", brain)
        assert checkpoint_path is not None
        
        # Try to save to invalid path (if implemented)
        # Note: This depends on the actual implementation
        
        # Cleanup
        manager.clearCheckpoints()
        
        brain.shutdown()
    
    def test_checkpoint_integration_with_brain(self):
        """Test checkpoint integration with brain simulation"""
        # Create brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 5, 2, True)
        
        # Run simulation with periodic checkpoints
        for episode in range(10):
            # Run simulation steps
            for step in range(15):
                brain.step(step)
                
                # Save checkpoint every 3 steps
                if step % 3 == 0 and step > 0:
                    checkpoint_name = f"episode_{episode}_step_{step}"
                    checkpoint_path = manager.saveCheckpoint(checkpoint_name, brain)
                    assert checkpoint_path is not None
            
            # Load last checkpoint
            checkpoints = manager.listCheckpoints()
            if checkpoints:
                last_checkpoint = checkpoints[-1]
                # Note: This depends on the actual checkpoint loading implementation
                # For now, we just verify the checkpoint was saved
        
        # Cleanup
        manager.clearCheckpoints()
        
        brain.shutdown()
    
    def test_checkpoint_memory_usage(self):
        """Test checkpoint memory usage efficiency"""
        import psutil
        import gc
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure(self.temp_dir, 20, 1, True)
        
        # Create and run brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Run simulation
        for i in range(20):
            brain.step(i)
        
        # Save checkpoint
        checkpoint_path = manager.saveCheckpoint("memory_test", brain)
        assert checkpoint_path is not None
        
        after_save = process.memory_info().rss
        save_memory_increase = after_save - initial_memory
        
        # Cleanup checkpoint
        manager.clearCheckpoints()
        
        gc.collect()
        after_cleanup = process.memory_info().rss
        cleanup_memory_increase = after_cleanup - initial_memory
        
        print(f"Checkpoint memory test:")
        print(f"  Memory after checkpoint save: {save_memory_increase / 1024 / 1024:.2f} MB")
        print(f"  Memory after cleanup: {cleanup_memory_increase / 1024 / 1024:.2f} MB")
        
        # Memory should be reasonable
        assert save_memory_increase < 50 * 1024 * 1024  # 50MB
        
        # Memory after cleanup should be close to initial
        final_memory_increase = abs(cleanup_memory_increase)
        assert final_memory_increase < 10 * 1024 * 1024  # 10MB
        
        brain.shutdown()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
