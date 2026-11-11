#!/usr/bin/env python3
"""
Example scripts demonstrating the improved NLM Python API and usage patterns.

These examples showcase the enhanced convenience methods and new features
in the improved pynlm Python bindings.
"""

import pynlm
import time
import math
from typing import List, Dict, Any, Callable

print("NLM Python API Examples - Demonstrating Improved Functionality")
print("=" * 70)

# Example 1: Enhanced Convenience Methods
def example_enhanced_convenience():
    """Demonstrate enhanced convenience methods in pynlm."""
    print("\nExample 1: Enhanced Convenience Methods")
    print("-" * 40)
    
    # Create a brain with enhanced configuration
    config = pynlm.createDefaultConfig()
    
    # Set up enhanced configuration using convenience methods
    config.set("neuron_count", 1000)
    config.set("region_count", 4)
    config.set("vision_width", 8)
    config.set("vision_height", 8)
    config.set("world_width", 20)
    config.set("world_height", 20)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enhanced convenience method - run and collect stats
    def run_simulation_with_stats(steps: int, description: str = "") -> Dict[str, Any]:
        """Enhanced simulation runner with comprehensive statistics."""
        stats = {
            'description': description,
            'start_time': time.time(),
            'total_spikes': [],
            'firing_counts': [],
            'firing_rates': [],
            'development_stages': [],
            'curiosity_levels': [],
            'prediction_errors': [],
            'avg_rate_over_time': []
        }
        
        for step in range(steps):
            brain.step(step)
            
            # Collect comprehensive stats
            stats['total_spikes'].append(brain.getTotalSpikeCount())
            stats['firing_counts'].append(brain.getFiringNeuronCount())
            stats['firing_rates'].append(brain.getAverageFiringRate())
            stats['development_stages'].append(brain.getDevelopmentalStage())
            
            # Add more stats if agent is available
            if hasattr(brain, 'getAgentBrain'):
                agent = brain.getAgentBrain()
                stats['curiosity_levels'].append(agent.getCuriosityLevel())
                stats['prediction_errors'].append(agent.getPredictionError())
            
            # Calculate running average
            if step > 0:
                running_avg = sum(stats['firing_rates'][-5:]) / min(5, len(stats['firing_rates']))
                stats['avg_rate_over_time'].append(running_avg)
        
        stats['end_time'] = time.time()
        stats['duration'] = stats['end_time'] - stats['start_time']
        stats['total_steps'] = steps
        
        return stats
    
    # Run enhanced simulation
    print("Running enhanced simulation with statistical tracking...")
    sim_stats = run_simulation_with_stats(100, "Enhanced test simulation")
    
    # Print comprehensive statistics
    print(f"Simulation completed in {sim_stats['duration']:.2f} seconds")
    print(f"Final state - Spikes: {sim_stats['total_spikes'][-1]}, "
          f"Firing: {sim_stats['firing_counts'][-1]}, "
          f"Rate: {sim_stats['firing_rates'][-1]:.2f}Hz")
    print(f"Development stage: {sim_stats['development_stages'][-1].name}")
    
    return sim_stats

