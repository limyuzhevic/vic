import pynlm

def create_agent_simulation(num_steps=1000, enable_learning=True):
    """
    Create a complete agent simulation with brain, world, and agent.
    
    This function demonstrates the common pattern for running an agent
    in the NLM environment.
    """
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)  # Center of world
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
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
        
        # Apply reward modulation (simple: use internal signal as reward)
        if enable_learning:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if enable_learning:
            agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            if enable_learning:
                print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
                print(f"  Prediction error: {agent.getPredictionError():.3f}")
                print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'total_steps': num_steps,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    }

def run_multiple_episodes(num_episodes=5, steps_per_episode=500):
    """
    Run multiple agent episodes with fresh brains for each.
    
    This demonstrates batch processing capabilities.
    """
    results = []
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        episode_result = create_agent_simulation(
            num_steps=steps_per_episode,
            enable_learning=True
        )
        
        results.append(episode_result)
        
        # Save brain state for later analysis
        brain_file = f"brain_episode_{episode + 1}.bin"
        episode_result['brain'].save(brain_file)
        print(f"Brain saved to {brain_file}")
    
    # Calculate summary statistics
    avg_firing_rate = sum(r['firing_rate'] for r in results) / len(results)
    total_spikes = sum(r['total_spikes'] for r in results)
    
    print(f"\n=== Episode Summary ===")
    print(f"Average firing rate: {avg_firing_rate:.2f}")
    print(f"Total spikes across all episodes: {total_spikes}")
    
    return results

if __name__ == "__main__":
    print("Running agent simulation...")
    
    # Run single episode
    result = create_agent_simulation(num_steps=1000, enable_learning=True)
    
    print(f"\n=== Single Episode Results ===")
    print(f"Total steps: {result['total_steps']}")
    print(f"Total spikes: {result['total_spikes']}")
    print(f"Average firing rate: {result['firing_rate']:.2f}")
    
    # Optionally run multiple episodes
    # print("\n" + "="*60)
    # print("Running multiple episodes...")
    # all_results = run_multiple_episodes(num_episodes=3, steps_per_episode=500)
