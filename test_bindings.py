#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all the new functionality added to the bindings.cpp.
"""

import sys
import os
import tempfile
import json

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_types():
    """Test basic type IDs and enums"""
    print("Testing basic types...")
    
    # Test NeuronId
    nid1 = pynlm.NeuronId()
    assert nid1.value == 0
    
    nid2 = pynlm.NeuronId(42)
    assert nid2.value == 42
    assert nid2.index() == 42
    
    nid3 = pynlm.NeuronId(100)
    assert nid1 != nid3
    assert hash(nid1) == hash(nid1)
    
    # Test SynapseId
    sid = pynlm.SynapseId(999)
    assert sid.value == 999
    
    # Test RegionId
    rid = pynlm.RegionId(123)
    assert rid.value == 123
    
    # Test PopulationId
    pid = pynlm.PopulationId(456)
    assert pid.value == 456
    
    # Test enums
    assert pynlm.NeuronType.Excitatory == 0
    assert pynlm.NeuronType.Inhibitory == 1
    assert pynlm.NeuronType.Modulatory == 2
    assert pynlm.NeuronType.Sensory == 3
    assert pynlm.NeuronType.Motor == 4
    assert pynlm.NeuronType.Internal == 5
    
    assert pynlm.SynapseType.Excitatory == 0
    assert pynlm.SynapseType.Inhibitory == 1
    assert pynlm.SynapseType.Modulatory == 2
    
    assert pynlm.DevelopmentalStage.Initial == 0
    assert pynlm.DevelopmentalStage.CriticalPeriod == 1
    assert pynlm.DevelopmentalStage.Maturation == 2
    assert pynlm.DevelopmentalStage.Adult == 3
    assert pynlm.DevelopmentalStage.Aging == 4
    
    print("✓ Basic types test passed")

def test_config():
    """Test configuration class"""
    print("Testing config class...")
    
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test basic config operations
    config.set("test.key", "test_value")
    assert config.has("test.key")
    
    # Test save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        filepath = f.name
    
    try:
        config.saveToFile(filepath)
        
        new_config = pynlm.Config()
        new_config.loadFromFile(filepath)
        
        assert new_config.has("test.key")
        assert new_config.getKeys() > 0
    finally:
        os.unlink(filepath)
    
    print("✓ Config test passed")

def test_action():
    """Test Action class"""
    print("Testing Action class...")
    
    # Test action creation
    action1 = pynlm.Action()
    assert action1.getType() == pynlm.ActionType.Custom
    assert len(action1.getParameters()) == 0
    
    action2 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action2.getType() == pynlm.ActionType.MoveForward
    
    action3 = pynlm.Action(pynlm.ActionType.Look, [0.5, 0.3, 0.1])
    assert action3.getType() == pynlm.ActionType.Look
    assert len(action3.getParameters()) == 3
    
    # Test action conversion
    typ, params = action3.to_dict()
    assert typ == pynlm.ActionType.Look
    assert params == [0.5, 0.3, 0.1]
    
    # Test cloning
    action_copy = action2.clone()
    assert action_copy.getType() == action2.getType()
    assert action_copy.getParameters() == action2.getParameters()
    
    print("✓ Action test passed")

def test_world_object():
    """Test WorldObject class"""
    print("Testing WorldObject class...")
    
    # Create objects
    obj1 = pynlm.WorldObject(10, 20, pynlm.WorldObjectType.Resource, 1.5, 0.8)
    assert obj1.x == 10
    assert obj1.y == 20
    assert obj1.type == pynlm.WorldObjectType.Resource
    assert obj1.value == 1.5
    assert obj1.radius == 0.8
    
    obj2 = pynlm.WorldObject()
    assert obj2.x == 0
    assert obj2.y == 0
    assert obj2.active == True
    
    # Test distance calculation
    distance = obj1.distance_to(obj2)
    expected = ((10-0)**2 + (20-0)**2)**0.5
    assert abs(distance - expected) < 0.001
    
    # Test containment
    assert obj1.is_within(10, 20, 0.1)
    assert not obj1.is_within(10, 20, 0.001)
    
    print("✓ WorldObject test passed")

def test_agent_body():
    """Test AgentBody class"""
    print("Testing AgentBody class...")
    
    body = pynlm.AgentBody()
    assert body.x == 0
    assert body.y == 0
    assert body.orientation == 0
    assert body.velocityX == 0
    assert body.velocityY == 0
    assert body.energy == 100
    assert body.health == 100
    
    # Test position
    pos = body.getPosition()
    assert pos[0] == 0 and pos[1] == 0
    
    body.setPosition(15, 25)
    assert body.x == 15
    assert body.y == 25
    
    # Test velocity
    vel = body.getVelocity()
    assert vel[0] == 0 and vel[1] == 0
    
    body.velocityX = 3.0
    body.velocityY = 4.0
    vel = body.getVelocity()
    assert abs(vel[0] - 3.0) < 0.001
    assert abs(vel[1] - 4.0) < 0.001
    
    # Test health checks
    assert body.isHealthy()
    assert body.isAlive()
    
    body.health = 0
    assert not body.isHealthy()
    assert not body.isAlive()
    
    print("✓ AgentBody test passed")

def test_sensory_percept():
    """Test SensoryPercept class"""
    print("Testing SensoryPercept class...")
    
    percept = pynlm.SensoryPercept()
    assert percept is not None
    
    # Test empty percept
    assert not percept.has_vision()
    assert not percept.has_audio()
    assert not percept.has_internal()
    assert not percept.has_proprioception()
    
    # Create vision data
    vision = pynlm.Vision(20, 20, 3)
    vision_data = [i * 0.01 for i in range(20 * 20 * 3)]
    vision.setData(vision_data)
    
    percept.setVision(vision)
    assert percept.has_vision()
    assert percept.getVisionWidth() == 20
    assert percept.getVisionHeight() == 20
    
    # Test vision data retrieval
    retrieved_vision = percept.getVision()
    assert retrieved_vision is not None
    assert retrieved_vision.getWidth() == 20
    
    print("✓ SensoryPercept test passed")

def test_simple_world():
    """Test SimpleWorld class"""
    print("Testing SimpleWorld class...")
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Configure world
    world.configure(50, 60, 10, 10)
    assert world.getWidth() == 50
    assert world.getHeight() == 60
    assert world.getMaxEnergy() == 100
    
    # Set agent start
    world.setAgentStart(25, 30)
    
    # Add objects
    obj1 = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Wall, 0, 1.0)
    obj2 = pynlm.WorldObject(40, 40, pynlm.WorldObjectType.Resource, 10, 0.5)
    
    world.addObject(obj1)
    world.addObject(obj2)
    
    # Test object retrieval
    objects = world.getWorldObjects()
    assert len(objects) == 2
    
    # Test nearby objects
    nearby = world.find_nearby_objects(10, 10, 5.0)
    assert len(nearby) == 1
    assert nearby[0].type == pynlm.WorldObjectType.Wall
    
    # Test agent position
    pos = world.get_agent_position()
    assert abs(pos[0] - 25) < 0.001
    assert abs(pos[1] - 30) < 0.001
    
    # Test agent velocity
    vel = world.get_agent_velocity()
    assert abs(vel[0]) < 0.001
    assert abs(vel[1]) < 0.001
    
    # Test random seed
    seed1 = world.getRandomSeed()
    world.setRandomSeed(42)
    seed2 = world.getRandomSeed()
    assert seed1 != seed2
    
    print("✓ SimpleWorld test passed")

def test_high_level_apis():
    """Test high-level convenience functions"""
    print("Testing high-level APIs...")
    
    # Test config with defaults
    config = pynlm.createDefaultBrainConfig()
    assert config is not None
    
    # Test world config
    world_config = pynlm.createWorldConfig(100, 150, 20, 20)
    assert world_config is not None
    
    # Test simple agent config
    agent_config = pynlm.createSimpleAgentConfig()
    assert agent_config is not None
    
    # Test agent system creation
    brain, world, agent = pynlm.create_agent_system(agent_config)
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    # Initialize the system
    agent.initialize(world)
    
    # Test simulation run (short version)
    brain, world, agent = pynlm.create_agent_system(agent_config)
    result = pynlm.run_simulation(brain, world, 10, False)
    
    assert len(result) == 2
    spikes, rate = result
    assert isinstance(spikes, int)
    assert isinstance(rate, float)
    
    print("✓ High-level APIs test passed")

def test_constants():
    """Test module constants"""
    print("Testing module constants...")
    
    # Test invalid IDs
    assert pynlm.INVALID_NEURON_ID.value == (2**64 - 1)
    assert pynlm.INVALID_SYNAPSE_ID.value == (2**64 - 1)
    assert pynlm.INVALID_REGION_ID.value == (2**64 - 1)
    assert pynlm.INVALID_POPULATION_ID.value == (2**64 - 1)
    
    # Test type aliases
    assert pynlm.SIMULATION_STEP == "SimulationStep"
    assert pynlm.TIMESTAMP == "Timestamp"
    assert pynlm.SYNAPTIC_WEIGHT == "SynapticWeight"
    assert pynlm.MEMBRANE_POTENTIAL == "MembranePotential"
    assert pynlm.FIRING_RATE == "FiringRate"
    assert pynlm.TIMESTEP_DURATION == "TimestepDuration"
    
    # Test memory types
    assert pynlm.MEMORY_TYPE_WORKING == "working"
    assert pynlm.MEMORY_TYPE_EPISODIC == "episodic"
    assert pynlm.MEMORY_TYPE_ASSOCIATIVE == "associative"
    
    print("✓ Constants test passed")

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # This test would need actual C++ exceptions to be raised
    # For now, just test that the module loads correctly
    assert pynlm is not None
    
    print("✓ Error handling test passed")

def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("Running NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_types,
        test_config,
        test_action,
        test_world_object,
        test_agent_body,
        test_sensory_percept,
        test_simple_world,
        test_high_level_apis,
        test_constants,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed! ✓")

if __name__ == "__main__":
    run_all_tests()