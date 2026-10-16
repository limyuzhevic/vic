#!/usr/bin/env python3
"""
Enhanced Python utilities for NLM (Neural Learning Machine)

This module provides higher-level utilities and convenience functions for working with NLM,
including simulation management, data analysis, visualization helpers, and configuration tools.
"""

import json
import time
from typing import Dict, List, Optional, Tuple, Any, Callable
from pathlib import Path
import numpy as np

# Try to import the NLM bindings
try:
    import pynlm as nlm
    HAS_NLM = True
except ImportError:
    HAS_NLM = False
    nlm = None

class SimulationManager:
    """High-level simulation management utilities."""
    
    def __init__(self, config=None, world=None):
        """
        Initialize simulation manager.
        
        Args:
            config: NLM Config object
            world: SimpleWorld object
        """
        if not HAS_NLM:
            raise ImportError("NLM bindings not available")
        
        self.config = config or nlm.createDefaultConfig()
        self.world = world or nlm.createSimpleWorld()
        self.agent = None
        self.brain = None
        self.world_objects = []
        
        # Initialize if both config and world provided
        if config and world:
            self.initialize()
    
    def initialize(self, width=100, height=100, visionWidth=21, visionHeight=21):
        """
        Initialize simulation with configuration.
        
        Args:
            width: World width
            height: World height
            visionWidth: Agent vision width
            visionHeight: Agent vision height
        """
        self.brain = nlm.createBrain(self.config)
        self.agent = nlm.createAgentBrain(self.brain)
        self.world.configure(width, height, visionWidth, visionHeight)
        self.world.reset()
    
    def run_episode(self, max_steps=1000, random_seed=None):
        """
        Run a single simulation episode.
        
        Args:
            max_steps: Maximum number of steps
            random_seed: Random seed for reproducibility
            
        Returns:
            Dictionary with episode statistics
        """
        if not self.agent or not self.world:
            raise RuntimeError("Simulation not initialized. Call initialize() first.")
        
        if random_seed is not None:
            self.world.setRandomSeed(random_seed)
        
        self.agent.reset()
        self.world.reset()
        
        # Get initial percept
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        episode_stats = {
            'steps': 0,
            'total_reward': 0.0,
            'max_reward': float('-inf'),
            'min_reward': float('inf'),
            'total_spikes': 0,
            'active_neurons': 0,
            'firing_rate': 0.0,
            'objects_collected': 0,
            'hazards_avoided': 0,
            'success': False,
            'final_action': None
        }
        
        for step in range(max_steps):
            # Get current action
            action = self.agent.decodeMotorCommand()
            episode_stats['final_action'] = {
                'type': action.getType(),
                'parameters': action.getParameters()
            }
            
            # Apply action in world
            world_result = self.world.applyMotorCommand(action, float(step))
            episode_stats['total_reward'] += world_result.reward
            episode_stats['max_reward'] = max(episode_stats['max_reward'], world_result.reward)
            episode_stats['min_reward'] = min(episode_stats['min_reward'], world_result.reward)
            
            # Update statistics based on world result
            if world_result.success:
                if world_result.message and "resource" in world_result.message.lower():
                    episode_stats['objects_collected'] += 1
                elif world_result.message and "hazard" in world_result.message.lower():
                    episode_stats['hazards_avoided'] += 1
            
            # Get new percept and update agent
            next_percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(next_percept)
            
            # Update episode stats with brain state
            if self.brain:
                episode_stats['total_spikes'] = self.brain.getTotalSpikeCount()
                episode_stats['active_neurons'] = self.brain.getActiveNeuronCount()
                episode_stats['firing_rate'] = self.brain.getAverageFiringRate()
            
            episode_stats['steps'] = step + 1
            
            # Check for termination condition
            if self.agent.getCuriosityLevel() > 0.95 and self.agent.getNeuromodulationLevel() > 0.8:
                # High curiosity and neuromodulation suggest exploration is complete
                break
        
        # Final statistics
        episode_stats['success'] = episode_stats['total_reward'] > 0.0
        
        return episode_stats
    
    def add_resource(self, x, y, value=1.0, radius=0.5):
        """Add a resource object to the world."""
        if not HAS_NLM:
            return
        
        obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, value, radius)
        self.world.addObject(obj)
        self.world_objects.append(('resource', obj))
    
    def add_hazard(self, x, y, value=-1.0, radius=0.5):
        """Add a hazard object to the world."""
        if not HAS_NLM:
            return
        
        obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, value, radius)
        self.world.addObject(obj)
        self.world_objects.append(('hazard', obj))
    
    def add_wall(self, x, y, radius=0.5):
        """Add a wall to the world."""
        if not HAS_NLM:
            return
        
        obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Wall, 0.0, radius)
        self.world.addObject(obj)
        self.world_objects.append(('wall', obj))
    
    def clear_objects(self):
        """Clear all world objects."""
        if not HAS_NLM:
            return
        
        for obj in self.world_objects:
            self.world.removeObject(obj[1].x, obj[1].y)
        self.world_objects.clear()
    
    def get_sensors(self):
        """Get current sensor readings."""
        if not self.agent:
            raise RuntimeError("Agent not initialized")
        
        return self.world.getSensoryPercept()
    
    def get_position(self):
        """Get agent position."""
        if not self.world:
            raise RuntimeError("World not initialized")
        
        body = self.world.getAgentBody()
        return {'x': body.x, 'y': body.y, 'orientation': body.orientation}
    
    def get_status(self):
        """Get current simulation status."""
        status = {
            'initialized': self.agent is not None and self.world is not None,
            'brain_region_count': self.brain.getRegionCount() if self.brain else 0,
            'total_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
            'total_synapses': self.brain.getTotalSynapseCount() if self.brain else 0,
            'active_neurons': self.brain.getActiveNeuronCount() if self.brain else 0,
            'firing_rate': self.brain.getAverageFiringRate() if self.brain else 0.0,
            'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0.0,
            'novelty_level': self.agent.getNoveltyLevel() if self.agent else 0.0,
            'neuromodulation_level': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
            'world_size': {'width': self.world.getWidth(), 'height': self.world.getHeight()} if self.world else {},
            'object_count': len(self.world_objects)
        }
        
        return status
