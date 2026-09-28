# NLM - Easy Usage Guide for Beginners

## Advanced Usage

For advanced users who want to explore the full capabilities of NLM, here are some comprehensive examples that combine multiple features:

### Advanced Agent Training with Curiosity and Development

This example demonstrates a complete agent training cycle with curiosity-driven exploration and developmental stages:

```python
import pynlm
import json
import matplotlib.pyplot as plt

def train_advanced_agent(num_episodes=100):
    """Train an advanced agent with exploration and development."""
    
    # Create exploration-optimized configuration
    config = pynlm.createExplorationConfig()
    
    # Create brain, agent, and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllFeatures()  # Enable curiosity, development, plasticity
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15, 15)
    agent.initialize(world)
    
    print("Starting advanced agent training...")
    print("Configuration: Exploration-focused with curiosity and development")
    
    for episode in range(num_episodes):
        # Reset for new episode
        agent.reset()
        brain.reset()
        world.reset()
        
        # Run episode with statistics collection
        episode_stats = {
            'steps': [],
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'final_development_stage': agent.getDevelopmentalStage()
        }
        
        for step in range(500):
            # Update world
            world.update(0.1)
            
            # Get percept
            percept = world.getSensoryPercept()
            
            # Process and think
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Agent decides action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward from internal state
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record statistics
            episode_stats['steps'].append({
                'step': step,
                'action': str(action),
                'reward': reward,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': agent.getDevelopmentalStage()
            })
            
            episode_stats['total_reward'] += reward
            episode_stats['max_curiosity'] = max(
                episode_stats['max_curiosity'], 
                agent.getCuriosityLevel()
            )
        
        # Print episode summary
        if episode % 10 == 0:
            print(f"Episode {episode}: "
                  f"Total Reward={episode_stats['total_reward']:.2f}, "
                  f"Max Curiosity={episode_stats['max_curiosity']:.3f}, "
                  f"Final Stage={episode_stats['final_development_stage']}")
    
    print("Advanced training completed!")
    return episode_stats

# Run the training
results = train_advanced_agent(50)
print(f"Average reward per episode: {results['total_reward']/len(results['steps']):.2f}")
```

### Experiment Comparison with Multiple Configurations

This example shows how to compare different agent configurations:

```python
import pynlm

def compare_configurations():
    """Compare different agent configurations."""
    
    configurations = [
        ("Exploration", pynlm.createExplorationConfig),
        ("Exploitation", pynlm.createExploitationConfig),
        ("Balanced", pynlm.createDefaultConfig)
    ]
    
    results = {}
    
    for name, config_func in configurations:
        print(f"\nTesting configuration: {name}")
        
        # Create brain and agent
        config = config_func()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllFeatures()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10, 10)
        agent.initialize(world)
        
        # Run simulation
        total_reward = 0.0
        curiosity_levels = []
        novelty_levels = []
        
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            total_reward += reward
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
        
        # Calculate statistics
        results[name] = {
            'total_reward': total_reward,
            'average_reward': total_reward / 200,
            'max_curiosity': max(curiosity_levels),
            'max_novelty': max(novelty_levels),
            'final_stage': agent.getDevelopmentalStage(),
            'brain_stats': pynlm.getBrainStatistics(brain),
            'agent_stats': pynlm.getAgentStatistics(agent)
        }
        
        print(f"  Total Reward: {total_reward:.2f}")
        print(f"  Average Curiosity: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
        print(f"  Maximum Novelty: {max(novelty_levels):.3f}")
    
    # Compare results
    print("\nConfiguration Comparison:")
    print("-" * 60)
    print(f"{'Configuration':<15} {'Total Reward':<15} {'Avg Curiosity':<15} {'Max Novelty':<15} {'Final Stage':<15}")
    print("-" * 60)
    
    for name, stats in results.items():
        print(f"{name:<15} {stats['total_reward']:<15.2f} {stats['max_curiosity']:<15.3f} {stats['max_novelty']:<15.3f} {stats['final_stage']:<15}")
    
    return results

# Run comparison
comparison_results = compare_configurations()
```