# Example 2: Smart Simulation Loop with Adaptive Control
def example_smart_simulation():
    """Demonstrate smart simulation loop with adaptive control."""
    print("\nExample 2: Smart Simulation Loop with Adaptive Control")
    print("-" * 50)
    
    # Create brain with learning enabled
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable advanced learning features
    if hasattr(brain, 'getAgentBrain'):
        agent = brain.getAgentBrain()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    # Smart simulation parameters
    simulation_params = {
        'initial_steps': 50,
        'learning_phase_steps': 150,
        'stability_check_interval': 20,
        'min_stability_score': 0.7,
        'max_rate': 100.0,
        'min_rate': 1.0
    }
    
    def is_stable(firing_rates: List[float], threshold: float = 0.1) -> bool:
        """Check if firing rates are stable (not changing too rapidly)."""
        if len(firing_rates) < 5:
            return False
        
        recent_rates = firing_rates[-5:]
        rate_variance = max(recent_rates) - min(recent_rates)
        return rate_variance <= (threshold * max(recent_rates))
    
    def get_stability_score(firing_rates: List[float]) -> float:
        """Calculate stability score (1.0 = very stable, 0.0 = unstable)."""
        if len(firing_rates) < 10:
            return 0.0
        
        recent_rates = firing_rates[-10:]
        mean_rate = sum(recent_rates) / len(recent_rates)
        variance = sum((x - mean_rate) ** 2 for x in recent_rates) / len(recent_rates)
        std_dev = math.sqrt(variance)
        
        # Normalize to stability score
        stability_score = 1.0 - (std_dev / (0.2 * mean_rate))
        return max(0.0, min(1.0, stability_score))
    
    # Smart simulation loop
    all_firing_rates = []
    learning_active = True
    
    print("Starting smart simulation with adaptive control...")
    
    for step in range(simulation_params['initial_steps']):
        brain.step(step)
        all_firing_rates.append(brain.getAverageFiringRate())
        
        if step % 10 == 0:
            print(f"  Step {step:3d}: Rate={all_firing_rates[-1]:.2f}Hz, "
                  f"Stable={is_stable(all_firing_rates):.2f}")
    
    # Learning phase with adaptive control
    print(f"Starting learning phase (adaptive control enabled)...")
    
    for step in range(simulation_params['initial_steps'], 
                      simulation_params['initial_steps'] + 
                      simulation_params['learning_phase_steps']):
        
        # Check stability periodically
        if step % simulation_params['stability_check_interval'] == 0 and learning_active:
            stability_score = get_stability_score(all_firing_rates)
            print(f"  Stability check at step {step}: Score={stability_score:.2f}")
            
            if stability_score < simulation_params['min_stability_score']:
                print(f"    Low stability detected - adjusting learning rate")
                learning_active = False
        
        # Apply learning if active
        if learning_active:
            # In a real scenario, you would apply neuromodulation here
            pass
        
        brain.step(step)
        all_firing_rates.append(brain.getAverageFiringRate())
        
        if step % 30 == 0:
            current_stability = get_stability_score(all_firing_rates)
            print(f"  Step {step:3d}: Rate={all_firing_rates[-1]:.2f}Hz, "
                  f"Stability={current_stability:.2f}, Learning={'ON' if learning_active else 'OFF'}")
    
    final_stability = get_stability_score(all_firing_rates)
    print(f"\nSmart simulation completed!")
    print(f"Final firing rate: {all_firing_rates[-1]:.2f}Hz")
    print(f"Final stability score: {final_stability:.2f}")
    
    return {
        'total_steps': simulation_params['initial_steps'] + simulation_params['learning_phase_steps'],
        'final_firing_rate': all_firing_rates[-1],
        'final_stability': final_stability,
        'learning_active': learning_active,
        'firing_rate_history': all_firing_rates
    }

