#!/usr/bin/env python3
"""
Python test suite for NLM enhanced Python bindings
Tests new convenience functions and enhanced features
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_bindings():
    """Test basic Python bindings work correctly."""
    print("Testing basic Python bindings...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic methods
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    assert brain.getTotalSpikeCount() >= 0
    
    print("  PASSED: Basic bindings work correctly")

def test_agent_bindings():
    """Test enhanced AgentBrain bindings."""
    print("Testing AgentBrain bindings...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test enable/disable methods
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    print("  PASSED: AgentBrain enable/disable methods work")

def test_enhanced_config():
    """Test new configuration helper functions."""
    print("Testing enhanced configuration...")
    
    # Test custom config creation
    config = pynlm.create_default_config_with_settings(
        neuron_count=500,
        connection_probability=0.08,
        enable_stdp=True,
        enable_hebbian=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() == 500
    print("  PASSED: Custom config creation works")

def test_convenience_functions():
    """Test convenience functions."""
    print("Testing convenience functions...")
    
    # Test create_agent_simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    result = pynlm.create_agent_simulation(brain, world, 50)
    assert len(result) == 3
    retrieved_brain, retrieved_agent, retrieved_world = result
    assert retrieved_brain.getTotalSpikeCount() >= 0
    
    print("  PASSED: create_agent_simulation works")

def test_validation_function():
    """Test configuration validation."""
    print("Testing configuration validation...")
    
    config = pynlm.createDefaultConfig()
    errors = pynlm.validate_simulation_config(config)
    assert len(errors) == 0, f"Expected no errors, got: {errors}"
    
    print("  PASSED: Configuration validation works")

def test_analysis_function():
    """Test brain analysis function."""
    print("Testing brain analysis...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for i in range(50):
        brain.step(i)
    
    stats = pynlm.analyze_brain_dynamics(brain, 50)
    assert "mean_firing_rate" in stats
    assert "std_firing_rate" in stats
    assert "total_spikes" in stats
    assert stats["mean_firing_rate"] >= 0.0
    assert stats["std_firing_rate"] >= 0.0
    
    print("  PASSED: Brain analysis works")

def test_learning_episode():
    """Test enhanced learning episode function."""
    print("Testing enhanced learning episode...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    result = pynlm.run_learning_episode(
        brain, world, 
        steps=30, 
        enable_plasticity=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    assert len(result) == 3
    retrieved_brain, retrieved_agent, retrieved_world = result
    assert retrieved_brain.getTotalSpikeCount() >= 0
    
    print("  PASSED: Learning episode function works")

def test_complex_agent():
    """Test complex agent creation."""
    print("Testing complex agent creation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    settings = {
        "enable_reward_modulation": True,
        "enable_structural_plasticity": False,
        "enable_development": True,
        "enable_curiosity": True
    }
    
    agent = pynlm.create_complex_agent(brain, world, settings)
    assert agent is not None
    assert agent.isRewardModulationEnabled()
    assert not agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    print("  PASSED: Complex agent creation works")

def test_optimized_simulation():
    """Test optimized simulation function."""
    print("Testing optimized simulation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    result = pynlm.run_optimized_simulation(brain, world, 50, 1)
    assert len(result) == 3
    retrieved_brain, retrieved_agent, retrieved_world = result
    assert retrieved_brain.getTotalSpikeCount() >= 0
    
    print("  PASSED: Optimized simulation works")

def test_benchmark_function():
    """Test benchmarking function."""
    print("Testing benchmark function...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    results = pynlm.benchmark_brain_performance(brain, world, 100)
    
    assert "total_time_ms" in results
    assert "steps_per_second" in results
    assert "final_spike_count" in results
    assert "final_firing_rate" in results
    assert results["total_time_ms"] >= 0
    assert results["steps_per_second"] >= 0
    assert results["final_spike_count"] >= 0
    assert results["final_firing_rate"] >= 0.0
    
    print(f"  PASSED: Benchmark function works (performance: {results['steps_per_second']:.1f} steps/sec)")

def test_comprehensive_workflow():
    """Test a comprehensive workflow using multiple new features."""
    print("Testing comprehensive workflow...")
    
    # Create custom configuration
    config = pynlm.create_default_config_with_settings(
        neuron_count=300,
        connection_probability=0.12,
        enable_stdp=True,
        enable_hebbian=True
    )
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run learning episode
    result = pynlm.run_learning_episode(
        brain, world, steps=50,
        enable_plasticity=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    retrieved_brain, retrieved_agent, retrieved_world = result
    
    # Validate results
    assert retrieved_brain.getTotalNeuronCount() == 300
    assert retrieved_brain.getTotalSpikeCount() >= 0
    assert retrieved_agent.getCuriosityLevel() >= 0.0
    assert retrieved_agent.getNoveltyLevel() >= 0.0
    assert retrieved_agent.getPredictionError() >= 0.0
    
    # Analyze performance
    stats = pynlm.analyze_brain_dynamics(retrieved_brain, 50)
    assert stats["mean_firing_rate"] >= 0.0
    
    # Validate configuration
    errors = pynlm.validate_simulation_config(config)
    assert len(errors) == 0
    
    print("  PASSED: Comprehensive workflow works")

def main():
    """Run all tests."""
    print("=== NLM Enhanced Python Bindings Test Suite ===")
    print()
    
    tests = [
        test_basic_bindings,
        test_agent_bindings,
        test_enhanced_config,
        test_convenience_functions,
        test_validation_function,
        test_analysis_function,
        test_learning_episode,
        test_complex_agent,
        test_optimized_simulation,
        test_benchmark_function,
        test_comprehensive_workflow,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  FAILED: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
        print()
    
    print("=== Test Summary ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {len(tests)}")
    
    if failed == 0:
        print("\n=== All Tests Passed ===")
        return 0
    else:
        print(f"\n=== {failed} Tests Failed ===")
        return 1

if __name__ == "__main__":
    sys.exit(main())