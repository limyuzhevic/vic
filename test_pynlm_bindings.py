#!/usr/bin/env python3

"""
Python binding tests for NLM (pynlm)

This script tests the Python API bindings for NLM.
It verifies that the pynlm module can be imported and
basic functionality works correctly.
"""

import os
import sys

# Add this script's directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import sys
import os

def test_basic_import():
    """Test that pynlm can be imported and basic types exist"""
    print("Testing basic import...")
    
    # Check that pynlm module exists
    assert hasattr(pynlm, '__version__'), "pynlm version not available"
    
    # Check key classes exist
    assert hasattr(pynlm, 'createBrain'), "createBrain function not available"
    assert hasattr(pynlm, 'createDefaultConfig'), "createDefaultConfig function not available"
    assert hasattr(pynlm, 'createSimpleWorld'), "createSimpleWorld function not available"
    assert hasattr(pynlm, 'createAgentBrain'), "createAgentBrain function not available"
    
    # Check key enumerations
    assert hasattr(pynlm, 'NeuronType'), "NeuronType enumeration not available"
    assert hasattr(pynlm, 'ActionType'), "ActionType enumeration not available"
    assert hasattr(pynlm, 'WorldObjectType'), "WorldObjectType enumeration not available"
    
    print("✓ Basic import test passed")

def test_config_creation():
    """Test that configuration can be created and modified"""
    print("Testing config creation...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic properties
    assert hasattr(config, 'getKeys'), "Config missing getKeys method"
    assert hasattr(config, 'has'), "Config missing has method"
    assert hasattr(config, 'set'), "Config missing set method"
    assert hasattr(config, 'get'), "Config missing get method"
    assert hasattr(config, 'getOr'), "Config missing getOr method"
    
    # Test setting values
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.1)
    
    # Test querying values
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    
    keys = config.getKeys()
    assert "brain.neuron_count" in keys
    assert "brain.synapse_density" in keys
    
    print("✓ Config creation test passed")

def test_brain_creation():
    """Test that brains can be created and initialized"""
    print("Testing brain creation...")
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    
    # Test basic properties
    assert hasattr(brain, 'initialize'), "Brain missing initialize method"
    assert hasattr(brain, 'step'), "Brain missing step method"
    assert hasattr(brain, 'reset'), "Brain missing reset method"
    assert hasattr(brain, 'save'), "Brain missing save method"
    assert hasattr(brain, 'load'), "Brain missing load method"
    
    # Test statistics methods
    assert hasattr(brain, 'getTotalNeuronCount'), "Brain missing getTotalNeuronCount method"
    assert hasattr(brain, 'getTotalSynapseCount'), "Brain missing getTotalSynapseCount method"
    assert hasattr(brain, 'getFiringNeuronCount'), "Brain missing getFiringNeuronCount method"
    assert hasattr(brain, 'getAverageFiringRate'), "Brain missing getAverageFiringRate method"
    
    # Initialize brain
    success = brain.initialize()
    assert success, "Brain initialization failed"
    
    # Test basic statistics after initialization
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    print("✓ Brain creation test passed")

def test_world_creation():
    """Test that worlds can be created and configured"""
    print("Testing world creation...")
    
    world = pynlm.createSimpleWorld()
    
    # Test configuration methods
    assert hasattr(world, 'configure'), "World missing configure method"
    assert hasattr(world, 'reset'), "World missing reset method"
    assert hasattr(world, 'update'), "World missing update method"
    assert hasattr(world, 'getSensoryPercept'), "World missing getSensoryPercept method"
    
    # Configure world
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    
    # Test world properties
    assert hasattr(world, 'getWidth'), "World missing getWidth method"
    assert hasattr(world, 'getHeight'), "World missing getHeight method"
    assert hasattr(world, 'getSimulationTime'), "World missing getSimulationTime method"
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    
    print("✓ World creation test passed")