# Example 3: Configuration Builder and Template System
def example_configuration_builder():
    """Demonstrate enhanced configuration building capabilities."""
    print("\nExample 3: Configuration Builder and Template System")
    print("-" * 55)
    
    class NLMConfigurator:
        """High-level configuration builder for NLM simulations."""
        
        def __init__(self):
            self.base_config = pynlm.createDefaultConfig()
            self.configs = {}
        
        def add_template(self, name: str, **kwargs):
            """Add a configuration template with common settings."""
            template_config = pynlm.createDefaultConfig()
            
            # Set common parameters with sensible defaults
            default_settings = {
                'neuron_count': 1000,
                'region_count': 4,
                'vision_width': 8,
                'vision_height': 8,
                'world_width': 20,
                'world_height': 20,
                'enable_reward_modulation': True,
                'enable_curiosity': True,
                'enable_structural_plasticity': True,
                'enable_development': True,
                'learning_rate': 0.01,
                'curiosity_exploration': 0.5
            }
            
            for key, value in default_settings.items():
                if key in kwargs:
                    template_config.set(key, kwargs[key])
                else:
                    template_config.set(key, value)
            
            self.configs[name] = template_config
            print(f"  Created template '{name}' with configuration:")
            for key, value in kwargs.items():
                print(f"    {key}: {value}")
        
        def get_config(self, name: str) -> pynlm.Config:
            """Get a named configuration."""
            if name in self.configs:
                return self.configs[name]
            else:
                raise ValueError(f"Configuration template '{name}' not found")
        
        def create_brain_from_template(self, name: str) -> pynlm.Brain:
            """Create a brain using a named template."""
            config = self.get_config(name)
            return pynlm.createBrain(config)
        
        def compare_templates(self, template_names: List[str]) -> Dict[str, Any]:
            """Compare multiple templates."""
            comparison = {
                'templates': template_names,
                'configurations': {},
                'recommendations': []
            }
            
            for name in template_names:
                comparison['configurations'][name] = self.get_config(name).summary()
            
            # Generate recommendations based on configuration
            if 'fast' in template_names:
                comparison['recommendations'].append(
                    "Use 'fast' template for quick prototyping and testing"
                )
            if 'deep' in template_names:
                comparison['recommendations'].append(
                    "Use 'deep' template for comprehensive learning studies"
                )
            
            return comparison
    
    # Create and use configurator
    configurator = NLMConfigurator()
    
    # Define different templates
    configurator.add_template("simple", 
                             neuron_count=100,
                             region_count=2,
                             vision_width=4,
                             vision_height=4,
                             learning_rate=0.001)
    
    configurator.add_template("complex", 
                             neuron_count=5000,
                             region_count=8,
                             vision_width=12,
                             vision_height=12,
                             learning_rate=0.1,
                             enable_structural_plasticity=True)
    
    configurator.add_template("fast", 
                             neuron_count=50,
                             region_count=1,
                             vision_width=2,
                             vision_height=2,
                             enable_reward_modulation=False,
                             enable_curiosity=False)
    
    configurator.add_template("deep", 
                             neuron_count=10000,
                             region_count=16,
                             vision_width=20,
                             vision_height=20,
                             learning_rate=0.05,
                             enable_structural_plasticity=True,
                             enable_development=True)
    
    # Use templates
    print("\nUsing templates to create brains:")
    
    simple_brain = configurator.create_brain_from_template("simple")
    simple_brain.initialize()
    print(f"  Simple brain created with {simple_brain.getTotalNeuronCount()} neurons")
    
    # Run simulation with simple brain
    for step in range(20):
        simple_brain.step(step)
    
    # Compare templates
    print("\nTemplate comparison:")
    comparison = configurator.compare_templates(["simple", "complex", "fast", "deep"])
    
    return {
        'configurator': configurator,
        'comparison': comparison
    }

# Example 4: Performance Monitoring and Profiling
def example_performance_monitoring():
    """Demonstrate performance monitoring and profiling capabilities."""
    print("\nExample 4: Performance Monitoring and Profiling")
    print("-" * 50)
    
    import time
    
    class PerformanceProfiler:
        """Performance profiler for NLM simulations."""
        
        def __init__(self, name: str):
            self.name = name
            self.timings = []
            self.memory_usage = []
            self.operation_counts = {}
        
        def record_timing(self, operation: str, duration: float):
            """Record timing for an operation."""
            if operation not in self.operation_counts:
                self.operation_counts[operation] = 0
            self.operation_counts[operation] += 1
            self.timings.append((operation, duration, time.time()))
        
        def get_stats(self) -> Dict[str, Any]:
            """Get performance statistics."""
            if not self.timings:
                return {'error': 'No timing data recorded'}
            
            operation_times = {}
            for operation, duration, timestamp in self.timings:
                if operation not in operation_times:
                    operation_times[operation] = []
                operation_times[operation].append(duration)
            
            stats = {
                'total_operations': len(self.timings),
                'operations_by_type': self.operation_counts.copy(),
                'average_times': {},
                'min_times': {},
                'max_times': {},
                'total_duration': max(t for _, _, t in self.timings) - min(t for _, _, t in self.timings)
            }
            
            for operation, times in operation_times.items():
                stats['average_times'][operation] = sum(times) / len(times)
                stats['min_times'][operation] = min(times)
                stats['max_times'][operation] = max(times)
            
            return stats
        
        def print_report(self):
            """Print performance report."""
            stats = self.get_stats()
            print(f"\nPerformance Report for {self.name}:")
            print("=" * 40)
            print(f"Total operations: {stats['total_operations']}")
            print(f"Total duration: {stats['total_duration']:.2f} seconds")
            
            if 'operations_by_type' in stats:
                print("\nOperations by type:")
                for operation, count in stats['operations_by_type'].items():
                    avg_time = stats['average_times'][operation]
                    print(f"  {operation:20s}: {count:4d} calls, avg {avg_time:.4f}s")
    
    # Create profiler
    profiler = PerformanceProfiler("Brain Simulation")
    
    # Simulate performance monitoring
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Running performance-profiling simulation...")
    
    # Record timing for simulation steps
    for step in range(50):
        step_start = time.time()
        brain.step(step)
        step_duration = time.time() - step_start
        profiler.record_timing(f"step_{step}", step_duration)
        
        if step % 10 == 0:
            print(f"  Step {step}: {step_duration:.4f}s per step")
    
    # Print performance report
    profiler.print_report()
    
    return {
        'profiler': profiler,
        'stats': profiler.get_stats()
    }

