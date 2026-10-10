#!/usr/bin/env python3
"""
NLM agent simulation example.

This example demonstrates complete agent simulation with brain and world interaction.
"""

import pynlm
from pynlm.advanced_features import get_profiler

def run_agent_simulation(num_steps=500):
    print("=== NLM Agent Simulation Example ===")
    
    # Start profiling
    profiler = get_profiler()
    profiler.start_profiling("agent_simulation", {"steps": num_steps})
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        print(f"Created config with {num_steps} simulation steps")
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        print("World configured and reset")
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("Learning subsystems enabled")
        
        # Run simulation loop
        for step in range(num_steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Get final statistics
        final_stats = {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
        
        print(f"\n=== Simulation Complete ===")
        for key, value in final_stats.items():
            print(f"  {key}: {value}")
        
        # Generate performance report
        results = profiler.stop_profiling()
        print(f"\nPerformance: {results['duration']:.2f}s, "
              f"{results['steps_per_second']:.0f} steps/s")
        
        return final_stats
        
    except Exception as e:
        profiler.stop_profiling()
        print(f"Error during simulation: {e}")
        raise

if __name__ == "__main__":
    run_agent_simulation(500)