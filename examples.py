#!/usr/bin/env python3
"""
NLM Python Usage Examples
=========================

This file demonstrates practical usage of the enhanced NLM Python bindings.
Copy and modify these examples for your own projects.

Usage:
    python3 examples.py
"""

import sys
import os
import time
import math

# Add the Python bindings directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    # Try to import both binding versions
    import pynlm as nlm_original
    import pynlm_enhanced as nlm
    print("Both NLM binding versions loaded successfully")
except ImportError as e:
    print(f"Import error: {e}")
    print("Please ensure pybind11 is properly installed and bindings are built")
    sys.exit(1)

def example_basic_original():
    """Example using the original NLM bindings"""
    print("\n" + "="*60)
    print("Example 1: Basic Original NLM Usage")
    print("="*60)
    
    # Create default configuration
    config = nlm_original.createDefaultConfig()
    print(f"Created default config: {config.summary()}")
    
    # Create brain
    brain = nlm_original.createBrain(config)
    print("Brain created successfully")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    # Run some simulation steps
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    # Get some statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    print("Example 1 completed successfully")

def example_enhanced_basic():
    """Example using the enhanced NLM bindings"""
    print("\n" + "="*60)
    print("Example 2: Enhanced NLM Basic Usage")
    print("="*60)
    
    # Create brain with default configuration using factory builder
    brain = nlm.BrainSimulator.createDefault(neurons=500)
    print(f"Created brain with {brain.getBrain().getTotalNeuronCount()} neurons")
    
    # Configure for plasticity
    brain.set_stdp_parameters(ltp_weight=0.02, ltd_weight=0.015)
    brain.enable_all_plasticity(True)
    print("Configured for plasticity")
    
    # Run simulation with progress callback
    def progress_callback(step, progress):
        if step % 100 == 0:
            print(f"  Progress: {progress*100:.1f}%")
    
    print("Running simulation...")
    start_time = time.time()
    brain.run_simulation(1000, progress_callback)
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Get comprehensive metrics
    metrics = brain.get_metrics()
    print("\nSimulation Metrics:")
    print(f"  Average firing rate: {metrics.averageFiringRate:.3f}")
    print(f"  Total spikes: {metrics.totalSpikes}")
    print(f"  Active neurons: {metrics.activeNeurons}")
    print(f"  Firing neurons: {metrics.firingNeurons}")
    print(f"  E/I ratio: {metrics.excitationInhibitionRatio:.3f}")
    print(f"  Current time: {metrics.currentTime:.3f}s")
    
    print("Example 2 completed successfully")

def example_enhanced_advanced():
    """Advanced example using enhanced NLM features"""
    print("\n" + "="*60)
    print("Example 3: Advanced Enhanced NLM Usage")
    print("="*60)
    
    # Create configuration with custom parameters
    config = nlm.ConfigBuilder.createDefaultConfig()
    config.set_timestep(0.01)
    config.set_neuron_count(1000)
    config.set_region_count(2)
    
    # Create brain with custom configuration
    brain = nlm.BrainSimulator(config)
    print(f"Created brain with custom configuration")
    
    # Configure plasticity and development
    brain.set_stdp_parameters(0.03, 0.025)
    brain.set_plasticity_parameters(0.0005, 0.00005)
    brain.enable_all_plasticity(True)
    brain.develop_to_stage(nlm.DevelopmentalStage::CriticalPeriod)
    print("Configured for critical period development")
    
    # Create sensory input patterns
    vision_pattern = [math.sin(i * 0.1) * 0.5 + 0.5 for i in range(100)]
    touch_pattern = [0.8, 0.2, 0.9, 0.1, 0.7, 0.3]
    internal_pattern = [0.5, -0.2, 0.3, -0.1]
    audio_pattern = [0.1, 0.3, 0.5, 0.7, 0.9, 0.8, 0.6, 0.4]
    
    # Run multiple learning cycles
    for cycle in range(5):
        print(f"\nLearning Cycle {cycle + 1}:")
        
        # Apply sensory input
        brain.inject_sensory_pattern(vision_pattern)
        brain.apply_current_to_population(nlm.NeuronType.Sensory, 20.0)
        
        # Run simulation steps
        brain.run_simulation(200)
        
        # Get cycle metrics
        metrics = brain.get_metrics()
        print(f"  Average firing rate: {metrics.averageFiringRate:.3f}")
        print(f"  Total spikes: {metrics.totalSpikes}")
        print(f"  Active neurons: {metrics.activeNeurons}")
        
        time.sleep(0.1)
    
    # Save brain state
    brain.save_state("brain_checkpoint.nlt")
    print(f"\nSaved brain state to 'brain_checkpoint.nlt'")
    
    # Load and test recovery
    recovery_brain = nlm.BrainSimulator.createDefault()
    recovery_brain.load_state("brain_checkpoint.nlt")
    print("Loaded brain state successfully")
    
    # Test metrics recovery
    recovery_metrics = recovery_brain.get_metrics()
    print(f"Recovered brain average firing rate: {recovery_metrics.averageFiringRate:.3f}")
    
    print("\nExample 3 completed successfully")

