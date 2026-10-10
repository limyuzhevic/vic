#!/usr/bin/env python3
"""
NLM Python Usage Examples
========================

This script demonstrates various ways to use the NLM (熙然) neural simulation framework
through its Python bindings. These examples cover basic usage, advanced features,
and practical applications.

Prerequisites:
    - NLM Python bindings (pynlm) installed
    - Build with pybind11 support

Installation:
    $ pip install -e .
    # or
    $ python setup.py build_ext --inplace
"""

import pynlm
import time
import sys

def example_basic_usage():
    """Example 1: Basic Brain Creation and Simulation"""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.4f}")
    
    # Run simulation for a few steps
    for step in range(10):
        brain.step(step, step * 0.001)
        if step % 5 == 0:
            print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.4f}")
    
    print(f"Final total spikes: {brain.getTotalSpikeCount()}")
    print()

def example_custom_configuration():
    """Example 2: Custom Configuration"""
    print("=== Example 2: Custom Configuration ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("random_seed", 12345)
    config.set("connection_probability", 0.15)
    
    print("Custom configuration:")
    print(config.summary())
    
    # Create brain with custom config
    brain = pynlm.createBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print()

def example_world_agent_integration():
    """Example 3: World-Agent Integration"""
    print("=== Example 3: World-Agent Integration ===")
    
    # Create components
    world = pynlm.createWorld(width=20, height=20, visionWidth=7, visionHeight=7)
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    
    # Set up agent in world
    world.setAgentStart(10, 10)
    agent.initialize(world)
    
    # Enable agent capabilities
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("World and agent created successfully")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent starting position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run simulation loop
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step, step * 0.001)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, step * 0.001)
        
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
                  f"Energy {body.energy:.1f}, Age {body.age}")
    
    print()

