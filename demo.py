#!/usr/bin/env python3
"""
NLM Python Interface Demonstration

This script demonstrates the key features of the improved NLM Python interface
and serves as a starting point for users to explore the library.
"""

import sys
import time
import pynlm

def print_header(title):
    """Print a formatted header."""
    print("\n" + "="*60)
    print(f"  {title}")
    print("="*60)

def example_1_simple_brain():
    """Example 1: Simple brain simulation."""
    print_header("Example 1: Simple Brain Simulation")
    
    print("Creating a simple brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    print("Initializing brain...")
    brain.initialize()
    
    print("Running simulation...")
    for i in range(10):
        brain.step(i)
    
    print(f"\nResults:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

def example_2_agent_world():
    """Example 2: Agent interacting with world."""
    print_header("Example 2: Agent-World Interaction")
    
    print("Setting up brain and world...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Running agent-world simulation...")
    for step in range(15):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            body = world.getAgentBody()
            stats = agent.get_stats()
            print(f"  Step {step}: Pos=({body.x:.1f},{body.y:.1f}), "
                  f"Action={action}, Neuromod={stats[0]:.3f}")
    
    print(f"\nFinal Results:")
    print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain, world, agent

def example_3_learning_agent():
    """Example 3: Learning agent with rewards."""
    print_header("Example 3: Learning Agent")
    
    print("Creating learning agent configuration...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("plasticity_rate", 1.2)
    config.set("enable_development", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enable_all_features()
    
    print("Running learning simulation...")
    for episode in range(3):
        print(f"\nEpisode {episode + 1}:")
        
        agent.reset()
        agent.initialize(world)  # Re-initialize with new world state
        
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simulate reward (e.g., getting closer to center)
            body = world.getAgentBody()
            distance_to_center = abs(body.x - 10) + abs(body.y - 10)
            reward = max(0, 1.0 - distance_to_center / 20.0)
            
            predicted = 0.0  # Simple prediction
            agent.applyRewardModulation(reward, predicted)
            
            if step % 5 == 0:
                stats = agent.get_stats()
                print(f"    Step {step}: Reward={reward:.3f}, "
                      f"Stage={stats[4]}, Neuromod={stats[0]:.3f}")
    
    print(f"\nLearning Results:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final developmental stage: {agent.getDevelopmentalStage()}")
    
    return brain, agent

def example_4_performance_monitor():
    """Example 4: Performance monitoring."""
    print_header("Example 4: Performance Monitoring")
    
    print("Starting performance monitoring...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enable_all_features()
    
    start_time = time.time()
    steps_completed = 0
    
    print("Running performance test (30 steps)...")
    
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        steps_completed += 1
        
        if step % 10 == 0:
            elapsed = time.time() - start_time
            steps_per_sec = steps_completed / elapsed if elapsed > 0 else 0
            print(f"  Step {step}: {steps_per_sec:.1f} steps/sec, "
                  f"spikes={brain.getTotalSpikeCount()}")
    
    elapsed_time = time.time() - start_time
    steps_per_sec = steps_completed / elapsed_time if elapsed_time > 0 else 0
    
    print(f"\nPerformance Results:")
    print(f"  Total steps: {steps_completed}")
    print(f"  Time elapsed: {elapsed_time:.2f} seconds")
    print(f"  Performance: {steps_per_sec:.1f} steps/sec")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain, agent

def example_6_visualization():
    """Example 6: Visualization and debugging."""
    print_header("Example 6: Visualization and Debugging")
    
    print("Creating brain and visualization interface...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create visualization interface
    visualization = pynlm.VisualizationInterface()
    
    # Initialize visualization
    if visualization.initialize():
        print("✓ Visualization initialized successfully")
        visualization.setUpdateRate(10.0)  # Update rate in Hz
        
        print("Running simulation with visualization...")
        for step in range(10):
            brain.step(step)
            visualization.update()
            visualization.render()
            
            if step % 5 == 0:
                print(f"  Step {step}: Visualization active={visualization.isActive()}")
        
        visualization.close()
        print("✓ Visualization closed")
    else:
        print("⚠ Visualization not available (Phase 2 implementation)")
        
        # Simulate visualization without actual graphics
        for step in range(5):
            brain.step(step)
            print(f"  Step {step}: [Simulation running - visualization placeholder]")
    
    print(f"\nVisualization Results:")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain, visualization

def main():
    """Run demonstration examples."""
    print("🚀 NLM Python Interface Demonstration")
    print("Running basic examples to showcase NLM capabilities...")
    
    try:
        # Run examples
        example_1_simple_brain()
        example_2_agent_world()
        example_3_learning_agent()
        example_4_performance_monitor()
        example_6_visualization()
        example_5_demo_summary()
        
        print("\n" + "="*60)
        print("✅ All demonstrations completed successfully!")
        print("="*60)
        print("\nThe NLM Python interface is working correctly.")
        print("\nNext steps:")
        print("  1. Read README_PYTHON.md for detailed documentation")
        print("  2. Explore examples/python_examples.md for comprehensive examples")
        print("  3. Start your own NLM experiments!")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Error during demonstration: {e}")
        print(f"\nStack trace:")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())