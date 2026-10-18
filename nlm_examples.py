#!/usr/bin/env python3
"""
Example Scripts for Enhanced NLM Python Bindings

This script contains practical examples demonstrating the enhanced
Python bindings for the NLM (Neural Learning Machine) framework.
"""

import sys
import time
from typing import Optional

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Error: Could not import pynlm: {e}")
    print("Please install the NLM Python bindings using:")
    print("  pip install pynlm")
    sys.exit(1)


def example_1_basic_usage():
    """
    Example 1: Basic Usage
    
    This example demonstrates the fundamental usage patterns of the NLM Python API.
    """
    print("=" * 60)
    print("Example 1: Basic Usage")
    print("=" * 60)
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize a brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run a basic simulation
    print("Running basic simulation for 50 steps...")
    for step in range(50):
        brain.step(step)
        
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"  Step {step:2d}: {firing:3d} firing neurons, {rate:6.2f} avg rate")
    
    print("Basic usage example complete!")
    print()


def example_2_world_interaction():
    """
    Example 2: World Interaction
    
    This example demonstrates how to create and interact with the world
    environment using the NLM Python API.
    """
    print("=" * 60)
    print("Example 2: World Interaction")
    print("=" * 60)
    
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)  # width, height, vision width, vision height
    world.reset()
    world.setAgentStart(10.0, 10.0)  # Set agent starting position
    
    # Create agent to connect brain and world
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created world: {world.getWidth()}x{world.getHeight()} cells")
    print(f"Agent enabled: reward modulation, curiosity, development")
    
    # Run simulation with world interaction
    print("Running simulation with world interaction...")
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes sensory input
        brain.step(step)
        
        # Get motor command from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward based on position
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        # Give reward for staying in bounds, penalty for going out
        if 0 <= x < width and 0 <= y < height:
            reward = 1.0
        else:
            reward = -1.0
            
        # Apply reward modulation to brain
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print periodic updates
        if step % 20 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            firing = brain.getFiringNeuronCount()
            print(f"  Step {step:3d}: curiosity={curiosity:.2f}, novelty={novelty:.2f}, "
                  f"firing={firing}, reward={reward:5.1f}")
    
    print("World interaction example complete!")
    print()


def example_3_advanced_configuration():
    """
    Example 3: Advanced Configuration
    
    This example demonstrates advanced configuration options and
    customization of the NLM brain.
    """
    print("=" * 60)
    print("Example 3: Advanced Configuration")
    print("=" * 60)
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Customize configuration for specific experiment
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.08f)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Advanced configuration settings:")
    print(f"  Neuron count: {config.get('neuron_count')}")
    print(f"  Region count: {config.get('region_count')}")
    print(f"  Connection probability: {config.get('connection_probability')}")
    print(f"  STDP LTP weight: {config.get('stdp_ltp_weight')}")
    print(f"  STDP LTD weight: {config.get('stdp_ltd_weight')}")
    print(f"  Synaptogenesis rate: {config.get('synaptogenesis_rate')}")
    print(f"  Pruning rate: {config.get('pruning_rate')}")
    print(f"  Random seed: {config.get('random_seed')}")
    
    # Run experiment with custom configuration
    print("\nRunning experiment with advanced configuration...")
    
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            region_count = brain.getRegionCount()
            print(f"  Step {step:3d}: {firing:3d} firing neurons, "
                  f"{rate:6.2f} avg rate, {region_count} regions")
    
    print("Advanced configuration example complete!")
    print()


