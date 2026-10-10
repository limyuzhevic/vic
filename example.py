"""
Example usage of the NLM Python bindings.

This script demonstrates how to use the NLM Python API to create and run a neural simulation.
"""

import pynlm as nlm

def example_minimal():
    """Minimal example: Create and initialize a brain."""
    print("=== Minimal Example ===")
    
    # Create a default configuration
    config = nlm.createDefaultConfig()
    
    # Optionally configure specific parameters
    # config.set("brain.neuron_count", 500)
    # config.set("brain.synapse_density", 0.1)
    
    # Create and initialize the brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    return brain

def example_simulation(brain):
    """Run a simulation with the given brain."""
    print("\n=== Simulation Example ===")
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("Simulation complete!")

def example_world_interaction():
    """Demonstrate world interaction."""
    print("\n=== World Interaction Example ===")
    
    # Create brain and world
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a short episode
    for step in range(50):
        world.update(timestep=0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Firing: {brain.getFiringNeuronCount()}, Action: {action}")

def example_advanced_features():
    """Demonstrate advanced NLM features."""
    print("\n=== Advanced Features Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Enable learning features
    agent = nlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled learning subsystems:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    print(f"Current developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Current neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Current curiosity level: {agent.getCuriosityLevel():.3f}")

def example_checkpointing(brain):
    """Demonstrate checkpoint saving and loading."""
    print("\n=== Checkpoint Example ===")
    
    # Run a bit of simulation
    for step in range(50):
        brain.step(step)
    
    # Save brain state
    brain.save("nlm_checkpoint.bin")
    print("Brain state saved to nlm_checkpoint.bin")
    
    # Create a new brain and load the state
    new_config = nlm.createDefaultConfig()
    new_brain = nlm.createBrain(new_config)
    new_brain.load("nlm_checkpoint.bin")
    
    print(f"Loaded brain status:")
    print(f"  Step count: {new_brain.getTotalSpikeCount() // 10}")  # Approximate
    print(f"  Neurons: {new_brain.getTotalNeuronCount()}")
    print(f"  Synapses: {new_brain.getTotalSynapseCount()}")

def main():
    """Run all examples."""
    print("NLM Python API Examples")
    print("=" * 50)
    
    try:
        # Example 1: Minimal usage
        brain = example_minimal()
        
        # Example 2: Basic simulation
        example_simulation(brain)
        
        # Example 3: World interaction
        example_world_interaction()
        
        # Example 4: Advanced features
        example_advanced_features()
        
        # Example 5: Checkpointing
        example_checkpointing(brain)
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nTo get started, try importing pynlm and using:")
        print("  from pynlm import createDefaultConfig, createBrain")
        print("  config = createDefaultConfig()")
        print("  brain = createBrain(config)")
        print("  brain.initialize()")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
