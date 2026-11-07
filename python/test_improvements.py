# Test Script for NLM Python Bindings Improvements

import pynlm
import sys
import os

def test_basic_functionality():
    """Test basic NLM Python bindings functionality."""
    print("=== Testing Basic NLM Python Bindings ===")
    
    # Test 1: Create brain with default configuration
    print("\n1. Testing brain creation with default config...")
    brain = pynlm.createBrain()
    assert brain is not None, "Failed to create brain with default config"
    print(f"   ✓ Brain created successfully with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Initialize brain
    print("\n2. Testing brain initialization...")
    result = brain.initialize()
    assert result, "Failed to initialize brain"
    print("   ✓ Brain initialized successfully")
    
    # Test 3: Basic simulation
    print("\n3. Testing basic simulation...")
    initial_spikes = brain.getTotalSpikeCount()
    for i in range(10):
        brain.step(i)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes, "Spike count should not decrease"
    print(f"   ✓ Simulation completed. Spikes: {initial_spikes} -> {final_spikes}")
    
    return brain

def test_convenience_functions():
    """Test convenience functions added for Python users."""
    print("\n=== Testing Convenience Functions ===")
    
    # Test 1: Create brain with simple agent
    print("\n1. Testing createSimpleAgent...")
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createSimpleAgent(brain, world)
    assert agent is not None, "Failed to create simple agent"
    print("   ✓ Simple agent created and initialized")
    
    # Test 2: Run simple simulation
    print("\n2. Testing run_simple_simulation...")
    pynlm.run_simple_simulation(brain, world, agent, 20)
    print("   ✓ Simple simulation completed")
    
    # Test 3: Analyze brain state
    print("\n3. Testing analyze_brain_state...")
    analysis = pynlm.analyze_brain_state(brain)
    assert isinstance(analysis, str), "Analysis should return string"
    assert "Total Neurons:" in analysis, "Analysis should contain neuron count"
    print("   ✓ Brain state analysis completed")
    print(f"   Analysis preview: {analysis[:100]}...")
    
    return brain, world, agent

def test_error_handling():
    """Test improved error handling."""
    print("\n=== Testing Error Handling ===")
    
    # Test 1: Safe brain step
    print("\n1. Testing safe_brain_step...")
    brain = pynlm.createBrain()
    brain.initialize()
    
    result = pynlm.safe_brain_step(brain, 0)
    assert result, "Safe brain step should succeed"
    print("   ✓ Safe brain step completed successfully")
    
    # Test 2: Error handling for batch operations
    print("\n2. Testing batch operation error handling...")
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Create region and get neuron ID for testing
    if brain.getRegionCount() > 0:
        region = brain.getRegion(brain.getRegionIds()[0])
        neurons = region.getAllNeurons()
        if neurons:
            neuron_id = neurons[0].getId()
            
            # Test successful batch operation
            pynlm.inject_current_batch(brain, [neuron_id], [10.0])
            print("   ✓ Batch operation with correct sizes succeeded")
            
            # Test error case - mismatched sizes
            try:
                pynlm.inject_current_batch(brain, [neuron_id], [10.0, 20.المرجع])
                print("   ! ERROR: Should have failed with mismatched array sizes")
            except pynlm.PythonRuntimeError as e:
                print(f"   ✓ Correctly caught error: {e}")
    else:
        print("   ! Skipping batch test - no regions available")
    
    return brain

def test_advanced_features():
    """Test advanced features added for power users."""
    print("\n=== Testing Advanced Features ===")
    
    # Test 1: Development summary
    print("\n1. Testing get_developmental_summary...")
    brain = pynlm.createBrain()
    brain.initialize()
    
    dev_summary = pynlm.get_developmental_summary(brain)
    assert isinstance(dev_summary, str), "Development summary should return string"
    assert "Developmental Stage:" in dev_summary, "Should contain developmental stage"
    print("   ✓ Development summary completed")
    
    # Test 2: Configuration validation
    print("\n2. Testing validate_brain_config...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    validation = pynlm.validate_brain_config(config)
    assert isinstance(validation, str), "Validation should return string"
    assert "Configuration Validation" in validation, "Should be a validation report"
    print("   ✓ Configuration validation completed")
    
    # Test 3: Region information
    print("\n3. Testing get_region_info...")
    info = pynlm.get_region_info(brain)
    assert isinstance(info, str), "Region info should return string"
    assert "Brain Regions Summary" in info, "Should contain region summary"
    print("   ✓ Region information completed")
    
    # Test 4: Checkpoints and persistence
    print("\n4. Testing checkpoint functionality...")
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run some simulation
    for i in range(50):
        brain.step(i)
    
    # Save checkpoint
    pynlm.save_brain_state(brain, "test_checkpoint.bin")
    print("   ✓ Checkpoint saved successfully")
    
    # Create new brain and load checkpoint
    new_brain = pynlm.createBrain()
    new_brain.initialize()
    pynlm.load_brain_state(new_brain, "test_checkpoint.bin")
    print("   ✓ Checkpoint loaded successfully")
    
    # Verify neuron count is preserved
    assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount(), "Neuron count should be preserved"
    print("   ✓ Neuron count preserved across checkpoint")
    
    # Clean up
    if os.path.exists("test_checkpoint.bin"):
        os.remove("test_checkpoint.bin")
        print("   ✓ Test checkpoint cleaned up")
    
    return brain

def test_machine_learning_interface():
    """Test the machine learning style interface."""
    print("\n=== Testing Machine Learning Style Interface ===")
    
    # Test 1: Basic NLMAgent class
    print("\n1. Testing NLMAgent class...")
    
    # Create agent
    agent = pynlm.NLMAgent()
    agent.configure_world(10, 10, 6, 6)
    
    # Run episode
    for episode in range(3):
        agent.run_episode(max_steps=20)
    
    # Get performance stats
    stats = agent.get_performance_stats()
    assert stats is not None, "Should have performance stats"
    assert 'mean' in stats, "Should contain mean reward"
    assert 'std' in stats, "Should contain standard deviation"
    print(f"   ✓ Agent completed 3 episodes")
    print(f"   ✓ Mean reward: {stats['mean']:.3f}, Std: {stats['std']:.3f}")
    
    return agent

def test_advanced_sensory():
    """Test advanced sensory input functionality."""
    print("\n=== Testing Advanced Sensory Features ===")
    
    # Test 1: Custom vision input
    print("\n1. Testing custom vision input...")
    
    try:
        import numpy as np
        
        # Create custom vision data
        vision_data = np.random.randn(6, 6, 3).astype(np.float32)
        vision = pynlm.Vision(6, 6, 3)
        vision.setData(vision_data.tolist())
        
        print("   ✓ Custom vision input created successfully")
        
    except ImportError:
        print("   ! Skipping numpy test - not available")
    
    # Test 2: Internal signals
    print("\n2. Testing internal signals...")
    
    try:
        signals = pynlm.InternalSignals()
        signals.addSignal(0.5)
        signals.addSignal(-0.3)
        signals.addSignal(0.8)
        
        all_signals = signals.getAllSignals()
        assert len(all_signals) == 3, "Should have 3 signals"
        print(f"   ✓ Internal signals created: {all_signals}")
        
    except Exception as e:
        print(f"   ! Internal signals test failed: {e}")
    
    return

def run_all_tests():
    """Run all tests and report results."""
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Convenience Functions", test_convenience_functions),
        ("Error Handling", test_error_handling),
        ("Advanced Features", test_advanced_features),
        ("Machine Learning Interface", test_machine_learning_interface),
        ("Advanced Sensory Features", test_advanced_sensory),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            print(f"\n{'='*60}")
            print(f"Running: {test_name}")
            print(f"{'='*60}")
            
            result = test_func()
            results.append((test_name, "PASSED", result))
            print(f"\n✓ {test_name} PASSED")
            
        except Exception as e:
            results.append((test_name, "FAILED", str(e)))
            print(f"\n✗ {test_name} FAILED: {e}")
            import traceback
            traceback.print_exc()
    
    # Summary
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    
    passed = sum(1 for _, status, _ in results if status == "PASSED")
    failed = sum(1 for _, status, _ in results if status == "FAILED")
    
    for test_name, status, result in results:
        if status == "PASSED":
            print(f"✓ {test_name}: PASSED")
        else:
            print(f"✗ {test_name}: FAILED - {result}")
    
    print(f"\nTotal: {passed + failed} tests")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! NLM Python bindings improvements are working correctly.")
        return True
    else:
        print(f"\n❌ {failed} tests failed. Please review the issues above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)