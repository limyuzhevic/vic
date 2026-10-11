# NLM Advanced User Guide

## Overview

This guide provides comprehensive examples and advanced usage scenarios for the NLM framework.
It builds upon the basic tutorials in `easy_usage.md` and `HOW_TO_USE.md` to demonstrate
complex applications, real-world usage patterns, and advanced features.

## Table of Contents

1. [Complex Agent Learning Scenarios](#complex-agent-learning-scenarios)
2. [Advanced World Environments](#advanced-world-environments)
3. [Performance Optimization and Monitoring](#performance-optimization-and-monitoring)
4. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
5. [Custom Configurations and Extensions](#custom-configurations-and-extensions)
6. [Scientific Research Applications](#scientific-research-applications)
7. [Batch Processing and Simulation Management](#batch-processing-and-simulation-management)
8. [Integration with External Systems](#integration-with-external-systems)

---

## Complex Agent Learning Scenarios

### Scenario 1: Multi-Object Foraging with Competition

This scenario simulates multiple agents competing for limited resources in a complex environment.

```python
import pynlm
import numpy as np
import time

def setup_competitive_environment(num_agents=3, resource_density=0.3):
    """Setup environment with multiple agents and limited resources."""
    
    # Create world with moderate complexity
    world = pynlm.createSimpleWorld()
    world.configure(
        width=50,
        height=50,
        visionWidth=12,
        visionHeight=12
    )
    world.reset()
    world.setRandomSeed(42)  # Reproducible results
    
    # Create agents with different brain configurations
    agents = []
    config_templates = [
        {"brain.neuron_count": 500, "brain.synapse_density": 0.08},
        {"brain.neuron_count": 800, "brain.synapse_density": 0.05},
        {"brain.neuron_count": 300, "brain.synapse_density": 0.12}
    ]
    
    for i, config_dict in enumerate(config_templates):
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Set starting positions for each agent
        start_x = (i + 1) * 10
        start_y = (i + 1) * 10
        world.setAgentStart(start_x, start_y)
        
        agents.append(agent)
    
    return world, agents

def run_competitive_simulation(world, agents, num_steps=1000):
    """Run competitive foraging simulation with monitoring."""
    
    simulation_results = {
        'steps': [],
        'agent_stats': {},
        'resource_events': [],
        'competitive_events': []
    }
    
    # Initialize agent statistics
    for i, agent in enumerate(agents):
        simulation_results['agent_stats'][i] = {
            'steps_without_food': 0,
            'total_food_collected': 0,
            'energy_peaks': [],
            'near_misses': 0
        }
    
    # Create some resources in the environment
    world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Resource, value=50.0))
    world.addObject(pynlm.WorldObject(35, 25, pynlm.WorldObjectType.Resource, value=75.0))
    world.addObject(pynlm.WorldObject(25, 40, pynlm.WorldObjectType.Resource, value=60.0))
    
    print("Starting competitive foraging simulation...")
    print("Initial agent positions:")
    for i, agent in enumerate(agents):
        body = world.getAgentBody()
        print(f"  Agent {i}: ({body.x:.1f}, {body.y:.1f})")
    
    # Main simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Record state before agent actions
        step_data = {
            'step': step,
            'world_time': world.getSimulationTime(),
            'agent_positions': []
        }
        
        # Each agent takes its turn
        for agent_idx, agent in enumerate(agents):
            # Get current percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Get brain decision
            brain = agent.getBrain()
            brain.step(step)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Check for resource collection
            agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
            
            # Check for nearby resources (simplified implementation)
            nearby_resources = []
            # This would normally check world objects for proximity
            
            # For demonstration, we'll simulate resource collection
            distance_to_resource = np.sqrt((agent_x - 25)**2 + (agent_y - 25)**2)
            
            if distance_to_resource < 2.0:  # Close enough to collect
                food_value = 10.0 - distance_to_resource
                agents[agent_idx].getBrain().injectCurrent(
                    agents[agent_idx].getBrain().getTotalNeuronCount() // 2, 
                    5.0 * food_value
                )
                
                simulation_results['resource_events'].append({
                    'step': step,
                    'agent': agent_idx,
                    'type': 'resource_collection',
                    'value': food_value,
                    'position': (agent_x, agent_y)
                })
                
                simulation_results['agent_stats'][agent_idx]['total_food_collected'] += food_value
            
            # Track steps without food
            if distance_to_resource > 8.0:  # Not close to any resource
                simulation_results['agent_stats'][agent_idx]['steps_without_food'] += 1
            
            # Record agent statistics
            step_data['agent_positions'].append((agent_x, agent_y))
            
            # Record energy peaks
            energy = agent.getBrain().getTotalNeuronCount() * 0.01  # Simplified energy calculation
            simulation_results['agent_stats'][agent_idx]['energy_peaks'].append(energy)
        
        # Check for competitive events (agents close to each other)
        for i in range(len(agents)):
            for j in range(i + 1, len(agents)):
                pos_i = step_data['agent_positions'][i]
                pos_j = step_data['agent_positions'][j]
                distance = np.sqrt((pos_i[0] - pos_j[0])**2 + (pos_i[1] - pos_j[1])**2)
                
                if distance < 5.0:  # Close enough for competition
                    simulation_results['competitive_events'].append({
                        'step': step,
                        'agents': (i, j),
                        'distance': distance,
                        'type': 'close_proximity'
                    })
        
        simulation_results['steps'].append(step_data)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: World time = {world.getSimulationTime():.1f}")
    
    return simulation_results

def analyze_competitive_results(results):
    """Analyze and print competitive simulation results."""
    
    print("\n=== Competitive Simulation Results ===")
    
    # Analyze agent performance
    print("\nAgent Performance:")
    for agent_idx, stats in results['agent_stats'].items():
        avg_energy = np.mean(stats['energy_peaks']) if stats['energy_peaks'] else 0
        max_energy = np.max(stats['energy_peaks']) if stats['energy_peaks'] else 0
        
        print(f"  Agent {agent_idx}:")
        print(f"    Total food collected: {stats['total_food_collected']:.1f}")
        print(f"    Steps without food: {stats['steps_without_food']}")
        print(f"    Average energy: {avg_energy:.2f}")
        print(f"    Peak energy: {max_energy:.2f}")
        print(f"    Near misses: {stats['near_misses']}")
    
    # Analyze competitive events
    print(f"\nCompetitive Events: {len(results['competitive_events'])}")
    
    # Analyze resource events
    print(f"\nResource Collection Events: {len(results['resource_events'])}")
    total_resources_collected = sum(event['value'] for event in results['resource_events'])
    print(f"Total resources collected: {total_resources_collected:.1f}")

if __name__ == "__main__":
    # Setup and run competitive simulation
    world, agents = setup_competitive_environment(num_agents=3)
    results = run_competitive_simulation(world, agents, num_steps=1000)
    analyze_competitive_results(results)
```