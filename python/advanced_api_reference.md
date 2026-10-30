#!/usr/bin/env python3
"""
NLM Advanced Usage Documentation

Comprehensive documentation for advanced NLM Python usage patterns,
including plugin system, performance optimization, and experimental workflows.
"""

import pynlm
from typing import Dict, Any, List, Optional, Callable
import json
import time
from pathlib import Path

# ================================================
# ADVANCED API REFERENCE
# ================================================

class NLMAPIReference:
    """
    Reference for advanced NLM Python API usage.
    
    This class provides comprehensive documentation and examples for
    advanced NLM Python features beyond basic usage.
    """
    
    @staticmethod
    def create_advanced_brain(config_dict: Dict[str, Any]) -> 'pynlm.Brain':
        """
        Create a brain with advanced configuration.
        
        Args:
            config_dict: Dictionary of configuration parameters
            
        Returns:
            Configured Brain instance
        """
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Apply advanced configuration
        for key, value in config_dict.items():
            brain.getConfig().set(key, value)
        
        brain.initialize()
        return brain
    
    @staticmethod
    def create_complex_world(width: int, height: int, 
                           vision_width: int, vision_height: int,
                           environment_complexity: str = 'moderate') -> 'pynlm.SimpleWorld':
        """
        Create a complex world with advanced environmental features.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width
            vision_height: Agent vision height
            environment_complexity: 'simple', 'moderate', or 'complex'
            
        Returns:
            Configured SimpleWorld instance
        """
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        
        # Add environmental complexity based on settings
        if environment_complexity == 'moderate':
            _add_moderate_environment(world, width, height)
        elif environment_complexity == 'complex':
            _add_complex_environment(world, width, height)
        
        world.reset()
        return world

def _add_moderate_environment(world: 'pynlm.SimpleWorld', width: int, height: int):
    """Add moderate environmental complexity."""
    # Add some resources
    for i in range(5):
        world.addObject(pynlm.WorldObject(
            x=float(i * 8 + 4),
            y=float(i * 6 + 8),
            type=pynlm.WorldObjectType::Resource,
            value=float(5 + i * 3)
        ))
    
    # Add some hazards
    for i in range(3):
        world.addObject(pynlm.WorldObject(
            x=float((i + 1) * 10 - 5),
            y=float((i + 1) * 8 + 3),
            type=pynlm.WorldObjectType::Hazard,
            radius=0.7,
            value=-15.0
        ))

def _add_complex_environment(world: 'pynlm.SimpleWorld', width: int, height: int):
    """Add complex environmental features."""
    # Complex resource distribution
    for y in range(8, height - 8, 6):
        for x in range(8, width - 8, 6):
            if (x + y) % 12 == 0:  # Patterned distribution
                world.addObject(pynlm.WorldObject(
                    x=float(x),
                    y=float(y),
                    type=pynlm.WorldObjectType::Resource,
                    value=float(10.0)
                ))
    
    # Complex hazard patterns
    for i in range(5):
        for j in range(5):
            if (i + j) % 2 == 0:
                world.addObject(pynlm.WorldObject(
                    x=float(15 + i * 5),
                    y=float(15 + j * 5),
                    type=pynlm.WorldObjectType::Hazard,
                    radius=1.0,
                    value=-25.0
                ))

# ================================================
# PERFORMANCE OPTIMIZATION GUIDE
# ================================================

