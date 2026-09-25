#!/usr/bin/env python3
"""
Test script for NLM Python bindings.
Tests both basic functionality and new high-level convenience functions.
"""

import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic functionality...")
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    # Check statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getTotalSpikeCount() >= 0
    
    print("✓ Basic functionality test passed")

def test_world_functionality():
    """Test world functionality."""
    print("Testing world functionality...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a few steps
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    
    print("✓ World functionality test passed")

def test_high_level_functions():
    """Test new high-level convenience functions."""
    print("Testing high-level functions...")
    
    # Test createLearningAgent
    brain, world, agent = pynlm.createLearningAgent(width=15, height=15)
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    # Test enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    assert brain.getTotalSpikeCount() >= 0
    print("✓ createLearningAgent test passed")
    
    # Test simulation runner
    runner = pynlm.createSimulationRunner(width=10, height=10)
    runner.run(30)
    
    brain = runner.getBrain()
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    print("✓ SimulationRunner test passed")
    
    # Test world creation utilities
    world_with_obstacles = pynlm.createWorldWithObstacles(
        width=20, height=20,
        obstacles=[(5, 5), (10, 10), (15, 15)]
    )
    assert world_with_obstacles is not None
    
    world_with_resources = pynlm.createWorldWithResources(
        width=20, height=20,
        resources=[(3, 3), (7, 7), (12, 12)]
    )
    assert world_with_resources is not None
    print("✓ World creation utilities test passed")
    
    # Test simple agent controller
    controller = pynlm.createSimpleAgentController(width=10, height=10)
    controller.run()
    
    assert controller.getAverageFiringRate() >= 0.0
    assert controller.getTotalSpikeCount() >= 0
    assert controller.getRegionCount() >= 0
    print("✓ SimpleAgentController test passed")

def test_statistics_collection():
    """Test statistics collection functions."""
    print("Testing statistics collection...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation with statistics
    stats = pynlm.runSimulationWithStats(brain, world, agent, 100)
    
    assert stats.totalSteps == 100
    assert stats.finalSpikeCount >= 0
    assert stats.averageFiringRate >= 0.0
    assert stats.maxFiringRate >= 0.0
    assert stats.totalNeuronCount > 0
    assert stats.activeNeuronCount >= 0
    assert stats.regionCount >= 0
    
    # Test stats string representation
    stats_str = stats.toString()
    assert "SimulationStats:" in stats_str
    assert "Total Steps: 100" in stats_str
    print("✓ Statistics collection test passed")

def test_training_and_checkpointing():
    """Test training and checkpointing functionality."""
    print("Testing training and checkpointing...")
    
    # Create a temporary file for the trained brain
    with tempfile.NamedTemporaryFile(suffix='.pkl', delete=False) as temp_file:
        temp_path = temp_file.name
    
    try:
        # Train a brain and save it
        brain = pynlm.trainAndSaveBrain(
            filepath=temp_path,
            width=10, height=10,
            trainingSteps=100
        )
        
        assert brain is not None
        assert os.path.exists(temp_path)
        
        # Load the trained brain
        loaded_brain = pynlm.loadTrainedBrain(temp_path)
        assert loaded_brain is not None
        loaded_brain.initialize()
        
        # Run the loaded brain
        for step in range(50):
            loaded_brain.step(step)
        
        print("✓ Training and checkpointing test passed")
    
    finally:
        # Clean up
        if os.path.exists(temp_path):
            os.unlink(temp_path)

def test_debug_and_diagnostics():
    """Test debug and diagnostic utilities."""
    print("Testing debug and diagnostics...")
    
    # Create demo environment
    demo_brain, demo_world, demo_agent = pynlm.createDemoEnvironment()
    
    # Print brain stats (this should not raise an exception)
    pynlm.printBrainStats(demo_brain)
    
    # Validate setup
    assert pynlm.validateAgentSetup(demo_brain, demo_world, demo_agent) == True
    
    # Run some steps
    for step in range(20):
        demo_world.update(0.1)
        demo_agent.processSensoryInput(demo_world.getSensoryPercept())
        demo_brain.step(step)
        action = demo_agent.decodeMotorCommand()
        demo_world.applyMotorCommand(action, demo_world.getSimulationTime())
    
    print("✓ Debug and diagnostics test passed")

def test_additional_convenience_functions():
    """Test additional convenience functions."""
    print("Testing additional convenience functions...")
    
    # Test createAgentWorldPair
    brain, world, agent = pynlm.createAgentWorldPair()
    assert brain is not None
    assert world is not None
    assert agent is not None
    print("✓ createAgentWorldPair test passed")
    
    # Test getVersion
    version = pynlm.getVersion()
    assert isinstance(version, str)
    assert "pynlm" in version
    print("✓ getVersion test passed")
    
    # Test getFeatures
    features = pynlm.getFeatures()
    assert isinstance(features, list)
    assert len(features) > 0
    print("✓ getFeatures test passed")
    
    # Test getExamplePatterns
    examples = pynlm.getExamplePatterns()
    assert isinstance(examples, dict)
    assert "simple_brain" in examples
    assert "complete_agent" in examples
    assert "simulation_runner" in examples
    print("✓ getExamplePatterns test passed")

def test_ex exploration_simulation():
    """Test exploration simulation."""
    print("Testing exploration simulation...")
    
    brain, world, agent = pynlm.runExplorationSimulation(
        worldWidth=15, worldHeight=15,
        steps=50,
        enableCuriosity=True
    )
    
    assert brain is not None
    assert world is not None
    assert agent is not None
    assert brain.getTotalSpikeCount() >= 0
    print("✓ runExplorationSimulation test passed")

def test_demo_environment():
    """Test demo environment creation."""
    print("Testing demo environment...")
    
    brain, world, agent = pynlm.createDemoEnvironment()
    
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    # Check that features are enabled
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run a few steps
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("✓ Demo environment test passed")

def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("Running NLM Python Bindings Tests")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_world_functionality()
        test_high_level_functions()
        test_statistics_collection()
        test_training_and_checkpointing()
        test_debug_and_diagnostics()
        test_additional_convenience_functions()
        test_ex exploration_simulation()
        test_demo_environment()
        
        print("=" * 60)
        print("All tests passed! ✓")
        print("=" * 60)
        print("\nThe NLM Python bindings are working correctly.")
        print("New high-level convenience functions are available:")
        print("  - createLearningAgent()")
        print("  - createSimulationRunner()")
        print("  - createWorldWithObstacles()")
        print("  - createWorldWithResources()")
        print("  - createSimpleAgentController()")
        print("  - runSimulationWithStats()")
        print("  - trainAndSaveBrain()")
        print("  - loadTrainedBrain()")
        print("  - printBrainStats()")
        print("  - createDemoEnvironment()")
        print("  - runExplorationSimulation()")
        print("  - And more...")
        
    except Exception as e:
        print("=" * 60)
        print(f"Test failed! ✗")
        print("=" * 60)
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)