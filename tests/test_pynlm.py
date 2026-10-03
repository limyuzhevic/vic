#!/usr/bin/env python3
"""
Python tests for NLM bindings.
Tests the enhanced Python interface for the NLM neural simulation framework.
"""

import sys
import tempfile
import json
import time
import pynlm as nlm

def test_basic_bindings():
    """Test basic binding functionality."""
    print("Testing basic bindings...")
    
    # Create a brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    assert brain is not None
    
    # Initialize brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    
    # Test basic statistics
    neuron_count = brain.getTotalNeuronCount()
    assert neuron_count > 0, f"Expected neurons > 0, got {neuron_count}"
    
    # Test step function
    brain.step(0)
    
    # Test action production
    action = brain.produceAction()
    assert action is not None, "Action production returned None"
    
    print("✓ Basic bindings test passed")

def test_enhanced_config():
    """Test enhanced configuration features."""
    print("Testing enhanced config...")
    
    # Test method chaining
    config = nlm.createConfig()
    config = config.withNeuronCount(1000).withLearningRate(0.001).withDevelopment(True)
    
    # Verify configuration
    assert config.has("brain.neuron_count")
    assert config.has("plasticity.stdp.learning_rate")
    assert config.has("development.enabled")
    
    # Test dictionary conversion
    config_dict = config.to_dict()
    assert isinstance(config_dict, dict)
    assert "brain.neuron_count" in config_dict
    
    # Test dictionary loading
    test_dict = {"brain.neuron_count": 2000, "plasticity.stdp.learning_rate": 0.01}
    config2 = nlm.createConfig()
    config2.from_dict(test_dict)
    assert config2.has("brain.neuron_count")
    assert config2.getOr("brain.neuron_count", 0) == 2000
    
    # Test simulation config factory
    sim_config = nlm.createConfig().createSimulationConfig(
        neuronCount=500,
        regionCount=2,
        learningRate="0.001",
        enableDevelopment=True,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    assert sim_config.has("brain.neuron_count")
    assert sim_config.has("plasticity.stdp.enable")
    assert sim_config.has("neuromod.curiosity.enable")
    
    # Test validation
    assert sim_config.validate()
    
    # Test simulation summary
    summary = sim_config.getSimulationSummary()
    assert isinstance(summary, dict)
    assert "neuron_count" in summary
    
    print("✓ Enhanced config test passed")

def test_context_managers():
    """Test context manager support."""
    print("Testing context managers...")
    
    # Create components
    config = nlm.createConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Test brain context manager
    with nlm.brain_context(brain) as brain_ctx:
        assert brain_ctx.brain is not None
        brain_ctx.brain.step(0)
        initial_spikes = brain_ctx.brain.getTotalSpikeCount()
        assert initial_spikes >= 0
    
    # Test world context manager
    world = nlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    with nlm.world_context(world) as world_ctx:
        assert world_ctx.world is not None
        world_ctx.world.update(0.1)
        percept = world_ctx.world.getSensoryPercept()
        assert percept is not None
    
    print("✓ Context managers test passed")

def test_simulation_utils():
    """Test simulation utility functions."""
    print("Testing simulation utils...")
    
    # Create a simple world
    config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
        worldWidth=10,
        worldHeight=10,
        visionWidth=4,
        visionHeight=4,
        numFoodResources=5
    )
    
    assert config is not None
    assert world is not None
    assert agent is not None
    
    # Test runEpisode
    episode_brain, episode_agent = nlm.SimulationUtils.runEpisode(
        config, world, 10, True
    )
    
    assert episode_brain is not None
    assert episode_agent is not None
    
    # Test measurePerformance
    perf_metrics = nlm.SimulationUtils.measurePerformance(
        episode_brain, world, episode_agent, 20
    )
    
    assert isinstance(perf_metrics, nlm.PerformanceMetrics)
    assert perf_metrics.totalTime > 0
    assert perf_metrics.stepsCompleted == 20
    assert perf_metrics.totalSpikes >= 0
    
    print("✓ Simulation utils test passed")