class DataAnalyzer:
    """Utilities for analyzing simulation data."""
    
    @staticmethod
    def extract_time_series(episode_stats_list):
        """
        Extract time series data from multiple episodes.
        
        Args:
            episode_stats_list: List of episode statistics dictionaries
            
        Returns:
            Dictionary with time series data
        """
        time_series = {
            'step': [],
            'reward': [],
            'curiosity': [],
            'novelty': [],
            'neuromodulation': [],
            'firing_rate': [],
            'active_neurons': [],
            'brain_age': []
        }
        
        for stats in episode_stats_list:
            # Since we don't have step-by-step data, we'll create synthetic time series
            # based on episode statistics
            steps = stats['steps']
            if steps > 0:
                time_series['step'].extend(range(steps))
                # Generate synthetic data for visualization
                rewards = np.linspace(stats['min_reward'], stats['max_reward'], steps)
                time_series['reward'].extend(rewards)
                time_series['curiosity'].extend([stats['curiosity_level']] * steps)
                time_series['novelty'].extend([stats['novelty_level']] * steps)
                time_series['neuromodulation'].extend([stats['neuromodulation_level']] * steps)
                time_series['firing_rate'].extend([stats['firing_rate']] * steps)
                time_series['active_neurons'].extend([stats['active_neurons']] * steps)
                time_series['brain_age'].extend([stats['brain_age']] * steps)
        
        return time_series
    
    @staticmethod
    def calculate_metrics(episode_stats_list):
        """
        Calculate statistical metrics from multiple episodes.
        
        Args:
            episode_stats_list: List of episode statistics dictionaries
            
        Returns:
            Dictionary with calculated metrics
        """
        if not episode_stats_list:
            return {}
        
        rewards = [stats['total_reward'] for stats in episode_stats_list]
        steps = [stats['steps'] for stats in episode_stats_list]
        
        metrics = {
            'episode_count': len(episode_stats_list),
            'mean_reward': np.mean(rewards),
            'std_reward': np.std(rewards),
            'max_reward': np.max(rewards),
            'min_reward': np.min(rewards),
            'mean_steps': np.mean(steps),
            'success_rate': sum(1 for stats in episode_stats_list if stats['success']) / len(episode_stats_list),
            'mean_objects_collected': np.mean([stats['objects_collected'] for stats in episode_stats_list]),
            'mean_hazards_avoided': np.mean([stats['hazards_avoided'] for stats in episode_stats_list])
        }
        
        return metrics
    
    @staticmethod
    def export_data(stats, filepath):
        """
        Export episode statistics to JSON file.
        
        Args:
            stats: Episode statistics dictionary
            filepath: Output file path
        """
        with open(filepath, 'w') as f:
            json.dump(stats, f, indent=2)
