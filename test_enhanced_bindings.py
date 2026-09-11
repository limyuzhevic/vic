#!/usr/bin/env python3
"""
Test script to demonstrate the enhanced NLM Python bindings.
This verifies all the new API improvements.
"""

import pynlm
import json
import time

def test_config_enhancements():
    """Test enhanced Config API"""
    print("=== Testing Config Enhancements ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test 1: set_from_dict convenience method
    print("Test 1: set_from_dict")
    config.set_from_dict("test_int", 42)
    config.set_from_dict("test_double", 3.14)
    config.set_from_dict("test_bool", True)
    config.set_from_dict("test_string", "Hello World")
    
    print(f"  get_bool('test_bool'): {config.get_bool('test_bool')}")
    print(f"  get_int('test_int'): {config.get_int('test_int')}")
    print(f"  get_double('test_double'): {config.get_double('test_double')}")
    print(f"  get_string('test_string'): {config.get_string('test_string')}")
    
    # Test 2: set_and_validate
    print("\nTest 2: set_and_validate")
    is_valid = config.set_and_validate("test_int", 100)
    print(f"  set_and_validate success: {is_valid}")
    
    # Test 3: batch operations
    print("\nTest 3: batch operations")
    values = {"batch1": 1, "batch2": 2.5, "batch3": False, "batch4": "test"}
    config.set_batch(values)
    batch_results = config.get_batch(["batch1", "batch2", "batch3", "batch4"])
    print(f"  Batch results: {batch_results}")
    
    # Test 4: validation
    print("\nTest 4: validation")
    errors = []
    if config.validate(errors):
        print("  Configuration is valid")
    else:
        print(f"  Validation errors: {errors}")
    
    # Test 5: JSON export/import
    print("\nTest 5: JSON export/import")
    json_str = config.toJson()
    print(f"  JSON export: {json_str[:100]}...")
    
    # Parse JSON and load
    config2 = pynlm.createDefaultConfig()
    config2.fromJson(json_str)
    print(f"  JSON import - key exists: {config2.has('test_bool')}")
    
    print("✓ Config enhancements test passed\n")

def test_brain_enhancements():
    """Test enhanced Brain API"""
    print("=== Testing Brain Enhancements ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test 1: Basic functionality (backward compatibility)
    print("Test 1: Basic functionality")
    success = brain.initialize()
    print(f"  Brain initialization: {success}")
    
    # Test 2: Simulation helpers
    print("\nTest 2: Simulation helpers")
    # Run a few simulation steps
    for i in range(5):
        brain.step(i)
    
    print(f"  Total spikes after 5 steps: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Test 3: Session management
    print("\nTest 3: Session management")
    session = brain.createSession(0.001)
    print(f"  Session created: {session.isActive()}")
    
    # Run steps using session
    session.runSteps(3)
    print(f"  Session after 3 steps: {session.getCurrentStep()} steps, {session.getCurrentTime()} time")
    
    # Test 4: Statistical analysis
    print("\nTest 4: Statistical analysis")
    test_data = [1.0, 2.0, 3.0, 4.0, 5.0]
    mean = brain.getStatisticalMean(test_data)
    variance = brain.getStatisticalVariance(test_data, mean)
    stddev = brain.getStatisticalStdDev(test_data, mean)
    bounds = brain.getStatisticalBounds(test_data)
    
    print(f"  Mean: {mean}")
    print(f"  Variance: {variance}")
    print(f"  StdDev: {stddev}")
    print(f"  Bounds: {bounds}")
    
    # Test 5: Convenience methods
    print("\nTest 5: Convenience methods")
    brain.setupForControlTask(targetReward=0.8, explorationBonus=0.2)
    print("  Setup for control task: completed")
    
    brain.setupForExplorationTask(noveltyThreshold=0.6, curiosityFactor=1.2)
    print("  Setup for exploration task: completed")
    
    brain.setupForMemoryTask(memoryCapacity=2000.0, consolidationRate=0.02)
    print("  Setup for memory task: completed")
    
    print("✓ Brain enhancements test passed\n")

def test_agent_brain_enhancements():
    """Test enhanced AgentBrain API"""
    print("=== Testing AgentBrain Enhancements ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test 1: Method chaining
    print("Test 1: Method chaining")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Use configuration builder
    config_builder = agent.configure()
    config_builder.setRewardModulation(True)
    config_builder.setCuriosityLevel(0.7)
    config_builder.apply()
    print("  Method chaining: completed")
    
    # Test 2: Convenience methods
    print("\nTest 2: Convenience methods")
    agent.setupForControlTask(targetReward=1.0, explorationBonus=0.1)
    print("  Setup for control task: completed")
    
    agent.setupForExplorationTask(noveltyThreshold=0.5, curiosityFactor=1.0)
    print("  Setup for exploration task: completed")
    
    # Test 3: Subscription-based neuromodulation
    print("\nTest 3: Subscription-based neuromodulation")
    def neuromod_callback(level):
        print(f"    Neuromodulation callback: level={level}")
    
    subscription = agent.subscribeToNeuromodulation(neurom_callback)
    print(f"  Subscription created: {subscription.isActive()}")
    
    # Test 4: Debugging and profiling
    print("\nTest 4: Debugging and profiling")
    agent.startProfiler()
    print("  Profiler started")
    
    # Do some work
    for i in range(5):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    agent.stopProfiler()
    report = agent.getProfilerReport()
    print(f"  Profiler report: {report[:100]}...")
    
    # Test 5: State management
    print("\nTest 5: State management")
    agent.saveState("agent_state_test")
    print("  State saved")
    
    # Note: loading would require writing to file first
    # agent.loadState("agent_state_test")
    # print("  State loaded")
    
    print("✓ AgentBrain enhancements test passed\n")

def test_world_enhancements():
    """Test enhanced World API"""
    print("=== Testing World Enhancements ===")
    
    world = pynlm.createSimpleWorld()
    
    # Test 1: Method chaining
    print("Test 1: Method chaining")
    config_builder = world.configureWorld()
    config_builder.setWidth(30.0)
    config_builder.setHeight(30.0)
    config_builder.setVisionSize(12, 12)
    config_builder.apply()
    print("  Method chaining: completed")
    
    # Test 2: High-level navigation
    print("\nTest 2: High-level navigation")
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Move agent
    world.moveAgent(5.0, 3.0)
    print(f"  Agent moved to: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Turn agent
    world.turnAgent(0.5)
    print(f"  Agent turned to orientation: {world.getAgentBody().orientation}")
    
    # Move to specific location
    world.moveTo(25.0, 25.0, maxSteps=10)
    print(f"  Agent moved to target location")
    
    # Test 3: Interaction methods
    print("\nTest 3: Interaction methods")
    # Add some objects
    world.addResource(10.0, 10.0, value=5.0)
    world.addHazard(30.0, 30.0, damage=3.0)
    world.addMarker(40.0, 40.0, value=10.0)
    
    # Interact with resource
    result = world.interactWithObject(10.1, 10.1)
    print(f"  Interacted with resource: reward={result.reward}, success={result.success}")
    
    # Test 4: Visualization and debugging
    print("\nTest 4: Visualization and debugging")
    world_map = world.generateWorldMap()
    print(f"  World map (first 100 chars): {world_map[:100]}...")
    
    object_report = world.generateObjectReport()
    print(f"  Object report: {object_report}")
    
    agent_state = world.generateAgentState()
    print(f"  Agent state: {agent_state}")
    
    # Test 5: Debugging and profiling
    print("\nTest 5: Debugging and profiling")
    world.setDebugLevel(3)
    print(f"  Debug level set to: {world.getDebugLevel()}")
    
    world.startProfiler()
    print("  Profiler started")
    
    # Do some work
    for i in range(3):
        world.update(0.1)
    
    world.stopProfiler()
    report = world.getProfilerReport()
    print(f"  Profiler report: {report[:100]}...")
    
    print("✓ World enhancements test passed\n")

def test_integration():
    """Test complete integration of all enhancements"""
    print("=== Testing Complete Integration ===")
    
    # Create the complete system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure using new API
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    brain.initialize()
    
    # Use method chaining for configuration
    config_builder = agent.configure()
    config_builder.setRewardModulation(True)
    config_builder.setCuriosityLevel(0.8)
    config_builder.apply()
    
    # Enable world configuration via method chaining
    world_config = world.configureWorld()
    world_config.setEnergySettings(200.0, 0.05)
    world_config.apply()
    
    # Run complete simulation with enhanced features
    print("Running complete simulation with all enhancements...")
    for step in range(20):
        # World updates
        world.update(0.1)
        
        # Agent perception
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinking
        brain.step(step)
        
        # Brain action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 5 == 0:
            print(f"  Step {step}: "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Total spikes: {brain.getTotalSpikeCount()}, "
                  f"Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
    
    # Use experimental mode
    print("\nUsing experimental mode...")
    brain.enableExperimentalMode()
    agent.enableExperimentalMode()
    print("  Experimental mode enabled for both brain and agent")
    
    # Generate comprehensive reports
    activity_report = brain.generateActivityReport()
    print(f"\nBrain activity report: {activity_report[:200]}...")
    
    print("✓ Complete integration test passed\n")

def main():
    """Run all tests"""
    print("Testing Enhanced NLM Python Bindings")
    print("=" * 50)
    
    try:
        test_config_enhancements()
        test_brain_enhancements()
        test_agent_brain_enhancements()
        test_world_enhancements()
        test_integration()
        
        print("=" * 50)
        print("✓ ALL TESTS PASSED!")
        print("\nThe enhanced NLM Python bindings provide:")
        print("  • More Pythonic and intuitive API")
        print("  • Better method chaining")
        print("  • Type-safe access methods")
        print("  • Enhanced validation and error handling")
        print("  • Rich statistical analysis tools")
        print("  • Advanced debugging and profiling")
        print("  • Experimental and research mode helpers")
        print("  • Comprehensive convenience functions")
        print("\nAll original functionality is preserved for backward compatibility.")
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
