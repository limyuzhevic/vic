"""
Simple NLM Examples for Beginners

This file contains simple examples to help users get started with the NLM Python API.
These examples build upon each other to teach fundamental concepts.
"""

import pynlm

def example1_basic_brain():
    """
    Example 1: Basic Brain Operations
    
    This example shows the simplest way to create and use a brain.
    """
    print("Example 1: Basic Brain Operations")
    print("-" * 40)
    
    # Step 1: Create a configuration
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create a brain with the configuration
    brain = pynlm.createBrain(config)
    
    # Step 3: Initialize the brain
    brain.initialize()
    
    # Step 4: Run a simple simulation
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Total spikes after {10} steps: {brain.getTotalSpikeCount()}")
    print()

def example2_brain_with_world():
    """
    Example 2: Brain and World Interaction
    
    This example shows how a brain interacts with an environment.
    """
    print("Example 2: Brain and World Interaction")
    print("-" * 40)
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Run simulation
    print("Running agent in world simulation...")
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: Position ({agent.getBrain().getState().membranePotential:.1f}, "
                  f"{agent.getBrain().getState().refractoryRemaining}) Firing: {brain.getFiringNeuronCount()}")
    
    print("World simulation complete!")
    print()

def example3_learning_features():
    """
    Example 3: Learning and Adaptation
    
    This example demonstrates how brains learn and adapt through experience.
    """
    print("Example 3: Learning and Adaptation")
    print("-" * 40)
    
    # Create brain with learning enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run learning simulation
    print("Running learning simulation...")
    learning_stats = {
        "total_reward": 0.0,
        "max_curiosity": 0.0,
        "steps": 0
    }
    
    for step in range(100):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward based on performance
        reward = 0.0
        if world.getSimulationTime() > 5.0 and world.getSimulationTime() < 95.0:
            # Positive reward for staying in world
            reward = 0.1
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        learning_stats["total_reward"] += agent.getNeuromodulationLevel()
        learning_stats["max_curiosity"] = max(learning_stats["max_curiosity"], agent.getCuriosityLevel())
        learning_stats["steps"] += 1
        
        if step % 25 == 0:
            print(f"Step {step}: Reward={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print(f"\nLearning Summary:")
    print(f"  Total Reward: {learning_stats['total_reward']/learning_stats['steps']:.3f}")
    print(f"  Max Curiosity: {learning_stats['max_curiosity']:.3f}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.1f} Hz")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
    print()

def example4_brain_saving():
    """
    Example 4: Saving and Loading Brain State
    
    This example shows how to save and load brain state for persistence.
    """
    print("Example 4: Saving and Loading Brain State")
    print("-" * 40)
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to generate interesting brain state
    for step in range(50):
        brain.step(step)
    
    print(f"Initial state - Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save brain state
    brain.save("test_brain_checkpoint.bin")
    print("Brain state saved to 'test_brain_checkpoint.bin'")
    
    # Create new brain and load saved state
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Run some steps to generate different state
    for step in range(20):
        brain2.step(step)
    
    print(f"Before load - Total spikes: {brain2.getTotalSpikeCount()}")
    
    # Load the saved state
    brain2.load("test_brain_checkpoint.bin")
    
    print(f"After load - Total spikes: {brain2.getTotalSpikeCount()}")
    print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
    
    # Verify the loaded brain is functional
    brain2.step(50)
    print(f"After one more step - Total spikes: {brain2.getTotalSpikeCount()}")
    
    print("Brain save/load example complete!")
    print()

def example5_advanced_features():
    """
    Example 5: Advanced Features and Configuration
    
    This example demonstrates advanced configuration options and features.
    """
    print("Example 5: Advanced Features and Configuration")
    print("-" * 40)
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Configure various parameters
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.08)
    config.set("brain.v_thresh", -55.0f)
    config.set("brain.v_rest", -70.0f)
    config.set("brain.v_reset", -75.0f)
    
    # Configure plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    # Configure neuromodulation
    config.set("neuromod.dopamine.scale", 1.2f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Configure development
    config.set("development.enable", True)
    config.set("development.critical_period_steps", 100)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    print(f"Custom brain configuration:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Run simulation with advanced features
    for step in range(30):
        brain.step(step)
        
        if step % 10 == 0:
            stats = brain.getBrainStatistics()
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Rate={brain.getAverageFiringRate():.1f}Hz, "
                  f"Stage={brain.getDevelopmentalStage()}")
    
    print("Advanced features example complete!")
    print()

def run_all_examples():
    """Run all simple examples"""
    print("Simple NLM Examples for Beginners")
    print("=" * 50)
    print()
    
    example1_basic_brain()
    example2_brain_with_world()
    example3_learning_features()
    example4_brain_saving()
    example5_advanced_features()
    
    print("=" * 50)
    print("All examples completed successfully!")
    print("=" * 50)

if __name__ == "__main__":
    run_all_examples()
