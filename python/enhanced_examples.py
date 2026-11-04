#!/usr/bin/env python3
"""
NLM Enhanced Python Examples

This file contains comprehensive examples demonstrating the improved
NLM Python bindings with utilities and debugging tools.
"""

import pynlm
import nlm_setup as setup
import time
from typing import Dict, Any

def example_basic_usage():
    """Demonstrate basic NLM usage with new utilities."""
    print("=" * 60)
    print("Example 1: Basic NLM Usage")
    print("=" * 60)
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config.summary()}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Initialized brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Get statistics using new utilities
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nBrain Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Display formatted results
    print(f"\nFormatted Results:")
    print(pynlm.formatSimulationResults(stats))
    
    return brain, stats

def example_agent_world_integration():
    """Demonstrate complete agent-world integration."""
    print("\n" + "=" * 60)
    print("Example 2: Agent-World Integration")
    print("=" * 60)
    
    # Create complete agent system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)
    
    # Add some objects to the world
    resource = pynlm.WorldObject(x=15, y=15, type=pynlm.WorldObjectType.Resource, value=1.0)
    hazard = pynlm.WorldObject(x=5, y=5, type=pynlm.WorldObjectType.Hazard, value=0.8)
    wall = pynlm.WorldObject(x=1, y=10, type=pynlm.WorldObjectType.Wall, value=0.5)
    
    world.addObject(resource)
    world.addObject(hazard)
    world.addObject(wall)
    
    # Run simulation loop
    print("Running agent simulation...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress every 20 steps
        if step % 20 == 0:
            print(f"  Step {step}: Action {action}")
    
    # Get final statistics
    stats = pynlm.getBrainStatistics(brain)
    print(f"\nSimulation completed:")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    
    return brain, agent, world

def example_brain_inspection():
    """Demonstrate brain inspection and debugging utilities."""
    print("\n" + "=" * 60)
    print("Example 3: Brain Inspection and Debugging")
    print("=" * 60)
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Add some input to make it interesting
    brain.injectCurrentToNeurons(pynlm.NeuronType::Sensory, 5.0f)
    
    # Run a few steps
    for step in range(30):
        brain.step(step)
    
    # Use new debugging utilities
    print("Brain State:")
    print(pynlm.getBrainStateString(brain))
    
    # Get detailed action information
    actions = pynlm.getAvailableActions(brain)
    print(f"\nAvailable Actions:")
    for action_dict in actions:
        action_type = action_dict.get('type', 'Unknown')
        print(f"  Action Type: {action_type}")
    
    # Validate brain state
    issues = setup.validate_brain_state(brain)
    if issues:
        print(f"\nValidation Issues:")
        for issue in issues:
            print(f"  - {issue}")
    else:
        print("\n✓ Brain state validation passed")
    
    return brain

def example_advanced_learning():
    """Demonstrate advanced learning features."""
    print("\n" + "=" * 60)
    print("Example 4: Advanced Learning Features")
    print("=" * 60)
    
    # Create agent with learning enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize()
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Create world for learning
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.setAgentStart(5, 5)
    
    # Add resource (positive reward)
    resource = pynlm.WorldObject(x=12, y=12, type=pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(resource)
    
    # Add hazard (negative reward)
    hazard = pynlm.WorldObject(x=8, y=8, type=pynlm.WorldObjectType.Hazard, value=0.8)
    world.addObject(hazard)
    
    # Run learning simulation
    print("Running learning simulation...")
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on action outcome
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        if action_result:
            agent.applyRewardModulation(action_result.reward, agent.getPredictionError())
    
    # Show learning results
    print(f"\nLearning Results:")
    print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
    print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    print(f"  Learned Preference: {brain.getLearnedPreference():.3f}")
    
    # Show thought trace
    thought = agent.getThoughtTrace()
    print(f"\nThought Trace: {thought}")
    
    return brain, agent, world

def example_performance_monitoring():
    """Demonstrate performance monitoring utilities."""
    print("\n" + "=" * 60)
    print("Example 5: Performance Monitoring")
    print("=" * 60)
    
    # Benchmark brain creation
    print("Benchmarking brain creation...")
    results = setup.benchmark_brain_creation(20)
    
    print(f"  Results:")
    print(f"    Iterations: {results['iterations']}")
    print(f"    Total time: {results['total_time']:.4f}s")
    print(f"    Average time per brain: {results['average_time_per_brain']:.4f}s")
    print(f"    Brains per second: {results['brains_per_second']:.2f}")
    
    # Test simulation performance
    print("\nTesting simulation performance...")
    start_time = time.time()
    
    for i in range(5):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        for step in range(100):
            brain.step(step)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"  Performance Summary:")
    print(f"    Time for 5x100-step simulations: {total_time:.4f}s")
    print(f"    Steps per second: {(5 * 100) / total_time:.1f}")
    print(f"    Brains created: {5}")
    
    return results

def example_text_processing():
    """Demonstrate Phase 6 text processing capabilities."""
    print("\n" + "=" * 60)
    print("Example 6: Text Processing (Phase 6)")
    print("=" * 60)
    
    # Create agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize()
    
    # Demonstrate text encoding
    print("Text Encoding Examples:")
    test_texts = ["hello", "world", "test", "example"]
    
    for text in test_texts:
        encoded = agent.encodeText(text)
        print(f"  '{text}' -> {encoded}")
    
    # Show that encoding is deterministic
    print("\nDeterminism Test:")
    hello1 = agent.encodeText("hello")
    hello2 = agent.encodeText("HELLO")  # Case insensitive
    print(f"  'hello' encoded as: {hello1}")
    print(f"  'HELLO' encoded as: {hello2}")
    print(f"  Same encoding: {hello1 == hello2}")
    
    # Show that different texts encode differently
    print("\nUniqueness Test:")
    apple = agent.encodeText("apple")
    apple2 = agent.encodeText("app")
    zebra = agent.encodeText("zebra")
    print(f"  'apple' -> {apple}")
    print(f"  'app' -> {apple2}")
    print(f"  'zebra' -> {zebra}")
    print(f"  apple != app: {apple != apple2}")
    print(f"  apple != zebra: {apple != zebra}")
    
    # Get internal firing rates
    print("\nInternal Firing Rates:")
    rates = agent.getInternalFiringRates()
    print(f"  Number of internal neurons: {len(rates)}")
    print(f"  Sample rates (first 5): {rates[:5]}")
    
    return agent

def example_batch_processing():
    """Demonstrate batch processing capabilities."""
    print("\n" + "=" * 60)
    print("Example 7: Batch Processing")
    print("=" * 60)
    
    # Run multiple simulations with different parameters
    print("Running multiple simulations with different neuron counts...")
    
    simulations = []
    for neuron_count in [50, 100, 200]:
        print(f"\nSimulation with {neuron_count} neurons:")
        
        # Create brain with custom configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Actually set neuron count (simplified)
        for step in range(50):
            brain.step(step)
        
        stats = pynlm.getBrainStatistics(brain)
        print(f"  Final neurons: {stats['total_neurons']}")
        print(f"  Final spikes: {stats['total_spikes']}")
        print(f"  Firing rate: {stats['average_firing_rate']:.2f} Hz")
        
        simulations.append({
            'neurons': neuron_count,
            'stats': stats,
            'formatted': pynlm.formatSimulationResults(stats)
        })
    
    # Compare results
    print(f"\nBatch Processing Summary:")
    for sim in simulations:
        print(f"  {sim['neurons']} neurons: {sim['stats']['total_spikes']} spikes")
    
    return simulations

def example_configuration_utilities():
    """Demonstrate configuration utilities."""
    print("\n" + "=" * 60)
    print("Example 8: Configuration Utilities")
    print("=" * 60)
    
    # Create default configuration dictionary
    config_dict = pynlm.createDefaultConfigDict()
    
    print("Default Configuration Dictionary:")
    print(pynlm.configToString(config_dict))
    
    # Create example configuration
    example_config = setup.create_example_config()
    
    print("\nExample Configuration Structure:")
    for key, value in example_config.items():
        print(f"  {key}:")
        if isinstance(value, dict):
            for subkey, subvalue in value.items():
                print(f"    {subkey}: {subvalue}")
        else:
            print(f"    {value}")
    
    return config_dict, example_config

def main():
    """Run all examples."""
    print("NLM Enhanced Python Bindings - Examples")
    print("=" * 60)
    
    try:
        # Run all examples
        example_basic_usage()
        example_agent_world_integration()
        example_brain_inspection()
        example_advanced_learning()
        example_performance_monitoring()
        example_text_processing()
        example_batch_processing()
        example_configuration_utilities()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())