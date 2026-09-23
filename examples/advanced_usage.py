#!/usr/bin/env python3
"""
Advanced NLM Examples for Experienced Users

This script demonstrates advanced usage patterns and features for experienced users
working with the NLM neural simulation framework.

Key features covered:
- Advanced configuration management
- Performance monitoring
- Debugging and troubleshooting
- Custom simulation workflows
- Batch processing and simulation management
"""

import pynlm
import time
import json
import numpy as np

def example_1_advanced_configuration():
    """Example 1: Advanced configuration with validation and validation"""
    print("=== Example 1: Advanced Configuration ===")
    
    # Create and configure a brain with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration for research simulations
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.region_count", 3)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Validate configuration
    is_valid = pynlm.validate_config(config)
    print(f"Configuration valid: {is_valid}")
    
    # Create brain with validated configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Display brain statistics
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain created with {brain.getTotalSynapseCount()} synapses")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

def example_2_performance_monitoring():
    """Example 2: Performance monitoring during simulation"""
    print("\n=== Example 2: Performance Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial system info
    system_info = pynlm.get_system_info()
    print(f"System: {system_info['version']} by {system_info['author']}")
    print(f"Model: {system_info['neuron_model']} neurons with {system_info['plasticity_rules']}")
    
    # Performance monitoring during simulation
    start_time = time.time()
    step_times = []
    
    for step in range(100):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        step_times.append(step_time)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"spike rate: {brain.getAverageFiringRate():.2f} Hz")
    
    avg_step_time = np.mean(step_times)
    std_step_time = np.std(step_times)
    total_time = time.time() - start_time
    
    print(f"\nPerformance Summary:")
    print(f"Total simulation time: {total_time:.3f} seconds")
    print(f"Average step time: {avg_step_time*1000:.2f} ± {std_step_time*1000:.2f} ms")
    print(f"Steps per second: {100/total_time:.1f}")
    
    return brain, step_times

def example_3_debugging_and_troubleshooting():
    """Example 3: Debugging and troubleshooting utilities"""
    print("\n=== Example 3: Debugging and Troubleshooting ===")
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check system info for debugging
    system_info = pynlm.get_system_info()
    print(f"System info: {system_info}")
    
    # Create agent for debugging
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all advanced features for debugging
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled all advanced features for debugging")
    
    # Run simulation with debugging output
    for step in range(20):
        world.update(0.1)
        
        # Get sensory input for debugging
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Debug output every 5 steps
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Action type: {action.getType() if action else 'None'}")
    
    return brain, agent, world

def example_4_batch_processing():
    """Example 4: Batch processing multiple simulations"""
    print("\n=== Example 4: Batch Processing ===")
    
    # Configuration for batch processing
    base_config = pynlm.createDefaultConfig()
    
    # Different configurations to test
    configs = []
    for neuron_count in [100, 500, 1000]:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        configs.append(config)
    
    results = []
    
    for i, config in enumerate(configs):
        print(f"\nRunning simulation {i+1} with {config.get('brain.neuron_count')} neurons...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(200):
            brain.step(step)
        
        end_time = time.time()
        
        # Collect results
        result = {
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "firing": brain.getFiringNeuronCount(),
            "spikes": brain.getTotalSpikeCount(),
            "rate": brain.getAverageFiringRate(),
            "time_seconds": end_time - start_time,
            "steps_per_second": 200 / (end_time - start_time)
        }
        
        results.append(result)
        print(f"  Completed in {result['time_seconds']:.3f}s at {result['steps_per_second']:.1f} steps/s")
        print(f"  Final firing rate: {result['rate']:.2f} Hz")
    
    # Summary of batch results
    print(f"\nBatch Processing Summary:")
    for i, result in enumerate(results):
        print(f"  Config {i+1}: {result['neurons']} neurons, "
              f"{result['rate']:.2f} Hz, {result['steps_per_second']:.1f} steps/s")
    
    return results

def example_5_custom_simulation_workflow():
    """Example 5: Custom simulation workflow with checkpoints and replay"""
    print("\n=== Example 5: Custom Simulation Workflow ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Custom simulation workflow
    simulation_phases = {
        "exploration": {"duration": 100, "curiosity_weight": 1.0},
        "exploitation": {"duration": 150, "reward_weight": 1.0},
        "consolidation": {"duration": 200, "replay_weight": 1.0}
    }
    
    phase_results = {}
    
    for phase_name, phase_config in simulation_phases.items():
        print(f"\nStarting {phase_name} phase...")
        phase_start = time.time()
        
        # Modify agent behavior for each phase
        if phase_name == "exploration":
            agent.enableCuriosity(True)
            agent.enableRewardModulation(False)
        elif phase_name == "exploitation":
            agent.enableCuriosity(False)
            agent.enableRewardModulation(True)
        elif phase_name == "consolidation":
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
        
        # Run phase simulation
        for step in range(phase_config["duration"]):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation with phase-specific weights
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward * phase_config.get("reward_weight", 1.0),
                                      predicted * phase_config.get("curiosity_weight", 1.0))
        
        phase_end = time.time()
        phase_results[phase_name] = {
            "duration_seconds": phase_end - phase_start,
            "steps": phase_config["duration"],
            "final_firing_rate": brain.getAverageFiringRate(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError()
        }
        
        print(f"  Completed {phase_name} phase in {phase_results[phase_name]['duration_seconds']:.2f}s")
        print(f"  Final state: {phase_results[phase_name]['final_firing_rate']:.2f} Hz, "
              f"curiosity: {phase_results[phase_name]['curiosity_level']:.3f}")
        
        # Save checkpoint after each phase
        checkpoint_file = f"checkpoint_{phase_name}.bin"
        brain.save(checkpoint_file)
        print(f"  Saved checkpoint to {checkpoint_file}")
    
    print(f"\nCustom workflow summary:")
    for phase_name, results in phase_results.items():
        print(f"  {phase_name}: {results['duration_seconds']:.2f}s, "
              f"{results['final_firing_rate']:.2f} Hz, "
              f"curiosity: {results['curiosity_level']:.3f}")
    
    return brain, agent, world, phase_results

def main():
    """Run all advanced examples"""
    print("Advanced NLM Examples for Experienced Users")
    print("=" * 50)
    
    try:
        # Example 1: Advanced configuration
        brain1 = example_1_advanced_configuration()
        
        # Example 2: Performance monitoring
        brain2, step_times = example_2_performance_monitoring()
        
        # Example 3: Debugging and troubleshooting
        brain3, agent3, world3 = example_3_debugging_and_troubleshooting()
        
        # Example 4: Batch processing
        batch_results = example_4_batch_processing()
        
        # Example 5: Custom simulation workflow
        brain5, agent5, world5, phase_results = example_5_custom_simulation_workflow()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nKey features demonstrated:")
        print("  • Advanced configuration with validation")
        print("  • Performance monitoring and statistics")
        print("  • Debugging and troubleshooting utilities")
        print("  • Batch processing for parameter sweeps")
        print("  • Custom simulation workflows with checkpoints")
        print("  • Phase-based learning with neuromodulation")
        
        return True
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
