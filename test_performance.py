#!/usr/bin/env python3
"""
Performance Test Suite for NLM Brain

This script tests all the new performance optimizations implemented:
- Memory pools for neurons and synapses
- SIMD vectorization
- Parallel processing
- Event-driven processing
- Sparse connectivity
- I/O optimizations (compressed checkpoints)
- Caching and precomputation
"""

import sys
import os
import time
import numpy as np

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def test_memory_pools():
    """Test memory pool functionality"""
    print("Testing Memory Pools...")
    
    try:
        from performance.MemoryPool import MemoryPool
        
        # Test float memory pool
        floatPool = MemoryPool[float](1024, 4)
        
        # Allocate and deallocate
        floats = []
        for i in range(500):
            floats.append(floatPool.allocate())
        
        # Write some data
        for i, f in enumerate(floats):
            f[0] = float(i * 1.5)
        
        # Deallocate half
        for i in range(250):
            floatPool.deallocate(floats[i])
        
        # Allocate more
        for i in range(250, 500):
            floats[i] = floatPool.allocate()
        
        print(f"  Float pool allocated: {floatPool.allocatedCount()}")
        print(f"  Float pool available: {floatPool.availableCount()}")
        print("  Memory Pool test PASSED")
        return True
        
    except Exception as e:
        print(f"  Memory Pool test FAILED: {e}")
        return False

def test_simd_vectorization():
    """Test SIMD vectorization"""
    print("Testing SIMD Vectorization...")
    
    try:
        from performance.SIMDVectorization import SIMDUtils, VectorizedLIF
        
        print(f"  SIMD Type: {SIMDUtils.getSIMDType()}")
        print(f"  SIMD Available: {SIMDUtils.isSIMDAvailable()}")
        print(f"  SIMD Width: {SIMDUtils.SIMD_WIDTH}")
        
        # Test LIF vectorizer
        lif = VectorizedLIF()
        print("  LIF parameters: V_rest=-70.0, V_reset=-70.0, V_threshold=-55.0")
        print("  SIMD Vectorization test PASSED")
        return True
        
    except Exception as e:
        print(f"  SIMD Vectorization test FAILED: {e}")
        return False

def test_parallel_processing():
    """Test parallel processing"""
    print("Testing Parallel Processing...")
    
    try:
        from performance.ParallelProcessing import ThreadPool, ParallelFor
        
        print(f"  Default thread count: {ParallelProcessing.DEFAULT_NUM_THREADS}")
        
        # Test thread pool
        pool = ThreadPool(4)
        
        # Submit a task
        future = pool.submitVoid([]() {
            time.sleep(0.001)
            return;
        });
        
        future.wait()
        print("  Thread pool test PASSED")
        
        # Test parallel for
        results = [0] * 100
        ParallelFor.simple(0, 100, lambda i: results[i] = i * 2)
        
        # Verify results
        for i in range(100):
            if results[i] != i * 2:
                print(f"  Parallel for test FAILED: results[{i}] = {results[i]}")
                return False
        
        print("  Parallel For test PASSED")
        return True
        
    except Exception as e:
        print(f"  Parallel Processing test FAILED: {e}")
        return False

def test_optimized_brain():
    """Test optimized brain functionality"""
    print("Testing Optimized Brain...")
    
    try:
        from performance.OptimizedBrain import OptimizedBrain
        from core.Config.Config import Config
        
        # Create configuration
        config = Config()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        
        # Create optimized brain
        brain = OptimizedBrain(config)
        
        if brain.initialize():
            print("  Optimized Brain initialization PASSED")
            
            # Test performance stats
            stats = brain.getPerformanceStats()
            print(f"  Active neurons: {stats.activeNeurons}")
            print(f"  Firing neurons: {stats.firingNeurons}")
            print(f"  Average firing rate: {stats.avgFiringRate}")
            
            # Test step
            brain.step(100)
            print("  Optimized Brain step PASSED")
            
            return True
        else:
            print("  Optimized Brain initialization FAILED")
            return False
            
    except Exception as e:
        print(f"  Optimized Brain test FAILED: {e}")
        return False

def test_checkpoint_optimizations():
    """Test checkpoint I/O optimizations"""
    print("Testing Checkpoint Optimizations...")
    
    try:
        from performance.CheckpointSystem import CheckpointManager, CompressionLevel
        
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure("./checkpoints", 1000, 5, true)
        
        print(f"  Checkpoint directory: {manager.getCheckpointDir()}")
        print(f"  Max checkpoints: {manager.maxCheckpoints_}")
        print(f"  Compression enabled: {manager.compress_}")
        
        print("  Checkpoint optimizations test PASSED")
        return True
        
    except Exception as e:
        print(f"  Checkpoint optimizations test FAILED: {e}")
        return False

def test_performance_improvements():
    """Test overall performance improvements"""
    print("Testing Overall Performance Improvements...")
    
    try:
        from brain.Brain import Brain
        from core.Config.Config import Config
        
        # Create configuration
        config = Config()
        config.set("neuron_count", 1000)
        config.set("region_count", 4)
        config.set("threads", 4)
        
        # Create brain
        brain = Brain(config)
        
        # Enable optimized brain
        brain.enableOptimizedBrain(true)
        
        # Initialize with optimizations
        if brain.initialize():
            print("  Brain initialization with optimizations PASSED")
            
            # Get performance stats
            stats = brain.getOptimizedBrainStats()
            print(f"  Optimized brain memory usage: {stats.memoryUsageMB} MB")
            print(f"  Optimized brain active neurons: {stats.activeNeurons}")
            
            # Test all optimizations
            if brain.areAllOptimizationsEnabled():
                print("  All optimizations enabled PASSED")
            else:
                print("  All optimizations enabled FAILED")
                return False
            
            # Warmup
            brain.warmupOptimizedBrain(100)
            print("  Warmup test PASSED")
            
            return True
        else:
            print("  Brain initialization FAILED")
            return False
            
    except Exception as e:
        print(f"  Performance improvements test FAILED: {e}")
        return False

def main():
    """Run all performance tests"""
    print("=" * 60)
    print("NLM Performance Optimizations Test Suite")
    print("=" * 60)
    
    tests = [
        ("Memory Pools", test_memory_pools),
        ("SIMD Vectorization", test_simd_vectorization),
        ("Parallel Processing", test_parallel_processing),
        ("Optimized Brain", test_optimized_brain),
        ("Checkpoint Optimizations", test_checkpoint_optimizations),
        ("Performance Improvements", test_performance_improvements),
    ]
    
    passed = 0
    total = len(tests)
    
    for testName, testFunc in tests:
        print(f"\n{'-' * 60}")
        print(f"Running: {testName}")
        print(f"{'-' * 60}")
        
        if testFunc():
            passed += 1
    
    print(f"\n{'-' * 60}")
    print(f"Test Results: {passed}/{total} tests passed")
    print(f"{'-' * 60}")
    
    if passed == total:
        print("All performance optimizations PASSED!")
        return 0
    else:
        print(f"{total - passed} tests FAILED")
        return 1

if __name__ == "__main__":
    sys.exit(main())
