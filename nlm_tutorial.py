#!/usr/bin/env python3
"""
NLM Python API Tutorial
======================

This tutorial demonstrates the enhanced NLM (Neural Learning Machine) Python API.
It covers the new user-friendly features, convenience methods, and advanced patterns.

The NLM Python API provides a clean interface to the C++ neural simulation framework,
allowing you to create, simulate, and interact with neural brains in Python.
"""

import pynlm
from typing import Dict, List, Tuple

def hello_world():
    """The simplest possible example - just create and run a brain."""
    print("=== Hello World Example ===")
    
    # Create a default brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize the brain
    brain.initialize()
    
    # Run it for a few steps
    for i in range(10):
        brain.step(i)
    
    print(f"Your brain ran for 10 steps!")
    print(f"It processed {brain.getTotalSpikeCount()} spikes")
    print(f"Has {brain.getTotalNeuronCount()} neurons")
    print()

def basic_brain_setup():
    """Example showing how to set up a brain with custom configuration."""
    print("=== Basic Brain Setup ===")
    
    # Method 1: Using default configuration
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    # Method 2: Using convenience methods for configuration
    config2 = pynlm.createDefaultConfig()
    config2.configureBrain(neuron_count=500, synapse_density=0.2, 
                          connection_probability=0.1)
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    print(f"Brain 1: {brain1.getTotalNeuronCount()} neurons")
    print(f"Brain 2: {brain2.getTotalNeuronCount()} neurons (configured)")
    print()

