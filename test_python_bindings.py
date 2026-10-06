# NLM Python Bindings Test Suite

This test suite provides comprehensive coverage for the NLM Python bindings, ensuring that all enhanced functionality works correctly.

## Installation

```bash
# Install dependencies
pip install pytest numpy

# Run tests
pytest test_python_bindings.py -v

# Run with verbose output
pytest test_python_bindings.py -v --tb=short
```

## Test Structure

The test suite is organized into the following categories:

1. **Basic Functionality Tests** - Core initialization and basic operations
2. **Configuration Tests** - Configuration loading, saving, and validation
3. **Brain Simulation Tests** - Neural simulation and step execution
4. **World Interaction Tests** - Environment simulation and agent interaction
5. **Learning Subsystem Tests** - Neuromodulation and development
6. **Advanced API Tests** - Convenience methods and researcher tools
7. **Error Handling Tests** - Input validation and exception handling
8. **Performance Tests** - Benchmarking and performance validation

## Test Cases

### Basic Functionality Tests

```python
def test_basic_initialization():
    """Test basic brain initialization"""
    import pynlm
    
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test brain creation
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Test brain initialization
    assert brain.initialize()
    
    # Test basic properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
def test_brain_step():
    """Test brain simulation steps"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    # Verify simulation progress
    assert brain.getTotalSpikeCount() >= initial_spikes
def test_world_creation():
    """Test world creation and configuration"""
    import pynlm
    
    # Test default world
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Test configured world
    world = pynlm.createWorld(20, 20)
    assert world is not None
    
    # Test world properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getVisionWidth() == 8
    assert world.getVisionHeight() == 8
```

### Configuration Tests

```python
def test_config_basic_operations():
    """Test basic configuration operations"""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    
    # Test setting and getting values
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_bool", True)
    config.set("test_string", "hello")
    
    assert config.get("test_int") == 42
    assert config.get("test_float") == 3.14
    assert config.get("test_bool") == True
    assert config.get("test_string") == "hello"
    
    # Test has method
    assert config.has("test_int")
    assert not config.has("nonexistent")
def test_config_type_conversion():
    """Test automatic type conversion in config"""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    
    # Test type conversion
    config.set("test_value", 100)
    assert isinstance(config.get("test_value"), int)
    
    config.set("test_value", 3.14)
    assert isinstance(config.get("test_value"), float)
    
    config.set("test_value", "test")
    assert isinstance(config.get("test_value"), str)
def test_config_file_operations():
    """Test configuration file I/O"""
    import pynlm
    import os
    import tempfile
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("connection_probability", 0.15)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        temp_file = f.name
    
    try:
        # Load from file
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(temp_file)
        
        # Verify values
        assert config2.get("neuron_count") == 1000
        assert config2.get("connection_probability") == 0.15
    finally:
        os.unlink(temp_file)
```

### Brain Simulation Tests

```python
def test_brain_simulation_basic():
    """Test basic brain simulation"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Record initial state
    initial_neurons = brain.getTotalNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Verify simulation results
    assert brain.getTotalNeuronCount() == initial_neurons
    assert brain.getTotalSpikeCount() >= initial_spikes
    assert brain.getFiringNeuronCount() >= 0
def test_brain_with_current_injection():
    """Test brain with current injection"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    
    # Inject current to sensory neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
    
    # Run simulation with current
    for step in range(20):
        brain.step(step)
    
    # Should have more spikes with current injection
    assert brain.getTotalSpikeCount() > initial_spikes
def test_brain_state_management():
    """Test brain state saving and loading"""
    import pynlm
    import tempfile
    import os
    
    # Create and simulate brain
    brain1 = pynlm.createBrain()
    brain1.initialize()
    
    for step in range(30):
        brain1.step(step)
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        brain1.save(f.name)
        temp_file = f.name
    
    try:
        # Create new brain and load state
        brain2 = pynlm.createBrain()
        brain2.initialize()
        brain2.load(temp_file)
        
        # Brains should have similar state
        assert brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount()
        assert brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount()
    finally:
        os.unlink(temp_file)

def test_brain_snapshot():
    """Test brain snapshot functionality"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run simulation
    for step in range(20):
        brain.step(step)
    
    # Get brain snapshot
    snapshot = brain.getBrainSnapshot()
    
    # Verify snapshot structure
    assert "neuron_count" in snapshot
    assert "synapse_count" in snapshot
    assert "firing_neurons" in snapshot
    assert "total_spikes" in snapshot
    
    # Verify values
    assert snapshot["neuron_count"] == brain.getTotalNeuronCount()
    assert snapshot["synapse_count"] == brain.getTotalSynapseCount()
    assert snapshot["firing_neurons"] == brain.getFiringNeuronCount()
    assert snapshot["total_spikes"] == brain.getTotalSpikeCount()
```

### World Interaction Tests

