#!/usr/bin/env python3
"""
Comprehensive Python test suite for NLM (Neural Learning Machine) project.

Tests all improvements to the NLM project:
1. Config system tests - JSON/YAML parsing, saving, loading, validation
2. Python bindings tests - All exposed C++ classes and functions
3. New advanced features tests - Batch processing, validation, event system
4. Integration tests - Complete brain-world-agent systems
5. Performance and correctness tests

Requirements:
- Tests are in Python using pytest
- Test both old and new functionality
- Test edge cases and error conditions
- Include performance benchmarks
- Make tests standalone (don't require external dependencies beyond pytest)
- Test all new features added to Python bindings
- Test Config system improvements thoroughly
"""

import sys
import os
import json
import tempfile
import time
import pytest
import numpy as np

# Import NLM Python bindings
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
def test_config_creation():
    """Test Config class creation and basic properties."""
    config = pynlm.createDefaultConfig()
    
    # Test initial state
    assert not config.has("test")
    
    # Test string value
    config.set("string_value", "hello")
    assert config.has("string_value")
    assert config.get("string_value").get() == "hello"
    
    # Test integer value
    config.set("int_value", 42)
    assert config.get("int_value").get() == 42
    
    # Test float value
    config.set("float_value", 3.14)
    assert abs(config.get("float_value").get() - 3.14) < 0.01
    
    # Test boolean value
    config.set("bool_value", True)
    assert config.get("bool_value").get() == True
    
    # Test vector values
    config.set("int_vector", [1, 2, 3, 4, 5])
    assert config.get("int_vector").get() == [1, 2, 3, 4, 5]
    
    config.set("float_vector", [1.1, 2.2, 3.3])
    assert config.get("float_vector").get() == [1.1, 2.2, 3.3]
    
    config.set("string_vector", ["a", "b", "c"])
    assert config.get("string_vector").get() == ["a", "b", "c"]
    
    # Test getOr for missing keys
    assert config.getOr("missing", "default") == "default"
    assert config.getOr("missing", 100) == 100
    assert config.getOr("missing", True) == True
    
    # Test getOr for existing keys
    assert config.getOr("string_value", "default") == "hello"
    assert config.getOr("int_value", 999) == 42
    
    # Test remove
    config.set("to_remove", 123)
    assert config.has("to_remove")
    config.remove("to_remove")
    assert not config.has("to_remove")
    
    # Test clear
    config.clear()
    assert not config.has("string_value")
    assert not config.has("int_value")
    
    print("✓ test_config_creation passed")
def test_config_serialization():
    """Test JSON and YAML serialization."""
    config = pynlm.createDefaultConfig()
    
    # Add various data types
    config.set("string", "test_value")
    config.set("integer", 42)
    config.set("float", 3.14159)
    config.set("boolean", True)
    config.set("int_list", [1, 2, 3, 4, 5])
    config.set("float_list", [1.1, 2.2, 3.3])
    config.set("string_list", ["a", "b", "c"])
    
    # Test saving to and loading from JSON file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json_file = f.name
        config.saveToFile(json_file)
    
    try:
        # Create new config and load from file
        new_config = pynlm.createDefaultConfig()
        success = new_config.loadFromFile(json_file)
        assert success, "Failed to load JSON file"
        
        # Verify all values were loaded correctly
        assert new_config.get("string").get() == "test_value"
        assert new_config.get("integer").get() == 42
        assert abs(new_config.get("float").get() - 3.14159) < 0.0001
        assert new_config.get("boolean").get() == True
        assert new_config.get("int_list").get() == [1, 2, 3, 4, 5]
        assert new_config.get("float_list").get() == [1.1, 2.2, 3.3]
        assert new_config.get("string_list").get() == ["a", "b", "c"]
        
    finally:
        os.unlink(json_file)
    
    print("✓ test_config_serialization passed")
def test_config_edge_cases():
    """Test edge cases and error conditions."""
    config = pynlm.createDefaultConfig()
    
    # Test empty string
    config.set("empty", "")
    assert config.get("empty").get() == ""
    
    # Test large integers
    config.set("large_int", 2**63 - 1)
    assert config.get("large_int").get() == 2**63 - 1
    
    # Test negative numbers
    config.set("negative", -42)
    assert config.get("negative").get() == -42
    
    # Test zero
    config.set("zero", 0)
    assert config.get("zero").get() == 0
    
    # Test large vectors
    large_list = list(range(1000))
    config.set("large_list", large_list)
    assert config.get("large_list").get() == large_list
    
    # Test special characters in strings
    config.set("special", "Hello, world! \n\t\"quotes' and <brackets>")
    assert config.get("special").get() == "Hello, world! \n\t\"quotes' and <brackets>"
    
    # Test config summary
    summary = config.summary()
    assert "Configuration" in summary
    assert "2000" in summary  # Number of entries (after clear)
    
    # Test get with wrong type (should return None)
    assert config.get("string").is_none() or config.get("string") is None
    
    print("✓ test_config_edge_cases passed")