class VisualizationHelper:
    """Helper functions for visualization."""
    
    @staticmethod
    def create_2d_world_map(world_width, world_height, objects):
        """
        Create a 2D representation of the world.
        
        Args:
            world_width: World width
            world_height: World height
            objects: List of world objects
            
        Returns:
            2D array representing the world
        """
        # Create empty world map
        world_map = np.zeros((world_height, world_width))
        
        # Add objects to map (simplified representation)
        for obj in objects:
            x, y = int(obj.x), int(obj.y)
            radius = int(obj.radius * 10)  # Scale for display
            
            if 0 <= x < world_width and 0 <= y < world_height:
                # Draw object
                for i in range(max(0, x - radius), min(world_width, x + radius + 1)):
                    for j in range(max(0, y - radius), min(world_height, y + radius + 1)):
                        dist = np.sqrt((i - x) ** 2 + (j - y) ** 2)
                        if dist <= radius:
                            if obj.type == nlm.WorldObjectType.Resource:
                                world_map[j, i] = 0.8  # Green-ish
                            elif obj.type == nlm.WorldObjectType.Hazard:
                                world_map[j, i] = 0.2  # Red-ish
                            elif obj.type == nlm.WorldObjectType.Wall:
                                world_map[j, i] = 0.5  # Gray
        
        return world_map
    
    @staticmethod
    def plot_brain_metrics(neural_stats, output_file=None):
        """
        Create visualization of brain metrics.
        
        Args:
            neural_stats: Dictionary with neural statistics
            output_file: Optional output file path
        """
        try:
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Brain Metrics', fontsize=16)
            
            # Plot 1: Neural population
            if 'region_data' in neural_stats:
                ax = axes[0, 0]
                regions = neural_stats['region_data'].keys()
                neuron_counts = [neural_stats['region_data'][r]['neuron_count'] for r in regions]
                ax.bar(regions, neuron_counts)
                ax.set_title('Neurons per Region')
                ax.set_ylabel('Neuron Count')
            
            # Plot 2: Synaptic connections
            if 'synaptic_data' in neural_stats:
                ax = axes[0, 1]
                types = neural_stats['synaptic_data'].keys()
                counts = [neural_stats['synaptic_data'][t]['count'] for t in types]
                ax.bar(types, counts, color=['green', 'red', 'blue', 'orange'])
                ax.set_title('Synaptic Connections by Type')
                ax.set_ylabel('Connection Count')
            
            # Plot 3: Activity over time
            if 'activity_data' in neural_stats:
                ax = axes[1, 0]
                steps = neural_stats['activity_data']['step']
                firing_rates = neural_stats['activity_data']['firing_rate']
                ax.plot(steps, firing_rates)
                ax.set_title('Firing Rate Over Time')
                ax.set_xlabel('Step')
                ax.set_ylabel('Firing Rate (Hz)')
            
            # Plot 4: Neuromodulation levels
            if 'neuromodulation_data' in neural_stats:
                ax = axes[1, 1]
                neuromodulators = ['Dopamine', 'Curiosity', 'Novelty', 'Prediction Error']
                levels = [
                    neural_stats['neuromodulation_data'].get('dopamine', 0),
                    neural_stats['neuromodulation_data'].get('curiosity', 0),
                    neural_stats['neuromodulation_data'].get('novelty', 0),
                    neural_stats['neuromodulation_data'].get('prediction_error', 0)
                ]
                ax.bar(neuromodulators, levels, color=['blue', 'green', 'red', 'orange'])
                ax.set_title('Neuromodulation Levels')
                ax.set_ylabel('Level')
                ax.set_ylim(0, 1)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                plt.close()
                return output_file
            else:
                plt.show()
                
        except ImportError:
            print("Matplotlib not available. Skipping visualization.")
            return None
    
    @staticmethod
    def create_video_from_frames(frames, output_path, fps=30):
        """
        Create video from simulation frames.
        
        Args:
            frames: List of frame arrays
            output_path: Output video file path
            fps: Frames per second
        """
        try:
            import cv2
            
            # Get frame dimensions
            height, width = frames[0].shape[:2]
            fourcc = cv2.VideoWriter_fourcc(*'mp4v')
            video_writer = cv2.VideoWriter(output_path, fourcc, fps, (width, height))
            
            for frame in frames:
                video_writer.write(frame)
            
            video_writer.release()
            return output_path
            
        except ImportError:
            print("OpenCV not available. Skipping video creation.")
            return None
