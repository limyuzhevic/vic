"""Advanced NLM examples demonstrating various use cases."""

import pynlm
import numpy as np


def example_advanced_brain_config():
    """Example: Create a brain with advanced configuration."""
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Configure with specific parameters for different behavior
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Synapse density: {brain.getTotalSynapseCount()} synapses")
    print(f"Default developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain


def example_complete_agent_simulation():
    """Example: Run a complete agent simulation with all subsystems."""
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(100.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting agent simulation with all subsystems enabled...")
    print("Running 500 simulation steps...")
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computes and learns
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and update development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted = 0.0  # Would normally use agent's prediction
        agent.applyRewardModulation(reward, predicted)
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    print("Simulation complete!")
    return brain, world, agent


def example_learning_curves():
    """Example: Demonstrate learning metrics and progress tracking."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Collecting learning metrics over 200 steps...")
    
    metrics = {
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'development_stages': []
    }
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        metrics['firing_rates'].append(brain.getFiringNeuronCount())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['prediction_errors'].append(agent.getPredictionError())
        metrics['development_stages'].append(brain.getDevelopmentalStage())
    
    # Analyze metrics
    print("\nLearning Analysis:")
    print(f"Average firing rate: {np.mean(metrics['firing_rates']):.1f}")
    print(f"Max curiosity achieved: {np.max(metrics['curiosity_levels']):.3f}")
    print(f"Average novelty: {np.mean(metrics['novelty_levels']):.3f}")
    print(f"Average prediction error: {np.mean(metrics['prediction_errors']):.3f}")
    
    # Check if development progressed
    stages = list(set(metrics['development_stages']))
    print(f"Development stages visited: {len(stages)}")
    print(f"Stages: {[str(s) for s in stages]}")
    
    return metrics


def example_visualization_data():
    """Example: Extract data for visualization."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a simple world to simulate in
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=7, visionHeight=7)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a few steps to get interesting activity
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Extract visualization data
    data = {
        'step': 50,
        'total_neurons': brain.getTotalNeuronCount(),
        'active_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
        'agent_energy': world.getAgentBody().energy,
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    }
    
    print("Visualization Data:")
    for key, value in data.items():
        print(f"  {key}: {value}")
    
    return data


def example_advanced_agent_control():
    """Example: Advanced agent control with multiple strategies."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=9, visionHeight=9)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Custom exploration strategy with curiosity
    exploration_count = 0
    learning_count = 0
    survival_count = 0
    
    print("Running adaptive agent with mixed strategies...")
    
    for step in range(300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Adaptive behavior based on curiosity and reward
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.8 and reward < 0.5:
            # High curiosity but low reward - encourage exploration
            agent.enableCuriosity(True)
            exploration_count += 1
        elif reward > 0.5:
            # Good reward - reinforce learning
            agent.enableRewardModulation(True)
            learning_count += 1
        else:
            # Neutral conditions - maintain basic survival
            agent.enableDevelopment(True)
            survival_count += 1
        
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        agent.updateDevelopment(0.1)
        
        # Print adaptive behavior summary periodically
        if step % 100 == 0:
            print(f"Step {step}: Exploration={exploration_count}, Learning={learning_count}, Survival={survival_count}")
            print(f"  Current strategies - Curiosity:{agent.isCuriosityEnabled()}, Reward:{agent.isRewardModulationEnabled()}, Dev:{agent.isDevelopmentEnabled()}")
    
    print(f"\nStrategy Summary:")
    print(f"  Exploration phases: {exploration_count} steps")
    print(f"  Learning phases: {learning_count} steps")
    print(f"  Survival phases: {survival_count} steps")
    
    return agent


if __name__ == "__main__":
    print("=== Advanced NLM Examples ===\n")
    
    print("1. Advanced Brain Configuration Example:")
    example_advanced_brain_config()
    print("\n" + "="*50 + "\n")
    
    print("2. Complete Agent Simulation Example:")
    example_complete_agent_simulation()
    print("\n" + "="*50 + "\n")
    
    print("3. Learning Curves Analysis Example:")
    metrics = example_learning_curves()
    print("\n" + "="*50 + "\n")
    
    print("4. Visualization Data Example:")
    viz_data = example_visualization_data()
    print("\n" + "="*50 + "\n")
    
    print("5. Advanced Agent Control Example:")
    adaptive_agent = example_advanced_agent_control()
    print("\n=== All Examples Completed ===")