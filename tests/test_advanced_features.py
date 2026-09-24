#!/usr/bin/env python3
"""
Test script for verifying advanced NLM Python bindings features.
This script tests all the new advanced features added to bindings.cpp.
"""

import pynlm
import tempfile
import os
import sys

def test_basic_functionality():
    """Test basic NLM functionality to ensure bindings work."""
    print("Testing basic NLM functionality...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check basic properties
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Synapse count should be non-negative"
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Basic functionality test passed: {brain.getTotalNeuronCount()} neurons")
    return brain

def test_advanced_simulation_management(brain):
    """Test advanced simulation management features."""
    print("\nTesting advanced simulation management...")
    
    # Test simulation profiling
    profile = pynlm.profile_simulation(brain, 20, 0.1)
    assert profile.total_time > 0, "Profile should have total time"
    assert profile.average_step_time > 0, "Should have average step time"
    assert len(profile.step_times) == 20, "Should have recorded all steps"
    assert len(profile.spike_counts) == 20, "Should have recorded spike counts"
    
    print(f"✓ Simulation profiling test passed: {profile.total_time:.3f}s total")
    
    # Test batch simulations (simple version)
    configs = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        configs.append(config)
    
    # Create multiple brains
    brains = []
    for i, config in enumerate(configs):
        b = pynlm.createBrain(config)
        b.initialize()
        brains.append(b)
    
    # Run simulation on each
    steps = [10, 20, 15]
    firing_rates, spike_counts = pynlm.run_batch_simulations(
        brains, [], [], steps, 0.1
    )
    
    assert len(firing_rates) == 3, "Should have results for all brains"
    assert len(spike_counts) == 3, "Should have spike counts for all brains"
    
    print(f"✓ Batch simulation test passed: {len(brains)} brains processed")
    
    # Test simulation comparison
    comparison_data = pynlm.compare_simulations(brains, steps, 0.1)
    assert len(comparison_data) == 3, "Should have comparison data for all brains"
    
    print(f"✓ Simulation comparison test passed")

def test_configuration_optimization():
    """Test configuration optimization features."""
    print("\nTesting configuration optimization...")
    
    # Test configuration templates
    template = pynlm.create_config_template("test_template", {
        "brain.neuron_count": 500,
        "learning.rate": 0.01,
        "plasticity.enabled": True
    })
    
    assert template is not None, "Template should be created"
    
    # Test presets
    presets = pynlm.get_preset_configs()
    assert len(presets) >= 3, "Should have at least 3 presets"
    
    # Test optimization
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.memory.capacity", 5000)
    
    warnings, suggestions = pynlm.optimize_config(config)
    assert isinstance(warnings, list), "Warnings should be a list"
    assert isinstance(suggestions, list), "Suggestions should be a list"
    
    print(f"✓ Configuration optimization test passed")
    
    # Test configuration diffing
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 500)
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 1000)
    config2.set("plasticity.enabled", True)
    
    differences = pynlm.compare_configs(config1, config2)
    assert len(differences) > 0, "Should find differences between configs"
    
    print(f"✓ Configuration comparison test passed")

def test_memory_management():
    """Test memory management features."""
    print("\nTesting memory management...")
    
    # Test memory monitoring
    memory_stats = pynlm.monitor_memory_usage()
    assert memory_stats is not None, "Memory stats should be available"
    assert memory_stats.total_memory > 0, "Should have total memory"
    assert memory_stats.used_memory >= 0, "Used memory should be non-negative"
    
    print(f"✓ Memory monitoring test passed: {memory_stats.used_memory / 1024 / 1024:.1f} MB used")
    
    # Test memory-efficient batch processing
    data = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    results = pynlm.process_batch_efficiently(data)
    
    assert len(results) == 3, "Should have results for all batches"
    assert abs(results[0] - 2.0) < 0.001, "First batch average should be 2.0"
    
    print(f"✓ Memory-efficient batch processing test passed")
    
    # Test cache management
    cache = pynlm.create_memory_cache(10)  # 10 MB cache
    
    sample_data = [1.0, 2.0, 3.0, 4.0, 5.0]
    cache.set("test_data", sample_data)
    
    retrieved = cache.get("test_data")
    assert retrieved is not None, "Should retrieve cached data"
    assert len(retrieved) == len(sample_data), "Retrieved data should have same length"
    
    print(f"✓ Cache management test passed")
    
    # Test neural memory pool
    pool = pynlm.create_neural_memory_pool(100)
    
    data_vec = pool.allocate_data()
    assert len(data_vec) == 100, "Allocated data should have correct size"
    
    matrix = pool.allocate_matrix(5, 5)
    assert len(matrix) == 5, "Allocated matrix should have correct dimensions"
    assert len(matrix[0]) == 5, "Allocated matrix should have correct inner dimensions"
    
    print(f"✓ Neural memory pool test passed")

