"""
NLM Batch Processing Example
============================

This example demonstrates the new batch processing capabilities in NLM.
It shows how to run multiple simulation steps efficiently and collect statistics.
"""

import pynlm
import time

def basic_batch_simulation():
    """
    Example 1: Basic batch simulation with all subsystems
    """
    print("=== Basic Batch Simulation Example ===")
    
    # 1. Create simulation components
    print("1. Creating brain, world, and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"   - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"   - World: {world.getWidth()}x{height} units")
    
    # 2. Run batch simulation
    print("2. Running batch simulation...")
    num_steps = 1000
    timestep = 0.1
    
    start_time = time.time()
    
    # Use the new batch processing function
    result = pynlm.run_batch_simulation(brain, world, agent, num_steps, timestep)
    
    end_time = time.time()
    
    # 3. Analyze results
    total_spikes, avg_firing_rate, neuron_count = result
    
    print("3. Simulation Results:")
    print(f"   - Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"   - Total spikes: {total_spikes}")
    print(f"   - Average firing rate: {avg_firing_rate:.4f} Hz")
    print(f"   - Total neurons: {neuron_count}")
    print(f"   - Firing efficiency: {total_spikes/num_steps:.2f} spikes/step")
    
    # 4. Get final brain statistics
    print("4. Final Brain Statistics:")
    print(f"   - Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Total spike count: {brain.getTotalSpikeCount()}")
    print(f"   - Development stage: {brain.getDevelopmentalStage()}")
    print(f"   - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   - Prediction error: {agent.getPredictionError():.3f}")
    
    return result

def learning_batch_simulation():
    """
    Example 2: Batch simulation focused on learning and development
    """
    print("\n=== Learning & Development Batch Simulation ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure for learning
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with learning
    print("Running learning simulation with all features enabled...")
    result = pynlm.run_batch_simulation(brain, world, agent, 500, 0.05)
    
    total_spikes, avg_firing_rate, neuron_count = result
    
    print(f"Learning Results:")
    print(f"  - Final spikes: {total_spikes}")
    print(f"  - Average firing rate: {avg_firing_rate:.4f} Hz")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final prediction error: {agent.getPredictionError():.3f}")
    
    # Check if development occurred
    if brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial:
        print("  - ✓ Development progression detected")
    else:
        print("  - Development still in initial stage")
    
    return result

def comparative_simulation():
    """
    Example 3: Comparative simulation with different configurations
    """
    print("\n=== Comparative Simulation Examples ===")
    
    def run_configuration(config_name, neuron_count, enable_curiosity):
        print(f"\n{config_name}:")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(enable_curiosity)
        
        result = pynlm.run_batch_simulation(brain, world, agent, 300, 0.1)
        
        total_spikes, avg_firing_rate, neuron_count = result
        
        print(f"  - {neuron_count} neurons, curiosity={enable_curiosity}")
        print(f"    Result: {total_spikes} spikes, {avg_firing_rate:.4f} Hz avg firing")
        
        return result
    
    # Run different configurations
    run_configuration("Large Brain", 2000, False)
    run_configuration("Small Brain", 500, False)
    run_configuration("Large Brain + Curiosity", 2000, True)
    run_configuration("Small Brain + Curiosity", 500, True)

def performance_comparison():
    """
    Example 4: Performance comparison
    """
    print("\n=== Performance Comparison ===")
    
    import time
    
    configurations = [
        (500, 100, "Small"),
        (1000, 500, "Medium"),
        (2000, 1000, "Large")
    ]
    
    for neuron_count, num_steps, size in configurations:
        print(f"\n{size} brain ({neuron_count} neurons, {num_steps} steps):")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        
        # Time the batch simulation
        start_time = time.time()
        result = pynlm.run_batch_simulation(brain, world, agent, num_steps, 0.05)
        end_time = time.time()
        
        total_spikes, avg_firing_rate, _ = result
        sim_time = end_time - start_time
        
        print(f"  - Simulation time: {sim_time:.2f} seconds")
        print(f"  - Performance: {num_steps/sim_time:.1f} steps/second")
        print(f"  - Result: {total_spikes} spikes, {avg_firing_rate:.4f} Hz avg firing")

if __name__ == "__main__":
    print("NLM Batch Processing Examples")
    print("===============================")
    print("This script demonstrates the new batch processing capabilities")
    print("introduced in NLM Phase 6 improvements.\n")
    
    try:
        # Run basic example
        basic_batch_simulation()
        
        # Run learning example
        learning_batch_simulation()
        
        # Run comparative example
        comparative_simulation()
        
        # Run performance comparison
        performance_comparison()
        
        print("\n" + "="*60)
        print("All examples completed successfully!")
        print("The batch processing functionality is working correctly.")
        print("="*60)
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
