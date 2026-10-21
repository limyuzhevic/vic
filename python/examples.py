"""
Example scripts for the improved NLM Python bindings.

This file contains comprehensive examples demonstrating the advanced features
and capabilities of the improved NLM Python interface.
"""

import pynlm
import time
import numpy as np
from typing import Dict, Any, List

def example_basic_usage():
    """Example 1: Basic usage with context manager."""
    print("=== Example 1: Basic Usage ===")
    
    # Create brain with context manager
    with pynlm.simulation_session() as brain:
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        print(f"✓ Simulation complete!")
        print(f"  - Neurons: {brain.total_neurons}")
        print(f"  - Synapses: {brain.total_synapses}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {brain.average_firing_rate:.2f}")
    
    print()

def example_property_access():
    """Example 2: Property-style access."""
    print("=== Example 2: Property-Style Access ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Initial state:")
    print(f"  - Total neurons: {brain.total_neurons}")
    print(f"  - Total synapses: {brain.total_synapses}")
    print(f"  - Firing neurons: {brain.firing_neurons}")
    print(f"  - Average firing rate: {brain.average_firing_rate:.2f}")
    print(f"  - E/I ratio: {brain.e_i_ratio:.2f}")
    print(f"  - Developmental stage: {brain.developmental_stage}")
    
    # Modify properties
    print(f"\nModifying properties...")
    
    # Configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    print(f"After enabling learning features:")
    print(f"  - Neuromodulation level: {brain.neuromodulation_level:.2f}")
    print(f"  - Curiosity level: {brain.getCuriosityLevel():.2f}")
    print(f"  - Developmental age: {brain.developmental_age:.1f}")
    print(f"  - Plasticity modifier: {brain.plasticity_modifier:.2f}")
    
    print()

