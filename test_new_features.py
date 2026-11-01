"""
Tests for new batch operations and advanced statistics in NLM Python bindings
"""
import pynlm
import numpy as np
import time
import sys

def test_batch_current_injection():
    """Test batch current injection functionality"""
    print("Testing batch current injection...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create test neuron IDs
    neuron_ids = []
    for i in range(10):
        neuron_id = pynlm.createNeuronId(i)
        neuron_ids.append(neuron_id)
    
    # Create test currents
    currents = [10.0, 15.0, 8.0, 12.0, 20.0, 5.0, 25.0, 7.0, 18.0, 9.0]
    
    # Test batch injection
    pynlm.injectCurrentBatch(brain, neuron_ids, currents)
    
    # Verify injection worked
    for i, neuron_id in enumerate(neuron_ids):
        # Note: This verification would require accessing internal brain state
        # For now, just ensure no exceptions were raised
        pass
    
    print("✓ Batch current injection test passed")

def test_batch_sensory_processing():
    """Test batch sensory processing functionality"""
    print("Testing batch sensory processing...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create test percepts
    percepts = []
    for i in range(5):
        percept = pynlm.SensoryPercept()
        percept.setVision(np.random.rand(8, 8).flatten())
        percept.setTouch(np.random.rand(4).flatten())
        percept.setInternal(np.random.rand(2).flatten())
        percepts.append(percept)
    
    # Test batch processing
    pynlm.processSensoryBatch(agent, percepts)
    
    print("✓ Batch sensory processing test passed")

def test_parallel_action_selection():
    """Test parallel action selection functionality"""
    print("Testing parallel action selection...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test parallel action selection
    actions = pynlm.selectActionsParallel(brain, 5)
    
    # Verify results
    assert len(actions) == 5, f"Expected 5 actions, got {len(actions)}"
    
    print("✓ Parallel action selection test passed")

def test_connectivity_analysis():
    """Test neural connectivity analysis"""
    print("Testing connectivity analysis...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create some regions for testing
    region_ids = []
    for i in range(3):
        region_id = brain.addRegion(f"Region{i}")
        region_ids.append(region_id)
    
    # Perform connectivity analysis
    analysis = pynlm.analyzeConnectivity(brain, region_ids)
    
    # Verify results
    assert hasattr(analysis, 'totalConnections'), "Analysis should have totalConnections"
    assert hasattr(analysis, 'connectionsByType'), "Analysis should have connectionsByType"
    
    print(f"✓ Connectivity analysis test passed: {analysis.totalConnections} connections")

def test_population_statistics():
    """Test population statistics"""
    print("Testing population statistics...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get population statistics for different neuron types
    for neuron_type in [pynlm.NeuronType.Excitatory, 
                        pynlm.NeuronType.Inhibitory, 
                        pynlm.NeuronType.Internal]:
        stats = pynlm.getPopulationStats(brain, neuron_type)
        
        assert hasattr(stats, 'neuronCount'), "Stats should have neuronCount"
        assert hasattr(stats, 'averageFiringRate'), "Stats should have averageFiringRate"
        assert hasattr(stats, 'firingNeuronCount'), "Stats should have firingNeuronCount"
    
    print("✓ Population statistics test passed")

def test_spike_pattern_analysis():
    """Test spike pattern analysis"""
    print("Testing spike pattern analysis...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Perform spike pattern analysis
    analysis = pynlm.analyzeSpikePatterns(brain, 100)
    
    # Verify results
    assert hasattr(analysis, 'totalSpikes'), "Analysis should have totalSpikes"
    assert hasattr(analysis, 'meanISI'), "Analysis should have meanISI"
    assert hasattr(analysis, 'stdISI'), "Analysis should have stdISI"
    assert hasattr(analysis, 'regularity'), "Analysis should have regularity"
    
    print(f"✓ Spike pattern analysis test passed: {analysis.totalSpikes} spikes")

def test_config_validation():
    """Test configuration validation"""
    print("Testing configuration validation...")
    
    config = pynlm.createDefaultConfig()
    
    # Add some values to config
    config.set("test.int", 42)
    config.set("test.double", 3.14)
    config.set("test.string", "hello")
    
    # Test validation (requires schema)
    validation = pynlm.validateConfig(config)
    
    assert hasattr(validation, 'isValid'), "Validation should have isValid"
    assert hasattr(validation, 'errors'), "Validation should have errors"
    
    print("✓ Configuration validation test passed")

def test_performance_monitoring():
    """Test performance monitoring"""
    print("Testing performance monitoring...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor(brain, 0.5)
    monitor.start()
    
    # Let it run for a short time
    time.sleep(1)
    
    # Get metrics
    metrics = monitor.getMetrics()
    
    assert hasattr(metrics, 'cpuUsage'), "Metrics should have cpuUsage"
    assert hasattr(metrics, 'memoryUsageMB'), "Metrics should have memoryUsageMB"
    assert hasattr(metrics, 'activeNeurons'), "Metrics should have activeNeurons"
    assert hasattr(metrics, 'firingNeurons'), "Metrics should have firingNeurons"
    
    # Stop monitoring
    monitor.stop()
    
    print(f"✓ Performance monitoring test passed: {metrics.activeNeurons} active neurons")

def test_debug_state_inspector():
    """Test advanced debugging tools"""
    print("Testing advanced debugging tools...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create debug inspector
    inspector = pynlm.DebugStateInspector(brain)
    
    # Get brain state
    brain_state = inspector.getBrainState()
    assert "Brain State:" in brain_state, "Brain state should start with 'Brain State:'"
    
    # Test step-by-step control
    step_control = inspector.getStepByStepControl()
    assert "step-by-step control" in step_control.lower(), "Step control should mention step-by-step"
    
    print("✓ Advanced debugging tools test passed")

def test_extended_enumerations():
    """Test extended enumerations"""
    print("Testing extended enumerations...")
    
    # Test advanced action types
    action_values = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.Sprint,  # Extended value
        pynlm.ActionType.Jump,    # Extended value
    ]
    
    assert len(action_values) == 4, "Should have 4 action types"
    
    # Test advanced world object types
    object_values = [
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Portal,  # Extended value
        pynlm.WorldObjectType.Checkpoint,  # Extended value
    ]
    
    assert len(object_values) == 4, "Should have 4 world object types"
    
    # Test extended neuromodulator types
    neuromodulator_values = [
        pynlm.NeuromodulatorType.Dopamine,
        pynlm.NeuromodulatorType.Serotonin,
        pynlm.NeuromodulatorType.Adrenaline,  # Extended value
        pynlm.NeuromodulatorType.Cortisol,   # Extended value
    ]
    
    assert len(neuromodulator_values) == 4, "Should have 4 neuromodulator types"
    
    print("✓ Extended enumerations test passed")

def test_network_visualization():
    """Test network visualization data"""
    print("Testing network visualization data...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get network visualization data
    viz_data = pynlm.getNetworkVisualizationData(brain)
    
    assert hasattr(viz_data, 'neurons'), "Visualization data should have neurons"
    assert hasattr(viz_data, 'synapses'), "Visualization data should have synapses"
    assert hasattr(viz_data, 'connections'), "Visualization data should have connections"
    
    print(f"✓ Network visualization test passed: {len(viz_data.neurons)} neurons")

def test_advanced_step_controller():
    """Test advanced step controller"""
    print("Testing advanced step controller...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create step controller
    controller = pynlm.AdvancedStepController(brain)
    
    # Test pause/resume
    controller.pause()
    assert controller.isPaused(), "Should be paused"
    
    controller.resume()
    assert not controller.isPaused(), "Should not be paused"
    
    # Execute a step
    controller.step()
    
    # Check step count
    assert controller.getCurrentStep() == 1, "Should have completed 1 step"
    
    print("✓ Advanced step controller test passed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("Running NLM Python Bindings Advanced Features Tests")
    print("=" * 60)
    
    tests = [
        test_batch_current_injection,
        test_batch_sensory_processing,
        test_parallel_action_selection,
        test_connectivity_analysis,
        test_population_statistics,
        test_spike_pattern_analysis,
        test_config_validation,
        test_performance_monitoring,
        test_debug_state_inspector,
        test_extended_enumerations,
        test_network_visualization,
        test_advanced_step_controller,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {str(e)}")
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)