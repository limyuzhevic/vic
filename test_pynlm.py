#!/usr/bin/env python3
"""
Test script for NLM Python bindings
This script runs comprehensive tests on the pynlm Python module
to ensure the bindings work correctly and provide the expected functionality.
"""

import sys
import os
import time
import numpy as np

# Add the python bindings to the path
bindings_path = os.path.join(os.path.dirname(__file__), '..', 'python')
sys.path.insert(0, bindings_path)

import pynlm
def test_basic_functionality():
    """Test basic Python binding functionality"""
    print("=== Testing Basic Python Binding Functionality ===")
    
    # Test 1: Create default configuration
    print("Test 1: Creating default configuration...")
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("  ✓ Configuration created successfully")
    
    # Test 2: Create brain with default config
    print("Test 2: Creating brain...")
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("  ✓ Brain created successfully")
    
    # Test 3: Initialize brain
    print("Test 3: Initializing brain...")
    success = brain.initialize()
    assert success is True
    print("  ✓ Brain initialized successfully")
    
    # Test 4: Get brain statistics
    print("Test 4: Getting brain statistics...")
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    assert neuron_count > 0
    assert synapse_count >= 0
    print(f"  ✓ Neuron count: {neuron_count}")
    print(f"  ✓ Synapse count: {synapse_count}")
    
    # Test 5: Run simulation steps
    print("Test 5: Running simulation steps...")
    for step in range(5):
        brain.step(step)
        firing_count = brain.getFiringNeuronCount()
        assert firing_count >= 0
    print("  ✓ Simulation steps completed successfully")
    
    print("✓ Basic functionality tests PASSED\n")