def example_explore_api():
    """Example exploring the enhanced API"""
    print("\n" + "="*60)
    print("Example 4: Exploring Enhanced API Features")
    print("="*60)
    
    # Create brain
    brain = nlm.BrainSimulator.createDefault(neurons=200)
    brain.enable_curiosity(True)
    brain.enable_development(True)
    
    # Get available metric names
    metric_names = nlm.BrainSimulator.getMetricNames()
    print("Available metrics:")
    for name in metric_names:
        print(f"  - {name}")
    
    # Create action with parameters
    action = nlm.Action()
    action.set_type(nlm.ActionType::MoveForward)
    action.set_parameter("speed", 1.0)
    action.set_parameter("duration", 5.0)
    action.set_parameter("angle", 0.0)
    
    print(f"\nCreated action: {action.get_name()}")
    print(f"Parameters: speed={action.get_parameter('speed', 0.0)}, duration={action.get_parameter('duration', 0.0)}")
    
    # Test world interaction
    world = nlm.createSimpleWorld()
    world.configure(width=100, height=100, vision_width=16, vision_height=16)
    world.setAgentStart(50, 50)
    
    print(f"\nCreated world: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent starts at: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Update world state
    world.update(0.1)
    print(f"After update - Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    vision_data = percept.get_vision()
    print(f"Vision dimensions: {vision_data.get_width()}x{vision_data.get_height()}")
    
    print("Example 4 completed successfully")

def example_experiments():
    """Example running experiments with enhanced NLM"""
    print("\n" + "="*60)
    print("Example 5: Running Experiments")
    print("="*60)
    
    # Run integration experiment
    print("Running integration experiment...")
    integration_result = nlm.ExperimentRunner.run_integration_experiment(
        neuron_count=500,
        max_steps=2000
    )
    
    print(f"Integration experiment results:")
    print(f"  Total reward: {integration_result.totalReward:.2f}")
    print(f"  Average firing rate: {integration_result.avgFiringRate:.3f}")
    print(f"  Episodes stored: {integration_result.memoryEpisodesStored}")
    print(f"  Dopamine level: {integration_result.dopamineLevel:.3f}")
    print(f"  Wall clock time: {integration_result.totalWallClockTime:.2f}s")
    
    # Run plasticity experiment
    print("\nRunning plasticity experiment...")
    plasticity_result = nlm.ExperimentRunner.run_plasticity_experiment(
        neuron_count=200,
        steps=500
    )
    
    print(f"Plasticity experiment results:")
    print(f"  Total reward: {plasticity_result.totalReward:.2f}")
    print(f"  Average firing rate: {plasticity_result.avgFiringRate:.3f}")
    print(f"  Total spikes: {plasticity_result.totalSpikes}")
    print(f"  Execution time: {plasticity_result.totalWallClockTime:.2f}s")
    
    # Create phase 6 configuration
    phase6_config = nlm.Phase6Config()
    phase6_config.neuron_count = 300
    phase6_config.max_steps = 1500
    phase6_config.enableCheckpointing = True
    phase6_config.enableReplay = True
    phase6_config.enableDevelopment = True
    
    print(f"\nPhase 6 configuration:")
    print(f"  Neurons: {phase6_config.neuron_count}")
    print(f"  Max steps: {phase6_config.max_steps}")
    print(f"  Checkpointing: {phase6_config.enableCheckpointing}")
    print(f"  Replay: {phase6_config.enableReplay}")
    print(f"  Development: {phase6_config.enableDevelopment}")
    
    print("Example 5 completed successfully")

def benchmark_original_vs_enhanced():
    """Benchmark original vs enhanced bindings"""
    print("\n" + "="*60)
    print("Example 6: Benchmarking Original vs Enhanced")
    print("="*60)
    
    # Create identical configurations for fair comparison
    config1 = nlm_original.createDefaultConfig()
    config2 = nlm.ConfigBuilder.createDefaultConfig()
    
    # Benchmark original
    print("Benchmarking original bindings...")
    brain1 = nlm_original.createBrain(config1)
    brain1.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain1.step(step)
    original_time = time.time() - start_time
    
    print(f"Original bindings: {original_time:.3f} seconds for 100 steps")
    
    # Benchmark enhanced
    print("Benchmarking enhanced bindings...")
    brain2 = nlm.BrainSimulator(config2)
    brain2.initialize()
    
    start_time = time.time()
    for step in range(100):
        brain2.step(step)
    enhanced_time = time.time() - start_time
    
    print(f"Enhanced bindings: {enhanced_time:.3f} seconds for 100 steps")
    
    if enhanced_time > 0:
        speedup = original_time / enhanced_time
        print(f"Speed ratio (original/enhanced): {speedup:.2f}x")
        
        if speedup > 1.0:
            print("Enhanced bindings are FASTER than original!")
        else:
            print("Original bindings are FASTER than enhanced")
    else:
        print("Cannot calculate speed ratio - enhanced time is zero")
    
    print("Example 6 completed successfully")

def main():
    """Main function demonstrating all examples"""
    print("NLM Python Examples")
    print("===================")
    print("This script demonstrates usage of both original and enhanced NLM bindings")
    print()
    
    try:
        example_basic_original()
        example_enhanced_basic()
        example_enhanced_advanced()
        example_explore_api()
        example_experiments()
        benchmark_original_vs_enhanced()
        
        print("\n" + "="*60)
        print("ALL EXAMPLES COMPLETED SUCCESSFULLY!")
        print("="*60)
        print("\nThe enhanced NLM bindings provide:")
        print("✓ Modern C++ features and type safety")
        print("✓ Factory builders and convenience methods")
        print("✓ Progress callbacks and monitoring")
        print("✓ Comprehensive metrics and analytics")
        print("✓ Enhanced error handling and validation")
        print("✓ Factory configuration patterns")
        print("✓ Advanced experiment support")
        
    except Exception as e:
        print(f"\nERROR: Example failed with exception: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
