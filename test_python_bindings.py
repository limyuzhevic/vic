#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings functionality.
This script tests the documented Python API.
"""

import pynlm
import numpy as np
import tempfile
import os

def test_python_bindings():
    """Test all documented Python bindings functionality."""
    print("Testing NLM Python Bindings...")
    
    # Test 1: createDefaultConfig()
    print("\n1. Testing createDefaultConfig()...")
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("   ✓ Default config created")
    
    # Test 2: createBrain()
    print("\n2. Testing createBrain()...")
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("   ✓ Brain created")
    
    # Test 3: createSimpleWorld()
    print("\n3. Testing createSimpleWorld()...")
    world = pynlm.createSimpleWorld()
    assert world is not None
    print("   ✓ World created")
    
    # Test 4: configure world
    print("\n4. Testing world configuration...")
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    assert world.getWidth() == 20.0
    assert world.getHeight() == 20.0
    assert world.getSensoryPercept().getVisionWidth() == 8
    assert world.getSensoryPercept().getVisionHeight() == 8
    print("   ✓ World configured correctly")
    
    # Test 5: createAgentBrain()
    print("\n5. Testing createAgentBrain()...")
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    print("   ✓ Agent brain created")
    
    # Test 6: initialize brain
    print("\n6. Testing brain initialization...")
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    print("   ✓ Brain initialized with", brain.getTotalNeuronCount(), "neurons")
    
    # Test 7: agent initialization
    print("\n7. Testing agent initialization...")
    world.reset()
    agent.initialize(world)
    print("   ✓ Agent initialized with world")
    
    # Test 8: enable subsystems
    print("\n8. Testing subsystem controls...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    print("   ✓ All subsystems enabled")
    
    # Test 9: simulation step
    print("\n9. Testing simulation step...")
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    motor_cmd = agent.decodeMotorCommand()
    print(f"   ✓ Brain step completed, motor command: {motor_cmd}")
    
    # Test 10: statistics
    print("\n10. Testing brain statistics...")
    total_spikes = brain.getTotalSpikeCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing = brain.getAverageFiringRate()
    print(f"   ✓ Stats - Spikes: {total_spikes}, Firing: {firing_neurons}, Rate: {avg_firing:.2f} Hz")
    
    # Test 11: neuromodulation
    print("\n11. Testing neuromodulation...")
    agent.applyRewardModulation(0.5, 0.3)
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    dopamine = agent.getNeuromodulationLevel()
    print(f"   ✓ Neuromodulation - Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}, Dopamine: {dopamine:.3f}")
    
    # Test 12: development
    print("\n12. Testing development...")
    stage = agent.getDevelopmentalStage()
    print(f"   ✓ Development stage: {stage}")
    
    # Test 13: save/load functionality
    print("\n13. Testing save/load...")
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        save_file = f.name
    
    try:
        brain.save(save_file)
        print("   ✓ Brain saved")
        
        # Create new brain and load
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        new_brain.load(save_file)
        print("   ✓ Brain loaded successfully")
    finally:
        if os.path.exists(save_file):
            os.unlink(save_file)
    
    # Test 14: simple world interaction
    print("\n14. Testing world interaction...")
    world.setAgentStart(10.0, 10.0)
    world.update(0.1)
    percept = world.getSensoryPercept()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    print(f"   ✓ World updated: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
    
    # Test 15: world objects
    print("\n15. Testing world objects...")
    # Add a resource object
    obj = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(obj)
    assert world.getObjectAt(15.0, 15.0) is not None
    world.removeObject(15.0, 15.0)
    assert world.getObjectAt(15.0, 15.0) is None
    print("   ✓ World object management working")
    
    print("\n" + "="*60)
    print("ALL PYTHON BINDINGS TESTS PASSED! ✓")
    print("The NLM Python API is fully functional and documented.")
    print("="*60)

if __name__ == "__main__":
    test_python_bindings()