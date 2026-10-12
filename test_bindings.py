#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings.

This script tests the improved Python bindings to ensure they work correctly
and maintain backward compatibility.
"""

import sys
import os
import tempfile

# Add the current directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    import pynlm.utils as nlm_utils
    print("✅ Successfully imported pynlm and pynlm.utils")
except ImportError as e:
    print(f"❌ Failed to import: {e}")
    sys.exit(1)

def test_version():
    """Test version information"""
    print("\n--- Testing Version Information ---")
    print(f"Version: {pynlm.__version__}")
    print(f"Version Name: {pynlm.__version_name__}")
    print(f"Author: {pynlm.__author__}")
    print(f"License: {pynlm.__license__}")
    
    assert pynlm.__version__ == "0.2.0", f"Expected version 0.2.0, got {pynlm.__version__}"
    print("✅ Version test passed")

def test_core_functionality():
    """Test core NLM functionality"""
    print("\n--- Testing Core Functionality ---")
    
    # Create configuration
    config = pynlm.Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    
    # Create brain
    brain = pynlm.Brain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Completed {brain.getTotalSpikeCount()} steps")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    print("✅ Core functionality test passed")

def test_world_simulation():
    """Test world simulation functionality"""
    print("\n--- Testing World Simulation ---")
    
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    world = pynlm.SimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    print(f"Created world: {world.getWidth()} x {world.getHeight()}")
    print(f"Max energy: {world.getMaxEnergy()}")
    
    # Test world update
    world.update(0.1)
    print(f"Updated world, simulation time: {world.getSimulationTime():.2f}")
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    print(f"Got sensory percept (vision: {percept.getVisionWidth()}x{percept.getVisionHeight()})")
    
    print("✅ World simulation test passed")

def test_agent_functionality():
    """Test agent functionality"""
    print("\n--- Testing Agent Functionality ---")
    
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    agent = pynlm.AgentBrain(brain)
    world = pynlm.SimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created agent with neuromodulation levels:")
    print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    print("✅ Agent functionality test passed")

def test_utilities():
    """Test utility classes"""
    print("\n--- Testing Utilities ---")
    
    # Test Experiment class
    config = nlm_utils.create_default_config()
    experiment = nlm_utils.Experiment(config, "test_experiment")
    
    print(f"Created experiment: {experiment.name}")
    print(f"Experiment steps: {experiment.steps_completed}")
    
    # Test DataHandler
    data = nlm_utils.DataHandler()
    print("Created DataHandler instance")
    
    # Test Visualization
    vis = nlm_utils.Visualization()
    print("Created Visualization instance")
    
    # Test quick test
    success = nlm_utils.run_quick_test()
    print(f"Quick test result: {'✅ Passed' if success else '❌ Failed'}")
    
    print("✅ Utilities test passed")

def test_progress_tracking():
    """Test progress tracking functionality"""
    print("\n--- Testing Progress Tracking ---")
    
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Create progress tracker
    tracker = pynlm.ProgressTracker()
    tracker.setMaxSteps(100)
    
    print(f"Created progress tracker with max steps: {tracker.getMaxSteps()}")
    
    # Simulate progress updates
    for step in range(50):
        tracker.incrementStep(f"Step {step + 1}")
    
    print(f"Progress tracker current step: {tracker.getCurrentStep()}")
    print(f"Progress tracker progress: {tracker.getProgress() * 100:.1f}%")
    
    print("✅ Progress tracking test passed")

def test_checkpointing():
    """Test checkpointing functionality"""
    print("\n--- Testing Checkpointing ---")
    
    config = pynlm.Config()
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Run some steps to generate state
    for step in range(50):
        brain.step(step)
    
    initial_spikes = brain.getTotalSpikeCount()
    print(f"Initial spike count: {initial_spikes}")
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp_file:
        checkpoint_path = tmp_file.name
    
    try:
        print(f"Saving checkpoint to: {checkpoint_path}")
        save_result = brain.save(checkpoint_path)
        
        if save_result:
            print("✅ Checkpoint saved successfully")
            
            # Load checkpoint
            print("Loading checkpoint...")
            config2 = pynlm.Config()
            brain2 = pynlm.Brain(config2)
            brain2.initialize()
            
            load_result = brain2.load(checkpoint_path)
            if load_result:
                print("✅ Checkpoint loaded successfully")
            else:
                print("❌ Failed to load checkpoint")
        else:
            print("❌ Failed to save checkpoint")
            
    finally:
        # Clean up
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
            print("Cleaned up checkpoint file")
    
    print("✅ Checkpointing test completed")

def test_factory_functions():
    """Test factory functions"""
    print("\n--- Testing Factory Functions ---")
    
    # Test createDefaultConfig
    config = pynlm.createDefaultConfig()
    print("✅ Created default config")
    
    # Test createBrain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✅ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test createSimpleWorld
    world = pynlm.createSimpleWorld()
    print("✅ Created simple world")
    
    # Test createAgentBrain
    agent = pynlm.createAgentBrain(brain)
    print("✅ Created agent brain")
    
    # Test createProgressTracker
    tracker = pynlm.createProgressTracker()
    print("✅ Created progress tracker")
    
    print("✅ Factory functions test passed")

def test_constants():
    """Test constants"""
    print("\n--- Testing Constants ---")
    
    print(f"INVALID_NEURON_ID: {pynlm.INVALID_NEURON_ID}")
    print(f"INVALID_SYNAPSE_ID: {pynlm.INVALID_SYNAPSE_ID}")
    print(f"INVALID_REGION_ID: {pynlm.INVALID_REGION_ID}")
    print(f"INVALID_POPULATION_ID: {pynlm.INVALID_POPULATION_ID}")
    
    assert pynlm.INVALID_NEURON_ID != 0, "INVALID_NEURON_ID should not be 0"
    print("✅ Constants test passed")

def run_all_tests():
    """Run all tests"""
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    tests = [
        test_version,
        test_core_functionality,
        test_world_simulation,
        test_agent_functionality,
        test_utilities,
        test_progress_tracking,
        test_checkpointing,
        test_factory_functions,
        test_constants,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"❌ Test failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed!")
    else:
        print("⚠️  Some tests failed")
        sys.exit(1)

if __name__ == "__main__":
    run_all_tests()
