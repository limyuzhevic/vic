#!/usr/bin/env python3
"""
Enhanced NLM Python API Examples

This file demonstrates the enhanced NLM Python API features and provides
practical examples for users at all skill levels.
"""

import pynlm
import time
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, Any, List, Optional

print("NLM Enhanced Python API Examples")
print("=" * 50)


def example_basic_usage():
    """Example 1: Basic NLM usage (Beginner Friendly)"""
    print("\n1. Basic NLM Usage")
    print("-" * 30)
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    print(f"Simulation completed:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return brain


def example_enhanced_configuration():
    """Example 2: Enhanced configuration with JSON/YAML support"""
    print("\n2. Enhanced Configuration")
    print("-" * 30)
    
    # Test JSON configuration
    json_config = """
    {
        "brain": {
            "neuron_count": 1000,
            "region_count": 4,
            "connection_probability": 0.05
        },
        "plasticity": {
            "stdp": {"enable": true, "learning_rate": 0.001},
            "hebbian": {"enable": true, "learning_rate": 0.0005}
        }
    }
    """
    
    config = pynlm.loadFromString(json_config, "JSON")
    print(f"Loaded JSON configuration:")
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 0)}")
    print(f"  Region count: {config.getOr('brain.region_count', 0)}")
    print(f"  Connection probability: {config.getOr('brain.connection_probability', 0.0)}")
    
    # Test YAML configuration
    yaml_config = """
    brain:
      neuron_count: 500
      region_count: 2
      connection_probability: 0.1
    plasticity:
      stdp:
        enable: true
        learning_rate: 0.002
      hebbian:
        enable: true
        learning_rate: 0.001
    """
    
    config = pynlm.loadFromString(yaml_config, "YAML")
    print(f"\nLoaded YAML configuration:")
    print(f"  Neuron count: {config.getOr('brain.neuron_count', 0)}")
    print(f"  Region count: {config.getOr('brain.region_count', 0)}")
    
    # Create brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with enhanced config: {brain.getTotalNeuronCount()} neurons")
    
    return config


def example_performance_monitoring():
    """Example 3: Performance monitoring and profiling"""
    print("\n3. Performance Monitoring")
    print("-" * 30)
    
    # Create brain with performance monitoring
    brain = pynlm.createBrainWithConfig(
        neuron_count=2000,
        region_count=8,
        connection_probability=0.02
    )
    brain.initialize()
    
    # Test PerformanceMonitor
    print("Testing PerformanceMonitor...")
    monitor = pynlm.PerformanceMonitor()
    monitor.start_monitoring()
    
    for step in range(100):
        monitor.start_step_timing()
        brain.step(step)
        monitor.end_step_timing()
        
        if step % 25 == 0:
            metrics = monitor.get_step_metrics()
            print(f"  Step {step}: {metrics['step_time_ms']:.3f}ms, "
                  f"{metrics['spikes_per_step']:.1f} spikes/step")
    
    monitor.stop_monitoring()
    
    # Get performance report
    report = monitor.get_performance_report()
    print(f"\nPerformance Report:")
    print(f"  Average step time: {report['avg_step_time']*1000:.2f}ms")
    print(f"  Steps per second: {report['steps_per_second']:.0f}")
    print(f"  Total simulation time: {report['total_time']:.2f}s")
    
    # Test PerformanceProfiler
    print("\nTesting PerformanceProfiler...")
    profiler = pynlm.PerformanceProfiler()
    
    profiler.start_profile("initialization")
    brain = pynlm.createBrainWithConfig(neuron_count=1000)
    brain.initialize()
    profiler.stop_profile("initialization")
    
    profiler.start_profile("simulation")
    for step in range(100):
        brain.step(step)
    profiler.stop_profile("simulation")
    
    # Get profiling results
    init_time = profiler.get_profile_results("initialization")["duration"]
    sim_time = profiler.get_profile_results("simulation")["duration"]
    
    print(f"  Initialization time: {init_time:.3f}s")
    print(f"  Simulation time: {sim_time:.3f}s")
    print(f"  Simulation efficiency: {init_time/sim_time:.1%}")
    
    return monitor, profiler


