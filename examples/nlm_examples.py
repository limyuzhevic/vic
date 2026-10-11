#!/usr/bin/env python3
"""
Comprehensive Python examples for NLM (Neural Learning Machine)

This script demonstrates the full range of capabilities available in the NLM Python API,
from basic neural computation to advanced learning simulations.
"""

import pynlm
import time
import sys
from typing import List, Optional

def example_basic_brain():
    """Example 1: Create and run a basic neural brain"""
    print("=" * 60)
    print("Example 1: Basic Neural Brain")
    print("=" * 60)
    
    # Create a simple brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain initialized with {brain.getTotalSynapseCount()} synapses")
    
    # Run a basic simulation
    print("\nRunning simulation...")
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def example_brain_world_agent():
    """Example 2: Complete agent simulation with world interaction"""
    print("=" * 60)
    print("Example 2: Agent in a World Simulation")
    print("=" * 60)
    
    # Setup brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting agent simulation...")
    print(f"Initial position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    
    # Run simulation
    for step in range(50):
        world.update(0.1)  # Update world state
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)  # Brain processes sensory input
        
        brain.step(step)  # Brain thinks
        
        action = agent.decodeMotorCommand()  # Brain decides action
        world.applyMotorCommand(action, world.getSimulationTime())  # Agent acts
        
        # Apply reward based on environment interaction
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"Final position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    print(f"Total simulation time: {world.getSimulationTime():.1f}")
    print()

def example_learning_simulation():
    """Example 3: Demonstrates learning and plasticity"""
    print("=" * 60)
    print("Example 3: Learning and Plasticity Simulation")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Starting learning simulation with multiple plasticity mechanisms...")
    
    # Enable all plasticity mechanisms
    brain.getSTDP().enablePlasticity(True)
    brain.getHebbian().enablePlasticity(True)
    brain.getStructuralPlasticity().enablePlasticity(True)
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_synapses = brain.getTotalSynapseCount()
    
    print(f"Initial state - Spikes: {initial_spikes}, Synapses: {initial_synapses}")
    
    # Run learning simulation with varied input patterns
    learning_steps = 1000
    for step in range(learning_steps):
        # Create varied input patterns to stimulate different learning mechanisms
        if step < learning_steps // 4:
            # Strong excitatory input for initial potentiation
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        elif step < learning_steps // 2:
            # Correlated input for STDP
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 20.0f)
        else:
            # Mixed input for Hebbian learning
            brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 15.0f)
        
        brain.step(step)
        
        if step % 200 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount() - initial_spikes} new spikes")
    
    print(f"Learning complete. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final synapse count: {brain.getTotalSynapseCount()}")
    print(f"Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print()

def example_world_exploration():
    """Example 4: World exploration with intelligent agent"""
    print("=" * 60)
    print("Example 4: World Exploration with Intelligent Agent")
    print("=" * 60)
    
    # Create agent with high learning capacity
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Place agent in center
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Optimize for exploration
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("Agent starting exploration...")
    
    # Track exploration statistics
    visited_positions = set()
    resource_collisions = 0
    hazard_encounters = 0
    
    for step in range(200):
        world.update(0.05)
        
        # Track position
        x, y = world.getAgentBody().x, world.getAgentBody().y
        visited_positions.add((round(x, 1), round(y, 1)))
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (check nearby objects)
        agent_body = world.getAgentBody()
        for obj in [world.getObjectAt(agent_body.x, agent_body.y)]:
            if obj:
                if obj.type == pynlm.WorldObjectType.Resource:
                    resource_collisions += 1
                elif obj.type == pynlm.WorldObjectType.Hazard:
                    hazard_encounters += 1
        
        agent.applyRewardModulation(0.0, 0.0)  # Small exploration bonus
        
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    Position: ({x:.1f}, {y:.1f})")
            print(f"    Visited {len(visited_positions)} unique positions")
            print(f"    Resources found: {resource_collisions}")
            print(f"    Hazards avoided: {hazard_encounters}")
            print(f"    Energy: {world.getAgentBody().energy:.1f}")
    
    print(f"Exploration complete. Coverage: {len(visited_positions)/900*100:.1f}% of world")
    print(f"Resources collected: {resource_collisions}")
    print(f"Hazards encountered: {hazard_encounters}")
    print()

def example_performance_benchmarking():
    """Example 5: Performance benchmarking and metrics"""
    print("=" * 60)
    print("Example 5: Performance Benchmarking")
    print("=" * 60)
    
    import time
    
    # Test different brain configurations
    configs = [
        {"neuron_count": 500, "description": "Small brain"},
        {"neuron_count": 1000, "description": "Medium brain"},
        {"neuron_count": 2000, "description": "Large brain"},
    ]
    
    for config_spec in configs:
        print(f"\nTesting {config_spec['description']} ({config_spec['neuron_count']} neurons)...")
        
        config = pynlm.createDefaultConfig()
        if config_spec['neuron_count'] != 1000:  # Default
            config.set("brain.neuron_count", config_spec['neuron_count'])
        
        brain = pynlm.createBrain(config)
        
        # Time initialization
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Time simulation steps
        steps = 100
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        sim_time = time.time() - start_time
        
        print(f"  Initialization: {init_time:.3f}s")
        print(f"  Simulation ({steps} steps): {sim_time:.3f}s")
        print(f"  Average step time: {sim_time/steps*1000:.2f}ms")
        print(f"  Spikes per second: {brain.getTotalSpikeCount()/sim_time:.0f}")
    
    print()

def example_advanced_learning():
    """Example 6: Advanced learning with multiple plasticity mechanisms"""
    print("=" * 60)
    print("Example 6: Advanced Learning with Multiple Plasticity")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Demonstrating integration of multiple learning mechanisms...")
    
    # Create learning conditions that engage different plasticity rules
    learning_cycles = 5
    steps_per_cycle = 200
    
    for cycle in range(learning_cycles):
        print(f"\nLearning Cycle {cycle + 1}/{learning_cycles}")
        
        # Reset brain for controlled learning
        brain.reset()
        brain.initialize()
        
        # Phase 1: STDP learning (temporal correlation)
        print("  Phase 1: STDP (Spike-Timing-Dependent Plasticity)")
        for step in range(steps_per_cycle // 4):
            # Create pre-post spike patterns
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0f)
            brain.step(step)
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 40.0f)
            brain.step(step * 2)
        
        # Phase 2: Hebbian learning (correlation-based)
        print("  Phase 2: Hebbian (Co-activation)")
        for step in range(steps_per_cycle // 4):
            # Co-activate related neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 25.0f)
            brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 25.0f)
            brain.step(step)
        
        # Phase 3: Structural plasticity
        print("  Phase 3: Structural Plasticity")
        for step in range(steps_per_cycle // 4):
            # Stimulate structural changes
            brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 20.0f)
            brain.step(step)
        
        # Phase 4: Neuromodulation
        print("  Phase 4: Neuromodulation")
        for step in range(steps_per_cycle // 4):
            # Apply reward-based learning
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0f)
            brain.step(step)
            brain.applyRewardModulation(1.0, 0.5)  # Reward signal
        
        print(f"    Cycle {cycle + 1} complete - {brain.getTotalSpikeCount()} spikes")
    
    print("\nAdvanced learning demonstration complete!")
    print()

