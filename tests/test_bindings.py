#!/usr/bin/env python3
"""
Python tests for NLM (Neural Learning Machine) bindings.

This file tests the Python bindings created by pybind11 for the NLM C++ neural
simulation framework. It covers basic functionality, class interfaces, and
integration between components.

To run these tests:
    python -m pytest tests/test_bindings.py -v
    or
    cd tests && python test_bindings.py
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Warning: pynlm module not available. Tests will be skipped.")

import numpy as np

def test_imports():
    """Test that all expected modules and classes can be imported."""
    if not PYNL_AVAILABLE:
        print("Skipping import tests - pynlm not available")
        return
        
    # Test basic imports
    assert hasattr(pynlm, 'createDefaultConfig'), "createDefaultConfig not available"
    assert hasattr(pynlm, 'createBrain'), "createBrain not available"
    assert hasattr(pynlm, 'createSimpleWorld'), "createSimpleWorld not available"
    assert hasattr(pynlm, 'createAgentBrain'), "createAgentBrain not available"
    
    # Test core classes
    assert hasattr(pynlm, 'Config'), "Config class not available"
    assert hasattr(pynlm, 'Brain'), "Brain class not available"
    assert hasattr(pynlm, 'AgentBrain'), "AgentBrain class not available"
    assert hasattr(pynlm, 'SimpleWorld'), "SimpleWorld class not available"
    assert hasattr(pynlm, 'Action'), "Action class not available"
    
    # Test enumerations
    assert hasattr(pynlm, 'NeuronType'), "NeuronType enum not available"
    assert hasattr(pynlm, 'SynapseType'), "SynapseType enum not available"
    assert hasattr(pynlm, 'DevelopmentalStage'), "DevelopmentalStage enum not available"
    assert hasattr(pynlm, 'FiringState'), "FiringState enum not available"
    assert hasattr(pynlm, 'ActionType'), "ActionType enum not available"
    assert hasattr(pynlm, 'MotorCommand'), "MotorCommand enum not available"
    assert hasattr(pynlm, 'WorldObjectType'), "WorldObjectType enum not available"
    
    print("✓ All imports successful")

def test_config():
    """Test Config class functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping config tests - pynlm not available")
        return
        
    config = pynlm.createDefaultConfig()
    assert config is not None, "Config creation failed"
    
    # Test basic methods
    assert hasattr(config, 'has'), "has method not available"
    assert hasattr(config, 'getKeys'), "getKeys method not available"
    assert hasattr(config, 'clear'), "clear method not available"
    assert hasattr(config, 'summary'), "summary method not available"
    
    # Test setting values
    config.set("brain.neuron_count", 1000)
    assert config.has("brain.neuron_count"), "Setting config value failed"
    
    # Test file operations
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        assert os.path.exists(f.name), "Config save failed"
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(f.name)
        assert new_config.has("brain.neuron_count"), "Config load failed"
        
        os.unlink(f.name)
    
    print("✓ Config tests passed")

def test_brain_creation():
    """Test Brain class creation and basic functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping brain tests - pynlm not available")
        return
        
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    assert brain is not None, "Brain creation failed"
    
    # Test initialization
    assert brain.initialize(), "Brain initialization failed"
    
    # Test basic statistics
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Brain should have synapses"
    assert brain.getFiringNeuronCount() >= 0, "Firing neuron count should be valid"
    assert brain.getAverageFiringRate() >= 0, "Average firing rate should be valid"
    assert brain.getTotalSpikeCount() >= 0, "Total spike count should be valid"
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage is not None, "Developmental stage should be accessible"
    
    # Test brain reset
    brain.reset()
    assert brain.getTotalNeuronCount() > 0, "Brain should still have neurons after reset"
    
    print("✓ Brain creation and basic tests passed")

def test_agent_brain():
    """Test AgentBrain class functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping AgentBrain tests - pynlm not available")
        return
        
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None, "AgentBrain creation failed"
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test initialization
    agent.initialize(world)
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    assert percept is not None, "Should get sensory percept"
    
    agent.processSensoryInput(percept)
    
    # Test action decoding
    brain.step(0)
    action = agent.decodeMotorCommand()
    assert action is not None, "Should decode motor command"
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    
    # Test development
    agent.updateDevelopment(0.1)
    
    # Test status getters
    assert agent.getDevelopmentalStage() is not None, "Should get developmental stage"
    assert agent.getNeuromodulationLevel() >= 0, "Neuromodulation level should be valid"
    assert agent.getCuriosityLevel() >= 0, "Curiosity level should be valid"
    assert agent.getNoveltyLevel() >= 0, "Novelty level should be valid"
    assert agent.getPredictionError() >= 0, "Prediction error should be valid"
    
    # Test enable/disable
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
    assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
    assert agent.isDevelopmentEnabled(), "Development should be enabled"
    assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
    
    print("✓ AgentBrain tests passed")

