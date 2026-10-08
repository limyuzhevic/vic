#!/usr/bin/env python3
"""
Advanced NLM Usage Examples and Tutorials

This file demonstrates advanced usage patterns for the NLM Python bindings,
showing how to leverage the new Pythonic API features, helper functions,
and advanced usage patterns.
"""

import pynlm

print("=== Advanced NLM Usage Examples ===")
print()

def example_1_advanced_brain_creation():
    """Example 1: Advanced brain creation with different complexity levels"""
    print("1. Advanced Brain Creation")
    print("-" * 30)
    
    # Method 1: Using factory functions
    brain = pynlm.createBrain("simple")
    print(f"Created simple brain with {brain.getTotalNeuronCount()} neurons")
    
    # Method 2: Using configureBrain helper
    config = pynlm.createDefaultConfig()
    pynlm.configureBrain(config, 1000, 10, 0.15, 0.5, 0.2, -60.0, -70.0, -75.0, 30.0, 4.0)
    brain2 = pynlm.createBrain(config)
    print(f"Created custom brain with {brain2.getTotalNeuronCount()} neurons")
    
    # Method 3: Using context manager for controlled operations
    with brain2.create_context() as ctx:
        ctx.step(0)
        snapshot = ctx.getSnapshot()
        print(f"Context snapshot: {snapshot.to_string()}")
    print()

def example_2_learning_agent_setup():
    """Example 2: Setting up advanced learning agents"""
    print("2. Advanced Learning Agent Setup")
    print("-" * 30)
    
    # Create brain and agent with all learning subsystems
    brain = pynlm.createBrain("complex")
    agent = pynlm.createLearningAgent(
        brain,
        enableRewardModulation=True,
        enableStructuralPlasticity=True,
        enableDevelopment=True,
        enableCuriosity=True
    )
    
    # Check learning status
    status = agent.getLearningStatus()
    print("Learning subsystems enabled:")
    for key, value in status.items():
        if key != "neuromodulation_level":
            print(f"  {key}: {value}")
    
    # Run simulation
    print(f"Brain developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
    print()

def example_3_world_interaction():
    """Example 3: Advanced world interaction patterns"""
    print("3. Advanced World Interaction")
    print("-" * 30)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Set agent start position
    world.setAgentStart(15.0, 15.0)
    
    # Create agent brain
    brain = pynlm.createBrain("medium")
    agent = pynlm.createLearningAgent(brain)
    agent.initialize(world)
    
    # Add interesting objects to the world
    world.addResource(5.0, 5.0, 10.0)
    world.addHazard(25.0, 25.0, 1.0)
    world.addWall(20.0, 10.0, 1.5)
    
    # Run simulation loop
    print("Running simulation with world interaction...")
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
                  f"energy: {body.energy:.1f}")
    
    print()

def example_4_data_processing():
    """Example 4: Advanced data processing and serialization"""
    print("4. Advanced Data Processing")
    print("-" * 30)
    
    # Create complex percept
    brain = pynlm.createBrain("simple")
    brain.initialize()
    
    # Simulate to generate some data
    for i in range(50):
        brain.step(i)
    
    # Get current percept
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create percept and add different data types
    percept1 = world.getSensoryPercept()
    percept2 = world.getSensoryPercept()
    
    # Merge percepts
    percept1.merge(percept2)
    
    # Serialize to dictionary
    data = percept1.to_dict()
    print("Percept data structure:")
    for key, value in data.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for subkey, subvalue in value.items():
                print(f"    {subkey}: {subvalue}")
        else:
            print(f"  {key}: {value}")
    
    # Create brain snapshot
    with brain.create_context() as ctx:
        ctx.step(0)
        snapshot = ctx.getSnapshot()
        print(f"\nBrain snapshot: {snapshot.to_string()}")
    print()

def example_5_error_handling_advanced():
    """Example 5: Advanced error handling and validation"""
    print("5. Advanced Error Handling")
    print("-" * 30)
    
    # Test 1: Invalid configuration handling
    print("Testing invalid configuration...")
    try:
        config = pynlm.createDefaultConfig()
        pynlm.configureBrain(config, -100, 5)  # Invalid: negative neurons
        print("  ✗ Should have raised ValueError")
    except ValueError as e:
        print(f"  ✓ Correctly caught error: {e}")
    except Exception as e:
        print(f"  ? Caught unexpected error: {e}")
    
    # Test 2: Context manager error handling
    print("\nTesting context manager error handling...")
    brain = pynlm.createBrain(100, 2)
    brain.initialize()
    
    try:
        # This should work
        with brain.create_context() as ctx:
            ctx.step(0)
        print("  ✓ Context manager completed successfully")
        
        # Test error handling within context
        with brain.create_context() as ctx:
            ctx.step(1)
            print("  ✓ Context manager handles operations correctly")
            
    except Exception as e:
        print(f"  ✗ Context manager error: {e}")
    print()

