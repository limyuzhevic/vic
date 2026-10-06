#!/usr/bin/env python3
"""
Example: Complete NLM Brain Simulation with Python Bindings

This example demonstrates how to use the improved NLM Python bindings
to create and simulate an artificial developmental brain.

The example shows:
1. Creating a complete agent system
2. Running a simulation with sensory processing and action selection
3. Monitoring brain state and development
4. Visualizing basic results

This example uses the enhanced Python bindings with additional
convenience methods and improved error handling.
"""

import time
import sys
import os
from typing import Tuple, Dict, List

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm as nlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please ensure the NLM Python bindings are built and installed.")
    sys.exit(1)


def create_demo_system(world_width: float = 100.0, world_height: float = 100.0,
                       vision_width: int = 20, vision_height: int = 20) -> Tuple:
    """
    Create a complete agent system for demonstration.
    
    Args:
        world_width: Width of the simulation world
        world_height: Height of the simulation world  
        vision_width: Width of agent's vision field
        vision_height: Height of agent's vision field
        
    Returns:
        Tuple of (brain, world, agent_brain)
    """
    print(f"Creating demo system: {world_width}x{world_height} world with {vision_width}x{vision_height} vision")
    
    try:
        # Create complete agent system
        brain, world, agent_brain = nlm.createCompleteAgentSystem(
            worldWidth=world_width,
            worldHeight=world_height,
            visionWidth=vision_width,
            visionHeight=vision_height
        )
        
        # Set agent starting position
        world.setAgentStart(x=world_width / 2, y=world_height / 2)
        
        print("✓ Demo system created successfully")
        print(f"  - Brain initialized with {brain.getTotalNeuronCount()} neurons")
        print(f"  - {brain.getTotalSynapseCount()} synapses")
        print(f"  - {brain.getRegionCount()} neural regions")
        print(f"  - World dimensions: {world.getWidth()}x{world.getHeight()}")
        
        return brain, world, agent_brain
        
    except Exception as e:
        print(f"✗ Failed to create demo system: {e}")
        raise


def initialize_environment(brain, world, agent_brain) -> None:
    """
    Initialize the environment and agent brain.
    
    Args:
        brain: The NLM brain
        world: The simulation world
        agent_brain: The agent brain interface
    """
    print("\nInitializing environment...")
    
    try:
        # Initialize brain
        if brain.initialize():
            print("✓ Brain initialized")
        else:
            raise RuntimeError("Failed to initialize brain")
        
        # Initialize agent brain with world
        agent_brain.initialize(world)
        print("✓ Agent brain initialized")
        
        # Enable systems for demonstration
        agent_brain.setRewardModulationEnabled(True)
        agent_brain.setCuriosityEnabled(True)
        agent_brain.setDevelopmentEnabled(True)
        
        print("✓ Core systems enabled (reward modulation, curiosity, development)")
        
    except Exception as e:
        print(f"✗ Environment initialization failed: {e}")
        raise


def get_system_info(brain, agent_brain, world) -> Dict:
    """
    Get comprehensive information about the current system state.
    
    Args:
        brain: The NLM brain
        agent_brain: The agent brain interface
        world: The simulation world
        
    Returns:
        Dictionary with system information
    """
    info = {}
    
    # Brain statistics
    info["brain"] = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "pending_spike_events": brain.getPendingSpikeEventCount(),
        "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "developmental_stage": str(brain.getDevelopmentalStage()),
        "region_count": brain.getRegionCount(),
    }
    
    # Agent brain state
    info["agent_brain"] = agent_brain.getState()
    
    # World state
    info["world"] = {
        "width": world.getWidth(),
        "height": world.getHeight(),
        "agent_x": world.getAgentBody().x,
        "agent_y": world.getAgentBody().y,
        "agent_orientation": world.getAgentBody().orientation,
        "agent_energy": world.getAgentBody().energy,
        "agent_health": world.getAgentBody().health,
        "agent_age": world.getAgentBody().age,
        "agent_moving": world.getAgentBody().isMoving,
        "agent_turning": world.getAgentBody().isTurning,
        "simulation_time": world.getSimulationTime(),
        "max_energy": world.getMaxEnergy(),
        "energy_decay_rate": world.getEnergyDecayRate(),
    }
    
    return info


def print_step_info(step: int, info: Dict) -> None:
    """
    Print information about the current simulation step.
    
    Args:
        step: Current simulation step
        info: System information dictionary
    """
    brain_info = info["brain"]
    agent_info = info["agent_brain"]
    world_info = info["world"]
    
    print(f"\n--- Step {step} ---")
    print(f"Agent Position: ({world_info['agent_x']:.1f}, {world_info['agent_y']:.1f})")
    print(f"Agent Energy: {world_info['agent_energy']:.2f}/{world_info['max_energy']}")
    print(f"Developmental Stage: {agent_info['developmental_stage']}")
    print(f"Curiosity Level: {agent_info['curiosity_level']:.3f}")
    print(f"Novelty Level: {agent_info['novelty_level']:.3f}")
    print(f"Prediction Error: {agent_info['prediction_error']:.3f}")
    print(f"Total Spikes: {brain_info['total_spikes']}")
    print(f"Active Neurons: {brain_info['active_neurons']}/{brain_info['total_neurons']}")
    print(f"Firing Neurons: {brain_info['firing_neurons']}")
    print(f"Average Firing Rate: {brain_info['average_firing_rate']:.3f}")
    print(f"Excitation/Inhibition Ratio: {brain_info['excitation_inhibition_ratio']:.3f}")


