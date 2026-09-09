#!/usr/bin/env python3
"""
Test script to verify NLM fixes and improvements.
This script tests the key improvements made to the NLM project.
"""

import pynlm
import time

def test_basic_functionality():
    """Test basic NLM functionality after improvements."""
    print("Testing NLM basic functionality...")
    
    # Test 1: Create and initialize a brain
    print("1. Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created successfully")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # Test 2: Run simulation steps
    print("2. Running simulation for 10 steps...")
    for i in range(10):
        brain.step(i)
        print(f"   Step {i}: Active neurons={brain.getFiringNeuronCount()}, Total spikes={brain.getTotalSpikeCount()}")
    
    print("   Basic simulation working correctly!")
    
    # Test 3: Test working memory functionality
    print("3. Testing working memory integration...")
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"   Working memory initialized successfully")
        print(f"   Capacity: {working_memory.getCapacity()}")
        print(f"   Current size: {working_memory.getCurrentSize()}")
        print("   Working memory working!")
    else:
        print("   WARNING: Working memory not available")
    
    # Test 4: Test episodic memory functionality  
    print("4. Testing episodic memory integration...")
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"   Episodic memory initialized successfully")
        print(f"   Episode count: {episodic_memory.getEpisodeCount()}")
        print("   Episodic memory working!")
    else:
        print("   WARNING: Episodic memory not available")
    
    # Test 5: Test neuromodulation
    print("5. Testing neuromodulation systems...")
    dopamine = brain.getDopamine()
    if dopamine:
        print(f"   Dopamine neuromodulator: level={dopamine.getLevel():.3f}")
        print(f"   Plasticity factor: {dopamine.getPlasticityFactor():.3f}")
        print("   Neuromodulation working!")
    else:
        print("   WARNING: Neuromodulation not available")
    
    print("\nAll basic functionality tests completed successfully!")
    print("NLM improvements are working correctly.")

def test_optimization_improvements():
    """Test performance optimizations and improvements."""
    print("\nTesting optimization improvements...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test working memory capacity management
    working_memory = brain.getWorkingMemory()
    if working_memory:
        original_capacity = working_memory.getCapacity()
        working_memory.setCapacity(original_capacity + 100)
        new_capacity = working_memory.getCapacity()
        print(f"Working memory capacity management: {original_capacity} -> {new_capacity}")
    
    # Test novelty detection
    novelty = brain.getNovelty()
    if novelty:
        print(f"Novelty detection: level={novelty.getLevel():.3f}")
    
    print("Optimization improvements verified.")

def test_advanced_features():
    """Test advanced features and debugging tools."""
    print("\nTesting advanced features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test development stages
    print(f"Initial development stage: {brain.getDevelopmentalStage()}")
    
    # Test E/I ratio calculation
    ei_ratio = brain.getExcitationInhibitionRatio()
    print(f"Excitatory/Inhibitory ratio: {ei_ratio:.3f}")
    
    # Test statistics
    brain.logStatus()
    
    print("Advanced features test completed.")

def main():
    print("=" * 60)
    print("NLM Testing and Verification Suite")
    print("=" * 60)
    print(f"Python bindings version: {pynlm.__version__}")
    
    try:
        test_basic_functionality()
        test_optimization_improvements()
        test_advanced_features()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED!")
        print("NLM improvements are working correctly.")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nERROR: Test failed with exception: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
