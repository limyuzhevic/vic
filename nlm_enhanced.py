#!/usr/bin/env python3

"""
NLM Python Enhanced API

This module provides an enhanced Python interface for the NLM neural simulation framework.
It adds convenience methods, advanced functionality, and better error handling to the
base pybind11 bindings.

Key improvements:
- Intuitive initialization methods
- Comprehensive memory management
- Advanced simulation control
- Detailed performance monitoring
- Enhanced error handling and logging
"""

import numpy as np
import time
import logging
from typing import Optional, List, Dict, Any, Callable

# Try to import the base NLM bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some enhanced features will not work.")
class NLMConfigurator:
    """
    Enhanced configuration manager for NLM brains with validation and presets.
    """
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLMConfigurator")
        
        self._config = pynlm.createDefaultConfig()
        self._presets = {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'neuromod.dopamine.scale': 1.0
            },
            'balanced': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.15,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'neuromod.dopamine.scale': 1.0,
                'neuromod.curiosity.enable': True
            },
            'complex': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.2,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.dopamine.scale': 1.5,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True
            },
            'research': {
                'brain.neuron_count': 10000,
                'brain.synapse_density': 0.25,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.dopamine.scale': 2.0,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.prediction_error.enable': True
            }
        }
    
    def apply_preset(self, preset_name: str):
        """Apply a predefined configuration preset."""
        if preset_name not in self._presets:
            available = list(self._presets.keys())
            raise ValueError(f"Unknown preset '{preset_name}'. Available: {available}")
        
        for key, value in self._presets[preset_name].items():
            self._config.set(key, value)
        
        return self
    
    def set_neural_parameters(self, neuron_count: int = None, synapse_density: float = None,
                             connection_probability: float = None, threshold: float = None,
                             resting_potential: float = None):
        """Set neural system parameters with validation."""
        if neuron_count is not None:
            if neuron_count < 10 or neuron_count > 100000:
                raise ValueError("neuron_count must be between 10 and 100000")
            self._config.set("brain.neuron_count", neuron_count)
        
        if synapse_density is not None:
            if synapse_density < 0.01 or synapse_density > 1.0:
                raise ValueError("synapse_density must be between 0.01 and 1.0")
            self._config.set("brain.synapse_density", synapse_density)
        
        if connection_probability is not None:
            if connection_probability < 0.0 or connection_probability > 1.0:
                raise ValueError("connection_probability must be between 0.0 and 1.0")
            self._config.set("brain.connection_probability", connection_probability)
        
        if threshold is not None:
            self._config.set("brain.v_thresh", threshold)
        
        if resting_potential is not None:
            self._config.set("brain.v_rest", resting_potential)
        
        return self
    
    def set_plasticity_parameters(self, stdp_enable: bool = None, stdp_rate: float = None,
                                  hebbian_enable: bool = None, structural_enable: bool = None):
        """Set plasticity mechanism parameters."""
        if stdp_enable is not None:
            self._config.set("plasticity.stdp.enable", stdp_enable)
        
        if stdp_rate is not None:
            if stdp_rate < 0.0 or stdp_rate > 1.0:
                raise ValueError("stdp_rate must be between 0.0 and 1.0")
            self._config.set("plasticity.stdp.learning_rate", stdp_rate)
        
        if hebbian_enable is not None:
            self._config.set("plasticity.hebbian.enable", hebbian_enable)
        
        if structural_enable is not None:
            self._config.set("plasticity.structural.enable", structural_enable)
        
        return self
    
    def set_neuromodulation_parameters(self, dopamine_scale: float = None,
                                      curiosity_enable: bool = None,
                                      novelty_enable: bool = None,
                                      prediction_error_enable: bool = None):
        """Set neuromodulation parameters."""
        if dopamine_scale is not None:
            if dopamine_scale < 0.0:
                raise ValueError("dopamine_scale must be non-negative")
            self._config.set("neuromod.dopamine.scale", dopamine_scale)
        
        if curiosity_enable is not None:
            self._config.set("neuromod.curiosity.enable", curiosity_enable)
        
        if novelty_enable is not None:
            self._config.set("neuromod.novelty.enable", novelty_enable)
        
        if prediction_error_enable is not None:
            self._config.set("neuromod.prediction_error.enable", prediction_error_enable)
        
        return self
    
    def validate_config(self) -> Dict[str, Any]:
        """Validate current configuration and return validation results."""
        validation_results = {
            'valid': True,
            'warnings': [],
            'errors': []
        }
        
        # Check neuron count
        neuron_count = self._config.getOr("brain.neuron_count", 1000)
        if neuron_count < 10:
            validation_results['errors'].append("Neuron count too low (< 10)")
            validation_results['valid'] = False
        elif neuron_count > 100000:
            validation_results['warnings'].append("Large neuron count may impact performance")
        
        # Check synapse density
        synapse_density = self._config.getOr("brain.synapse_density", 0.15)
        if synapse_density < 0.01 or synapse_density > 1.0:
            validation_results['errors'].append("Invalid synapse density")
            validation_results['valid'] = False
        
        # Check for conflicting configurations
        stdp_enabled = self._config.getOr("plasticity.stdp.enable", True)
        hebbian_enabled = self._config.getOr("plasticity.hebbian.enable", True)
        
        if stdp_enabled and hebbian_enabled:
            validation_results['warnings'].append("Both STDP and Hebbian enabled - may cause interference")
        
        return validation_results
    
    def get_config_summary(self) -> str:
        """Get a human-readable summary of the configuration."""
        validation = self.validate_config()
        
        summary_lines = [
            "NLM Configuration Summary",
            "=========================",
            f"Neurons: {self._config.getOr('brain.neuron_count', 1000)}",
            f"Synapse Density: {self._config.getOr('brain.synapse_density', 0.15):.3f}",
            f"STDP Enabled: {self._config.getOr('plasticity.stdp.enable', True)}",
            f"Hebbian Enabled: {self._config.getOr('plasticity.hebbian.enable', True)}",
            f"Curiosity Enabled: {self._config.getOr('neuromod.curiosity.enable', True)}",
        ]
        
        if validation['warnings']:
            summary_lines.append("")
            summary_lines.append("Warnings:")
            for warning in validation['warnings']:
                summary_lines.append(f"  - {warning}")
        
        return "\n".join(summary_lines)
    
    def save_to_file(self, filepath: str):
        """Save configuration to file."""
        self._config.saveToFile(filepath)
    
    def load_from_file(self, filepath: str):
        """Load configuration from file."""
        self._config.loadFromFile(filepath)
        return self
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        pass
class NLMSimulator:
    """
    Enhanced neural simulator with advanced features and monitoring.
    """
    
    def __init__(self, config=None):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLMSimulator")
        
        if config is None:
            self.config = NLMConfigurator()
        else:
            self.config = config
        
        self.brain = pynlm.createBrain(self.config._config)
        self.agent = None
        self.world = None
        
        # Performance monitoring
        self._performance_data = {
            'step_times': [],
            'spike_counts': [],
            'firing_rates': [],
            'activity_levels': []
        }
        
        # Event callbacks
        self._step_callbacks = []
        self._completion_callbacks = []
        
        # Simulation state
        self._initialized = False
        self._simulating = False
    
    def initialize(self, world=None):
        """Initialize the simulator with optional world setup."""
        if self._initialized:
            print("Simulator already initialized")
            return self
        
        self.brain.initialize()
        self._initialized = True
        
        if world is not None:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(**world.get('config', {}))
            self.world.reset()
            
            if world.get('start_position'):
                self.world.setAgentStart(*world['start_position'])
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable subsystems based on config
            config = self.config._config
            self.agent.enableRewardModulation(config.getOr('neuromod.dopamine.enable', True))
            self.agent.enableStructuralPlasticity(config.getOr('plasticity.structural.enable', True))
            self.agent.enableDevelopment(config.getOr('development.enable', True))
            self.agent.enableCuriosity(config.getOr('neuromod.curiosity.enable', True))
        
        return self
    
    def run_step(self, step_number: int, world_update_time: float = 0.1,
                 collect_metrics: bool = True):
        """Run a single simulation step with metrics collection."""
        if not self._initialized:
            raise RuntimeError("Simulator not initialized. Call initialize() first.")
        
        start_time = time.time()
        
        # Collect metrics if requested
        if collect_metrics:
            metrics = self._collect_step_metrics(step_number)
        
        # Execute simulation step
        if self.agent and self.world:
            # Full agent simulation
            self.world.update(world_update_time)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step_number)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            if collect_metrics:
                self._performance_data['step_times'].append(time.time() - start_time)
        else:
            # Simple brain simulation
            self.brain.step(step_number)
            if collect_metrics:
                self._performance_data['step_times'].append(time.time() - start_time)
        
        # Trigger callbacks
        for callback in self._step_callbacks:
            callback(step_number, self)
        
        return self
    
    def run_episode(self, num_steps: int, world_config: Dict = None,
                    collect_metrics: bool = True, callback: Callable = None):
        """Run a complete simulation episode."""
        if not self._initialized:
            self.initialize(world_config)
        
        self._simulating = True
        
        for step in range(num_steps):
            self.run_step(step, collect_metrics=collect_metrics)
            
            if callback:
                callback(step, self)
        
        self._simulating = False
        
        # Trigger completion callbacks
        for callback in self._completion_callbacks:
            callback(self)
        
        return self
    
    def add_step_callback(self, callback: Callable):
        """Add a callback to be called after each simulation step."""
        self._step_callbacks.append(callback)
        return self
    
    def add_completion_callback(self, callback: Callable):
        """Add a callback to be called when simulation completes."""
        self._completion_callbacks.append(callback)
        return self
    
    def _collect_step_metrics(self, step_number: int):
        """Collect metrics for the current step."""
        metrics = {
            'step': step_number,
            'time': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'activity_level': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        if self.agent:
            metrics.update({
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation': self.agent.getNeuromodulationLevel()
            })
        
        # Store metrics
        for key, value in metrics.items():
            if key in self._performance_data:
                self._performance_data[key].append(value)
        
        return metrics
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get a summary of simulation performance."""
        if not self._performance_data['step_times']:
            return {'error': 'No performance data collected'}
        
        summary = {
            'total_steps': len(self._performance_data['step_times']),
            'total_time': sum(self._performance_data['step_times']),
            'avg_step_time': np.mean(self._performance_data['step_times']),
            'min_step_time': np.min(self._performance_data['step_times']),
            'max_step_time': np.max(self._performance_data['step_times']),
            'final_neuron_count': self._performance_data['neuron_count'][-1],
            'final_synapse_count': self._performance_data['synapse_count'][-1],
            'final_firing_rate': self._performance_data['firing_rate'][-1],
            'activity_trend': self._calculate_activity_trend()
        }
        
        if self.agent:
            summary.update({
                'final_curiosity': self._performance_data['curiosity'][-1],
                'final_novelty': self._performance_data['novelty'][-1],
                'final_prediction_error': self._performance_data['prediction_error'][-1]
            })
        
        return summary
    
    def _calculate_activity_trend(self) -> str:
        """Calculate the overall activity trend."""
        if len(self._performance_data['firing_rate']) < 2:
            return 'insufficient_data'
        
        recent_rates = self._performance_data['firing_rate'][-10:]
        earlier_rates = self._performance_data['firing_rate'][:-10] if len(self._performance_data['firing_rate']) > 10 else []
        
        if not earlier_rates:
            return 'stable'
        
        recent_avg = np.mean(recent_rates)
        earlier_avg = np.mean(earlier_rates)
        
        if recent_avg > earlier_avg * 1.1:
            return 'increasing'
        elif recent_avg < earlier_avg * 0.9:
            return 'decreasing'
        else:
            return 'stable'
    
    def save_state(self, filepath: str):
        """Save simulation state to file."""
        if self.world:
            self.brain.save(filepath)
    
    def load_state(self, filepath: str):
        """Load simulation state from file."""
        self.brain.load(filepath)
        return self
    
    def reset(self):
        """Reset the simulator to initial state."""
        if self._initialized:
            self.brain.reset()
            if self.agent and self.world:
                self.agent.reset()
            self._performance_data = {
                'step_times': [],
                'spike_counts': [],
                'firing_rates': [],
                'activity_levels': []
            }
        
        return self
class NLMEnvironment:
    """
    Advanced environment manager for NLM simulations with multiple world types.
    """
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLMEnvironment")
    
    def create_simple_world(self, width: int = 20, height: int = 20,
                          vision_width: int = 8, vision_height: int = 8,
                          max_energy: float = 100.0) -> Dict:
        """Create a simple 2D world configuration."""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        world.setMaxEnergy(max_energy)
        world.setRandomSeed(42)
        
        return {
            'world': world,
            'config': {
                'width': width,
                'height': height,
                'vision_width': vision_width,
                'vision_height': vision_height,
                'max_energy': max_energy
            }
        }
    
    def create_multi_room_world(self, rooms: List[Dict], connections: List[Dict] = None):
        """
        Create a multi-room world with doors and obstacles.
        This is a simplified example that extends the simple world.
        """
        # This would be implemented as an extension of SimpleWorld
        # For now, return a basic configuration
        if not connections:
            connections = []
        
        return {
            'world_type': 'multi_room',
            'rooms': rooms,
            'connections': connections
        }
    
    def create_sensory_rich_world(self, object_density: float = 0.3,
                                 resource_ratio: float = 0.6,
                                 hazard_ratio: float = 0.3):
        """
        Create a world with rich sensory input (many objects, resources, hazards).
        """
        world_config = self.create_simple_world(width=30, height=30,
                                               vision_width=12, vision_height=12)
        
        # Add diverse objects
        for _ in range(int(object_density * 100)):
            # Random position
            x = np.random.uniform(1, world_config['config']['width'] - 1)
            y = np.random.uniform(1, world_config['config']['height'] - 1)
            
            # Random object type
            obj_type = np.random.choice([
                pynlm.WorldObjectType.Resource,
                pynlm.WorldObjectType.Hazard,
                pynlm.WorldObjectType.Wall
            ])
            
            # Random value
            value = np.random.uniform(0.5, 2.0)
            
            world_config['world'].addObject(pynlm.WorldObject(x, y, obj_type, value))
        
        return world_config
    
    def create_developmental_world(self, stages: List[Dict], transition_probabilities: float = 0.1):
        """
        Create a world designed for developmental experiments.
        """
        world_config = self.create_simple_world(width=25, height=25,
                                               vision_width=10, vision_height=10)
        
        # Add developmental stages (simplified)
        for stage in stages:
            # This would be implemented as part of the developmental system
            pass
        
        return world_config
class NLMAnalysis:
    """
    Advanced analysis tools for NLM simulation data.
    """
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLMAnalysis")
    
    def analyze_brain_activity(self, brain, steps: int = 1000) -> Dict[str, Any]:
        """Analyze brain activity patterns over time."""
        activity_data = []
        
        for step in range(steps):
            brain.step(step)
            activity = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'ei_ratio': brain.getExcitationInhibitionRatio()
            }
            activity_data.append(activity)
        
        analysis = self._compute_activity_statistics(activity_data)
        analysis['raw_data'] = activity_data
        
        return analysis
    
    def analyze_plasticity_dynamics(self, brain, steps: int = 1000) -> Dict[str, Any]:
        """Analyze synaptic plasticity dynamics."""
        # This would require access to plasticity system
        # For now, return a basic structure
        return {
            'stdp_effects': [],
            'hebbian_effects': [],
            'structural_changes': []
        }
    
    def _compute_activity_statistics(self, activity_data: List[Dict]) -> Dict[str, Any]:
        """Compute statistical analysis of activity data."""
        firing_rates = [d['firing_rate'] for d in activity_data]
        firing_neuron_counts = [d['firing_neurons'] for d in activity_data]
        
        def compute_stats(data, name):
            return {
                'mean': np.mean(data),
                'std': np.std(data),
                'min': np.min(data),
                'max': np.max(data),
                'median': np.median(data),
                'percentiles': {
                    '25': np.percentile(data, 25),
                    '75': np.percentile(data, 75)
                }
            }
        
        return {
            'firing_rate_stats': compute_stats(firing_rates, 'firing_rate'),
            'firing_neuron_stats': compute_stats(firing_neuron_counts, 'firing_neuron_count'),
            'activity_patterns': self._detect_activity_patterns(activity_data)
        }
    
    def _detect_activity_patterns(self, activity_data: List[Dict]) -> Dict[str, Any]:
        """Detect interesting activity patterns."""
        patterns = {
            'bursting': [],
            'silencing': [],
            'oscillations': [],
            'extinction': []
        }
        
        # Simple pattern detection
        for i in range(1, len(activity_data) - 1):
            current_rate = activity_data[i]['firing_rate']
            prev_rate = activity_data[i-1]['firing_rate']
            next_rate = activity_data[i+1]['firing_rate']
            
            # Burst detection (rate increase followed by decrease)
            if current_rate > prev_rate * 1.5 and current_rate > next_rate * 1.5:
                patterns['bursting'].append(activity_data[i]['step'])
            
            # Silencing (rate decrease)
            elif current_rate < prev_rate * 0.5:
                patterns['silencing'].append(activity_data[i]['step'])
        
        return patterns
class NLMExampleGenerator:
    """
    Generate example simulations and educational content for NLM.
    """
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLMExampleGenerator")
    
    def generate_neuron_learning_example(self, output_file: str = None):
        """Generate an example showing neuron learning through plasticity."""
        config = NLMConfigurator()
        config.set_neural_parameters(neuron_count=200, synapse_density=0.1)
        config.set_plasticity_parameters(stdp_enable=True, hebbian_enable=True)
        
        simulator = NLMSimulator(config)
        
        # Run learning episode
        simulator.initialize()
        
        def learning_callback(step, sim):
            if step % 100 == 0:
                print(f"Step {step}: {sim.brain.getAverageFiringRate():.2f} Hz")
        
        simulator.add_step_callback(learning_callback)
        simulator.run_episode(500)
        
        if output_file:
            performance = simulator.get_performance_summary()
            with open(output_file, 'w') as f:
                f.write(f"Neuron Learning Example\n")
                f.write(f"======================\n")
                f.write(f"Total Steps: {performance['total_steps']}\n")
                f.write(f"Average Step Time: {performance['avg_step_time']:.4f}s\n")
                f.write(f"Final Firing Rate: {performance['final_firing_rate']:.2f} Hz\n")
                f.write(f"Activity Trend: {performance['activity_trend']}\n")
        
        return simulator
    
    def generate_agent_exploration_example(self, output_file: str = None):
        """Generate an example showing agent exploration and learning."""
        config = NLMConfigurator().apply_preset('complex')
        
        # Create rich world
        world_config = {
            'config': {
                'width': 30,
                'height': 30,
                'vision_width': 12,
                'vision_height': 12
            },
            'start_position': (15.0, 15.0)
        }
        
        simulator = NLMSimulator(config)
        simulator.initialize(world_config)
        
        # Run exploration episode
        def exploration_callback(step, sim):
            if step % 100 == 0:
                print(f"Step {step}: Curiosity={sim.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={sim.agent.getNoveltyLevel():.3f}")
        
        simulator.add_step_callback(exploration_callback)
        simulator.run_episode(1000)
        
        if output_file:
            performance = simulator.get_performance_summary()
            with open(output_file, 'w') as f:
                f.write(f"Agent Exploration Example\n")
                f.write(f"=========================\n")
                f.write(f"Total Steps: {performance['total_steps']}\n")
                f.write(f"Final Curiosity: {performance['final_curiosity']:.3f}\n")
                f.write(f"Final Novelty: {performance['final_novelty']:.3f}\n")
                f.write(f"Final Prediction Error: {performance['final_prediction_error']:.3f}\n")
        
        return simulator
    
    def generate_performance_benchmark_example(self, output_file: str = None):
        """Generate a performance benchmark example."""
        import time
        
        configs = {
            'minimal': NLMConfigurator().apply_preset('minimal'),
            'balanced': NLMConfigurator().apply_preset('balanced'),
            'complex': NLMConfigurator().apply_preset('complex')
        }
        
        benchmarks = {}
        
        for name, config in configs.items():
            print(f"Benchmarking {name} configuration...")
            
            simulator = NLMSimulator(config)
            start_time = time.time()
            
            simulator.initialize()
            simulator.run_episode(200)
            
            end_time = time.time()
            
            benchmarks[name] = {
                'total_time': end_time - start_time,
                'steps_per_second': 200 / (end_time - start_time),
                'final_firing_rate': simulator.get_performance_summary()['final_firing_rate']
            }
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(f"NLM Performance Benchmark\n")
                f.write(f"==========================\n")
                
                for name, results in benchmarks.items():
                    f.write(f"\n{name.upper()} configuration:\n")
                    f.write(f"  Total time: {results['total_time']:.2f}s\n")
                    f.write(f"  Steps per second: {results['steps_per_second']:.1f}\n")
                    f.write(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz\n")
        
        return benchmarks
# Main class that wraps all the enhanced functionality
class NLM:
    """
    Enhanced NLM (Neural Learning Machine) interface with advanced features.
    
    This class provides a comprehensive interface to the NLM neural simulation framework,
    including configuration management, simulation control, environment creation,
    data analysis, and educational examples.
    """
    
    def __init__(self, auto_initialize: bool = False):
        """
        Initialize the enhanced NLM interface.
        
        Args:
            auto_initialize (bool): Whether to automatically initialize on creation
        """
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for NLM enhanced interface")
        
        self._configurator = NLMConfigurator()
        self._simulator = None
        self._environment = NLMEnvironment()
        self._analysis = NLMAnalysis()
        self._examples = NLMExampleGenerator()
        
        if auto_initialize:
            self._simulator = NLMSimulator(self._configurator)
            self._simulator.initialize()
    
    # Configuration interface
    def create_config(self, preset: str = None) -> NLMConfigurator:
        """Create a new NLM configurator."""
        if preset:
            return self._configurator.apply_preset(preset)
        return NLMConfigurator()
    
    # Simulation interface
    def create_simulator(self, config: NLMConfigurator = None) -> NLMSimulator:
        """Create a new NLM simulator."""
        if config is None:
            config = self._configurator
        return NLMSimulator(config)
    
    # Environment interface
    def create_environment(self) -> NLMEnvironment:
        """Create an environment manager."""
        return NLMEnvironment()
    
    # Analysis interface
    def create_analysis(self) -> NLMAnalysis:
        """Create an analysis tool."""
        return NLMAnalysis()
    
    # Example interface
    def create_examples(self) -> NLMExampleGenerator:
        """Create an example generator."""
        return NLMExampleGenerator()
    
    # Quick access methods
    def quick_run(self, steps: int = 100, preset: str = 'minimal') -> NLMSimulator:
        """Run a quick simulation with a preset configuration."""
        config = self.create_config(preset)
        simulator = self.create_simulator(config)
        simulator.initialize()
        simulator.run_episode(steps)
        return simulator
    
    def benchmark(self, steps: int = 200, presets: List[str] = None) -> Dict[str, Any]:
        """Benchmark different configuration presets."""
        if presets is None:
            presets = ['minimal', 'balanced', 'complex']
        
        results = {}
        for preset in presets:
            print(f"Benchmarking {preset} preset...")
            config = self.create_config(preset)
            simulator = self.create_simulator(config)
            
            import time
            start_time = time.time()
            simulator.initialize()
            simulator.run_episode(steps)
            end_time = time.time()
            
            results[preset] = {
                'time': end_time - start_time,
                'steps_per_second': steps / (end_time - start_time),
                'performance': simulator.get_performance_summary()
            }
        
        return results
    
    def example_learning(self, output_file: str = None) -> NLMSimulator:
        """Generate and run a neuron learning example."""
        return self._examples.generate_neuron_learning_example(output_file)
    
    def example_exploration(self, output_file: str = None) -> NLMSimulator:
        """Generate and run an agent exploration example."""
        return self._examples.generate_agent_exploration_example(output_file)
    
    def example_benchmark(self, output_file: str = None) -> Dict[str, Any]:
        """Generate and run a performance benchmark."""
        return self._examples.generate_performance_benchmark_example(output_file)
# Create a global instance for convenience
_nlm_instance = None

def get_nlm() -> NLM:
    """Get or create the global NLM instance."""
    global _nlm_instance
    if _nlm_instance is None:
        _nlm_instance = NLM()
    return _nlm_instance

# Convenience functions for quick usage
def quick_brain(neuron_count: int = 100, steps: int = 100) -> pynlm.Brain:
    """Create and run a quick brain simulation."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain

def quick_agent_episode(steps: int = 200, preset: str = 'balanced') -> NLMSimulator:
    """Run a quick agent episode with preset configuration."""
    nlm = get_nlm()
    return nlm.quick_run(steps, preset)

def benchmark_presets(steps: int = 200, presets: List[str] = None) -> Dict[str, Any]:
    """Benchmark different configuration presets."""
    nlm = get_nlm()
    return nlm.benchmark(steps, presets)
if __name__ == "__main__":
    print("NLM Enhanced Python Interface")
    print("============================")
    print("Import this module to use enhanced NLM functionality:")
    print("  from nlm_enhanced import NLM, get_nlm, quick_brain")
    print()
    print("Quick examples:")
    print("  nlm = NLM()")
    print("  sim = nlm.quick_run(100, 'balanced')")
    print("  nlm.benchmark_presets(200, ['minimal', 'balanced', 'complex'])")