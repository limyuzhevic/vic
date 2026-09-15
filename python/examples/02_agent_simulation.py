#!/usr/bin/env python3
"""
Example: Complete Agent-Environment Simulation

This example demonstrates a complete agent-environment simulation where
an NLM brain learns to interact with a dynamic world.

The agent learns through:
- Reward modulation (dopamine-based learning)
- Curiosity-driven exploration
- Structural plasticity (growing new connections)
- Developmental changes over time

Expected output: "Agent simulation completed!"
"""

import pynlm

def run_agent_simulation(config=None):
    """
    Run a complete agent-environment simulation.
    
    Args:
        config: Optional configuration dictionary with custom settings
    
    Returns:
        Dictionary with simulation results and metrics
    """
    if config is None:
        config = {}
    
    print("=== NLM Agent-Environment Example ===")
    print("Creating agent brain with learning capabilities...")
    
    # Step 1: Create brain and configure for learning
    print("\n1. Setting up learning brain...")
    brain_config = pynlm.createDefaultConfig()
    
    # Configure for learning
    brain_config.set("neuron_count", config.get('neuron_count', 500))
    brain_config.set("region_count", config.get('region_count', 2))
    brain_config.set("connection_probability", config.get('connection_prob', 0.15))
    
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Step 2: Create world environment
    print("\n2. Creating simulated world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=config.get('world_width', 20),
        height=config.get('world_height', 20),
        vision_width=config.get('vision_width', 8),
        vision_height=config.get('vision_height', 8)
    )
    world.reset()
    world.setAgentStart(10.0f, 10.0f)
    print(f"   ✓ World created: {world.getWidth()}x{world.getHeight()} cells")
    
    # Step 3: Create agent interface
    print("\n3. Creating agent brain interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   ✓ Agent interface created")
    
    # Step 4: Enable advanced learning features
    print("\n4. Enabling advanced learning systems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("   ✓ Reward modulation: ENABLED")
    print("   ✓ Structural plasticity: ENABLED")
    print("   ✓ Development: ENABLED")
    print("   ✓ Curiosity: ENABLED")
    
    # Step 5: Run simulation
    print("\n5. Running simulation...")
    num_steps = config.get('num_steps', 200)
    
    # Track metrics
    metrics = {
        'steps': [],
        'rewards': [],
        'firing_rates': [],
        'neuron_counts': [],
        'development_stages': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward (positive for resource collection, negative for hazards)
        # This is a simplified reward calculation
        body = world.getAgentBody()
        reward = 0.0f
        if body.energy > world.getMaxEnergy() * 0.8:
            reward += 1.0f  # Reward for having energy
        elif body.health < world.getMaxEnergy() * 0.2:
            reward -= 1.0f  # Penalty for low health
            
        # Apply reward-based learning
        agent.applyRewardModulation(reward, 0.0f)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        metrics['steps'].append(step)
        metrics['rewards'].append(reward)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['neuron_counts'].append(brain.getFiringNeuronCount())
        metrics['development_stages'].append(agent.getDevelopmentalStage())
        
        # Print progress
        if step % 50 == 0:
            print(f"   Step {step:3d}/{num_steps}: Reward={reward:+.1f}, "
                  f"Firing={brain.getAverageFiringRate():.2f}Hz, "
                  f"Development={agent.getDevelopmentalStage()}")
    
    # Step 6: Collect results
    print("\n6. Simulation Results:")
    avg_reward = sum(metrics['rewards']) / len(metrics['rewards']) if metrics['rewards'] else 0
    avg_firing_rate = sum(metrics['firing_rates']) / len(metrics['firing_rates']) if metrics['firing_rates'] else 0
    final_stage = metrics['development_stages'][-1] if metrics['development_stages'] else agent.getDevelopmentalStage()
    
    results = {
        'total_steps': num_steps,
        'total_reward': sum(metrics['rewards']),
        'average_reward': avg_reward,
        'average_firing_rate': avg_firing_rate,
        'final_development_stage': final_stage,
        'final_neurons_firing': metrics['neuron_counts'][-1] if metrics['neuron_counts'] else 0,
        'final_total_spikes': brain.getTotalSpikeCount(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    }
    
    print(f"   Total reward: {results['total_reward']:.2f}")
    print(f"   Average reward per step: {results['average_reward']:.3f}")
    print(f"   Average firing rate: {results['average_firing_rate']:.2f} Hz")
    print(f"   Final developmental stage: {results['final_development_stage']}")
    print(f"   Neuromodulation level: {results['neuromodulation_level']:.3f}")
    print(f"   Curiosity level: {results['curiosity_level']:.3f}")
    
    print("\n" + "="*50)
    print("✓ AGENT SIMULATION COMPLETED!")
    print("="*50)
    
    return results, metrics

def main():
    # Custom configuration for the simulation
    config = {
        'neuron_count': 300,
        'region_count': 2,
        'connection_prob': 0.12,
        'world_width': 15,
        'world_height': 15,
        'vision_width': 6,
        'vision_height': 6,
        'num_steps': 200
    }
    
    # Run simulation
    results, metrics = run_agent_simulation(config)
    
    # Save results to file (optional)
    print(f"\nSimulation completed with {results['total_reward']:.2f} total reward")
    
    return 0

if __name__ == "__main__":
    exit(main())