def example_configuration_management():
    """Example 7: Configuration management and serialization"""
    print("=" * 60)
    print("Example 7: Configuration Management")
    print("=" * 60)
    
    # Create and modify configuration
    config = pynlm.createDefaultConfig()
    
    print("Default configuration:")
    print(config.summary())
    
    # Customize configuration
    print("\nModifying configuration...")
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    print("Modified configuration:")
    print(config.summary())
    
    # Save and reload configuration
    config.saveToFile("brain_config.json")
    print("\nConfiguration saved to brain_config.json")
    
    # Load configuration
    new_config = pynlm.createDefaultConfig()
    new_config.loadFromFile("brain_config.json")
    print("Configuration loaded from brain_config.json")
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(new_config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain initialized with {brain.getTotalSynapseCount()} synapses")
    print()

def main():
    """Run all examples"""
    print("\n" + "=" * 60)
    print("NLM Python API - Comprehensive Examples")
    print("=" * 60)
    print("This script demonstrates the full range of NLM capabilities")
    print("for users ranging from beginners to advanced practitioners.")
    print()
    
    try:
        example_basic_brain()
        example_brain_world_agent()
        example_learning_simulation()
        example_world_exploration()
        example_performance_benchmarking()
        example_advanced_learning()
        example_configuration_management()
        
        print("=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        print("\nFor more advanced usage, refer to:")
        print("  - HOW_TO_USE.md: Detailed API documentation")
        print("  - docs/ARCHITECTURE.md: System architecture")
        print("  - src/agent/AgentBrain.hpp: Agent interface details")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())