def example_memory_management():
    """Example 3: Memory management."""
    print("=== Example 3: Memory Management ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create memory manager
    memory_manager = pynlm.MemoryManager(brain)
    
    print(f"Initial memory stats:")
    initial_stats = memory_manager.get_memory_stats()
    for category, stats in initial_stats.items():
        print(f"  - {category}:")
        for key, value in stats.items():
            print(f"    {key}: {value}")
    
    # Run simulation to generate memory usage
    for step in range(200):
        brain.step(step)
    
    print(f"\nAfter simulation:")
    memory_manager.optimize_working_memory(500)
    memory_manager.consolidate_memories(0.3)
    
    final_stats = memory_manager.get_memory_stats()
    for category, stats in final_stats.items():
        print(f"  - {category}:")
        for key, value in stats.items():
            print(f"    {key}: {value}")
    
    print()

def example_complete_simulation():
    """Example 4: Complete simulation with world and agent."""
    print("=== Example 4: Complete Simulation ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Run episode
    print("Running simulation episode...")
    start_time = time.time()
    
    for step in range(500):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"  Step {step}:")
            print(f"    - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"    - Curiosity: {brain.getCuriosityLevel():.3f}")
            print(f"    - Novelty: {brain.getNoveltyLevel():.3f}")
            print(f"    - Dev Stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    
    print(f"\nEpisode completed in {end_time - start_time:.2f} seconds")
    print(f"Final stats:")
    print(f"  - Steps completed: 500")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  - Final curiosity: {brain.getCuriosityLevel():.3f}")
    print(f"  - Final novelty: {brain.getNoveltyLevel():.3f}")
    print(f"  - Final developmental stage: {brain.getDevelopmentalStage()}")
    
    print()

def example_parameter_tuning():
    """Example 5: Parameter tuning with memory management."""
    print("=== Example 5: Parameter Tuning ===")
    
    base_config = pynlm.createDefaultConfig()
    
    # Test different configurations
    configurations = [
        {"name": "Small Brain", "neuron_count": 500, "connection_prob": 0.1},
        {"name": "Medium Brain", "neuron_count": 1000, "connection_prob": 0.15},
        {"name": "Large Brain", "neuron_count": 2000, "connection_prob": 0.2},
    ]
    
    results = []
    
    for config_info in configurations:
        print(f"\nTesting {config_info['name']}...")
        
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", config_info['neuron_count'])
        config.set("brain.connection_probability", config_info['connection_prob'])
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create simulation environment
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run simulation
        start_time = time.time()
        
        for step in range(300):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Collect metrics
        result = {
            "name": config_info['name'],
            "neuron_count": config_info['neuron_count'],
            "connection_probability": config_info['connection_prob'],
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "final_curiosity": brain.getCuriosityLevel(),
            "simulation_time": end_time - start_time,
            "steps_per_second": 300 / (end_time - start_time)
        }
        
        results.append(result)
        
        print(f"  - Total spikes: {result['total_spikes']}")
        print(f"  - Average firing rate: {result['average_firing_rate']:.2f}")
        print(f"  - Final curiosity: {result['final_curiosity']:.3f}")
        print(f"  - Simulation time: {result['simulation_time']:.2f}s")
        print(f"  - Performance: {result['steps_per_second']:.1f} steps/s")
    
    # Analyze results
    print(f"\n=== Results Analysis ===")
    best_performance = max(results, key=lambda x: x['steps_per_second'])
    most_stable = min(results, key=lambda x: abs(x['final_curiosity'] - 0.5))
    most_active = max(results, key=lambda x: x['total_spikes'])
    
    print(f"Best performance: {best_performance['name']}")
    print(f"  - Steps per second: {best_performance['steps_per_second']:.1f}")
    print(f"  - Neuron count: {best_performance['neuron_count']}")
    
    print(f"\nMost stable learning: {most_stable['name']}")
    print(f"  - Final curiosity: {most_stable['final_curiosity']:.3f}")
    
    print(f"\nMost active: {most_active['name']}")
    print(f"  - Total spikes: {most_active['total_spikes']}")
    print(f"  - Neuron count: {most_active['neuron_count']}")
    
    print()

def example_debugging_session():
    """Example 6: Debugging session with advanced features."""
    print("=== Example 6: Debugging Session ===")
    
    # Create brain with debugging enabled
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable debugging features
    brain.enable_debug_mode(True)
    brain.enable_state_inspection(True)
    
    print("Starting debugging session...")
    print("Commands: 'n' (next step), 's' (show state), 'r' (record), 'q' (quit)")
    
    for step in range(50):
        print(f"\nStep {step}:")
        
        # Get user command
        command = input("Command (n/s/r/q): ").strip().lower()
        
        if command == 'q':
            print("Quitting debug session")
            break
        elif command == 'n':
            # Execute one step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            brain.step(step)
            
            # Show current state
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
        elif command == 's':
            # Show detailed state
            brain.log_status()
        elif command == 'r':
            # Record state
            brain.record_state(f"step_{step}")
            print(f"  State recorded at step {step}")
        else:
            print(f"Unknown command: {command}")
    
    print("\nDebug session completed")
    print(f"Final stats:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print()

def example_advanced_features():
    """Example 7: Advanced features demonstration."""
    print("=== Example 7: Advanced Features ===")
    
    # Create brain with advanced features
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.set_max_energy(100.0)
    world.set_energy_decay_rate(0.01)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced agent features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    agent.enable_structural_plasticity(True)
    
    print("Running advanced simulation...")
    print("Features enabled:")
    print(f"  - Reward modulation: {agent.is_reward_modulation_enabled()}")
    print(f"  - Curiosity: {agent.is_curiosity_enabled()}")
    print(f"  - Development: {agent.is_development_enabled()}")
    print(f"  - Structural plasticity: {agent.is_structural_plasticity_enabled()}")
    
    # Run advanced simulation
    for step in range(200):
        # Dynamic environment modification
        if step % 50 == 0:
            # Add objects to world
            from pynlm import WorldObject, WorldObjectType
            world.addObject(WorldObject(5.0 + step % 5, 5.0 + step % 5, 
                                      WorldObjectType.Resource, value=1.0, radius=0.3))
            print(f"  Added resource at step {step}")
        
        # Dynamic brain parameter adjustment
        if step % 100 == 0:
            # Adjust neuromodulation
            current_curiosity = agent.getCuriosityLevel()
            agent.set_curiosity_level(current_curiosity * 1.1)
            
            # Adjust development rate
            current_plasticity = agent.get_plasticity_modifier()
            agent.set_plasticity_modifier(current_plasticity * 0.95)
            
            print(f"  Dynamic adjustment at step {step}:")
            print(f"    - Curiosity: {current_curiosity:.3f} -> {agent.getCuriosityLevel():.3f}")
            print(f"    - Plasticity: {current_plasticity:.3f} -> {agent.get_plasticity_modifier():.3f}")
        
        # Normal simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log advanced metrics
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  - World energy: {world.getAgentBody().energy:.1f}")
            print(f"  - Agent health: {world.getAgentBody().health:.1f}")
            print(f"  - Agent age: {world.getAgentBody().age:.1f}")
            print(f"  - Neuromodulation: {brain.getNeuromodulationLevel():.3f}")
            print(f"  - Curiosity: {brain.getCuriosityLevel():.3f}")
            print(f"  - Novelty: {brain.getNoveltyLevel():.3f}")
            print(f"  - Prediction error: {brain.getPredictionError():.3f}")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nAdvanced simulation completed!")
    print(f"Final advanced metrics:")
    print(f"  - World bounds: {world.getWidth()} x {world.getHeight()}")
    print(f"  - Total time: {world.getSimulationTime():.2f}")
    print(f"  - Max energy: {world.getMaxEnergy():.1f}")
    print(f"  - Energy decay: {world.getEnergyDecayRate():.4f}")
    
    print()

def run_all_examples():
    """Run all example scripts."""
    print("=" * 60)
    print("NLM Python Interface Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Usage", example_basic_usage),
        ("Property Access", example_property_access),
        ("Memory Management", example_memory_management),
        ("Complete Simulation", example_complete_simulation),
        ("Parameter Tuning", example_parameter_tuning),
        ("Debugging Session", example_debugging_session),
        ("Advanced Features", example_advanced_features),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
        
        input("Press Enter to continue to next example...")
    
    print("\n" + "=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    run_all_examples()