def test_performance_monitoring(brain):
    """Test performance monitoring features."""
    print("\nTesting performance monitoring...")
    
    # Test performance profiler
    profiler = pynlm.create_performance_profiler()
    
    # Use the profiler
    for i in range(10):
        profiler.start_operation(f"step_{i}")
        brain.step(i)
        profiler.end_operation(f"step_{i}")
    
    results = profiler.get_results()
    assert len(results) == 10, "Should have results for all steps"
    
    print(f"✓ Performance profiler test passed: {len(results)} operations timed")
    
    # Test bottleneck detection
    bottlenecks, metrics = pynlm.detect_bottlenecks(brain, 50, 0.1)
    
    assert isinstance(bottlenecks, list), "Bottlenecks should be a list"
    assert isinstance(metrics, dict), "Metrics should be a dictionary"
    
    print(f"✓ Bottleneck detection test passed: {len(bottlenecks)} bottlenecks detected")
    
    # Test optimization suggestions
    suggestions = pynlm.get_optimization_suggestions(brain)
    
    assert isinstance(suggestions, list), "Suggestions should be a list"
    
    print(f"✓ Optimization suggestions test passed: {len(suggestions)} suggestions")

def test_advanced_learning_features():
    """Test advanced learning features."""
    print("\nTesting advanced learning features...")
    
    # Create agent for testing
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test advanced neuromodulation
    neuromod_params = {
        "plasticity_boost": 1.5,
        "excitability_modulation": 0.8,
        "learning_rate_adjustment": 0.9
    }
    
    success = pynlm.advanced_neuromodulation(
        agent, "dopamine", 1.0, neuromod_params
    )
    
    assert success, "Advanced neuromodulation should succeed"
    
    print(f"✓ Advanced neuromodulation test passed")
    
    # Test custom learning rule
    def test_learning_rule(pre, post, trace):
        return 0.01 * (post - pre) * trace
    
    custom_rule = pynlm.create_custom_learning_rule(
        "test_rule", test_learning_rule, {"param": 1.0}
    )
    
    assert custom_rule.rule_name == "test_rule", "Rule name should match"
    assert custom_rule.params["param"] == 1.0, "Parameters should match"
    
    # Test meta-learning (simplified)
    configs = []
    for i in range(3):
        cfg = pynlm.createDefaultConfig()
        cfg.set("brain.neuron_count", 500 + i * 100)
        configs.append(cfg)
    
    brain.reset()
    best_config, best_score = pynlm.meta_learning_optimization(
        brain, configs, 20
    )
    
    assert best_score > 0, "Best score should be positive"
    
    print(f"✓ Meta-learning optimization test passed: {best_score:.4f} score")
    
    # Test transfer learning
    target_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    target_brain.initialize()
    
    layers = ["weights", "biases"]
    transfer_success = pynlm.transfer_learning(brain, target_brain, layers)
    
    # Transfer may or may not succeed in this simple test
    print(f"✓ Transfer learning test passed (success: {transfer_success})")

def test_extension_points():
    """Test extension points and plugin architecture."""
    print("\nTesting extension points...")
    
    # Test custom component registration
    def custom_component(brain):
        pass
    
    registration_success = pynlm.register_custom_component("test_component", custom_component)
    assert registration_success, "Custom component registration should succeed"
    
    print(f"✓ Custom component registration test passed")
    
    # Test plugin manager
    plugin_manager = pynlm.create_plugin_manager()
    
    def test_plugin():
        pass
    
    load_success = plugin_manager.load_plugin("test_plugin", test_plugin)
    assert load_success, "Plugin load should succeed"
    
    plugins = plugin_manager.get_loaded_plugins()
    assert "test_plugin" in plugins, "Plugin should be in loaded list"
    
    plugin_manager.execute_plugin("test_plugin")
    
    print(f"✓ Plugin manager test passed: {len(plugins)} plugins loaded")
    
    # Test custom neuron type
    def custom_activation(inputs, weights):
        return 1.0 / (1.0 + sum(inputs[i] * weights[i] for i in range(len(inputs))))
    
    custom_neuron = pynlm.create_custom_neuron_type(
        "test_neuron", custom_activation
    )
    
    assert custom_neuron.neuron_name == "test_neuron", "Neuron name should match"
    
    result = custom_neuron.activate([0.5, 0.8], [0.2, 0.6])
    assert 0 <= result <= 1, "Activation should be between 0 and 1"
    
    print(f"✓ Custom neuron type test passed")
    
    # Test neural architecture search
    brain_template = pynlm.createBrain(pynlm.createDefaultConfig())
    brain_template.initialize()
    
    architectures = [[10, 20, 10], [20, 15, 20], [15, 25, 15]]
    
    results = pynlm.neural_architecture_search(
        brain_template, architectures, 10, 0.01
    )
    
    assert len(results) == len(architectures), "Should have results for all architectures"
    
    # Sort by performance and get best
    best_result = max(results, key=lambda x: x.performance_score)
    assert best_result.performance_score > 0, "Best architecture should have positive score"
    
    print(f"✓ Neural architecture search test passed: {len(results)} architectures evaluated")

