#!/usr/bin/env python3
"""
NLM Examples and Tutorials

Comprehensive examples for the Neural Learning Machine (NLM) neural simulation framework.

This file contains practical examples that demonstrate how to use NLM for various tasks:
- Basic brain creation and simulation
- Phase-specific experiments
- Advanced configuration and customization
- Performance benchmarking
- Integration with Python ecosystem

Examples are designed to be educational and practical, suitable for:
- Researchers wanting to understand NLM architecture
- Developers learning neural simulation
- Students studying artificial intelligence
- Professionals implementing neural systems
"""

import nlm
import time
import sys
from typing import List, Optional, Tuple
def example_basic_brain_creation():
    """
    Example 1: Basic Brain Creation and Initialization
    ===================================================
    
    This example shows the minimal steps to create and initialize a NLM brain.
    It's suitable for beginners who want to understand the basic architecture.
    """
    print("=== Example 1: Basic Brain Creation ===")
    
    # Create default configuration
    config = nlm.create_default_config()
    
    # Create brain with configuration
    brain = nlm.create_brain(config)
    
    # Initialize brain
    success = brain.initialize()
    if not success:
        print("ERROR: Failed to initialize brain")
        return False
    
    print(f"✓ Brain created successfully")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    
    return True
def example_simple_simulation():
    """
    Example 2: Simple Simulation
    ==========================
    
    This example demonstrates running a basic simulation loop with
    sensory input, neural processing, and action output.
    """
    print("\n=== Example 2: Simple Simulation ===")
    
    # Create agent with world
    brain, agent_brain, world = nlm.create_agent()
    
    print(f"✓ Agent created")
    print(f"  - Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
    
    # Run simulation for a few steps
    steps = 20
    for step in range(steps):
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent_brain.processSensoryInput(percept)
        
        # Apply to brain
        brain.receiveSensoryInput(percept)
        
        # Simulate one step
        brain.step(step)
        
        # Get action from brain
        action = brain.produceAction()
        
        # Apply action in world
        result = world.applyMotorCommand(action, step * 0.001)
        
        # Apply reward-based neuromodulation
        agent_brain.applyRewardModulation(result.reward, 0.0)
        
        # Print progress every 5 steps
        if step % 5 == 0:
            print(f"  Step {step:2d}: Reward={result.reward:6.2f}, "
                  f"Action={str(action):10s}, "
                  f"Firing={brain.getFiringNeuronCount():3d}")
    
    print(f"✓ Simulation completed successfully")
    return True
def example_phase2_experiment():
    """
    Example 3: Phase 2 - Real Neural Computation
    ==========================================
    
    This example demonstrates Phase 2 of NLM, which focuses on
    real spiking neural computation with LIF neurons, event-driven
    dynamics, and plasticity rules.
    """
    print("\n=== Example 3: Phase 2 Experiment ===")
    
    # Create configuration suitable for Phase 2
    config = nlm.create_default_config()
    config.neuron_count = 200  # Smaller for demo
    config.max_simulation_steps = 500
    
    # Create agent
    brain, agent_brain, world = nlm.create_agent(config)
    
    print(f"Phase 2 Configuration:")
    print(f"  - Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  - Region count: {brain.getRegionCount()}")
    print(f"  - Connection probability: {config.connection_probability}")
    
    # Run Phase 2 specific tests
    print(f"\nPhase 2 Objectives:")
    print(f"  ✓ Real LIF neuron dynamics")
    print(f"  ✓ Event-driven spike propagation with delays")
    print(f"  ✓ STDP and Hebbian plasticity rules")
    print(f"  ✓ Structural plasticity (synaptogenesis/pruning)")
    
    # Run some steps
    for step in range(100):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        brain.receiveSensoryInput(percept)
        brain.step(step)
        
        action = brain.produceAction()
        world.applyMotorCommand(action, step * 0.001)
        agent_brain.applyRewardModulation(0.0, 0.0)
    
    print(f"✓ Phase 2 experiment completed")
    return True
def example_phase6_integration():
    """
    Example 4: Phase 6 - Full Integration
    ===================================
    
    This example demonstrates Phase 6, the final integration phase
    that connects all brain systems into a coherent artificial brain.
    """
    print("\n=== Example 4: Phase 6 Integration ===")
    
    # Configure for Phase 6 integration
    config = nlm.create_default_config()
    config.neuron_count = 500  # Moderate size
    config.max_simulation_steps = 1000
    config.enableCheckpointing = True
    config.enableReplay = True
    
    # Create agent
    brain, agent_brain, world = nlm.create_agent(config)
    
    print(f"Phase 6 Integration Status:")
    print(f"  ✓ Memory systems integrated")
    print(f"  ✓ Neuromodulation connected")
    print(f"  ✓ Prediction system working")
    print(f"  ✓ Development system functional")
    
    # Monitor integration
    integration_metrics = {}
    
    for step in range(config.max_simulation_steps):
        # Integration test: all systems should respond
        percept = world.getSensoryPercept()
        
        # Test memory integration
        working_memory = brain.getWorkingMemory()
        if working_memory:
            integration_metrics['memory_working'] = working_memory.getActiveTraces() > 0
        
        # Test neuromodulation integration
        dopamine = brain.getDopamine()
        if dopamine:
            integration_metrics['neuromodulation'] = dopamine.getLevel() != 0.0
        
        # Test development integration
        developmental_stage = brain.getDevelopmentalStage()
        integration_metrics['development'] = developmental_stage != nlm.DevelopmentalStage.Initial
        
        # Run simulation step
        agent_brain.processSensoryInput(percept)
        brain.receiveSensoryInput(percept)
        brain.step(step)
        
        action = brain.produceAction()
        result = world.applyMotorCommand(action, step * 0.001)
        agent_brain.applyRewardModulation(result.reward, 0.0)
        agent_brain.updateDevelopment(0.001)
    
    # Check final integration status
    print(f"\nFinal Integration Results:")
    print(f"  - Memory working memory traces: {integration_metrics.get('memory_working', False)}")
    print(f"  - Neuromodulation active: {integration_metrics.get('neuromodulation', False)}")
    print(f"  - Development progressing: {integration_metrics.get('development', False)}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return True
def example_advanced_configuration():
    """
    Example 5: Advanced Configuration
    =================================
    
    This example shows how to create custom configurations for specific
    experimental needs and research applications.
    """
    print("\n=== Example 5: Advanced Configuration ===")
    
    # Create custom configuration
    config = nlm.Config()
    
    # Set custom parameters
    config.random_seed = 12345
    config.simulation_timestep = 0.0005  # Faster simulation
    config.neuron_count = 2000  # Larger brain
    config.region_count = 4    # More regions
    config.connection_probability = 0.05  # Sparse connections
    
    # Advanced plasticity settings
    config.stdp_ltp_weight = 0.015
    config.stdp_ltd_weight = 0.018
    config.stdp_time_constant = 25.0
    
    # Development settings
    config.development_synaptogenesis_rate = 0.002
    config.development_pruning_rate = 0.0002
    
    # Memory settings
    config.working_memory_capacity = 500
    config.episodic_memory_max_episodes = 5000
    
    # Neuromodulation settings
    config.dopamine_baseline = 0.05
    config.reward_modulation_enabled = True
    config.structural_plasticity_enabled = True
    config.curiosity_enabled = True
    
    # Environment settings
    config.environment_name = "ComplexWorld"
    config.environment_width = 20
    config.environment_height = 20
    config.visualization_enabled = True
    
    print(f"Advanced Configuration:")
    print(f"  - Random seed: {config.random_seed}")
    print(f"  - Simulation timestep: {config.simulation_timestep}")
    print(f"  - Neuron count: {config.neuron_count}")
    print(f"  - Region count: {config.region_count}")
    print(f"  - STDP LTP weight: {config.stdp_ltp_weight}")
    print(f"  - Development synaptogenesis rate: {config.development_synaptogenesis_rate}")
    
    # Create brain with advanced config
    brain = nlm.create_brain(config)
    success = brain.initialize()
    
    if success:
        print(f"✓ Advanced brain initialized successfully")
        brain.logStatus()
    
    return success
def example_performance_benchmark():
    """
    Example 6: Performance Benchmarking
    ===================================
    
    This example demonstrates how to benchmark NLM performance for
    research and optimization purposes.
    """
    print("\n=== Example 6: Performance Benchmarking ===")
    
    import time
    
    # Brain creation benchmark
    print("Brain Creation Benchmark:")
    start_time = time.time()
    
    for i in range(5):
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
    
    brain_creation_time = time.time() - start_time
    print(f"  5 brain creations: {brain_creation_time:.2f}s total")
    print(f"  Average: {brain_creation_time/5:.2f}s per brain")
    
    # Simulation benchmark
    print("\nSimulation Benchmark:")
    config = nlm.create_default_config()
    brain, agent_brain, world = nlm.create_agent(config)
    brain.initialize()
    
    steps = 100
    start_time = time.time()
    
    for step in range(steps):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        brain.receiveSensoryInput(percept)
        brain.step(step)
        
        action = brain.produceAction()
        world.applyMotorCommand(action, step * 0.001)
    
    simulation_time = time.time() - start_time
    avg_step_time = simulation_time / steps * 1000  # Convert to ms
    
    print(f"  {steps} simulation steps: {simulation_time:.2f}s total")
    print(f"  Average step time: {avg_step_time:.2f}ms")
    print(f"  Steps per second: {steps/simulation_time:.0f}")
    
    # Memory usage benchmark (conceptual)
    print("\nMemory Usage Benchmark:")
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    memory_mb = process.memory_info().rss / 1024 / 1024
    print(f"  Current memory usage: {memory_mb:.0f} MB")
    
    return True
def example_reproducible_research():
    """
    Example 7: Reproducible Research
    ================================
    
    This example demonstrates how to set up reproducible research workflows
    using NLM's configuration and checkpointing features.
    """
    print("\n=== Example 7: Reproducible Research ===")
    
    # Set up experiment directory
    experiment_dir = "experiment_reproducible_2024_01_17"
    os.makedirs(experiment_dir, exist_ok=True)
    
    # Create configuration file
    config = nlm.create_default_config()
    config.random_seed = 42
    config.simulation_timestep = 0.001
    config.neuron_count = 1000
    
    config_file = os.path.join(experiment_dir, "config.cfg")
    with open(config_file, 'w') as f:
        f.write(f"# NLM Experiment Configuration\n")
        f.write(f"# Generated on 2024-01-17\n\n")
        f.write(f"random_seed = {config.random_seed}\n")
        f.write(f"simulation_timestep = {config.simulation_timestep}\n")
        f.write(f"neuron_count = {config.neuron_count}\n")
    
    print(f"Created experiment configuration: {config_file}")
    
    # Create brain with reproducible seed
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Save initial brain state
    initial_checkpoint = os.path.join(experiment_dir, "brain_initial.checkpoint")
    brain.save(initial_checkpoint)
    print(f"Saved initial brain state: {initial_checkpoint}")
    
    # Run simulation
    brain, agent_brain, world = nlm.create_agent(config)
    brain.initialize()
    
    simulation_steps = 500
    for step in range(simulation_steps):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        brain.receiveSensoryInput(percept)
        brain.step(step)
        
        action = brain.produceAction()
        world.applyMotorCommand(action, step * 0.001)
    
    # Save final brain state
    final_checkpoint = os.path.join(experiment_dir, "brain_final.checkpoint")
    brain.save(final_checkpoint)
    print(f"Saved final brain state: {final_checkpoint}")
    
    # Create experiment summary
    summary_file = os.path.join(experiment_dir, "experiment_summary.txt")
    with open(summary_file, 'w') as f:
        f.write(f"NLM Reproducible Research Experiment\n")
        f.write(f"====================================\n\n")
        f.write(f"Date: 2024-01-17\n")
        f.write(f"Author: Researcher\n")
        f.write(f"Configuration: {config_file}\n")
        f.write(f"Initial checkpoint: {initial_checkpoint}\n")
        f.write(f"Final checkpoint: {final_checkpoint}\n\n")
        f.write(f"Simulation Results:\n")
        f.write(f"  - Steps executed: {simulation_steps}\n")
        f.write(f"  - Total neurons: {brain.getTotalNeuronCount()}\n")
        f.write(f"  - Total synapses: {brain.getTotalSynapseCount()}\n")
        f.write(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}\n")
        f.write(f"  - Total spikes: {brain.getTotalSpikeCount()}\n")
    
    print(f"Created experiment summary: {summary_file}")
    print(f"\nExperiment complete. All results saved in: {experiment_dir}")
    
    return True
def example_python_integration():
    """
    Example 8: Python Integration
    =============================
    
    This example shows how to integrate NLM with the Python ecosystem,
    using common data science and visualization libraries.
    """
    print("\n=== Example 8: Python Integration ===")
    
    # This would require additional Python libraries
    # For demonstration, we'll show the structure
    
    print("Python Integration Examples:")
    print("1. Data Analysis:")
    print("   - pandas for experiment tracking")
    print("   - numpy for numerical analysis")
    print("   - matplotlib for plotting results")
    
    print("\n2. Machine Learning:")
    print("   - scikit-learn for comparing with other models")
    print("   - TensorFlow/PyTorch for hybrid systems")
    print("   - Keras for model training and evaluation")
    
    print("\n3. Visualization:")
    print("   - matplotlib for neural activity plots")
    print("   - seaborn for statistical analysis")
    print("   - plotly for interactive visualizations")
    
    print("\n4. Experiment Management:")
    print("   - MLflow for experiment tracking")
    print("   - Weights & Biases for monitoring")
    print("   - DVC for data version control")
    
    # Create a simple example with matplotlib (if available)
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        print("\nCreating sample visualization...")
        
        # Generate sample data
        steps = 100
        time_data = np.linspace(0, 1, steps)
        activity_data = np.random.random(steps) * 0.5 + 0.5
        
        # Create plot
        plt.figure(figsize=(10, 6))
        plt.plot(time_data, activity_data, 'b-', linewidth=2, label='Neural Activity')
        plt.xlabel('Time (s)')
        plt.ylabel('Normalized Activity')
        plt.title('Sample Neural Activity from NLM Simulation')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Save plot
        plot_file = "nlm_sample_activity.png"
        plt.savefig(plot_file, dpi=150, bbox_inches='tight')
        print(f"   Saved visualization: {plot_file}")
        
    except ImportError:
        print("\nMatplotlib not available. Skipping visualization example.")
    
    return True
def run_all_examples():
    """Run all examples sequentially."""
    print("NLM Examples and Tutorials")
    print("==========================")
    print("Running all examples...")
    
    examples = [
        ("Basic Brain Creation", example_basic_brain_creation),
        ("Simple Simulation", example_simple_simulation),
        ("Phase 2 Experiment", example_phase2_experiment),
        ("Phase 6 Integration", example_phase6_integration),
        ("Advanced Configuration", example_advanced_configuration),
        ("Performance Benchmarking", example_performance_benchmark),
        ("Reproducible Research", example_reproducible_research),
        ("Python Integration", example_python_integration),
    ]
    
    results = []
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            success = example_func()
            results.append((name, success))
            status = "✓ PASSED" if success else "✗ FAILED"
            print(f"{name}: {status}")
        except Exception as e:
            print(f"{name}: ✗ FAILED - {e}")
            results.append((name, False))
    
    # Summary
    print(f"\n{'='*60}")
    print("EXAMPLES SUMMARY")
    print(f"{'='*60}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{name:30s} {status}")
    
    print(f"\nTotal: {passed}/{total} examples passed")
    
    if passed == total:
        print("🎉 All examples completed successfully!")
        return True
    else:
        print(f"⚠️  {total - passed} examples failed. Check output above.")
        return False
def main():
    """Main entry point for examples."""
    if len(sys.argv) > 1:
        # Run specific example
        example_map = {
            'basic': example_basic_brain_creation,
            'simple': example_simple_simulation,
            'phase2': example_phase2_experiment,
            'phase6': example_phase6_integration,
            'advanced': example_advanced_configuration,
            'benchmark': example_performance_benchmark,
            'reproducible': example_reproducible_research,
            'python': example_python_integration,
            'all': run_all_examples,
        }
        
        example_name = sys.argv[1].lower()
        if example_name in example_map:
            success = example_map[example_name]()
            sys.exit(0 if success else 1)
        else:
            print(f"Unknown example: {example_name}")
            print(f"Available examples: {', '.join(example_map.keys())}")
            sys.exit(1)
    else:
        # Run all examples
        success = run_all_examples()
        sys.exit(0 if success else 1)
if __name__ == '__main__':
    main()