def example_memory_systems():
    """Example 4: Memory Systems"""
    print("=== Example 4: Memory Systems ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Check memory system availability
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    semantic_mem = brain.getSemanticMemory()
    
    print("Memory system availability:")
    print(f"  Working Memory: {'Available' if working_mem else 'Not Available'}")
    print(f"  Episodic Memory: {'Available' if episodic_mem else 'Not Available'}")
    print(f"  Semantic Memory: {'Available' if semantic_mem else 'Not Available'}")
    
    # Simulate some memory-related activity
    for step in range(100):
        brain.step(step, step * 0.001)
        
        # Inject some sensory activity to generate experiences
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        
        if step % 20 == 0:
            print(f"Step {step}: "
                  f"Working Memory traces: {working_mem.getActiveTraces() if working_mem else 0}")
            if episodic_mem:
                print(f"  Episodic Memory episodes: {episodic_mem.getEpisodeCount()}")
    
    print()

def example_plasticity_and_learning():
    """Example 5: Plasticity and Learning"""
    print("=== Example 5: Plasticity and Learning ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Enable plasticity
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    
    # Get initial statistics
    initial_firing = brain.getAverageFiringRate()
    initial_spikes = brain.getTotalSpikeCount()
    
    print(f"Initial state - Firing rate: {initial_firing:.4f}, Total spikes: {initial_spikes}")
    
    # Apply learning experience
    for step in range(200):
        # Create input pattern
        for i in range(10):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
        
        brain.step(step, step * 0.001)
        
        if step % 50 == 49:
            current_firing = brain.getAverageFiringRate()
            current_spikes = brain.getTotalSpikeCount()
            print(f"Step {step}: Firing rate: {current_firing:.4f}, "
                  f"Total spikes: {current_spikes}")
    
    # Final statistics
    final_firing = brain.getAverageFiringRate()
    final_spikes = brain.getTotalSpikeCount()
    
    print(f"Final state - Firing rate: {final_firing:.4f}, Total spikes: {final_spikes}")
    print(f"Change in firing rate: {final_firing - initial_firing:.4f}")
    print()

def example_neuromodulation():
    """Example 6: Neuromodulation"""
    print("=== Example 6: Neuromodulation ===")
    
    world = pynlm.createWorld()
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Neuromodulation systems enabled")
    
    # Simulate learning with neuromodulation
    for step in range(100):
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Apply reward for exploration
        reward = 0.1 if step % 20 == 0 else 0.0  # Occasional reward for exploration
        
        brain.step(step, step * 0.001)
        agent.applyRewardModulation(reward, 0.0)  # No predicted reward
        
        # Update development
        agent.updateDevelopment(0.001)
        
        if step % 25 == 0:
            neuromod_level = agent.getNeuromodulationLevel()
            curiosity_level = agent.getCuriosityLevel()
            novelty_level = agent.getNoveltyLevel()
            
            print(f"Step {step}:")
            print(f"  Neuromodulation level: {neuromod_level:.4f}")
            print(f"  Curiosity level: {curiosity_level:.4f}")
            print(f"  Novelty level: {novelty_level:.4f}")
    
    print()

def example_checkpointer():
    """Example 7: Checkpointing"""
    print("=== Example 7: Checkpointing ===")
    
    # Create and run brain
    brain1 = pynlm.createBrain()
    brain1.initialize()
    
    for step in range(100):
        brain1.step(step, step * 0.001)
    
    print(f"Brain1 created with {brain1.getTotalSpikeCount()} spikes")
    
    # Save checkpoint
    checkpoint_file = "/tmp/nlm_checkpoint_example.bin"
    if brain1.save(checkpoint_file):
        print(f"Checkpoint saved to {checkpoint_file}")
    else:
        print("Warning: Checkpoint save failed (expected in demo)")
    
    # Create new brain and load checkpoint
    brain2 = pynlm.createBrain()
    brain2.initialize()
    
    if brain2.load(checkpoint_file):
        print("Checkpoint loaded successfully")
        print(f"Brain2 loaded with {brain2.getTotalSpikeCount()} spikes")
    else:
        print("Checkpoint load failed (expected in demo)")
    
    # Clean up
    import os
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
    
    print()

def example_validation_utilities():
    """Example 8: Validation Utilities"""
    print("=== Example 8: Validation Utilities ===")
    
    # Create valid configuration
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    try:
        pynlm.validateConfig(config)
        print("Configuration validation passed")
    except Exception as e:
        print(f"Configuration validation failed: {e}")
    
    # Test utility functions
    test_vector = [0.2, 0.5, 0.8, 0.1, 0.9]
    normalized = pynlm.normalizeVector(test_vector)
    
    print(f"Original vector: {pynlm.vectorToString(test_vector)}")
    print(f"Normalized vector: {pynlm.vectorToString(normalized)}")
    print(f"All values in [0,1]: {all(0 <= x <= 1 for x in normalized)}")
    
    # Test validation functions
    print(f"Is 5.0 a positive number? {pynlm.isPositiveNumber(5.0)}")
    print(f"Is -3.0 a positive number? {pynlm.isPositiveNumber(-3.0)}")
    print(f"Is 42 a positive integer? {pynlm.isPositiveInt(42)}")
    print(f"Is 0 a positive integer? {pynlm.isPositiveInt(0)}")
    
    print()

def example_advanced_agent_behavior():
    """Example 9: Advanced Agent Behavior"""
    print("=== Example 9: Advanced Agent Behavior ===")
    
    # Create embodied agent
    world = pynlm.createWorld(width=30, height=30, visionWidth=5, visionHeight=5)
    brain = pynlm.createBrain()
    agent = pynlm.createAgentBrain(brain)
    
    # Set up world with objects
    world.setAgentStart(15, 15)
    agent.initialize(world)
    
    # Add resources and hazards
    resource = pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 10.0)
    world.addObject(resource)
    
    hazard = pynlm.WorldObject(27, 27, pynlm.WorldObjectType.Hazard, -5.0)
    world.addObject(hazard)
    
    wall = pynlm.WorldObject(10, 20, pynlm.WorldObjectType.Wall, 0.0)
    world.addObject(wall)
    
    print("World setup complete:")
    print(f"  Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"  Resources: {world.getWidth()}x{world.getHeight()} vision")
    print(f"  Objects: {len([obj for obj in [] if obj.active])} active")
    
    # Enable agent capabilities for advanced behavior
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    print("\nRunning advanced agent simulation...")
    for step in range(75):
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step, step * 0.001)
        
        # Decision making: explore if curiosity high, exploit if resources nearby
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.001)
        
        if step % 15 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Pos ({body.x:5.1f}, {body.y:5.1f}), "
                  f"Energy {body.energy:5.1f}, Age {body.age:3d}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    print()

def run_all_examples():
    """Run all examples"""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_usage,
        example_custom_configuration,
        example_world_agent_integration,
        example_memory_systems,
        example_plasticity_and_learning,
        example_neuromodulation,
        example_checkpointer,
        example_validation_utilities,
        example_advanced_agent_behavior,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
            print(f"Example {i} completed successfully!")
        except Exception as e:
            print(f"Example {i} failed with error: {e}")
            import traceback
            traceback.print_exc()
        
        print("-" * 50)
    
    print("All examples completed!")

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--run-all":
        run_all_examples()
    else:
        print("To run all examples, use: python examples.py --run-all")
        print("\nTo run individual examples, uncomment the calls below:")
        print("# example_basic_usage()")
        print("# example_custom_configuration()")
        print("# etc.")