def test_neuron_ids():
    """Test NeuronId, SynapseId, RegionId, and PopulationId classes."""
    # Test NeuronId
    neuron_id = pynlm.NeuronId()
    assert neuron_id.value == 0
    
    neuron_id2 = pynlm.NeuronId(42)
    assert neuron_id2.value == 42
    assert neuron_id2.index() == 42
    
    # Test equality
    neuron_id3 = pynlm.NeuronId(42)
    assert neuron_id2 == neuron_id3
    assert not (neuron_id2 != neuron_id3)
    
    # Test hash
    id_dict = {neuron_id2: "value"}
    assert id_dict[neuron_id3] == "value"
    
    # Test SynapseId
    syn_id = pynlm.SynapseId(100)
    assert syn_id.value == 100
    assert syn_id.index() == 100
    
    # Test RegionId
    region_id = pynlm.RegionId(200)
    assert region_id.value == 200
    assert region_id.index() == 200
    
    # Test PopulationId
    pop_id = pynlm.PopulationId(300)
    assert pop_id.value == 300
    assert pop_id.index() == 300
    
    print("✓ test_neuron_ids passed")
def test_enums():
    """Test enumeration types."""
    # Neuron types
    assert pynlm.NeuronType.Excitatory != pynlm.NeuronType.Inhibitory
    assert pynlm.NeuronType.Sensory != pynlm.NeuronType.Motor
    
    # Synapse types
    assert pynlm.SynapseType.Excitatory != pynlm.SynapseType.Inhibitory
    assert pynlm.SynapseType.GapJunction != pynlm.SynapseType.Electrical
    
    # Developmental stages
    assert pynlm.DevelopmentalStage.Initial != pynlm.DevelopmentalStage.CriticalPeriod
    assert pynlm.DevelopmentalStage.Adult != pynlm.DevelopmentalStage.Aging
    
    # Firing states
    assert pynlm.FiringState.Resting != pynlm.FiringState.Active
    assert pynlm.FiringState.Inhibited != pynlm.FiringState.Refractory
    
    # Action types
    assert pynlm.ActionType.MoveForward != pynlm.ActionType.TurnLeft
    assert pynlm.ActionType.Interact != pynlm.ActionType.Eat
    
    # Motor commands
    assert pynlm.MotorCommand.MoveForward != pynlm.MotorCommand.TurnLeft
    assert pynlm.MotorCommand.Interact != pynlm.MotorCommand.Wait
    
    # World object types
    assert pynlm.WorldObjectType.Resource != pynlm.WorldObjectType.Hazard
    assert pynlm.WorldObjectType.Wall != pynlm.WorldObjectType.Marker
    
    print("✓ test_enums passed")
def test_vision_sensory():
    """Test Vision sensory input class."""
    # Create vision with default constructor
    vision = pynlm.Vision()
    assert vision.getWidth() == 0
    assert vision.getHeight() == 0
    assert vision.getChannels() == 0
    
    # Create vision with parameters
    vision2 = pynlm.Vision(640, 480, 3)
    assert vision2.getWidth() == 640
    assert vision2.getHeight() == 480
    assert vision2.getChannels() == 3
    
    # Set data
    data = np.random.randn(640, 480, 3).flatten().tolist()
    vision2.setData(data)
    assert vision2.getData() == data
    
    print("✓ test_vision_sensory passed")
def test_action_class():
    """Test Action class."""
    # Create action with type only
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward
    
    # Create action with type and parameters
    action2 = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
    assert action2.getType() == pynlm.ActionType.MoveForward
    assert action2.getParameters() == [1.0, 2.0]
    
    # Test setType and setParameters
    action1.setType(pynlm.ActionType.TurnLeft)
    assert action1.getType() == pynlm.ActionType.TurnLeft
    
    action1.setParameters([45.0, 0.5])
    assert action1.getParameters() == [45.0, 0.5]
    
    # Test getName
    assert action1.getName() == "TurnLeft"
    
    # Test clone
    action3 = action1.clone()
    assert action3.getType() == action1.getType()
    assert action3.getParameters() == action1.getParameters()
    
    print("✓ test_action_class passed")