class PerformanceOptimization:
    """
    Performance optimization strategies for NLM Python bindings.
    
    This class provides guidance on optimizing NLM performance for
    different use cases and hardware configurations.
    """
    
    @staticmethod
    def optimize_for_realtime(config: 'pynlm.Config') -> 'pynlm.Config':
        """
        Configure brain for real-time performance.
        
        Args:
            config: Configuration to optimize
            
        Returns:
            Optimized configuration
        """
        config.set("brain.neuron_count", 200)  # Smaller for speed
        config.set("brain.synapse_density", 0.05)  # Fewer connections
        config.set("plasticity.stdp.enable", False)  # Disable STDP for speed
        config.set("plasticity.hebbian.enable", False)  # Disable Hebbian for speed
        config.set("plasticity.structural.enable", False)  # Disable structural for speed
        
        return config
    
    @staticmethod
    def optimize_for_accuracy(config: 'pynlm.Config') -> 'pynlm.Config':
        """
        Configure brain for maximum accuracy.
        
        Args:
            config: Configuration to optimize
            
        Returns:
            Optimized configuration
        """
        config.set("brain.neuron_count", 2000)  # Larger for capacity
        config.set("brain.synapse_density", 0.2)  # More connections
        config.set("plasticity.stdp.enable", True)  # Enable all plasticity
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        return config
    
    @staticmethod
    def benchmark_configuration(config: 'pynlm.Config', steps: int = 100) -> Dict[str, float]:
        """
        Benchmark configuration performance.
        
        Args:
            config: Configuration to benchmark
            steps: Number of simulation steps
            
        Returns:
            Dictionary of performance metrics
        """
        import timeit
        
        def run_simulation():
            brain = pynlm.createBrain(config)
            brain.initialize()
            for step in range(steps):
                brain.step(step)
            return brain
        
        time_taken = timeit.timeit(run_simulation, number=1)
        
        return {
            'total_time_seconds': time_taken,
            'steps_per_second': steps / time_taken,
            'average_step_time_ms': (time_taken * 1000) / steps
        }

# ================================================
# EXPERIMENTAL WORKFLOW GUIDE
# ================================================

class ExperimentalWorkflow:
    """
    Guide for designing and running NLM experiments.
    
    This class provides a structured approach to designing scientific
    experiments with NLM, including experimental design,
    data collection, and analysis.
    """
    
    def __init__(self, name: str):
        self.name = name
        self.config = {}
        self.results = []
        self.metadata = {
            'created': time.time(),
            'nlm_version': '2.0.0',
            'python_version': f"{__import__('sys').version_info.major}."
                          f"{__import__('sys').version_info.minor}"
        }
    
    def set_brain_configuration(self, **kwargs):
        """Set brain configuration parameters."""
        self.config['brain'] = kwargs
        return self
    
    def set_environment_configuration(self, **kwargs):
        """Set environment configuration parameters."""
        self.config['environment'] = kwargs
        return self
    
    def set_learning_configuration(self, **kwargs):
        """Set learning configuration parameters."""
        self.config['learning'] = kwargs
        return self
    
    def run_experiment(self, duration: int) -> Dict[str, Any]:
        """
        Run the experiment for specified duration.
        
        Args:
            duration: Number of simulation steps
            
        Returns:
            Experiment results
        """
        print(f"Running experiment '{self.name}' for {duration} steps...")
        
        # Create components
        config = pynlm.createDefaultConfig()
        
        # Apply brain configuration
        if 'brain' in self.config:
            for key, value in self.config['brain'].items():
                config.set(f"brain.{key}", value)
        
        # Apply learning configuration
        if 'learning' in self.config:
            for key, value in self.config['learning'].items():
                config.set(f"plasticity.{key}", value)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        step_metrics = []
        for step in range(duration):
            brain.step(step)
            
            # Collect metrics
            metrics = {
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'spike_count': brain.getTotalSpikeCount() - step_metrics[-1]['total_spikes'] if step_metrics else 0
            }
            step_metrics.append(metrics)
            
            if step % 100 == 0:
                print(f"  Step {step:4d}: {brain.getFiringNeuronCount():6d} firing, "
                      f"{brain.getAverageFiringRate():6.2f} avg rate")
        
        # Compile results
        results = {
            'experiment_name': self.name,
            'config': self.config,
            'metadata': self.metadata,
            'duration': duration,
            'final_metrics': step_metrics[-1],
            'step_metrics': step_metrics,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'spike_efficiency': brain.getTotalSpikeCount() / duration
        }
        
        self.results.append(results)
        return results
    
    def save_results(self, filepath: str):
        """Save experiment results to file."""
        with open(filepath, 'w') as f:
            json.dump(self.results[-1], f, indent=2)
        print(f"Results saved to {filepath}")
    
    def compare_with_previous(self, metric: str = 'spike_efficiency') -> Dict[str, Any]:
        """
        Compare current experiment results with previous ones.
        
        Args:
            metric: Metric to compare ('spike_efficiency', 'total_spikes', etc.)
            
        Returns:
            Comparison results
        """
        if len(self.results) < 2:
            return {'error': 'Need at least 2 experiments for comparison'}
        
        current = self.results[-1]
        previous = self.results[-2]
        
        current_value = current['final_metrics'][metric]
        previous_value = previous['final_metrics'][metric]
        
        difference = current_value - previous_value
        percentage_change = (difference / previous_value) * 100 if previous_value != 0 else 0
        
        return {
            'metric': metric,
            'current_value': current_value,
            'previous_value': previous_value,
            'difference': difference,
            'percentage_change': percentage_change,
            'improvement': difference > 0
        }

