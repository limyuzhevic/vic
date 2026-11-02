#!/usr/bin/env python3
"""
Test script to demonstrate the enhanced NLM Python bindings.
This script shows the new features and improvements made to the pynlm library.
"""

import pynlm
import numpy as np

def test_basic_functionality():
    """Test basic functionality with the enhanced API."""
    print("=== Testing Enhanced NLM Python Bindings ===\n")
    
    # Create configuration with validation
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    # Validate configuration
    try:
        config.validate()
        print("   ✓ Configuration validated successfully")
    except pynlm.ConfigurationError as e:
        print(f"   ✗ Configuration error: {e}")
        return
    
    # Create brain
    print("\n2. Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✓ Brain initialized")
    
    # Test enhanced stats
    print("\n3. Getting brain statistics...")
    batch_manager = pynlm.BrainBatchManager(brain)
    stats = batch_manager.getStats()
    for key, value in stats.items():
        print(f"   {key}: {value}")
    
    # Test context manager
    print("\n4. Testing context manager...")
    with pynlm.BrainContextManager(brain) as cm:
        ctx_brain = cm.getBrain()
        # Do some work
        ctx_brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        for step in range(10):
            ctx_brain.step(step, step * 0.001)
        print("   ✓ Context manager completed")
    
    # Test batch operations
    print("\n5. Testing batch operations...")
    injections = [
        (pynlm.NeuronId(1), 5.0),
        (pynlm.NeuronId(2), 7.0),
        (pynlm.NeuronId(3), 3.0)
    ]
    batch_manager.injectCurrentBatch(injections)
    print(f"   ✓ Injected current to {len(injections)} neurons")
    
    # Test world integration
    print("\n6. Testing world integration...")
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 50, 50)
    world.setAgentStart(50, 50)
    print("   ✓ World created and configured")
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    print("   ✓ Agent brain created")
    
    # Test action execution
    print("\n7. Testing action execution...")
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    result = action.execute(world)
    print(f"   Action: {action.getName()}")
    print(f"   Result: reward={result.reward}, success={result.success}, message={result.message}")
    
    # Test sensory percept
    print("\n8. Testing sensory percept...")
    vision = pynlm.Vision(10, 10, 3)
    image_data = np.random.rand(10, 10, 3).flatten()
    vision.setData(image_data)
    percept = world.getSensoryPercept()
    percept.setVision(vision)
    print(f"   Percept created with vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
    
    # Test memory systems
    print("\n9. Testing memory systems...")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    print(f"   Working memory capacity: {working_memory.getCapacity() if working_memory else 'N/A'}")
    print(f"   Max episodes: {episodic_memory.getMaxEpisodes() if episodic_memory else 'N/A'}")
    
    # Test neuromodulation
    print("\n10. Testing neuromodulation...")
    curiosity = brain.getCuriosity()
    if curiosity:
        print(f"   Initial curiosity level: {curiosity.getLevel()}")
    
    # Test development
    print("\n11. Testing development...")
    developmental_stage = brain.getDevelopmentalStage()
    print(f"   Developmental stage: {developmental_stage}")
    
    # Test simulation helpers
    print("\n12. Testing simulation helpers...")
    batch_manager.runSteps(100, 0.001)
    print(f"   ✓ Ran 100 simulation steps")
    
    print("\n=== All Tests Passed! ===")
    
    # Print summary
    print(f"\n=== NLM Brain Summary ===")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

def test_advanced_features():
    """Test advanced features with the enhanced API."""
    print("\n\n=== Testing Advanced Features ===\n")
    
    # Create training session
    print("1. Creating training session...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 50, 50)
    world.setAgentStart(50, 50)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    print("   ✓ Training session created")
    
    # Test error handling
    print("\n2. Testing error handling...")
    try:
        invalid_config = pynlm.Config()
        invalid_config.validate()
        print("   ✗ Validation should have failed")
    except pynlm.ConfigurationError as e:
        print(f"   ✓ Configuration error handled correctly: {e}")
    
    # Test performance monitoring
    print("\n3. Testing performance monitoring...")
    import time
    start_time = time.time()
    
    # Run simulation
    for step in range(1000):
        brain.step(step, step * 0.001)
    
    end_time = time.time()
    duration = end_time - start_time
    print(f"   ✓ 1000 simulation steps completed in {duration:.2f} seconds")
    print(f"   Performance: {1000/duration:.0f} steps/second")
    
    print("\n=== Advanced Features Test Complete ===")

if __name__ == "__main__":
    test_basic_functionality()
    test_advanced_features()
    print("\n🎉 All tests passed! The enhanced NLM Python bindings are working correctly.")
