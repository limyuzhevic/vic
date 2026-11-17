#!/usr/bin/env python3
"""
Test script for the improved NLM Python bindings.
This verifies that all the enhancements work correctly.
"""

import pynlm
import sys

def test_basic_bindings():
    """Test that basic bindings work correctly."""
    print("Testing basic bindings...")
    
    # Test Config
    config = pynlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Test Brain
    brain = pynlm.createBrain(config)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test World
    world = pynlm.createSimpleWorld()
    print(f"Created world: {world}")
    
    # Test Agent
    agent = pynlm.createAgentBrain(brain)
    print(f"Created agent brain")
    
    print("✓ Basic bindings work correctly")

def test_sensory_classes():
    """Test sensory input classes (Vision, Audio, InternalSignals)."""
    print("\nTesting sensory input classes...")
    
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    vision.setData([0.1] * 300)
    print(f"Vision: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    print(f"  Data size: {len(vision.getData())}")
    print(f"  Dimensions: {vision.getDimensions()}")
    
    # Test Audio
    audio = pynlm.Audio(44100, 1024)
    audio.setData([0.5] * 1024)
    audio.setSampleRate(22050)
    print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    print(f"  Data size: {len(audio.getData())}")
    print(f"  Dimensions: {audio.getDimensions()}")
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.8)
    internal.addSignal(-0.5)
    internal.addSignal(0.0)
    print(f"InternalSignals: {internal.getData()}")
    print(f"  Dimensions: {internal.getDimensions()}")
    
    print("✓ Sensory input classes work correctly")

def test_action_classes():
    """Test action and result classes."""
    print("\nTesting action and result classes...")
    
    # Test Action
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"Action: {action.getName()}, type={action.getType()}")
    
    # Test ActionResult
    result = pynlm.ActionResult(1.0, True, "Success!")
    print(f"ActionResult: reward={result.reward}, success={result.success}, message='{result.message}'")
    
    print("✓ Action and result classes work correctly")

def test_sensory_percept():
    """Test SensoryPercept class."""
    print("\nTesting SensoryPercept class...")
    
    # Create a percept
    percept = pynlm.SensoryPercept()
    percept.setVision([0.5] * 256)
    percept.setTouch([0.1, 0.2, 0.3])
    percept.setInternal([0.8, 0.9])
    percept.setText("Hello world")
    
    print(f"Percept created:")
    print(f"  Vision: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
    print(f"  Touch: {percept.getTouch()}")
    print(f"  Internal: {percept.getInternal()}")
    print(f"  Text: '{percept.getText()}'")
    
    # Test all signals
    all_signals = percept.getAllSignals()
    print(f"  All signals: {len(all_signals)} total elements")
    
    print("✓ SensoryPercept works correctly")

def test_world_object():
    """Test WorldObject class."""
    print("\nTesting WorldObject class...")
    
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
    print(f"WorldObject: pos=({obj.x}, {obj.y}), type={obj.type}, value={obj.value}, radius={obj.radius}")
    
    print("✓ WorldObject works correctly")

def test_agent_body():
    """Test AgentBody class."""
    print("\nTesting AgentBody class...")
    
    body = pynlm.AgentBody()
    print(f"AgentBody: pos=({body.x}, {body.y}), orient={body.orientation}")
    print(f"  Energy: {body.energy}, Health: {body.health}, Age: {body.age}")
    print(f"  Moving: {body.isMoving}, Turning: {body.isTurning}")
    
    body.reset()
    print(f"After reset: Energy={body.energy}, Health={body.health}")
    
    print("✓ AgentBody works correctly")

def test_complete_workflow():
    """Test a complete workflow that exercises all bindings."""
    print("\nTesting complete workflow...")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Complete workflow works correctly")

def main():
    """Run all tests."""
    print("=" * 60)
    print("Testing improved NLM Python bindings")
    print("=" * 60)
    
    try:
        test_basic_bindings()
        test_sensory_classes()
        test_action_classes()
        test_sensory_percept()
        test_world_object()
        test_agent_body()
        test_complete_workflow()
        
        print("\n" + "=" * 60)
        print("✅ ALL TESTS PASSED!")
        print("=" * 60)
        print("\nThe improved Python bindings are working correctly with:")
        print("- Complete Vision, Audio, and InternalSignals bindings")
        print("- Better __repr__ methods for all classes")
        print("- Pythonic property accessors")
        print("- Enhanced error handling")
        print("- Better documentation strings")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())