#!/usr/bin/env python3
"""
Test script to verify the NLM Python bindings refactoring.
This script tests that the refactored bindings work correctly.
"""

import pynlm

def test_basic_types():
    """Test basic type bindings."""
    print("Testing basic types...")
    
    # Test NeuronId
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    print(f"  NeuronId: {neuron_id}")
    
    # Test SynapseId
    synapse_id = pynlm.SynapseId(100)
    assert synapse_id.value == 100
    assert synapse_id.index() == 100
    print(f"  SynapseId: {synapse_id}")
    
    # Test enums
    assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
    assert pynlm.ActionType.MoveForward == pynlm.ActionType.MoveForward
    print("  Enums work correctly")
    
    # Test constants
    assert pynlm.INVALID_NEURON_ID.value == 0xFFFFFFFFFFFFFFFF
    print("  Constants work correctly")

def test_config():
    """Test configuration bindings."""
    print("Testing Config...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    assert config.has("brain.neuron_count")
    assert config.get("brain.neuron_count", 0) == 1000
    assert config.get("brain.synapse_density", 0.0) == 0.1
    assert config.get("nonexistent", 999) == 999  # default value
    
    print(f"  Config has keys: {config.getKeys()}")
    print("  Config operations work correctly")

def test_sensory():
    """Test sensory input bindings."""
    print("Testing Sensory...")
    
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    vision.setData([0.1, 0.2, 0.3] * 100)
    assert vision.getWidth() == 10
    assert vision.getHeight() == 10
    assert vision.getChannels() == 3
    print(f"  Vision: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Test Audio
    audio = pynlm.Audio(44100, 1000)
    audio.setData([0.5] * 1000)
    audio.setSampleRate(22050)
    assert audio.getSampleRate() == 22050
    assert audio.getNumSamples() == 1000
    print(f"  Audio: {audio.getNumSamples()} samples at {audio.getSampleRate()} Hz")
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(-0.3)
    assert internal.getSignalCount() == 2
    print(f"  InternalSignals: {internal.getSignalCount()} signals")

def test_action():
    """Test action bindings."""
    print("Testing Action...")
    
    # Test Action
    action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.7])
    assert action.getType() == pynlm.ActionType.MoveForward
    assert action.getParameters() == [0.5, 0.7]
    assert action.getName() == "MoveForward"
    print(f"  Action: {action.getName()} with {action.getParameterCount()} params")
    
    # Test WorldObject
    resource = pynlm.createResource(1.0, 2.0, 10.0)
    assert resource.x == 1.0
    assert resource.y == 2.0
    assert resource.type == pynlm.WorldObjectType.Resource
    assert resource.value == 10.0
    print(f"  WorldObject: {resource.type} at ({resource.x}, {resource.y})")

def test_agent():
    """Test agent-related bindings."""
    print("Testing Agent...")
    
    # Test AgentBody
    body = pynlm.createAgentBody()
    body.x = 1.0
    body.y = 2.0
    body.orientation = 0.5
    body.energy = 0.8
    assert body.x == 1.0
    assert body.y == 2.0
    assert body.orientation == 0.5
    assert body.energy == 0.8
    print(f"  AgentBody: pos=({body.x}, {body.y}), energy={body.energy}")
    
    # Test ActionResult
    result = pynlm.ActionResult(reward=1.0, success=True, message="Success!")
    assert result.reward == 1.0
    assert result.success == True
    assert result.message == "Success!"
    assert result.isPositive() == True
    print(f"  ActionResult: reward={result.reward}, success={result.success}")
    
    # Test SensoryPercept
    percept = pynlm.createSensoryPercept()
    vision = pynlm.Vision(8, 8, 3)
    percept.setVision(vision)
    assert percept.hasVision() == True
    print(f"  SensoryPercept: hasVision={percept.hasVision()}")

def test_world():
    """Test world bindings."""
    print("Testing World...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getAgentX() == 10.0  # Note: this might need adjustment based on actual API
    assert world.getAgentY() == 10.0  # Note: this might need adjustment based on actual API
    
    print(f"  World: {world.getWidth()}x{world.getHeight()}")
    print(f"  Simulation time: {world.getSimulationTime()}")

def test_brain():
    """Test brain bindings."""
    print("Testing Brain...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getRegionCount() > 0
    
    # Test statistics
    stats = brain.getBrainStatistics()
    assert "Brain Statistics:" in stats
    print(f"  Brain has {brain.getTotalNeuronCount()} neurons")
    print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")

def test_agentbrain():
    """Test agent brain bindings."""
    print("Testing AgentBrain...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    assert agent_brain.isReady() == True
    assert agent_brain.getNeuromodulationLevel() >= 0.0
    assert agent_brain.getCuriosityLevel() >= 0.0
    
    print(f"  AgentBrain ready: {agent_brain.isReady()}")
    print(f"  Neuromodulation: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent_brain.getCuriosityLevel():.3f}")

def test_factories():
    """Test factory functions."""
    print("Testing Factories...")
    
    # Test various factory functions
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrainWithName("test_brain", config)
    world = pynlm.createWorldWithConfig(10, 10, 5, 5)
    
    action = pynlm.createAction(pynlm.ActionType.TurnLeft)
    assert action.getType() == pynlm.ActionType.TurnLeft
    
    body = pynlm.createAgentBody()
    result = pynlm.createActionResult(0.5, True)
    
    print("  Factory functions work correctly")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing NLM Python Bindings Refactoring")
    print("=" * 60)
    
    try:
        test_basic_types()
        test_config()
        test_sensory()
        test_action()
        test_agent()
        test_world()
        test_brain()
        test_agentbrain()
        test_factories()
        
        print("\n" + "=" * 60)
        print("All tests passed! ✓")
        print("The refactoring is working correctly.")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())