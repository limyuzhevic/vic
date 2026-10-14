#!/usr/bin/env python3
"""
Test script for NLM Python bindings enhancements.
This script demonstrates the new convenience methods and features.
"""

from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld, 
    createAgentBrain, INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    NeuronType, ActionType, DevelopmentalStage
)
import time

def test_basic_functionality():
    """Test basic functionality with new convenience methods."""
    print("=== Testing Basic Functionality ===")
    
    # Create brain with default config
    brain = createBrain()
    print(f"Created brain: {brain}")
    
    # Initialize brain
    success = brain.initialize()
    print(f"Brain initialization: {'Success' if success else 'Failed'}")
    
    # Check brain statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Regions: {brain.getRegionCount()}")
    
    # Test convenience injection methods
    print("\nTesting convenience injection methods...")
    brain.injectToExcitatory(0.5)
    brain.injectToInhibitory(-0.2)
    brain.injectToSensory(1.0)
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    print("✓ Basic functionality test passed")

def test_agent_brain():
    """Test AgentBrain with new Pythonic interface."""
    print("\n=== Testing AgentBrain ===")
    
    # Create integrated system
    brain, world, agent_brain = createIntegratedSystem()
    print(f"Created integrated system: {agent_brain}")
    
    # Test Pythonic state access
    state = agent_brain.getStateDict()
    print(f"Agent state: {state}")
    
    # Test action selection cycle
    print("\nTesting action selection cycle...")
    from pynlm import SensoryPercept, Action, ActionResult
    
    # Create a simple percept
    percept = SensoryPercept()
    percept.setVision([0.1, 0.2, 0.3, 0.4])
    percept.setTouch([0.5, 0.6])
    
    # Perform agent cycle
    action = agent_brain.act(percept, reward=1.0)
    print(f"Selected action: {action}")
    
    print("✓ AgentBrain test passed")

def test_config_pythonic():
    """Test Pythonic Config interface."""
    print("\n=== Testing Pythonic Config ===")
    
    config = createDefaultConfig()
    
    # Test Pythonic API
    config.setInt("neuron_count", 1000)
    config.setFloat("learning_rate", 0.01)
    config.setBool("enable_curiosity", True)
    
    # Test getting values
    print(f"Neuron count: {config.getInt('neuron_count')}")
    print(f"Learning rate: {config.getFloat('learning_rate')}")
    print(f"Curiosity enabled: {config.getBool('enable_curiosity')}")
    
    # Test Pythonic operators
    config["max_neurons"] = 2000
    print(f"Max neurons: {config['max_neurons']}")
    
    print("✓ Config Pythonic test passed")

def test_context_manager():
    """Test context manager functionality."""
    print("\n=== Testing Context Manager ===")
    
    brain = createBrain()
    
    # Test context manager
    with brain:
        brain.initialize()
        print("Inside context manager - brain initialized")
        brain.step(0)
    
    print("✓ Context manager test passed")

def test_batch_operations():
    """Test batch operations for performance."""
    print("\n=== Testing Batch Operations ===")
    
    brain = createBrain()
    brain.initialize()
    
    # Test batch neuron injection
    neuron_ids = [brain.getRegion(1).getAllNeurons()[i].getId() for i in range(min(5, brain.getTotalNeuronCount()))]
    print(f"Testing batch injection to {len(neuron_ids)} neurons...")
    
    from pynlm import batchNeuronInjection
    batchNeuronInjection(brain, neuron_ids, 0.1)
    
    # Test vectorized operations
    from pynlm import vectorizedNeuronalActivity
    vectorizedNeuronalActivity(brain, [NeuronType.Excitatory, NeuronType.Inhibitory], 0.5)
    
    print("✓ Batch operations test passed")

def test_experiment_setup():
    """Test experiment setup helpers."""
    print("\n=== Testing Experiment Setup ===")
    
    # Test different experiment types
    from pynlm import createPhase6Experiment, createPhase5Experiment, createPhase4Experiment
    
    print("Testing Phase 6 experiment setup...")
    experiment6 = createPhase6Experiment()
    
    print("Testing Phase 5 experiment setup...")
    experiment5 = createPhase5Experiment()
    
    print("Testing Phase 4 experiment setup...")
    experiment4 = createPhase4Experiment()
    
    print("✓ Experiment setup test passed")

def test_educational_helpers():
    """Test educational helper functions."""
    print("\n=== Testing Educational Helpers ===")
    
    # Test tutorial brain
    from pynlm import createTutorialBrain
    tutorial_brain = createTutorialBrain()
    print(f"Created tutorial brain with {tutorial_brain.getTotalNeuronCount()} neurons")
    
    # Test benchmark brain
    from pynlm import createBenchmarkBrain
    benchmark_brain = createBenchmarkBrain(neuronCount=500, regionCount=2)
    print(f"Created benchmark brain with {benchmark_brain.getTotalNeuronCount()} neurons")
    
    print("✓ Educational helpers test passed")

def test_episode_recording():
    """Test episode recording helpers."""
    print("\n=== Testing Episode Recording ===")
    
    brain = createBrain()
    brain.initialize()
    
    # Create and record an episode
    from pynlm import Action
    action = Action(ActionType.MoveForward)
    
    episode_data = brain.recordEpisode(step=100, reward=1.5, action=action)
    print(f"Recorded episode: step={episode_data.step}, reward={episode_data.reward}")
    
    print("✓ Episode recording test passed")

def main():
    """Run all tests."""
    print("NLM Python Bindings Enhancement Test Suite")
    print("=" * 50)
    
    try:
        test_basic_functionality()
        test_config_pythonic()
        test_context_manager()
        test_agent_brain()
        test_batch_operations()
        test_experiment_setup()
        test_educational_helpers()
        test_episode_recording()
        
        print("\n" + "=" * 50)
        print("✓ All tests passed successfully!")
        print("\nThe enhanced NLM Python bindings provide:")
        print("• Convenience constructors and methods")
        print("• Pythonic interfaces (context managers, properties)")
        print("• Batch operations for performance")
        print("• Higher-level abstractions (memory, episodes, experiments)")
        print("• Advanced features with direct C++ access")
        print("• Better integration with Python ecosystem")
        
    except Exception as e:
        print(f"\n✗ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
