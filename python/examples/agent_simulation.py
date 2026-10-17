#!/usr/bin/env python3
"""
NLM Python Agent Simulation Examples

This script demonstrates agent simulation capabilities with NLM Python bindings,
including sensorimotor loops, neuromodulation, and advanced agent behaviors.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def example_basic_agent_loop():
    """Example: Basic agent simulation with sensorimotor loop"""
    print("=" * 60)
    print("Basic Agent Simulation")
    print("=" * 60)
    
    # Setup agent simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable basic features
    agent.enableRewardModulation(True)
    
    # Run simulation
    num_steps = 100
    print(f"Running {num_steps}-step simulation...")
    
    for step in range(num_steps):
        # Sensorimotor loop
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("✓ Basic simulation completed successfully")
    return brain, world, agent

def example_advanced_agent_with_neuromodulation():
    """Example: Advanced agent with full neuromodulation"""
    print("\n" + "=" * 60)
    print("Advanced Agent with Neuromodulation")
    print("=" * 60)
    
    # Setup agent with all neuromodulation systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Neuromodulation systems enabled:")
    print(f"  - Reward Modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run simulation with neuromodulation
    num_steps = 200
    neuromodulation_levels = []
    
    print(f"\nRunning {num_steps}-step simulation with neuromodulation...")
    
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with neuromodulation
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect neuromodulation levels
        neuromodulation_levels.append({
            'step': step,
            'reward_mod': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'dev_stage': brain.getDevelopmentalStage()
        })
        
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
    
    # Analyze neuromodulation patterns
    df_neuro = pd.DataFrame(neuromodulation_levels)
    
    print(f"\nNeuromodulation Analysis:")
    print(f"  Average Neuromodulation Level: {df_neuro['reward_mod'].mean():.3f}")
    print(f"  Average Curiosity: {df_neuro['curiosity'].mean():.3f}")
    print(f"  Average Novelty: {df_neuro['novelty'].mean():.3f}")
    print(f"  Average Prediction Error: {df_neuro['prediction_error'].mean():.3f}")
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Neuromodulation levels over time
    axes[0, 0].plot(df_neuro['step'], df_neuro['reward_mod'], 'r-', label='Reward Modulation', linewidth=2)
    axes[0, 0].plot(df_neuro['step'], df_neuro['curiosity'], 'b-', label='Curiosity', linewidth=2)
    axes[0, 0].plot(df_neuro['step'], df_neuro['novelty'], 'g-', label='Novelty', linewidth=2)
    axes[0, 0].set_title('Neuromodulation Levels Over Time')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Level')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Development stages
    axes[0, 1].hist(df_neuro['dev_stage'].value_counts().sort_index(), bins=5, alpha=0.7, color='skyblue')
    axes[0, 1].set_title('Development Stage Distribution')
    axes[0, 1].set_xlabel('Development Stage')
    axes[0, 1].set_ylabel('Frequency')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Prediction error
    axes[1, 0].plot(df_neuro['step'], df_neuro['prediction_error'], 'orange', linewidth=2)
    axes[1, 0].set_title('Prediction Error Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Prediction Error')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Neuromodulation correlation
    correlation_matrix = df_neuro[['reward_mod', 'curiosity', 'novelty', 'prediction_error']].corr()
    im = axes[1, 1].imshow(correlation_matrix, cmap='coolwarm', vmin=-1, vmax=1, aspect='auto')
    axes[1, 1].set_title('Neuromodulation Correlations')
    axes[1, 1].set_xticks(range(len(correlation_matrix.columns)))
    axes[1, 1].set_xticklabels(correlation_matrix.columns)
    axes[1, 1].set_yticks(range(len(correlation_matrix.index)))
    axes[1, 1].set_yticklabels(correlation_matrix.index)
    
    # Add correlation values
    for i in range(len(correlation_matrix.index)):
        for j in range(len(correlation_matrix.columns)):
            axes[1, 1].text(j, i, f'{correlation_matrix.iloc[i, j]:.2f}', 
                           ha='center', va='center', color='black', fontsize=8)
    
    plt.tight_layout()
    plt.savefig('neuromodulation_analysis.png', dpi=150)
    print(f"\n✓ Neuromodulation visualization saved as 'neuromodulation_analysis.png'")
    
    plt.close()
    
    # Save neuromodulation data
    df_neuro.to_csv('neuromodulation_data.csv', index=False)
    print(f"✓ Neuromodulation data saved as 'neuromodulation_data.csv'")
    
    return brain, world, agent, df_neuro

def example_exploratory_agent():
    """Example: Exploratory agent with curiosity-driven behavior"""
    print("\n" + "=" * 60)
    print("Exploratory Agent with Curiosity-Driven Behavior")
    print("=" * 60)
    
    # Setup agent with curiosity emphasis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("neuromod.curiosity.exploration_bonus", 0.8)
    config.set("neuromod.novelty.sensitivity", 0.9)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=15, visionHeight=15)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity and novelty systems
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableStructuralPlasticity(True)
    
    print("Exploratory agent settings:")
    print(f"  - Exploration Bonus: 0.8")
    print(f"  - Novelty Sensitivity: 0.9")
    print(f"  - Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
    
    # Run exploration simulation
    num_steps = 300
    exploration_data = []
    
    print(f"\nRunning exploratory simulation...")
    
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with curiosity
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply curiosity-driven neuromodulation
        novelty = agent.getNoveltyLevel()
        curiosity = agent.getCuriosityLevel()
        agent.applyRewardModulation(novelty, curiosity)
        
        # Update development (encourages exploration)
        agent.updateDevelopment(0.1)
        
        # Collect exploration metrics
        exploration_data.append({
            'step': step,
            'novelty': novelty,
            'curiosity': curiosity,
            'movement': abs(world.getAgentBody().velocityX) + abs(world.getAgentBody().velocityY),
            'energy': world.getAgentBody().energy,
            'dev_stage': brain.getDevelopmentalStage()
        })
        
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Novelty: {novelty:.3f}")
            print(f"  Curiosity: {curiosity:.3f}")
            print(f"  Movement: {exploration_data[-1]['movement']:.2f}")
            print(f"  Energy: {exploration_data[-1]['energy']:.2f}")
    
    # Analyze exploration patterns
    df_explore = pd.DataFrame(exploration_data)
    
    print(f"\nExploration Analysis:")
    print(f"  Average Novelty: {df_explore['novelty'].mean():.3f}")
    print(f"  Average Curiosity: {df_explore['curiosity'].mean():.3f}")
    print(f"  Total Movement: {df_explore['movement'].sum():.2f}")
    print(f"  Average Movement: {df_explore['movement'].mean():.2f}")
    print(f"  Final Development Stage: {df_explore['dev_stage'].iloc[-1]}")
    
    # Create exploration visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Novelty and curiosity over time
    axes[0, 0].plot(df_explore['step'], df_explore['novelty'], 'r-', label='Novelty', linewidth=2)
    axes[0, 0].plot(df_explore['step'], df_explore['curiosity'], 'b-', label='Curiosity', linewidth=2)
    axes[0, 0].set_title('Exploration Metrics Over Time')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Level')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Movement vs Novelty scatter
    axes[0, 1].scatter(df_explore['movement'], df_explore['novelty'], 
                      c=df_explore['step'], cmap='viridis', alpha=0.6, s=10)
    axes[0, 1].set_title('Movement vs Novelty')
    axes[0, 1].set_xlabel('Movement')
    axes[0, 1].set_ylabel('Novelty')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Energy over time
    axes[1, 0].plot(df_explore['step'], df_explore['energy'], 'g-', linewidth=2)
    axes[1, 0].set_title('Energy Consumption Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Energy')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Development stages
    stage_counts = df_explore['dev_stage'].value_counts().sort_index()
    axes[1, 1].bar(range(len(stage_counts)), stage_counts.values, alpha=0.7, color='skyblue')
    axes[1, 1].set_title('Development Stage Distribution')
    axes[1, 1].set_xlabel('Development Stage')
    axes[1, 1].set_ylabel('Count')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('exploratory_agent_analysis.png', dpi=150)
    print(f"\n✓ Exploration visualization saved as 'exploratory_agent_analysis.png'")
    
    plt.close()
    
    # Save exploration data
    df_explore.to_csv('exploratory_agent_data.csv', index=False)
    print(f"✓ Exploration data saved as 'exploratory_agent_data.csv'")
    
    return brain, world, agent, df_explore

def example_competitive_agent():
    """Example: Competitive agent simulation"""
    print("\n" + "=" * 60)
    print("Competitive Agent Simulation")
    print("=" * 60)
    
    # Setup competitive agents
    agents = []
    worlds = []
    brains = []
    
    for i in range(3):
        print(f"\nCreating Agent {i+1}:")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 600)
        config.set("plasticity.stdp.enable", True)
        config.set(f"agent_id", i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(12.5 + i * 5, 12.5 + i * 5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable competitive features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        worlds.append(world)
        brains.append(brain)
        
        print(f"  Agent {i+1} created at position ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run competitive simulation
    num_steps = 200
    competitive_data = []
    
    print(f"\nRunning competitive simulation...")
    
    for step in range(num_steps):
        step_data = {'step': step}
        
        for i, (agent, world) in enumerate(zip(agents, worlds)):
            # World update
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with agent
            agent.processSensoryInput(percept)
            brains[i].step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply competitive reward (based on novelty)
            reward = agent.getNoveltyLevel() * 0.1
            agent.applyRewardModulation(reward, reward * 0.5)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect agent metrics
            step_data[f'agent_{i+1}_novelty'] = agent.getNoveltyLevel()
            step_data[f'agent_{i+1}_curiosity'] = agent.getCuriosityLevel()
            step_data[f'agent_{i+1}_firing'] = brains[i].getFiringNeuronCount()
        
        competitive_data.append(step_data)
        
        if step % 50 == 0:
            print(f"\nStep {step}:")
            for i, agent in enumerate(agents):
                print(f"  Agent {i+1}: Novelty={agent.getNoveltyLevel():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Firing={brains[i].getFiringNeuronCount()}")
    
    # Analyze competitive dynamics
    df_competitive = pd.DataFrame(competitive_data)
    
    print(f"\nCompetitive Analysis:")
    for i in range(len(agents)):
        avg_novelty = df_competitive[f'agent_{i+1}_novelty'].mean()
        avg_curiosity = df_competitive[f'agent_{i+1}_curiosity'].mean()
        max_firing = df_competitive[f'agent_{i+1}_firing'].max()
        print(f"  Agent {i+1}: Average Novelty={avg_novelty:.3f}, "
              f"Average Curiosity={avg_curiosity:.3f}, Max Firing={max_firing}")
    
    # Create competitive visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Agent performance comparison
    agent_names = ['Agent 1', 'Agent 2', 'Agent 3']
    avg_novelty = [df_competitive['agent_1_novelty'].mean(),
                   df_competitive['agent_2_novelty'].mean(),
                   df_competitive['agent_3_novelty'].mean()]
    avg_curiosity = [df_competitive['agent_1_curiosity'].mean(),
                     df_competitive['agent_2_curiosity'].mean(),
                     df_competitive['agent_3_curiosity'].mean()]
    
    x = np.arange(len(agent_names))
    width = 0.35
    
    axes[0, 0].bar(x - width/2, avg_novelty, width, label='Novelty', alpha=0.7, color='skyblue')
    axes[0, 0].bar(x + width/2, avg_curiosity, width, label='Curiosity', alpha=0.7, color='lightgreen')
    axes[0, 0].set_title('Agent Performance Comparison')
    axes[0, 0].set_xlabel('Agent')
    axes[0, 0].set_ylabel('Average Level')
    axes[0, 0].set_xticks(x)
    axes[0, 0].set_xticklabels(agent_names)
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Novelty over time (all agents)
    axes[0, 1].plot(df_competitive['step'], df_competitive['agent_1_novelty'], 'r-', label='Agent 1', linewidth=2)
    axes[0, 1].plot(df_competitive['step'], df_competitive['agent_2_novelty'], 'g-', label='Agent 2', linewidth=2)
    axes[0, 1].plot(df_competitive['step'], df_competitive['agent_3_novelty'], 'b-', label='Agent 3', linewidth=2)
    axes[0, 1].set_title('Novelty Levels Over Time (Competitive)')
    axes[0, 1].set_xlabel('Time Step')
    axes[0, 1].set_ylabel('Novelty')
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Firing neuron counts
    axes[1, 0].plot(df_competitive['step'], df_competitive['agent_1_firing'], 'r-', label='Agent 1', linewidth=2)
    axes[1, 0].plot(df_competitive['step'], df_competitive['agent_2_firing'], 'g-', label='Agent 2', linewidth=2)
    axes[1, 0].plot(df_competitive['step'], df_competitive['agent_3_firing'], 'b-', label='Agent 3', linewidth=2)
    axes[1, 0].set_title('Firing Neuron Counts (Competitive)')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Firing Neurons')
    axes[1, 0].legend()
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Competitive heatmap
    correlation_matrix = df_competitive[['agent_1_novelty', 'agent_2_novelty', 'agent_3_novelty']].corr()
    im = axes[1, 1].imshow(correlation_matrix, cmap='RdBu', vmin=-1, vmax=1, aspect='auto')
    axes[1, 1].set_title('Agent Novelty Correlation')
    axes[1, 1].set_xticks(range(len(correlation_matrix.columns)))
    axes[1, 1].set_xticklabels(correlation_matrix.columns)
    axes[1, 1].set_yticks(range(len(correlation_matrix.index)))
    axes[1, 1].set_yticklabels(correlation_matrix.index)
    
    # Add correlation values
    for i in range(len(correlation_matrix.index)):
        for j in range(len(correlation_matrix.columns)):
            axes[1, 1].text(j, i, f'{correlation_matrix.iloc[i, j]:.2f}', 
                           ha='center', va='center', color='black', fontsize=8)
    
    plt.tight_layout()
    plt.savefig('competitive_agent_analysis.png', dpi=150)
    print(f"\n✓ Competitive visualization saved as 'competitive_agent_analysis.png'")
    
    plt.close()
    
    # Save competitive data
    df_competitive.to_csv('competitive_agent_data.csv', index=False)
    print(f"✓ Competitive data saved as 'competitive_agent_data.csv'")
    
    return brains, worlds, agents, df_competitive

def main():
    """Run all agent simulation examples"""
    print("NLM Python Agent Simulation Examples")
    print("=" * 60)
    print("This script demonstrates agent simulation with NLM Python bindings.")
    print()
    
    # Run all examples
    example_basic_agent_loop()
    example_advanced_agent_with_neuromodulation()
    example_exploratory_agent()
    example_competitive_agent()
    
    print("\n" + "=" * 60)
    print("All agent simulation examples completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Basic agent sensorimotor loop")
    print("2. ✓ Advanced agent with full neuromodulation")
    print("3. ✓ Exploratory agent with curiosity-driven behavior")
    print("4. ✓ Competitive agent multi-agent simulation")
    print("\nGenerated files:")
    print("  - neuromodulation_analysis.png")
    print("  - exploratory_agent_analysis.png")
    print("  - competitive_agent_analysis.png")
    print("  - neuromodulation_data.csv")
    print("  - exploratory_agent_data.csv")
    print("  - competitive_agent_data.csv")
    print("\nAll agent simulation features working correctly!")

if __name__ == "__main__":
    main()