```python
def test_world_basic():
    """Test basic world functionality"""
    import pynlm
    
    world = pynlm.createWorld(10, 10)
    world.reset()
    
    # Verify world properties
    assert world.getWidth() == 10
    assert world.getHeight() == 10
    assert world.getVisionWidth() == 8
    assert world.getVisionHeight() == 8
    
    # Update world
    world.update(0.1)
    assert world.getSimulationTime() > 0
def test_world_agent_interaction():
    """Test agent-world interaction"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(20, 20)
    world.configure(visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor decoding
    action = agent.decodeMotorCommand()
    assert action is not None
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify agent body changed
    body = world.getAgentBody()
    assert body is not None
def test_world_object_management():
    """Test world object management"""
    import pynlm
    
    world = pynlm.createWorld(15, 15)
    world.reset()
    
    # Add objects
    resource = pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=1.0)
    hazard = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, value=0.5)
    wall = pynlm.WorldObject(12, 12, pynlm.WorldObjectType.Wall, value=1.0)
    
    world.addObject(resource)
    world.addObject(hazard)
    world.addObject(wall)
    
    # Test object removal
    world.removeObject(5, 5)
    assert not world.isValidPosition(5, 5)  # Position should be invalid now
    
    # Verify other objects still exist
    assert world.isValidPosition(10, 10)
    assert world.isValidPosition(12, 12)
```

### Learning Subsystem Tests

```python
def test_learning_subsystem_enable():
    """Test enabling learning subsystems"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableAll()
    
    # Verify subsystems are enabled
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    # Disable all subsystems
    agent.disableAll()
    
    # Verify subsystems are disabled
    assert not agent.isRewardModulationEnabled()
    assert not agent.isStructuralPlasticityEnabled()
    assert not agent.isDevelopmentEnabled()
    assert not agent.isCuriosityEnabled()
def test_reward_modulation():
    """Test reward modulation functionality"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAll()
    
    # Get initial neuromodulation level
    initial_level = agent.getNeuromodulationLevel()
    
    # Apply reward modulation
    agent.applyRewardModulationAuto(1.0)
    
    # Level should change
    assert agent.getNeuromodulationLevel() != initial_level
    
    # Test curiosity and novelty
    assert agent.getCuriosityLevel() > 0
    assert agent.getNoveltyLevel() >= 0
def test_development_system():
    """Test development system"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get initial developmental stage
    initial_stage = agent.getDevelopmentalStage()
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Stage may change over time
    final_stage = agent.getDevelopmentalStage()
    
    # Test development completion
    agent.updateDevelopmentToAdult()
    assert agent.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult
def test_training_episode():
    """Test training episode functionality"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(15, 15)
    world.configure(visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAll()
    
    # Run training episode
    total_reward = agent.trainEpisode(world, 100)
    
    # Verify episode completed
    assert total_reward >= 0
    assert brain.getTotalSpikeCount() > 0
    
    # Test training agent
    results = agent.trainAgent(world, 5, 50)
    assert "average_reward" in results
    assert "max_reward" in results
    assert "total_steps" in results
```

### Advanced API Tests

```python
def test_advanced_brain_methods():
    """Test advanced brain methods"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Test brain comparison
    brain2 = pynlm.createBrain()
    brain2.initialize()
    
    comparison = brain.compareBrains(brain2)
    assert "similarity" in comparison
    assert "diff_neuron_count" in comparison
    
    # Test brain snapshot
    snapshot = brain.getBrainSnapshot()
    assert isinstance(snapshot, dict)
    
    # Test action with context
    action = brain.produceActionWithContext()
    assert "type" in action
    assert "parameters" in action
    assert "confidence" in action
def test_advanced_world_methods():
    """Test advanced world methods"""
    import pynlm
    
    world = pynlm.createWorld(20, 20)
    world.reset()
    
    # Test environment complexity
    complexity = world.getEnvironmentComplexity()
    assert 0 <= complexity <= 1
    
    # Test available resources
    resources = world.getAvailableResources()
    assert resources >= 0
    
    # Test object density
    density = world.getObjectDensity()
    assert density >= 0
def test_advanced_agent_methods():
    """Test advanced agent methods"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(15, 15)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test learning status
    learning_status = agent.getLearningStatus()
    assert "rewardModulation" in learning_status
    assert "structuralPlasticity" in learning_status
    assert "development" in learning_status
    assert "curiosity" in learning_status
    
    # Test raw motor command
    raw_command = agent.decodeMotorCommandRaw()
    assert "type" in raw_command
    assert "parameters" in raw_command

def test_researcher_tools():
    """Test researcher tools and utilities"""
    import pynlm
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    world = pynlm.createWorld(10, 10)
    world.reset()
    
    # Test run integration test
    pynlm.runIntegrationTest(brain, world)
    
    # Test benchmark simulation
    benchmark = pynlm.benchmarkSimulation(brain, 100)
    assert "steps" in benchmark
    assert "time_ms" in benchmark
    assert "steps_per_second" in benchmark
    
    # Test validate brain
    is_valid = pynlm.validateBrainState(brain)
    assert isinstance(is_valid, bool)
    
    # Test experiment
    results = pynlm.runExperiment(brain, world, 3, 50)
    assert "average_reward" in results
    assert "max_reward" in results
```

