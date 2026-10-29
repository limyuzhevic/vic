#!/usr/bin/env python3
"""
Basic Usage Example for NLM Python Bindings

This example demonstrates the fundamental concepts of using the NLM (Neural Learning Machine)
Python API for neural simulation.
"""

import pynlm
import time

def basic_setup():
    """Create and configure a basic NLM brain."""
    print("=== Basic NLM Setup ===")
    
    # Create default configuration
    config = pynlm.Config()
    
    # Configure basic parameters
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Create brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    
    return brain

def simple_simulation(brain, steps=100):
    """Run a simple simulation loop."""
    print(f"\n=== Simple Simulation ({steps} steps) ===")
    
    for step in range(steps):
        current_time = step * 0.001  # timestep from config
        
        # Step the brain
        brain.step(step, current_time)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}: "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Active neurons: {brain.getActiveNeuronCount()}")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")

def sensory_motor_loop():
    """Demonstrate a complete sensory-motor loop."""
    print("\n=== Sensory-Motor Loop Example ===")
    
    # Create world and agent brain
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)  # width, height, vision width, vision height
    world.setAgentStart(50, 50)
    
    # Create brain and agent brain
    config = pynlm.Config()
    config.set("neuron_count", 200)
    config.set("region_count", 1)
    
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Initialize systems
    agent_brain.initialize(world)
    brain.initialize()
    
    print("Starting sensory-motor loop...")
    
    # Run episode
    for episode in range(3):
        print(f"\nEpisode {episode + 1}:")
        
        # Reset and run episode
        world.reset()
        steps = agent_brain.runEpisode(world, 200)
        
        print(f"  Steps completed: {steps}")
        print(f"  Final brain status:")
        print(f"    Total neurons: {brain.getTotalNeuronCount()}")
        print(f"    Total synapses: {brain.getTotalSynapseCount()}")
        print(f"    Total spikes: {brain.getTotalSpikeCount()}")
        print(f"    Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("Sensory-motor loop complete!")

def learning_demonstration():
    """Demonstrate learning and adaptation."""
    print("\n=== Learning Demonstration ===")
    
    # Create a learning-capable agent
    config = pynlm.Config()
    config.set("neuron_count", 300)
    config.set("region_count", 1)
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(80, 80, 16, 16)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable learning systems
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableRewardModulation(True)
    
    brain.initialize()
    
    print("Running learning demonstration...")
    print("This simulates how the brain learns from experience through:")
    print("  - Development (age-based changes)")
    print("  - Curiosity-driven exploration")
    print("  - Reward-based plasticity")
    
    # Create two different episodes with different rewards
    for episode in range(2):
        print(f"\nLearning episode {episode + 1}:")
        
        # Reset brain
        agent_brain.reset()
        brain.reset()
        brain.initialize()
        
        # Run with different conditions
        world.reset()
        agent_brain.runEpisode(world, 100)
        
        # Provide feedback
        reward = 1.0 if episode == 0 else 0.5
        prediction = reward * 0.8 if episode == 0 else 0.0
        
        agent_brain.applyRewardModulation(reward, prediction)
        agent_brain.updateEpisodeReward(reward)
        
        print(f"  Reward: {reward}, Prediction: {prediction}")
        print(f"  Dopamine level: {agent_brain.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
        print(f"  Developmental stage: {agent_brain.getDevelopmentalStage()}")
        print(f"  Plasticity modifier: {brain.getConfig().getOr<float>('plasticity_modifier', 1.0):.3f}")
    
    print("\nLearning demonstration complete!")

def advanced_features_demo():
    """Demonstrate advanced features."""
    print("\n=== Advanced Features Demo ===")
    
    # Create brain with monitoring
    config = pynlm.Config()
    config.set("neuron_count", 250)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(60, 60, 12, 12)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable advanced features
    agent_brain.enableDebugLogging(True)
    agent_brain.enablePerformanceMonitoring(True)
    agent_brain.setSimulationSpeed(0.5)  # Slow motion for observation
    
    brain.initialize()
    
    print("Running with advanced features enabled...")
    
    # Run episode with monitoring
    world.reset()
    steps = agent_brain.runEpisode(world, 150)
    
    # Get performance statistics
    stats = agent_brain.getPerformanceStats()
    
    print(f"\nPerformance Statistics:")
    print(f"  Total steps: {stats.totalSteps}")
    print(f"  Total time: {stats.totalTime:.3f}s")
    print(f"  Steps per second: {stats.avgStepsPerSecond:.1f}")
    print(f"  Average firing rate: {stats.avgFiringRate:.2f} Hz")
    print(f"  Maximum firing rate: {stats.maxFiringRate:.2f} Hz")
    print(f"  Episode reward: {stats.episodeReward:.3f}")
    print(f"  Episode novelty: {stats.episodeNovelty:.3f}")
    
    # Get action sequence
    action_seq = agent_brain.getActionSequence()
    print(f"\nAction sequence length: {len(action_seq)}")
    if action_seq:
        print(f"First few actions: {action_seq[:5]}")
    
    # Get comprehensive system status
    status = agent_brain.getSystemStatus()
    print(f"\nSystem Status:")
    print(f"  Simulation speed: {status.simulationSpeed}x")
    print(f"  Simulation paused: {status.simulationPaused}")
    print(f"  Developmental stage: {status.developmentalStage}")
    print(f"  Current neuromodulation: {status.currentNeuromodulation:.3f}")
    
    print("\nAdvanced features demonstration complete!")

def main():
    """Run all demonstrations."""
    print("NLM Python Bindings - Usage Examples")
    print("=" * 50)
    
    try:
        # Basic setup
        brain = basic_setup()
        
        # Simple simulation
        simple_simulation(brain)
        
        # Sensory-motor loop
        sensory_motor_loop()
        
        # Learning demonstration
        learning_demonstration()
        
        # Advanced features
        advanced_features_demo()
        
        print("\n" + "=" * 50)
        print("All demonstrations completed successfully!")
        print("\nKey takeaways:")
        print("1. NLM provides a complete neural simulation framework")
        print("2. The brain can learn and adapt through experience")
        print("3. Advanced features enable detailed monitoring and control")
        print("4. The API supports both simple and complex use cases")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