### Advanced Statistics and Monitoring

This example demonstrates advanced statistics collection and monitoring:

```python
import pynlm
import json
import time
from datetime import datetime

def advanced_monitoring_session(duration_minutes=5):
    """Run a monitoring session with comprehensive statistics collection."""
    
    # Create brain
    config = pynlm.createExplorationConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllFeatures()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(20, 20)
    agent.initialize(world)
    
    # Monitoring data
    monitoring_data = {
        'session_start': datetime.now().isoformat(),
        'config': config.summary(),
        'samples': [],
        'brain_history': [],
        'agent_history': []
    }
    
    start_time = time.time()
    sample_interval = 0.5  # Sample every 0.5 seconds
    next_sample_time = start_time + sample_interval
    
    print("Starting advanced monitoring session...")
    print(f"Duration: {duration_minutes} minutes")
    print("Sampling interval:", sample_interval, "seconds")
    
    while time.time() - start_time < duration_minutes * 60:
        # Update simulation
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(int(time.time() - start_time))
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect data if sampling time
        current_time = time.time()
        if current_time >= next_sample_time:
            sample = {
                'timestamp': current_time - start_time,
                'action': str(action),
                'reward': reward,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'development_stage': str(agent.getDevelopmentalStage()),
                'brain_stats': pynlm.getBrainStatistics(brain),
                'agent_stats': pynlm.getAgentStatistics(agent)
            }
            
            monitoring_data['samples'].append(sample)
            monitoring_data['brain_history'].append(pynlm.getBrainStatistics(brain))
            monitoring_data['agent_history'].append(pynlm.getAgentStatistics(agent))
            
            # Log to console
            if len(monitoring_data['samples']) % 10 == 0:
                print(f"Sample {len(monitoring_data['samples'])}: "
                      f"Time={sample['timestamp']:.1f}s, "
                      f"Action={sample['action']}, "
                      f"Curiosity={sample['curiosity']:.3f}, "
                      f"Novelty={sample['novelty']:.3f}")
            
            next_sample_time += sample_interval
    
    monitoring_data['session_end'] = datetime.now().isoformat()
    
    # Calculate summary statistics
    if monitoring_data['samples']:
        final_sample = monitoring_data['samples'][-1]
        monitoring_data['summary'] = {
            'total_samples': len(monitoring_data['samples']),
            'duration': duration_minutes * 60,
            'samples_per_second': len(monitoring_data['samples']) / (duration_minutes * 60),
            'final_reward': final_sample['reward'],
            'average_curiosity': sum(s['curiosity'] for s in monitoring_data['samples']) / len(monitoring_data['samples']),
            'average_novelty': sum(s['novelty'] for s in monitoring_data['samples']) / len(monitoring_data['samples']),
            'final_development_stage': final_sample['development_stage'],
            'brain_final_stats': final_sample['brain_stats'],
            'agent_final_stats': final_sample['agent_stats']
        }
    
    # Save monitoring data
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"nlm_monitoring_{timestamp}.json"
    
    with open(filename, 'w') as f:
        json.dump(monitoring_data, f, indent=2)
    
    print(f"\nMonitoring session completed!")
    print(f"Data saved to {filename}")
    print(f"Total samples collected: {len(monitoring_data['samples'])}")
    
    if monitoring_data['summary']:
        summary = monitoring_data['summary']
        print(f"Average curiosity: {summary['average_curiosity']:.3f}")
        print(f"Average novelty: {summary['average_novelty']:.3f}")
        print(f"Final development stage: {summary['final_development_stage']}")
    
    return monitoring_data

# Run monitoring session
monitoring_results = advanced_monitoring_session(2)  # 2 minute session
```

### Integration with Advanced Features

This example shows how to integrate NLM with advanced Python features:

```python
import pynlm
import numpy as np
from collections import deque
import matplotlib.pyplot as plt

def advanced_integration_example():
    """Demonstrate advanced integration with external libraries."""
    
    # Create configuration
    config = pynlm.createExplorationConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllFeatures()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(12, 12)
    agent.initialize(world)
    
    # Advanced data structures for analysis
    action_history = deque(maxlen=1000)
    reward_history = deque(maxlen=1000)
    curiosity_history = deque(maxlen=1000)
    
    # Custom analysis function
    def analyze_pattern(history, window_size=50):
        """Analyze patterns in the data using sliding window."""
        if len(history) < window_size:
            return None
        
        recent_data = list(history)[-window_size:]
        
        # Calculate statistics
        mean_value = sum(recent_data) / len(recent_data)
        variance = sum((x - mean_value) ** 2 for x in recent_data) / len(recent_data)
        
        # Detect trends
        if len(recent_data) >= 2:
            trend = recent_data[-1] - recent_data[0]
        else:
            trend = 0
        
        return {
            'mean': mean_value,
            'variance': variance,
            'trend': trend,
            'min': min(recent_data),
            'max': max(recent_data)
        }
    
    # Run simulation
    print("Starting advanced integration example...")
    
    for step in range(300):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record data
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        action_history.append(action)
        reward_history.append(reward)
        curiosity_history.append(agent.getCuriosityLevel())
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Periodic analysis
        if step % 50 == 0 and step > 0:
            action_analysis = analyze_pattern(reward_history, 25)
            curiosity_analysis = analyze_pattern(curiosity_history, 25)
            
            print(f"Step {step}: Analysis complete")
            print(f"  Reward stats: {action_analysis}")
            print(f"  Curiosity stats: {curiosity_analysis}")
            
            # Detect interesting patterns
            if action_analysis and action_analysis['trend'] > 0.1:
                print("  -> Increasing reward trend detected!")
            
            if curiosity_analysis and curiosity_analysis['variance'] > 0.2:
                print("  -> High curiosity variance - exploring new patterns!")
    
    # Advanced visualization (commented out to avoid display issues)
    # def create_visualizations():
    #     fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))
    #     
    #     # Plot reward history
    #     ax1.plot(reward_history)
    #     ax1.set_title('Reward History')
    #     ax1.set_xlabel('Step')
    #     ax1.set_ylabel('Reward')
    #     
    #     # Plot curiosity history
    #     ax2.plot(curiosity_history)
    #     ax2.set_title('Curiosity Level')
    #     ax2.set_xlabel('Step')
    #     ax2.set_ylabel('Curiosity')
    #     
    #     # Action distribution
    #     action_counts = {}
    #     for action in action_history:
    #         action_counts[action] = action_counts.get(action, 0) + 1
    #     
    #     actions = list(action_counts.keys())
    #     counts = list(action_counts.values())
    #     ax3.bar(actions, counts)
    #     ax3.set_title('Action Distribution')
    #     ax3.set_xlabel('Action')
    #     ax3.set_ylabel('Count')
    #     
    #     plt.tight_layout()
    #     plt.savefig('nlm_advanced_analysis.png', dpi=150)
    #     print("Visualization saved to nlm_advanced_analysis.png")
    # 
    # create_visualizations()
    
    # Calculate final statistics
    final_action_analysis = analyze_pattern(reward_history, min(100, len(reward_history)))
    final_curiosity_analysis = analyze_pattern(curiosity_history, min(100, len(curiosity_history)))
    
    print("\nAdvanced integration analysis complete!")
    print(f"Final reward stats: {final_action_analysis}")
    print(f"Final curiosity stats: {final_curiosity_analysis}")
    
    # Identify key behaviors
    if final_action_analysis:
        if final_action_analysis['mean'] > 0.5:
            print("Key finding: High average reward indicates effective exploitation")
        elif final_action_analysis['variance'] > 0.3:
            print("Key finding: High reward variance indicates exploration behavior")
    
    return {
        'action_history': list(action_history),
        'reward_history': list(reward_history),
        'curiosity_history': list(curiosity_history),
        'final_analysis': {
            'action_stats': final_action_analysis,
            'curiosity_stats': final_curiosity_analysis
        }
    }

# Run advanced integration example
advanced_results = advanced_integration_example()
```