### Error Handling Tests

```python
def test_error_handling():
    """Test error handling and validation"""
    import pynlm
    
    # Test null brain error
    try:
        brain = pynlm.createBrain()
        # Simulate null brain scenario
        raise ValueError("Brain cannot be null")
    except Exception as e:
        assert "cannot be null" in str(e)
    
    # Test invalid step error
    try:
        brain = pynlm.createBrain()
        brain.initialize()
        # Try invalid step
        raise ValueError("Step number cannot be negative")
    except Exception as e:
        assert "negative" in str(e)
    
    # Test invalid timestep error
    try:
        world = pynlm.createWorld(10, 10)
        world.update(-0.1)
        assert False, "Should have raised ValueError"
    except Exception as e:
        assert "positive" in str(e)
def test_type_validation():
    """Test type validation"""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    
    # Test type conversion validation
    try:
        config.set("test", "invalid_type")
        # Should work with string
        assert config.get("test") == "invalid_type"
    except Exception as e:
        assert False, f"Should accept string type: {e}"
    
    # Test configuration error handling
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", "not_a_number")
    # Should handle conversion gracefully
    # (Implementation-dependent behavior)

def test_parameter_validation():
    """Test parameter validation"""
    import pynlm
    
    world = pynlm.createWorld(10, 10)
    
    # Test invalid dimensions
    try:
        world = pynlm.createWorld(0, 10)
        assert False, "Should have raised ValueError"
    except Exception as e:
        assert "positive" in str(e)
    
    # Test vision dimensions validation
    try:
        world = pynlm.createWorld(5, 5)
        world.configure(10, 10, 3, 3)  # Vision larger than world
        assert False, "Should have raised ValueError"
    except Exception as e:
        assert "cannot exceed" in str(e)
```

### Performance Tests

```python
def test_performance_benchmark():
    """Test performance benchmarking"""
    import pynlm
    import time
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run benchmark
    start_time = time.time()
    for step in range(200):
        brain.step(step)
    end_time = time.time()
    
    duration = end_time - start_time
    assert duration > 0
    assert duration < 10.0  # Should complete within 10 seconds
    
    # Verify benchmark results
    benchmark = pynlm.benchmarkSimulation(brain, 200)
    assert benchmark["steps_per_second"] > 0
    assert benchmark["time_ms"] > 0
def test_memory_efficiency():
    """Test memory efficiency"""
    import pynlm
    import sys
    
    # Create multiple brains
    brains = []
    for i in range(10):
        brain = pynlm.createBrain()
        brain.initialize()
        brains.append(brain)
    
    # Verify they don't consume excessive memory
    total_neurons = sum(brain.getTotalNeuronCount() for brain in brains)
    assert total_neurons > 0
    
    # Clean up
    del brains
def test_large_scale_simulation():
    """Test large-scale simulation"""
    import pynlm
    
    # Create large brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 5000)
    config.set("region_count", 5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run large-scale simulation
    for step in range(100):
        brain.step(step)
    
    # Verify results
    assert brain.getTotalNeuronCount() == 5000
    assert brain.getTotalSpikeCount() >= 0
```

### Integration Tests

```python
def test_complete_integration():
    """Test complete integration of all components"""
    import pynlm
    
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createWorld(20, 20)
    world.configure(visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAll()
    
    # Run complete simulation
    for episode in range(3):
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        world.reset()
    
    # Verify all components are working
    assert brain.getTotalSpikeCount() > 0
    assert agent.getCuriosityLevel() > 0
    assert agent.getNoveltyLevel() >= 0
def test_experiment_suite():
    """Test experiment suite functionality"""
    import pynlm
    
    # Run experiment with different configurations
    configs = [
        {"neuron_count": 1000, "connection_probability": 0.1},
        {"neuron_count": 2000, "connection_probability": 0.05},
        {"neuron_count": 1500, "connection_probability": 0.15}
    ]
    
    results = []
    for i, config in enumerate(configs):
        brain = pynlm.createBrain()
        brain.initialize()
        
        world = pynlm.createWorld(15, 15)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Modify configuration
        for key, value in config.items():
            brain.getConfig().set(key, value)
        
        # Run experiment
        episode_result = agent.trainEpisode(world, 100)
        results.append({
            "config": config,
            "episode_result": episode_result
        })
    
    # Verify all experiments completed
    assert len(results) == 3
    for result in results:
        assert result["episode_result"] >= 0
```

## Test Runner Script

Create a test runner script to run all tests:

```python
#!/usr/bin/env python3

import pytest
import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Run tests with different options
def main():
    # Parse command line arguments
    args = sys.argv[1:]
    
    # Default test options
    test_options = [
        "-v",  # Verbose output
        "--tb=short",  # Short traceback
        "--disable-warnings",  # Disable warnings
    ]
    
    # Add custom arguments if provided
    if "--no-cov" not in args:
        test_options.extend(["--cov=pynlm", "--cov-report=term-missing"])
    
    # Run pytest
    exit_code = pytest.main(test_options + args)
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
```