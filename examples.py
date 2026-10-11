#!/usr/bin/env python3
"""
NLM - Simple Examples

This file contains basic example scripts for the NLM (Neural Learning Machine) library.
These examples demonstrate simple use cases and are suitable for beginners.

To run these examples:
1. Ensure you have pynlm installed: pip install pynlm
2. Run: python examples.py [example_number]
3. Or run: python examples.py to run all examples
"""

import pynlm
import sys


def print_header(title):
    """Print a formatted header for example output."""
    print("=" * 50)
    print(title)
    print("=" * 50)


def example_1_basic():
    """Example 1: Basic Brain Operations
    
    This is the simplest example showing how to create and use a basic NLM brain.
    """
    print_header("Example 1: Basic Brain Operations")
    
    print("Creating brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    print("Initializing brain...")
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    print("Running simulation for 50 steps...")
    for i in range(50):
        brain.step(i)
        if i % 10 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print("✓ Basic brain operations example completed successfully\n")


def example_2_agent_world():
    """Example 2: Agent-World Interaction
    
    This example demonstrates how an agent interacts with a simulated world.
    """
    print_header("Example 2: Agent-World Interaction")
    
    # Create components
    print("Setting up agent-world simulation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    print(f"World: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run simulation
    print("\nRunning agent-world simulation for 50 steps...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"\n✓ Agent-world simulation completed")
    print("✓ Agent-world interaction example completed successfully\n")


def example_3_checkpoints():
    """Example 3: Checkpointing
    
    This example demonstrates saving and loading brain states.
    """
    print_header("Example 3: Checkpointing")
    
    print("Creating and simulating brain...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    print("Running simulation for 50 steps...")
    for step in range(50):
        brain.step(step)
    
    # Save and load (if supported)
    try:
        checkpoint_file = "/tmp/nlm_checkpoint_test.bin"
        
        # Save
        saved = brain.save(checkpoint_file)
        if saved:
            print(f"✓ Checkpoint saved to: {checkpoint_file}")
            
            # Create new brain and load
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            loaded = new_brain.load(checkpoint_file)
            if loaded:
                print("✓ Checkpoint loaded successfully")
                print("✓ Checkpointing example completed")
            else:
                print("⚠ Failed to load checkpoint")
        else:
            print("⚠ Failed to save checkpoint")
            
    except Exception as e:
        print(f"⚠ Checkpointing not available: {e}")
        print("This is expected if checkpointing is not fully implemented")
    
    print("✓ Checkpointing example completed\n")


def example_4_learning():
    """Example 4: Learning with Agent
    
    This example shows how to enable learning features in an agent.
    """
    print_header("Example 4: Learning with Agent")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Learning features enabled: Reward modulation, Curiosity, Development")
    print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run learning simulation
    print("\nRunning learning simulation for 30 steps...")
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
    
    print(f"\n✓ Learning simulation completed")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print("✓ Learning example completed successfully\n")


def example_5_configuration():
    """Example 5: Configuration
    
    This example demonstrates how to create and use configurations.
    """
    print_header("Example 5: Configuration")
    
    print("Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    print("Configuration created successfully")
    print(f"Config has keys: {len(config.getKeys())}")
    
    if config.has("neuron_count"):
        print("Config has 'neuron_count' key")
    
    # Create brain with config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with config: {brain.getTotalNeuronCount()} neurons")
    print("✓ Configuration example completed successfully\n")


def run_all_examples():
    """Run all examples sequentially."""
    print("NLM Simple Examples")
    print("Running all examples to demonstrate NLM functionality...\n")
    
    examples = [
        example_1_basic,
        example_2_agent_world,
        example_3_checkpoints,
        example_4_learning,
        example_5_configuration
    ]
    
    completed = 0
    failed = 0
    
    for example in examples:
        try:
            example()
            completed += 1
        except Exception as e:
            print(f"❌ Example failed: {e}")
            failed += 1
    
    print("=" * 50)
    print("SUMMARY")
    print("=" * 50)
    print(f"Total examples: {len(examples)}")
    print(f"Completed successfully: {completed}")
    print(f"Failed: {failed}")
    print("=" * 50)
    
    if failed == 0:
        print("🎉 All examples completed successfully!")
    else:
        print(f"⚠️  {failed} examples had issues")
    
    print()


def main():
    """Main function."""
    try:
        if len(sys.argv) > 1:
            example_num = int(sys.argv[1])
            
            examples = [
                example_1_basic,
                example_2_agent_world,
                example_3_checkpoints,
                example_4_learning,
                example_5_configuration
            ]
            
            if 1 <= example_num <= len(examples):
                examples[example_num - 1]()
                return 0
            else:
                print(f"❌ Invalid example number: {example_num}")
                print(f"Available examples: 1-{len(examples)}")
                return 1
        
        run_all_examples()
        return 0
        
    except Exception as e:
        print(f"❌ Fatal error: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
