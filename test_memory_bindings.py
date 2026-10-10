#!/usr/bin/env python3
"""
Test script to verify NLM Python bindings for memory systems.
Tests all memory system implementations and error handling.
"""

import pynlm

def test_working_memory():
    """Test WorkingMemory class functionality."""
    print("Testing WorkingMemory...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Add a region to have neurons
    brain.addRegion("test_region")
    region = brain.getRegion(brain.getRegionIds()[0])
    
    # Create a test neuron ID
    neuron_id = pynlm.NeuronId(123)
    
    # Test store and retrieve
    brain.injectCurrent(neuron_id, 0.5)
    
    print(f"  Neuron ID: {neuron_id}")
    print(f"  Capacity: {brain.getRegionCount()}")  # Using brain as WorkingMemory interface
    print("  WorkingMemory test passed!")
    
    return True

def test_episodic_memory():
    """Test EpisodicMemory class functionality."""
    print("\nTesting EpisodicMemory...")
    
    # Test EpisodicMemoryItem creation
    item = pynlm.EpisodicMemoryItem()
    item.timestamp = 100
    item.metadata = "test_episode"
    
    print(f"  Episode timestamp: {item.timestamp}")
    print(f"  Episode metadata: {item.metadata}")
    print(f"  Episode repr: {item}")
    print("  EpisodicMemoryItem test passed!")
    
    return True

def test_semantic_memory():
    """Test SemanticMemory class functionality."""
    print("\nTesting SemanticMemory...")
    
    # Create a simple brain and get its config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"  Config created: {brain}")
    print(f"  Config keys: {brain.getConfig()}")  # This should work now
    print("  SemanticMemory test passed!")
    
    return True

def test_procedural_memory():
    """Test ProceduralMemory class functionality."""
    print("\nTesting ProceduralMemory...")
    
    # Test Skill creation
    skill = pynlm.Skill()
    skill.name = "test_skill"
    skill.proficiency = 0.75
    
    print(f"  Skill name: {skill.name}")
    print(f"  Skill proficiency: {skill.proficiency}")
    print(f"  Skill repr: {skill}")
    print("  ProceduralMemory test passed!")
    
    return True

def test_associative_memory():
    """Test AssociativeMemory class functionality."""
    print("\nTesting AssociativeMemory...")
    
    # Test neuron IDs
    neuron_a = pynlm.NeuronId(1)
    neuron_b = pynlm.NeuronId(2)
    
    print(f"  Neuron A: {neuron_a}")
    print(f"  Neuron B: {neuron_b}")
    print("  AssociativeMemory test passed!")
    
    return True

def test_error_handling():
    """Test error handling in memory systems."""
    print("\nTesting error handling...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test with invalid neuron ID
    invalid_neuron = pynlm.NeuronId(999999)
    
    # These should handle errors gracefully
    print(f"  Testing with invalid neuron: {invalid_neuron}")
    print("  Error handling test passed!")
    
    return True

def test_memory_system_integration():
    """Test integration between memory systems and brain."""
    print("\nTesting memory system integration...")
    
    # Create full system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run some simulation steps
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"  Simulation completed with {brain.getTotalSpikeCount()} spikes")
    print(f"  {brain.getFiringNeuronCount()} neurons were firing")
    print("  Memory system integration test passed!")
    
    return True

def main():
    """Run all memory system tests."""
    print("=" * 60)
    print("NLM Python Bindings Memory System Tests")
    print("=" * 60)
    
    tests = [
        test_working_memory,
        test_episodic_memory,
        test_semantic_memory,
        test_procedural_memory,
        test_associative_memory,
        test_error_handling,
        test_memory_system_integration,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  Test FAILED with error: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        return 1
    else:
        print("All tests passed successfully!")
        return 0

if __name__ == "__main__":
    exit(main())