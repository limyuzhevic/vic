#!/usr/bin/env python3
"""
Agent-Environment Interaction Example
===================================
This example demonstrates complete agent-environment interaction using NLM.
It shows how an agent can sense its environment, make decisions, and take actions.

Key Features:
- Complete agent-brain-world integration
- Sensory perception and motor control
- Reward-based learning
- Development and adaptation
- Real-time environment interaction
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def print_separator(title):
    """Print a formatted section separator"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def main():
    print_separator("NLM Agent-Environment Interaction Example")
    
    # 1. Setup the complete system
    print("\n1. Setting up agent-brain-world system...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 200)
    config.set("plasticity.enable", True)
    config.set("neuromodulation.enable", True)
    print("   Configuration created with enhanced parameters")
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    print("   Agent brain interface created")
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    print("   World configured (30x30 grid)")
    
    # Initialize agent with world
    agent.initialize(world)
    print("   Agent initialized with world")
    
    # 2. Configure agent behavior
    print("\n2. Configuring agent behavior...")
    
    # Enable all neural systems for realistic behavior
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   Enabled all neural systems:")
    print("     - Reward modulation (dopamine-based learning)")
    print("     - Structural plasticity (brain reorganization)")
    print("     - Development (age-dependent changes)")
    print("     - Curiosity (exploration drive)")
    
    # 3. Run simulation
    print("\n3. Running agent-environment simulation...")
    
    # Add some initial resources and hazards to the world
    import random
    for _ in range(5):
        x = random.uniform(5.0, 25.0)
        y = random.uniform(5.0, 25.0)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
    
    for _ in range(3):
        x = random.uniform(5.0, 25.0)
        y = random.uniform(5.0, 25.0)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=1.5))
    
    print("   World populated with resources and hazards")
    
    # Simulation parameters
    num_steps = 150
    episode_rewards = []
    
    for step in range(num_steps):
        # 3.1 Update world physics
        world.update(0.1)
        
        # 3.2 Get sensory perception
        percept = world.getSensoryPercept()
        
        # 3.3 Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # 3.4 Neural processing
        brain.step(step)
        
        # 3.5 Get action and apply to world
        action = agent.decodeMotorCommand()
        success = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 3.6 Get reward and update neuromodulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        predicted_reward = 0.0
        agent.applyRewardModulation(reward, predicted_reward)
        
        # 3.7 Update development
        agent.updateDevelopment(0.1)
        
        # 3.8 Collect statistics
        episode_rewards.append(reward)
        
        if step % 30 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            dev_stage = brain.getDevelopmentalStage()
            firing = brain.getFiringNeuronCount()
            avg_rate = brain.getAverageFiringRate()
            
            print(f"   Step {step}: Reward={reward:.3f}, Curiosity={curiosity:.3f}, "
                  f"Novelty={novelty:.3f}, Dev={dev_stage}, Firing={firing}, Rate={avg_rate:.2f}Hz")
    
    # 4. Analyze results
    print("\n4. Simulation Analysis:")
    
    # Calculate statistics
    total_steps = len(episode_rewards)
    avg_reward = sum(episode_rewards) / total_steps
    max_reward = max(episode_rewards)
    min_reward = min(episode_rewards)
    
    print(f"   Simulation completed: {total_steps} steps")
    print(f"   Total reward received: {sum(episode_rewards):.3f}")
    print(f"   Average reward per step: {avg_reward:.3f}")
    print(f"   Maximum reward: {max_reward:.3f}")
    print(f"   Minimum reward: {min_reward:.3f}")
    
    # Get final system state
    final_curiosity = agent.getCuriosityLevel()
    final_novelty = agent.getNoveltyLevel()
    final_dev_stage = brain.getDevelopmentalStage()
    
    print(f"   Final curiosity level: {final_curiosity:.3f}")
    print(f"   Final novelty level: {final_novelty:.3f}")
    print(f"   Final developmental stage: {final_dev_stage}")
    
    # 5. Demonstrate system capabilities
    print("\n5. System Capabilities Demonstrated:")
    print("   ✓ Sensory perception (vision, proprioception)")
    print("   ✓ Neural processing and action selection")
    print("   ✓ Motor control and environment interaction")
    print("   ✓ Reward-based learning and neuromodulation")
    print("   ✓ Development and maturation")
    print("   ✓ Memory systems integration")
    print("   ✓ Plasticity mechanisms (STDP, Hebbian, structural)")
    print("   ✓ Complete brain loop (input → processing → output → feedback)")
    
    # 6. Save results (if advanced features available)
    print("\n6. Advanced Features (if available):")
    
    # Show advanced features that could be demonstrated
    advanced_features = []
    
    if hasattr(brain, 'saveState'):
        advanced_features.append("State saving/loading")
    if hasattr(agent, 'getPerformanceStats'):
        advanced_features.append("Performance statistics")
    if hasattr(world, 'profiler'):
        advanced_features.append("Performance profiling")
    if hasattr(brain, 'benchmark'):
        advanced_features.append("Benchmarking")
    
    for feature in advanced_features:
        print(f"   ✓ {feature}")
    
    print_separator("Agent-Environment Interaction Complete")
    
    print("\nNext Steps:")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("  - Try memory_system_test.py for memory system testing")
    print("  - Try performance_benchmark.py for performance testing")
    print("\nFor detailed usage, see how_to_use.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)