#!/usr/bin/env python3
"""
Enhanced Python bindings test script for NLM (Neural Learning Machine)

This script demonstrates the new advanced interfaces and utilities
added to the NLM Python bindings in Phase 6.
"""

import sys
import os
import tempfile
import shutil
import json

# Add the path to the NLM Python bindings
sys.path.insert(0, '/workspace/app/python')

try:
    import pynlm
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM Python bindings: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality with enhanced bindings"""
    print("\n=== Testing Basic NLM Functionality ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print("✓ Created default configuration")
    
    # Create brain
    brain = pynlm.createBrain(config)
    print("✓ Created brain instance")
    
    # Create world
    world = pynlm.createSimpleWorld()
    print("✓ Created simple world")
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain interface")
    
    # Initialize brain
    if brain.initialize():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        return False
    
    # Test basic statistics
    stats = pynlm.getBrainStatistics(brain)
    print(f"✓ Brain statistics retrieved: {dict(stats)}")
    
    # Test configuration utilities
    config_summary = pynlm.getConfigSummary(config)
    print(f"✓ Configuration summary: {config_summary}")
    
    return True

def test_advanced_checkpointing():
    """Test advanced checkpointing features"""
    print("\n=== Testing Advanced Checkpointing ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for checkpointing test")
        return False
    
    # Create temporary directory for checkpoints
    temp_dir = tempfile.mkdtemp(prefix="nlm_checkpoints_")
    print(f"✓ Created checkpoint directory: {temp_dir}")
    
    try:
        # Run some simulation steps
        for i in range(50):
            brain.step(i)
        
        # Test save/load checkpoint
        checkpoint_file = os.path.join(temp_dir, "test_checkpoint.bin")
        
        if brain.save(checkpoint_file):
            print(f"✓ Brain checkpoint saved: {checkpoint_file}")
        else:
            print("✗ Failed to save checkpoint")
            return False
        
        # Test checkpoint listing
        checkpoints = pynlm.listCheckpoints(temp_dir)
        print(f"✓ Checkpoint listing: {len(checkpoints)} checkpoints found")
        
        # Create new brain and load checkpoint
        brain2 = pynlm.createBrain(config)
        
        if brain2.load(checkpoint_file):
            print("✓ Brain checkpoint loaded successfully")
            
            # Verify statistics are preserved
            stats1 = pynlm.getBrainStatistics(brain)
            stats2 = pynlm.getBrainStatistics(brain2)
            
            if stats1['total_spikes'] == stats2['total_spikes']:
                print("✓ Checkpoint integrity verified")
            else:
                print("✗ Checkpoint integrity check failed")
                return False
        else:
            print("✗ Failed to load checkpoint")
            return False
            
    finally:
        # Clean up
        shutil.rmtree(temp_dir)
        print(f"✓ Cleaned up temporary directory: {temp_dir}")
    
    return True

def test_memory_utilities():
    """Test memory tracking and episode utilities"""
    print("\n=== Testing Memory Utilities ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for memory test")
        return False
    
    # Run some simulation steps
    for i in range(20):
        # Create some sensory input
        sensory_data = [0.5 * (i % 5), 0.3 * (i % 3), 0.8 * (i % 2)]
        
        # For demonstration, just test that we can access memory info
        # In a real scenario, you'd use proper sensory input
        # brain.receiveSensoryInput(pynlm.Vision(sensory_data))
        # brain.step(i)
        
        # Simulate some neural activity
        brain.step(i)
    
    # Get brain memory information
    memory_info = pynlm.brainMemoryInfo(brain)
    print(f"✓ Memory info retrieved: {dict(memory_info)}")
    
    # Get brain history (memory)
    brain_history = pynlm.getBrainHistory(brain)
    print(f"✓ Brain history retrieved with keys: {list(brain_history.keys())}")
    
    if 'working_memory' in brain_history:
        print("✓ Working memory data available")
    if 'episodes' in brain_history:
        print(f"✓ Episodic memory contains {len(brain_history['episodes'])} episodes")
    
    return True

def test_developmental_utilities():
    """Test developmental stage utilities"""
    print("\n=== Testing Developmental Utilities ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for developmental test")
        return False
    
    # Test developmental report
    dev_report = pynlm.developmentalReport(brain)
    print(f"✓ Developmental report: {dict(dev_report)}")
    
    # Test developmental stage methods
    current_stage = brain.getDevelopmentalStage()
    print(f"✓ Current developmental stage: {current_stage}")
    
    # Test development system access
    dev_system = brain.getDevelopmentSystem()
    if dev_system:
        print("✓ Development system accessible")
    
    # Test neuromodulation levels
    curiosity = brain.getCuriosityLevel()
    novelty = brain.getNoveltyLevel()
    dopamine = brain.getNeuromodulationLevel()
    
    print(f"✓ Neuromodulation levels - Curiosity: {curiosity}, Novelty: {novelty}, Dopamine: {dopamine}")
    
    return True

def test_performance_monitoring():
    """Test performance monitoring utilities"""
    print("\n=== Testing Performance Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for performance test")
        return False
    
    # Test performance monitoring
    perf_results = pynlm.performanceMonitor(brain, 30)
    print(f"✓ Performance monitoring results: {dict(perf_results)}")
    
    # Verify performance metrics
    required_keys = ['avg_firing_rate', 'total_spikes', 'steps_processed']
    if all(key in perf_results for key in required_keys):
        print("✓ All required performance metrics present")
        return True
    else:
        print("✗ Missing performance metrics")
        return False

def test_batch_operations():
    """Test batch processing capabilities"""
    print("\n=== Testing Batch Operations ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for batch test")
        return False
    
    # Create test inputs
    test_inputs = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.6, 0.7, 0.8, 0.9, 1.0],
        [0.0, 0.5, 0.0, 0.5, 0.0]
    ]
    
    # Test batch processing
    temp_dir = tempfile.mkdtemp(prefix="nlm_batch_")
    print(f"✓ Created batch processing directory: {temp_dir}")
    
    try:
        results = pynlm.batchProcess(brain, test_inputs, True, temp_dir)
        print(f"✓ Batch processing completed: {len(results)} results")
        print(f"  Results: {results}")
        
        # Verify checkpoints were created
        checkpoint_files = [f for f in os.listdir(temp_dir) if f.endswith('.bin')]
        print(f"✓ Created {len(checkpoint_files)} checkpoint files")
        
    finally:
        shutil.rmtree(temp_dir)
        print(f"✓ Cleaned up batch processing directory")
    
    return True

def test_experimental_utilities():
    """Test experimental utilities"""
    print("\n=== Testing Experimental Utilities ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("✗ Failed to initialize brain for experimental test")
        return False
    
    # Test experimental simulation
    test_input = [0.3, 0.7, 0.2, 0.8, 0.4]
    
    result = pynlm.brainExperiment(brain, test_input, 25, "TestExperiment")
    print(f"✓ Experimental simulation completed with result: {result}")
    
    return True

def main():
    """Run all enhanced Python bindings tests"""
    print("=" * 60)
    print("NLM Python Bindings - Enhanced Features Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_advanced_checkpointing,
        test_memory_utilities,
        test_developmental_utilities,
        test_performance_monitoring,
        test_batch_operations,
        test_experimental_utilities
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
                print(f"✓ {test.__name__} PASSED")
            else:
                failed += 1
                print(f"✗ {test.__name__} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} FAILED with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed! Enhanced Python bindings are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())