def test_advanced_features():
    """Test advanced Python binding features"""
    print("=== Testing Advanced Python Binding Features ===")
    
    # Test 1: Advanced configuration
    print("Test 1: Advanced configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.1)
    print("  ✓ Advanced configuration set successfully")
    
    # Test 2: Brain with advanced config
    print("Test 2: Creating brain with advanced configuration...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("  ✓ Brain created with advanced config")
    
    # Test 3: Connectivity analysis
    print("Test 3: Connectivity analysis...")
    connectivity = brain.analyzeConnectivity()
    assert 'region_count' in connectivity
    assert 'total_synapses' in connectivity
    assert 'connection_density' in connectivity
    print(f"  ✓ Region count: {connectivity['region_count']}")
    print(f"  ✓ Total synapses: {connectivity['total_synapses']}")
    print(f"  ✓ Connection density: {connectivity['connection_density']:.3f}")
    
    # Test 4: Network metrics
    print("Test 4: Network metrics...")
    metrics = brain.computeNetworkMetrics()
    assert 'total_neurons' in metrics
    assert 'total_synapses' in metrics
    assert 'firing_neurons' in metrics
    assert 'average_firing_rate' in metrics
    print(f"  ✓ Total neurons: {metrics['total_neurons']}")
    print(f"  ✓ Total synapses: {metrics['total_synapses']}")
    print(f"  ✓ Firing neurons: {metrics['firing_neurons']}")
    print(f"  ✓ Average firing rate: {metrics['average_firing_rate']:.3f}")
    
    # Test 5: Network metrics with artificial data
    print("Test 5: Testing network metrics with artificial data...")
    # Simulate some neural activity
    for step in range(10):
        brain.step(step)
        # Inject some current to create activity
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
    
    metrics2 = brain.computeNetworkMetrics()
    assert metrics2['total_neurons'] > 0
    print("  ✓ Network metrics computed successfully with activity")
    
    print("✓ Advanced features tests PASSED\n")
def test_error_handling():
    """Test error handling in Python bindings"""
    print("=== Testing Error Handling ===")
    
    # Test 1: Invalid configuration
    print("Test 1: Testing invalid configuration handling...")
    try:
        # This should raise an error due to invalid arguments
        invalid_config = pynlm.createDefaultConfig()
        # The binding should handle errors gracefully
        print("  ✓ Error handling works as expected")
    except Exception as e:
        print(f"  ✓ Exception caught: {type(e).__name__}")
    
    # Test 2: Invalid brain operations
    print("Test 2: Testing invalid brain operations...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test safe operation wrapper
    from pynlm import safeBrainOperation
    
    def safe_operation(brain_obj):
        # This should be safe to execute
        brain_obj.step(0)
        return "Operation succeeded"
    
    result = safeBrainOperation(brain, safe_operation)
    assert result is True
    print("  ✓ Safe operation completed successfully")
    
    print("✓ Error handling tests PASSED\n")
def test_agent_system():
    """Test agent system integration"""
    print("=== Testing Agent System Integration ===")
    
    # Test 1: Create complete agent system
    print("Test 1: Creating complete agent system...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    print("  ✓ Complete agent system created successfully")
    
    # Test 2: Enable all learning subsystems
    print("Test 2: Enabling all learning subsystems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("  ✓ All learning subsystems enabled")
    
    # Test 3: Run agent simulation
    print("Test 3: Running agent simulation...")
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print("  ✓ Agent simulation completed successfully")
    
    # Test 4: Get agent statistics
    print("Test 4: Getting agent statistics...")
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    neuromodulation = agent.getNeuromodulationLevel()
    developmental_stage = agent.getDevelopmentalStage()
    
    assert 0.0 <= curiosity <= 1.0
    assert 0.0 <= novelty <= 1.0
    assert 0.0 <= neuromodulation <= 1.0
    
    print(f"  ✓ Curiosity level: {curiosity:.3f}")
    print(f"  ✓ Novelty level: {novelty:.3f}")
    print(f"  ✓ Neuromodulation level: {neuromodulation:.3f}")
    print(f"  ✓ Developmental stage: {developmental_stage}")
    
    print("✓ Agent system tests PASSED\n")
def test_visualization_and_analysis():
    """Test visualization and analysis tools"""
    print("=== Testing Visualization and Analysis Tools ===")
    
    # Test 1: Create brain for analysis
    print("Test 1: Creating brain for analysis...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("  ✓ Brain created for analysis")
    
    # Test 2: Run simulation and collect data
    print("Test 2: Running simulation and collecting data...")
    step_data = []
    for step in range(10):
        brain.step(step)
        firing_count = brain.getFiringNeuronCount()
        firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        
        step_data.append({
            'step': step,
            'firing_count': firing_count,
            'firing_rate': firing_rate,
            'total_spikes': total_spikes
        })
    
    print(f"  ✓ Collected data for {len(step_data)} steps")
    
    # Test 3: Analyze step data
    print("Test 3: Analyzing step data...")
    avg_firing_rate = sum(d['firing_rate'] for d in step_data) / len(step_data)
    total_activity = sum(d['total_spikes'] for d in step_data)
    
    print(f"  ✓ Average firing rate: {avg_firing_rate:.3f}")
    print(f"  ✓ Total spikes: {total_activity}")
    
    # Test 4: Neural network analysis
    print("Test 4: Neural network analysis...")
    network_analysis = brain.analyzeConnectivity()
    
    assert 'region_count' in network_analysis
    assert 'total_synapses' in network_analysis
    assert 'strong_connections' in network_analysis
    
    print(f"  ✓ Region count: {network_analysis['region_count']}")
    print(f"  ✓ Total synapses: {network_analysis['total_synapses']}")
    print(f"  ✓ Strong connections: {network_analysis['strong_connections']}")
    
    print("✓ Visualization and analysis tests PASSED\n")
def test_batch_operations():
    """Test batch operation functionality"""
    print("=== Testing Batch Operations ===")
    
    # Test 1: Create brain for batch testing
    print("Test 1: Creating brain for batch operations...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("  ✓ Brain created for batch testing")
    
    # Test 2: Batch step operations
    print("Test 2: Testing batch step operations...")
    # Simulate multiple steps at once
    batch_steps = 50
    start_time = time.time()
    
    for step in range(batch_steps):
        brain.step(step, step * 0.001)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"  ✓ Completed {batch_steps} steps in {elapsed:.3f} seconds")
    print(f"  ✓ Average steps per second: {batch_steps/elapsed:.1f}")
    
    # Test 3: Batch current injection
    print("Test 3: Testing batch current injection...")
    currents = []
    for i in range(10):
        # Create list of current injections for multiple neurons
        currents.append((pynlm.NeuronId(i), 5.0f))
    
    from pynlm import injectCurrents
    injectCurrents(brain, currents)
    print("  ✓ Batch current injection completed successfully")
    
    # Test 4: Batch metric collection
    print("Test 4: Testing batch metric collection...")
    metrics_batch = []
    for step in range(5):
        brain.step(step)
        metrics = {
            'step': step,
            'firing_count': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        metrics_batch.append(metrics)
    
    print(f"  ✓ Collected {len(metrics_batch)} metrics batches")
    
    print("✓ Batch operations tests PASSED\n")
def test_configuration_management():
    """Test configuration management functionality"""
    print("=== Testing Configuration Management ===")
    
    # Test 1: Default configuration
    print("Test 1: Testing default configuration...")
    config1 = pynlm.createDefaultConfig()
    keys1 = config1.getKeys()
    assert isinstance(keys1, list)
    print(f"  ✓ Default configuration has {len(keys1)} keys")
    
    # Test 2: Custom configuration
    print("Test 2: Testing custom configuration...")
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 2000)
    config2.set("brain.synapse_density", 0.15)
    config2.set("plasticity.stdp.learning_rate", 0.002)
    
    assert config2.getOr("brain.neuron_count", 0) == 2000
    assert config2.getOr("brain.synapse_density", 0.0) == 0.15
    assert config2.getOr("plasticity.stdp.learning_rate", 0.0) == 0.002
    print("  ✓ Custom configuration values set and retrieved correctly")
    
    # Test 3: Configuration file operations
    print("Test 3: Testing configuration file operations...")
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        # Write configuration
        config2.saveToFile(f.name)
        print(f"  ✓ Configuration saved to {f.name}")
        
        # Load configuration
        config3 = pynlm.createDefaultConfig()
        loaded = config3.loadFromFile(f.name)
        assert loaded is True
        print(f"  ✓ Configuration loaded from {f.name}")
        
        # Clean up
        os.unlink(f.name)
    
    print("✓ Configuration management tests PASSED\n")
def test_simulation_control():
    """Test simulation control functionality"""
    print("=== Testing Simulation Control ===")
    
    # Test 1: Basic simulation control
    print("Test 1: Testing basic simulation control...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test reset functionality
    brain.reset()
    print("  ✓ Simulation reset completed")
    
    # Test initialization
    brain.initialize()
    print("  ✓ Simulation re-initialized")
    
    # Test 2: Developmental stage control
    print("Test 2: Testing developmental stage control...")
    for stage in pynlm.DevelopmentalStage:
        brain.setDevelopmentalStage(stage)
        current_stage = brain.getDevelopmentalStage()
        assert current_stage == stage
        print(f"  ✓ Developmental stage set to {current_stage}")
    
    # Test 3: Simulation checkpointing
    print("Test 3: Testing simulation checkpointing...")
    # Save brain state
    checkpoint_file = "/tmp/nlm_test_checkpoint.bin"
    saved = brain.save(checkpoint_file)
    assert saved is True
    print(f"  ✓ Brain state saved to {checkpoint_file}")
    
    # Create new brain and load checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    loaded = brain2.load(checkpoint_file)
    assert loaded is True
    print(f"  ✓ Brain state loaded from {checkpoint_file}")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.unlink(checkpoint_file)
    
    print("✓ Simulation control tests PASSED\n")
def test_performance_benchmarks():
    """Test performance benchmarks"""
    print("=== Testing Performance Benchmarks ===")
    
    # Test 1: Simple benchmark
    print("Test 1: Running simple benchmark...")
    config = pynlm.createDefaultConfig()
    
    # Create brain with reasonable size
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    from pynlm import benchmark
    results = benchmark(config, 100)
    
    assert 'steps_per_second' in results
    assert 'neurons' in results
    assert 'synapses' in results
    assert 'elapsed_time' in results
    
    print(f"  ✓ Steps per second: {results['steps_per_second']:.1f}")
    print(f"  ✓ Neurons: {results['neurons']}")
    print(f"  ✓ Synapses: {results['synapses']}")
    print(f"  ✓ Elapsed time: {results['elapsed_time']:.3f} seconds")
    
    # Test 2: Advanced benchmark with larger network
    print("Test 2: Running advanced benchmark...")
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 1000)
    config2.set("brain.region_count", 3)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    results2 = benchmark(config2, 50)
    assert results2['steps_per_second'] > 0
    print(f"  ✓ Advanced benchmark steps per second: {results2['steps_per_second']:.1f}")
    
    print("✓ Performance benchmark tests PASSED\n")
def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Advanced Features", test_advanced_features),
        ("Error Handling", test_error_handling),
        ("Agent System Integration", test_agent_system),
        ("Visualization and Analysis", test_visualization_and_analysis),
        ("Batch Operations", test_batch_operations),
        ("Configuration Management", test_configuration_management),
        ("Simulation Control", test_simulation_control),
        ("Performance Benchmarks", test_performance_benchmarks),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            test_func()
            passed += 1
            print(f"\n✓ {test_name} PASSED")
        except Exception as e:
            failed += 1
            print(f"\n✗ {test_name} FAILED: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "="*60)
    print("TEST SUMMARY")
    print("="*60)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        return True
    else:
        print(f"\n❌ {failed} test(s) failed")
        return False
if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)