def example_4_agent_capabilities():
    """
    Example 4: Agent Capabilities
    
    This example demonstrates the full capabilities of the AgentBrain class
    for connecting NLM brains to world environments.
    """
    print("=" * 60)
    print("Example 4: Agent Capabilities")
    print("=" * 60)
    
    # Setup complete agent system
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(25, 25, 12, 12)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all agent capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Agent System Capabilities Enabled:")
    print(f"  ✓ Reward Modulation: Learns from rewards and punishments")
    print(f"  ✓ Structural Plasticity: Can create new connections")
    print(f"  ✓ Development: Brain matures over time")
    print(f"  ✓ Curiosity: Explores novel experiences")
    
    # Demonstrate agent capabilities
    print("\nDemonstrating agent capabilities...")
    
    for step in range(150):
        # 1. Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 2. Brain makes decisions
        brain.step(step)
        
        # 3. Decode motor command
        action = agent.decodeMotorCommand()
        
        # 4. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 5. Apply reward modulation
        reward = 0.1  # Small constant reward
        agent.applyRewardModulation(reward, 0.0)
        
        # 6. Update development
        agent.updateDevelopment(0.1)
        
        # 7. Check agent status
        if step % 30 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            neuromod = agent.getNeuromodulationLevel()
            dev_stage = agent.getDevelopmentalStage()
            
            print(f"  Step {step:3d}: curiosity={curiosity:.2f}, novelty={novelty:.2f}, "
                  f"neuromod={neuromod:.2f}, dev_stage={dev_stage}")
    
    print("\nAgent capabilities summary:")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"  Final neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Total neurons used: {brain.getTotalNeuronCount()}")
    print(f"  Active synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("Agent capabilities example complete!")
    print()


def example_5_simulation_monitoring():
    """
    Example 5: Simulation Monitoring
    
    This example demonstrates monitoring and data collection during simulations.
    """
    print("=" * 60)
    print("Example 5: Simulation Monitoring")
    print("=" * 60)
    
    # Create brain and world for monitoring
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 400)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(18, 18, 10, 10)
    world.reset()
    world.setAgentStart(9.0, 9.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting simulation with comprehensive monitoring...")
    
    # Monitoring data collection
    simulation_data = []
    
    for step in range(300):
        # Update environment
        world.update(0.1)
        
        # Get sensor readings
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        body = world.getAgentBody()
        x, y = body.x, body.y
        width, height = world.getWidth(), world.getHeight()
        
        position_reward = 0.5 if (0 <= x < width and 0 <= y < height) else -0.5
        activity_reward = brain.getFiringNeuronCount() * 0.001
        curiosity_reward = agent.getCuriosityLevel() * 0.05
        
        total_reward = position_reward + activity_reward + curiosity_reward
        agent.applyRewardModulation(total_reward, 0.0)
        
        # Record simulation metrics
        step_data = {
            'step': step,
            'timestamp': world.getSimulationTime(),
            'total_reward': total_reward,
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'brain_age': body.age if body else 0,
            'agent_x': body.x if body else 0,
            'agent_y': body.y if body else 0,
            'reward_prediction': 0.0  # Would be calculated in full implementation
        }
        simulation_data.append(step_data)
        
        # Print periodic monitoring updates
        if step % 50 == 0:
            print(f"  Step {step:3d}: "
                  f"reward={total_reward:7.2f}, "
                  f"curiosity={agent.getCuriosityLevel():5.2f}, "
                  f"firing={brain.getFiringNeuronCount():3d}, "
                  f"pos=({x:5.1f},{y:5.1f})")
    
    # Analysis of collected data
    print("\n" + "=" * 60)
    print("SIMULATION ANALYSIS")
    print("=" * 60)
    
    total_steps = len(simulation_data)
    avg_reward = sum(d['total_reward'] for d in simulation_data) / total_steps
    max_reward = max(d['total_reward'] for d in simulation_data)
    min_reward = min(d['total_reward'] for d in simulation_data)
    
    avg_firing = sum(d['avg_firing_rate'] for d in simulation_data) / total_steps
    max_firing = max(d['avg_firing_rate'] for d in simulation_data)
    min_firing = min(d['avg_firing_rate'] for d in simulation_data)
    
    final_curiosity = simulation_data[-1]['curiosity_level']
    final_novelty = simulation_data[-1]['novelty_level']
    
    print(f"Simulation Summary ({total_steps} steps):")
    print(f"  Total reward: {sum(d['total_reward'] for d in simulation_data):8.2f}")
    print(f"  Average reward per step: {avg_reward:8.2f}")
    print(f"  Reward range: [{min_reward:6.2f}, {max_reward:6.2f}]")
    print()
    print(f"Firing Rate Statistics:")
    print(f"  Average: {avg_firing:8.2f}")
    print(f"  Maximum: {max_firing:8.2f}")
    print(f"  Minimum: {min_firing:8.2f}")
    print(f"  Standard deviation: {sum((d['avg_firing_rate'] - avg_firing)**2 for d in simulation_data)/total_steps)**0.5:8.2f}")
    print()
    print(f"Final State:")
    print(f"  Curiosity level: {final_curiosity:8.2f}")
    print(f"  Novelty level: {final_novelty:8.2f}")
    print(f"  Neuromodulation level: {simulation_data[-1]['neuromodulation_level']:8.2f}")
    print(f"  Development stage: {simulation_data[-1]['development_stage']}")
    print(f"  Agent position: ({simulation_data[-1]['agent_x']:6.1f}, {simulation_data[-1]['agent_y']:6.1f})")
    
    print("\nSimulation monitoring example complete!")
    print()


def main():
    """Run all example scripts."""
    print("NLM Enhanced Python Bindings - Example Scripts")
    print("=" * 60)
    print()
    print("This script provides practical examples of using the enhanced")
    print("NLM Python bindings for neural simulation and agent development.")
    print()
    
    if not PYNLM_AVAILABLE:
        sys.exit(1)
    
    # Run all examples
    examples = [
        ("Basic Usage", example_1_basic_usage),
        ("World Interaction", example_2_world_interaction),
        ("Advanced Configuration", example_3_advanced_configuration),
        ("Agent Capabilities", example_4_agent_capabilities),
        ("Simulation Monitoring", example_5_simulation_monitoring),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error running {name}: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print()
    print("The examples demonstrate:")
    print("  1. Basic brain creation and simulation")
    print("  2. World environment interaction")
    print("  3. Advanced configuration options")
    print("  4. Complete agent system capabilities")
    print("  5. Comprehensive simulation monitoring")
    print()
    print("These examples showcase the enhanced Python bindings for")
    print("NLM, providing users with powerful tools for neural simulation")
    print("and artificial intelligence research.")


if __name__ == '__main__':
    main()