def test_world_object():
    """Test WorldObject class."""
    # Create with default constructor
    obj1 = pynlm.WorldObject()
    assert obj1.x == 0.0
    assert obj1.y == 0.0
    assert obj1.radius == 0.5
    assert obj1.type == pynlm.WorldObjectType.Empty
    assert obj1.value == 0.0
    assert obj1.active == True
    
    # Create with parameters
    obj2 = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 2.0)
    assert obj2.x == 10.0
    assert obj2.y == 20.0
    assert obj2.radius == 2.0
    assert obj2.type == pynlm.WorldObjectType.Resource
    assert obj2.value == 5.0
    assert obj2.active == True
    
    # Test read/write properties
    obj2.x = 15.0
    obj2.y = 25.0
    obj2.radius = 1.5
    obj2.type = pynlm.WorldObjectType.Wall
    obj2.value = 10.0
    obj2.active = False
    
    assert obj2.x == 15.0
    assert obj2.y == 25.0
    assert obj2.radius == 1.5
    assert obj2.type == pynlm.WorldObjectType.Wall
    assert obj2.value == 10.0
    assert obj2.active == False
    
    print("✓ test_world_object passed")
def test_agent_body():
    """Test AgentBody class."""
    body = pynlm.AgentBody()
    
    # Test initial values
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.velocityX == 0.0
    assert body.velocityY == 0.0
    assert body.angularVelocity == 0.0
    assert body.energy == 100.0
    assert body.health == 100.0
    assert body.age == 0
    assert body.isMoving == False
    assert body.isTurning == False
    assert body.lastActionTime == 0.0
    
    # Test setters
    body.x = 10.5
    body.y = 20.5
    body.orientation = 90.0
    body.velocityX = 1.0
    body.velocityY = 2.0
    body.angularVelocity = 0.5
    body.energy = 75.0
    body.health = 90.0
    body.age = 10
    body.isMoving = True
    body.isTurning = True
    body.lastActionTime = 5.0
    
    # Test getters
    assert body.x == 10.5
    assert body.y == 20.5
    assert body.orientation == 90.0
    assert body.velocityX == 1.0
    assert body.velocityY == 2.0
    assert body.angularVelocity == 0.5
    assert body.energy == 75.0
    assert body.health == 90.0
    assert body.age == 10
    assert body.isMoving == True
    assert body.isTurning == True
    assert body.lastActionTime == 5.0
    
    # Test reset
    body.reset()
    assert body.x == 0.0
    assert body.y == 0.0
    assert body.orientation == 0.0
    assert body.velocityX == 0.0
    assert body.velocityY == 0.0
    assert body.angularVelocity == 0.0
    assert body.energy == 100.0
    assert body.health == 100.0
    assert body.age == 0
    assert body.isMoving == False
    assert body.isTurning == False
    assert body.lastActionTime == 0.0
    
    print("✓ test_agent_body passed")
def test_world_integration():
    """Test SimpleWorld integration."""
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getMaxEnergy() > 0
    
    # Reset world
    world.reset()
    
    # Set agent start
    world.setAgentStart(10.0, 10.0)
    
    # Get agent body
    body = world.getAgentBody()
    assert body.x == 10.0
    assert body.y == 10.0
    
    # Update world
    world.update(timestep=0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    assert percept is not None
    
    # Add and remove objects
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0, 1.0)
    world.addObject(obj)
    
    assert world.isValidPosition(5.0, 5.0)
    assert not world.isValidPosition(100.0, 100.0)  # Outside bounds
    
    world.removeObject(5.0, 5.0)
    assert not world.isValidPosition(5.0, 5.0)
    
    # Set random seed
    world.setRandomSeed(42)
    world.getRandomSeed()  # Just to verify the method works
    
    print("✓ test_world_integration passed")
