#!/usr/bin/env python3
"""
Example scripts demonstrating the enhanced NLM Python API.

These examples showcase the new convenience methods, utility functions,
and visualization capabilities added to pynlm.
"""

import pynlm
import time

def basic_usage():
    """Example 1: Basic usage with context manager"""
    print("=== Example 1: Basic usage with context manager ===")
    
    # Create brain using convenience function
    brain = pynlm.utils.create_brain_with_preset("medium")
    
    # Use context manager for automatic initialization/cleanup
    with brain:
        print(f"Initialized brain with {brain.getTotalNeuronCount()} neurons")
        print(f"Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Run episode
        brain.runEpisode(100)
        
        # Get statistics
        stats = brain.getStats()
        print(f"Final neuron count: {stats['total_neurons']}")
        print(f"Final firing rate: {stats['average_firing_rate']:.3f}")
    
    print("Episode completed with automatic cleanup\n")

def utility_functions():
    """Example 2: Using utility functions"""
    print("=== Example 2: Using utility functions ===")
    
    # Create multiple brains using presets
    small_brain = pynlm.utils.create_brain_with_preset("small")
    medium_brain = pynlm.utils.create_brain_with_preset("medium")
    large_brain = pynlm.utils.create_brain_with_preset("large")
    
    print(f"Small brain: {small_brain.getTotalNeuronCount()} neurons")
    print(f"Medium brain: {medium_brain.getTotalNeuronCount()} neurons")
    print(f"Large brain: {large_brain.getTotalNeuronCount()} neurons")
    
    # Compare brains
    brains = [small_brain, medium_brain, large_brain]
    comparison = pynlm.utils.compare_brains(brains, 50)
    
    for brain_name, brain_stats in comparison.items():
        print(f"\n{brain_name}:")
        print(f"  Neurons: {brain_stats['neurons']}")
        print(f"  Synapses: {brain_stats['synapses']}")
        print(f"  Active neurons: {brain_stats['active_neurons']}")
        print(f"  Firing neurons: {brain_stats['firing_neurons']}")
    
    print("\nBrain comparison completed\n")

def experiment_management():
    """Example 3: Running multiple experiments"""
    print("=== Example 3: Running multiple experiments ===")
    
    # Define a factory function for different brain types
    def create_curiosity_brain():
        config = pynlm.utils.create_experiment_config("curiosity")
        brain = pynlm.createBrain(config)
        return brain
    
    def create_development_brain():
        config = pynlm.utils.create_experiment_config("development")
        brain = pynlm.createBrain(config)
        return brain
    
    def create_prediction_brain():
        config = pynlm.utils.create_experiment_config("prediction")
        brain = pynlm.createBrain(config)
        return brain
    
    # Run repeated experiments
    experiments = {
        "curiosity": create_curiosity_brain,
        "development": create_development_brain,
        "prediction": create_prediction_brain,
    }
    
    results = {}
    for exp_name, brain_factory in experiments.items():
        print(f"Running {exp_name} experiment...")
        run_results = pynlm.utils.run_repeated_experiments(
            brain_factory, exp_name, numRuns=5, stepsPerRun=100
        )
        results[exp_name] = run_results
        
        # Show summary
        for run_key, run_data in run_results.items():
            print(f"  {run_key}: {run_data['final_spikes']} spikes, "
                  f"Stage {run_data['developmental_stage']}")
    
    print("\nAll experiments completed\n")

def benchmark_and_performance():
    """Example 4: Benchmarking different brain sizes"""
    print("=== Example 4: Benchmarking different brain sizes ===")
    
    # Benchmark small brain
    print("Benchmarking small brain...")
    small_bench = pynlm.utils.benchmark_performance(neuronCount=100, steps=100)
    print(f"Small brain: {small_bench['total_time_ms']:.2f} ms for {small_bench['steps']} steps")
    print(f"  Average time per step: {small_bench['time_per_step_ms']:.2f} ms")
    print(f"  Spikes per step: {small_bench['spikes_per_step']:.2f}")
    
    # Benchmark medium brain
    print("\nBenchmarking medium brain...")
    medium_bench = pynlm.utils.benchmark_performance(neuronCount=1000, steps=100)
    print(f"Medium brain: {medium_bench['total_time_ms']:.2f} ms for {medium_bench['steps']} steps")
    print(f"  Average time per step: {medium_bench['time_per_step_ms']:.2f} ms")
    print(f"  Spikes per step: {medium_bench['spikes_per_step']:.2f}")
    
    # Benchmark large brain
    print("\nBenchmarking large brain...")
    large_bench = pynlm.utils.benchmark_performance(neuronCount=5000, steps=50)
    print(f"Large brain: {large_bench['total_time_ms']:.2f} ms for {large_bench['steps']} steps")
    print(f"  Average time per step: {large_bench['time_per_step_ms']:.2f} ms")
    print(f"  Spikes per step: {large_bench['spikes_per_step']:.2f}")
    
    print("\nBenchmarking completed\n")

def configuration_presets():
    """Example 5: Using configuration presets"""
    print("=== Example 5: Using configuration presets ===")
    
    # Get all available presets
    presets = pynlm.utils.get_all_presets()
    
    print("Available presets:")
    for preset_name, preset_config in presets.items():
        print(f"\n{preset_name.upper()} preset:")
        for key, value in preset_config.items():
            print(f"  {key}: {value}")
    
    # Create brain with different presets
    print("\nCreating brains with different presets:")
    for preset_name in ["small", "medium", "large"]:
        brain = pynlm.utils.create_brain_with_preset(preset_name)
        stats = brain.getStats()
        print(f"{preset_name}: {stats['total_neurons']} neurons, "
              f"{stats['total_spikes']} spikes")
    
    print("\nConfiguration presets example completed\n")

def advanced_api_features():
    """Example 6: Advanced API features"""
    print("=== Example 6: Advanced API features ===")
    
    # Create brain and configure it
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Customize configuration
    brain.getConfig().set("plasticity.stdp.enable", True)
    brain.getConfig().set("neuromod.curiosity.enable", True)
    brain.getConfig().set("brain.v_thresh", -55.0)
    
    brain.initialize()
    
    # Run simulation with statistics
    print("Running advanced simulation...")
    start_time = time.time()
    
    for step in range(200):
        brain.step(step)
        
        # Log progress
        if step % 50 == 0:
            stats = brain.getStats()
            print(f"Step {step}: {stats['firing_neurons']} firing neurons, "
                  f"{stats['average_firing_rate']:.3f} firing rate")
    
    end_time = time.time()
    final_stats = brain.getStats()
    
    print(f"\nAdvanced simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final state: {final_stats['total_spikes']} total spikes")
    print(f"Developmental stage: {final_stats['developmental_stage']}")
    print(f"E/I ratio: {final_stats['excitation_inhibition_ratio']:.3f}")
    
    print("\nAdvanced API features example completed\n")

def complete_agent_example():
    """Example 7: Complete agent simulation with all systems"""
    print("=== Example 7: Complete agent simulation ===")
    
    # 1. Create configuration
    config = pynlm.utils.create_experiment_config("curiosity")
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    num_steps = 500
    print(f"Running complete agent simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal() and percept.getInternal()[0] is not None:
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("\nComplete agent simulation finished!")
    print("\nComplete agent example completed\n")

def main():
    """Run all examples"""
    print("NLM Python API Examples")
    print("=" * 50)
    print()
    
    basic_usage()
    utility_functions()
    experiment_management()
    benchmark_and_performance()
    configuration_presets()
    advanced_api_features()
    complete_agent_example()
    
    print("All examples completed successfully!")
    print("\nThe enhanced pynlm library provides:")
    print("- Context manager support for automatic resource management")
    print("- Utility functions for common operations")
    print("- Experiment management tools")
    print("- Performance benchmarking")
    print("- Configuration presets")
    print("- Advanced API features")
    print("- Complete agent simulation capabilities")

if __name__ == "__main__":
    main()