def test_simple_world():
    """Test SimpleWorld class functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping SimpleWorld tests - pynlm not available")
        return
        
    world = pynlm.createSimpleWorld()
    assert world is not None, "World creation failed"
    
    # Test configuration
    world.configure(width=20, height=30, visionWidth=8, visionHeight=8)
    
    # Test basic properties
    assert world.getWidth() == 20, "World width should be 20"
    assert world.getHeight() == 30, "World height should be 30"
    
    # Test reset
    world.reset()
    
    # Test agent start
    world.setAgentStart(10.0, 10.0)
    
    # Test update
    world.update(0.1)
    assert world.getSimulationTime() > 0, "Simulation time should progress"
    
    # Test object manipulation
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0, radius=0.5)
    assert obj.x == 5.0, "Object x position should be 5.0"
    assert obj.y == 5.0, "Object y position should be 5.0"
    assert obj.type == pynlm.WorldObjectType.Resource, "Object type should be Resource"
    assert obj.value == 1.0, "Object value should be 1.0"
    
    world.addObject(obj)
    world.removeObject(5.0, 5.0)
    assert world.isValidPosition(10.0, 10.0), "Agent position should be valid"
    
    # Test energy and decay
    world.setMaxEnergy(100.0)
    assert world.getMaxEnergy() == 100.0, "Max energy should be 100.0"
    
    world.setEnergyDecayRate(0.1)
    assert world.getEnergyDecayRate() == 0.1, "Energy decay rate should be 0.1"
    
    print("✓ SimpleWorld tests passed")

def test_enums():
    """Test enumeration values."""
    if not PYNL_AVAILABLE:
        print("Skipping enum tests - pynlm not available")
        return
        
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None, "Excitatory should be defined"
    assert pynlm.NeuronType.Inhibitory is not None, "Inhibitory should be defined"
    assert pynlm.NeuronType.Sensory is not None, "Sensory should be defined"
    assert pynlm.NeuronType.Motor is not None, "Motor should be defined"
    assert pynlm.NeuronType.Modulatory is not None, "Modulatory should be defined"
    assert pynlm.NeuronType.Internal is not None, "Internal should be defined"
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory is not None, "Excitatory should be defined"
    assert pynlm.SynapseType.Inhibitory is not None, "Inhibitory should be defined"
    assert pynlm.SynapseType.Modulatory is not None, "Modulatory should be defined"
    assert pynlm.SynapseType.Electrical is not None, "Electrical should be defined"
    assert pynlm.SynapseType.GapJunction is not None, "GapJunction should be defined"
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial is not None, "Initial should be defined"
    assert pynlm.DevelopmentalStage.CriticalPeriod is not None, "CriticalPeriod should be defined"
    assert pynlm.DevelopmentalStage.Maturation is not None, "Maturation should be defined"
    assert pynlm.DevelopmentalStage.Adult is not None, "Adult should be defined"
    assert pynlm.DevelopmentalStage.Aging is not None, "Aging should be defined"
    
    # Test FiringState
    assert pynlm.FiringState.Resting is not None, "Resting should be defined"
    assert pynlm.FiringState.Active is not None, "Active should be defined"
    assert pynlm.FiringState.Refractory is not None, "Refractory should be defined"
    assert pynlm.FiringState.Inhibited is not None, "Inhibited should be defined"
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None, "MoveForward should be defined"
    assert pynlm.ActionType.MoveBackward is not None, "MoveBackward should be defined"
    assert pynlm.ActionType.TurnLeft is not None, "TurnLeft should be defined"
    assert pynlm.ActionType.TurnRight is not None, "TurnRight should be defined"
    assert pynlm.ActionType.Interact is not None, "Interact should be defined"
    assert pynlm.ActionType.Eat is not None, "Eat should be defined"
    assert pynlm.ActionType.Drink is not None, "Drink should be defined"
    assert pynlm.ActionType.Rest is not None, "Rest should be defined"
    assert pynlm.ActionType.Wait is not None, "Wait should be defined"
    assert pynlm.ActionType.Custom is not None, "Custom should be defined"
    
    # Test MotorCommand
    assert pynlm.MotorCommand.MoveForward is not None, "MoveForward should be defined"
    assert pynlm.MotorCommand.MoveBackward is not None, "MoveBackward should be defined"
    assert pynlm.MotorCommand.TurnLeft is not None, "TurnLeft should be defined"
    assert pynlm.MotorCommand.TurnRight is not None, "TurnRight should be defined"
    assert pynlm.MotorCommand.LookLeft is not None, "LookLeft should be defined"
    assert pynlm.MotorCommand.LookRight is not None, "LookRight should be defined"
    assert pynlm.MotorCommand.Interact is not None, "Interact should be defined"
    assert pynlm.MotorCommand.Wait is not None, "Wait should be defined"
    
    # Test WorldObjectType
    assert pynlm.WorldObjectType.Empty is not None, "Empty should be defined"
    assert pynlm.WorldObjectType.Resource is not None, "Resource should be defined"
    assert pynlm.WorldObjectType.Hazard is not None, "Hazard should be defined"
    assert pynlm.WorldObjectType.Wall is not None, "Wall should be defined"
    assert pynlm.WorldObjectType.Marker is not None, "Marker should be defined"
    
    print("✓ Enumeration tests passed")

def test_action():
    """Test Action class functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping Action tests - pynlm not available")
        return
        
    # Test creation with type
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action1.getType() == pynlm.ActionType.MoveForward, "Action type should match"
    assert action1.getName() == "MoveForward", "Action name should match"
    
    # Test creation with parameters
    action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
    assert action2.getType() == pynlm.ActionType.Custom, "Action type should match"
    params = action2.getParameters()
    assert len(params) == 3, "Should have 3 parameters"
    assert params[0] == 1.0, "First parameter should be 1.0"
    
    # Test setting parameters
    action2.setParameters([0.5, 0.5])
    params = action2.getParameters()
    assert len(params) == 2, "Should have 2 parameters after update"
    
    # Test cloning
    action3 = action2.clone()
    assert action3.getType() == action2.getType(), "Cloned action should have same type"
    assert action3.getParameters() == action2.getParameters(), "Cloned action should have same parameters"
    
    print("✓ Action tests passed")

