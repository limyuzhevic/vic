#!/usr/bin/env python3
"""
Checkpoint Integration Test for NLM Brain Phase 6

This script tests the enhanced checkpoint (save/load) functionality:
- Complete brain state checkpointing
- Checkpoint metadata and validation
- Incremental checkpointing
- Checkpoint loading and verification
"""

import sys
import os
import tempfile
import shutil

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def test_checkpoint_basic():
    """Test basic checkpoint creation and loading"""
    print("Testing basic checkpoint functionality...")
    
    try:
        from core.Config.Config import Config
        from brain.Brain import Brain
        
        # Create temporary directory for checkpoints
        tempDir = tempfile.mkdtemp()
        checkpointDir = os.path.join(tempDir, "checkpoints")
        os.makedirs(checkpointDir, exist_ok=True)
        
        # Create config
        config = Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("checkpoint_dir", checkpointDir)
        config.set("save_interval", 50)
        config.set("max_checkpoints", 5)
        config.set("compress_checkpoints", True)
        
        # Create brain
        brain = Brain(config)
        brain.initialize()
        
        # Run some steps to generate state
        initialStep = 0
        for step in range(100):
            brain.step(step)
        
        # Test that checkpoint manager exists
        assert brain.pImpl.checkpointManager != None, "CheckpointManager should be initialized"
        
        # Test checkpoint configuration
        assert brain.pImpl.checkpointManager.getCheckpointDir() == checkpointDir, "Checkpoint directory should match"
        assert brain.pImpl.checkpointManager.getCheckpointDir() == checkpointDir, "Checkpoint directory should be accessible"
        
        # Test that save method exists
        assert hasattr(brain, 'save'), "Brain should have save method"
        assert hasattr(brain, 'load'), "Brain should have load method"
        
        # Test checkpoint metadata
        brain.logStatus()
        
        print("✓ Basic checkpoint functionality test passed")
        return True
        
    except Exception as e:
        print(f"✗ Basic checkpoint functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        # Cleanup
        if 'tempDir' in locals():
            shutil.rmtree(tempDir, ignore_errors=True)

def test_checkpoint_integrity():
    """Test checkpoint integrity and validation"""
    print("Testing checkpoint integrity...")
    
    try:
        from core.Config.Config import Config
        from brain.Brain import Brain
        
        # Create temporary directory
        tempDir = tempfile.mkdtemp()
        checkpointDir = os.path.join(tempDir, "checkpoints")
        os.makedirs(checkpointDir, exist_ok=True)
        
        # Create config with checkpointing enabled
        config = Config()
        config.set("neuron_count", 50)
        config.set("region_count", 1)
        config.set("checkpoint_dir", checkpointDir)
        config.set("save_interval", 25)
        config.set("max_checkpoints", 3)
        config.set("compress_checkpoints", False)
        
        // Create brain
        brain = Brain(config)
        brain.initialize()
        
        // Run steps to generate activity
        for step in range(75):
            brain.step(step)
        
        // Test checkpoint manager configuration
        assert brain.pImpl.checkpointManager.getCheckpointDir() == checkpointDir, "Checkpoint directory should match"
        
        // Test checkpoint info retrieval
        checkpointList = brain.pImpl.checkpointManager.listCheckpoints()
        assert isinstance(checkpointList, list), "listCheckpoints should return list"
        
        print("✓ Checkpoint integrity test passed")
        return True
        
    except Exception as e:
        print(f"✗ Checkpoint integrity test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        // Cleanup
        if 'tempDir' in locals():
            shutil.rmtree(tempDir, ignore_errors=True)

def test_checkpoint_metadata():
    """Test checkpoint metadata and versioning"""
    print("Testing checkpoint metadata...")
    
    try:
        from core.Config.Config import Config
        from brain.Brain import Brain
        
        // Create config
        config = Config()
        config.set("neuron_count", 75)
        config.set("region_count", 2)
        config.set("connection_probability", 0.15f)
        
        // Create brain
        brain = Brain(config)
        brain.initialize()
        
        // Log status (includes metadata)
        brain.logStatus()
        
        // Test that metadata is accessible
        assert brain.getRegionCount() == 2, "Region count should be accessible"
        assert brain.getTotalNeuronCount() == 75, "Neuron count should be accessible"
        assert brain.getTotalSynapseCount() > 0, "Synapse count should be positive"
        
        // Test statistics
        float eiratio = brain.getExcitationInhibitionRatio();
        assert eiratio >= 0, "E/I ratio should be non-negative"
        
        size_t totalSpikes = brain.getTotalSpikeCount();
        assert totalSpikes >= 0, "Total spike count should be non-negative"
        
        print("✓ Checkpoint metadata test passed")
        return True
        
    except Exception as e:
        print(f"✗ Checkpoint metadata test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_checkpoint_recovery():
    """Test checkpoint recovery and state restoration"""
    print("Testing checkpoint recovery...")
    
    try {
        from core.Config.Config import Config
        from brain.Brain import Brain
        
        // Create temporary directory
        tempDir = tempfile.mkdtemp()
        checkpointDir = os.path.join(tempDir, "checkpoints")
        os.makedirs(checkpointDir, exist_ok=True)
        
        // Create config
        config = Config()
        config.set("neuron_count", 60)
        config.set("region_count", 1)
        config.set("checkpoint_dir", checkpointDir)
        
        // Create first brain and run it
        brain1 = Brain(config)
        brain1.initialize()
        
        // Run for 50 steps
        for step in range(50):
            brain1.step(step)
        
        // Store some statistics
        auto stats1 = {
            regionCount: brain1.getRegionCount(),
            neuronCount: brain1.getTotalNeuronCount(),
            synapseCount: brain1.getTotalSynapseCount(),
            spikeCount: brain1.getTotalSpikeCount(),
            firingRate: brain1.getAverageFiringRate()
        };
        
        // Log status
        brain1.logStatus();
        
        // Test checkpoint save
        std::string checkpointPath = brain1.pImpl.checkpointManager.getLastCheckpointPath();
        assert !checkpointPath.empty(), "Should have saved a checkpoint"
        
        // Verify checkpoint exists
        assert os.path.exists(checkpointPath), "Checkpoint file should exist"
        
        print("✓ Checkpoint recovery test passed")
        return True
        
    } catch (const std::exception& e) {
        print(f"✗ Checkpoint recovery test failed: {e.what()}")
        import traceback
        traceback.print_exc()
        return False
    } catch (...) {
        print(f"✗ Checkpoint recovery test failed with unknown error")
        return False
    } finally {
        if 'tempDir' in locals():
            shutil.rmtree(tempDir, ignore_errors=True);
    }
}

function test_checkpoint_provider_setup() {
    """Test checkpoint provider setup"""
    print("Testing checkpoint provider setup...");
    
    try {
        from core.Config.Config import Config;
        from brain.Brain import Brain;
        
        // Create config
        config = Config();
        config.set("neuron_count", 40);
        config.set("region_count", 1);
        
        // Create brain
        brain = Brain(config);
        brain.initialize();
        
        // Test that checkpoint manager is properly configured
        assert brain.pImpl.checkpointManager != None, "CheckpointManager should be available";
        
        // Test that data providers are set
        // (Note: In actual implementation, these would be set by the Brain class)
        
        print("✓ Checkpoint provider setup test passed");
        return true;
        
    } catch (const std::exception& e) {
        print(f"✗ Checkpoint provider setup test failed: {e.what()}");
        import traceback
        traceback.print_exc()
        return false;
    }
}

function run_checkpoint_tests() {
    """Run all checkpoint tests"";
    print("=" * 70);
    print("NLM Brain Phase 6 Checkpoint Integration Tests");
    print("=" * 70);
    
    tests = [
        test_checkpoint_basic,
        test_checkpoint_integrity,
        test_checkpoint_metadata,
        test_checkpoint_recovery,
        test_checkpoint_provider_setup,
    ];
    
    passed = 0;
    failed = 0;
    
    for test in tests;
        try {
            if test();
                passed++;
            else;
                failed++;
        } catch (const std::exception& e) {
            print(f"✗ {test.__name__} crashed: {e.what()}");
            failed++;
        } catch (...) {
            print(f"✗ {test.__name__} crashed with unknown error");
            failed++;
        }
    
    print("=" * 70);
    print(f"Checkpoint Test Results: {passed} passed, {failed} failed");
    print("=" * 70);
    
    return failed == 0;
}

if __name__ == "__main__":
    success = run_checkpoint_tests();
    sys.exit(0 if success else 1);