class ConfigurationTools:
    """Tools for working with NLM configurations."""
    
    @staticmethod
    def create_preset_config(preset_name):
        """
        Create a preset configuration.
        
        Args:
            preset_name: Name of preset ("basic", "advanced", "research")
            
        Returns:
            Config object with preset settings
        """
        if not HAS_NLM:
            return None
        
        config = nlm.createDefaultConfig()
        
        if preset_name.lower() == "basic":
            # Basic configuration for simple experiments
            config.setInt("brain.region_count", 3)
            config.setInt("neuron.count_per_region", 100)
            config.setInt("neuron.excitatory_ratio", 0.8)
            config.setInt("neuron.inhibitory_ratio", 0.2)
            config.setFloat("neuron.threshold", -55.0)
            config.setFloat("neuron.reset_potential", -70.0)
            config.setFloat("neuron.leak_conductance", 10.0)
            config.setFloat("synapse.weight_range", 0.5)
            config.setInt("plasticity.stdp.enabled", 1)
            config.setInt("plasticity.hebbian.enabled", 0)
            config.setInt("development.enabled", 1)
            config.setFloat("development.critical_period_end", 0.5)
            
        elif preset_name.lower() == "advanced":
            # Advanced configuration for complex experiments
            config.setInt("brain.region_count", 8)
            config.setInt("neuron.count_per_region", 500)
            config.setInt("neuron.excitatory_ratio", 0.7)
            config.setInt("neuron.inhibitory_ratio", 0.2)
            config.setInt("neuron.modulatory_ratio", 0.1)
            config.setFloat("neuron.threshold", -50.0)
            config.setFloat("neuron.reset_potential", -65.0)
            config.setFloat("neuron.leak_conductance", 15.0)
            config.setFloat("synapse.weight_range", 1.0)
            config.setInt("plasticity.stdp.enabled", 1)
            config.setInt("plasticity.hebbian.enabled", 1)
            config.setInt("plasticity.reward_modulated.enabled", 1)
            config.setInt("plasticity.structural.enabled", 1)
            config.setInt("development.enabled", 1)
            config.setInt("neuromodulation.dopamine.enabled", 1)
            config.setInt("neuromodulation.curiosity.enabled", 1)
            config.setInt("neuromodulation.novelty.enabled", 1)
            config.setInt("neuromodulation.prediction_error.enabled", 1)
            config.setInt("memory.working.enabled", 1)
            config.setInt("memory.episodic.enabled", 1)
            config.setInt("memory.associative.enabled", 1)
            
        elif preset_name.lower() == "research":
            # Research configuration for comprehensive studies
            config.setInt("brain.region_count", 12)
            config.setInt("neuron.count_per_region", 1000)
            config.setInt("neuron.excitatory_ratio", 0.65)
            config.setInt("neuron.inhibitory_ratio", 0.2)
            config.setInt("neuron.modulatory_ratio", 0.1)
            config.setInt("neuron.sensory_ratio", 0.03)
            config.setInt("neuron.motor_ratio", 0.03)
            config.setFloat("neuron.threshold", -45.0)
            config.setFloat("neuron.reset_potential", -60.0)
            config.setFloat("neuron.leak_conductance", 20.0)
            config.setFloat("synapse.weight_range", 2.0)
            config.setInt("plasticity.stdp.enabled", 1)
            config.setInt("plasticity.hebbian.enabled", 1)
            config.setInt("plasticity.reward_modulated.enabled", 1)
            config.setInt("plasticity.structural.enabled", 1)
            config.setInt("development.enabled", 1)
            config.setInt("neuromodulation.dopamine.enabled", 1)
            config.setInt("neuromodulation.curiosity.enabled", 1)
            config.setInt("neuromodulation.novelty.enabled", 1)
            config.setInt("neuromodulation.prediction_error.enabled", 1)
            config.setInt("memory.working.enabled", 1)
            config.setInt("memory.episodic.enabled", 1)
            config.setInt("memory.associative.enabled", 1)
            config.setInt("prediction.enabled", 1)
            config.setInt("cognition.planning.enabled", 1)
            config.setInt("cognition.concept_formation.enabled", 1)
            config.setInt("cognition.attention.enabled", 1)
            config.setFloat("simulation.dt", 0.01)
            config.setInt("simulation.max_steps", 10000)
            
        else:
            raise ValueError(f"Unknown preset: {preset_name}")
        
        return config
    
    @staticmethod
    def export_config(config, filepath):
        """
        Export configuration to JSON file.
        
        Args:
            config: Config object
            filepath: Output file path
        """
        config_data = {
            'keys': config.getKeys(),
            'values': {}
        }
        
        for key in config.getKeys():
            # Get value as string representation
            if config.has(key):
                config_data['values'][key] = str(config.get(key))
        
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
    
    @staticmethod
    def import_config(filepath):
        """
        Import configuration from JSON file.
        
        Args:
            filepath: Input file path
            
        Returns:
            Config object with imported settings
        """
        if not HAS_NLM:
            return None
        
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = nlm.createDefaultConfig()
        
        # Import configuration values
        for key, value_str in config_data.get('values', {}).items():
            # Try to parse value based on content
            if value_str.lower() == 'true':
                config.set(key, True)
            elif value_str.lower() == 'false':
                config.set(key, False)
            elif value_str.isdigit():
                config.set(key, int(value_str))
            else:
                try:
                    float_val = float(value_str)
                    if '.' in value_str:
                        config.set(key, float_val)
                    else:
                        config.set(key, int(float_val))
                except ValueError:
                    config.set(key, value_str)
        
        return config