# ================================================
# PLUGIN SYSTEM GUIDE
# ================================================

class NLMPlugin:
    """
    Base class for NLM Python plugins.
    
    This class provides the foundation for extending NLM functionality
    through Python plugins. Plugins can add custom components,
    modify existing behavior, or provide new analysis tools.
    """
    
    def __init__(self, name: str):
        self.name = name
        self.version = '1.0.0'
        self.author = 'NLM Plugin Author'
        self.description = 'NLM Plugin'
    
    def initialize(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Initialize plugin with brain and world objects."""
        pass
    
    def step(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Called each simulation step."""
        pass
    
    def cleanup(self):
        """Cleanup plugin resources."""
        pass

class PluginManager:
    """
    Manager for NLM Python plugins.
    
    This class manages the loading, initialization, and lifecycle
    of NLM plugins.
    """
    
    def __init__(self):
        self.plugins: Dict[str, NLMPlugin] = {}
        self.enabled_plugins: List[str] = []
    
    def load_plugin(self, plugin_class: type, name: str = None) -> bool:
        """
        Load a plugin class.
        
        Args:
            plugin_class: Plugin class to load
            name: Plugin name (defaults to class name)
            
        Returns:
            True if plugin loaded successfully
        """
        if name is None:
            name = plugin_class.__name__
        
        try:
            plugin_instance = plugin_class(name)
            self.plugins[name] = plugin_instance
            print(f"Loaded plugin: {name}")
            return True
        except Exception as e:
            print(f"Failed to load plugin {name}: {e}")
            return False
    
    def enable_plugin(self, name: str) -> bool:
        """Enable a plugin."""
        if name not in self.plugins:
            print(f"Plugin {name} not found")
            return False
        
        self.enabled_plugins.append(name)
        print(f"Enabled plugin: {name}")
        return True
    
    def disable_plugin(self, name: str) -> bool:
        """Disable a plugin."""
        if name in self.enabled_plugins:
            self.enabled_plugins.remove(name)
            print(f"Disabled plugin: {name}")
            return True
        print(f"Plugin {name} not enabled")
        return False
    
    def initialize_plugins(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Initialize all enabled plugins."""
        for name in self.enabled_plugins:
            plugin = self.plugins[name]
            try:
                plugin.initialize(brain, world)
                print(f"Initialized plugin: {name}")
            except Exception as e:
                print(f"Failed to initialize plugin {name}: {e}")
    
    def run_step(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Run a simulation step for all enabled plugins."""
        for name in self.enabled_plugins:
            plugin = self.plugins[name]
            try:
                plugin.step(step, brain, world)
            except Exception as e:
                print(f"Plugin {name} error at step {step}: {e}")
    
    def cleanup_plugins(self):
        """Cleanup all enabled plugins."""
        for name in self.enabled_plugins:
            plugin = self.plugins[name]
            try:
                plugin.cleanup()
                print(f"Cleaned up plugin: {name}")
            except Exception as e:
                print(f"Error cleaning up plugin {name}: {e}")

# ================================================
# UTILITY FUNCTIONS
# ================================================

def validate_brain_config(config: 'pynlm.Config') -> Dict[str, Any]:
    """
    Validate brain configuration.
    
    Args:
        config: Configuration to validate
        
    Returns:
        Validation results
    """
    issues = []
    warnings = []
    
    # Check neuron count
    neuron_count = config.getOr<int64_t>("brain.neuron_count", 0)
    if neuron_count == 0:
        issues.append("brain.neuron_count not set")
    elif neuron_count > 10000:
        warnings.append(f"brain.neuron_count ({neuron_count}) very high - may impact performance")
    
    # Check synapse density
    synapse_density = config.getOr<double>("brain.synapse_density", 0.0)
    if synapse_density < 0.0 or synapse_density > 1.0:
        issues.append(f"brain.synapse_density ({synapse_density}) out of valid range [0.0, 1.0]")
    
    # Check plasticity settings
    if config.getOr<bool>("plasticity.stdp.enable", True):
        if config.getOr<bool>("plasticity.stdp.learning_rate", 0.0) <= 0.0:
            warnings.append("STDP enabled but learning rate is zero")
    
    return {
        'valid': len(issues) == 0,
        'issues': issues,
        'warnings': warnings
    }

def export_results(results: Dict[str, Any], filepath: str):
    """
    Export experiment results to file.
    
    Args:
        results: Results to export
        filepath: Output file path
    """
    extension = Path(filepath).suffix.lower()
    
    if extension == '.json':
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
    elif extension == '.csv':
        with open(filepath, 'w') as f:
            if 'step_metrics' in results:
                headers = results['step_metrics'][0].keys()
                f.write(','.join(headers) + '\n')
                for step_data in results['step_metrics']:
                    f.write(','.join(str(step_data[h]) for h in headers) + '\n')
    
    print(f"Results exported to {filepath}")

def import_results(filepath: str) -> Dict[str, Any]:
    """
    Import experiment results from file.
    
    Args:
        filepath: Input file path
        
    Returns:
        Imported results
    """
    extension = Path(filepath).suffix.lower()
    
    with open(filepath, 'r') as f:
        if extension == '.json':
            results = json.load(f)
        else:
            raise ValueError(f"Unsupported file format: {extension}")
    
    print(f"Results imported from {filepath}")
    return results

# ================================================
# MAIN DOCUMENTATION EXAMPLE
# ================================================

def demonstrate_advanced_features():
    """
    Demonstrate advanced NLM Python features with comprehensive examples.
    
    This function shows how to use all the advanced features covered in this documentation.
    """
    print("NLM Advanced Features Demonstration")
    print("=" * 40)
    
    # 1. Create experimental workflow
    experiment = (ExperimentalWorkflow("Advanced Learning Experiment")
                 .set_brain_configuration(
                     neuron_count=500,
                     synapse_density=0.1
                 )
                 .set_learning_configuration(
                     stdp=True,
                     hebbian=True,
                     structural=True,
                     learning_rate=0.01
                 )
                 .set_environment_configuration(
                     complexity='complex'
                 ))
    
    # 2. Run experiment
    results = experiment.run_experiment(500)
    
    # 3. Compare with previous scenario
    comparison = experiment.compare_with_previous('spike_efficiency')
    print(f"\nExperiment comparison: {comparison['metric']} changed by {comparison['percentage_change']:.2f}%")
    
    # 4. Performance optimization
    opt_config = pynlm.createDefaultConfig()
    optimized_config = PerformanceOptimization.optimize_for_realtime(opt_config)
    
    # Benchmark optimized config
    benchmarks = PerformanceOptimization.benchmark_configuration(optimized_config, 50)
    print(f"\nOptimized configuration benchmarks:")
    print(f"  Steps per second: {benchmarks['steps_per_second']:.2f}")
    print(f"  Average step time: {benchmarks['average_step_time_ms']:.2f}ms")
    
    # 5. Validation
    validation = validate_brain_config(optimized_config)
    print(f"\nConfiguration validation: {'PASS' if validation['valid'] else 'FAIL'}")
    if validation['warnings']:
        print("Warnings:")
        for warning in validation['warnings']:
            print(f"  - {warning}")
    
    return {
        'experiment': results,
        'comparison': comparison,
        'benchmarks': benchmarks,
        'validation': validation
    }

if __name__ == "__main__":
    # Run demonstration
    demo_results = demonstrate_advanced_features()
    
    # Save demonstration results
    export_results(demo_results, "advanced_features_demo.json")
    
    print("\nAdvanced features demonstration complete!")
    print("For more examples, see:")
    print("  - advanced_examples.py")
    print("  - Plugin system documentation")
    print("  - Performance optimization guides")