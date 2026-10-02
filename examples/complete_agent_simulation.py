#!/usr/bin/env python3
"""
Complete NLM agent simulation example.

This example demonstrates a complete agent simulation with brain,
world, and agent integration, showing the full NLM pipeline.
"""

import pynlm
import time

def run_complete_agent_simulation(num_steps=500):
    """
    Run a complete agent simulation with brain and world integration.
    
    Args:
        num_steps (int): Number of simulation steps to run.
    """
    print("NLM Complete Agent Simulation")
    print("=" * 60)
    
    # 1. Create and configure brain
    print("1. Creating brain...")
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 2000)
    config.set("environment.width", 25)
    config.set("environment.height", 25)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Create agent brain interface
    print("2. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    print("3. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=25,
        height=25,
        visionWidth=8,
        visionHeight=8
    )
    world.setAgentStart(12.5, 12.5)  # Center of world
    world.reset()
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    
    # 4. Initialize agent with world
    print("4. Initializing agent...")
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    print("5. Enabling learning subsystems...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    print("   - Reward modulation: enabled")
    print("   - Curiosity: enabled") 
    print("   - Development: enabled")
    
    # 6. Run simulation loop
    print(f"\n6. Running simulation for {num_steps} steps...")
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        reward = 0.0
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            reward = internal[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"   Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}, "
                  f"DevStage={brain.getDevelopmentalStage()}")
    
    elapsed_time = time.time() - start_time
    
    # 7. Print final statistics
    print(f"\n7. Simulation completed!")
    print(f"   Total time: {elapsed_time:.2f} seconds")
    print(f"   Steps per second: {num_steps / elapsed_time:.1f}")
    
    print(f"\nFinal Statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Development progress: {brain.getDevelopmentProgress():.1%}")
    
    # 8. Save final brain state
    print(f"\n8. Saving brain state...")
    brain.save("agent_simulation_checkpoint.bin")
    print(f"   Saved to 'agent_simulation_checkpoint.bin'")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'metrics': {
            'total_steps': num_steps,
            'elapsed_time': elapsed_time,
            'steps_per_second': num_steps / elapsed_time,
            'final_neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_dev_stage': brain.getDevelopmentalStage()
        }
    }

def run_multiple_episodes(num_episodes=3, steps_per_episode=200):
    """
    Run multiple agent episodes to demonstrate continual learning.
    
    Args:
        num_episodes (int): Number of episodes to run.
        steps_per_episode (int): Steps per episode.
    """
    print("\n" + "=" * 60)
    print("Multiple Episodes - Continual Learning Demonstration")
    print("=" * 60)
    
    all_results = []
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        print("-" * 40)
        
        # Reset configuration for each episode
        config = pynlm.create_default_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.setAgentStart(10, 10)
        agent.initialize(world)
        
        # Enable learning for continual learning
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        # Run episode
        for step in range(steps_per_episode):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(1.0, 0.0)  # Constant reward
            agent.updateDevelopment(0.1)
        
        result = {
            'episode': episode + 1,
            'final_firing': brain.getFiringNeuronCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_dev_stage': brain.getDevelopmentalStage()
        }
        
        all_results.append(result)
        print(f"   Result: Firing={result['final_firing']}, "
              f"Curiosity={result['final_curiosity']:.2f}, "
              f"DevStage={result['final_dev_stage']}")
    
    print(f"\nContinual Learning Summary:")
    for result in all_results:
        print(f"   Episode {result['episode']}: "
              f"Firing={result['final_firing']}, "
              f"Curiosity={result['final_curiosity']:.2f}")
    
    return all_results

def main():
    """Run example demonstrations."""
    print("NLM Complete Agent Simulation Examples")
    print("This example demonstrates:")
    print("  1. Basic agent simulation with brain-world integration")
    print("  2. Learning and development over time")
    print("  3. Continual learning across multiple episodes")
    print()
    
    # Run complete agent simulation
    result = run_complete_agent_simulation(500)
    
    # Run multiple episodes for continual learning demo
    run_multiple_episodes(3, 200)
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)

if __name__ == "__main__":
    main()
