#!/usr/bin/env python3
"""
NLM Python Example: Advanced Features Demo

This example demonstrates advanced NLM features including learning,
development, and complex agent behaviors.
"""

import pynlm
import time
import json

def advanced_features_example():
    """Demonstrate advanced NLM features"""
    print("=== NLM Advanced Features Example ===")
    
    # Step 1: Create development-oriented configuration
    config = pynlm.createPresetConfig("learning")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Setup complete with advanced features enabled")
    print(f"  Learning system: Reward modulation enabled")
    print(f"  Brain growth: Structural plasticity enabled")
    print(f"  Development: Developmental stages enabled")
    print(f"  Exploration: Curiosity-driven exploration enabled")
    
    # Step 2: Create sample environment
    print(f"\nCreating sample environment...")
    world.addResource(5.0, 5.0, 20.0)    # Resource with value 20
    world.addResource(25.0, 25.0, 30.0)  # Resource with value 30
    world.addHazard(10.0, 10.0, -10.0)   # Hazard with damage -10
    world.addHazard(20.0, 20.0, -15.0)  # Hazard with damage -15
    print(f"  Added 2 resources and 2 hazards to environment")
    
    # Step 3: Run exploration learning simulation
    print(f"\nRunning exploration learning simulation...")
    exploration_stats = {
        'steps': 0,
        'episodes': 0,
        'total_reward': 0.0,
        'curiosity_peaks': 0,
        'spike_counts': [],
        'development_stages': []
    }
    
    for episode in range(3):  # Run 3 episodes
        print(f"\n--- Episode {episode + 1} ---")
        
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step + episode * 100)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            result = world.applyMotorCommand(action, world.getSimulationTime())
            reward = result.reward if result else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Track statistics
            exploration_stats['steps'] += 1
            exploration_stats['total_reward'] += reward
            exploration_stats['spike_counts'].append(brain.getTotalSpikeCount())
            exploration_stats['development_stages'].append(agent.getDevelopmentalStage())
            
            # Log progress
            if step % 25 == 0:
                curiosity = agent.getCuriosityLevel()
                if curiosity > 0.8:
                    exploration_stats['curiosity_peaks'] += 1
                    print(f"  Step {step}: HIGH curiosity ({curiosity:.3f}) - exploring!")
                else:
                    print(f"  Step {step}: Curiosity {curiosity:.3f}")
        
        exploration_stats['episodes'] += 1
        print(f"  Episode completed: Reward {reward:.2f}")
    
    # Step 4: Analyze learning results
    print(f"\n=== Learning Analysis ===")
    
    # Get brain statistics using convenience method
    brain_stats = brain.getStats()
    print(f"Brain Statistics:")
    print(f"  Total neurons: {brain_stats['neuron_count']}")
    print(f"  Total synapses: {brain_stats['synapse_count']}")
    print(f"  Current firing neurons: {brain_stats['firing_neuron_count']}")
    print(f"  Average firing rate: {brain_stats['average_firing_rate']:.2f} Hz")
    print(f"  Total spikes: {brain_stats['total_spikes']}")
    print(f"  Active neurons: {brain_stats['active_neurons']}")
    print(f"  E/I ratio: {brain_stats['ei_ratio']:.3f}")
    
    # Agent statistics using convenience method
    agent_stats = agent.getAgentStats()
    print(f"\nAgent Statistics:")
    print(f"  Curiosity level: {agent_stats['curiosity']:.3f}")
    print(f"  Novelty level: {agent_stats['novelty']:.3f}")
    print(f"  Prediction error: {agent_stats['prediction_error']:.3f}")
    print(f"  Neuromodulation level: {agent_stats['neuromodulation_level']:.3f}")
    print(f"  Developmental stage: {agent_stats['developmental_stage']}")
    
    # Learning metrics
    avg_spikes = sum(exploration_stats['spike_counts']) / len(exploration_stats['spike_counts'])
    max_curiosity = max(agent.getCuriosityLevel() for _ in range(10))  # placeholder
    
    print(f"\nLearning Metrics:")
    print(f"  Total exploration time: {exploration_stats['steps']} steps")
    print(f"  Total reward accumulated: {exploration_stats['total_reward']:.2f}")
    print(f"  Average curiosity peaks: {exploration_stats['curiosity_peaks']}")
    print(f"  Average spike count per step: {avg_spikes:.1f}")
    print(f"  Final developmental stage: {agent.getDevelopmentalStage()}")
    
    # Save learning session (if file writing is available)
    try:
        learning_data = {
            'brain_stats': brain_stats,
            'agent_stats': agent_stats,
            'exploration_stats': exploration_stats,
            'timestamp': time.time()
        }
        
        with open('learning_session.json', 'w') as f:
            json.dump(learning_data, f, indent=2)
        print(f"  Learning session saved to 'learning_session.json'")
    except Exception as e:
        print(f"  Note: Could not save learning session: {e}")
    
    return brain, world, agent, exploration_stats

if __name__ == "__main__":
    try:
        brain, world, agent, stats = advanced_features_example()
        print(f"\n✓ Advanced features example completed successfully!")
        print(f"  Total reward: {stats['total_reward']:.2f}")
        print(f"  Total spikes: {sum(stats['spike_counts'])}")
        print(f"  Curiosity peaks: {stats['curiosity_peaks']}")
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
