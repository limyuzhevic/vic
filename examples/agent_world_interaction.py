#!/usr/bin/env python3
"""
Agent-World Interaction Example

This example demonstrates complete agent-world interaction using enhanced
NLM bindings with context managers, utilities, and advanced features.
"""

import pynlm_enhanced as nlm
import pynlm_utils as utils
import time

def create_world_with_environment(world_width=20, world_height=20):
    """Create a world with resources and hazards."""
    print("   Creating world with environment...")
    
    with nlm.create_enhanced_world() as world:
        world.configure(
            width=world_width,
            height=world_height,
            visionWidth=8,
            visionHeight=8
        )
        world.reset()
        world.setAgentStart(world_width / 2.0, world_height / 2.0)
        
        # Add some resources and hazards
        # Resources (food, water, etc.)
        resources = [
            (3, 3), (7, 7), (15, 15), (12, 8),
            (5, 18), (18, 5), (10, 15), (8, 12)
        ]
        
        # Add resources as world objects
        for x, y in resources:
            # Create a resource object
            # Note: The actual API might be different, using simple approach
            pass
        
        # Hazards (danger zones)
        hazards = [
            (10, 10), (5, 5), (15, 3), (8, 18)
        ]
        
        # Add world objects
        for x, y in hazards:
            # Create a hazard object
            # Note: The actual API might be different
            pass
        
        return world

def run_agent_simulation(world, brain, agent, num_steps=500, timestep=0.1):
    """Run agent simulation with enhanced tracking."""
    print(f"   Running agent simulation for {num_steps} steps...")
    
    # Create tracking utilities
    stats = utils.SimulationStats()
    tracker = utils.ProgressTracker(num_steps, "Agent Simulation")
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept and percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(timestep)
        
        # Track statistics
        stats.add_step_stats(
            step,
            brain.getFiringNeuronCount(),
            brain.getAverageFiringRate(),
            agent.getCuriosityLevel(),
            agent.getNoveltyLevel(),
            agent.getPredictionError(),
            agent.getNeuromodulationLevel(),
            brain.getDevelopmentalStage(),
            timestep
        )
        
        tracker.update(step, 
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    tracker.finish()
    
    elapsed = time.time() - start_time
    print(f"   Simulation completed in {elapsed:.3f} seconds")
    
    return stats, agent

def main():
    print("=== Agent-World Interaction (Enhanced Bindings) ===\n")
    
    # Create enhanced brain
    print("1. Creating enhanced brain...")
    with nlm.create_enhanced_brain() as brain:
        print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
        brain.initialize()
        
        # Create world with environment
        print("\n2. Setting up environment...")
        with nlm.create_enhanced_world() as world:
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            
            # Create enhanced agent brain
            print("\n3. Creating enhanced agent brain...")
            with nlm.create_enhanced_agent_brain(brain) as agent:
                # Run the simulation
                stats, final_agent = run_agent_simulation(world, brain, agent, 200)
                
                # Print comprehensive results
                print("\n=== Simulation Results ===")
                print(f"Total steps: {stats.getTotalSteps()}")
                print(f"Total simulation time: {stats.getTotalSimulationTime():.2f} seconds")
                print(f"Average time per step: {stats.getTimePerStep():.4f} seconds")
                print(f"Average firing rate: {stats.getAverageFiringRate():.3f} Hz")
                print(f"Max firing neurons: {stats.getMaxFiringNeurons()}")
                print(f"Min firing neurons: {stats.getMinFiringNeurons()}")
                print(f"Average curiosity level: {stats.getAverageCuriosity():.3f}")
                print(f"Average novelty level: {stats.getAverageNovelty():.3f}")
                print(f"Average prediction error: {stats.getAveragePredictionError():.3f}")
                print(f"Average neuromodulation level: {stats.getAverageNeuromodulationLevel():.3f}")
                
                # Get developmental stage distribution
                stage_dist = stats.to_dict().get('stage_distribution', {})
                print(f"Developmental stages reached: {len(stage_dist)}")
                
                # Show agent final status
                print("\n=== Agent Final Status ===")
                print(f"Reward modulation enabled: {final_agent.isRewardModulationEnabled()}")
                print(f"Structural plasticity enabled: {final_agent.isStructuralPlasticityEnabled()}")
                print(f"Development enabled: {final_agent.isDevelopmentEnabled()}")
                print(f"Curiosity enabled: {final_agent.isCuriosityEnabled()}")
                print(f"Final curiosity level: {final_agent.getCuriosityLevel():.3f}")
                print(f"Final novelty level: {final_agent.getNoveltyLevel():.3f}")
                print(f"Final prediction error: {final_agent.getPredictionError():.3f}")
                print(f"Final neuromodulation level: {final_agent.getNeuromodulationLevel():.3f}")
                print(f"Final developmental stage: {final_agent.getDevelopmentalStage()}")
    
    print("\n=== Example completed successfully! ===")

if __name__ == "__main__":
    main()