def example_6_performance_monitoring():
    """Example 6: Performance monitoring and statistics"""
    print("6. Performance Monitoring")
    print("-" * 30)
    
    brain = pynlm.createBrain("medium")
    brain.initialize()
    
    print("Running performance benchmark...")
    steps = 100
    
    for step in range(steps):
        brain.step(step)
    
    # Get performance statistics
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    firing = brain.getFiringNeuronCount()
    avg_rate = brain.getAverageFiringRate()
    total_spikes = brain.getTotalSpikeCount()
    
    print(f"Performance metrics after {steps} steps:")
    print(f"  Neurons: {neurons:,}")
    print(f"  Synapses: {synapses:,}")
    print(f"  Active neurons: {firing}")
    print(f"  Average firing rate: {avg_rate:.3f} Hz")
    print(f"  Total spikes: {total_spikes:,}")
    
    # Calculate spikes per neuron
    if neurons > 0:
        spikes_per_neuron = total_spikes / neurons
        print(f"  Spikes per neuron: {spikes_per_neuron:.3f}")
    
    # Formatted stats
    print(f"\nFormatted brain stats:\n{brain.formatStats()}")
    print()

def example_7_advanced_configuration():
    """Example 7: Advanced configuration patterns"""
    print("7. Advanced Configuration Patterns")
    print("-" * 30)
    
    # Create configuration using different approaches
    configs = []
    
    # Config 1: Using configureBrain helper
    config1 = pynlm.createDefaultConfig()
    pynlm.configureBrain(config1, 500, 5)
    configs.append(("Helper Method", config1))
    
    # Config 2: Using dictionary-like interface
    config2 = pynlm.createDefaultConfig()
    config2["brain.neuron_count"] = 750
    config2["brain.region_count"] = 8
    config2["brain.connection_probability"] = 0.12
    configs.append(("Dictionary Interface", config2))
    
    # Config 3: Using factory function
    config3 = pynlm.createDefaultConfig()
    pynlm.configureBrain(config3, 1000, 10, connectionProbability=0.15)
    configs.append(("Partial Helper", config3))
    
    # Test all configurations
    for name, config in configs:
        brain = pynlm.createBrain(config)
        print(f"{name}: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getRegionCount()} regions")
    
    print()

def example_8_agent_lifecycle():
    """Example 8: Complete agent lifecycle management"""
    print("8. Complete Agent Lifecycle")
    print("-" * 30)
    
    # Initialize agent
    brain = pynlm.createBrain("complex")
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 10, 10)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createLearningAgent(brain)
    agent.initialize(world)
    
    # Get initial state
    initial_neuromodulation = agent.getNeuromodulationLevel()
    initial_curiosity = agent.getCuriosityLevel()
    initial_stage = brain.getDevelopmentalStage()
    
    print(f"Initial state:")
    print(f"  Neuromodulation: {initial_neuromodulation:.3f}")
    print(f"  Curiosity: {initial_curiosity:.3f}")
    print(f"  Development stage: {initial_stage}")
    
    # Run simulation
    for step in range(30):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (simulated)
        agent.applyRewardModulation(0.5, 0.3)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"  Step {step}: "
                  f"Neuromodulation={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Stage={brain.getDevelopmentalStage()}")
    
    # Final learning status
    final_status = agent.getLearningStatus()
    print(f"\nFinal learning status:")
    for key, value in final_status.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    print()

def main():
    """Run all examples"""
    print("=" * 60)
    print("Advanced NLM Usage Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_1_advanced_brain_creation,
        example_2_learning_agent_setup,
        example_3_world_interaction,
        example_4_data_processing,
        example_5_error_handling_advanced,
        example_6_performance_monitoring,
        example_7_advanced_configuration,
        example_8_agent_lifecycle,
    ]
    
    for example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {example_func.__name__}: {e}")
            print()
    
    print("=" * 60)
    print("Advanced examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    main()
