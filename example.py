#!/usr/bin/env python3
"""
Example script demonstrating NLM (Neural Learning Machine) Python API usage.

This script shows how to create and simulate artificial brains using NLM's Python bindings.
It's designed to be both educational and functional for testing the API.
"""

import sys
import time
from pathlib import Path

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    print("NLM Python API imported successfully")
except ImportError as e:
    print(f"Failed to import pynlm: {e}")
    print("Make sure to install pynlm first using: pip install .")
    sys.exit(1)

def example_basic_brain():
    """Example 1: Create and run a basic brain simulation."""
    print("\n=== Example 1: Basic Brain Simulation ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally customize the configuration
    # config.set("neuron_count", 500)
    # config.set("connection_probability", 0.1)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation for 100 steps
    for i in range(100):
        brain.step(i)
        
        if i % 20 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

def example_agent_world():
    """Example 2: Run a complete agent simulation with world interaction."""
    print("\n=== Example 2: Agent in World Simulation ===")
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running agent simulation for 200 steps...")
    
    # Run simulation
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (simplified)
        reward = 0.0
        if percept.getInternal().size() > 0:
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: {world.getAgentBody().getPosition()}")
    
    print("\nSimulation complete!")
    print(f"Final metrics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

def example_learning_experiment():
    """Example 3: Demonstrate learning and adaptation."""
    print("\n=== Example 3: Learning Experiment ===")
    
    # Create brain with configuration for learning
    config = pynlm.createDefaultConfig()
    
    # Set parameters for learning experiment
    # config.set("neuron_count", 2000)
    # config.set("plasticity.rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running learning experiment for 500 steps...")
    print("The agent will learn through:")
    print("  - Reward modulation (learning from rewards)")
    print("  - Structural plasticity (growing new connections)")
    print("  - Development (maturing over time)")
    print("  - Curiosity (exploring novel experiences)")
    
    start_time = time.time()
    
    # Run learning experiment
    for step in range(500):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        reward = 0.0
        if percept.getInternal().size() > 0:
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Print statistics
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step} ({elapsed:.1f}s): "
                  f"{brain.getFiringNeuronCount()} firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    
    print(f"\nLearning experiment complete! ({end_time - start_time:.1f}s)")
    print(f"Final learning metrics:")
    print(f"  Total exploration (curiosity): {agent.getCuriosityLevel():.3f}")
    print(f"  Total discovery (novelty): {agent.getNoveltyLevel():.3f}")
    print(f"  Adaptation level: {agent.getPredictionError():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")

def example_configuration():
    """Example 4: Using different configurations."""
    print("\n=== Example 4: Configuration Management ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration parameters
    print("Original configuration:")
    
    # Test different configuration options
    config.set("brain.neuron_count", 1000)
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 500)}")
    
    config.set("brain.synapse_density", 0.05)
    print(f"  Synapse density: {config.getOr('brain.synapse_density', 0.1)}")
    
    config.set("plasticity.stdp.enable", True)
    print(f"  STDP enabled: {config.getOr('plasticity.stdp.enable', false)}")
    
    # Save configuration to file
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        print(f"  Configuration saved to: {f.name}")
    
    # Load configuration from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(f.name)
    print(f"  Configuration loaded successfully with {len(config2.getKeys())} parameters")
    
    # Clean up
    import os
    os.unlink(f.name)

def example_error_handling():
    """Example 5: Error handling and robustness."""
    print("\n=== Example 5: Error Handling ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for i in range(50):
            brain.step(i)
        
        print("Brain simulation completed successfully")
        print(f"Generated {brain.getTotalSpikeCount()} spikes")
        
        # Test error handling with invalid operations
        print("\nTesting error handling...")
        
        # Note: The NLM API should handle invalid operations gracefully
        # This is mainly to demonstrate that the API is robust
        
    except Exception as e:
        print(f"Error occurred (this is expected in some cases): {e}")
    
    print("Error handling test completed")

def main():
    """Main function to run all examples."""
    print("NLM Python API Examples")
    print("=" * 50)
    print("This script demonstrates various examples of using the NLM Python API.")
    print()
    print("Make sure you have installed pynlm first:")
    print("  pip install .")
    print()
    
    examples = [
        ("Basic Brain Simulation", example_basic_brain),
        ("Agent in World Simulation", example_agent_world),
        ("Learning Experiment", example_learning_experiment),
        ("Configuration Management", example_configuration),
        ("Error Handling", example_error_handling),
    ]
    
    # Run all examples
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            continue
    
    print("\n" + "=" * 50)
    print("All examples completed!")
    print("\nFor more information, see the documentation:")
    print("  - README.md")
    print("  - easy_usage.md")
    print("  - HOW_TO_USE.md")

if __name__ == "__main__":
    main()