def enhanced_agent_simulation():
    """
    Comprehensive agent simulation demonstrating all key features.
    This example shows the full integration of brain, world, and agent.
    """
    print("=== Enhanced Agent Simulation ===")
    
    # 1. Create and configure the brain
    config = pynlm.createDefaultConfig()
    config.configureBrain(neuron_count=1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create the world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    
    # 3. Create the agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 5. Run the simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 20 steps
        if step % 20 == 0:
            print(f"Step {step}: {agent.getActionName()} at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Brain stats: {brain.getFiringNeuronCount()} firing neurons")
            print(f"  Agent stats: curiosity={agent.getCuriosityLevel():.2f}, stage={brain.getDevelopmentalStage()}")
    
    print("Simulation complete!")
    print()

def simulation_monitoring():
    """Example showing how to monitor and collect data during simulation."""
    print("=== Simulation Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Collect simulation data
    simulation_data = {
        'steps': [],
        'positions': [],
        'actions': [],
        'rewards': [],
        'brain_stats': [],
        'agent_stats': []
    }
    
    for step in range(50):
        world.update(0.1)
        
        # Record state before processing
        agent_body = world.getAgentBody()
        simulation_data['steps'].append(step)
        simulation_data['positions'].append((agent_body.x, agent_body.y))
        
        # Process and simulate
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record after processing
        action = agent.decodeMotorCommand()
        simulation_data['actions'].append(action.getName())
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply simple learning
        reward = 1.0 if action.getName() != "Wait" else 0.0
        simulation_data['rewards'].append(reward)
        agent.applyRewardModulation(reward, 0.0)
        
        # Record stats
        simulation_data['brain_stats'].append({
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount()
        })
        
        simulation_data['agent_stats'].append({
            'curiosity': agent.getCuriosityLevel(),
            'stage': static_cast[int>(brain.getDevelopmentalStage()),
            'novelty': agent.getNoveltyLevel()
        })
    
    # Analyze collected data
    print(f"Collected data for {len(simulation_data['steps'])} steps")
    print(f"Average curiosity level: {sum(d['curiosity'] for d in simulation_data['agent_stats']) / len(simulation_data['agent_stats']):.3f}")
    print(f"Most common action: {max(set(simulation_data['actions']), key=simulation_data['actions'].count)}")
    print(f"Total reward received: {sum(simulation_data['rewards'])}")
    print()

def configuration_management():
    """Example showing advanced configuration management."""
    print("=== Configuration Management ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Method 1: Set individual values
    config.set("brain.neuron_count", "500")
    config.set("brain.synapse_density", "0.15")
    config.set("plasticity.stdp.learning_rate", "0.005")
    
    # Method 2: Use convenience method
    config.configureBrain(neuron_count=1000, synapse_density=0.1)
    
    # Method 3: Merge configurations
    config2 = pynlm.createDefaultConfig()
    config2.set("plasticity.hebbian.enable", "true")
    config2.set("neuromod.dopamine.scale", "2.0")
    
    config.merge(config2)  # config2 values override config1 values
    
    # Validate configuration
    if config.validate():
        print("Configuration is valid!")
    else:
        print("Configuration has issues!")
    
    # Get configuration values
    print(f"Neuron count: {config.getNeuronCount()}")
    print(f"Synapse density: {config.getSynapseDensity()}")
    
    # Create brain with this configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print()

def batch_simulation():
    """Example showing how to run multiple simulations efficiently."""
    print("=== Batch Simulation ===")
    
    simulations = []
    
    # Run multiple simulations with different configurations
    for i in range(3):
        print(f"Running simulation {i+1}...")
        
        config = pynlm.createDefaultConfig()
        config.configureBrain(neuron_count=500 + i*200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        # Store results
        simulations.append({
            'id': i+1,
            'neurons': brain.getTotalNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'final_stage': static_cast[int>(brain.getDevelopmentalStage())
        })
    
    # Compare results
    print("\nSimulation Results:")
    print("ID | Neurons | Spikes | Firing Rate | Stage")
    print("-" * 50)
    for sim in simulations:
        print(f"{sim['id']}  | {sim['neurons']}    | {sim['spikes']}   | {sim['firing_rate']:.3f}     | {sim['final_stage']}")
    
    # Find best performing simulation
    best = max(simulations, key=lambda x: x['spikes'])
    print(f"\nBest performing simulation: {best['id']} with {best['spikes']} spikes")
    print()

def world_interaction():
    """Example showing world manipulation and object interaction."""
    print("=== World Interaction ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    
    # Add objects to the world
    print("Adding objects to the world...")
    
    # Add resources
    world.addResource(5.0, 5.0, 10.0)   # Resource with value 10
    world.addResource(15.0, 5.0, 5.0)
    
    # Add hazards
    world.addHazard(10.0, 10.0, 2.0)    # Hazard with radius 2
    world.addHazard(18.0, 15.0, 1.0)
    
    # Add agent start
    world.setAgentStart(2.0, 2.0)
    world.reset()
    
    # Get initial status
    status = world.getStatus()
    print(f"World status: {status}")
    
    # Create agent and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(30):
        world.update(0.1)
        
        # Check if agent is near a resource
        agent_x, agent_y = world.getAgentBody().x, world.getAgentBody().y
        percept = world.getSensoryPercept()
        
        # For this example, we'll just show what's available
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}: Agent at ({agent_x:.1f}, {agent_y:.1f}), Action: {action.getName()}")
    
    print("World interaction complete!")
    print()

def error_handling_demo():
    """Example showing improved error handling."""
    print("=== Error Handling Demonstration ===")
    
    try:
        # This should work fine
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("✓ Brain created and initialized successfully")
        
        # This should work with convenience methods
        config = pynlm.createDefaultConfig()
        config.configureBrain(neuron_count=100)
        print("✓ Configuration completed successfully")
        
        # This should work with world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10)
        print("✓ World created successfully")
        
    except Exception as e:
        print(f"✗ Error occurred: {e}")
    
    # Demonstrate validation
    print("\nConfiguration validation:")
    config = pynlm.createDefaultConfig()
    if config.validate():
        print("✓ Configuration is valid")
    else:
        print("✗ Configuration validation failed")
    
    print("\nError handling demo complete!")
    print()

def main():
    """Run all tutorial examples."""
    print("NLM Python API Tutorial")
    print("=" * 50)
    print()
    
    # Run examples in order
    hello_world()
    basic_brain_setup()
    configuration_management()
    enhanced_agent_simulation()
    simulation_monitoring()
    world_interaction()
    batch_simulation()
    error_handling_demo()
    
    print("=" * 50)
    print("Tutorial Complete!")
    print()
    print("Key Improvements Demonstrated:")
    print("✓ Enhanced convenience methods for configuration")
    print("✓ Better error handling and validation")
    print("✓ Simulation monitoring and data collection")
    print("✓ Advanced agent and world interaction")
    print("✓ Batch processing capabilities")
    print("✓ Improved user experience")
    print()
    print("For more information, see HOW_TO_USE.md and advanced_examples.py")

if __name__ == "__main__":
    main()
