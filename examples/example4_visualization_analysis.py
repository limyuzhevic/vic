#!/usr/bin/env python3
"""
Example 4: Visualization and Analysis
Demonstrates visualization and analysis capabilities using the NLM visualization tools.
"""

import pynlm
from nlm.visualization import plot_neural_dynamics, plot_agent_performance
from nlm.analysis import PerformanceAnalyzer
import matplotlib.pyplot as plt
import time

def visualization_analysis_example():
    """Example of visualization and analysis."""
    print("=== NLM Visualization and Analysis Example ===")
    
    # Create and train a simple agent
    print("Creating and training agent...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Collect data for visualization
    simulation_data = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'firing_rates': [],
        'times': [],
    }
    
    print("Running simulation...")
    start_time = time.time()
    
    for step in range(100):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        internal = percept.getInternal()
        reward = internal[0] if internal else 0.0
        
        simulation_data['steps'].append(step)
        simulation_data['rewards'].append(reward)
        simulation_data['curiosity'].append(agent.getCuriosityLevel())
        simulation_data['novelty'].append(agent.getNoveltyLevel())
        simulation_data['firing_rates'].append(brain.getAverageFiringRate())
        simulation_data['times'].append(world.getSimulationTime())
        
        # Check if simulation ended
        if world.getAgentBody().health <= 0:
            break
    
    end_time = time.time()
    print(f"✓ Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Create visualizations
    print("\nCreating visualizations...")
    
    # 1. Brain dynamics visualization
    fig1 = plot_neural_dynamics(brain)
    fig1.suptitle('Neural Dynamics Visualization', fontsize=16)
    plt.tight_layout()
    
    # Save brain dynamics plot
    plt.figure(fig1.number)
    plt.savefig('brain_dynamics.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("✓ Brain dynamics plot saved to brain_dynamics.png")
    
    # 2. Agent performance visualization
    fig2, metrics = plot_agent_performance(agent, world, num_steps=100)
    fig2.suptitle('Agent Performance Analysis', fontsize=16)
    plt.tight_layout()
    
    # Save agent performance plot
    plt.figure(fig2.number)
    plt.savefig('agent_performance.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("✓ Agent performance plot saved to agent_performance.png")
    
    # 3. World dynamics visualization
    from nlm.visualization import plot_world_dynamics
    fig3 = plot_world_dynamics(world, duration=50, timestep=0.1)
    fig3.suptitle('World Dynamics Visualization', fontsize=16)
    plt.tight_layout()
    
    # Save world dynamics plot
    plt.figure(fig3.number)
    plt.savefig('world_dynamics.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("✓ World dynamics plot saved to world_dynamics.png")
    
    # Perform advanced analysis
    print("\nPerforming advanced analysis...")
    
    # Create performance analyzer
    analyzer = PerformanceAnalyzer()
    
    # Create training-like results for analysis
    training_results = {
        'episode_rewards': simulation_data['rewards'],
        'episode_curiosity': simulation_data['curiosity'],
        'episode_novelty': simulation_data['novelty'],
        'episode_firing_rates': simulation_data['firing_rates'],
    }
    
    # Analyze performance
    analysis = analyzer.analyze_training(training_results)
    
    print("\n=== Analysis Results ===")
    
    print("Training Dynamics:")
    for key, value in analysis['training_dynamics'].items():
        print(f"  {key.replace('_', ' ').title()}: {value:.3f}")
    
    print(f"\nPerformance Trajectory:")
    reward_trend = analysis['performance_trajectory']['reward_trend']
    print(f"  Reward Trend: {reward_trend['trend']}")
    print(f"  Reward Rate: {reward_trend['rate']:.4f}")
    print(f"  R-squared: {reward_trend['r_squared']:.3f}")
    
    print(f"\nLearning Insights:")
    for insight in analysis['learning_insights']:
        print(f"  • {insight}")
    
    print(f"\nOptimization Recommendations:")
    for recommendation in analysis['optimization_recommendations']:
        print(f"  • {recommendation}")
    
    # Create comprehensive report
    print("\n=== Comprehensive Analysis Report ===")
    
    report = f"""
NLM Simulation Analysis Report
================================

Simulation Summary:
  Duration: {end_time - start_time:.2f} seconds
  Steps Completed: {len(simulation_data['steps'])}
  Average Reward: {np.mean(simulation_data['rewards']):.3f}
  Average Curiosity: {np.mean(simulation_data['curiosity']):.3f}
  Average Novelty: {np.mean(simulation_data['novelty']):.3f}
  Average Firing Rate: {np.mean(simulation_data['firing_rates']):.2f} Hz

Performance Assessment:
  Convergence Rate: {analysis['training_dynamics']['convergence_rate']:.3f}
  Stability Score: {analysis['training_dynamics']['stability_score']:.3f}
  Exploration Efficiency: {analysis['training_dynamics']['exploration_efficiency']:.3f}
  Neural Stability: {analysis['training_dynamics']['neural_stability']:.3f}

Learning Quality:
  Reward Learning: {"Excellent" if analysis['performance_trajectory']['reward_trend']['trend'] == 'increasing' else "Needs Improvement"}
  Exploration Quality: {"Good" if analysis['training_dynamics']['exploration_efficiency'] > 0.5 else "Fair" if analysis['training_dynamics']['exploration_efficiency'] > 0.3 else "Poor"}
  Neural Stability: {"Excellent" if analysis['training_dynamics']['neural_stability'] > 0.8 else "Good" if analysis['training_dynamics']['neural_stability'] > 0.6 else "Needs Improvement"}

Recommendations:
{"\n  ".join('  • ' + rec for rec in analysis['optimization_recommendations'])}

Files Generated:
  • brain_dynamics.png - Neural activity and firing rate visualization
  • agent_performance.png - Agent learning progress and exploration analysis
  • world_dynamics.png - Environmental interaction and movement patterns
"""
    
    # Save report
    with open('analysis_report.txt', 'w') as f:
        f.write(report)
    
    print("✓ Comprehensive analysis report saved to analysis_report.txt")
    
    print(f"\n✓ Visualization and analysis completed successfully!")
    print(f"\nAll visualization files have been saved in the current directory.")
    
    return simulation_data, analysis

if __name__ == "__main__":
    print("NLM Visualization and Analysis Examples")
    print("=" * 60)
    
    # Run visualization example
    data, analysis = visualization_analysis_example()
    
    print("\n" + "="*60)
    print("Visualization and analysis example completed successfully!")