class PerformanceMonitor:
    """Monitor and profile simulation performance."""
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_speed': [],
            'spike_counts': []
        }
        self.start_time = None
        self.step_count = 0
    
    def start_step(self):
        """Mark start of simulation step."""
        self.start_time = time.time()
    
    def end_step(self, brain_stats=None):
        """
        Mark end of simulation step and record metrics.
        
        Args:
            brain_stats: Brain statistics dictionary
        """
        if self.start_time is None:
            return
        
        step_time = time.time() - self.start_time
        self.metrics['step_times'].append(step_time)
        self.metrics['simulation_speed'].append(1.0 / step_time if step_time > 0 else 0)
        
        if brain_stats:
            self.metrics['spike_counts'].append(brain_stats.get('spike_count', 0))
        
        # Keep only last 1000 entries
        for key in self.metrics:
            if len(self.metrics[key]) > 1000:
                self.metrics[key] = self.metrics[key][-1000:]
        
        self.start_time = None
        self.step_count += 1
    
    def get_average_step_time(self):
        """Get average step time."""
        if not self.metrics['step_times']:
            return 0.0
        return sum(self.metrics['step_times']) / len(self.metrics['step_times'])
    
    def get_average_simulation_speed(self):
        """Get average simulation speed (steps per second)."""
        if not self.metrics['simulation_speed']:
            return 0.0
        return sum(self.metrics['simulation_speed']) / len(self.metrics['simulation_speed'])
    
    def get_performance_report(self):
        """Get performance report."""
        if not self.metrics['step_times']:
            return {'status': 'no_data', 'step_count': self.step_count}
        
        return {
            'status': 'active',
            'step_count': self.step_count,
            'average_step_time': self.get_average_step_time(),
            'average_simulation_speed': self.get_average_simulation_speed(),
            'total_steps_recorded': len(self.metrics['step_times']),
            'recent_step_time_std': np.std(self.metrics['step_times'][-100:]) if len(self.metrics['step_times']) >= 100 else 0.0
        }
    
    def reset(self):
        """Reset performance metrics."""
        self.metrics = {
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_speed': [],
            'spike_counts': []
        }
        self.start_time = None
        self.step_count = 0
