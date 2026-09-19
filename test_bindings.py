#!/usr/bin/env python3
"""
Python bindings test script for NLM (Neural Learning Machine)
Tests the Python API created via pybind11 bindings.
"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic Python bindings functionality."""
    print("Testing basic NLM Python bindings...")
    
    # Test 1: Create default configuration
    print("1. Creating default configuration...")
    config = pynlm.createDefaultConfig()
    print(f"   Config created: {config}")
    
    # Test 2: Create brain with configuration
    print("2. Creating brain...")
    brain = pynlm.createBrain(config)
    print(f"   Brain created: {brain}")
    
    # Test 3: Initialize brain
    print("3. Initializing brain...")
    brain.initialize()
    print(f"   Brain initialized")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # Test 4: Create world
    print("4. Creating simple world...")
    world = pynlm.createSimpleWorld()
    print(f"   World created")
    
    # Test 5: Configure world
    print("5. Configuring world...")
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    print(f"   World configured and reset")
    
    # Test 6: Create agent brain
    print("6. Creating agent brain interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f"   Agent brain created and initialized")
    
    # Test 7: Enable subsystems
    print("7. Enabling subsystems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print(f"   All subsystems enabled")
    
    # Test 8: Get agent statistics
    print("8. Getting agent statistics...")
    print(f"   Sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Motor output size: {agent.getMotorOutputSize()}")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")
    
    # Test 9: Run simulation steps
    print("9. Running simulation steps...")
    for step in range(100):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Active neurons: {brain.getActiveNeuronCount()}")
            print(f"     Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"     E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
            print(f"     Total spikes: {brain.getTotalSpikeCount()}")
            print(f"     Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Test 10: Configuration operations
    print("10. Testing configuration operations...")
    
    # Test has()
    config.set("test.int", 123)
    assert config.has("test.int"), "Config should have 'test.int'"
    print(f"   Config has 'test.int': {config.has('test.int')}")
    
    # Test get() with different types
    config.set("test.double", 3.14159)
    config.set("test.string", "hello world")
    config.set("test.bool", True)
    
    print(f"   Config has 'test.double': {config.has('test.double')}")
    print(f"   Config has 'test.string': {config.has('test.string')}")
    print(f"   Config has 'test.bool': {config.has('test.bool')}")
    
    # Test getKeys()
    keys = config.getKeys()
    print(f"   All config keys: {keys}")
    
    # Test getOr()
    default_value = config.getOr("missing.key", "default_value")
    print(f"   Missing key getOr: {default_value}")
    
    # Test save and load
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
        config_path = f.name
    
    config.saveToFile(config_path)
    print(f"   Config saved to: {config_path}")
    
    # Create new config and load
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile(config_path)
    print(f"   New config loaded")
    
    # Cleanup
    os.unlink(config_path)
    
    # Test 11: Enum access
    print("11. Testing enum access...")
    print(f"   NeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
    print(f"   NeuronType.Inhibitory: {pynlm.NeuronType.Inhibitory}")
    print(f"   ActionType.MoveForward: {pynlm.ActionType.MoveForward}")
    print(f"   ActionType.TurnLeft: {pynlm.ActionType.TurnLeft}")
    print(f"   MotorCommand.MoveForward: {pynlm.MotorCommand.MoveForward}")
    print(f"   WorldObjectType.Resource: {pynlm.WorldObjectType.Resource}")
    
    # Test 12: World object creation
    print("12. Testing world object creation...")
    resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
    print(f"   Resource object: x={resource.x}, y={resource.y}, type={resource.type}, value={resource.value}")
    
    hazard = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, -20.0)
    print(f"   Hazard object: x={hazard.x}, y={hazard.y}, type={hazard.type}, value={hazard.value}")
    
    wall = pynlm.WorldObject(2.0, 8.0, pynlm.WorldObjectType.Wall)
    print(f"   Wall object: x={wall.x}, y={wall.y}, type={wall.type}")
    
    # Test 13: Sensory input
    print("13. Testing sensory input...")
    vision = pynlm.Vision(width=16, height=16)
    vision_data = [0.1] * 256
    vision.setData(vision_data)
    print(f"   Vision input: width={vision.getWidth()}, height={vision.getHeight()}, type={vision.getType()}")
    
    audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
    audio_data = [0.5] * 1024
    audio.setData(audio_data)
    print(f"   Audio input: sampleRate={audio.getSampleRate()}, numSamples={audio.getNumSamples()}")
    
    internal = pynlm.InternalSignals()
    internal.addSignal(0.3)
    internal.addSignal(0.7)
    print(f"   Internal signals: {[internal.getData()[i] for i in range(internal.getDimensions())]}")
    
    # Test 14: Action creation
    print("14. Testing action creation...")
    move_action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"   Move action: type={move_action.getType()}, name={move_action.getName()}")
    
    turn_action = pynlm.Action(pynlm.ActionType.TurnLeft, [45.0])
    print(f"   Turn action: type={turn_action.getType()}, params={turn_action.getParameters()}, name={turn_action.getName()}")
    
    # Test 15: Brain statistics
    print("15. Testing brain statistics...")
    print(f"   Brain region count: {brain.getRegionCount()}")
    print(f"   Total neuron count: {brain.getTotalNeuronCount()}")
    print(f"   Total synapse count: {brain.getTotalSynapseCount()}")
    print(f"   Active neuron count: {brain.getActiveNeuronCount()}")
    print(f"   Firing neuron count: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"   Total spike count: {brain.getTotalSpikeCount()}")
    
    # Test 16: Save and load brain
    print("16. Testing brain save/load...")
    import tempfile
    import os
    
    brain_path = "test_brain_checkpoint.bin"
    
    brain.save(brain_path)
    print(f"   Brain saved to: {brain_path}")
    
    # Reset and reload
    brain.reset()
    brain.load(brain_path)
    print(f"   Brain loaded from: {brain_path}")
    
    # Cleanup
    if os.path.exists(brain_path):
        os.unlink(brain_path)
    
    print("\n" + "="*50)
    print("All tests passed successfully!")
    print("="*50)
    return True

def test_error_handling():
    """Test error handling in Python bindings."""
    print("\nTesting error handling...")
    
    try:
        config = pynlm.createDefaultConfig()
        
        # Test accessing missing configuration
        result = config.getOr("nonexistent.key", 0)
        print(f"   Missing key returns default: {result}")
        
        # Test has() on missing key
        print(f"   Config has 'missing.key': {config.has('missing.key')}")
        
        print("   Error handling tests passed")
        return True
        
    except Exception as e:
        print(f"   Error in error handling test: {e}")
        return False

if __name__ == "__main__":
    print("NLM Python Bindings Test Script")
    print("="*50)
    
    # Run tests
    success = test_basic_functionality()
    
    if success:
        success = test_error_handling()
    
    if success:
        print("\n✓ All Python bindings tests completed successfully!")
        sys.exit(0)
    else:
        print("\n✗ Some tests failed!")
        sys.exit(1)