#!/usr/bin/env python3
"""
NLM Python API - Comprehensive Examples and Tutorials

This file contains comprehensive examples for using the NLM (Neural Learning Machine)
Python bindings. These examples demonstrate the full range of NLM functionality
from basic brain creation to complete agent simulations.

Author: NLM Development Team
Version: 0.1.0
"""

import pynlm

def example_basic_brain():
    """Example 1: Create and run a basic neural brain"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain will have {brain.getTotalSynapseCount()} synapses")
    
    # Run the brain for 100 simulation steps
    print("\nRunning brain for 100 steps...")
    for i in range(100):
        brain.step(i)
        if i % 20 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {i}: {firing} neurons firing, {spikes} total spikes, {rate:.2f} Hz average")
    
    # Final statistics
    print(f"\nFinal Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return brain

def example_world_interaction():
    """Example 2: Brain interacting with a virtual world"""
    print("\n\n=== Example 2: World Interaction ===")
    
    # Setup brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(10.0, 10.0)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Environment setup complete:")
    print(f"  World size: {world.getWidth()} x {world.getHeight()} meters")
    print(f"  Agent start: (10.0, 10.0)")
    print(f"  Vision: {world.getWidth()} x {world.getHeight()} meters (8x8 grid)")
    
    # Run agent simulation
    print("\nRunning agent simulation for 100 steps...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get what agent sees
        percept = world.getSensoryPercept()
        
        # Tell brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Do the action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 20 steps
        if step % 20 == 0:
            pos = world.getAgentBody()
            percept = world.getSensoryPercept()
            print(f"  Step {step}: Agent at ({pos.x:.1f}, {pos.y:.1f}), "
                  f"firing: {brain.getFiringNeuronCount()}, "
                  f"reward: {percept.getInternal()[0] if percept.getInternal() else 0:.2f}")
    
    print(f"\nSimulation complete!")
    print(f"  Agent final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Total steps: 100")
    print(f"  Brain development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

def example_advanced_learning():
    """Example 3: Advanced learning with all subsystems enabled"""
    print("\n\n=== Example 3: Advanced Learning with All Systems ===")
    
    # Custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable ALL learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced brain configuration:")
    print(f"  Neurons: {config.getOr('brain.neuron_count', 2000)}")
    print(f"  Synapse density: {config.getOr('brain.synapse_density', 0.05)}")
    print(f"  STDP enabled: {config.getOr('plasticity.stdp.enable', True)}")
    print(f"  Hebbian enabled: {config.getOr('plasticity.hebbian.enable', True)}")
    print(f"  Development enabled: {config.getOr('neuromod.curiosity.enable', True)}")
    
    # Run learning experiment
    print("\nRunning advanced learning experiment (500 steps)...")
    total_reward = 0.0
    learning_metrics = {}
    
    for step in range(500):
        # World update
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process and think
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from reward
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        total_reward += result.reward
        
        # Collect metrics
        if step % 100 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction_error = agent.getPredictionError()
            dev_stage = brain.getDevelopmentalStage()
            
            print(f"  Step {step}: "
                  f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}, "
                  f"PredError={prediction_error:.3f}, Dev={dev_stage}")
    
    # Learning summary
    print(f"\n=== Learning Experiment Complete ===")
    print(f"  Total reward: {total_reward:.2f}")
    print(f"  Average reward per step: {total_reward/500:.3f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Final prediction error: {agent.getPredictionError():.3f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

def example_neural_planning():
    """Example 4: Neural planning and decision making"""
    print("\n\n=== Example 4: Neural Planning and Decision Making ===")
    
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable planning and curiosity
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("Neural planning simulation with exploration-exploitation:")
    print("  Agent starts at (5.0, 5.0)")
    print("  Curiosity drives exploration, reward guides exploitation")
    
    # Create a simple goal
    goal_x, goal_y = 20.0, 20.0
    print(f"\nGoal: Reach target position ({goal_y:.1f}, {goal_x:.1f})")
    
    # Simulation
    steps_to_goal = 0
    max_steps = 300
    
    for step in range(max_steps):
        world.update(0.1)
        
        # Check if reached goal
        agent_pos = world.getAgentBody()
        distance_to_goal = ((agent_pos.x - goal_x)**2 + (agent_pos.y - goal_y)**2)**0.5
        
        if distance_to_goal < 1.0:
            steps_to_goal = step
            print(f"\n✓ Goal reached in {step} steps!")
            break
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural planning and decision
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Print progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            print(f"  Step {step}: "
                  f"Position=({agent_pos.x:.1f},{agent_pos.y:.1f}), "
                  f"Curiosity={curiosity:.2f}, Distance={distance_to_goal:.1f}")
    
    if steps_to_goal == 0:
        print(f"\nNote: Goal not reached in {max_steps} steps")
        print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, world, agent

def example_checkpoints_and_persistence():
    """Example 5: Brain checkpointing and persistence"""
    print("\n\n=== Example 5: Checkpointing and Brain Persistence ===")
    
    import os
    import tempfile
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps to build state
    print("Building brain state...")
    for i in range(100):
        brain.step(i)
    
    print(f"Initial state: {brain.getTotalSpikeCount()} spikes, "
          f"{brain.getFiringNeuronCount()} firing neurons")
    
    # Create temporary checkpoint file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        # Save checkpoint
        print(f"\nSaving checkpoint to: {checkpoint_path}")
        if brain.save(checkpoint_path):
            print("✓ Checkpoint saved successfully")
        else:
            print("✗ Failed to save checkpoint")
            return brain
        
        # Simulate some more steps
        print("Running more steps...")
        for i in range(100, 200):
            brain.step(i)
        
        print(f"State after more steps: {brain.getTotalSpikeCount()} spikes")
        
        # Reset and reload from checkpoint
        print("\nResetting brain and loading from checkpoint...")
        brain.reset()
        brain.initialize()
        
        if brain.load(checkpoint_path):
            print("✓ Checkpoint loaded successfully")
            print(f"Reloaded state: {brain.getTotalSpikeCount()} spikes")
        else:
            print("✗ Failed to load checkpoint")
        
        # Continue from loaded state
        print("Continuing simulation after reload...")
        for i in range(200, 250):
            brain.step(i)
        
        print(f"State after continuation: {brain.getTotalSpikeCount()} spikes")
        
    finally:
        # Clean up checkpoint file
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
            print(f"\nCleaned up checkpoint file")
    
    return brain

def example_development_stages():
    """Example 6: Brain development stages"""
    print("\n\n=== Example 6: Brain Development Stages ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    print("Brain development simulation:")
    print("  Development enables structural changes over time")
    print("  Plasticity decreases with maturation")
    
    # Monitor development over time
    for stage in range(10):
        # Update development
        agent.updateDevelopment(0.5)
        
        # Get current developmental stage
        stage_name = brain.getDevelopmentalStage()
        plasticity = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        
        print(f"  Development step {stage}: "
              f"Stage={stage_name}, Plasticity={plasticity:.3f}, "
              f"Curiosity={curiosity:.3f}")
    
    print(f"\nFinal developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

def main():
    """Run all examples"""
    print("=" * 70)
    print("NLM Python API - Comprehensive Examples")
    print("=" * 70)
    
    # Run examples
    example_basic_brain()
    example_world_interaction()
    example_advanced_learning()
    example_neural_planning()
    example_checkpoints_and_persistence()
    example_development_stages()
    
    print("\n" + "=" * 70)
    print("All examples completed successfully!")
    print("=" * 70)
    
    print("\n\nKey takeaways:")
    print("1. Create brains with pynlm.createBrain() and initialize()")
    print("2. Use agents to connect brains to worlds (pynlm.createAgentBrain())")
    print("3. Enable learning subsystems with agent.enable*() methods")
    print("4. Brains evolve through development stages over time")
    print("5. Use checkpoints for persistent brain state")
    print("6. Curiosity drives exploration, reward guides learning")

if __name__ == "__main__":
    main()