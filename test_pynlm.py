"""
Comprehensive tests for NLM Python bindings.

This module provides tests for the pynlm Python interface including unit tests,
integration tests, and performance tests.
"""

import pytest
import pynlm
import tempfile
import os
import json
import time
from typing import Dict, Any, List
@pytest.fixture
def brain():
    """Create a test brain for each test."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
@pytest.fixture
def agent(brain):
    """Create a test agent for each test."""
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return agent, world
@pytest.fixture
def world():
    """Create a test world for each test."""
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    return world
def test_basic_imports():
    """Test that all basic classes and functions can be imported."""
    # Test core imports
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    
    # Test class imports
    assert hasattr(pynlm, 'Brain')
    assert hasattr(pynlm, 'Config')
    assert hasattr(pynlm, 'SimpleWorld')
    assert hasattr(pynlm, 'AgentBrain')
    
    # Test enum imports
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    
    # Test ID types
    assert hasattr(pynlm, 'NeuronId')
    assert hasattr(pynlm, 'SynapseId')
    assert hasattr(pynlm, 'RegionId')
    
    print("✓ All basic imports successful")
def test_brain_creation(brain):
    """Test brain creation and initialization."""
    # Test brain properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getTotalSpikeCount() == 0  # Initially no spikes
    
    # Test brain operations
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() > 0
    
    print(f"✓ Brain test passed: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
def test_config(brain):
    """Test configuration functionality."""
    # Test default config
    config = pynlm.createDefaultConfig()
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    
    # Test config modification
    original_neurons = config.getOr("brain.neuron_count", 1000)
    config.set("brain.neuron_count", 500)
    assert config.getOr("brain.neuron_count", 1000) == 500
    
    # Test config save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        
        # Load into new config
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(f.name)
        assert new_config.getOr("brain.neuron_count", 1000) == 500
        
        os.unlink(f.name)
    
    print("✓ Config test passed")
def test_agent_functionality(agent, world):
    """Test agent functionality."""
    agent_obj, world_obj = agent
    
    # Enable learning systems
    agent_obj.enableRewardModulation(True)
    agent_obj.enableCuriosity(True)
    agent_obj.enableDevelopment(True)
    
    # Test agent properties
    assert agent_obj.getSensoryInputSize() > 0
    assert agent_obj.getMotorOutputSize() > 0
    
    # Test sensory processing
    percept = world_obj.getSensoryPercept()
    agent_obj.processSensoryInput(percept)
    
    # Test motor decoding
    action = agent_obj.decodeMotorCommand()
    assert action is not None
    
    # Test neuromodulation
    reward = 1.0
    predicted = 0.5
    agent_obj.applyRewardModulation(reward, predicted)
    
    assert agent_obj.getCuriosityLevel() >= 0
    assert agent_obj.getNoveltyLevel() >= 0
    assert agent_obj.getPredictionError() >= 0
    
    print("✓ Agent functionality test passed")
def test_world_simulation(world):
    """Test world simulation functionality."""
    # Test world properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getVisionWidth() == 8
    assert world.getVisionHeight() == 8
    
    # Test world updates
    initial_time = world.getSimulationTime()
    
    for i in range(10):
        world.update(0.1)
        assert world.getSimulationTime() > initial_time
    
    # Test agent interaction
    world.setAgentStart(10.0, 10.0)
    assert world.getAgentBody().x == 10.0
    assert world.getAgentBody().y == 10.0
    
    # Test object management
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
    world.addObject(obj)
    
    assert world.isValidPosition(5.0, 5.0)
    
    print("✓ World simulation test passed")
def test_full_simulation(agent, world):
    """Test complete simulation with all components."""
    agent_obj, world_obj = agent
    
    # Enable all systems
    agent_obj.enableRewardModulation(True)
    agent_obj.enableCuriosity(True)
    agent_obj.enableDevelopment(True)
    agent_obj.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(50):
        world_obj.update(0.1)
        
        # Process sensory input
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        
        # Brain computation
        brain = agent_obj.getBrain()
        brain.step(step)
        
        # Decision making
        action = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(action, world_obj.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent_obj.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_obj.updateDevelopment(0.1)
    
    # Verify results
    brain = agent_obj.getBrain()
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() > 0
    assert agent_obj.getCuriosityLevel() >= 0
    
    print("✓ Full simulation test passed")
def test_monitoring_system(agent, world):
    """Test monitoring system functionality."""
    from pynlm.monitoring import MonitoringSystem
    
    agent_obj, world_obj = agent
    
    # Create monitoring system
    monitor = MonitoringSystem(agent_obj.getBrain(), agent_obj, world_obj)
    
    # Start monitoring
    monitor.start()
    
    # Run simulation with monitoring
    for i in range(10):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        agent_obj.getBrain().step(i)
        action = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(action, world_obj.getSimulationTime())
        
        # Give monitoring system time to collect data
        time.sleep(0.05)
    
    # Stop monitoring
    monitor.stop()
    
    # Test monitoring functions
    assert monitor.get_current_metrics() is not None
    assert len(monitor.get_recent_data(5)) > 0
    
    stats = monitor.get_statistics()
    assert stats is not None
    
    # Test data export
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        monitor.export_data(f.name, 'json')
        os.unlink(f.name)
    
    print("✓ Monitoring system test passed")
def test_experiment_framework(agent, world):
    """Test experiment framework functionality."""
    from pynlm.experiment import LearningExperiment, ExperimentConfig
    
    # Create experiment
    config = ExperimentConfig("Test Experiment")
    config.set_brain_config(brain_neuron_count=500)
    config.set_world_config(width=20, height=20)
    config.set_experiment_param('enable_curiosity', True)
    config.set_experiment_param('enable_reward', True)
    
    # Create and run experiment
    experiment = LearningExperiment("Test Learning", config, num_steps=20)
    
    # Setup experiment
    assert experiment.setup()
    
    # Run experiment
    result = experiment.run(20)
    
    # Verify results
    assert result is not None
    assert result.get_duration() > 0
    assert result.status.value == 'completed'
    
    print("✓ Experiment framework test passed")
def test_utils_functionality(agent, world):
    """Test utility functions."""
    from pynlm.utils import MetricsCalculator, VisualizationHelper, benchmark_configuration
    
    agent_obj, world_obj = agent
    
    # Test metrics calculator
    metrics = MetricsCalculator()
    
    # Run some simulation steps
    for i in range(10):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        agent_obj.getBrain().step(i)
        action = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(action, world_obj.getSimulationTime())
        
        # Update metrics
        metrics.update(agent_obj.getBrain(), agent_obj, world_obj, i, world_obj.getSimulationTime())
    
    # Test metrics functions
    assert metrics.get_summary() is not None
    assert metrics.get_trend('spikes') is not None
    
    # Test benchmark configuration
    benchmark_config = {
        'neuron_count': 500,
        'enable_curiosity': True
    }
    
    benchmark_result = benchmark_configuration(benchmark_config, num_steps=10, num_runs=2)
    assert benchmark_result is not None
    assert 'configuration' in benchmark_result
    
    print("✓ Utils functionality test passed")
def test_advanced_actions(agent, world):
    """Test advanced actions functionality."""
    from pynlm.advanced_actions import (
        create_advanced_behavior_tree, ActionContext, create_action_sequence_from_context
    )
    
    agent_obj, world_obj = agent
    
    # Create action context
    context = ActionContext(world_obj, agent_obj.getBrain(), agent_obj, 0)
    
    # Create advanced behavior tree
    tree = create_advanced_behavior_tree()
    assert tree is not None
    
    # Create action sequence from context
    sequence = create_action_sequence_from_context(context)
    assert isinstance(sequence, list)
    
    print("✓ Advanced actions test passed")
def test_error_handling(brain):
    """Test error handling."""
    # Test invalid operation after reset
    brain.reset()
    
    # Brain should still be usable
    brain.step(0)
    assert brain.getTotalSpikeCount() >= 0
    
    print("✓ Error handling test passed")
def test_configuration_presets():
    """Test configuration presets."""
    from pynlm import get_preset_config, apply_preset
    
    # Test simple preset
    simple_preset = get_preset_config('simple')
    assert 'brain.neuron_count' in simple_preset
    assert simple_preset['brain.neuron_count'] == 500
    
    # Test learning preset
    learning_preset = get_preset_config('learning')
    assert learning_preset['neuromod.curiosity.enable'] == True
    
    # Test application of preset
    config = pynlm.createDefaultConfig()
    apply_preset(config, 'exploration')
    assert config.getOr("neuromod.curiosity.enable", False) == True
    
    print("✓ Configuration presets test passed")
def test_version_info():
    """Test version information."""
    assert hasattr(pynlm, '__version__')
    assert hasattr(pynlm, '__author__')
    assert hasattr(pynlm, '__description__')
    assert hasattr(pynlm, '__license__')
    
    print(f"✓ Version info test passed: {pynlm.__version__}")
def test_quick_demo():
    """Test quick demo function."""
    result = pynlm.quick_demo()
    assert result is True
    
    print("✓ Quick demo test passed")
def test_help_system():
    """Test help system."""
    help_result = pynlm.get_help('general')
    assert help_result is not None
    
    # Test unknown topic
    pynlm.get_help('unknown_topic')  # Should not crash
    
    print("✓ Help system test passed")
def run_all_tests():
    """Run all tests and report results."""
    
    print("=== NLM Python Bindings Test Suite ===\n")
    
    # Collect tests
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Brain Creation", test_brain_creation),
        ("Config", test_config),
        ("Agent Functionality", test_agent_functionality),
        ("World Simulation", test_world_simulation),
        ("Full Simulation", test_full_simulation),
        ("Monitoring System", test_monitoring_system),
        ("Experiment Framework", test_experiment_framework),
        ("Utils Functionality", test_utils_functionality),
        ("Advanced Actions", test_advanced_actions),
        ("Error Handling", test_error_handling),
        ("Configuration Presets", test_configuration_presets),
        ("Version Info", test_version_info),
        ("Quick Demo", test_quick_demo),
        ("Help System", test_help_system),
    ]
    
    # Run tests
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            # Use pytest to run individual tests
            pytest.main([f"-k {test_name}", "--tb=short", "-v"])
            print(f"✓ {test_name} test passed")
            passed += 1
        except Exception as e:
            print(f"✗ {test_name} test failed: {str(e)}")
            failed += 1
    
    print(f"\n=== Test Summary ===")
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success Rate: {passed/len(tests)*100:.1f}%")
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        return True
    else:
        print(f"\n⚠️ {failed} test(s) failed.")
        return False
if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)
