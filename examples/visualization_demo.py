#!/usr/bin/env python3
"""
Visualization Demo Example

This example demonstrates data collection and preparation for visualization
using enhanced NLM bindings and utilities.
"""

import pynlm_enhanced as nlm
import pynlm_utils as utils
import time
import math

def collect_simulation_data(brain, world, agent, num_steps=100, timestep=0.1):
    """Collect simulation data for visualization."""
    print(f"Collecting simulation data for {num_steps} steps...")
    
    # Create statistics collector
    stats = utils.SimulationStats()
    
    # Initialize agent
    agent.initialize(world)
    
    # Collect data
    for step in range(num_steps):
        world.update(timestep)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept and percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(timestep)
        
        # Collect statistics
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
        
        if step % 20 == 0:
            print(f"   Collected data for step {step}")
    
    return stats

def prepare_visualization_data(stats):
    """Prepare data for visualization."""
    print("\nPreparing visualization data...")
    
    # Get statistics dictionary
    stats_dict = stats.to_dict()
    
    # Create time series data
    time_series = []
    for step in range(stats.getTotalSteps()):
        time_series.append({
            'step': step,
            'firing_rate': stats.getAverageFiringRate(),
            'curiosity': stats.getAverageCuriosity(),
            'novelty': stats.getAverageNovelty(),
            'prediction_error': stats.getAveragePredictionError(),
            'neuromodulation': stats.getAverageNeuromodulationLevel()
        })
    
    # Create stage timeline
    stage_timeline = []
    stage_dist = stats_dict.get('stage_distribution', {})
    
    print(f"   Visualization data prepared:")
    print(f"   - Time series points: {len(time_series)}")
    print(f"   - Developmental stages: {list(stage_dist.keys())}")
    print(f"   - Average simulation time per step: {stats.getTimePerStep():.4f} seconds")
    
    return {
        'time_series': time_series,
        'stage_distribution': stage_dist,
        'statistics': stats_dict,
        'ready_for_viz': True
    }

def main():
    print("=== Visualization Demo (Enhanced Bindings) ===\n")
    
    # Create enhanced brain
    print("1. Creating enhanced brain...")
    with nlm.create_enhanced_brain() as brain:
        print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
        brain.initialize()
        
        # Create enhanced world
        print("\n2. Setting up world...")
        with nlm.create_enhanced_world() as world:
            world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
            world.reset()
            world.setAgentStart(7.5, 7.5)
            
            # Create enhanced agent brain
            print("\n3. Creating enhanced agent brain...")
            with nlm.create_enhanced_agent_brain(brain) as agent:
                # Collect simulation data
                print("\n4. Collecting simulation data...")
                stats = collect_simulation_data(brain, world, agent, 100)
                
                # Prepare visualization data
                print("\n5. Preparing visualization data...")
                viz_data = prepare_visualization_data(stats)
                
                # Print summary
                print("\n=== Visualization Summary ===")
                print(f"Data collection completed: {stats.getTotalSteps()} steps")
                print(f"Average firing rate: {stats.getAverageFiringRate():.3f} Hz")
                print(f"Data ready for visualization: {viz_data['ready_for_viz']}")
                
                # Example of how data could be used for plotting
                print("\n=== Example Usage ===")
                print("The collected data can be used with:")
                print("  - matplotlib for static plots")
                print("  - plotly for interactive visualizations")
                print("  - bokeh for web-based visualizations")
                print("  - D3.js for web dashboards")
                
                # Example data access
                time_series = viz_data['time_series']
                if time_series:
                    print(f"\nSample data point (step 0): {time_series[0]}")
                    print(f"Sample data point (step -1): {time_series[-1]}")
    
    print("\n=== Visualization demo completed successfully! ===")

if __name__ == "__main__":
    main()