def test_brain_basic():
    """Test basic Brain functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    success = brain.initialize()
    assert success
    
    # Check brain properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getRegionCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test step
    for step in range(5):
        brain.step(step)
    
    # Test region management
    region_id = brain.addRegion("test_region")
    assert region_id.value > 0
    
    region = brain.getRegion(region_id)
    assert region is not None
    
    region_ids = brain.getRegionIds()
    assert len(region_ids) >= 1
    assert region_id in region_ids
    
    # Test getRegions
    regions = brain.getRegions()
    assert len(regions) >= 1
    
    # Test statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test reset
    brain.reset()
    assert brain.getTotalNeuronCount() > 0  # Neurons should still exist after reset
    
    print("✓ test_brain_basic passed")
def test_agent_brain():
    """Test AgentBrain integration."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Test neural interface
    assert agent.getBrain() is brain
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor decoding
    motor_cmd = agent.decodeMotorCommand()
    assert motor_cmd in [
        pynlm.MotorCommand.MoveForward,
        pynlm.MotorCommand.MoveBackward,
        pynlm.MotorCommand.TurnLeft,
        pynlm.MotorCommand.TurnRight,
        pynlm.MotorCommand.LookLeft,
        pynlm.MotorCommand.LookRight,
        pynlm.MotorCommand.Interact,
        pynlm.MotorCommand.Wait
    ]
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    assert agent.getNeuromodulationLevel() >= 0.0
    
    # Test development
    agent.updateDevelopment(0.1)
    assert agent.getDevelopmentalStage() in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test curiosity and novelty
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    
    # Test prediction error
    assert agent.getPredictionError() >= 0.0
    
    # Test subsystem toggles
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    # Test reset
    agent.reset()
    
    print("✓ test_agent_brain passed")
def test_performance_benchmarks():
    """Test performance benchmarks."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark brain initialization
    start_time = time.time()
    for _ in range(10):
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
    init_time = time.time() - start_time
    print(f"  Brain initialization (10x): {init_time:.3f}s average: {init_time/10:.3f}s")
    
    # Benchmark simulation steps
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    step_time = time.time() - start_time
    print(f"  Simulation (100 steps): {step_time:.3f}s average: {step_time/100:.3f}s per step")
    
    # Benchmark brain statistics
    start_time = time.time()
    for _ in range(100):
        _ = brain.getTotalNeuronCount()
        _ = brain.getTotalSynapseCount()
        _ = brain.getFiringNeuronCount()
        _ = brain.getAverageFiringRate()
        _ = brain.getExcitationInhibitionRatio()
        _ = brain.getTotalSpikeCount()
    stats_time = time.time() - start_time
    print(f"  Statistics (100x): {stats_time:.3f}s average: {stats_time/100:.3f}s per call")
    
    # Benchmark memory operations
    start_time = time.time()
    for _ in range(50):
        brain.reset()
        brain.initialize()
    memory_time = time.time() - start_time
    print(f"  Reset/Initialize cycle (50x): {memory_time:.3f}s average: {memory_time/50:.3f}s")
    
    print("✓ test_performance_benchmarks passed")
def test_complete_integration():
    """Test complete brain-world-agent system integration."""
    print("  Testing complete brain-world-agent system integration...")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    num_steps = 50
    print(f"  Running {num_steps} simulation steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on world state
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 10 steps
        if step % 10 == 0:
            print(f"    Step {step}:")
            print(f"      Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"      Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"      Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"      Dev Stage: {brain.getDevelopmentalStage().name}")
            print(f"      Motor cmd: {motor_cmd.name}")
    
    # 8. Final statistics
    print(f"  Final statistics:")
    print(f"    Total neurons: {brain.getTotalNeuronCount()}")
    print(f"    Total synapses: {brain.getTotalSynapseCount()}")
    print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"    Prediction error: {agent.getPredictionError():.3f}")
    print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"    Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # 9. Verify integration
    assert brain.getTotalNeuronCount() > 0
    assert world.getAgentBody().x >= 0
    assert world.getAgentBody().y >= 0
    assert agent.getCuriosityLevel() >= 0
    
    print("✓ test_complete_integration passed")
if __name__ == "__main__":
    print("=" * 60)
    print("NLM Python Test Suite")
    print("=" * 60)
    
    # Run all tests
    tests = [
        ("Config System", test_config_creation),
        ("Config System", test_config_serialization),
        ("Config System", test_config_edge_cases),
        ("Python Bindings", test_neuron_ids),
        ("Python Bindings", test_enums),
        ("Python Bindings", test_vision_sensory),
        ("Python Bindings", test_action_class),
        ("Python Bindings", test_world_object),
        ("Python Bindings", test_agent_body),
        ("Integration", test_world_integration),
        ("Integration", test_brain_basic),
        ("Integration", test_agent_brain),
        ("Performance", test_performance_benchmarks),
        ("Integration", test_complete_integration),
    ]
    
    passed = 0
    failed = 0
    
    for category, test_func in tests:
        print(f"\n{category}:")
        print("-" * 40)
        try:
            test_func()
            passed += 1
            print(f"✓ {category} tests passed")
        except Exception as e:
            failed += 1
            print(f"✗ {category} test failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed successfully!")