def test_sensory_input():
    """Test sensory input classes."""
    if not PYNL_AVAILABLE:
        print("Skipping sensory input tests - pynlm not available")
        return
        
    # Test Vision
    vision = pynlm.Vision(8, 8, 3)
    assert vision.getWidth() == 8, "Vision width should be 8"
    assert vision.getHeight() == 8, "Vision height should be 8"
    assert vision.getChannels() == 3, "Vision channels should be 3"
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    assert audio.getSampleRate() == 44100, "Audio sample rate should be 44100"
    assert audio.getNumSamples() == 1024, "Audio num samples should be 1024"
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(1.0)
    assert internal.getTimestamp() >= 0, "Timestamp should be set"
    
    print("✓ Sensory input tests passed")

def test_simulation():
    """Test complete simulation workflow."""
    if not PYNL_AVAILABLE:
        print("Skipping simulation tests - pynlm not available")
        return
        
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check statistics
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    print("✓ Simulation workflow tests passed")

def test_agent_body():
    """Test AgentBody class functionality."""
    if not PYNL_AVAILABLE:
        print("Skipping AgentBody tests - pynlm not available")
        return
        
    body = pynlm.AgentBody()
    assert body.x == 0.0, "Default x should be 0.0"
    assert body.y == 0.0, "Default y should be 0.0"
    assert body.orientation == 0.0, "Default orientation should be 0.0"
    assert body.velocityX == 0.0, "Default velocityX should be 0.0"
    assert body.velocityY == 0.0, "Default velocityY should be 0.0"
    assert body.energy == 1.0, "Default energy should be 1.0"
    assert body.health == 1.0, "Default health should be 1.0"
    assert body.age == 0.0, "Default age should be 0.0"
    
    # Test reset
    body.x = 10.0
    body.y = 10.0
    body.energy = 0.5
    body.reset()
    assert body.x == 0.0, "Reset x should be 0.0"
    assert body.y == 0.0, "Reset y should be 0.0"
    assert body.energy == 1.0, "Reset energy should be 1.0"
    
    print("✓ AgentBody tests passed")

def test_utility_functions():
    """Test utility functions and module attributes."""
    if not PYNL_AVAILABLE:
        print("Skipping utility tests - pynlm not available")
        return
        
    # Test invalid ID sentinels
    assert pynlm.INVALID_NEURON_ID is not None, "INVALID_NEURON_ID should be defined"
    assert pynlm.INVALID_SYNAPSE_ID is not None, "INVALID_SYNAPSE_ID should be defined"
    assert pynlm.INVALID_REGION_ID is not None, "INVALID_REGION_ID should be defined"
    assert pynlm.INVALID_POPULATION_ID is not None, "INVALID_POPULATION_ID should be defined"
    
    print("✓ Utility tests passed")

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    if not PYNL_AVAILABLE:
        print("\nERROR: pynlm module not available!")
        print("Please build the Python bindings first:")
        print("  mkdir build && cd build && cmake .. && make -j4")
        print("  or pip install .")
        return 1
    
    tests = [
        test_imports,
        test_enums,
        test_config,
        test_brain_creation,
        test_agent_body,
        test_action,
        test_sensory_input,
        test_simple_world,
        test_agent_brain,
        test_simulation,
        test_utility_functions,
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
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return 0 if failed == 0 else 1

if __name__ == "__main__":
    exit(main())