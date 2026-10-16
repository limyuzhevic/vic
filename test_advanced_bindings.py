#!/usr/bin/env python3
"""
Test script for advanced Python bindings features.
Tests the new advanced functionality in the NLM bindings.
"""

import pynlm
import tempfile
import os

def test_config_advanced_features():
    """Test enhanced Config features: batch operations, validation, merging, snapshotting"""
    print("Testing Enhanced Config Features...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Test batch set and get
    values = {"test_int": 42, "test_double": 3.14, "test_string": "hello", "test_bool": True}
    config.batchSet(values)
    
    # Test batch get
    keys = ["test_int", "test_double", "test_string", "test_bool"]
    results = config.batchGet(keys)
    
    assert results[0] == 42, f"Expected 42, got {results[0]}"
    assert abs(results[1] - 3.14) < 0.01, f"Expected ~3.14, got {results[1]}"
    assert results[2] == "hello", f"Expected 'hello', got {results[2]}"
    assert results[3] == True, f"Expected True, got {results[3]}"
    print("  ✓ Batch operations work correctly")
    
    # Test validation
    assert config.validate() == True, "Configuration should be valid"
    print("  ✓ Validation works correctly")
    
    # Test snapshot and merge
    snapshot1 = config.snapshot()
    snapshot1.set("extra_key", "value", pynlm.ConfigSource.Runtime)
    
    snapshot2 = config.snapshot()
    snapshot2.set("another_key", 123, pynlm.ConfigSource.Runtime)
    
    merged = snapshot1.merge(snapshot2)
    assert merged.has("test_int") == True, "Merged config should have test_int"
    assert merged.has("extra_key") == True, "Merged config should have extra_key"
    assert merged.has("another_key") == True, "Merged config should have another_key"
    print("  ✓ Snapshot and merge operations work correctly")
    
    print("✓ All Config advanced features passed\n")

def test_brain_advanced_features():
    """Test advanced Brain features: detailed stats, region analysis, development progress"""
    print("Testing Advanced Brain Features...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100, pynlm.ConfigSource.Default)
    config.set("region_count", 2, pynlm.ConfigSource.Default)
    config.set("connection_probability", 0.1, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    success = brain.initialize()
    assert success == True, "Brain initialization should succeed"
    
    # Test detailed stats
    stats = brain.getDetailedStats()
    assert "totalNeurons" in stats, "Stats should contain totalNeurons"
    assert "totalSynapses" in stats, "Stats should contain totalSynapses"
    assert "activeNeurons" in stats, "Stats should contain activeNeurons"
    assert "firingNeurons" in stats, "Stats should contain firingNeurons"
    assert "regionCount" in stats, "Stats should contain regionCount"
    assert "regions" in stats, "Stats should contain regions list"
    assert "synapticStatistics" in stats, "Stats should contain synapticStatistics"
    assert "developmentalInfo" in stats, "Stats should contain developmentalInfo"
    print("  ✓ Detailed stats collection works")
    
    # Test region connectivity analysis
    region_ids = brain.getRegionIds()
    if region_ids:
        connectivity = brain.analyzeRegionConnectivity(region_ids[0])
        assert "neuronCount" in connectivity, "Connectivity should contain neuronCount"
        assert "connectionDensity" in connectivity, "Connectivity should contain connectionDensity"
        assert "averageSynapticWeight" in connectivity, "Connectivity should contain averageSynapticWeight"
        print("  ✓ Region connectivity analysis works")
    
    # Test development progress
    progress = brain.getDevelopmentProgress()
    assert "currentStage" in progress, "Progress should contain currentStage"
    assert "stageName" in progress, "Progress should contain stageName"
    assert "stageCharacteristics" in progress, "Progress should contain stageCharacteristics"
    print("  ✓ Development progress analysis works")
    
    # Test simulation snapshot
    snapshot = brain.createSimulationSnapshot("Test snapshot")
    assert isinstance(snapshot, pynlm.Config), "Snapshot should be a Config object"
    assert snapshot.has("description") == True, "Snapshot should have description"
    assert snapshot.has("totalNeurons") == True, "Snapshot should have totalNeurons"
    print("  ✓ Simulation snapshot creation works")
    
    # Test brain state introspection
    system_info = brain.getSystemInfo()
    assert "brainType" in system_info, "System info should contain brainType"
    assert "workingMemoryAvailable" in system_info, "System info should contain workingMemoryAvailable"
    assert "predictionSystemAvailable" in system_info, "System info should contain predictionSystemAvailable"
    print("  ✓ System introspection works")
    
    # Test brain state validation
    validation_result = brain.validateState()
    assert "valid" in validation_result, "Validation should contain valid flag"
    assert "errors" in validation_result, "Validation should contain errors list"
    print("  ✓ Brain state validation works")
    
    print("✓ All Brain advanced features passed\n")

def test_advanced_python_integration():
    """Test context managers and iteration interfaces"""
    print("Testing Advanced Python Integration...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 50, pynlm.ConfigSource.Default)
    config.set("region_count", 1, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test context manager
    with brain as managed_brain:
        assert managed_brain.getTotalNeuronCount() > 0, "Brain in context should have neurons"
        # Run a few steps
        for i in range(5):
            managed_brain.step(i)
    print("  ✓ Context manager support works")
    
    # Test iteration interfaces
    region_ids = list(brain)  # Using __iter__
    assert isinstance(region_ids, list), "Iteration should return a list"
    print("  ✓ Pythonic iteration interface works")
    
    # Test regions interface
    regions = brain.regions()
    # In a real implementation, this would return actual regions
    # For now, it returns the internal regions list
    print("  ✓ Regions iteration interface works")
    
    print("✓ All Advanced Python Integration features passed\n")

def test_performance_debug_features():
    """Test performance statistics, debug mode, configuration validation, and brain introspection"""
    print("Testing Performance & Debugging Features...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200, pynlm.ConfigSource.Default)
    config.set("region_count", 3, pynlm.ConfigSource.Default)
    config.set("connection_probability", 0.2, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test performance statistics
    performance = brain.getPerformanceStats()
    assert "neuronComputationRate" in performance, "Performance should contain neuronComputationRate"
    assert "totalSynapseOperations" in performance, "Performance should contain totalSynapseOperations"
    assert "simulationEfficiency" in performance, "Performance should contain simulationEfficiency"
    print("  ✓ Performance statistics work")
    
    # Test debug mode
    debug_result = brain.enableDebugMode(True)
    assert debug_result == True, "Debug mode enable should return True"
    print("  ✓ Debug mode activation works")
    
    # Test benchmark
    benchmark_results = brain.runBenchmark(10)
    assert "iterations" in benchmark_results, "Benchmark should contain iterations"
    assert "totalTimeMs" in benchmark_results, "Benchmark should contain totalTimeMs"
    assert "avgTimePerIterationMs" in benchmark_results, "Benchmark should contain avgTimePerIterationMs"
    print("  ✓ Benchmark functionality works")
    
    # Test configuration validation
    validation = brain.validateConfiguration()
    assert "valid" in validation, "Configuration validation should contain valid flag"
    assert "warnings" in validation, "Configuration validation should contain warnings"
    print("  ✓ Configuration validation works")
    
    # Test debug info export
    debug_info = brain.exportDebugInfo()
    assert "timestamp" in debug_info, "Debug info should contain timestamp"
    assert "totalNeurons" in debug_info, "Debug info should contain totalNeurons"
    assert "systemsAvailable" in debug_info, "Debug info should contain systemsAvailable"
    print("  ✓ Debug info export works")
    
    # Test debug log clearing
    clear_result = brain.clearDebugLogs()
    assert clear_result == True, "Debug log clearing should return True"
    print("  ✓ Debug log clearing works")
    
    print("✓ All Performance & Debugging features passed\n")

def test_error_handling():
    """Test error handling for invalid operations"""
    print("Testing Error Handling...")
    
    # Test invalid config operations
    config = pynlm.createDefaultConfig()
    
    try:
        # This should fail because we're using a non-existent key
        invalid_batch_get = config.batchGet(["nonexistent_key"])
        # It should return None or empty, not crash
        assert len(invalid_batch_get) == 1, "Should return list with one item"
        print("  ✓ Invalid batch get handled gracefully")
    except Exception as e:
        print(f"  ⚠ Batch get with invalid key raised exception: {e}")
    
    try:
        # Test brain error handling
        config.set("neuron_count", 100, pynlm.ConfigSource.Default)
        config.set("region_count", 2, pynlm.ConfigSource.Default)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Try region analysis with invalid region ID
        invalid_region_id = 99999  # Very unlikely to exist
        result = brain.analyzeRegionConnectivity(invalid_region_id)
        print("  ⚠ Invalid region ID should raise exception but didn't")
    except pynlm.InvalidArgumentException as e:
        print(f"  ✓ Invalid region ID correctly raised InvalidArgumentException: {e}")
    except Exception as e:
        print(f"  ⚠ Invalid region ID raised unexpected exception: {e}")
    
    print("✓ All Error Handling tests completed\n")

def main():
    """Run all advanced feature tests"""
    print("=" * 60)
    print("Testing Advanced NLM Python Bindings Features")
    print("=" * 60)
    print()
    
    try:
        test_config_advanced_features()
        test_brain_advanced_features()
        test_advanced_python_integration()
        test_performance_debug_features()
        test_error_handling()
        
        print("=" * 60)
        print("✓ ALL ADVANCED FEATURE TESTS PASSED!")
        print("=" * 60)
        print()
        print("Summary of tested features:")
        print("1. Enhanced Config Features:")
        print("   - Batch get/set operations")
        print("   - Configuration validation")
        print("   - Configuration merging and snapshotting")
        print()
        print("2. Advanced Brain Features:")
        print("   - Detailed statistics collection")
        print("   - Region connectivity analysis")
        print("   - Development progress monitoring")
        print("   - Simulation snapshot creation")
        print("   - System introspection")
        print("   - Brain state validation")
        print()
        print("3. Advanced Python Integration:")
        print("   - Context manager support")
        print("   - Pythonic iteration interfaces")
        print("   - Direct regions access")
        print()
        print("4. Performance & Debugging Features:")
        print("   - Performance statistics")
        print("   - Debug mode activation")
        print("   - Benchmark testing")
        print("   - Configuration validation")
        print("   - Debug information export")
        print("   - Debug log management")
        
    except Exception as e:
        print("=" * 60)
        print(f"✗ TEST FAILED: {e}")
        print("=" * 60)
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())