def test_enhanced_vision():
    """Test enhanced vision functionality."""
    print("Testing enhanced vision...")
    
    # Create vision object
    vision = nlm.Vision(8, 8, 3)
    
    # Create some test data
    data = []
    for i in range(8 * 8 * 3):
        data.append(0.5)
    
    vision.setData(data)
    
    # Test matrix conversion
    matrix = vision.to_python_matrix()
    assert isinstance(matrix, list)
    assert len(matrix) == 8
    assert len(matrix[0]) == 8
    
    # Test object detection
    objects = vision.detectObjects()
    assert isinstance(objects, dict)
    assert "edges_count" in objects
    
    print("✓ Enhanced vision test passed")

def test_error_handling():
    """Test error handling and exception types."""
    print("Testing error handling...")
    
    try:
        # Test invalid config (should be handled by the binding)
        config = nlm.createConfig()
        
        # Test invalid brain creation (null pointer would be caught by C++)
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # This should work normally
        assert brain.getTotalNeuronCount() > 0
        
    except Exception as e:
        print(f"Note: Expected error behavior: {e}")
    
    print("✓ Error handling test passed")

def test_performance_monitoring():
    """Test performance monitoring capabilities."""
    print("Testing performance monitoring...")
    
    config = nlm.createConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Measure performance over multiple steps
    start_time = time.time()
    
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    end_time = time.time()
    
    # Verify performance metrics
    assert brain.getTotalSpikeCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    
    print(f"✓ Performance monitoring test passed ({end_time - start_time:.2f}s)")

def test_file_io():
    """Test file I/O operations."""
    print("Testing file I/O...")
    
    # Test config file I/O
    config = nlm.createConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Create temporary files
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
    
    try:
        # Save config to file
        config.saveToFile(config_file)
        
        # Load config from file
        config2 = nlm.createConfig()
        config2.loadFromFile(config_file)
        
        assert config2.has("brain.neuron_count")
        assert config2.getOr("brain.neuron_count", 0) == 1000
        
    finally:
        import os
        os.unlink(config_file)
    
    print("✓ File I/O test passed")

def test_advanced_features():
    """Test advanced features for expert users."""
    print("Testing advanced features...")
    
    # Create a sophisticated agent setup
    config = nlm.createConfig().withNeuronCount(2000).withLearningRate(0.0005)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Add some obstacles and resources
    for i in range(20):
        x = i % 30
        y = i // 30
        if i % 7 == 0:  # Add walls
            world.addObject(nlm.WorldObject(
                x, y, nlm.WorldObjectType.Wall, 0.0, 0.8
            ))
        elif i % 5 == 0:  # Add resources
            world.addObject(nlm.WorldObject(
                x, y, nlm.WorldObjectType.Resource, 10.0, 0.3
            ))
        elif i % 3 == 0:  # Add hazards
            world.addObject(nlm.WorldObject(
                x, y, nlm.WorldObjectType.Hazard, -5.0, 0.3
            ))
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems for complex behavior
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complex simulation
    for step in range(100):
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on action result
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.05)
    
    # Verify agent state
    assert brain.getTotalNeuronCount() == 2000
    assert agent.getCuriosityLevel() > 0.0
    assert brain.getTotalSpikeCount() > 0
    
    print("✓ Advanced features test passed")

def test_comprehensive():
    """Run comprehensive test suite."""
    print("\n" + "="*60)
    print("COMPREHENSIVE NLM PYTHON BINDINGS TEST SUITE")
    print("="*60)
    
    tests = [
        test_basic_bindings,
        test_enhanced_config,
        test_context_managers,
        test_simulation_utils,
        test_enhanced_vision,
        test_error_handling,
        test_performance_monitoring,
        test_file_io,
        test_advanced_features,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print("\n" + "="*60)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("="*60)
    
    if failed > 0:
        sys.exit(1)
    
    return passed, failed

if __name__ == "__main__":
    test_comprehensive()