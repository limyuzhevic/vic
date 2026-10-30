import pynlm

def advanced_agent_simulation(num_steps=1000):
    """
    Advanced agent simulation with curiosity, development, and environmental interaction.
    
    This example demonstrates:
    - Complete agent lifecycle management
    - Multiple sensory modalities
    - Development-driven learning
    - Curiosity-based exploration
    - Goal-oriented behavior
    """
    
    # 1. Create and initialize the brain
    config = pynlm.createDefaultConfig()
    
    # Configure brain with more neurons for better processing
    config.configureBrain(neuron_count=2000, synapse_density=0.15, 
                         connection_probability=0.08)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create and configure the world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Add structured environment elements
    # Add resources throughout the world
    for x in range(10, 25, 5):
        for y in range(10, 25, 5):
            world.addResource(x, y, 5.0)
    
    # Add hazards in corners
    for x in range(0, 5):
        for y in range(0, 5):
            world.addHazard(x, y, 1.0)
    
    # Place agent in center
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable advanced features for complex learning
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 5. Initialize tracking variables for analysis
    episode_data = {
        'step': 0,
        'position_history': [],
        'energy_history': [],
        'curiosity_history': [],
        'novelty_history': [],
        'action_history': [],
        'reward_history': [],
        'development_stages': []
    }
    
    # 6. Run the simulation with enhanced monitoring
    for step in range(num_steps):
        # Store initial state for analysis
        agent_body = world.getAgentBody()
        episode_data['step'] = step
        episode_data['position_history'].append((agent_body.x, agent_body.y))
        episode_data['energy_history'].append(agent_body.energy)
        episode_data['curiosity_history'].append(agent.getCuriosityLevel())
        episode_data['novelty_history'].append(agent.getNoveltyLevel())
        episode_data['development_stages'].append(static_cast[int>(brain.getDevelopmentalStage()))
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Compute and apply reward modulation
        percept_internal = percept.getInternal()
        reward = percept_internal[0] if percept_internal else 0.0
        
        # Use prediction error for more sophisticated learning
        prediction_error = agent.getPredictionError()
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update development based on age
        agent.updateDevelopment(0.1)
        
        # Advance developmental stage every 500 steps
        if step % 500 == 0:
            brain.advanceDevelopment()
        
        # Print progress and detailed stats every 100 steps
        if step % 100 == 0:
            action_name = agent.getActionName()
            print(f"Step {step}:")
            print(f"  Position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
            print(f"  Action: {action_name}")
            print(f"  Energy: {agent_body.energy:.1f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    
    # 7. Analyze episode results
    final_pos = episode_data['position_history'][-1]
    final_energy = episode_data['energy_history'][-1]
    max_curiosity = max(episode_data['curiosity_history'])
    final_stage = episode_data['development_stages'][-1]
    
    print("\n=== Episode Analysis ===")
    print(f"Final position: {final_pos}")
    print(f"Final energy: {final_energy:.2f}")
    print(f"Max curiosity achieved: {max_curiosity:.3f}")
    print(f"Final developmental stage: {final_stage}")
    print(f"Total steps completed: {num_steps}")
    
    # Create episode summary
    episode_summary = {
        'final_position': final_pos,
        'final_energy': final_energy,
        'max_curiosity': max_curiosity,
        'final_stage': final_stage,
        'total_steps': num_steps,
        'brain_stats': {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
    }
    
    return episode_summary

def curiosity_driven_exploration(num_steps=500):
    """
    Example showing how curiosity drives exploration behavior.
    """
    print("=== Curiosity-Driven Exploration ===")
    
    # Create agent with high curiosity
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity but disable reward modulation to focus on exploration
    agent.enableCuriosity(True)
    agent.enableRewardModulation(False)
    agent.enableDevelopment(True)
    
    exploration_data = {
        'novelty_events': 0,
        'total_steps': num_steps,
        'position_changes': 0
    }
    
    prev_x, prev_y = world.getAgentBody().x, world.getAgentBody().y
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Check for novelty (high curiosity triggers exploration)
        novelty = agent.getNoveltyLevel()
        if novelty > 0.7:
            exploration_data['novelty_events'] += 1
            print(f"  Step {step}: High novelty detected ({novelty:.3f}), exploring!")
        
        # Decode and apply motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track exploration
        curr_x, curr_y = world.getAgentBody().x, world.getAgentBody().y
        if abs(curr_x - prev_x) > 0.1 or abs(curr_y - prev_y) > 0.1:
            exploration_data['position_changes'] += 1
        
        prev_x, prev_y = curr_x, curr_y
        
        if step % 100 == 0:
            print(f"  Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={novelty:.3f}")
    
    print(f"\nExploration Summary:")
    print(f"  Novelty events: {exploration_data['novelty_events']}")
    print(f"  Position changes: {exploration_data['position_changes']}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")

def developmental_learning(num_steps=1000):
    """
    Example showing developmental changes over time.
    """
    print("=== Developmental Learning Simulation ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.setAgentStart(12.5, 12.5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    developmental_data = {
        'stages': [],
        'plasticity_changes': [],
        'learning_rates': []
    }
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record developmental stage
        stage = brain.getDevelopmentalStage()
        developmental_data['stages'].append(static_cast[int>(stage))
        
        # Track developmental changes
        if step % 100 == 0:
            print(f"Step {step}: Developmental Stage {stage}")
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Synapses: {brain.getTotalSynapseCount()}")
            print(f"  Learning rate proxy: {agent.getCuriosityLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
        
        # Change action based on developmental stage
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            # During critical period, explore more
            action = pynlm.ActionType.MoveForward
        elif stage == pynlm.DevelopmentalStage.Maturation:
            # During maturation, focus on learning
            action = pynlm.ActionType.Look
        else:
            # In adult stage, exploit known resources
            action = pynlm.ActionType.Interact
        
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)  # Provide learning signal
        agent.updateDevelopment(0.1)
    
    # Analyze developmental progression
    stages_sequence = developmental_data['stages']
    if len(set(stages_sequence)) > 1:
        print(f"\nDevelopment Analysis:")
        print(f"  Reached {len(set(stages_sequence))} different developmental stages")
        print(f"  Final stage: {stages_sequence[-1]}")
    else:
        print(f"\nDevelopment Analysis:")
        print(f"  Remained in stage {stages_sequence[0]} throughout simulation")

if __name__ == "__main__":
    print("NLM Advanced Python Examples")
    print("=" * 40)
    
    # Run different examples
    print("\n1. Advanced Agent Simulation:")
    result1 = advanced_agent_simulation(200)
    
    print("\n2. Curiosity-Driven Exploration:")
    curiosity_driven_exploration(150)
    
    print("\n3. Developmental Learning:")
    developmental_learning(300)
    
    print("\nAll examples completed successfully!")
