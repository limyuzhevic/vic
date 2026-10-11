#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Advanced Python Examples

This file contains comprehensive examples demonstrating NLM's capabilities
for users ranging from beginners to advanced researchers.

Examples covered:
1. Basic brain simulation
2. World interaction with agent
3. Learning and plasticity experiments
4. Advanced configuration and optimization
5. Performance monitoring and profiling
6. Checkpointing and replay
7. Custom plasticity rules
8. Neuromodulation and development
9. Memory systems integration
10. Complex multi-agent scenarios
"""

import pynlm
import time
import json
import os
from typing import List, Dict, Any


def example_basic_brain_simulation():
    """Example 1: Basic brain simulation - The simplest possible NLM brain"""
    print("=== Example 1: Basic Brain Simulation ===")
    
    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Configure for learning
    brain.getConfig().set("plasticity.stdp.enable", True)
    brain.getConfig().set("plasticity.hebbian.enable", True)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation with basic input
    for step in range(100):
        # Inject sensory input to simulate external stimuli
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
        
        if step % 25 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, {brain.getTotalSpikeCount()} total spikes")
    
    print(f"Simulation complete. Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()


def example_world_interaction():
    """Example 2: Agent in a world - Complete embodied agent simulation"""
    print("=== Example 2: World Interaction with Agent ===")
    
    # Setup the complete agent-world system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning subsystems for behavior development
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Agent setup complete. World: {world.getWidth()}x{world.getHeight()}, Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run multi-step simulation
    simulation_time = 500
    print(f"Running {simulation_time} simulation steps...")
    
    for step in range(simulation_time):
        # 1. Update world state (physics, etc.)
        world.update(0.1)
        
        # 2. Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Run brain computation
        brain.step(step)
        
        # 5. Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # 6. Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development (brain ages)
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"  Step {step}:")
            print(f"    Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"World simulation complete.")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print()


def example_plasticity_learning():
    """Example 3: Plasticity learning experiments - Demonstrating STDP and Hebbian learning"""
    print("=== Example 3: Plasticity Learning Experiments ===")
    
    # Create two identical brains for controlled comparison
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 200)
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    config2 = pynlm.createDefaultConfig()
    config2.set("neuron_count", 200)
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    print("Created two brains for comparison experiment")
    
    # Brain 1: Enable all plasticity
    brain1.getConfig().set("plasticity.stdp.enable", True)
    brain1.getConfig().set("plasticity.hebbian.enable", True)
    brain1.getConfig().set("plasticity.structural.enable", True)
    
    # Brain 2: No plasticity (control)
    brain1.getConfig().set("plasticity.stdp.enable", False)
    brain1.getConfig().set("plasticity.hebbian.enable", False)
    brain1.getConfig().set("plasticity.structural.enable", False)
    
    # Record initial state
    initial_spikes1 = brain1.getTotalSpikeCount()
    initial_spikes2 = brain2.getTotalSpikeCount()
    
    # Run correlated activity experiment (STDP)
    print("Running STDP experiment (correlated pre-post activity)...")
    for step in range(500):
        # Create correlated activity: sensory input followed by motor output
        brain1.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0f)
        brain1.step(step)
        
        brain1.injectCurrentToNeurons(pynlm.NeuronType.Motor, 40.0f)
        brain1.step(step + 1)
        
        # Control brain: same activity pattern
        brain2.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0f)
        brain2.step(step)
        
        brain2.injectCurrentToNeurons(pynlm.NeuronType.Motor, 40.0f)
        brain2.step(step + 1)
    
    # Compare results
    final_spikes1 = brain1.getTotalSpikeCount() - initial_spikes1
    final_spikes2 = brain2.getTotalSpikeCount() - initial_spikes2
    
    print(f"Brain 1 (with plasticity): {final_spikes1} additional spikes")
    print(f"Brain 2 (without plasticity): {final_spikes2} additional spikes")
    
    if final_spikes1 > final_spikes2:
        print("✓ STDP working: Correlated activity produced potentiation")
    else:
        print("✗ STDP may not be working as expected")
    
    # Hebbian learning demonstration
    print("\nRunning Hebbian learning experiment...")
    config3 = pynlm.createDefaultConfig()
    config3.set("neuron_count", 300)
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    # Enable Hebbian plasticity
    brain3.getConfig().set("plasticity.hebbian.enable", True)
    brain3.getConfig().set("plasticity.stdp.enable", False)
    
    # Create pattern: repeated sensory input
    for step in range(300):
        brain3.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
        brain3.step(step)
    
    print(f"Hebbian learning complete. Brain has {brain3.getTotalSynapseCount()} synapses")
    print(f"Average firing rate: {brain3.getAverageFiringRate():.2f} Hz")
    print()


def example_advanced_configuration():
    """Example 4: Advanced configuration and optimization - Custom brain configuration"""
    print("=== Example 4: Advanced Configuration and Optimization ===")
    
    # Create highly customized configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for specific use case: cognitive task
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.08)
    config.set("brain.connection_probability", 0.05)
    
    # Set neuron parameters for cognitive tasks
    config.set("brain.v_thresh", -45.0)      # Higher threshold for cognitive neurons
    config.set("brain.v_rest", -65.0)        # Standard resting potential
    config.set("brain.tau_mem", 15.0)        # Longer time constant for working memory
    config.set("brain.tau_ref", 3.0)         # Short refractory period
    
    # Configure plasticity for cognitive development
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synapse_creation_rate", 0.0001)
    config.set("plasticity.structural.synapse_pruning_rate", 0.00005)
    
    # Configure neuromodulation for cognitive control
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # Configure development stages
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_end", 60.0)
    config.set("development.maturation_end", 300.0)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced brain configuration:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Initial development stage: {brain.getDevelopmentalStage()}")
    
    # Run performance benchmark
    print("\nRunning performance benchmark...")
    start_time = time.time()
    
    for step in range(1000):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 25.0f)
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Performance: {1000/elapsed:.0f} steps/second")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_checkpointing_and_replay():
    """Example 5: Checkpointing and replay - Saving and loading brain states"""
    print("=== Example 5: Checkpointing and Replay ===")
    
    # Create and train a brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Training brain for 1000 steps...")
    for step in range(1000):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)
        
        if step % 250 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Save brain state (checkpoint)
    checkpoint_file = "nlm_checkpoint.bin"
    if brain.save(checkpoint_file):
        print(f"✓ Brain state saved to {checkpoint_file}")
        print(f"  File size: {os.path.getsize(checkpoint_file)} bytes")
    else:
        print("✗ Failed to save brain state")
        return
    
    # Reset brain to clean state
    brain.reset()
    brain.initialize()
    
    # Verify reset
    if brain.getTotalSynapseCount() == 0 and brain.getTotalSpikeCount() == 0:
        print("✓ Brain successfully reset")
    
    # Reload brain state from checkpoint
    if brain.load(checkpoint_file):
        print(f"✓ Brain state loaded from {checkpoint_file}")
        
        # Verify loaded state
        print(f"  Loaded brain has {brain.getTotalSynapseCount()} synapses")
        print(f"  Last spike count was: {brain.getTotalSpikeCount()}")
    else:
        print("✗ Failed to load brain state")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print("✓ Checkpoint file cleaned up")
    
    print()


def example_memory_systems():
    """Example 6: Memory systems integration - Working memory, episodic memory, and associative memory"""
    print("=== Example 6: Memory Systems Integration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"Created brain with memory systems:")
    print(f"  Working Memory neurons: {working_memory.getWorkingMemoryNeuronCount()}")
    print(f"  Episodic Memory capacity: {episodic_memory.getMaxCapacity()} episodes")
    print(f"  Associative Memory patterns: {associative_memory.getPatternCount()}")
    
    # Simulate memory formation through experience
    print("\nSimulating memory formation through experience...")
    
    # Create and store an experience
    experience_data = {
        "pattern": [0.1, 0.5, 0.9, 0.7, 0.3],
        "context": [1.0, 0.0, 0.5],
        "outcome": 1.0,
        "timestamp": 0.0
    }
    
    episode_id = episodic_memory.storeExperience(experience_data)
    print(f"Stored experience as episode {episode_id}")
    
    # Form associative memory
    pattern1 = [0.1, 0.2, 0.3]
    pattern2 = [0.4, 0.5, 0.6]
    association_strength = associative_memory.formAssociation(pattern1, pattern2)
    print(f"Formed association with strength {association_strength:.3f}")
    
    # Test memory retrieval
    retrieved = episodic_memory.retrieveExperience(episode_id)
    if retrieved:
        print(f"Retrieved experience: outcome={retrieved['outcome']}")
    
    # Test pattern completion
    partial_pattern = [0.1, 0.15, 0.25]
    completed = associative_memory.completePattern(partial_pattern)
    if completed:
        print(f"Pattern completion successful: {completed}")
    
    # Update working memory
    working_memory.updateWorkingMemory([0.8, 0.6, 0.4, 0.2])
    working_memory.constrainWorkingMemory(0.9)
    
    print(f"Working memory activity level: {working_memory.getWorkingMemoryActivity():.3f}")
    print()


def example_neuromodulation_and_development():
    """Example 7: Neuromodulation and development - Complex brain development"""
    print("=== Example 7: Neuromodulation and Development ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 600)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Initialized brain with full neuromodulation:")
    print(f"  Initial dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Initial curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Initial novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Initial development stage: {agent.getDevelopmentalStage()}")
    
    # Simulate development over time
    print("\nSimulating development over 500 simulation steps...")
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Apply reward
        reward = agent.getNoveltyLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print development progress
        if step % 100 == 0:
            print(f"  Step {step}:")
            print(f"    Development stage: {agent.getDevelopmentalStage()}")
            print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"    Plasticity modifier: {brain.getConfig().getOr<float>('plasticity.modifier', 1.0):.3f}")
    
    print(f"\nDevelopment simulation complete.")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()


def example_multi_agent_scenario():
    """Example 8: Multi-agent scenario - Complex social interaction simulation"""
    print("=== Example 8: Multi-Agent Scenario ===")
    
    # Create multiple agents sharing the same brain architecture
    agents = []
    brains = []
    worlds = []
    
    print("Creating multi-agent simulation...")
    
    for i in range(3):
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 400)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.setAgentStart(i * 3.0, i * 3.0)
        world.reset()
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
        worlds.append(world)
        
        print(f"  Agent {i}: Position ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Simulate interaction over time
    print("\nSimulating agent interaction...")
    
    for step in range(300):
        # Each agent perceives and acts independently
        for i, (agent, world) in enumerate(zip(agents, worlds)):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Add social signal from nearby agents
            for j, other_world in enumerate(worlds):
                if i != j:
                    distance = ((world.getAgentBody().x - other_world.getAgentBody().x) ** 2 + 
                              (world.getAgentBody().y - other_world.getAgentBody().y) ** 2) ** 0.5
                    if distance < 5.0:  # Social proximity threshold
                        # Add social signal to internal perception
                        internal_signals = percept.getInternal()
                        social_signal = min(1.0, 1.0 - distance / 5.0)
                        internal_signals.append(social_signal)
                        # Note: In real implementation, would set internal signals properly
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brains[i].step(step)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply social learning signal
            reward = agent.getNoveltyLevel()
            agent.applyRewardModulation(reward, 0.0)
        
        # Print statistics every 50 steps
        if step % 50 == 0:
            print(f"  Step {step}:")
            for i, (agent, world) in enumerate(zip(agents, worlds)):
                print(f"    Agent {i}: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                      f"Curiosity: {agent.getCuriosityLevel():.2f}")
    
    # Compare agent performance
    print("\n=== Multi-Agent Results ===")
    for i, (agent, brain) in enumerate(zip(agents, brains)):
        print(f"Agent {i}: {agent.getCuriosityLevel():.3f} curiosity, "
              f"{agent.getNeuromodulationLevel():.3f} neuromodulation, "
              f"{brain.getAverageFiringRate():.1f} Hz firing rate")
    
    print()


def example_performance_monitoring():
    """Example 9: Performance monitoring and profiling - Measuring NLM performance"""
    print("=== Example 9: Performance Monitoring and Profiling ===")
    
    def benchmark_brain(neuron_count, steps):
        """Benchmark brain performance with specific configuration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        config.set("connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        import time
        start_time = time.time()
        
        for step in range(steps):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        return {
            'neurons': neuron_count,
            'steps': steps,
            'time': elapsed,
            'steps_per_second': steps / elapsed,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    # Run benchmarks for different network sizes
    print("Running performance benchmarks...")
    
    benchmarks = [
        (100, 1000),    # Small network, many steps
        (500, 1000),    # Medium network
        (1000, 500),    # Large network, fewer steps
        (2000, 200),    # Very large network
    ]
    
    results = []
    for neurons, steps in benchmarks:
        print(f"  Benchmarking {neurons} neurons for {steps} steps...")
        result = benchmark_brain(neurons, steps)
        results.append(result)
        print(f"    {result['steps_per_second']:.0f} steps/second")
        print(f"    {result['firing_rate']:.1f} Hz average firing rate")
    
    # Performance summary
    print("\n=== Performance Summary ===")
    for result in results:
        print(f"  {result['neurons']:4d} neurons: {result['steps_per_second']:6.0f} steps/sec, "
              f"{result['firing_rate']:5.1f} Hz, {result['total_spikes']:6d} total spikes")
    
    # Analyze scaling
    print("\n=== Scaling Analysis ===")
    if len(results) >= 2:
        neurons1, steps1 = results[0]['neurons'], results[0]['steps_per_second']
        neurons2, steps2 = results[-1]['neurons'], results[-1]['steps_per_second']
        neuron_ratio = neurons2 / neurons1
        speed_ratio = steps2 / steps1
        print(f"  Neuron scaling: {neurons1} → {neurons2} ({neuron_ratio:.1f}x)")
        print(f"  Speed scaling: {steps1:.0f} → {steps2:.0f} steps/sec ({speed_ratio:.1f}x)")
        
        if speed_ratio < neuron_ratio:
            print(f"  Performance scales super-linearly with network size")
        elif speed_ratio > neuron_ratio:
            print(f"  Performance scales sub-linearly with network size")
        else:
            print(f"  Performance scales linearly with network size")
    
    print()


def run_all_examples():
    """Run all examples sequentially"""
    print("NLM Advanced Python Examples")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Brain Simulation", example_basic_brain_simulation),
        ("World Interaction with Agent", example_world_interaction),
        ("Plasticity Learning Experiments", example_plasticity_learning),
        ("Advanced Configuration", example_advanced_configuration),
        ("Checkpointing and Replay", example_checkpointing_and_replay),
        ("Memory Systems Integration", example_memory_systems),
        ("Neuromodulation and Development", example_neuromodulation_and_development),
        ("Multi-Agent Scenario", example_multi_agent_scenario),
        ("Performance Monitoring", example_performance_monitoring),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"ERROR in {name}: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("All examples completed!")


if __name__ == "__main__":
    run_all_examples()