def test_agent_creation():
    """Test that agent brains can be created and used"""
    print("Testing agent creation...")
    
    # Create brain first
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    
    # Test basic properties
    assert hasattr(agent, 'initialize'), "Agent missing initialize method"
    assert hasattr(agent, 'processSensoryInput'), "Agent missing processSensoryInput method"
    assert hasattr(agent, 'decodeMotorCommand'), "Agent missing decodeMotorCommand method"
    
    # Test subsystem methods
    assert hasattr(agent, 'enableRewardModulation'), "Agent missing enableRewardModulation method"
    assert hasattr(agent, 'enableStructuralPlasticity'), "Agent missing enableStructuralPlasticity method"
    assert hasattr(agent, 'enableDevelopment'), "Agent missing enableDevelopment method"
    assert hasattr(agent, 'enableCuriosity'), "Agent missing enableCuriosity method"
    
    # Test statistics methods
    assert hasattr(agent, 'getCuriosityLevel'), "Agent missing getCuriosityLevel method"
    assert hasattr(agent, 'getNoveltyLevel'), "Agent missing getNoveltyLevel method"
    assert hasattr(agent, 'getPredictionError'), "Agent missing getPredictionError method"
    assert hasattr(agent, 'getNeuromodulationLevel'), "Agent missing getNeuromodulationLevel method"
    assert hasattr(agent, 'getDevelopmentalStage'), "Agent missing getDevelopmentalStage method"
    
    print("✓ Agent creation test passed")

def test_enumerations():
    """Test that enumerations are properly exposed"""
    print("Testing enumerations...")
    
    # Test NeuronType enumeration
    neuron_types = [
        pynlm.NeuronType.Excitatory,
        pynlm.NeuronType.Inhibitory,
        pynlm.NeuronType.Sensory,
        pynlm.NeuronType.Motor,
        pynlm.NeuronType.Modulatory,
        pynlm.NeuronType.Internal
    ]
    
    for neuron_type in neuron_types:
        assert neuron_type is not None, f"Invalid NeuronType: {neuron_type}"
    
    # Test DevelopmentalStage enumeration
    developmental_stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    for stage in developmental_stages:
        assert stage is not None, f"Invalid DevelopmentalStage: {stage}"
    
    # Test WorldObjectType enumeration
    object_types = [
        pynlm.WorldObjectType.Empty,
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall,
        pynlm.WorldObjectType.Marker
    ]
    
    for obj_type in object_types:
        assert obj_type is not None, f"Invalid WorldObjectType: {obj_type}"
    
    print("✓ Enumerations test passed")

def test_complete_simulation():
    """Test a complete simulation with brain, world, and agent"""
    print("Testing complete simulation...")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Verify metrics
        assert brain.getFiringNeuronCount() >= 0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getDevelopmentalStage() is not None
    
    print("✓ Complete simulation test passed")

def test_checkpoint_functionality():
    """Test checkpoint save/load functionality"""
    print("Testing checkpoint functionality...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to generate state
    for i in range(20):
        brain.step(i)
    
    # Save brain state
    checkpoint_file = "test_checkpoint.bin"
    brain.save(checkpoint_file)
    
    # Create new brain and load state
    brain2 = pynlm.createBrain(config)
    brain2.load(checkpoint_file)
    
    # Both brains should have same neuron count
    assert brain.getTotalNeuronCount() == brain2.getTotalNeuronCount()
    assert brain.getTotalSynapseCount() == brain2.getTotalSynapseCount()
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
    
    print("✓ Checkpoint functionality test passed")

def run_all_tests():
    """Run all Python binding tests"""
    print("=" * 60)
    print("Running NLM Python Binding Tests")
    print("=" * 60)
    
    tests = [
        test_basic_import,
        test_config_creation,
        test_brain_creation,
        test_world_creation,
        test_agent_creation,
        test_enumerations,
        test_complete_simulation,
        test_checkpoint_functionality,
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
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All Python binding tests passed!")

if __name__ == "__main__":
    run_all_tests()