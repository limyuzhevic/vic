#!/usr/bin/env python3
"""
Quick test script to verify the improved Python bindings for NLM.
This script tests the new convenience methods and high-level API.
"""

import sys
import pynlm

def test_basic_bindings():
    """Test basic binding functionality."""
    print("Testing basic bindings...")
    
    # Test 1: Create brain with default configuration
    print("  1. Creating brain with default config...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain is not None
    print("     ✓ Brain created")
    
    # Test 2: Initialize brain
    print("  2. Initializing brain...")
    success = brain.initialize()
    assert success
    print("     ✓ Brain initialized")
    
    # Test 3: Run simulation
    print("  3. Running simulation...")
    spike_count = brain.runSteps(10)
    print(f"     ✓ Ran 10 steps, got {spike_count} spikes")
    
    # Test 4: Check brain stats
    print("  4. Checking brain stats...")
    neurons = brain.getTotalNeuronCount()
    regions = brain.getRegionCount()
    active = brain.getActiveNeuronCount()
    firing = brain.getFiringNeuronCount()
    print(f"     ✓ Neurons: {neurons}, Regions: {regions}, Active: {active}, Firing: {firing}")
    
    return True

def test_high_level_api():
    """Test high-level convenience API."""
    print("\nTesting high-level API...")
    
    # Test 1: Create simple world
    print("  1. Creating simple world...")
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("     ✓ World created")
    
    # Test 2: Create brain and agent
    print("  2. Creating brain and agent...")
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    print("     ✓ Agent created")
    
    # Test 3: Run agent simulation
    print("  3. Running agent simulation...")
    spikes = pynlm.runAgentSimulation(brain, world, agent, 5)
    print(f"     ✓ Agent simulation ran, got {spikes} spikes")
    
    return True

def test_configuration_api():
    """Test configuration dictionary API."""
    print("\nTesting configuration API...")
    
    # Test 1: Create config from dict
    print("  1. Creating config from dictionary...")
    config_dict = {
        "neuron_count": 200,
        "region_count": 3,
        "connection_probability": 0.15,
        "spike_threshold": -53.0
    }
    
    config = pynlm.createConfigFromDict(config_dict)
    assert config is not None
    print("     ✓ Config created from dict")
    
    # Test 2: Create brain from config dict
    print("  2. Creating brain from config dict...")
    brain = pynlm.createBrainFromDict(config_dict)
    assert brain is not None
    brain.initialize()
    print("     ✓ Brain created from config dict")
    
    return True

def test_experiment_class():
    """Test Experiment class functionality."""
    print("\nTesting Experiment class...")
    
    # Test 1: Create experiment
    print("  1. Creating experiment...")
    experiment = pynlm.Experiment()
    assert experiment is not None
    print("     ✓ Experiment created")
    
    # Test 2: Add components
    print("  2. Adding components to experiment...")
    brain = pynlm.createBrain()
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    experiment.addBrain(brain)
    experiment.addWorld(world)
    experiment.addAgent(agent)
    print("     ✓ Components added")
    
    # Test 3: Run episode
    print("  3. Running experiment episode...")
    experiment.runEpisode(10)
    print("     ✓ Episode completed")
    
    # Test 4: Collect results
    print("  4. Collecting results...")
    results = experiment.collectResults()
    assert results is not None
    print("     ✓ Results collected")
    
    return True

def test_metrics_collection():
    """Test metrics collection functionality."""
    print("\nTesting metrics collection...")
    
    # Test 1: Create metric collector
    print("  1. Creating metric collector...")
    metrics = pynlm.MetricCollector()
    assert metrics is not None
    print("     ✓ Metric collector created")
    
    # Test 2: Record metrics
    print("  2. Recording metrics...")
    for i in range(5):
        metrics.recordSpikeCount(100 + i * 10)
        metrics.recordFiringRate(5.0 + i * 0.5)
        metrics.recordMemoryUsage(50 + i * 5)
    
    print("     ✓ Metrics recorded")
    
    # Test 3: Get metrics
    print("  3. Getting metrics...")
    avg_spikes = metrics.getAverageSpikeCount()
    avg_rate = metrics.getAverageFiringRate()
    print(f"     ✓ Average spikes: {avg_spikes}, Average rate: {avg_rate}")
    
    # Test 4: Get summary
    print("  4. Getting summary...")
    summary = metrics.getSummary()
    print(f"     ✓ Summary: {summary}")
    
    return True

def test_standard_configurations():
    """Test standard configuration factory methods."""
    print("\nTesting standard configurations...")
    
    # Test 1: Research configuration
    print("  1. Testing research configuration...")
    research_config = pynlm.createResearchConfiguration()
    assert research_config is not None
    print("     ✓ Research config created")
    
    # Test 2: Development configuration
    print("  2. Testing development configuration...")
    dev_config = pynlm.createDevelopmentConfiguration()
    assert dev_config is not None
    print("     ✓ Development config created")
    
    # Test 3: Standard experiment config
    print("  3. Testing standard experiment config...")
    exp_config = pynlm.createStandardExperimentConfig()
    assert exp_config is not None
    print("     ✓ Standard experiment config created")
    
    return True

def test_error_handling():
    """Test error handling functionality."""
    print("\nTesting error handling...")
    
    # Test 1: Test null brain validation
    print("  1. Testing null validation...")
    try:
        pynlm.validateBrain(None)
        print("     ✗ Should have raised exception for null brain")
        return False
    except Exception as e:
        print(f"     ✓ Correctly raised exception: {type(e).__name__}")
    
    # Test 2: Test configuration error
    print("  2. Testing configuration error...")
    try:
        # This should work fine
        brain = pynlm.createBrain()
        print("     ✓ Normal configuration works")
    except Exception as e:
        print(f"     ✗ Unexpected error: {e}")
        return False
    
    return True

def test_build_info():
    """Test build information and documentation."""
    print("\nTesting build information...")
    
    # Test 1: Get build info
    print("  1. Getting build info...")
    info = pynlm.getBuildInfo()
    print(f"     ✓ Build info: {info}")
    
    # Test 2: Get quick start examples
    print("  2. Getting quick start examples...")
    examples = pynlm.getQuickStartExamples()
    print(f"     ✓ Got {len(examples)} examples")
    
    # Test 3: Check module attributes
    print("  3. Checking module attributes...")
    print(f"     ✓ Version: {pynlm.__version__}")
    print(f"     ✓ Author: {pynlm.__author__}")
    print(f"     ✓ Description: {pynlm.__description__}")
    
    return True

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings - Quick Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Bindings", test_basic_bindings),
        ("High-Level API", test_high_level_api),
        ("Configuration API", test_configuration_api),
        ("Experiment Class", test_experiment_class),
        ("Metrics Collection", test_metrics_collection),
        ("Standard Configurations", test_standard_configurations),
        ("Error Handling", test_error_handling),
        ("Build Information", test_build_info),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"Running: {test_name}")
        print(f"{'=' * 60}")
        
        try:
            if test_func():
                print(f"\n✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"\n✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"\n✗ {test_name} FAILED with exception: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print(f"\n{'=' * 60}")
    print(f"Test Summary: {passed} passed, {failed} failed")
    print(f"{'=' * 60}")
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved Python bindings are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
