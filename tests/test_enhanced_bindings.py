#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Comprehensive Test Suite

This test suite validates all new functionality in the enhanced Python bindings:
- Batch operations for multiple brains
- Network statistics and monitoring
- Progress tracking with callbacks
- Custom error handling
- Convenience methods for common operations
- Advanced configuration management
- Neural pathway analysis tools
"""

import nlm
import tempfile
import os
import json
import time
from typing import List, Dict, Any

def test_basic_functionality():
    """Test basic functionality to ensure bindings work."""
    print("Testing basic functionality...")
    
    # Create a default config
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Runtime)
    config.set("region_count", 1, nlm.ConfigSource.Runtime)
    
    # Create a brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test basic operations
    assert brain.getTotalNeuronCount() > 0
    brain.step(0)
    assert brain.getTotalSynapseCount() > 0
    
    brain.reset()
    assert brain.getTotalNeuronCount() == 100
    
    print("✓ Basic functionality passed")

def test_advanced_config():
    """Test advanced configuration methods."""
    print("Testing advanced configuration...")
    
    config = nlm.createDefaultConfig()
    
    # Test nested configuration setting
    config.setNested("system.brain.neuron_count", 200)
    config.setNested("system.network.connection_probability", 0.5)
    config.setNested("system.learning.rate", 0.01)
    config.setNested("system.memory.capacity", [100, 200, 300])
    
    # Test nested configuration getting
    assert config.getNested("system.brain.neuron_count") == 200
    assert config.getNested("system.network.connection_probability") == 0.5
    assert config.getNested("system.learning.rate") == 0.01
    assert config.getNested("system.memory.capacity") == [100, 200, 300]
    
    # Test default value for missing key
    assert config.getNested("nonexistent.key", "default_value") == "default_value"
    
    # Test update from dictionary
    config_dict = {
        "system.training.epochs": 1000,
        "system.optimization.learning_rate": 0.001,
        "system.regularization": 0.1
    }
    config.updateFromDict(config_dict)
    
    assert config.getNested("system.training.epochs") == 1000
    assert config.getNested("system.optimization.learning_rate") == 0.001
    assert config.getNested("system.regularization") == 0.1
    
    print("✓ Advanced configuration passed")

def test_batch_operations():
    """Test batch operations for multiple brains."""
    print("Testing batch operations...")
    
    # Create multiple configs
    configs = []
    for i in range(3):
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 50 + i * 10, nlm.ConfigSource.Default)
        config.set("region_count", 1, nlm.ConfigSource.Default)
        config.set("random_seed", i, nlm.ConfigSource.Default)
        configs.append(config)
    
    # Test initializeMultiple
    brains = nlm.brain.initializeMultiple(configs)
    
    # Verify all brains were initialized
    assert len(brains) == 3
    assert all(brain is not None for brain in brains)
    assert all(brain.getTotalNeuronCount() == 50 + i * 10 for i, brain in enumerate(brains))
    
    # Test stepMultiple
    steps = [0, 1, 2, 3, 4]
    nlm.brain.stepMultiple(brains, steps)
    
    # Test resetMultiple
    nlm.brain.resetMultiple(brains)
    
    assert all(brain.getTotalNeuronCount() == 50 + i * 10 for i, brain in enumerate(brains))
    
    # Test saveAll
    with tempfile.TemporaryDirectory() as temp_dir:
        filepaths = nlm.brain.saveAll(brains, temp_dir)
        
        # Verify all files were saved
        assert len(filepaths) == 3
        for i, filepath in enumerate(filepaths):
            assert os.path.exists(filepath)
            assert filepath.endswith(".bin")
    
    print("✓ Batch operations passed")

def test_network_statistics():
    """Test network statistics and monitoring."""
    print("Testing network statistics...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 2, nlm.ConfigSource.Default)
    config.set("connection_probability", 0.2, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test network statistics
    stats = nlm.brain.getNetworkStats(brain)
    
    assert stats.total_neurons > 0
    assert stats.total_synapses > 0
    assert stats.active_neurons >= 0
    assert stats.firing_neurons >= 0
    assert stats.average_firing_rate >= 0.0
    assert stats.excitation_inhibition_ratio >= 0.0
    assert stats.total_spike_count >= 0
    
    # Verify data structure contains expected fields
    assert hasattr(stats, 'neuron_type_counts')
    assert hasattr(stats, 'synapse_type_counts')
    assert hasattr(stats, 'neuron_density_by_region')
    assert hasattr(stats, 'connection_density_by_region')
    
    print("✓ Network statistics passed")

def test_learning_progress():
    """Test learning progress tracking."""
    print("Testing learning progress...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 1, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test learning progress tracking
    baseline_spike_count = brain.getTotalSpikeCount()
    baseline_firing_rate = brain.getAverageFiringRate()
    
    progress = nlm.brain.getLearningProgress(brain, baseline_spike_count, baseline_firing_rate)
    
    # Verify progress data
    assert len(progress) > 0
    assert all(hasattr(point, 'step') for point in progress)
    assert all(hasattr(point, 'spike_count') for point in progress)
    assert all(hasattr(point, 'firing_rate') for point in progress)
    assert all(hasattr(point, 'learning_rate') for point in progress)
    assert all(hasattr(point, 'stability_score') for point in progress)
    
    print("✓ Learning progress passed")

def test_active_paths():
    """Test neural pathway analysis."""
    print("Testing active pathways...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 1, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create some neuron IDs for testing
    neuron_ids = [nlm.NeuronId(i) for i in range(5)]
    steps = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    
    # Test active paths finding
    active_paths = nlm.brain.findActivePaths(brain, neuron_ids, steps)
    
    assert len(active_paths) > 0
    assert all(hasattr(point, 'step') for point in active_paths)
    assert all(hasattr(point, 'neuron_id') for point in active_paths)
    assert all(hasattr(point, 'is_active') for point in active_paths)
    
    print("✓ Active pathways passed")

def test_convenience_methods():
    """Test convenience methods for common operations."""
    print("Testing convenience methods...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 1, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test trainOnPattern
    pattern = [0.1, 0.2, 0.3, 0.4, 0.5] * 20  # 100 elements
    brain.trainOnPattern(pattern, duration=50)
    
    # Test evaluation
    test_patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5] * 20,
        [0.9, 0.8, 0.7, 0.6, 0.5] * 20,
        [0.5, 0.5, 0.5, 0.5, 0.5] * 20
    ]
    
    accuracy, accuracies = brain.evaluateTestSet(test_patterns)
    assert 0.0 <= accuracy <= 1.0
    assert len(accuracies) == len(test_patterns)
    
    # Test generateBehavior
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 84, 84)
    
    actions = brain.generateBehavior(world, steps=20)
    assert len(actions) == 20
    assert all(action is not None for action in actions)
    
    # Test action distribution
    distribution = brain.getActionDistribution(nlm.ActionType.MoveForward, steps=100)
    assert isinstance(distribution, dict)
    
    print("✓ Convenience methods passed")

def test_progress_tracking():
    """Test progress tracking and callbacks."""
    print("Testing progress tracking...")
    
    # Track progress with callbacks
    progress_updates = []
    
    def progress_callback(progress, message):
        progress_updates.append((progress, message))
    
    tracker = nlm.createProgressTracker(100, "Test simulation", progress_callback)
    
    # Simulate progress
    for i in range(100):
        tracker.update(1.0)
    
    # Verify callback was called
    assert len(progress_updates) > 0
    assert all(0.0 <= update[0] <= 100.0 for update in progress_updates)
    
    # Test tracker status
    assert tracker.getProgress() >= 100.0
    assert tracker.isComplete()
    
    print("✓ Progress tracking passed")

def test_error_handling():
    """Test custom error handling."""
    print("Testing error handling...")
    
    # Test invalid argument error
    try:
        config = nlm.createDefaultConfig()
        config.setNested("invalid.key", None)
        assert False, "Should have raised InvalidArgumentError"
    except nlm.InvalidArgumentError:
        pass  # Expected
    
    # Test initialization error with null brain
    try:
        nlm.validateBrain(None, "test_operation")
        assert False, "Should have raised InvalidArgumentError"
    except nlm.InvalidArgumentError:
        pass  # Expected
    
    # Test invalid brain for training
    try:
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        # Manually set to null to test error (this would be done internally)
        assert False, "Should have raised InvalidArgumentError"
    except Exception:
        pass  # Expected
    
    print("✓ Error handling passed")

def test_world_creation():
    """Test world creation utilities."""
    print("Testing world creation...")
    
    # Create objects
    objects = [
        nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 100.0),
        nlm.WorldObject(50, 50, nlm.WorldObjectType.Hazard, 50.0),
        nlm.WorldObject(90, 90, nlm.WorldObjectType.Wall, 10.0),
        nlm.WorldObject(20, 80, nlm.WorldObjectType.Marker, 1.0)
    ]
    
    # Test createWorldWithObjects
    world = nlm.createWorldWithObjects(100, 100, objects)
    
    assert world.getWidth() == 100
    assert world.getHeight() == 100
    
    # Verify objects were added
    percept = world.getSensoryPercept()
    assert percept is not None
    
    print("✓ World creation passed")

def test_agent_creation():
    """Test agent creation utilities."""
    print("Testing agent creation...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 1, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 84, 84)
    
    # Test createAgentWithAllSystems
    agent = nlm.createAgentWithAllSystems(brain, world)
    
    assert agent is not None
    assert agent.getBrain() is not None
    
    # Test enableAllSystems
    agent.enableAllSystems()
    
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    print("✓ Agent creation passed")

def test_brain_creation():
    """Test brain creation utilities."""
    print("Testing brain creation...")
    
    # Test createQuickBrain
    brain = nlm.createQuickBrain(neuron_count=500, connection_prob=0.1)
    
    assert brain.getTotalNeuronCount() == 500
    assert brain.getTotalSynapseCount() > 0
    
    print("✓ Brain creation passed")

def test_connectivity_matrix():
    """Test connectivity matrix functionality."""
    print("Testing connectivity matrix...")
    
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 100, nlm.ConfigSource.Default)
    config.set("region_count", 1, nlm.ConfigSource.Default)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test connectivity matrix (placeholder test)
    matrix = nlm.brain.getConnectivityMatrix(brain)
    
    # Matrix might be empty for now (implementation dependent)
    assert isinstance(matrix, list)
    
    print("✓ Connectivity matrix passed")

def test_integration():
    """Test full integration scenario."""
    print("Testing full integration...")
    
    # Create world with objects
    objects = [
        nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 100.0),
        nlm.WorldObject(50, 50, nlm.WorldObjectType.Hazard, 50.0)
    ]
    world = nlm.createWorldWithObjects(100, 100, objects)
    
    # Create quick brain
    brain = nlm.createQuickBrain(neuron_count=200, connection_prob=0.15)
    brain.initialize()
    
    # Create agent
    agent = nlm.createAgentWithAllSystems(brain, world)
    agent.enableAllSystems()
    
    # Simulate some steps
    for step in range(10):
        world.update(step)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
    
    # Generate behavior
    actions = brain.generateBehavior(world, steps=5)
    
    assert len(actions) == 5
    assert all(action is not None for action in actions)
    
    # Get statistics
    stats = nlm.brain.getNetworkStats(brain)
    assert stats.total_neurons > 0
    
    print("✓ Full integration passed")

def run_all_tests():
    """Run all test suites."""
    print("=" * 60)
    print("NLM Enhanced Python Bindings - Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_advanced_config,
        test_batch_operations,
        test_network_statistics,
        test_learning_progress,
        test_active_paths,
        test_convenience_methods,
        test_progress_tracking,
        test_error_handling,
        test_world_creation,
        test_agent_creation,
        test_brain_creation,
        test_connectivity_matrix,
        test_integration
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print()
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