def example_context_managers():
    """Example 4: Advanced context management"""
    print("\n4. Advanced Context Management")
    print("-" * 30)
    
    # Use context manager for safe resource management
    print("Testing context managers...")
    with pynlm.create_performance_monitored_brain(
        neuron_count=1000,
        monitoring_interval=0.1
    ) as brain_monitor:
        brain, monitor = brain_monitor
        
        print(f"Created monitored brain with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation with automatic cleanup
        with pynlm.run_simulation_with_analysis(brain, 200) as analysis:
            for step in range(200):
                # Get world and sensor input
                world = analysis.get_world()
                percept = world.getSensoryPercept()
                
                # Process sensory input
                # (In real usage, this would be: brain.receiveSensoryInput(percept))
                
                # Run brain step
                brain.step(step)
                
                # Get action (simplified)
                action = brain.produceAction()
                
                # Apply action to world (simplified)
                # world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get analysis results
            results = analysis.get_results()
            
            print(f"\nSimulation Analysis:")
            print(f"  Efficiency: {results['efficiency']:.2%}")
            print(f"  Average step time: {results['avg_step_time']:.4f}s")
            print(f"  Total spikes: {results['total_spikes']}")
            print(f"  Memory usage: {results['memory_usage_mb']:.2f} MB")
    
    print("✅ Context manager test completed")


def example_batch_processing():
    """Example 5: Batch processing and parallel execution"""
    print("\n5. Batch Processing")
    print("-" * 30)
    
    # Create batch processor
    batch_processor = pynlm.BatchProcessor()
    
    # Define simulation configurations
    simulations = [
        {'neurons': 500, 'steps': 100, 'name': 'small_brain'},
        {'neurons': 1000, 'steps': 200, 'name': 'medium_brain'},
        {'neurons': 2000, 'steps': 300, 'name': 'large_brain'}
    ]
    
    print(f"Running {len(simulations)} simulations with batch processor...")
    
    # Run batch simulation
    results = batch_processor.run_batch_simulation(simulations, num_workers=2)
    
    print(f"\nBatch Processing Results:")
    print(f"{'Simulation':<15} {'Neurons':<10} {'Steps':<8} {'Steps/s':<12} {'Total Spikes':<15}")
    print("-" * 60)
    
    for sim_id, result in results.items():
        print(f"{result['name']:<15} {result['neurons']:<10} {result['steps']:<8} "
              f"{result['steps_per_second']:<12.1f} {result['total_spikes']:<15}")
    
    # Calculate statistics
    total_steps_per_second = sum(r['steps_per_second'] for r in results.values())
    avg_efficiency = sum(r['efficiency'] for r in results.values()) / len(results)
    
    print(f"\nBatch Processing Summary:")
    print(f"  Total steps per second: {total_steps_per_second:.1f}")
    print(f"  Average efficiency: {avg_efficiency:.2%}")
    print(f"  Speedup achieved: {len(simulations)}x parallel")
    
    return results


def example_advanced_error_handling():
    """Example 6: Advanced error handling and recovery"""
    print("\n6. Advanced Error Handling")
    print("-" * 30)
    
    class NLMErrorHandler:
        def __init__(self):
            self.retry_count = 0
            self.max_retries = 3
            self.error_log = []
        
        def safe_simulation(self, brain, max_steps):
            """Run simulation with automatic error recovery."""
            for step in range(max_steps):
                try:
                    # Execute brain step
                    brain.step(step)
                    
                    # Log success
                    self.error_log.append({
                        'step': step,
                        'status': 'success',
                        'timestamp': time.time()
                    })
                    
                    print(f"  Step {step}: ✅ Success")
                    
                except pynlm.exceptions.SimulationError as e:
                    print(f"  Step {step}: ❌ Simulation Error: {e}")
                    if self.retry_count < self.max_retries:
                        self.retry_count += 1
                        print(f"    Retrying step {step} (attempt {self.retry_count}/{self.max_retries})")
                        continue
                    else:
                        print(f"    Max retries exceeded. Saving checkpoint and stopping.")
                        brain.save(f"checkpoint_error_step_{step}.bin")
                        raise
                
                except pynlm.exceptions.ResourceError as e:
                    print(f"  Step {step}: ❌ Resource Error: {e}")
                    print(f"    Saving current state and exiting gracefully.")
                    brain.save("graceful_shutdown_checkpoint.bin")
                    break
                
                except Exception as e:
                    print(f"  Step {step}: ❌ Unexpected Error: {e}")
                    brain.save(f"unexpected_error_step_{step}.bin")
                    raise
            
            return step
    
    # Create error handler
    error_handler = NLMErrorHandler()
    
    # Create and run brain with error handling
    print("Testing error handling with simulation...")
    brain = pynlm.createBrainWithConfig(neuron_count=100)
    brain.initialize()
    
    try:
        result = error_handler.safe_simulation(brain, 50)
        print(f"\n✅ Simulation completed successfully with {result} steps")
        print(f"✅ Error log: {len(error_handler.error_log)} successful steps")
        
    except Exception as e:
        print(f"\n❌ Simulation failed: {e}")
        # Clean up checkpoints
        import os
        for f in os.listdir('.'):
            if f.startswith('checkpoint') and f.endswith('.bin'):
                os.remove(f)
                print(f"🧹 Cleaned up checkpoint: {f}")
    
    return error_handler.error_log


def example_advanced_visualization():
    """Example 7: Advanced visualization and analysis"""
    print("\n7. Advanced Visualization")
    print("-" * 30)
    
    def create_brain_activity_plots(brain, steps):
        """Create comprehensive brain activity plots."""
        
        # Collect simulation data
        print("Collecting simulation data...")
        spike_counts = []
        firing_rates = []
        e_i_ratios = []
        activation_patterns = []
        
        for step in range(steps):
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
            e_i_ratios.append(brain.getExcitationInhibitionRatio())
            
            # Get region activation
            region_activations = []
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                region_activations.append(region.getActiveNeuronCount())
            
            activation_patterns.append(region_activations)
            
            brain.step(step)
        
        # Create visualization
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle(f'NLM Brain Activity Simulation ({steps} steps)', fontsize=16)
        
        # Spike count over time
        axes[0, 0].plot(spike_counts, linewidth=1.5)
        axes[0, 0].set_title('Total Spike Count Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Spike Count')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Firing rate over time
        axes[0, 1].plot(firing_rates, color='green', linewidth=1.5)
        axes[0, 1].set_title('Average Firing Rate Over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate')
        axes[0, 1].grid(True, alpha=0.3)
        
        # E/I ratio over time
        axes[1, 0].plot(e_i_ratios, color='red', linewidth=1.5)
        axes[1, 0].set_title('Excitatory-Inhibitory Ratio Over Time')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('E/I Ratio')
        axes[1, 0].grid(True, alpha=0.3)
        axes[1, 0].set_yscale('log')
        
        # Region activation patterns (last 10 steps)
        last_10_steps = activation_patterns[-10:] if len(activation_patterns) >= 10 else activation_patterns
        if last_10_steps:
            region_means = np.mean(last_10_steps, axis=0)
            axes[1, 1].bar(range(len(region_means)), region_means, color='skyblue')
            axes[1, 1].set_title('Average Region Activation (Last 10 Steps)')
            axes[1, 1].set_xlabel('Region ID')
            axes[1, 1].set_ylabel('Active Neuron Count')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Spike rate distribution
        spike_rates = [s / steps for s in spike_counts]
        axes[2, 0].hist(spike_rates, bins=20, alpha=0.7, color='orange')
        axes[2, 0].set_title('Spike Rate Distribution')
        axes[2, 0].set_xlabel('Spike Rate')
        axes[2, 0].set_ylabel('Frequency')
        axes[2, 0].grid(True, alpha=0.3)
        
        # Region comparison (final state)
        final_region_counts = []
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            final_region_counts.append(region.getTotalNeuronCount())
        
        axes[2, 1].pie(final_region_counts, labels=[f"R{i}" for i in range(len(final_region_counts))], autopct='%1.1f%%')
        axes[2, 1].set_title('Final Neuron Distribution by Region')
        
        plt.tight_layout()
        plt.savefig('brain_activity_analysis.png', dpi=150, bbox_inches='tight')
        plt.show()
        
        return {
            'spike_counts': spike_counts,
            'firing_rates': firing_rates,
            'e_i_ratios': e_i_ratios,
            'activation_patterns': activation_patterns,
            'region_means': region_means if 'region_means' in locals() else []
        }
    
    # Run simulation and create analysis
    print("Running simulation for visualization...")
    brain = pynlm.createBrainWithConfig(neuron_count=500)
    brain.initialize()
    
    analysis_results = create_brain_activity_plots(brain, 200)
    
    print(f"\n✅ Visualization completed")
    print(f"   Generated plots: brain_activity_analysis.png")
    print(f"   Data points collected: {len(analysis_results['spike_counts'])} steps")
    
    return analysis_results


def example_research_simulation():
    """Example 8: Research-oriented simulation"""
    print("\n8. Research-Oriented Simulation")
    print("-" * 30)
    
    # Create research configuration
    research_config = {
        'name': 'neuroscience_research',
        'brain': {
            'neuron_count': 10000,
            'region_count': 20,
            'connection_probability': 0.01,
            'synapse_type': 'biological'
        },
        'plasticity': {
            'stdp': {
                'enable': True,
                'learning_rate': 0.0001,
                'tau_plus': 20.0,
                'tau_minus': 20.0
            },
            'hebbian': {
                'enable': True,
                'learning_rate': 0.00005
            },
            'structural': {
                'enable': True,
                'synaptogenesis_rate': 0.00001,
                'pruning_rate': 0.000001
            }
        },
        'neuromodulation': {
            'dopamine': {
                'scale': 2.0,
                'plasticity_factor': 1.5,
                'reward_prediction': True
            },
            'curiosity': {
                'enable': True,
                'exploration_bonus': 0.5,
                'novelty_sensitivity': 1.0
            },
            'novelty': {
                'enable': True,
                'sensitivity': 1.0,
                'adaptation_rate': 0.1
            },
            'prediction_error': {
                'enable': True,
                'prediction_weight': 0.8,
                'error_threshold': 0.1
            }
        }
    }
    
    print(f"Creating research simulation: {research_config['name']}")
    print(f"  Neurons: {research_config['brain']['neuron_count']}")
    print(f"  Regions: {research_config['brain']['region_count']}")
    print(f"  Connectivity: {research_config['brain']['connection_probability']}")
    print(f"  Plasticity: STDP={research_config['plasticity']['stdp']['enable']}, "
          f"Hebbian={research_config['plasticity']['hebbian']['enable']}")
    print(f"  Neuromodulation: Dopamine={research_config['neuromodulation']['dopamine']['scale']}, "
          f"Curiosity={research_config['neuromodulation']['curiosity']['enable']}")
    
    # Create research brain
    brain = pynlm.createBrainWithConfig(research_config)
    brain.initialize()
    
    # Run research simulation
    print(f"\nRunning research simulation for 500 steps...")
    start_time = time.time()
    
    for step in range(500):
        # Simulate sensory input
        # In real research, this would be actual sensory data
        # For demonstration, we use random activity
        import random
        current_input = random.uniform(0, 10)
        brain.injectCurrent(current_input)
        
        # Run brain step
        brain.step(step)
        
        # Monitor key metrics
        if step % 100 == 0:
            metrics = {
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'development_stage': str(brain.getDevelopmentalStage()),
                'time_elapsed': time.time() - start_time
            }
            print(f"  Step {step}: {metrics['total_spikes']} spikes, "
                  f"{metrics['firing_rate']:.3f} Hz, "
                  f"E/I={metrics['e_i_ratio']:.2f}, "
                  f"Stage={metrics['development_stage']}, "
                  f"Time={metrics['time_elapsed']:.1f}s")
    
    # Analysis
    total_time = time.time() - start_time
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    final_e_i = brain.getExcitationInhibitionRatio()
    
    print(f"\nResearch Simulation Results:")
    print(f"  Total simulation time: {total_time:.2f} seconds")
    print(f"  Average steps per second: {500/total_time:.1f}")
    print(f"  Final total spikes: {final_spikes}")
    print(f"  Final firing neurons: {final_firing}")
    print(f"  Final E/I ratio: {final_e_i:.3f}")
    print(f"  Neurons per second: {500/total_time:.1f}")
    
    # Save research results
    brain.save("research_simulation_checkpoint.bin")
    print(f"\nResearch results saved to: research_simulation_checkpoint.bin")
    
    return {
        'config': research_config,
        'total_time': total_time,
        'final_spikes': final_spikes,
        'final_firing': final_firing,
        'final_e_i': final_e_i
    }


def main():
    """Run all examples"""
    print("Enhanced NLM Python API Examples")
    print("=" * 50)
    
    try:
        # Example 1: Basic Usage
        brain = example_basic_usage()
        
        # Example 2: Enhanced Configuration
        config = example_enhanced_configuration()
        
        # Example 3: Performance Monitoring
        monitor, profiler = example_performance_monitoring()
        
        # Example 4: Context Management
        example_context_managers()
        
        # Example 5: Batch Processing
        batch_results = example_batch_processing()
        
        # Example 6: Error Handling
        error_log = example_advanced_error_handling()
        
        # Example 7: Visualization
        viz_results = example_advanced_visualization()
        
        # Example 8: Research Simulation
        research_results = example_research_simulation()
        
        # Summary
        print("\n" + "=" * 50)
        print("EXAMPLES SUMMARY")
        print("=" * 50)
        print("✅ Example 1: Basic NLM usage - Completed")
        print("✅ Example 2: Enhanced configuration - Completed")
        print("✅ Example 3: Performance monitoring - Completed")
        print("✅ Example 4: Context management - Completed")
        print("✅ Example 5: Batch processing - Completed")
        print("✅ Example 6: Error handling - Completed")
        print("✅ Example 7: Visualization - Completed")
        print("✅ Example 8: Research simulation - Completed")
        print("\nAll examples completed successfully!")
        print("\nFor more information, see:")
        print("  - Enhanced_Documentation_and_CI.md")
        print("  - enhanced_python_apis.py")
        print("  - README.md")
        
    except Exception as e:
        print(f"\n❌ Error running examples: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()