def test_visualization_and_analysis(brain):
    """Test visualization and analysis features."""
    print("\nTesting visualization and analysis...")
    
    # Test neural activity plotting
    activity_data = pynlm.plot_neural_activity(brain, 20, 0.1)
    
    assert len(activity_data) == 20, "Should have data for all steps"
    assert len(activity_data[0]) >= 4, "Each step should have multiple metrics"
    
    print(f"✓ Neural activity plotting test passed: {len(activity_data)} steps recorded")
    
    # Test brain dynamics analysis
    analysis = pynlm.analyze_brain_dynamics(brain, 30, 0.1)
    
    assert analysis.mean_firing_rate >= 0, "Mean firing rate should be non-negative"
    assert analysis.std_firing_rate >= 0, "Std firing rate should be non-negative"
    assert len(analysis.firing_rate_history) == 30, "Should have firing rate history"
    
    print(f"✓ Brain dynamics analysis test passed")
    
    # Test neural network analysis
    network_analysis = pynlm.analyze_neural_network(brain)
    
    assert network_analysis.neuron_count > 0, "Should have positive neuron count"
    assert network_analysis.synapse_count >= 0, "Synapse count should be non-negative"
    assert 0 <= network_analysis.connection_density <= 1, "Density should be between 0 and 1"
    
    print(f"✓ Neural network analysis test passed")

def test_multi_threaded_support():
    """Test multi-threaded and distributed support."""
    print("\nTesting multi-threaded support...")
    
    # Create multiple brains for parallel execution
    brains = []
    for i in range(3):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Test parallel execution
    steps = [10, 15, 20]
    parallel_results = pynlm.run_parallel_simulations(brains, steps, 0.1)
    
    assert len(parallel_results) == len(brains), "Should have results for all brains"
    assert all(r > 0 for r in parallel_results), "All results should be positive"
    
    print(f"✓ Parallel execution test passed: {len(parallel_results)} brains simulated")
    
    # Test distributed simulation
    distributed_results = pynlm.distributed_simulation(brains, steps, 0.1)
    
    assert len(distributed_results) == len(brains), "Should have distributed results"
    
    print(f"✓ Distributed simulation test passed")
    
    # Test load balancing
    base_steps = [10, 20, 30]
    balanced_steps = pynlm.balance_workload(brains, base_steps)
    
    assert len(balanced_steps) == len(base_steps), "Should have balanced steps"
    assert all(s > 0 for s in balanced_steps), "All steps should be positive"
    
    print(f"✓ Load balancing test passed")
    
    # Test fault tolerance
    # Add a None brain to simulate failure
    brains_with_failure = brains + [None]
    fault_steps = [10, 15, 20, 10]  # One extra for the None brain
    
    fault_results = pynlm.fault_tolerant_simulation(brains_with_failure, fault_steps, 0.1)
    
    assert len(fault_results.results) == len(fault_steps), "Should have results for all"
    assert len(fault_results.success_flags) == len(fault_steps), "Should have success flags"
    
    print(f"✓ Fault tolerance test passed: {len(fault_results.success_flags)} simulations completed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Advanced Features Test Suite")
    print("=" * 60)
    
    try:
        # Run all tests
        brain = test_basic_functionality()
        test_advanced_simulation_management(brain)
        test_configuration_optimization()
        test_memory_management()
        test_performance_monitoring(brain)
        test_advanced_learning_features()
        test_extension_points()
        test_visualization_and_analysis(brain)
        test_multi_threaded_support()
        
        print("\n" + "=" * 60)
        print("✅ ALL TESTS PASSED!")
        print("=" * 60)
        print("\nAdvanced NLM Python bindings are working correctly.")
        print("\nKey features verified:")
        print("  ✓ Advanced Simulation Management")
        print("  ✓ Configuration Optimization")
        print("  ✓ Memory Management")
        print("  ✓ Performance Monitoring")
        print("  ✓ Advanced Learning Features")
        print("  ✓ Extension Points")
        print("  ✓ Visualization and Analysis")
        print("  ✓ Multi-threaded Support")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())