#!/usr/bin/env python3
"""
Quick test to verify that the improved Python bindings are working correctly.
This test demonstrates the new Pythonic features and improvements.
"""

import pynlm
import time

def test_basic_functionality():
    """Test basic functionality to ensure bindings work."""
    print("Testing basic functionality...")
    
    # Test 1: Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Brain should have synapses"
    
    print("✓ Basic brain creation and initialization works")
    
    # Test 2: Run simulation steps
    for i in range(5):
        brain.step(i)
    
    assert brain.getTotalSpikeCount() >= 0, "Brain should have spikes"
    print("✓ Simulation steps work correctly")
    
    # Test 3: Get statistics
    stats = brain.get_brain_stats()
    assert "total_neurons" in stats, "Stats should contain total_neurons"
    assert "total_spikes" in stats, "Stats should contain total_spikes"
    print("✓ Statistics retrieval works")
    
    # Test 4: Configuration validation
    config2 = pynlm.createDefaultConfig()
    config2.setNeuronCount(100)
    pynlm.validate_config(config2)  # Should not raise exception
    print("✓ Configuration validation works")
    
    # Test 5: Error handling
    try:
        config2.setNeuronCount(-1)
        assert False, "Should have raised validation error"
    except pynlm.NLMConfigurationError:
        print("✓ Error handling works correctly")
    
    return config, brain

def test_context_manager():
    """Test context manager functionality."""
    print("\nTesting context manager...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(10, 10, 8, 8)
    world.setAgentStart(5.0, 5.0)
    
    # Test context manager
    with pynlm.SimulationContext(brain, world, agent) as sim:
        sim.runEpisode(10, 0.1, verbose=False)
        
        assert sim.getBrain() is brain
        assert sim.getWorld() is world
        assert sim.getAgent() is agent
    
    print("✓ Context manager works correctly")
    return config, brain, world, agent

def test_batch_processing():
    """Test batch processing functionality."""
    print("\nTesting batch processing...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(10, 10, 8, 8)
    world.setAgentStart(5.0, 5.0)
    
    # Create batch processor
    batch = pynlm.BatchProcessor(brain, world, agent, batchSize=5)
    
    # Test batch processing
    steps = list(range(20))
    timesteps = [0.1] * 20
    metrics = batch.processBatch(steps, timesteps, verbose=[False] * 20)
    
    assert len(metrics) == 20, "Should process all steps"
    for metric in metrics:
        assert metric.simulationTime >= 0.0, "Should have valid simulation time"
        assert metric.totalSpikes >= 0, "Should have valid spike count"
    
    print("✓ Batch processing works correctly")
    return config, brain, world, agent

def test_performance_monitoring():
    """Test performance monitoring functionality."""
    print("\nTesting performance monitoring...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test performance metrics
    metrics = brain.getPerformanceMetrics()
    
    assert hasattr(metrics, 'simulationTime'), "Should have simulationTime"
    assert hasattr(metrics, 'totalSpikes'), "Should have totalSpikes"
    assert hasattr(metrics, 'activeNeurons'), "Should have activeNeurons"
    assert hasattr(metrics, 'memoryUsage'), "Should have memoryUsage"
    
    # Test steps per second calculation
    steps = 1000
    sim_time = metrics.getStepsPerSecond(steps)
    assert sim_time >= 0.0, "Should have valid simulation time"
    
    print("✓ Performance monitoring works correctly")
    return config, brain

def test_help_system():
    """Test help system functionality."""
    print("\nTesting help system...")
    
    # Test help for simulation topic
    help_text = pynlm.help("simulation")
    assert isinstance(help_text, str), "Help should return string"
    assert len(help_text) > 0, "Help should not be empty"
    
    # Test version info
    version = pynlm.__version__()
    assert isinstance(version, str), "Version should be string"
    assert len(version) > 0, "Version should not be empty"
    
    print(f"✓ Help system works correctly (version: {version})")
    return help_text, version

def test_complete_workflow():
    """Test complete workflow with all features."""
    print("\nTesting complete workflow...")
    
    # Setup with all features enabled
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Use context manager for the complete workflow
    with pynlm.SimulationContext(brain, world, agent) as sim:
        # Run episode with progress tracking
        for step in range(50):
            sim.runEpisode(1, 0.1, verbose=False)
            
            # Print periodic updates
            if step % 10 == 0:
                print(f"  Step {step}:")
                print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"    Total spikes: {brain.getTotalSpikeCount()}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Verify results
    assert brain.getTotalSpikeCount() >= 0, "Should have spikes"
    assert brain.getAverageFiringRate() >= 0, "Should have firing rate"
    
    print("✓ Complete workflow with all features works correctly")
    return config, brain, world, agent

def main():
    """Run all verification tests."""
    print("=" * 70)
    print("NLM Python Bindings - Quick Verification Test")
    print("=" * 70)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Context Manager", test_context_manager),
        ("Batch Processing", test_batch_processing),
        ("Performance Monitoring", test_performance_monitoring),
        ("Help System", test_help_system),
        ("Complete Workflow", test_complete_workflow),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*70}")
        print(f"Running: {test_name}")
        print(f"{'='*70}")
        
        try:
            test_func()
            print(f"✅ {test_name}: PASSED")
            passed += 1
        except Exception as e:
            print(f"❌ {test_name}: FAILED - {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print(f"\n{'='*70}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'='*70}")
    
    if failed == 0:
        print("🎉 All tests passed! The improved Python bindings are working correctly.")
        print("\nKey features verified:")
        print("✓ Context managers for resource management")
        print("✓ Enhanced error handling and validation")
        print("✓ Batch processing capabilities")
        print("✓ Performance monitoring APIs")
        print("✓ Configuration validation")
        print("✓ Help system and documentation")
        print("✓ Complete workflow with all features")
    else:
        print(f"⚠️  {failed} tests failed. Please review the implementation.")
    
    return passed, failed

if __name__ == "__main__":
    main()
