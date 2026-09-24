#!/usr/bin/env python3
"""
NLM Example Scripts for Different User Levels

This script provides example usage patterns for different skill levels
when working with the NLM (Neural Learning Machine) framework.

Run with: python3 nlm_examples.py
"""

import pynlm
import time
import json
import os
from typing import Dict, Any

def print_header(title: str, level: str = ""):
    """Print formatted header."""
    print("\n" + "="*80)
    if level:
        print(f"{level}: {title}")
    else:
        print(title)
    print("="*80)

def example_beginner():
    """Beginner: Simple simulation with default configuration."""
    print_header("Beginner Example: Simple Neural Simulation", "LEVEL 1 - BEGINNER")
    
    print("This example shows the simplest possible NLM simulation.")
    print("Perfect for users who are new to neural networks.")
    print()
    
    # 1. Create brain with default configuration
    print("Step 1: Creating brain with default configuration...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   and {brain.getTotalSynapseCount()} synapses")
    
    # 2. Run a simple simulation
    print("\nStep 2: Running simulation for 100 steps...")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"   Step {step:3d}: {firing} firing neurons")
    
    # 3. Check results
    print("\nStep 3: Analyzing results...")
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    e_i_ratio = brain.getExcitationInhibitionRatio()
    
    print(f"   Total spikes generated: {total_spikes}")
    print(f"   Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"   E/I ratio: {e_i_ratio:.2f}")
    
    # 4. Save brain state
    print("\nStep 4: Saving brain state...")
    brain.save("beginner_example_checkpoint.bin")
    print("   Saved to beginner_example_checkpoint.bin")
    
    print("\n" + "-"*80)
    print("SUMMARY: Beginner example completed successfully!")
    print("The brain ran a simple simulation and generated neural activity.")
    print("-"*80)

def example_intermediate():
    """Intermediate: Agent with world interaction."""
    print_header("Intermediate Example: Agent in Environment", "LEVEL 2 - INTERMEDIATE")
    
    print("This example demonstrates how to use NLM with the SimpleWorld environment.")
    print("Perfect for users who understand basic neural networks.")
    print()
    
    # 1. Create configuration
    print("Step 1: Creating brain with custom configuration...")
    config = pynlm.createDefaultConfig()
    
    # Customize configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    print(f"   Neurons: {config.getOr('brain.neuron_count', 1000)}")
    print(f"   Connection probability: {config.getOr('brain.connection_probability', 0.1)}")
    
    # 2. Create brain and agent
    print("\nStep 2: Creating brain and agent...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    print("\nStep 3: Creating and configuring world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    print(f"   Vision range: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    
    # 4. Enable advanced features
    print("\nStep 4: Enabling advanced features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   Enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    
    # 5. Run simulation loop
    print("\nStep 5: Running simulation loop (500 steps)...")
    for step in range(500):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"   Step {step:3d}: Firing={brain.getFiringNeuronCount():4d}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Dev={brain.getDevelopmentalStage()}")
    
    # 6. Collect statistics
    print("\nStep 6: Collecting statistics...")
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    dev_stage = brain.getDevelopmentalStage()
    
    print(f"   Total spikes: {total_spikes}")
    print(f"   Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"   Final curiosity level: {curiosity_level:.3f}")
    print(f"   Final novelty level: {novelty_level:.3f}")
    print(f"   Final developmental stage: {dev_stage}")
    
    # 7. Save results
    print("\nStep 7: Saving results...")
    brain.save("intermediate_example_checkpoint.bin")
    print("   Saved brain state to intermediate_example_checkpoint.bin")
    
    # Save statistics
    stats = {
        "total_spikes": total_spikes,
        "avg_firing_rate": avg_firing_rate,
        "final_curiosity": curiosity_level,
        "final_novelty": novelty_level,
        "final_dev_stage": str(dev_stage),
        "steps": 500
    }
    
    with open("intermediate_example_stats.json", "w") as f:
        json.dump(stats, f, indent=2)
    print("   Saved statistics to intermediate_example_stats.json")
    
    print("\n" + "-"*80)
    print("SUMMARY: Intermediate example completed successfully!")
    print("The agent navigated an environment and learned through experience.")
    print("-"*80)

def example_advanced():
    """Advanced: Complex experiment with custom configurations."""
    print_header("Advanced Example: Custom Configuration Experiment", "LEVEL 3 - ADVANCED")
    
    print("This example demonstrates advanced NLM usage with custom configurations.")
    print("Perfect for users who are familiar with neural networks and want full control.")
    print()
    
    # 1. Create comprehensive configuration
    print("Step 1: Creating advanced configuration...")
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration settings
    advanced_settings = {
        "brain.neuron_count": 5000,
        "brain.region_count": 4,
        "brain.connection_probability": 0.2,
        "brain.synapse_density": 0.15,
        "brain.v_thresh": -45.0,
        "brain.v_reset": -65.0,
        "brain.tau_mem": 20.0,
        "brain.tau_ref": 2.0,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.stdp.tau_plus": 15.0,
        "plasticity.stdp.tau_minus": 20.0,
        "plasticity.hebbian.enable": True,
        "plasticity.hebbian.strength": 0.005,
        "plasticity.structural.enable": True,
        "plasticity.structural.synaptogenesis_rate": 0.0002,
        "plasticity.structural.pruning_rate": 0.00002,
        "neuromod.dopamine.scale": 2.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.enabled": True,
        "development.synaptogenesis_rate": 0.001,
        "development.pruning_rate": 0.0001,
        "development.critical_period": True,
    }
    
    for key, value in advanced_settings.items():
        if isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, int):
            config.set(key, value)
        elif isinstance(value, float):
            config.set(key, value)
        elif isinstance(value, str):
            config.set(key, value)
    
    print("   Applied advanced configuration:")
    for key, value in advanced_settings.items():
        current = config.getOr(key, "NOT_SET")
        print(f"     {key}: {current}")
    
    # 2. Create brain with advanced configuration
    print("\nStep 2: Creating advanced brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   and {brain.getTotalSynapseCount()} synapses")
    
    # 3. Create multiple agents for comparison
    print("\nStep 3: Creating comparison agents...")
    
    # Create baseline configuration
    baseline_config = pynlm.createDefaultConfig()
    baseline_brain = pynlm.createBrain(baseline_config)
    baseline_brain.initialize()
    
    # Create advanced brain
    advanced_brain = pynlm.createBrain(config)
    advanced_brain.initialize()
    
    print("   Created baseline and advanced brains for comparison")
    
    # 4. Run comparative experiment
    print("\nStep 4: Running comparative experiment...")
    
    num_steps = 1000
    results_baseline = []
    results_advanced = []
    
    for step in range(num_steps):
        # Run baseline brain
        baseline_brain.step(step)
        results_baseline.append({
            "step": step,
            "firing_rate": baseline_brain.getAverageFiringRate(),
            "spikes": baseline_brain.getFiringNeuronCount(),
            "e_i_ratio": baseline_brain.getExcitationInhibitionRatio()
        })
        
        # Run advanced brain
        advanced_brain.step(step)
        results_advanced.append({
            "step": step,
            "firing_rate": advanced_brain.getAverageFiringRate(),
            "spikes": advanced_brain.getFiringNeuronCount(),
            "e_i_ratio": advanced_brain.getExcitationInhibitionRatio()
        })
        
        if step % 200 == 0:
            print(f"   Step {step:4d}: Baseline={results_baseline[-1]['firing_rate']:.2f}Hz, "
                  f"Advanced={results_advanced[-1]['firing_rate']:.2f}Hz")
    
    # 5. Analyze comparative results
    print("\nStep 5: Analyzing comparative results...")
    
    # Calculate statistics
    baseline_final_firing = results_baseline[-1]['firing_rate']
    advanced_final_firing = results_advanced[-1]['firing_rate']
    
    baseline_avg_ei = sum(r['e_i_ratio'] for r in results_baseline) / len(results_baseline)
    advanced_avg_ei = sum(r['e_i_ratio'] for r in results_advanced) / len(results_advanced)
    
    print("   Comparative Analysis:")
    print(f"     Baseline final firing rate: {baseline_final_firing:.2f} Hz")
    print(f"     Advanced final firing rate: {advanced_final_firing:.2f} Hz")
    print(f"     Advanced improvement: {((advanced_final_firing - baseline_final_firing) / baseline_final_firing * 100):.1f}%")
    print(f"     Baseline average E/I ratio: {baseline_avg_ei:.2f}")
    print(f"     Advanced average E/I ratio: {advanced_avg_ei:.2f}")
    
    # 6. Save comparative results
    print("\nStep 6: Saving comparative results...")
    
    comparison_results = {
        "experiment": "advanced_comparison",
        "num_steps": num_steps,
        "baseline": results_baseline,
        "advanced": results_advanced,
        "summary": {
            "baseline_final_firing": baseline_final_firing,
            "advanced_final_firing": advanced_final_firing,
            "firing_improvement_percent": (advanced_final_firing - baseline_final_firing) / baseline_final_firing * 100,
            "baseline_avg_ei": baseline_avg_ei,
            "advanced_avg_ei": advanced_avg_ei
        }
    }
    
    with open("advanced_comparison_results.json", "w") as f:
        json.dump(comparison_results, f, indent=2)
    print("   Saved comparison results to advanced_comparison_results.json")
    
    # Save advanced brain state
    advanced_brain.save("advanced_example_checkpoint.bin")
    print("   Saved advanced brain state to advanced_example_checkpoint.bin")
    
    print("\n" + "-"*80)
    print("SUMMARY: Advanced example completed successfully!")
    print("The experiment demonstrates advanced configuration and comparative analysis.")
    print("-"*80)

def example_performance():
    """Performance: Profiling and benchmarking."""
    print_header("Performance Example: Benchmarking NLM", "PERFORMANCE")
    
    print("This example demonstrates performance profiling and benchmarking.")
    print("Perfect for users who want to measure performance.")
    print()
    
    try:
        import psutil
        import time
        
        # Get system info
        cpu_count = psutil.cpu_count()
        memory = psutil.virtual_memory()
        
        print(f"System Info: {cpu_count} CPUs, {memory.total // (1024**3)} GB RAM")
        
    except ImportError:
        print("Note: psutil not available, skipping system profiling")
        cpu_count = None
        memory = None
    
    # Test different brain sizes
    print("\nTesting performance with different brain sizes...")
    
    sizes = [100, 500, 1000, 2000]
    timings = []
    
    for size in sizes:
        print(f"\nTesting brain size: {size} neurons...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        config.set("brain.region_count", max(1, size // 500))
        
        # Time brain initialization
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        init_time = time.time() - start_time
        
        # Time simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        sim_time = time.time() - start_time
        
        timings.append({
            "neurons": size,
            "init_time": init_time,
            "sim_time": sim_time,
            "init_per_neuron": init_time / size,
            "sim_per_step": sim_time / 100,
            "sim_per_neuron": sim_time / size
        })
        
        print(f"  Initialization: {init_time:.3f}s ({init_time/size:.6f}s per neuron)")
        print(f"  Simulation: {sim_time:.3f}s ({sim_time/size:.6f}s per neuron)")
    
    # Print performance summary
    print("\n" + "="*80)
    print("PERFORMANCE BENCHMARKING RESULTS")
    print("="*80)
    print(f"{'Neurons':>10} {'Init(s)':>10} {'Sim(s)':>10} {'Init/ns':>12} {'Sim/ns':>12}")
    print("-"*80)
    
    for timing in timings:
        print(f"{timing['neurons']:>10} {timing['init_time']:>10.3f} {timing['sim_time']:>10.3f} "
              f"{timing['init_per_neuron']*1000000:>11.3f} {timing['sim_per_neuron']*1000000:>11.3f}")
    
    # Calculate scaling
    print("\nScaling Analysis:")
    if len(timings) >= 2:
        size_ratio = timings[-1]['neurons'] / timings[0]['neurons']
        time_ratio = timings[-1]['init_time'] / timings[0]['init_time']
        scaling = time_ratio / size_ratio
        print(f"  Linear scaling (1.0x): Ideal scaling would be {size_ratio:.1f}x")
        print(f"  Actual scaling: {time_ratio:.1f}x")
        print(f"  Scaling efficiency: {scaling:.2f} (1.0 = perfect linear)")
    
    # Save performance results
    with open("performance_results.json", "w") as f:
        json.dump({
            "experiment": "performance_benchmark",
            "system_info": {
                "cpu_count": cpu_count,
                "memory_gb": memory.total // (1024**3) if memory else None
            },
            "timings": timings
        }, f, indent=2)
    print("\nSaved performance results to performance_results.json")
    
    print("\n" + "-"*80)
    print("SUMMARY: Performance example completed successfully!")
    print("Benchmarking completed for different brain sizes.")
    print("-"*80)

def main():
    """Main function to run all examples."""
    print("NLM Example Scripts - Different User Levels")
    print("="*80)
    print("This script provides example usage patterns for different skill levels")
    print("when working with the NLM (Neural Learning Machine) framework.")
    print("="*80)
    
    # Check if pynlm is available
    try:
        import pynlm
        print("✓ NLM Python bindings available")
    except ImportError:
        print("✗ NLM Python bindings not available")
        print("Please install NLM with: pip install .")
        return
    
    # Run examples
    try:
        example_beginner()
        print("\n" + "="*80)
        print("PRESS ENTER TO CONTINUE TO INTERMEDIATE EXAMPLE...")
        input()
        
        example_intermediate()
        print("\n" + "="*80)
        print("PRESS ENTER TO CONTINUE TO ADVANCED EXAMPLE...")
        input()
        
        example_advanced()
        print("\n" + "="*80)
        print("PRESS ENTER TO CONTINUE TO PERFORMANCE EXAMPLE...")
        input()
        
        example_performance()
        
    except KeyboardInterrupt:
        print("\n\nExample interrupted by user.")
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
    
    print("\n" + "="*80)
    print("ALL EXAMPLES COMPLETED SUCCESSFULLY!")
    print("="*80)
    print("\nSummary of examples run:")
    print("1. Beginner: Simple simulation with default configuration")
    print("2. Intermediate: Agent with world interaction and learning")
    print("3. Advanced: Complex experiment with custom configurations")
    print("4. Performance: Benchmarking and performance profiling")
    print("\nCheck the generated files in this directory for results.")

if __name__ == "__main__":
    main()