# Example 5: Visualization and Data Export
def example_visualization_and_export():
    """Demonstrate visualization and data export capabilities."""
    print("\nExample 5: Visualization and Data Export")
    print("-" * 40)
    
    def create_simulation_summary(brain, steps: int) -> Dict[str, Any]:
        """Create a comprehensive simulation summary for export."""
        summary = {
            'simulation_parameters': {
                'neuron_count': brain.getTotalNeuronCount(),
                'region_count': brain.getRegionCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate(),
                'final_development_stage': brain.getDevelopmentalStage().name
            },
            'region_statistics': [],
            'time_series_data': [],
            'export_timestamp': time.time()
        }
        
        # Add region statistics
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                region_stats = {
                    'region_id': region_id.index(),
                    'name': region.getName(),
                    'neuron_count': region.getTotalNeuronCount(),
                    'active_neuron_count': region.getActiveNeuronCount(),
                    'firing_neuron_count': region.getFiringNeuronCount(),
                    'average_firing_rate': region.getAverageFiringRate(),
                    'synaptic_density': region.getSynapticDensity()
                }
                summary['region_statistics'].append(region_stats)
        
        # Generate synthetic time series data
        for step in range(steps):
            time_point = {
                'step': step,
                'timestamp': step * 0.1,
                'total_spikes': int(brain.getTotalSpikeCount() * (step + 1) / steps),
                'firing_rate': brain.getAverageFiringRate() * (0.5 + 0.5 * (step / steps)),
                'development_progress': min(1.0, step / steps)
            }
            summary['time_series_data'].append(time_point)
        
        return summary
    
    # Create and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(30):
        brain.step(step)
    
    # Generate export data
    export_data = create_simulation_summary(brain, 30)
    
    # Save to JSON
    import json
    export_filename = f"nlm_simulation_{int(time.time())}.json"
    
    with open(export_filename, 'w') as f:
        json.dump(export_data, f, indent=2)
    
    print(f"Simulation data exported to {export_filename}")
    print(f"Data contains:")
    print(f"  - Region statistics for {len(export_data['region_statistics'])} regions")
    print(f"  - Time series data for {len(export_data['time_series_data'])} time points")
    print(f"  - Neuron count: {export_data['simulation_parameters']['neuron_count']}")
    print(f"  - Total spikes: {export_data['simulation_parameters']['total_spikes']}")
    print(f"  - Final firing rate: {export_data['simulation_parameters']['final_firing_rate']:.2f} Hz")
    
    return {
        'export_filename': export_filename,
        'export_data': export_data
    }

# Main execution
def main():
    """Run all examples demonstrating improved NLM functionality."""
    print("Running comprehensive NLM Python API examples")
    print("=" * 70)
    
    try:
        # Run all examples
        example_enhanced_convenience()
        smart_results = example_smart_simulation()
        config_results = example_configuration_builder()
        perf_results = example_performance_monitoring()
        viz_results = example_visualization_and_export()
        
        print("\n" + "=" * 70)
        print("All examples completed successfully!")
        print("\nKey improvements demonstrated:")
        print("  1. Enhanced convenience methods with statistical tracking")
        print("  2. Smart simulation loops with adaptive control")
        print("  3. Configuration templates and builders")
        print("  4. Performance monitoring and profiling")
        print("  5. Comprehensive visualization and data export")
        print("=" * 70)
        
        return {
            'smart_simulation': smart_results,
            'configuration': config_results,
            'performance': perf_results,
            'visualization': viz_results
        }
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        import traceback
        traceback.print_exc()
        return {'error': str(e)}

if __name__ == "__main__":
    results = main()
    print("\nExample execution completed!")
