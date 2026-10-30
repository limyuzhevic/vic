#!/usr/bin/env python3
"""
NLM Plugin System for Extensibility

This file implements the NLM plugin system, enabling users to extend NLM functionality
through custom Python plugins. The plugin system supports:
- Custom brain components
- Modified learning algorithms
- Additional sensory/motor systems
- Custom analysis tools
- Performance monitoring
- Visualization extensions
"""

import pynlm
import numpy as np
from typing import Dict, Any, List, Optional, Callable
import json
import time
import threading
from abc import ABC, abstractmethod

# ================================================
# PLUGIN FRAMEWORK BASE CLASSES
# ================================================

class NLMPluginBase(ABC):
    """
    Abstract base class for NLM plugins.
    
    Plugins extend NLM functionality by implementing specific hooks
    into the simulation pipeline.
    """
    
    def __init__(self, name: str, version: str = "1.0.0"):
        self.name = name
        self.version = version
        self.initialized = False
        self.owner_brain = None
        self.owner_world = None
        self.config = {}
        
        print(f"Plugin '{name}' v{version} loaded")
    
    @abstractmethod
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """
        Called when the brain is created.
        
        Args:
            brain: The NLM Brain instance
        """
        pass
    
    @abstractmethod
    def on_world_created(self, world: 'pynlm.SimpleWorld'):
        """
        Called when the world is created.
        
        Args:
            world: The NLM SimpleWorld instance
        """
        pass
    
    def on_brain_initialized(self, brain: 'pynlm.Brain'):
        """
        Called when the brain is initialized.
        
        Args:
            brain: The NLM Brain instance
        """
        pass
    
    def on_step_begin(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Called at the beginning of each simulation step.
        
        Args:
            step: Current simulation step
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        pass
    
    def on_sensory_processing(self, percept: 'pynlm.SensoryPercept', 
                             brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Called when sensory input is being processed.
        
        Args:
            percept: The sensory percept
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        pass
    
    def on_motor_output(self, action: 'pynlm.Action',
                       brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Called when motor output is generated.
        
        Args:
            action: The motor action
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        pass
    
    def on_step_end(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Called at the end of each simulation step.
        
        Args:
            step: Current simulation step
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        pass
    
    def on_simulation_end(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Called when simulation ends.
        
        Args:
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        pass
    
    def configure(self, config: Dict[str, Any]):
        """
        Configure the plugin with settings.
        
        Args:
            config: Configuration dictionary
        """
        self.config.update(config)
    
    def get_info(self) -> Dict[str, Any]:
        """
        Get plugin information.
        
        Returns:
            Dictionary containing plugin information
        """
        return {
            'name': self.name,
            'version': self.version,
            'initialized': self.initialized,
            'config': self.config.copy()
        }
class BrainExtensionPlugin(NLMPluginBase):
    """
    Plugin that extends brain functionality.
    
    This plugin allows modification of brain behavior,
    addition of custom neural mechanisms, or enhancement
    of existing functionality.
    """
    
    def __init__(self, name: str, custom_neurons: bool = False, 
                 custom_plasticity: bool = False):
        super().__init__(name, "1.0.0")
        self.custom_neurons = custom_neurons
        self.custom_plasticity = custom_plasticity
        self.custom_neurons_enabled = False
        self.custom_plasticity_enabled = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference and enable custom features if requested."""
        self.owner_brain = brain
        
        if self.custom_neurons:
            self._enable_custom_neurons()
        
        if self.custom_plasticity:
            self._enable_custom_plasticity()
    
    def _enable_custom_neurons(self):
        """Enable custom neuron dynamics."""
        # Add custom neuron properties to brain configuration
        config = self.owner_brain.getConfig()
        
        # Store original parameters
        self.original_v_thresh = config.getOr<double>("brain.v_thresh", -50.0)
        self.original_v_rest = config.getOr<double>("brain.v_rest", -70.0)
        
        # Apply custom parameters
        config.set("brain.v_thresh", -45.0)  # Lower threshold for faster firing
        config.set("brain.v_rest", -65.0)    # Higher resting potential
        
        self.custom_neurons_enabled = True
        print(f"  Custom neuron dynamics enabled for {self.name}")
    
    def _enable_custom_plasticity(self):
        """Enable custom plasticity rules."""
        config = self.owner_brain.getConfig()
        
        # Store original parameters
        self.original_stdp_rate = config.getOr<double>("plasticity.stdp.learning_rate", 0.001)
        self.original_hebbian_rate = config.getOr<double>("plasticity.hebbian.learning_rate", 0.001)
        
        # Apply enhanced plasticity parameters
        config.set("plasticity.stdp.learning_rate", 0.002)  # Double STDP rate
        config.set("plasticity.hebbian.learning_rate", 0.005)  # Stronger Hebbian
        
        self.custom_plasticity_enabled = True
        print(f"  Custom plasticity rules enabled for {self.name}")
class SensoryEnhancementPlugin(NLMPluginBase):
    """
    Plugin that enhances sensory processing.
    
    This plugin adds additional sensory modalities,
    enhances processing quality, or adds custom
    feature extraction.
    """
    
    def __init__(self, name: str, enhanced_vision: bool = False,
                 custom_sensory_features: bool = False):
        super().__init__(name, "1.0.0")
        self.enhanced_vision = enhanced_vision
        self.custom_sensory_features = custom_sensory_features
        self.vision_enhancement_active = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference."""
        self.owner_brain = brain
    
    def on_sensory_processing(self, percept: 'pynlm.SensoryPercept',
                             brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Enhance sensory processing if enabled."""
        if self.enhanced_vision:
            self._enhance_vision(percept)
        
        if self.custom_sensory_features:
            self._add_custom_features(percept, brain, world)
    
    def _enhance_vision(self, percept: 'pynlm.SensoryPercept'):
        """Apply vision enhancement algorithms."""
        try:
            # Get vision data
            vision_data = percept.getVision()
            if vision_data:
                # Apply contrast enhancement
                enhanced_data = self._contrast_enhancement(vision_data)
                
                # Apply edge detection
                edge_data = self._edge_detection(vision_data)
                
                # Store enhanced data (note: this is conceptual as the API may not support this)
                self.vision_enhancement_active = True
                print(f"  Vision enhancement applied for {self.name}")
        except Exception as e:
            print(f"  Vision enhancement error in {self.name}: {e}")
    
    def _contrast_enhancement(self, data: List[float]) -> List[float]:
        """Apply contrast enhancement to vision data."""
        # Simple contrast stretching
        if not data:
            return data
        
        min_val = min(data)
        max_val = max(data)
        
        if max_val - min_val < 0.01:  # Avoid division by zero
            return data
        
        enhanced = []
        for val in data:
            normalized = (val - min_val) / (max_val - min_val)
            enhanced_val = 2.0 * normalized - 1.0  # Stretch to [-1, 1]
            enhanced.append(enhanced_val)
        
        return enhanced
    
    def _edge_detection(self, data: List[float]) -> List[float]:
        """Apply simple edge detection."""
        if len(data) < 3:
            return data
        
        # Simple Sobel-like operator (conceptual)
        edges = []
        for i in range(len(data)):
            if i == 0 or i == len(data) - 1:
                edges.append(0.0)  # Border pixels
            else:
                # Calculate gradient
                prev = data[i-1]
                curr = data[i]
                next_val = data[i+1]
                gradient = abs(next_val - prev) / 2.0
                edges.append(gradient)
        
        return edges
    
    def _add_custom_features(self, percept: 'pynlm.SensoryPercept',
                            brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Add custom sensory features."""
        # This is a conceptual implementation showing how to add
        # custom features to sensory processing
        
        # Example: Add motion detection feature
        motion_feature = self._calculate_motion_feature(percept, world)
        
        # Example: Add object detection feature
        object_feature = self._detect_objects(percept)
        
        print(f"  Custom sensory features added for {self.name}")
    
    def _calculate_motion_feature(self, percept: 'pynlm.SensoryPercept',
                                  world: 'pynlm.SimpleWorld') -> float:
        """Calculate motion-based feature."""
        # Conceptual motion detection
        # In a real implementation, this would compare current and previous frames
        return 0.5  # Placeholder value
    
    def _detect_objects(self, percept: 'pynlm.SensoryPercept') -> List[float]:
        """Detect objects in sensory input."""
        # Conceptual object detection
        # In a real implementation, this would use computer vision techniques
        return [0.1, 0.2, 0.3]  # Placeholder values
class MotorControlPlugin(NLMPluginBase):
    """
    Plugin that modifies motor control behavior.
    
    This plugin allows modification of action selection,
    addition of custom motor commands, or enhancement
    of existing motor functionality.
    """
    
    def __init__(self, name: str, adaptive_control: bool = False,
                 custom_actions: bool = False):
        super().__init__(name, "1.0.0")
        self.adaptive_control = adaptive_control
        self.custom_actions = custom_actions
        self.adaptation_active = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference."""
        self.owner_brain = brain
    
    def on_motor_output(self, action: 'pynlm.Action',
                       brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Modify motor output if enabled."""
        if self.adaptive_control:
            self._apply_adaptive_control(action, brain, world)
        
        if self.custom_actions:
            self._add_custom_actions(action, brain, world)
    
    def _apply_adaptive_control(self, action: 'pynlm.Action',
                                brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Apply adaptive motor control."""
        original_type = action.getType()
        
        # Modify action based on context
        if self._should_increase_power(action, brain, world):
            # In real implementation, modify action parameters
            print(f"  Adaptive control increased power for {self.name}")
        
        # Store adaptation state
        self.adaptation_active = True
    
    def _should_increase_power(self, action: 'pynlm.Action',
                               brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld') -> bool:
        """Determine if action power should be increased."""
        # Conceptual logic: increase power when firing rate is high
        firing_rate = brain.getAverageFiringRate()
        return firing_rate > 10.0  # Threshold
    
    def _add_custom_actions(self, action: 'pynlm.Action',
                           brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Add custom motor actions."""
        # This is a conceptual implementation showing how to add
        # custom motor actions
        
        # Example: Add "freeze" action
        # Example: Add "explore" action
        # Example: Add "avoid" action
        
        print(f"  Custom actions available for {self.name}")
class MonitoringPlugin(NLMPluginBase):
    """
    Plugin for performance monitoring and analysis.
    
    This plugin collects performance metrics, monitors resource
    usage, and provides analysis tools for simulation optimization.
    """
    
    def __init__(self, name: str, performance_monitoring: bool = False,
                 resource_monitoring: bool = False):
        super().__init__(name, "1.0.0")
        self.performance_monitoring = performance_monitoring
        self.resource_monitoring = resource_monitoring
        self.metrics_collected = []
        self.monitoring_active = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference."""
        self.owner_brain = brain
    
    def on_step_begin(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Collect performance metrics at step beginning."""
        if self.performance_monitoring:
            self._collect_performance_metrics(step, brain, world)
        
        if self.resource_monitoring:
            self._collect_resource_metrics(step, brain, world)
    
    def _collect_performance_metrics(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Collect performance metrics."""
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'simulation_time': world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0.0,
            'memory_usage': self._estimate_memory_usage(),
            'cpu_usage': self._estimate_cpu_usage()
        }
        
        self.metrics_collected.append(metrics)
    
    def _collect_resource_metrics(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Collect resource usage metrics."""
        # Conceptual resource monitoring
        pass
    
    def _estimate_memory_usage(self) -> float:
        """Estimate memory usage."""
        # Conceptual memory estimation
        return 100.0  # MB placeholder
    
    def _estimate_cpu_usage(self) -> float:
        """Estimate CPU usage."""
        # Conceptual CPU estimation
        return 25.0  # Percentage placeholder
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """Get summary of collected metrics."""
        if not self.metrics_collected:
            return {}
        
        # Calculate statistics
        firing_rates = [m['firing_rate'] for m in self.metrics_collected]
        spike_counts = [m['total_spikes'] for m in self.metrics_collected]
        
        return {
            'total_steps_monitored': len(self.metrics_collected),
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'max_firing_rate': max(firing_rates),
            'min_firing_rate': min(firing_rates),
            'total_spikes_collected': sum(spike_counts),
            'monitoring_active': self.monitoring_active
        }
    
    def export_metrics(self, filepath: str):
        """Export collected metrics to file."""
        import json
        
        with open(filepath, 'w') as f:
            json.dump(self.metrics_collected, f, indent=2)
        
        print(f"Metrics exported to {filepath}")
class VisualizationPlugin(NLMPluginBase):
    """
    Plugin for simulation visualization.
    
    This plugin provides visualization capabilities for
    monitoring simulation progress, analyzing results,
    and creating visual representations of neural activity.
    """
    
    def __init__(self, name: str, real_time_viz: bool = False,
                 result_visualization: bool = False):
        super().__init__(name, "1.0.0")
        self.real_time_viz = real_time_viz
        self.result_visualization = result_visualization
        self.visualization_active = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference."""
        self.owner_brain = brain
    
    def on_step_begin(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Update visualization if real-time enabled."""
        if self.real_time_viz:
            self._update_real_time_viz(step, brain, world)
    
    def on_simulation_end(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Create result visualizations if enabled."""
        if self.result_visualization:
            self._create_result_visualizations(brain, world)
    
    def _update_real_time_viz(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Update real-time visualization."""
        # Conceptual real-time visualization
        # In a real implementation, this would create plots,
        # update displays, or send data to visualization tools
        
        firing_rate = brain.getAverageFiringRate()
        spike_count = brain.getTotalSpikeCount()
        
        print(f"  Real-time viz: Step {step}, Firing rate: {firing_rate:.2f}, "
              f"Total spikes: {spike_count}")
        
        self.visualization_active = True
    
    def _create_result_visualizations(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Create result visualizations."""
        # Conceptual result visualization
        # In a real implementation, this would create comprehensive
        # visualizations of simulation results
        
        print(f"  Creating result visualizations for {self.name}")
        print(f"    Final firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"    Final spike count: {brain.getTotalSpikeCount()}")
        print(f"    Simulation duration: {world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 'N/A'}s")
        
        self.visualization_active = True
class PluginManager:
    """
    Manager for NLM plugins.
    
    This class manages plugin lifecycle, including loading,
    configuration, initialization, and cleanup.
    """
    
    def __init__(self):
        self.plugins: Dict[str, NLMPluginBase] = {}
        self.enabled_plugins: List[str] = []
        self.plugin_configs: Dict[str, Dict[str, Any]] = {}
    
    def register_plugin(self, plugin_class: type, name: str = None,
                       default_config: Dict[str, Any] = None):
        """
        Register a plugin class.
        
        Args:
            plugin_class: Plugin class to register
            name: Plugin name (defaults to class name)
            default_config: Default configuration for the plugin
        """
        if name is None:
            name = plugin_class.__name__
        
        # Store plugin class
        self.plugins[name] = {
            'class': plugin_class,
            'default_config': default_config or {},
            'instance': None
        }
        
        print(f"Registered plugin: {name}")
        return True
    
    def configure_plugin(self, name: str, config: Dict[str, Any]):
        """
        Configure a plugin.
        
        Args:
            name: Plugin name
            config: Configuration dictionary
        """
        if name not in self.plugins:
            print(f"Plugin {name} not found")
            return False
        
        self.plugin_configs[name] = config.copy()
        print(f"Configured plugin: {name}")
        return True
    
    def enable_plugin(self, name: str) -> bool:
        """
        Enable a plugin.
        
        Args:
            name: Plugin name
            
        Returns:
            True if plugin enabled successfully
        """
        if name not in self.plugins:
            print(f"Plugin {name} not found")
            return False
        
        if name in self.enabled_plugins:
            print(f"Plugin {name} already enabled")
            return True
        
        # Create plugin instance
        plugin_info = self.plugins[name]
        plugin_instance = plugin_info['class'](name)
        
        # Apply configuration
        if name in self.plugin_configs:
            plugin_instance.configure(self.plugin_configs[name])
        
        # Store instance
        plugin_info['instance'] = plugin_instance
        self.enabled_plugins.append(name)
        
        print(f"Enabled plugin: {name}")
        return True
    
    def disable_plugin(self, name: str) -> bool:
        """
        Disable a plugin.
        
        Args:
            name: Plugin name
            
        Returns:
            True if plugin disabled successfully
        """
        if name not in self.enabled_plugins:
            print(f"Plugin {name} not enabled")
            return False
        
        # Cleanup plugin
        plugin_info = self.plugins[name]
        plugin_instance = plugin_info['instance']
        if plugin_instance:
            plugin_instance.cleanup()
        
        # Remove from enabled list
        self.enabled_plugins.remove(name)
        
        # Clear instance
        plugin_info['instance'] = None
        
        print(f"Disabled plugin: {name}")
        return True
    
    def initialize_plugins(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Initialize all enabled plugins.
        
        Args:
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        for name in self.enabled_plugins:
            plugin_info = self.plugins[name]
            plugin_instance = plugin_info['instance']
            
            if plugin_instance:
                try:
                    plugin_instance.on_brain_created(brain)
                    plugin_instance.on_brain_initialized(brain)
                    plugin_instance.on_world_created(world)
                    print(f"Initialized plugin: {name}")
                except Exception as e:
                    print(f"Failed to initialize plugin {name}: {e}")
    
    def run_step(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """
        Run a simulation step through all plugins.
        
        Args:
            step: Current simulation step
            brain: The NLM Brain instance
            world: The NLM SimpleWorld instance
        """
        for name in self.enabled_plugins:
            plugin_info = self.plugins[name]
            plugin_instance = plugin_info['instance']
            
            if plugin_instance:
                try:
                    # Call step hooks
                    plugin_instance.on_step_begin(step, brain, world)
                    plugin_instance.on_sensory_processing(None, brain, world)  # Placeholder
                    plugin_instance.on_motor_output(None, brain, world)  # Placeholder
                    plugin_instance.on_step_end(step, brain, world)
                except Exception as e:
                    print(f"Plugin {name} error at step {step}: {e}")
    
    def cleanup_plugins(self):
        """Clean up all plugins."""
        for name in self.enabled_plugins:
            plugin_info = self.plugins[name]
            plugin_instance = plugin_info['instance']
            
            if plugin_instance:
                try:
                    plugin_instance.on_simulation_end(None, None)  # Placeholder
                    print(f"Cleaned up plugin: {name}")
                except Exception as e:
                    print(f"Error cleaning up plugin {name}: {e}")
        
        self.enabled_plugins.clear()
        print("All plugins cleaned up")
    
    def get_plugin_info(self) -> Dict[str, Any]:
        """
        Get information about all plugins.
        
        Returns:
            Dictionary containing plugin information
        """
        info = {}
        
        for name, plugin_info in self.plugins.items():
            if plugin_info['instance']:
                plugin_instance = plugin_info['instance']
                info[name] = plugin_instance.get_info()
            else:
                info[name] = {
                    'name': name,
                    'version': '1.0.0',
                    'initialized': False
                }
        
        return info
# ================================================
# EXAMPLE PLUGINS
# ================================================

class LearningEnhancementPlugin(NLMPluginBase):
    """
    Example plugin that enhances learning capabilities.
    
    This plugin demonstrates how to add custom learning
    mechanisms to NLM simulations.
    """
    
    def __init__(self, name: str, enhancement_factor: float = 1.0):
        super().__init__(name, "1.0.0")
        self.enhancement_factor = enhancement_factor
        self.enhancement_active = False
    
    def on_brain_created(self, brain: 'pynlm.Brain'):
        """Store brain reference and enable enhancement."""
        self.owner_brain = brain
        
        # Modify brain configuration for enhanced learning
        config = brain.getConfig()
        
        # Store original learning rates
        self.original_stdp_rate = config.getOr<double>("plasticity.stdp.learning_rate", 0.001)
        self.original_hebbian_rate = config.getOr<double>("plasticity.hebbian.learning_rate", 0.001)
        
        # Apply enhanced learning rates
        config.set("plasticity.stdp.learning_rate", 
                   self.original_stdp_rate * self.enhancement_factor)
        config.set("plasticity.hebbian.learning_rate", 
                   self.original_hebbian_rate * self.enhancement_factor)
        
        self.enhancement_active = True
        print(f"  Learning enhancement enabled for {self.name} (factor: {self.enhancement_factor})")
class VisualizationExportPlugin(NLMPluginBase):
    """
    Example plugin for exporting visualizations.
    
    This plugin demonstrates how to add visualization
    and export capabilities to NLM simulations.
    """
    
    def __init__(self, name: str, output_format: str = "json", 
                 include_performance_metrics: bool = True):
        super().__init__(name, "1.0.0")
        self.output_format = output_format
        self.include_performance_metrics = include_performance_metrics
        self.export_data = []
    
    def on_step_begin(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Collect data for visualization export."""
        if self.include_performance_metrics:
            data_point = {
                'step': step,
                'timestamp': time.time(),
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getActiveNeuronCount()
            }
            self.export_data.append(data_point)
    
    def on_simulation_end(self, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Export collected data."""
        if self.export_data:
            self._export_data()
            print(f"  Visualization data exported for {self.name}")
    
    def _export_data(self):
        """Export visualization data."""
        if self.output_format == "json":
            # In a real implementation, this would save to file
            pass
        elif self.output_format == "csv":
            # In a real implementation, this would save to file
            pass
class SimpleExamplePlugin(NLMPluginBase):
    """
    Simple example plugin showing basic plugin functionality.
    
    This plugin provides minimal functionality to demonstrate
    how to create and use NLM plugins.
    """
    
    def __init__(self, name: str, custom_message: str = "Hello from plugin!"):
        super().__init__(name, "1.0.0")
        self.custom_message = custom_message
        self.message_count = 0
    
    def on_step_begin(self, step: int, brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld'):
        """Print custom message at each step."""
        self.message_count += 1
        print(f"  {self.name}: {self.custom_message} (step {self.message_count})")
# ================================================
# PLUGIN EXAMPLES AND USAGE
# ================================================

def demonstrate_plugin_system():
    """
    Demonstrate the NLM plugin system with example plugins.
    
    This function shows how to create and use plugins
    to extend NLM functionality.
    """
    print("NLM Plugin System Demonstration")
    print("=" * 40)
    
    # Create plugin manager
    plugin_manager = PluginManager()
    
    # Register plugins
    plugin_manager.register_plugin(SimpleExamplePlugin, "Basic Plugin", {
        'custom_message': "Basic plugin functionality"
    })
    
    plugin_manager.register_plugin(LearningEnhancementPlugin, "Learning Enhancer", {
        'enhancement_factor': 1.5
    })
    
    plugin_manager.register_plugin(MonitoringPlugin, "Performance Monitor", {
        'performance_monitoring': True,
        'resource_monitoring': False
    })
    
    # Configure plugins
    plugin_manager.configure_plugin("Basic Plugin", {
        'custom_message': "Hello from the basic plugin!"
    })
    
    plugin_manager.configure_plugin("Learning Enhancer", {
        'enhancement_factor': 2.0
    })
    
    # Enable plugins
    plugin_manager.enable_plugin("Basic Plugin")
    plugin_manager.enable_plugin("Learning Enhancer")
    plugin_manager.enable_plugin("Performance Monitor")
    
    # Create and configure simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Initialize plugins
    plugin_manager.initialize_plugins(brain, world)
    
    # Run simulation
    print("\nRunning simulation with plugins...")
    for step in range(50):
        # Simulate plugin processing
        world.update(0.1)
        
        if step % 10 == 0:
            print(f"  Step {step:2d} completed")
    
    # Cleanup
    plugin_manager.cleanup_plugins()
    
    print("\nPlugin system demonstration complete!")
    
    # Show plugin information
    info = plugin_manager.get_plugin_info()
    print(f"\nRegistered plugins: {len(info)}")
    for name, plugin_info in info.items():
        print(f"  - {name}: {plugin_info.get('version', '1.0.0')}")

# ================================================
# MAIN FUNCTION
# ================================================

def main():
    """
    Main function demonstrating plugin system usage.
    
    This function provides a complete example of how to
    use the NLM plugin system to extend simulation functionality.
    """
    print("NLM Plugin System Examples")
    print("=" * 30)
    
    # Demonstrate basic plugin system
    demonstrate_plugin_system()
    
    print("\n" + "=" * 30)
    print("Plugin system examples complete!")
    
    print("\nFor more plugin examples, see:")
    print("  - Plugin documentation")
    print("  - Advanced usage guides")
    print("  - Sample plugin implementations")

if __name__ == "__main__":
    main()