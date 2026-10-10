"""Enhanced Python interface for NLM with type hints and improved usability.

This module provides a more Pythonic interface to the NLM C++ neural simulation framework
with comprehensive type hints, error handling, and convenience methods.
"""

import warnings
from typing import Optional, List, Dict, Any, Union
import numpy as np

# Import the base pynlm module
import pynlm

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Type aliases for better readability
NeuronId = pynlm.NeuronId
SynapseId = pynlm.SynapseId
RegionId = pynlm.RegionId
PopulationId = pynlm.PopulationId
NeuronType = pynlm.NeuronType
SynapseType = pynlm.SynapseType
ActionType = pynlm.ActionType
WorldObjectType = pynlm.WorldObjectType
DevelopmentalStage = pynlm.DevelopmentalStage
Action = pynlm.Action
WorldObject = pynlm.WorldObject
AgentBody = pynlm.AgentBody
ActionResult = pynlm.ActionResult
SensoryPercept = pynlm.SensoryPercept

class EnhancedBrain(pynlm.Brain):
    """Enhanced brain interface with additional utility methods and type hints."""
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """Initialize enhanced brain with optional configuration.
        
        Args:
            config: Brain configuration. If None, creates default config.
        """
        if config is None:
            config = pynlm.createDefaultConfig()
        super().__init__(config)
    
    def run_simulation(self, num_steps: int, timestep: Optional[int] = None) -> Dict[str, Any]:
        """Run simulation with comprehensive statistics collection.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Optional timestep value for each step
            
        Returns:
            Dictionary containing simulation statistics
        """
        if num_steps < 0:
            raise ValueError("Number of steps must be non-negative")
        
        self.initialize()
        stats = {
            'total_steps': num_steps,
            'final_spike_count': 0,
            'max_firing_rate': 0.0,
            'avg_firing_rate': 0.0,
            'active_neurons': 0,
            'firing_neuron_count': 0,
            'total_synapse_count': 0,
            'excitatory_inhibitory_ratio': 0.0,
            'development_stage': None,
            'step_history': []
        }
        
        for step in range(num_steps):
            if timestep is not None:
                self.step(step, timestep)
            else:
                self.step(step)
            
            # Collect statistics at each step (every 10 steps to avoid performance impact)
            if step % 10 == 0 or step == num_steps - 1:
                step_stats = {
                    'step': step,
                    'firing_neuron_count': self.getFiringNeuronCount(),
                    'total_spike_count': self.getTotalSpikeCount(),
                    'avg_firing_rate': self.getAverageFiringRate(),
                    'active_neurons': self.getActiveNeuronCount(),
                    'total_synapse_count': self.getTotalSynapseCount(),
                    'e_i_ratio': self.getExcitationInhibitionRatio(),
                    'development_stage': self.getDevelopmentalStage()
                }
                stats['step_history'].append(step_stats)
        
        # Update final statistics
        stats['final_spike_count'] = self.getTotalSpikeCount()
        stats['max_firing_rate'] = max(s['avg_firing_rate'] for s in stats['step_history']) if stats['step_history'] else 0.0
        stats['avg_firing_rate'] = (self.getAverageFiringRate() if num_steps > 0 else 0.0)
        stats['active_neurons'] = self.getActiveNeuronCount()
        stats['firing_neuron_count'] = self.getFiringNeuronCount()
        stats['total_synapse_count'] = self.getTotalSynapseCount()
        stats['excitatory_inhibitory_ratio'] = self.getExcitationInhibitionRatio()
        stats['development_stage'] = self.getDevelopmentalStage()
        
        return stats
    
    def get_detailed_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return {
            'region_count': self.getRegionCount(),
            'total_neuron_count': self.getTotalNeuronCount(),
            'total_synapse_count': self.getTotalSynapseCount(),
            'firing_neuron_count': self.getFiringNeuronCount(),
            'active_neuron_count': self.getActiveNeuronCount(),
            'firing_rate': self.getAverageFiringRate(),
            'e_i_ratio': self.getExcitationInhibitionRatio(),
            'total_spike_count': self.getTotalSpikeCount(),
            'pending_spike_count': self.getPendingSpikeEventCount(),
            'development_stage': self.getDevelopmentalStage(),
            'region_ids': self.getRegionIds(),
            'regions': [self.getRegion(rid) for rid in self.getRegionIds()]
        }
    
    def add_regions(self, region_names: List[str]) -> List[RegionId]:
        """Add multiple regions at once.
        
        Args:
            region_names: List of region names to add
            
        Returns:
            List of created region IDs
        """
        region_ids = []
        for name in region_names:
            region_id = self.addRegion(name)
            region_ids.append(region_id)
        return region_ids
    
    def save_checkpoint(self, filepath: str, include_metadata: bool = True) -> bool:
        """Save brain state with optional metadata.
        
        Args:
            filepath: File path to save brain state
            include_metadata: Whether to include metadata in the saved file
            
        Returns:
            True if save successful, False otherwise
        """
        try:
            self.save(filepath)
            if include_metadata:
                # Save metadata as separate file
                import json
                metadata = {
                    'version': __version__,
                    'timestamp': self.getSimulationTime(),
                    'region_count': self.getRegionCount(),
                    'neuron_count': self.getTotalNeuronCount()
                }
                metadata_path = filepath.replace('.bin', '_metadata.json')
                with open(metadata_path, 'w') as f:
                    json.dump(metadata, f, indent=2)
            return True
        except Exception as e:
            warnings.warn(f"Failed to save checkpoint: {e}")
            return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state with metadata validation.
        
        Args:
            filepath: File path to load brain state
            
        Returns:
            True if load successful, False otherwise
        """
        try:
            self.load(filepath)
            # Load metadata if available
            import json
            metadata_path = filepath.replace('.bin', '_metadata.json')
            import os
            if os.path.exists(metadata_path):
                with open(metadata_path, 'r') as f:
                    metadata = json.load(f)
                if metadata.get('version') != __version__:
                    warnings.warn(f"Metadata version mismatch: expected {__version__}, got {metadata.get('version')}")
            return True
        except Exception as e:
            warnings.warn(f"Failed to load checkpoint: {e}")
            return False
class EnhancedAgent(pynlm.AgentBrain):
    """Enhanced agent interface with additional functionality and type safety."""
    
    def __init__(self, brain: EnhancedBrain):
        """Initialize enhanced agent with brain.
        
        Args:
            brain: EnhancedBrain instance to control
        """
        super().__init__(brain)
        self.world: Optional[pynlm.SimpleWorld] = None
        self.simulation_history: List[Dict[str, Any]] = []
    
    def initialize_with_world(self, world: pynlm.SimpleWorld) -> None:
        """Initialize agent with world and enable default subsystems.
        
        Args:
            world: SimpleWorld instance to interact with
        """
        self.world = world
        self.initialize(world)
        # Enable default learning and development capabilities
        self.enableRewardModulation(True)
        self.enableStructuralPlasticity(False)  # Can be enabled selectively
        self.enableDevelopment(True)
        self.enableCuriosity(True)
    
    def run_episode(self, max_steps: int, verbose: bool = False) -> Dict[str, Any]:
        """Run complete agent episode with comprehensive tracking.
        
        Args:
            max_steps: Maximum number of steps for this episode
            verbose: Whether to print detailed progress
            
        Returns:
            Dictionary containing episode results and statistics
        """
        if self.world is None:
            raise RuntimeError("Agent must be initialized with a world before running episodes")
        
        episode_stats = {
            'steps_taken': 0,
            'total_reward': 0.0,
            'max_reward': 0.0,
            'min_reward': 0.0,
            'actions_taken': 0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'prediction_error': 0.0,
            'neuromodulation_level': 0.0,
            'development_stage': None,
            'action_distribution': {},
            'step_details': []
        }
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.processSensoryInput(percept)
            
            # Get brain stats
            brain = self.getBrain()
            if verbose and step % 10 == 0:
                print(f"Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
                      f"Reward={self.getCuriosityLevel():.3f}, Novelty={self.getNoveltyLevel():.3f}")
            
            # Run brain step
            brain.step(step)
            
            # Get action from brain
            action = self.decodeMotorCommand()
            episode_stats['actions_taken'] += 1
            action_name = action.getName() if action else 'None'
            episode_stats['action_distribution'][action_name] = episode_stats['action_distribution'].get(action_name, 0) + 1
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept and percept.getInternal():
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            
            self.applyRewardModulation(reward, 0.0)
            episode_stats['total_reward'] += reward
            episode_stats['max_reward'] = max(episode_stats['max_reward'], reward)
            episode_stats['min_reward'] = min(episode_stats['min_reward'], reward) if episode_stats['steps_taken'] > 0 else reward
            
            # Update development
            self.updateDevelopment(0.1)
            
            # Collect step details
            step_detail = {
                'step': step,
                'reward': reward,
                'curiosity': self.getCuriosityLevel(),
                'novelty': self.getNoveltyLevel(),
                'prediction_error': self.getPredictionError(),
                'neuromodulation': self.getNeuromodulationLevel(),
                'development_stage': brain.getDevelopmentalStage(),
                'action': action_name,
                'firing_neuron_count': brain.getFiringNeuronCount()
            }
            episode_stats['step_details'].append(step_detail)
            
            # Update running statistics
            episode_stats['curiosity_level'] = self.getCuriosityLevel()
            episode_stats['novelty_level'] = self.getNoveltyLevel()
            episode_stats['prediction_error'] = self.getPredictionError()
            episode_stats['neuromodulation_level'] = self.getNeuromodulationLevel()
            episode_stats['development_stage'] = brain.getDevelopmentalStage()
            episode_stats['steps_taken'] = step + 1
            
            # Optional: Add termination conditions (e.g., life cycle, goals, etc.)
            # For now, we'll just complete all steps
        
        return episode_stats
    
    def set_behavior_policies(self, reward_modulation: bool = True, 
                            structural_plasticity: bool = False,
                            development: bool = True, curiosity: bool = True) -> None:
        """Set behavior policies for the agent.
        
        Args:
            reward_modulation: Enable reward-based learning
            structural_plasticity: Enable structural plasticity (grow new connections)
            development: Enable developmental stages and maturation
            curiosity: Enable curiosity-driven exploration
        """
        self.enableRewardModulation(reward_modulation)
        self.enableStructuralPlasticity(structural_plasticity)
        self.enableDevelopment(development)
        self.enableCuriosity(curiosity)
    
    def get_behavior_summary(self) -> Dict[str, Any]:
        """Get summary of current agent behavior settings."""
        return {
            'reward_modulation_enabled': self.isRewardModulationEnabled(),
            'structural_plasticity_enabled': self.isStructuralPlasticityEnabled(),
            'development_enabled': self.isDevelopmentEnabled(),
            'curiosity_enabled': self.isCuriosityEnabled(),
            'brain_stats': {
                'curiosity_level': self.getCuriosityLevel(),
                'novelty_level': self.getNoveltyLevel(),
                'prediction_error': self.getPredictionError(),
                'neuromodulation_level': self.getNeuromodulationLevel()
            }
        }
# Convenience functions with type hints and error handling
def create_enhanced_brain(config: Optional[pynlm.Config] = None) -> EnhancedBrain:
    """Create an enhanced brain with optional configuration.
    
    Args:
        config: Brain configuration. If None, creates default config.
        
    Returns:
        EnhancedBrain instance
    """
    return EnhancedBrain(config)
def create_enhanced_agent(brain: EnhancedBrain) -> EnhancedAgent:
    """Create an enhanced agent with the given brain.
    
    Args:
        brain: EnhancedBrain instance to control
        
    Returns:
        EnhancedAgent instance
    """
    return EnhancedAgent(brain)
def validate_brain_config(config: pynlm.Config) -> bool:
    """Validate brain configuration with comprehensive checks.
    
    Args:
        config: Configuration to validate
        
    Returns:
        True if configuration is valid, False otherwise
    """
    if not config:
        return False
    
    required_keys = ['neuron_count', 'region_count', 'connection_probability']
    for key in required_keys:
        if not config.has(key):
            warnings.warn(f"Missing required config key: {key}")
            return False
    
    neuron_count = config.get('neuron_count', 0)
    if neuron_count <= 0:
        warnings.warn("Neuron count must be positive")
        return False
    
    region_count = config.get('region_count', 0)
    if region_count < 1:
        warnings.warn("Region count must be at least 1")
        return False
    
    connection_prob = config.get('connection_probability', 0.0)
    if not (0.0 <= connection_prob <= 1.0):
        warnings.warn("Connection probability must be between 0 and 1")
        return False
    
    return True
def get_system_info() -> Dict[str, Any]:
    """Get comprehensive system information.
    
    Returns:
        Dictionary containing system information
    """
    return {
        'version': __version__,
        'description': __description__,
        'author': __author__,
        'python_bindings': {
            'module': 'pynlm',
            'version': pynlm.__version__ if hasattr(pynlm, '__version__') else 'unknown'
        },
        'capabilities': [
            'enhanced_brain_interface',
            'enhanced_agent_interface',
            'simulation_running',
            'checkpoint_management',
            'behavior_control',
            'comprehensive_statistics',
            'configuration_validation'
        ],
        'performance_features': [
            'type_hints',
            'error_handling',
            'convenience_methods',
            'simulation_tracking',
            'behavior_policies'
        ]
    }
def run_simple_simulation(num_steps: int = 100, timestep: Optional[int] = None) -> Dict[str, Any]:
    """Run a simple simulation with default settings.
    
    Args:
        num_steps: Number of simulation steps
        timestep: Optional timestep value
        
    Returns:
        Dictionary containing simulation results
    """
    brain = create_enhanced_brain()
    return brain.run_simulation(num_steps, timestep)
# Performance monitoring utilities
class PerformanceMonitor:
    """Monitor and track NLM performance metrics."""
    
    def __init__(self):
        self.metrics = {
            'simulation_time': [],
            'memory_usage': [],
            'cpu_usage': [],
            'step_times': [],
            'neuron_activity': []
        }
    
    def record_simulation(self, brain: EnhancedBrain, steps: int) -> None:
        """Record performance metrics for a simulation run.
        
        Args:
            brain: EnhancedBrain instance that was simulated
            steps: Number of steps simulated
        """
        import time
        
        start_time = time.time()
        brain.run_simulation(steps)
        end_time = time.time()
        
        simulation_time = end_time - start_time
        self.metrics['simulation_time'].append({
            'steps': steps,
            'time_seconds': simulation_time,
            'steps_per_second': steps / simulation_time if simulation_time > 0 else 0,
            'timestamp': start_time
        })
        
        # Record neuron activity
        self.metrics['neuron_activity'].append({
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        })
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self.metrics['simulation_time']:
            return {'message': 'No performance data available'}
        
        simulation_times = [m['time_seconds'] for m in self.metrics['simulation_time']]
        steps_per_second = [m['steps_per_second'] for m in self.metrics['simulation_time']]
        
        return {
            'total_simulations': len(simulation_times),
            'avg_simulation_time': sum(simulation_times) / len(simulation_times),
            'avg_steps_per_second': sum(steps_per_second) / len(steps_per_second),
            'min_simulation_time': min(simulation_times),
            'max_simulation_time': max(simulation_times),
            'total_steps_simulated': sum(m['steps'] for m in self.metrics['simulation_time']),
            'avg_firing_rate': sum(m['avg_firing_rate'] for m in self.metrics['neuron_activity']) / len(self.metrics['neuron_activity']) if self.metrics['neuron_activity'] else 0
        }
    
    def reset(self) -> None:
        """Reset all performance metrics."""
        self.metrics = {
            'simulation_time': [],
            'memory_usage': [],
            'cpu_usage': [],
            'step_times': [],
            'neuron_activity': []
        }
# Create a global performance monitor instance
performance_monitor = PerformanceMonitor()

# Create enhanced convenience aliases for common patterns
EnhancedBrain.brain = property(lambda self: self)
EnhancedAgent.agent = property(lambda self: self)

# Module-level convenience functions
__all__ = [
    'EnhancedBrain',
    'EnhancedAgent', 
    'create_enhanced_brain',
    'create_enhanced_agent',
    'validate_brain_config',
    'get_system_info',
    'run_simple_simulation',
    'PerformanceMonitor',
    'performance_monitor',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'ActionType',
    'WorldObjectType',
    'DevelopmentalStage',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept'
]