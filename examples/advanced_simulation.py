import pynlm

def run_complete_agent_simulation(num_steps=1000):
    """
    Complete agent simulation demonstrating Phase 6 integration.
    This example shows how to use all major components of the NLM Python API.
    """
    print("NLM Phase 6 Complete Agent Simulation")
    print("=" * 50)
    
    # 1. Create and initialize brain with Phase 6 capabilities
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create and configure environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all learning and development features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Run the simulation loop
    print(f"Running {num_steps} simulation steps...")
    print("Step | Firing | Curiosity | Novelty | Reward | Dev Stage")
    print("-----|--------|----------|---------|--------|----------")
    
    for step in range(num_steps):
        # 5.1 Update environment
        world.update(0.1)
        
        # 5.2 Get sensory input from world
        percept = world.getSensoryPercept()
        
        # 5.3 Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 5.4 Run brain computation
        brain.step(step)
        
        # 5.5 Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # 5.6 Apply motor command to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 5.7 Apply reward modulation based on outcome
        if result.success:
            agent.applyRewardModulation(result.reward, agent.getExpectedReward())
        
        # 5.8 Update development system
        agent.updateDevelopment(0.1)
        
        # 5.9 Log progress every 100 steps
        if step % 100 == 0:
            print(f"{step:4d} | {brain.getFiringNeuronCount():6d} | "
                  f"{agent.getCuriosityLevel():8.3f} | "
                  f"{agent.getNoveltyLevel():6.3f} | "
                  f"{agent.getNeuromodulationLevel():5.3f} | "
                  f"{brain.getDevelopmentalStage():9}")
    
    # 6. Print final statistics
    print("\n" + "=" * 50)
    print("Simulation Complete!")
    print(f"Total spikes: {brain.getTotalSpikeCount():,}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Total world time: {world.getSimulationTime():.1f}s")
    
    # 7. Access integrated memory systems
    print("\nMemory Systems Status:")
    working_mem = brain.getWorkingMemory()
    if working_mem:
        print(f"  Working memory traces: {working_mem.getActiveTraces()}")
    
    episodic_mem = brain.getEpisodicMemory()
    if episodic_mem:
        print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
    
    # 8. Save final brain state
    brain.save("final_brain_state.bin")
    print(f"\nBrain state saved to 'final_brain_state.bin'")
    
    return brain, agent, world

def run_learning_experiment():
    """
    Learning experiment demonstrating STDP and Hebbian plasticity.
    Shows how synaptic weights change through experience.
    """
    print("\nNLM Learning Experiment")
    print("=" * 40)
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    
    if stdp and hebbian:
        print("Plasticity systems initialized")
        
        # Record initial weights
        initial_weights = []
        region = brain.getRegion(pynlm.RegionId(1))
        if region:
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())
        
        if initial_weights:
            initial_avg = sum(initial_weights) / len(initial_weights)
            print(f"Initial average synaptic weight: {initial_avg:.4f}")
            
            # Apply correlated pre-post activity (LTP)
            print("\nApplying LTP-inducing activity (pre-before-post)...")
            for step in range(100):
                # Inject correlated spikes
                brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 20.0)
                brain.step(step)
            
            # Record final weights
            final_weights = []
            if region:
                for syn in region.getSynapses():
                    final_weights.append(syn.getWeight())
            
            if final_weights:
                final_avg = sum(final_weights) / len(final_weights)
                change = final_avg - initial_avg
                print(f"Final average synaptic weight: {final_avg:.4f}")
                print(f"Weight change (Δ): {change:.4f}")
                
                if change > 0.001:
                    print("✓ LTP (Long-Term Potentiation) detected!")
                elif change < -0.001:
                    print("✓ LTD (Long-Term Depression) detected!")
                else:
                    print("✗ No significant weight change detected")
    
    return brain

def run_development_simulation():
    """
    Developmental simulation showing brain maturation over time.
    Demonstrates how plasticity changes with developmental stages.
    """
    print("\nNLM Developmental Simulation")
    print("=" * 45)
    
    # Create brain for development experiment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)  # No world needed for pure development
    
    print("Monitoring brain development through simulation steps...")
    print("Step | Developmental Stage | Plasticity Modifier")
    print("-----|---------------------|-------------------")
    
    # Monitor development through extended simulation
    for step in range(3000):
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        stage = brain.getDevelopmentalStage()
        modifier = agent.getNeuromodulationLevel()
        
        if step % 300 == 0:
            print(f"{step:4d} | {str(stage):19} | {modifier:17.3f}")
    
    print(f"\nDevelopment simulation complete!")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent

def run_multi_agent_simulation():
    """
    Multi-agent simulation showing competition and cooperation.
    Demonstrates attention and concept formation systems.
    """
    print("\nNLM Multi-Agent Simulation")
    print("=" * 35)
    
    # Create multiple brains for competition
    brains = []
    agents = []
    
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(None)
        
        brains.append(brain)
        agents.append(agent)
        
        print(f"Created agent {i+1}: {brain.getTotalNeuronCount()} neurons")
    
    # Run competition phase
    print("\nRunning competition phase...")
    for episode in range(10):
        # Each agent runs independently
        for agent_idx, agent in enumerate(agents):
            brain = brains[agent_idx]
            
            # Apply unique developmental pressures
            if episode < 3:
                # Exploration phase
                agent.enableCuriosity(True)
            elif episode < 6:
                # Competition phase
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
            else:
                # Cooperation phase
                agent.enableCuriosity(False)
                agent.enableRewardModulation(True)
            
            # Run agent steps
            for step in range(100):
                brain.step(step + episode * 100)
                agent.updateDevelopment(0.1)
        
        if episode % 3 == 0:
            print(f"  Episode {episode + 1}/10 completed")
    
    # Compare final states
    print("\nFinal comparison:")
    print("Agent | Neurons | Spikes | Curios | Novelty")
    print("------|---------|--------|--------|--------")
    
    for i, (brain, agent) in enumerate(zip(brains, agents)):
        print(f"  {i+1}  | {brain.getTotalNeuronCount():6d} | "
              f"{brain.getTotalSpikeCount():5d} | "
              f"{agent.getCuriosityLevel():5.3f} | "
              f"{agent.getNoveltyLevel():5.3f}")
    
    return brains, agents

if __name__ == "__main__":
    print("NLM Python Advanced Examples")
    print("=" * 60)
    
    # Run examples
    print("\n1. Running Complete Agent Simulation...")
    brain1, agent1, world1 = run_complete_agent_simulation(500)
    
    print("\n2. Running Learning Experiment...")
    brain2 = run_learning_experiment()
    
    print("\n3. Running Development Simulation...")
    brain3, agent3 = run_development_simulation()
    
    print("\n4. Running Multi-Agent Simulation...")
    brains4, agents4 = run_multi_agent_simulation()
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("\nKey insights from these examples:")
    print("• Phase 6 integration allows complex brain-environment interaction")
    print("• Plasticity mechanisms enable learning and adaptation")
    print("• Development system guides brain maturation")
    print("• Multiple systems can interact and compete")
    print("• Python API provides full access to C++ capabilities")