"""
NLM (Neural Learning Machine) - Learning Curve Analysis Example

This example demonstrates learning curve analysis techniques for tracking
neural learning progress, stability, and performance over time.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm
import time
import statistics

def calculate_learning_metrics(brain, window_size=10):
    """Calculate comprehensive learning metrics"""
    metrics = {}
    
    # Basic learning indicators
    learning = pynlm.analyze_learning_progress(brain, window_size)
    metrics['learning_rate'] = learning.totalLearning
    metrics['learning_steps'] = learning.learningSteps
    metrics['learning_stability'] = abs(learning.totalLearning - 0.5)  # How close to stable state
    
    # Activity metrics
    stats = pynlm.get_activity_statistics(brain)
    metrics['average_firing_rate'] = stats.averageFiringRate
    metrics['max_firing_neurons'] = stats.maxFiringNeurons
    metrics['total_spikes'] = stats.spikeCount
    metrics['activity_variability'] = stats.activityVariability
    
    # Network metrics
    network = pynlm.analyze_network_connectivity(brain)
    metrics['network_efficiency'] = network.excitationInhibitionRatio
    metrics['synapse_density'] = network.averageWeight
    
    # Development metrics
    metrics['developmental_stage'] = brain.getDevelopmentalStage()
    
    return metrics

def plot_learning_curves(episode_data, save_file=None):
    """Generate learning curves from episode data"""
    print("\nLearning Curve Analysis")
    print("=" * 40)
    
    if not episode_data:
        print("No data available for analysis")
        return
    
    # Extract metrics over time
    episodes = [data['episode'] for data in episode_data]
    learning_rates = [data['learning_rate'] for data in episode_data]
    firing_rates = [data['average_firing_rate'] for data in episode_data]
    total_spikes = [data['total_spikes'] for data in episode_data]
    
    print(f"Analysis over {len(episode_data)} episodes:")
    print("-" * 50)
    
    # Calculate statistics
    print(f"Learning Rate Statistics:")
    print(f"  Mean: {statistics.mean(learning_rates):.3f}")
    print(f"  Std:  {statistics.stdev(learning_rates) if len(learning_rates) > 1 else 0:.3f}")
    print(f"  Min:  {min(learning_rates):.3f}")
    print(f"  Max:  {max(learning_rates):.3f}")
    
    print(f"\nFiring Rate Statistics:")
    print(f"  Mean: {statistics.mean(firing_rates):.3f} Hz")
    print(f"  Std:  {statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0:.3f} Hz")
    print(f"  Min:  {min(firing_rates):.3f} Hz")
    print(f"  Max:  {max(firing_rates):.3f} Hz")
    
    print(f"\nTotal Spikes Statistics:")
    print(f"  Mean: {statistics.mean(total_spikes):.0f}")
    print(f"  Std:  {statistics.stdev(total_spikes) if len(total_spikes) > 1 else 0:.0f}")
    
    # Learning trend analysis
    if len(learning_rates) > 5:
        first_half = learning_rates[:len(learning_rates)//2]
        second_half = learning_rates[len(learning_rates)//2:]
        
        first_avg = statistics.mean(first_half)
        second_avg = statistics.mean(second_half)
        
        print(f"\nLearning Trend Analysis:")
        print(f"  First half average: {first_avg:.3f}")
        print(f"  Second half average: {second_avg:.3f}")
        
        if second_avg > first_avg * 1.1:
            print(f"  Trend: Learning is IMPROVING")
        elif second_avg < first_avg * 0.9:
            print(f"  Trend: Learning is DECLINING")
        else:
            print(f"  Trend: Learning is STABLE")
    
    # Performance assessment
    final_learning = learning_rates[-1] if learning_rates else 0
    final_firing = firing_rates[-1] if firing_rates else 0
    final_spikes = total_spikes[-1] if total_spikes else 0
    
    print(f"\nFinal Performance Assessment:")
    print(f"  Final learning rate: {final_learning:.3f}")
    print(f"  Final firing rate: {final_firing:.3f} Hz")
    print(f"  Final total spikes: {final_spikes}")
    
    # Determine performance category
    if final_learning > 0.7 and final_firing > 5 and final_spikes > 100:
        print(f"  Performance: EXCELLENT")
    elif final_learning > 0.4 and final_firing > 2 and final_spikes > 50:
        print(f"  Performance: GOOD")
    elif final_learning > 0.2 and final_firing > 1 and final_spikes > 20:
        print(f"  Performance: ACCEPTABLE")
    else:
        print(f"  Performance: NEEDS IMPROVEMENT")
    
    return episode_data

def analyze_learning_stability(episode_data):
    """Analyze learning stability and convergence"""
    if len(episode_data) < 5:
        return
    
    print(f"\nLearning Stability Analysis")
    print("-" * 40)
    
    learning_rates = [data['learning_rate'] for data in episode_data]
    firing_rates = [data['average_firing_rate'] for data in episode_data]
    
    # Calculate volatility
    if len(learning_rates) > 1:
        learning_volatility = statistics.stdev(learning_rates)
        firing_volatility = statistics.stdev(firing_rates)
        
        print(f"Learning Volatility: {learning_volatility:.3f}")
        print(f"Firing Rate Volatility: {firing_volatility:.3f}")
        
        # Stability assessment
        if learning_volatility < 0.1 and firing_volatility < 1:
            print(f"Stability: HIGH - Learning is stable")
        elif learning_volatility < 0.3 and firing_volatility < 3:
            print(f"Stability: MODERATE - Some fluctuations")
        else:
            print(f"Stability: LOW - High volatility")
    
    # Check for convergence
    if len(learning_rates) >= 5:
        recent_rates = learning_rates[-5:]
        mean_recent = statistics.mean(recent_rates)
        std_recent = statistics.stdev(recent_rates) if len(recent_rates) > 1 else 0
        
        print(f"\nConvergence Analysis (last 5 episodes):")
        print(f"  Mean: {mean_recent:.3f}")
        print(f"  Std: {std_recent:.3f}")
        
        # Coefficient of variation
        cv = std_recent / mean_recent if mean_recent > 0 else 0
        print(f"  Coefficient of Variation: {cv:.3f}")
        
        if cv < 0.1:
            print(f"  Convergence: Strong")
        elif cv < 0.3:
            print(f"  Convergence: Moderate")
        else:
            print(f"  Convergence: Weak")


def main():
    print("NLM Learning Curve Analysis Example")
    print("=" * 60)
    print("This example demonstrates learning curve analysis techniques")
    print("for tracking neural learning progress and stability.")
    
    # Create experiment data
    episode_data = []
    
    print("\n1. Running learning experiments...")
    for episode in range(20):
        # Create fresh brain for each episode
        config = pynlm.create_advanced_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with some learning dynamics
        for step in range(50):
            brain.step(step)
            
            # Add some variation to make it interesting
            if episode % 5 == 0 and step % 10 == 0:
                # Simulate learning bursts
                pass
        
        # Collect metrics
        metrics = calculate_learning_metrics(brain, 10)
        metrics['episode'] = episode + 1
        
        episode_data.append(metrics)
        
        if (episode + 1) % 5 == 0:
            print(f"  Completed {episode + 1}/20 episodes")
    
    # Analyze learning curves
    plot_learning_curves(episode_data)
    
    # Analyze stability
    analyze_learning_stability(episode_data)
    
    # Demonstrate comparative learning
    print(f"\n2. Comparative learning analysis...")
    print("-" * 50)
    
    # Compare different configurations
    configs = [
        ("Default", pynlm.createDefaultConfig()),
        ("Advanced", pynlm.create_advanced_config()),
        ("Development", pynlm.create_development_config(1.2, 1.1))
    ]
    
    comparison_data = []
    
    for name, config in configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run longer simulation for comparison
        for step in range(100):
            brain.step(step)
        
        metrics = calculate_learning_metrics(brain, 20)
        metrics['config_name'] = name
        comparison_data.append(metrics)
        
        print(f"  {name:12} config: Learning={metrics['learning_rate']:.3f}, "
              f"Firing={metrics['average_firing_rate']:.3f} Hz, "
              f"Spikes={metrics['total_spikes']}")
    
    # Summary statistics
    print(f"\n3. Configuration Comparison Summary:")
    print("-" * 40)
    
    for data in comparison_data:
        name = data['config_name']
        learning = data['learning_rate']
        firing = data['average_firing_rate']
        spikes = data['total_spikes']
        
        print(f"{name:12}: Learning rate={learning:.3f}, "
              f"Firing rate={firing:.3f} Hz, Total spikes={spikes}")
    
    # Performance ranking
    print(f"\n4. Performance Ranking:")
    print("-" * 30)
    
    ranked = sorted(comparison_data, key=lambda x: x['learning_rate'], reverse=True)
    
    for i, data in enumerate(ranked, 1):
        name = data['config_name']
        learning = data['learning_rate']
        performance = 'EXCELLENT' if learning > 0.7 else 'GOOD' if learning > 0.4 else 'NEEDS WORK'
        
        print(f"{i}. {name:12}: Learning rate={learning:.3f} ({performance})")
    
    print("\n✅ Learning curve analysis example completed successfully!")
    print("\nKey techniques demonstrated:")
    print("  • Learning metrics calculation")
    print("  • Learning curve generation")
    print("  • Stability analysis")
    print("  • Configuration comparison")
    print("  • Performance ranking")
    print("  • Trend analysis")
    print("  • Convergence assessment")

if __name__ == "__main__":
    main()