def get_version():
    """Get NLM Python bindings version."""
    if not HAS_NLM:
        return "NLM not available"
    
    return nlm.version()
def get_system_info():
    """Get comprehensive system information."""
    info = {
        'nlm_available': HAS_NLM,
        'version': get_version(),
        'modules': []
    }
    
    if HAS_NLM:
        info['modules'] = [
            'SimulationManager',
            'DataAnalyzer', 
            'VisualizationHelper',
            'ConfigurationTools',
            'PerformanceMonitor'
        ]
    
    return info
# Convenience factory functions
def create_simulation(preset="basic", width=100, height=100):
    """
    Create a new simulation with preset configuration.
    
    Args:
        preset: Configuration preset ("basic", "advanced", "research")
        width: World width
        height: World height
        
    Returns:
        SimulationManager object
    """
    if not HAS_NLM:
        raise ImportError("NLM bindings not available")
    
    config = ConfigurationTools.create_preset_config(preset)
    world = nlm.createSimpleWorld()
    
    sim = SimulationManager(config, world)
    sim.initialize(width, height)
    
    return sim
def create_research_simulation(width=200, height=200):
    """
    Create a research-level simulation.
    
    Args:
        width: World width
        height: World height
        
    Returns:
        SimulationManager object with research preset
    """
    return create_simulation("research", width, height)
def create_learning_simulation(width=100, height=100):
    """
    Create a learning-focused simulation.
    
    Args:
        width: World width
        height: World height
        
    Returns:
        SimulationManager object with advanced preset
    """
    return create_simulation("advanced", width, height)
# Auto-register NLM with the Python module
def _register_nlm():
    """Register NLM bindings with the enhanced Python module."""
    if HAS_NLM:
        # Create a custom module that wraps NLM with enhanced utilities
        import sys
        import types
        
        # Create enhanced module
        enhanced_module = types.ModuleType('pynlm_enhanced')
        enhanced_module.__doc__ = __doc__
        
        # Add all utility classes
        enhanced_module.SimulationManager = SimulationManager
        enhanced_module.DataAnalyzer = DataAnalyzer
        enhanced_module.VisualizationHelper = VisualizationHelper
        enhanced_module.ConfigurationTools = ConfigurationTools
        enhanced_module.PerformanceMonitor = PerformanceMonitor
        
        # Add factory functions
        enhanced_module.create_simulation = create_simulation
        enhanced_module.create_research_simulation = create_research_simulation
        enhanced_module.create_learning_simulation = create_learning_simulation
        enhanced_module.get_version = get_version
        enhanced_module.get_system_info = get_system_info
        
        # Add original NLM bindings
        enhanced_module.nlm = nlm
        
        # Add utility functions
        enhanced_module.json = json
        enhanced_module.np = np
        
        # Register in sys.modules
        sys.modules['pynlm_enhanced'] = enhanced_module
        
        # Also make it available as an attribute of the original nlm module
        if hasattr(nlm, '__dict__'):
            nlm.SimulationManager = SimulationManager
            nlm.DataAnalyzer = DataAnalyzer
            nlm.VisualizationHelper = VisualizationHelper
            nlm.ConfigurationTools = ConfigurationTools
            nlm.PerformanceMonitor = PerformanceMonitor

# Register on module import
_register_nlm()
if __name__ == "__main__":
    # Test the enhanced module
    print("NLM Enhanced Python Utilities")
    print("=" * 40)
    
    info = get_system_info()
    print(f"NLM Available: {info['nlm_available']}")
    print(f"Version: {info['version']}")
    print(f"Available Modules: {', '.join(info['modules'])}")
    
    # Test simulation creation
    try:
        sim = create_simulation("basic", 50, 50)
        print(f"Created simulation with {sim.brain.getRegionCount()} regions")
        print(f"Total neurons: {sim.brain.getTotalNeuronCount()}")
        print(f"Status: {sim.get_status()}")
        
        # Run a short episode
        stats = sim.run_episode(max_steps=10, random_seed=42)
        print(f"Episode completed in {stats['steps']} steps")
        print(f"Total reward: {stats['total_reward']}")
        print(f"Success: {stats['success']}")
        
    except Exception as e:
        print(f"Test failed: {e}")
        import traceback
        traceback.print_exc()