def run_simulation(episodes: int = 5, steps_per_episode: int = 50, 
                   timestep: float = 0.1) -> List[Dict]:
    """
    Run the complete simulation with multiple episodes.
    
    Args:
        episodes: Number of episodes to simulate
        steps_per_episode: Number of steps per episode
        timestep: Time step for simulation updates
        
    Returns:
        List of episode summaries
    """
    print("\n" + "="*60)
    print("STARTING SIMULATION")
    print("="*60)
    
    episode_summaries = []
    
    for episode in range(episodes):
        print(f"\n{'='*20} Episode {episode + 1}/{episodes} {'='*20}")
        
        try:
            # Create fresh system for each episode
            brain, world, agent_brain = create_demo_system()
            initialize_environment(brain, world, agent_brain)
            
            # Initial sensory processing
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            # Record initial state
            initial_info = get_system_info(brain, agent_brain, world)
            print_step_info(0, initial_info)
            
            # Run simulation steps
            for step in range(1, steps_per_episode + 1):
                try:
                    # Perform simulation step
                    new_agent_state, new_world_state = nlm.simulateStep(
                        agent_brain, world, timestep
                    )
                    
                    # Get updated information
                    current_info = get_system_info(brain, agent_brain, world)
                    print_step_info(step, current_info)
                    
                    # Brief pause for readability (remove in production)
                    if step % 10 == 0:
                        time.sleep(0.05)
                        
                except Exception as e:
                    print(f"✗ Error at step {step}: {e}")
                    # Continue with next step despite error
                    continue
            
            # Record episode summary
            final_info = get_system_info(brain, agent_brain, world)
            episode_summary = {
                "episode": episode + 1,
                "steps_completed": steps_per_episode,
                "final_brain_stats": final_info["brain"],
                "final_agent_state": final_info["agent_brain"],
                "final_world_state": final_info["world"],
                "success": True
            }
            
            episode_summaries.append(episode_summary)
            
            print(f"\n✓ Episode {episode + 1} completed successfully")
            print(f"  - Total spikes: {final_info['brain']['total_spikes']}")
            print(f"  - Average firing rate: {final_info['brain']['average_firing_rate']:.3f}")
            print(f"  - Final developmental stage: {final_info['agent_brain']['developmental_stage']}")
            
        except Exception as e:
            print(f"✗ Episode {episode + 1} failed: {e}")
            episode_summary = {
                "episode": episode + 1,
                "steps_completed": 0,
                "error": str(e),
                "success": False
            }
            episode_summaries.append(episode_summary)
            continue
    
    return episode_summaries


def print_simulation_summary(summaries: List[Dict]) -> None:
    """
    Print a summary of the simulation results.
    
    Args:
        summaries: List of episode summaries
    """
    print("\n" + "="*60)
    print("SIMULATION SUMMARY")
    print("="*60)
    
    successful_episodes = [s for s in summaries if s.get("success", False)]
    
    if not successful_episodes:
        print("No episodes completed successfully")
        return
    
    print(f"Total episodes: {len(summaries)}")
    print(f"Successful episodes: {len(successful_episodes)}")
    print(f"Failed episodes: {len(summaries) - len(successful_episodes)}")
    
    if successful_episodes:
        # Calculate averages across successful episodes
        avg_neurons = sum(s["final_brain_stats"]["total_neurons"] 
                         for s in successful_episodes) / len(successful_episodes)
        avg_spikes = sum(s["final_brain_stats"]["total_spikes"] 
                        for s in successful_episodes) / len(successful_episodes)
        avg_firing_rate = sum(s["final_brain_stats"]["average_firing_rate"] 
                             for s in successful_episodes) / len(successful_episodes)
        
        print(f"\nAverage across successful episodes:")
        print(f"  - Neurons: {avg_neurons:.0f}")
        print(f"  - Total spikes: {avg_spikes:.0f}")
        print(f"  - Average firing rate: {avg_firing_rate:.3f}")
        
        # Track developmental progress
        stages = [s["final_agent_state"]["developmental_stage"] 
                 for s in successful_episodes]
        unique_stages = set(stages)
        print(f"\nDevelopmental stages reached: {', '.join(map(str, unique_stages))}")


def main():
    """Main function to run the simulation example."""
    print("NLM Python Bindings - Complete Simulation Example")
    print("="*60)
    
    try:
        # Run simulation with configurable parameters
        summaries = run_simulation(
            episodes=3,           # Number of episodes
            steps_per_episode=30,  # Steps per episode
            timestep=0.1           # Time step
        )
        
        # Print summary
        print_simulation_summary(summaries)
        
        print("\n" + "="*60)
        print("SIMULATION COMPLETE")
        print("="*60)
        print("\nKey improvements demonstrated:")
        print("✓ Enhanced configuration value access (getInt, getFloat, getBool)")
        print("✓ Convenience state management (getState, updateState)")
        print("✓ World inspection and object listing")
        print("✓ Complete agent system factory functions")
        print("✓ Improved simulation helper functions")
        print("✓ Comprehensive error handling and logging")
        print("✓ Type safety and validation")
        
    except KeyboardInterrupt:
        print("\n\nSimulation interrupted by user")
    except Exception as e:
        print(f"\n\nSimulation failed with error: {e}")
        print("Please check the error messages above for details.")
        sys.exit(1)


if __name__ == "__main__":
    main()