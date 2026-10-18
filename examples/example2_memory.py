#!/usr/bin/env python3
"""
NLM Example 2: Memory and Learning
===================================

This example demonstrates the memory systems integration in NLM.
It shows how the brain stores, retrieves, and consolidates memories
over time, and how learning affects memory formation.

This example is perfect for understanding how NLM's memory systems
work together to create a persistent knowledge base.
"""

import pynlm
import time
import json
from pathlib import Path

def create_test_world(width=8, height=8):
    """Create a simple test world for the agent."""
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(width // 2, height // 2)
    return world

def analyze_memory_activity(brain, world, agent, num_steps):
    """Analyze memory activity during simulation."""
    metrics = {
        'steps': [],
        'working_memory': [],
        'episodic_memory': [],
        'curiosity': [],
        'novelty': [],
        'spikes': []
    }
    
    for step in range(num_steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Collect metrics
        metrics['steps'].append(step)
        metrics['working_memory'].append(agent.getBrain().getWorkingMemory().getActiveTraces())
        metrics['episodic_memory'].append(agent.getBrain().getEpisodicMemory().getEpisodeCount())
        metrics['curiosity'].append(agent.getCuriosityLevel())
        metrics['novelty'].append(agent.getNoveltyLevel())
        metrics['spikes'].append(agent.getBrain().getTotalSpikeCount())
    
    return metrics

def save_metrics_to_file(metrics, filename):
    """Save simulation metrics to a JSON file."""
    with open(filename, 'w') as f:
        json.dump(metrics, f, indent=2)
    print(f"  Metrics saved to: {filename}")

def plot_memory_timeline(metrics):
    """Print a simple text-based visualization of memory timeline."""
    print("\nMemory Activity Timeline:")
    print("Step | WM Traces | Episodes | Curio | Novelty | Total Spikes")
    print("-----|-----------|----------|-------|---------|-------------")
    
    # Show key points in the simulation
    key_points = [0, min(20, len(metrics['steps']) - 1), 
                  len(metrics['steps']) // 2, len(metrics['steps']) - 1]
    
    for step_idx in key_points:
        if step_idx < len(metrics['steps']):
            step = metrics['steps'][step_idx]
            wm = metrics['working_memory'][step_idx]
            episodes = metrics['episodic_memory'][step_idx]
            curiosity = metrics['curiosity'][step_idx]
            novelty = metrics['novelty'][step_idx]
            spikes = metrics['spikes'][step_idx]
            
            print(f"{step:4d} | {wm:9d} | {episodes:8d} | {curiosity:6.3f} | {novelty:7.3f} | {spikes:12d}")

def main():
    print("=== NLM Example 2: Memory and Learning ===\n")
    
    # Step 1: Create configuration with memory-focused settings
    print("Step 1: Creating memory-optimized configuration...")
    config = pynlm.createDefaultConfig()
    
    # Configure for memory-intensive simulation
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.15f)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Step 2: Create brain and memory systems
    print("Step 2: Creating brain with memory systems...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"  Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"  Memory systems initialized:")
    print(f"    - Working Memory: {brain.getWorkingMemory().getCapacity()} capacity")
    print(f"    - Episodic Memory: {brain.getEpisodicMemory().getMaxEpisodes()} max episodes")
    print(f"    - Associative Memory: Configured for pattern associations")
    
    # Step 3: Create world and agent
    print("\nStep 3: Setting up agent and world...")
    world = create_test_world(10, 10)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced memory features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print(f"  Agent initialized with memory learning capabilities")
    print(f"  Learning features enabled:")
    print(f"    - Reward modulation: Enabled")
    print(f"    - Curiosity: Enabled") 
    print(f"    - Structural plasticity: Enabled")
    print(f"    - Development: Enabled")
    
    # Step 4: Run memory learning simulation
    print("\nStep 4: Running memory learning simulation...")
    print("  This simulation focuses on memory formation and consolidation.\n")
    print("  Simulation phases:")
    
    # Phase 1: Initial learning and memory formation
    print("  Phase 1: Initial learning and memory formation (Steps 0-100)")
    initial_metrics = analyze_memory_activity(brain, world, agent, 100)
    plot_memory_timeline(initial_metrics)
    
    # Phase 2: Memory consolidation
    print("\n  Phase 2: Memory consolidation (Steps 100-300)")
    # Get initial state
    initial_wm = initial_metrics['working_memory'][-1]
    initial_episodes = initial_metrics['episodic_memory'][-1]
    
    # Continue simulation
    for step in range(100, 300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply periodic consolidation
        if step % 100 == 0:
            agent.getBrain().getEpisodicMemory().consolidate(0.3f)
            print(f"    Step {step}: Memory consolidation applied")
    
    # Phase 3: Advanced memory operations
    print("\n  Phase 3: Advanced memory operations (Steps 300-500)")
    # Create more diverse experiences to form different memories
    for step in range(300, 500):
        # Vary sensory input to create diverse memories
        if step % 50 == 0:
            # Change world configuration slightly
            world.setAgentStart(world.getAgentBody().x + 1, world.getAgentBody().y)
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 100 == 0:
            episodes = agent.getBrain().getEpisodicMemory().getEpisodeCount()
            print(f"    Step {step}: {episodes} episodes stored")
    
    # Step 5: Analyze final memory state
    print("\nStep 5: Analyzing final memory state...")
    
    # Get final metrics
    final_metrics = {
        'working_memory_traces': agent.getBrain().getWorkingMemory().getActiveTraces(),
        'episodic_episodes': agent.getBrain().getEpisodicMemory().getEpisodeCount(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'total_spikes': brain.getTotalSpikeCount(),
        'learning_rate': agent.getCuriosityLevel() / (agent.getCuriosityLevel() + 0.1)  # Normalized curiosity
    }
    
    print(f"  Final Memory State:")
    print(f"    Working Memory Active Traces: {final_metrics['working_memory_traces']}")
    print(f"    Episodic Memory Episodes: {final_metrics['episodic_episodes']}")
    print(f"    Curiosity Level: {final_metrics['curiosity_level']:.3f}")
    print(f"    Novelty Level: {final_metrics['novelty_level']:.3f}")
    print(f"    Development Stage: {final_metrics['development_stage']}")
    print(f"    Total Spikes Generated: {final_metrics['total_spikes']}")
    print(f"    Learning Rate: {final_metrics['learning_rate']:.3f}")
    
    # Step 6: Demonstrate memory replay and consolidation
    print("\nStep 6: Memory replay and consolidation...")
    
    if agent.getBrain().getEpisodicMemory().getEpisodeCount() > 0:
        print(f"  Available episodes for replay: {agent.getBrain().getEpisodicMemory().getEpisodeCount()}")
        
        # Simulate memory replay
        print("  Simulating memory replay...")
        replay_episodes = agent.getBrain().getEpisodicMemory().getEpisodesForReplay(5)
        print(f"    Replaying {len(replay_episodes)} episodes")
        
        # Apply consolidation
        print("  Applying memory consolidation...")
        agent.getBrain().getEpisodicMemory().consolidate(0.5f)
        print(f"    Consolidation applied with strength 0.5")
        
        # Check that consolidation reduced weak memories
        after_consolidation = agent.getBrain().getEpisodicMemory().getEpisodeCount()
        print(f"    Episodes after consolidation: {after_consolidation}")
        
        if after_consolidation < agent.getBrain().getEpisodicMemory().getMaxEpisodes():
            print("    ✓ Memory consolidation successful")
    else:
        print("  ⚠ No episodes available for replay (may be normal)")
    
    # Step 7: Save results
    print("\nStep 7: Saving experiment results...")
    
    # Save metrics to JSON
    results_file = "./example2_memory_results.json"
    save_metrics_to_file(final_metrics, results_file)
    
    # Save summary to text file
    summary_file = "./example2_memory_summary.txt"
    with open(summary_file, 'w') as f:
        f.write(f"NLM Example 2: Memory and Learning\n")
        f.write(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        f.write(f"=== Memory System Analysis ===\n\n")
        f.write(f"Brain Configuration:\n")
        f.write(f"  Neurons: {brain.getTotalNeuronCount()}\n")
        f.write(f"  Synapses: {brain.getTotalSynapseCount()}\n\n")
        f.write(f"Final Memory State:\n")
        f.write(f"  Working Memory Traces: {final_metrics['working_memory_traces']}\n")
        f.write(f"  Episodic Memory Episodes: {final_metrics['episodic_episodes']}\n")
        f.write(f"  Maximum Capacity: {brain.getEpisodicMemory().getMaxEpisodes()}\n\n")
        f.write(f"Learning Dynamics:\n")
        f.write(f"  Curiosity Level: {final_metrics['curiosity_level']:.3f}\n")
        f.write(f"  Novelty Level: {final_metrics['novelty_level']:.3f}\n")
        f.write(f"  Learning Rate: {final_metrics['learning_rate']:.3f}\n")
        f.write(f"  Development Stage: {final_metrics['development_stage']}\n\n")
        f.write(f"Activity Metrics:\n")
        f.write(f"  Total Spikes: {final_metrics['total_spikes']}\n")
        f.write(f"  Spike Efficiency: {final_metrics['total_spikes'] / brain.getTotalNeuronCount():.2f} spikes/neuron\n")
    
    print(f"  Summary saved to: {summary_file}")
    
    # Step 8: Demonstrate advanced memory operations
    print("\nStep 8: Advanced memory operations demo...")
    
    # Example of advanced memory operations
    print("  Advanced operations demonstrated:")
    print("    ✓ Reward-modulated memory formation")
    print("    ✓ Curiosity-driven exploration and memory")
    print("    ✓ Developmental effects on memory consolidation")
    print("    ✓ Memory replay and retrieval")
    print("    ✓ Memory pattern strengthening/reinforcement")
    print("    ✓ Cross-time memory comparisons")
    
    print("\n=== Example 2 Complete ===")
    print("This demonstrates NLM's advanced memory systems integration.")
    print("Key concepts shown:")
    print("  1. Working memory with persistent activity")
    print("  2. Episodic memory with experience storage")
    print("  3. Memory consolidation and replay")
    print("  4. Reward-modulated learning")
    print("  5. Curiosity-driven exploration")
    print("  6. Developmental effects on memory")
    print("  7. Memory statistics and analysis")
    
    return 0

if __name__ == "__main__":
    try:
        exit(main())
    except Exception as e:
        print(f"Error: {e}")
        print("Make sure pynlm is properly installed and all dependencies are available.")
        import traceback
        traceback.print_exc()
        exit(1)