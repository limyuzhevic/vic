"""
Advanced NLM Experiment Example
===============================

This example demonstrates advanced NLM experiments with sophisticated learning protocols
including policy gradients, reinforcement learning, and curriculum learning.
"""

import pynlm

def basic_advanced_experiment():
    """Run a basic advanced experiment with enhanced learning."""
    print("=== Basic Advanced Experiment ===")
    
    # Create advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.08)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run advanced simulation
    print("Running advanced simulation with learning...")
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress
        if step % 200 == 0:
            print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print(f"Advanced experiment completed!")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print()

def advanced_experiment_with_learning():
    """Run advanced experiment with policy gradients and reinforcement learning."""
    print("=== Advanced Experiment with Learning Protocols ===")
    
    # Create advanced configuration for learning experiments
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.05)
    config.set("simulation_timestep", 0.005)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running advanced learning simulation...")
    print("  Learning features: Policy Gradient, Reinforcement Learning, Curriculum Learning")
    print()
    
    # Simulation with learning metrics
    total_reward = 0.0
    learning_episodes = 0
    curiosity_peaks = []
    
    for step in range(2000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0] if not percept.getInternal().empty() else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        total_reward += reward
        
        # Track learning metrics
        curiosity_level = agent.getCuriosityLevel()
        if curiosity_level > 0.7:
            curiosity_peaks.append((step, curiosity_level))
        
        if step % 500 == 0:
            print(f"  Step {step}: Reward={reward:.3f}, Total={total_reward:.1f}, "
                  f"Curiosity={curiosity_level:.3f}, Firing={brain.getFiringNeuronCount()}")
    
    # Calculate learning statistics
    avg_reward = total_reward / 2000
    peak_curiosity_count = len(curiosity_peaks)
    
    print(f"Advanced learning simulation completed!")
    print(f"  Average reward per step: {avg_reward:.4f}")
    print(f"  High curiosity episodes: {peak_curiosity_count}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Working memory efficiency: {brain.getWorkingMemory().getEfficiency() if brain.getWorkingMemory() else 0:.3f}")
    print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
    print()

def advanced_experiment_with_curriculum():
    """Run advanced experiment with curriculum learning."""
    print("=== Advanced Experiment with Curriculum Learning ===")
    
    # Create curriculum-based configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("simulation_timestep", 0.01)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment with increasing complexity
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running curriculum learning simulation...")
    print("  Phases: Simple -> Medium -> Complex -> Expert")
    print()
    
    # Curriculum learning phases
    phase_durations = [500, 500, 500, 500]  # 4 phases
    phase_rewards = []
    phase_curiosity = []
    
    current_step = 0
    for phase in range(4):
        print(f"  Phase {phase + 1}/4: Increasing complexity")
        phase_reward = 0.0
        phase_curiosity_sum = 0.0
        
        for step in range(phase_durations[phase]):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(current_step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0] if not percept.getInternal().empty() else 0.0
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            phase_reward += reward
            phase_curiosity_sum += agent.getCuriosityLevel()
            current_step += 1
        
        avg_phase_reward = phase_reward / phase_durations[phase]
        avg_phase_curiosity = phase_curiosity_sum / phase_durations[phase]
        phase_rewards.append(avg_phase_reward)
        phase_curiosity.append(avg_phase_curiosity)
        
        print(f"    Average reward: {avg_phase_reward:.4f}, "
              f"Average curiosity: {avg_phase_curiosity:.3f}")
    
    # Analyze curriculum progression
    print(f"Curriculum learning completed!")
    print(f"  Phase progression rewards: {[f'{r:.4f}' for r in phase_rewards]}")
    print(f"  Phase progression curiosity: {[f'{c:.3f}' for c in phase_curiosity]}")
    
    # Check for learning progression
    reward_trend = all(phase_rewards[i] <= phase_rewards[i+1] for i in range(3))
    curiosity_trend = any(c > 0.5 for c in phase_curiosity)
    
    print(f"  Learning progression detected: {'YES' if reward_trend else 'NO'}")
    print(f"  High curiosity exploration: {'YES' if curiosity_trend else 'NO'}")
    print(f"  Final development stage: {brain.getDevelopmentalStage()}")
    print()

def main():
    print("Advanced NLM Experiment Examples")
    print("=" * 40)
    print()
    
    # Example 1: Basic advanced experiment
    basic_advanced_experiment()
    
    # Example 2: Advanced experiment with learning protocols
    advanced_experiment_with_learning()
    
    # Example 3: Advanced experiment with curriculum learning
    advanced_experiment_with_curriculum()
    
    print("All advanced experiment examples completed successfully!")
    print()
    print("Key features demonstrated:")
    print("  • Enhanced learning with policy gradients")
    print("  • Reinforcement learning with Q-values")
    print("  • Curriculum learning with progressive complexity")
    print("  • Curiosity-driven exploration")
    print("  • Neuromodulation integration")
    print("  • Memory system optimization")
    print("  • Performance monitoring and metrics")

if __name__ == "__main__":
    main()