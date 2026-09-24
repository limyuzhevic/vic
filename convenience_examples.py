#!/usr/bin/env python3
"""
Example script demonstrating the new pynlm convenience methods.

This script provides practical examples of how to use the new Pythonic convenience
methods in the pynlm bindings. It's designed to be run after building the pynlm
module with the new convenience methods.
"""

import sys

def demonstrate_convenience_methods():
    """
    Demonstrate the new convenience methods with example usage patterns.
    
    This function contains example code that shows how to use the new methods.
    It doesn't actually run, but serves as documentation and reference.
    """
    
    print("=" * 80)
    print("pynlm Convenience Methods - Usage Examples")
    print("=" * 80)
    
    print("\n1. BASIC CONFIGURATION WITH PRESETS")
    print("-" * 50)
    print("""
    # Create configuration from preset
    import pynlm
    
    # Quick start with default settings
    config = pynlm.createConfigFromPreset("default")
    
    # High plasticity configuration for learning
    config = pynlm.createConfigFromPreset("high_plasticity")
    
    # Agent-specific training configuration  
    config = pynlm.createConfigFromPreset("agent_training")
    """)
    
    print("\n2. DICTIONARY CONFIGURATION")
    print("-" * 50)
    print("""
    # Set multiple configuration values at once
    config = pynlm.createDefaultConfig()
    
    config.set_from_dict({
        "brain.neuronCount": 2000,
        "brain.learningRate": 0.05,
        "world.width": 200.0,
        "world.height": 200.0,
        "world.visionWidth": 10,
        "world.visionHeight": 10,
        "agent.maxEnergy": 100.0,
        "agent.energyDecayRate": 0.01,
        "agent.rewardModulation": True,
        "agent.curiosityEnabled": True
    })
    
    # Get as dictionary for inspection or serialization
    config_dict = config.to_dict()
    print(f"Configuration has {len(config_dict)} settings")
    """)
    
    print("\n3. BRAIN SIMULATION WITH CONVENIENCE METHODS")
    print("-" * 50)
    print("""
    import pynlm
    
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Simple simulation - 100 steps
    print("Running simulation for 100 steps...")
    brain.run_for_steps(100)
    
    # Reset and run - convenient atomic operation
    print("Resetting and running 50 steps...")
    brain.reset_and_run(50)
    
    # Get comprehensive brain state
    state = brain.get_brain_state()
    print(f"Simulation stats:")
    print(f"  Total neurons: {state['total_neurons']}")
    print(f"  Active neurons: {state['active_neurons']}")
    print(f"  Average firing rate: {state['average_firing_rate']:.2f}")
    print(f"  Total spikes: {state['total_spikes']}")
    
    # Context manager for safe resource management
    print("Using context manager...")
    with brain:
        brain.run_for_steps(25)
    print("Brain automatically reset after context exit")
    """)
    
    print("\n4. AGENT TRAINING WITH CONVENIENCE METHODS")
    print("-" * 50)
    print("""
    import pynlm
    
    # Setup all components
    config = pynlm.createConfigFromPreset("agent_training")
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 5, 5)
    
    # Place agent and add resources
    world.place_agent(50, 50)
    world.add_resource(pynlm.WorldObjectType.Resource, 10.0)
    world.add_resource(pynlm.WorldObjectType.Resource, 20.0)
    
    # Run a single episode
    print("Running one agent episode...")
    agent.run_agent_episode(world, 100)
    
    # Train for multiple episodes
    print("Training for 1000 episodes...")
    agent.train_agent(world, 1000)
    
    # Get comprehensive statistics
    stats = agent.get_agent_stats()
    print(f"\nTraining completed:")
    print(f"  Episodes trained: 1000")
    print(f"  Total neurons: {stats['total_neurons']}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f}")
    print(f"  Neuromodulation level: {stats['neuromodulation_level']:.2f}")
    print(f"  Curiosity level: {stats['curiosity_level']:.2f}")
    
    # Context manager for training
    print("\nUsing context manager for safe training...")
    with agent:
        agent.train_agent(world, 100)
    print("Agent automatically reset after context exit")
    """)
    
    print("\n5. WORLD MANAGEMENT WITH CONVENIENCE METHODS")
    print("-" * 50)
    print("""
    import pynlm
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(200, 200, 10, 10)
    
    # Using Pythonic convenience methods
    world.place_agent(100, 100)
    world.add_resource(pynlm.WorldObjectType.Resource, 15.0)
    world.add_resource(pynlm.WorldObjectType.Resource, 25.0)
    world.add_resource(pynlm.WorldObjectType.Hazard, -5.0)
    
    # Context manager for safe world operations
    with world:
        print("Simulating environment...")
        for step in range(50):
            world.update(0.1)
            if step % 10 == 0:
                print(f"  Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print(f"Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    """)
    
    print("\n6. ADVANCED PATTERNS")
    print("-" * 50)
    print("""
    import pynlm
    
    # Create all components
    config = pynlm.createConfigFromPreset("high_plasticity")
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(150, 150, 8, 8)
    
    # Setup world
    world.place_agent(75, 75)
    world.add_resource(pynlm.WorldObjectType.Resource, 20.0)
    
    # Training loop with statistics collection
    print("Advanced training with statistics...")
    for episode in range(10):
        # Run episode
        agent.run_agent_episode(world, 50)
        
        # Get and log statistics
        stats = agent.get_agent_stats()
        print(f"Episode {episode + 1}:")
        print(f"  Spikes: {stats['total_spikes']}")
        print(f"  Firing rate: {stats['average_firing_rate']:.2f}")
        print(f"  Neuromodulation: {stats['neuromodulation_level']:.2f}")
        
        # Reset for next episode
        agent.reset()
    
    print("\nTraining completed!")
    """)
    
    print("\n7. ERROR HANDLING AND VALIDATION")
    print("-" * 50)
    print("""
    import pynlm
    
    # Invalid preset name - handled automatically
    try:
        config = pynlm.createConfigFromPreset("invalid_preset")
    except Exception as e:
        print(f"Caught error: {e}")
    
    # Invalid episodes count - validated in train_agent
    agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    world = pynlm.createSimpleWorld()
    
    try:
        agent.train_agent(world, 0)  # Should raise exception
    except Exception as e:
        print(f"Caught error for zero episodes: {e}")
    """)
    
    print("\n" + "=" * 80)
    print("KEY BENEFITS OF CONVENIENCE METHODS:")
    print("=" * 80)
    print("""
    ✓ LESS CODE: One-line operations vs. verbose C++ boilerplate
    ✓ BETTER READABILITY: Intuitive method names and parameters
    ✓ AUTOMATIC VALIDATION: Input checking with helpful error messages
    ✓ CONTEXT MANAGEMENT: Safe resource handling with 'with' statements
    ✓ COMPREHENSIVE STATE: Easy access to all statistics and metrics
    ✓ BACKWARD COMPATIBLE: All existing code continues to work
    ✓ PYTHONIC INTERFACES: Natural Python data structures and patterns
    """)
    
    print("\n" + "=" * 80)
    print("BEFORE vs. AFTER COMPARISON")
    print("=" * 80)
    print("""
    BEFORE (requires manual implementation):
        for i in range(100):
            brain.step(i)
            # Manual state access, error handling, etc.
        
    AFTER (convenience methods):
        brain.run_for_steps(100)
        
    BEFORE (requires manual loop):
        agent.initialize(world)
        for step in range(100):
            # Manual processing, decoding, applying
        agent.reset()
        
    AFTER (convenience methods):
        agent.run_agent_episode(world, 100)
    """)

if __name__ == "__main__":
    print("Note: This is a demonstration script, not executable code.")
    print("The actual pynlm module needs to be built with the new convenience methods.")
    print()
    demonstrate_convenience_methods()
    
    print("\n" + "=" * 80)
    print("To build and run these examples:")
    print("=" * 80)
    print("""
    1. Build the pynlm module with the new convenience methods:
       cmake --build python/build
       
    2. Run the test script to verify functionality:
       python test_convenience.py
       
    3. Run this example script after successful build:
       python convenience_examples.py
       
    The bindings.cpp file in python/ has been updated with 22 new
    convenience methods across Brain, Config, AgentBrain, and SimpleWorld classes.
    """)