#!/usr/bin/env python3
"""Test script to verify improved NLM Python bindings"""

import pynlm
import sys

def test_basic_functionality():
    """Test basic brain functionality"""
    print("Testing basic NLM functionality...")
    
    # Create brain with default config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic statistics methods
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Test step simulation
    for i in range(10):
        brain.step(i)
    
    print(f"  After 10 steps - firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spike count: {brain.getTotalSpikeCount()}")
    
    # Test world functionality
    print("\nTesting world functionality...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"  World width: {world.getWidth()}")
    print(f"  World height: {world.getHeight()}")
    print(f"  Simulation time: {world.getSimulationTime()}")
    
    # Test agent brain
    print("\nTesting agent brain...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"  Sensory input size: {agent.getSensoryInputSize()}")
    print(f"  Motor output size: {agent.getMotorOutputSize()}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Test config functionality
    print("\nTesting config functionality...")
    config = pynlm.createDefaultConfig()
    
    # Test has() method
    print(f"  Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
    print(f"  Has 'nonexistent.key': {config.has('nonexistent.key')}")
    
    # Test get() method
    neuron_count = config.get('brain.neuron_count')
    if neuron_count:
        print(f"  Got neuron_count: {neuron_count}")
    else:
        print(f"  neuron_count not found (nullopt)")
    
    # Test getOr() method
    neuron_count_or_default = config.getOr('brain.neuron_count', 100)
    print(f"  Got neuron_count with default: {neuron_count_or_default}")
    
    # Test with non-existent key
    non_existent_or_default = config.getOr('nonexistent.key', 42)
    print(f"  Got nonexistent key with default: {non_existent_or_default}")
    
    # Test summary()
    summary = config.summary()
    print(f"  Config summary: {summary}")
    
    print("\nAll tests completed successfully!")
    return True

if __name__ == "__main__":
    try:
        success = test_basic_functionality()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"Error during testing: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)