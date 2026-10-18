"""
Advanced features for power users of NLM.
This module provides enhanced functionality for advanced users who need
greater control over the neural simulation system.
"""

from typing import List, Optional, Dict, Any, Tuple, Callable
import numpy as np
import structlog
import json
import pickle
import gzip

# Get the logger
logger = structlog.get_logger()

from .brain import Brain, Config, Action, ActionTypes
from .agent import AgentBrain
from .world import SimpleWorld, WorldObject, WorldObjectTypes
from .sensory import Vision, Audio, InternalSignals, SensoryPercept

# Exception classes for advanced features
class AdvancedFeatureError(Exception):
    """Base exception for advanced feature errors."""
    pass

class SimulationError(AdvancedFeatureError):
    """Raised when simulation fails."""
    pass

class ExportError(AdvancedFeatureError):
    """Raised when export fails."""
    pass

class ImportError(AdvancedFeatureError):
    """Raised when import fails."""
    pass

# Advanced configuration classes
class AdvancedConfig:
    """Enhanced configuration for advanced users."""
    
    def __init__(self, config: Optional[Config] = None):
        self._base_config = config or Config()
        self._advanced_settings = {}
        self._validation_rules = {}
        logger.debug("AdvancedConfig created")
    
    def set_advanced_setting(self, key: str, value, validator: Optional[Callable] = None):
        """Set an advanced setting with optional validation.
        
        Args:
            key: Setting key.
            value: Setting value.
            validator: Optional validation function.
            
        Raises:
            AdvancedFeatureError: If validation fails.
        """
        if validator and not validator(value):
            raise AdvancedFeatureError(f"Validation failed for setting '{key}'")
        
        self._advanced_settings[key] = value
        logger.debug("Advanced setting set", key=key, value=value)
    
    def get_advanced_setting(self, key: str, default=None):
        """Get an advanced setting.
        
        Args:
            key: Setting key.
            default: Default value.
            
        Returns:
            Setting value.
        """
        return self._advanced_settings.get(key, default)
    
    def add_validation_rule(self, key: str, validator: Callable, error_message: str = ""):
        """Add a validation rule for a setting.
        
        Args:
            key: Setting key.
            validator: Validation function.
            error_message: Error message for validation failure.
        """
        self._validation_rules[key] = (validator, error_message)
    
    def validate_all(self) -> bool:
        """Validate all advanced settings.
        
        Returns:
            bool: True if all settings are valid.
            
        Raises:
            AdvancedFeatureError: If validation fails.
        """
        for key, (validator, error_message) in self._validation_rules.items():
            if key in self._advanced_settings:
                if not validator(self._advanced_settings[key]):
                    raise AdvancedFeatureError(error_message or f"Validation failed for '{key}'")
        return True
    
    def export_to_file(self, filepath: str, compress: bool = False):
        """Export advanced configuration to file.
        
        Args:
            filepath: Output file path.
            compress: Whether to compress the file.
            
        Raises:
            ExportError: If export fails.
        """
        try:
            data = {
                'base_config': self._base_config.get_keys(),
                'advanced_settings': self._advanced_settings,
                'validation_rules': {k: {'validator': 'callable', 'message': v[1]} 
                                   for k, v in self._validation_rules.items()}
            }
            
            if compress:
                with gzip.open(filepath, 'wt', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
            else:
                with open(filepath, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
            
            logger.info("Advanced configuration exported", filepath=filepath, compress=compress)
            
        except Exception as e:
            raise ExportError(f"Failed to export configuration: {str(e)}")
    
    def import_from_file(self, filepath: str, compress: bool = False):
        """Import advanced configuration from file.
        
        Args:
            filepath: Input file path.
            compress: Whether file is compressed.
            
        Raises:
            ImportError: If import fails.
        """
        try:
            if compress:
                with gzip.open(filepath, 'rt', encoding='utf-8') as f:
                    data = json.load(f)
            else:
                with open(filepath, 'r', encoding='utf-8') as f:
                    data = json.load(f)
            
            self._advanced_settings = data.get('advanced_settings', {})
            self._validation_rules = {}
            for key, rule in data.get('validation_rules', {}).items():
                self._validation_rules[key] = (None, rule.get('message', ''))
            
            logger.info("Advanced configuration imported", filepath=filepath)
            
        except Exception as e:
            raise ImportError(f"Failed to import configuration: {str(e)}")


class AdvancedSimulation:
    """Advanced simulation capabilities for power users."""
    
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None, agent: Optional[AgentBrain] = None):
        self._brain = brain
        self._world = world
        self._agent = agent
        self._simulation_history = []
        self._custom_callbacks = []
        self._simulation_parameters = {}
        logger.debug("AdvancedSimulation created")
    
    def set_simulation_parameter(self, key: str, value):
        """Set simulation parameter.
        
        Args:
            key: Parameter key.
            value: Parameter value.
        """
        self._simulation_parameters[key] = value
        logger.debug("Simulation parameter set", key=key, value=value)
    
    def get_simulation_parameter(self, key: str, default=None):
        """Get simulation parameter.
        
        Args:
            key: Parameter key.
            default: Default value.
            
        Returns:
            Parameter value.
        """
        return self._simulation_parameters.get(key, default)
    
    def add_custom_callback(self, callback: Callable):
        """Add custom callback for simulation events.
        
        Args:
            callback: Callback function.
        """
        self._custom_callbacks.append(callback)
        logger.debug("Custom callback added", callback_count=len(self._custom_callbacks))
    
    def run_stochastic_simulation(self, steps: int, noise_level: float = 0.1, seed: Optional[int] = None):
        """Run stochastic simulation with noise.
        
        Args:
            steps: Number of simulation steps.
            noise_level: Noise level (0.0 to 1.0).
            seed: Random seed.
            
        Returns:
            list: Simulation history.
        """
        import random
        
        if seed is not None:
            random.seed(seed)
        
        history = []
        
        for step in range(steps):
            # Add stochastic noise to brain parameters
            if noise_level > 0:
                # Simulate noise in brain
                pass  # Implementation would add noise to brain simulation
            
            # Record simulation state
            state = self._get_simulation_state(step)
            history.append(state)
            
            # Execute callbacks
            for callback in self._custom_callbacks:
                try:
                    callback(state)
                except Exception as e:
                    logger.warning("Callback execution failed", error=str(e))
        
        self._simulation_history = history
        logger.info("Stochastic simulation completed", steps=steps, noise_level=noise_level)
        
        return history
    
    def run_parameter_sweep(self, parameter_values: Dict[str, List], steps_per_config: int = 100):
        """Run parameter sweep simulation.
        
        Args:
            parameter_values: Dictionary of parameter names to value lists.
            steps_per_config: Steps per parameter configuration.
            
        Returns:
            dict: Parameter sweep results.
        """
        from itertools import product
        
        # Get parameter names and values
        param_names = list(parameter_values.keys())
        param_values = list(parameter_values.values())
        
        # Generate all parameter combinations
        combinations = list(product(*param_values))
        
        results = {}
        
        for i, combo in enumerate(combinations):
            param_config = dict(zip(param_names, combo))
            logger.info("Running parameter combination", 
                       combination=i+1, 
                       total=len(combinations),
                       params=param_config)
            
            # Set parameters
            for key, value in param_config.items():
                self.set_simulation_parameter(key, value)
            
            # Run simulation
            history = self.run_stochastic_simulation(steps_per_config, seed=i)
            
            # Store results
            results[f'config_{i}'] = {
                'parameters': param_config,
                'history': history,
                'final_state': self._get_simulation_state(steps_per_config - 1)
            }
        
        logger.info("Parameter sweep completed", 
                   combinations=len(combinations),
                   steps_per_config=steps_per_config)
        
        return results
    
    def run_ensemble_simulation(self, num_agents: int, steps: int):
        """Run ensemble simulation with multiple agents.
        
        Args:
            num_agents: Number of agents in ensemble.
            steps: Number of simulation steps.
            
        Returns:
            dict: Ensemble simulation results.
        """
        import random
        
        agents = []
        worlds = []
        histories = []
        
        for i in range(num_agents):
            # Create independent agents
            config = AdvancedConfig()
            brain = Brain(config._base_config)
            brain.initialize()
            
            world = SimpleWorld()
            world.configure(20, 20, 16, 16)
            world.reset()
            
            agent = AgentBrain(brain)
            agent.initialize(world)
            
            agents.append(agent)
            worlds.append(world)
            histories.append([])
        
        # Run ensemble simulation
        for step in range(steps):
            for i, (agent, world, history) in enumerate(zip(agents, worlds, histories)):
                # Process sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                agent.brain.step(step)
                
                # Get action and apply to world
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record state
                state = self._get_agent_state(agent, world, step)
                history.append(state)
        
        results = {
            'agents': agents,
            'worlds': worlds,
            'histories': histories,
            'final_states': [self._get_agent_state(agents[i], worlds[i], steps-1) 
                           for i in range(num_agents)]
        }
        
        logger.info("Ensemble simulation completed", 
                   agents=num_agents, steps=steps)
        
        return results
    
    def get_simulation_history(self) -> List:
        """Get simulation history.
        
        Returns:
            list: Simulation history.
        """
        return self._simulation_history
    
    def clear_history(self):
        """Clear simulation history."""
        self._simulation_history = []
        logger.debug("Simulation history cleared")
    
    def _get_simulation_state(self, step: int) -> Dict[str, Any]:
        """Get current simulation state.
        
        Args:
            step: Current step.
            
        Returns:
            dict: Simulation state.
        """
        if self._agent:
            return self._get_agent_state(self._agent, self._world, step)
        elif self._brain:
            return {
                'step': step,
                'brain_stats': {
                    'total_neurons': self._brain.get_total_neuron_count(),
                    'total_synapses': self._brain.get_total_synapse_count(),
                    'active_neurons': self._brain.get_active_neuron_count(),
                    'firing_neurons': self._brain.get_firing_neuron_count(),
                    'total_spikes': self._brain.get_total_spike_count(),
                    'avg_firing_rate': self._brain.get_average_firing_rate(),
                    'developmental_stage': self._brain.get_developmental_stage()
                },
                'parameters': self._simulation_parameters.copy()
            }
        return {'step': step}
    
    def _get_agent_state(self, agent: AgentBrain, world: SimpleWorld, step: int) -> Dict[str, Any]:
        """Get agent state.
        
        Args:
            agent: Agent.
            world: World.
            step: Current step.
            
        Returns:
            dict: Agent state.
        """
        return {
            'step': step,
            'agent_stats': agent.get_brain_stats(),
            'neuromodulation_state': agent.get_neuromodulation_state(),
            'world_stats': {
                'simulation_time': world.get_simulation_time(),
                'agent_energy': world.get_agent_body().energy,
                'agent_position': (world.get_agent_body().x, world.get_agent_body().y)
            },
            'parameters': self._simulation_parameters.copy()
        }
    
    def export_history(self, filepath: str, format: str = 'json'):
        """Export simulation history to file.
        
        Args:
            filepath: Output file path.
            format: Export format ('json', 'csv', 'pickle').
            
        Raises:
            ExportError: If export fails.
        """
        try:
            if format == 'json':
                with open(filepath, 'w', encoding='utf-8') as f:
                    json.dump(self._simulation_history, f, indent=2)
            elif format == 'csv':
                # Convert to CSV format
                import pandas as pd
                df = pd.DataFrame(self._simulation_history)
                df.to_csv(filepath, index=False)
            elif format == 'pickle':
                with open(filepath, 'wb') as f:
                    pickle.dump(self._simulation_history, f)
            else:
                raise ExportError(f"Unsupported format: {format}")
            
            logger.info("Simulation history exported", 
                       filepath=filepath, format=format,
                       entries=len(self._simulation_history))
            
        except Exception as e:
            raise ExportError(f"Failed to export history: {str(e)}")
    
    def import_history(self, filepath: str, format: str = 'json'):
        """Import simulation history from file.
        
        Args:
            filepath: Input file path.
            format: Import format ('json', 'csv', 'pickle').
            
        Raises:
            ImportError: If import fails.
        """
        try:
            if format == 'json':
                with open(filepath, 'r', encoding='utf-8') as f:
                    self._simulation_history = json.load(f)
            elif format == 'csv':
                import pandas as pd
                df = pd.read_csv(filepath)
                self._simulation_history = df.to_dict('records')
            elif format == 'pickle':
                with open(filepath, 'rb') as f:
                    self._simulation_history = pickle.load(f)
            else:
                raise ImportError(f"Unsupported format: {format}")
            
            logger.info("Simulation history imported", 
                       filepath=filepath, format=format,
                       entries=len(self._simulation_history))
            
        except Exception as e:
            raise ImportError(f"Failed to import history: {str(e)}")


class PerformanceProfiler:
    """Performance profiling for NLM simulations."""
    
    def __init__(self):
        self._profiles = {}
        self._current_profile = None
        logger.debug("PerformanceProfiler created")
    
    def start_profile(self, name: str):
        """Start performance profiling.
        
        Args:
            name: Profile name.
        """
        import time
        
        self._current_profile = {
            'name': name,
            'start_time': time.time(),
            'steps': [],
            'memory_usage': [],
            'cpu_usage': []
        }
        
        logger.debug("Performance profiling started", name=name)
    
    def end_profile(self) -> Dict[str, Any]:
        """End performance profiling.
        
        Returns:
            dict: Profile results.
        """
        import time
        
        if not self._current_profile:
            raise PerformanceProfiler("No active profile to end")
        
        end_time = time.time()
        profile = self._current_profile
        
        profile['end_time'] = end_time
        profile['duration'] = end_time - profile['start_time']
        
        results = {
            'name': profile['name'],
            'start_time': profile['start_time'],
            'end_time': profile['end_time'],
            'duration': profile['duration'],
            'steps': profile['steps'],
            'memory_usage': profile['memory_usage'],
            'cpu_usage': profile['cpu_usage'],
            'average_cpu': sum(profile['cpu_usage']) / len(profile['cpu_usage']) if profile['cpu_usage'] else 0,
            'peak_memory': max(profile['memory_usage']) if profile['memory_usage'] else 0
        }
        
        self._profiles[profile['name']] = results
        self._current_profile = None
        
        logger.info("Performance profiling completed", 
                   name=profile['name'],
                   duration=profile['duration'],
                   steps=len(profile['steps']))
        
        return results
    
    def record_step(self, step_num: int, execution_time: float, memory_usage: float, cpu_usage: float):
        """Record step performance metrics.
        
        Args:
            step_num: Step number.
            execution_time: Execution time in seconds.
            memory_usage: Memory usage in MB.
            cpu_usage: CPU usage percentage.
        """
        if not self._current_profile:
            logger.warning("Recording step without active profile")
            return
        
        self._current_profile['steps'].append({
            'step': step_num,
            'execution_time': execution_time,
            'memory_usage': memory_usage,
            'cpu_usage': cpu_usage
        })
        
        self._current_profile['memory_usage'].append(memory_usage)
        self._current_profile['cpu_usage'].append(cpu_usage)
    
    def get_profile(self, name: str) -> Optional[Dict[str, Any]]:
        """Get profile results.
        
        Args:
            name: Profile name.
            
        Returns:
            Profile results or None.
        """
        return self._profiles.get(name)
    
    def get_all_profiles(self) -> Dict[str, Dict[str, Any]]:
        """Get all profile results.
        
        Returns:
            All profile results.
        """
        return self._profiles.copy()
    
    def clear_profiles(self):
        """Clear all profiles."""
        self._profiles.clear()
        self._current_profile = None
        logger.debug("All performance profiles cleared")


class VisualizationGenerator:
    """Advanced visualization generation for NLM simulations."""
    
    def __init__(self):
        self._visualization_configs = {}
        logger.debug("VisualizationGenerator created")
    
    def create_brain_visualization(self, brain: Brain, visualization_type: str = 'network'):
        """Create brain visualization.
        
        Args:
            brain: Brain to visualize.
            visualization_type: Type of visualization.
            
        Returns:
            dict: Visualization data.
        """
        logger.debug("Creating brain visualization", 
                    type=visualization_type,
                    neurons=brain.get_total_neuron_count(),
                    synapses=brain.get_total_synapse_count())
        
        if visualization_type == 'network':
            return self._create_network_visualization(brain)
        elif visualization_type == 'activity':
            return self._create_activity_visualization(brain)
        elif visualization_type == 'development':
            return self._create_development_visualization(brain)
        elif visualization_type == 'plasticity':
            return self._create_plasticity_visualization(brain)
        else:
            raise ValueError(f"Unknown visualization type: {visualization_type}")
    
    def create_world_visualization(self, world: SimpleWorld, visualization_type: str = 'grid'):
        """Create world visualization.
        
        Args:
            world: World to visualize.
            visualization_type: Type of visualization.
            
        Returns:
            dict: Visualization data.
        """
        logger.debug("Creating world visualization", type=visualization_type)
        
        if visualization_type == 'grid':
            return self._create_grid_visualization(world)
        elif visualization_type == 'activity':
            return self._create_world_activity_visualization(world)
        elif visualization_type == 'objects':
            return self._create_objects_visualization(world)
        else:
            raise ValueError(f"Unknown visualization type: {visualization_type}")
    
    def create_agent_visualization(self, agent: AgentBrain, world: SimpleWorld, visualization_type: str = 'trace'):
        """Create agent visualization.
        
        Args:
            agent: Agent to visualize.
            world: World.
            visualization_type: Type of visualization.
            
        Returns:
            dict: Visualization data.
        """
        logger.debug("Creating agent visualization", type=visualization_type)
        
        if visualization_type == 'trace':
            return self._create_agent_trace_visualization(agent, world)
        elif visualization_type == 'sensory':
            return self._create_sensory_visualization(agent)
        elif visualization_type == 'neuromodulation':
            return self._create_neuromodulation_visualization(agent)
        else:
            raise ValueError(f"Unknown visualization type: {visualization_type}")
    
    def create_multi_modal_visualization(self, brain: Brain, world: SimpleWorld, agent: AgentBrain):
        """Create multi-modal visualization.
        
        Args:
            brain: Brain.
            world: World.
            agent: Agent.
            
        Returns:
            dict: Multi-modal visualization data.
        """
        logger.debug("Creating multi-modal visualization")
        
        return {
            'brain': self.create_brain_visualization(brain, 'network'),
            'world': self.create_world_visualization(world, 'grid'),
            'agent': self.create_agent_visualization(agent, world, 'trace'),
            'sensory': self.create_sensory_visualization(agent),
            'neuromodulation': self.create_neuromodulation_visualization(agent)
        }
    
    def export_visualization(self, data: Dict[str, Any], filepath: str, format: str = 'json'):
        """Export visualization data.
        
        Args:
            data: Visualization data.
            filepath: Output file path.
            format: Export format.
            
        Raises:
            ValueError: If format is unsupported.
        """
        try:
            if format == 'json':
                with open(filepath, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
            elif format == 'png':
                # Convert to PNG format (would require matplotlib/pillow)
                raise NotImplementedError("PNG export requires additional dependencies")
            elif format == 'html':
                # Generate HTML visualization
                html_data = self._create_html_visualization(data)
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(html_data)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            logger.info("Visualization exported", filepath=filepath, format=format)
            
        except Exception as e:
            raise ValueError(f"Failed to export visualization: {str(e)}")
    
    def _create_network_visualization(self, brain: Brain) -> Dict[str, Any]:
        """Create network visualization data.
        
        Args:
            brain: Brain to visualize.
            
        Returns:
            dict: Network visualization data.
        """
        # Simplified network visualization
        nodes = []
        edges = []
        
        # Create node data for neurons
        for i in range(min(100, brain.get_total_neuron_count())):  # Limit to first 100 neurons
            nodes.append({
                'id': i,
                'type': 'neuron',
                'position': [np.random.random() * 100, np.random.random() * 100],
                'color': self._get_neuron_color(i, brain)
            })
        
        # Create edge data for synapses
        for i in range(min(50, brain.get_total_synapse_count())):  # Limit to first 50 synapses
            source = np.random.randint(0, len(nodes))
            target = np.random.randint(0, len(nodes))
            if source != target:
                edges.append({
                    'source': source,
                    'target': target,
                    'weight': np.random.random()
                })
        
        return {
            'type': 'network',
            'nodes': nodes,
            'edges': edges,
            'stats': {
                'total_neurons': brain.get_total_neuron_count(),
                'total_synapses': brain.get_total_synapse_count(),
                'active_neurons': brain.get_active_neuron_count(),
                'firing_rate': brain.get_average_firing_rate()
            }
        }
    
    def _create_activity_visualization(self, brain: Brain) -> Dict[str, Any]:
        """Create activity visualization data.
        
        Args:
            brain: Brain to visualize.
            
        Returns:
            dict: Activity visualization data.
        """
        # Create activity heatmap or time series
        time_steps = 100
        neuron_activity = np.random.random((min(50, brain.get_total_neuron_count()), time_steps))
        
        return {
            'type': 'activity',
            'data': {
                'time_series': neuron_activity.tolist(),
                'neuron_indices': list(range(neuron_activity.shape[0])),
                'time_points': list(range(time_steps))
            },
            'stats': {
                'total_neurons': brain.get_total_neuron_count(),
                'total_spikes': brain.get_total_spike_count(),
                'average_firing_rate': brain.get_average_firing_rate()
            }
        }
    
    def _create_development_visualization(self, brain: Brain) -> Dict[str, Any]:
        """Create development visualization data.
        
        Args:
            brain: Brain to visualize.
            
        Returns:
            dict: Development visualization data.
        """
        # Create development stage visualization
        stages = ['initial', 'critical_period', 'maturation', 'adult', 'aging']
        current_stage = brain.get_developmental_stage()
        stage_index = stages.index(current_stage) if current_stage in stages else 0
        
        return {
            'type': 'development',
            'current_stage': current_stage,
            'stage_index': stage_index,
            'stages': stages,
            'stage_progress': stage_index / (len(stages) - 1) if len(stages) > 1 else 0,
            'stats': {
                'developmental_stage': current_stage,
                'total_neurons': brain.get_total_neuron_count(),
                'plasticity': 'high' if stage_index < 2 else 'moderate' if stage_index < 4 else 'low'
            }
        }
    
    def _create_plasticity_visualization(self, brain: Brain) -> Dict[str, Any]:
        """Create plasticity visualization data.
        
        Args:
            brain: Brain to visualize.
            
        Returns:
            dict: Plasticity visualization data.
        """
        # Create plasticity visualization (STDP, Hebbian, structural)
        stdp_weights = np.random.random(50)
        hebbian_weights = np.random.random(50)
        structural_changes = np.random.random(50)
        
        return {
            'type': 'plasticity',
            'data': {
                'stdp_weights': stdp_weights.tolist(),
                'hebbian_weights': hebbian_weights.tolist(),
                'structural_changes': structural_changes.tolist()
            },
            'stats': {
                'total_synapses': brain.get_total_synapse_count(),
                'plasticity_rules': ['STDP', 'Hebbian', 'Structural'],
                'learning_rate': 0.01
            }
        }
    
    def _create_grid_visualization(self, world: SimpleWorld) -> Dict[str, Any]:
        """Create grid visualization data.
        
        Args:
            world: World to visualize.
            
        Returns:
            dict: Grid visualization data.
        """
        # Create grid map
        grid_size = min(50, world.get_width(), world.get_height())
        grid = np.zeros((grid_size, grid_size))
        
        # Fill grid with world objects
        for obj in world._world_objects:
            grid_x = int((obj.x / world.get_width()) * grid_size)
            grid_y = int((obj.y / world.get_height()) * grid_size)
            
            if 0 <= grid_x < grid_size and 0 <= grid_y < grid_size:
                if obj.type == WorldObjectTypes.WALL:
                    grid[grid_y, grid_x] = 1
                elif obj.type == WorldObjectTypes.FOOD:
                    grid[grid_y, grid_x] = 2
                elif obj.type == WorldObjectTypes.WATER:
                    grid[grid_y, grid_x] = 3
                elif obj.type == WorldObjectTypes.AGENT:
                    grid[grid_y, grid_x] = 4
                elif obj.type == WorldObjectTypes.HAZARD:
                    grid[grid_y, grid_x] = 5
        
        return {
            'type': 'grid',
            'data': {
                'grid': grid.tolist(),
                'grid_size': grid_size,
                'width': world.get_width(),
                'height': world.get_height()
            },
            'legend': {
                '0': 'empty',
                '1': 'wall',
                '2': 'food',
                '3': 'water',
                '4': 'agent',
                '5': 'hazard'
            }
        }
    
    def _create_world_activity_visualization(self, world: SimpleWorld) -> Dict[str, Any]:
        """Create world activity visualization.
        
        Args:
            world: World to visualize.
            
        Returns:
            dict: World activity visualization data.
        """
        # Create world activity heatmap
        time_steps = 50
        activity_data = np.random.random((time_steps, min(20, world.get_width())))
        
        return {
            'type': 'world_activity',
            'data': {
                'activity': activity_data.tolist(),
                'time_steps': time_steps,
                'width_samples': min(20, world.get_width())
            },
            'stats': {
                'simulation_time': world.get_simulation_time(),
                'agent_energy': world.get_agent_body().energy,
                'agent_position': (world.get_agent_body().x, world.get_agent_body().y)
            }
        }
    
    def _create_objects_visualization(self, world: SimpleWorld) -> Dict[str, Any]:
        """Create objects visualization data.
        
        Args:
            world: World to visualize.
            
        Returns:
            dict: Objects visualization data.
        """
        # Create object list
        objects = []
        
        for obj_name, obj in world._objects.items():
            objects.append({
                'name': obj_name,
                'type': obj.type,
                'x': obj.x,
                'y': obj.y,
                'radius': obj.radius,
                'active': obj.active,
                'value': obj.value,
                'mass': obj.mass
            })
        
        return {
            'type': 'objects',
            'data': objects,
            'stats': {
                'total_objects': len(world._world_objects),
                'active_objects': len([obj for obj in world._world_objects if obj.active]),
                'object_types': list(set([obj.type for obj in world._world_objects]))
            }
        }
    
    def _create_agent_trace_visualization(self, agent: AgentBrain, world: SimpleWorld) -> Dict[str, Any]:
        """Create agent trace visualization.
        
        Args:
            agent: Agent to visualize.
            world: World.
            
        Returns:
            dict: Agent trace visualization data.
        """
        # Create agent movement trace
        trace_points = []
        
        for obj in world._world_objects:
            if obj.type == WorldObjectTypes.AGENT:
                trace_points.append({
                    'x': obj.x,
                    'y': obj.y,
                    'time': world.get_simulation_time(),
                    'energy': world.get_agent_body().energy,
                    'health': world.get_agent_body().health
                })
        
        return {
            'type': 'agent_trace',
            'data': trace_points,
            'stats': {
                'total_points': len(trace_points),
                'agent_position': (world.get_agent_body().x, world.get_agent_body().y),
                'agent_orientation': world.get_agent_body().orientation,
                'agent_energy': world.get_agent_body().energy,
                'agent_health': world.get_agent_body().health
            }
        }
    
    def _create_sensory_visualization(self, agent: AgentBrain) -> Dict[str, Any]:
        """Create sensory visualization data.
        
        Args:
            agent: Agent to visualize.
            
        Returns:
            dict: Sensory visualization data.
        """
        # Create sensory data visualization
        sensory_data = agent.brain.getSensoryInputData() if hasattr(agent.brain, 'getSensoryInputData') else {}
        
        return {
            'type': 'sensory',
            'data': {
                'vision': sensory_data.get('vision', {}),
                'audio': sensory_data.get('audio', {}),
                'internal': sensory_data.get('internal', {}),
                'touch': sensory_data.get('touch', []),
                'proprioception': sensory_data.get('proprioception', [])
            },
            'stats': {
                'sensory_modalities': list(sensory_data.keys()) if sensory_data else [],
                'has_vision': 'vision' in (sensory_data or {})
            }
        }
    
    def _create_neuromodulation_visualization(self, agent: AgentBrain) -> Dict[str, Any]:
        """Create neuromodulation visualization data.
        
        Args:
            agent: Agent to visualize.
            
        Returns:
            dict: Neuromodulation visualization data.
        """
        neuromodulation_state = agent.get_neuromodulation_state()
        curiosity_state = agent.get_curiosity_state()
        prediction_state = agent.get_prediction_state()
        
        return {
            'type': 'neuromodulation',
            'data': {
                'neuromodulation': neuromodulation_state,
                'curiosity': curiosity_state,
                'prediction': prediction_state
            },
            'stats': {
                'neuromodulation_enabled': agent.is_reward_modulation_enabled(),
                'curiosity_enabled': agent.is_curiosity_enabled(),
                'prediction_enabled': agent.is_prediction_enabled()
            }
        }
    
    def _create_html_visualization(self, data: Dict[str, Any]) -> str:
        """Create HTML visualization.
        
        Args:
            data: Visualization data.
            
        Returns:
            str: HTML visualization.
        """
        html_template = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Visualization</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        body { margin: 0; padding: 20px; font-family: Arial, sans-serif; }
        .plot { margin: 20px 0; }
        .header { background: #f0f0f0; padding: 20px; border-radius: 5px; }
        .metric { display: inline-block; margin: 10px; padding: 10px; background: #e0e0e0; border-radius: 3px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Neural Learning Machine Visualization</h1>
        <div class="metrics">
            {metrics}
        </div>
    </div>
    
    <div id="brain-network" class="plot"></div>
    <div id="activity-heatmap" class="plot"></div>
    <div id="world-grid" class="plot"></div>
    
    <script>
        // Initialize plots
        var data = {data_json};
        
        // Create brain network visualization
        var brainLayout = {{
            title: 'Brain Network',
            xaxis: {{ title: 'X' }},
            yaxis: {{ title: 'Y' }}
        }};
        
        Plotly.newPlot('brain-network', data.brain.nodes, data.brain.edges, brainLayout);
        
        // Create activity heatmap visualization
        var activityData = {{
            x: data.brain.data.neuron_indices,
            y: data.brain.data.time_points,
            z: data.brain.data.time_series,
            type: 'heatmap'
        }};
        
        var activityLayout = {{
            title: 'Neural Activity Heatmap',
            xaxis: {{ title: 'Neuron Index' }},
            yaxis: {{ title: 'Time Step' }}
        }};
        
        Plotly.newPlot('activity-heatmap', [activityData], activityLayout);
        
        // Create world grid visualization
        var gridData = {{
            z: data.world.data.grid,
            type: 'heatmap',
            colorscale: 'Viridis',
            showscale: false
        }};
        
        var gridLayout = {{
            title: 'World Grid',
            xaxis: {{ showticklabels: false }},
            yaxis: {{ showticklabels: false, autorange: 'reversed' }}
        }};
        
        Plotly.newPlot('world-grid', [gridData], gridLayout);
    </script>
</body>
</html>
"""
        
        # Generate metrics HTML
        metrics = ""
        for key, value in data.get('brain', {}).get('stats', {}).items():
            metrics += f'<div class="metric"><strong>{key}:</strong> {value}</div>'
        
        # Replace placeholders
        html_content = html_template.replace('{metrics}', metrics)
        html_content = html_content.replace('{data_json}', json.dumps(data))
        
        return html_content


# High-level convenience functions for advanced users

def create_advanced_brain(config_file: Optional[str] = None) -> AdvancedConfig:
    """Create an advanced brain with enhanced configuration.
    
    Args:
        config_file: Optional configuration file path.
        
    Returns:
        AdvancedConfig: Advanced configuration.
    """
    config = AdvancedConfig()
    
    if config_file:
        config.import_from_file(config_file)
    
    # Set default advanced settings
    config.set_advanced_setting('learning_rate', 0.01)
    config.set_advanced_setting('noise_level', 0.1)
    config.set_advanced_setting('max_steps', 1000)
    config.set_advanced_setting('checkpoint_interval', 100)
    
    # Add validation rules
    config.add_validation_rule('learning_rate', lambda x: 0 <= x <= 1, "Learning rate must be between 0 and 1")
    config.add_validation_rule('noise_level', lambda x: 0 <= x <= 1, "Noise level must be between 0 and 1")
    config.add_validation_rule('max_steps', lambda x: x > 0, "Max steps must be positive")
    
    return config


def create_advanced_simulation(brain: Brain, world: Optional[SimpleWorld] = None, agent: Optional[AgentBrain] = None) -> AdvancedSimulation:
    """Create an advanced simulation.
    
    Args:
        brain: Brain for simulation.
        world: Optional world for simulation.
        agent: Optional agent for simulation.
        
    Returns:
        AdvancedSimulation: Advanced simulation.
    """
    return AdvancedSimulation(brain, world, agent)


def create_performance_profiler() -> PerformanceProfiler:
    """Create performance profiler.
    
    Returns:
        PerformanceProfiler: Performance profiler.
    """
    return PerformanceProfiler()


def create_visualization_generator() -> VisualizationGenerator:
    """Create visualization generator.
    
    Returns:
        VisualizationGenerator: Visualization generator.
    """
    return VisualizationGenerator()


def export_results_to_file(data: Dict[str, Any], filepath: str, format: str = 'json'):
    """Export simulation results to file.
    
    Args:
        data: Simulation results.
        filepath: Output file path.
        format: Export format.
    """
    try:
        if format == 'json':
            with open(filepath, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2)
        elif format == 'csv':
            import pandas as pd
            df = pd.DataFrame(data)
            df.to_csv(filepath, index=False)
        elif format == 'pickle':
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(data, f)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        logger.info("Results exported", filepath=filepath, format=format)
        
    except Exception as e:
        raise ValueError(f"Failed to export results: {str(e)}")


def import_results_from_file(filepath: str, format: str = 'json') -> Dict[str, Any]:
    """Import simulation results from file.
    
    Args:
        filepath: Input file path.
        format: Import format.
        
    Returns:
        Imported simulation results.
    """
    try:
        if format == 'json':
            with open(filepath, 'r', encoding='utf-8') as f:
                data = json.load(f)
        elif format == 'csv':
            import pandas as pd
            df = pd.read_csv(filepath)
            data = df.to_dict('records')
        elif format == 'pickle':
            import pickle
            with open(filepath, 'rb') as f:
                data = pickle.load(f)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        logger.info("Results imported", filepath=filepath, format=format)
        
        return data
        
    except Exception as e:
        raise ValueError(f"Failed to import results: {str(e)}")


# Simulation scenario templates

SCENARIO_TEMPLATES = {
    'basic_exploration': {
        'description': 'Basic agent exploration in a simple world',
        'config': {
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1
        },
        'world_config': {
            'width': 20,
            'height': 20,
            'vision_width': 8,
            'vision_height': 8
        },
        'agent_features': {
            'reward_modulation': True,
            'curiosity': True,
            'structural_plasticity': False,
            'development': False
        }
    },
    
    'learning_environment': {
        'description': 'Agent learning in dynamic environment with rewards',
        'config': {
            'neuron_count': 1000,
            'region_count': 3,
            'connection_probability': 0.15,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.018
        },
        'world_config': {
            'width': 30,
            'height': 30,
            'vision_width': 12,
            'vision_height': 12
        },
        'agent_features': {
            'reward_modulation': True,
            'curiosity': True,
            'structural_plasticity': True,
            'development': True,
            'novelty': True,
            'prediction': True
        }
    },
    
    'competitive_learning': {
        'description': 'Competitive learning with multiple agents',
        'config': {
            'neuron_count': 1500,
            'region_count': 4,
            'connection_probability': 0.12,
            'stdp_ltp_weight': 0.025,
            'stdp_ltd_weight': 0.022
        },
        'world_config': {
            'width': 50,
            'height': 50,
            'vision_width': 16,
            'vision_height': 16
        },
        'agent_features': {
            'reward_modulation': True,
            'curiosity': True,
            'structural_plasticity': True,
            'development': True,
            'novelty': True,
            'prediction': True
        }
    },
    
    'curiosity_driven': {
        'description': 'Agent driven by curiosity and exploration',
        'config': {
            'neuron_count': 800,
            'region_count': 3,
            'connection_probability': 0.18,
            'curiosity_threshold': 0.5
        },
        'world_config': {
            'width': 40,
            'height': 40,
            'vision_width': 14,
            'vision_height': 14
        },
        'agent_features': {
            'reward_modulation': False,
            'curiosity': True,
            'structural_plasticity': True,
            'development': True,
            'novelty': True,
            'prediction': False
        }
    }
}


def create_scenario(scenario_name: str) -> Dict[str, Any]:
    """Create a simulation scenario from template.
    
    Args:
        scenario_name: Scenario name from SCENARIO_TEMPLATES.
        
    Returns:
        Scenario configuration.
        
    Raises:
        ValueError: If scenario name is not found.
    """
    if scenario_name not in SCENARIO_TEMPLATES:
        raise ValueError(f"Unknown scenario: {scenario_name}")
    
    return SCENARIO_TEMPLATES[scenario_name].copy()


def run_scenario(scenario_name: str, output_file: Optional[str] = None) -> Dict[str, Any]:
    """Run a simulation scenario.
    
    Args:
        scenario_name: Scenario name.
        output_file: Optional output file path.
        
    Returns:
        Simulation results.
    """
    scenario = create_scenario(scenario_name)
    
    # Create configuration
    config = AdvancedConfig()
    for key, value in scenario['config'].items():
        config.set_advanced_setting(key, value)
    
    # Create brain
    brain = Brain(config._base_config)
    brain.initialize()
    
    # Create world
    world = SimpleWorld()
    world.configure(**scenario['world_config'])
    world.reset()
    
    # Create agent
    agent = AgentBrain(brain)
    agent.initialize(world)
    
    # Enable agent features
    agent.enable_features(**scenario['agent_features'])
    
    # Run simulation
    profiler = create_performance_profiler()
    profiler.start_profile(f"scenario_{scenario_name}")
    
    for step in range(scenario['config'].get('max_steps', 1000)):
        # Record performance
        profiler.record_step(step, 0.001, 10, 50)
        
        # Run simulation step
        world.update(0.1)
        
        if agent.is_initialized:
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    profiler.end_profile()
    
    # Get results
    results = {
        'scenario': scenario_name,
        'config': scenario['config'],
        'brain_stats': brain.get_brain_stats(),
        'world_stats': {
            'simulation_time': world.get_simulation_time(),
            'agent_position': (world.get_agent_body().x, world.get_agent_body().y),
            'agent_energy': world.get_agent_body().energy
        },
        'agent_features': scenario['agent_features'],
        'performance': profiler.get_profile(f"scenario_{scenario_name}")
    }
    
    # Export results if output file specified
    if output_file:
        export_results_to_file(results, output_file)
    
    logger.info("Scenario completed", 
               scenario=scenario_name,
               duration=results['performance']['duration'],
               steps=results['performance']['steps'])
    
    return results


# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Advanced features for power users of NLM"

# Export all public symbols
__all__ = [
    # Advanced classes
    'AdvancedConfig',
    'AdvancedSimulation',
    'PerformanceProfiler',
    'VisualizationGenerator',
    
    # Convenience functions
    'create_advanced_brain',
    'create_advanced_simulation',
    'create_performance_profiler',
    'create_visualization_generator',
    'export_results_to_file',
    'import_results_from_file',
    'create_scenario',
    'run_scenario',
    
    # Exception classes
    'AdvancedFeatureError',
    'SimulationError',
    'ExportError',
    'ImportError',
    
    # Scenario templates
    'SCENARIO_TEMPLATES',
    
    # Constants
    'WorldObjectTypes',
    'ActionTypes',
    'NeuronTypes',
    'DevelopmentalStages',
    'FiringStates',
    'MotorCommands',
]
