"""
NLM Python Bindings - Advanced Examples

This module provides advanced examples for using the NLM Python bindings,
demonstrating more complex scenarios and features.
"""

import pynlm
import time
import json

def create_advanced_brain():
    """
    Create a brain with advanced configuration.
    
    This example shows how to create a brain with specialized configuration
    for different research purposes.
    
    Returns:
        Configuration and brain object.
    """
    # Create advanced configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for sensory-motor learning
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.v_thresh", -55.0)
    config.set("brain.v_reset", -70.0)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.0005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced brain:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return config, brain

def developmental_learning_demo():
    """
    Demonstrate developmental learning over time.
    
    This example shows how brain development affects plasticity rates
    and learning capabilities.
    """
    print("\nDevelopmental Learning Demo")
    print("-" * 50)
    
    config, brain = create_advanced_brain()
    
    # Run simulation to observe development
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"\nStep {step}:")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("\nDevelopment complete!")
    return brain

def multi_agent_simulation():
    """
    Simulate multiple interacting agents.
    
    This example shows how to create and coordinate multiple agents
    within the same world environment.
    """
    print("\nMulti-Agent Simulation")
    print("-" * 50)
    
    # Create shared configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    # Create multiple agent brains
    agents = []
    brains = []
    
    for i in range(3):
        print(f"\nCreating agent {i+1}:")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        agents.append(agent)
        brains.append(brain)
        
        print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"  Agent configured for {agent.getSensoryInputSize()} sensory inputs")
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Initialize all agents with the world
    for i, agent in enumerate(agents):
        agent.initialize(world)
        print(f"\nInitialized agent {i+1} with world")
    
    # Run simulation with all agents
    print(f"\nRunning simulation with {len(agents)} agents...")
    
    for step in range(100):
        world.update(0.1)
        
        # Each agent processes sensory input and makes decisions
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brains[i].step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 25 == 0:
            print(f"\nStep {step}:")
            for i, brain in enumerate(brains):
                print(f"  Agent {i+1}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("\nMulti-agent simulation complete!")
    return brains, world, agents

def save_load_demo():
    """
    Demonstrate brain state saving and loading.
    
    This example shows how to save and load brain states for continuity
    in long-running simulations or for sharing trained models.
    """
    print("\nSave/Load State Demo")
    print("-" * 50)
    
    # Create and train a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Train for a while
    print("Training brain...")
    for step in range(100):
        brain.step(step)
        if step % 25 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    print(f"\nInitial state: {brain.getTotalSpikeCount()} total spikes")
    
    # Save state
    save_file = "advanced_demo_checkpoint.bin"
    brain.save(save_file)
    print(f"✓ Saved brain state to {save_file}")
    
    # Create new brain and load state
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    brain2.load(save_file)
    print(f"✓ Loaded brain state: {brain2.getTotalSpikeCount()} total spikes")
    
    # Verify they have the same state
    spikes_match = brain.getTotalSpikeCount() == brain2.getTotalSpikeCount()
    print(f"✓ Spikes match: {spikes_match}")
    
    # Clean up
    import os
    if os.path.exists(save_file):
        os.remove(save_file)
        print(f"✓ Cleaned up {save_file}")
    
    return brain, brain2

def benchmark_demo():
    """
    Demonstrate benchmarking capabilities.
    
    This example shows how to measure performance of NLM simulations.
    """
    print("\nBenchmark Demo")
    print("-" * 50)
    
    # Test different configurations
    configs = [
        ("Small", {"brain.neuron_count": 200}),
        ("Medium", {"brain.neuron_count": 500}),
        ("Large", {"brain.neuron_count": 1000})
    ]
    
    results = []
    
    for name, config_dict in configs:
        print(f"\nBenchmarking {name} brain (neurons: {config_dict['brain.neuron_count']})...")
        
        start_time = time.time()
        
        # Create simulation
        sim = pynlm.create_simulation(config_dict)
        brain = sim['brain']
        world = sim['world']
        agent = sim['brain']
        
        # Enable features for realistic benchmarking
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        steps = 100
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 20 == 0:
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        result = {
            'name': name,
            'neurons': brain.getTotalNeuronCount(),
            'steps': steps,
            'elapsed_seconds': elapsed,
            'steps_per_second': steps / elapsed if elapsed > 0 else 0,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        }
        
        results.append(result)
        
        print(f"  Completed {steps} steps in {elapsed:.2f} seconds")
        print(f"  Performance: {result['steps_per_second']:.1f} steps/second")
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Average firing rate: {result['avg_firing_rate']:.2f} Hz")
    
    # Summary
    print(f"\nBenchmark Summary:")
    print(f"  {'Configuration':<10} | {'Neurons':<8} | {'Steps/sec':<10} | {'Total Spikes':<12}")
    print(f"  {'='*60}")
    
    for result in results:
        print(f"  {result['name']:<10} | {result['neurons']:<8} | {result['steps_per_second']:<10.1f} | {result['total_spikes']:<12d}")
    
    return results

def main():
    """Run all advanced examples."""
    print("NLM Python Bindings - Advanced Examples")
    print("=" * 70)
    
    try:
        # Run developmental learning demo
        brain = developmental_learning_demo()
        
        # Run multi-agent simulation
        multi_agent_simulation()
        
        # Run save/load demo
        save_load_demo()
        
        # Run benchmark demo
        benchmark_results = benchmark_demo()
        
        print("\n" + "=" * 70)
        print("All advanced examples completed successfully!")
        print("\nKey advanced features demonstrated:")
        print("  ✓ Custom brain configuration")
        print("  ✓ Developmental learning over time")
        print("  ✓ Multi-agent simulation")
        print("  ✓ State saving and loading")
        print("  ✓ Performance benchmarking")
        print("\nThe NLM Python bindings support advanced use cases:")
        print("  • Complex neural network architectures")
        print("  • Developmental and learning simulations")
        print("  • Multi-agent coordination")
        print("  • Long-running simulation persistence")
        print("  • Performance analysis and optimization")
        
    except Exception as e:
        print(f"\n✗ Advanced examples failed: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
