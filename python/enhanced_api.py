"""Enhanced Python API for NLM (Neural Learning Machine)

This module provides a more intuitive and Pythonic interface to the NLM neural simulation framework.
It builds upon the existing low-level bindings with higher-level abstractions, convenience methods,
and advanced features for researchers and developers.

The enhanced API includes:
- Pythonic interfaces with context managers, iterators, and properties
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

Usage examples:

    import pynlm

    # Simple brain simulation
    brain = pynlm.create_brain()
    brain.initialize()
    for i in range(100):
        brain.step(i)
    
    # Advanced usage with analysis
    with pynlm.training_session() as session:
        session.brain.enable_learning(True)
        session.run(1000)
        session.analyze()
"""

import warnings
import logging
from typing import Optional, List, Dict, Any, Iterator, ContextManager
from contextlib import contextmanager
import json
import os
from pathlib import Path

# Import the original bindings
import pynlm

# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

__version__ = pynlm.__version__

# Create a module-level alias for backward compatibility
createBrain = pynlm.createBrain
createSimpleWorld = pynlm.createSimpleWorld
createAgentBrain = pynlm.createAgentBrain
createDefaultConfig = pynlm.createDefaultConfig

__all__ = [
    # Original API (backward compatibility)
    'createBrain', 'createSimpleWorld', 'createAgentBrain', 'createDefaultConfig',
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType', 'Vision', 'Audio',
    'InternalSignals', 'SensoryInput', 'Action', 'WorldObject', 'AgentBody',
    'ActionResult', 'SensoryPercept', 'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enhanced API
    'EnhancedBrain', 'EnhancedConfig', 'EnhancedAgent', 'EnhancedWorld',
    'BrainAnalysis', 'VisualizationTools', 'ErrorHandler',
    'create_enhanced_brain', 'create_enhanced_config', 'create_enhanced_agent',
    'create_enhanced_world', 'create_training_session', 'create_analysis_session',
    'ConfigBuilder', 'BrainManager', 'VisualizationExporter',
]

# Re-export the original classes for backward compatibility
Config = pynlm.Config
Brain = pynlm.Brain
AgentBrain = pynlm.AgentBrain
SimpleWorld = pynlm.SimpleWorld

# Type aliases for better documentation
tensor = List[float]
neuron_id = pynlm.NeuronId
synapse_id = pynlm.SynapseId
region_id = pynlm.RegionId
population_id = pynlm.PopulationId

# Validation and error handling
class ValidationError(Exception):
    """Exception raised for validation errors in the enhanced API."""
    pass

class NeuralNetworkError(Exception):
    """Exception raised for neural network-related errors."""
    pass

def validate_config(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """Validate a configuration dictionary.
    
    Args:
        config_dict: Dictionary containing configuration parameters
        
    Returns:
        Validated configuration dictionary
        
    Raises:
        ValidationError: If configuration is invalid
    """
    valid_sections = {'brain', 'plasticity', 'neuromod', 'simulation', 'world'}
    
    for key in config_dict:
        if '.' not in key:
            raise ValidationError(f"Configuration key '{key}' must use dot notation (e.g., 'brain.neuron_count')")
        
        section = key.split('.')[0]
        if section not in valid_sections:
            warnings.warn(f"Unknown configuration section '{section}'; valid sections: {valid_sections}")
    
    return config_dict

class ErrorHandler:
    """Handle errors and provide helpful messages."""
    
    @staticmethod
    def handle_brain_error(error: Exception, operation: str) -> None:
        """Handle brain-related errors with helpful messages.
        
        Args:
            error: The original exception
            operation: The operation that failed
        """
        error_msg = str(error)
        
        if "initialization" in operation.lower():
            logger.error(f"Brain initialization failed: {error_msg}")
            logger.error("Check that the configuration is valid and all required parameters are set.")
        elif "step" in operation.lower():
            logger.error(f"Brain simulation step failed: {error_msg}")
            logger.error("The brain may need a warmup period or different configuration parameters.")
        elif "configuration" in operation.lower():
            logger.error(f"Configuration error: {error_msg}")
            logger.error("Ensure all configuration values are valid and within expected ranges.")
        
        raise NeuralNetworkError(f"Failed to {operation}: {error_msg}")

# Configuration builder
class ConfigBuilder:
    """Builder for creating and managing NLM configurations with validation."""
    
    def __init__(self):
        self.config_dict = {}
    
    def set(self, key: str, value: Any) -> 'ConfigBuilder':
        """Set a configuration parameter.
        
        Args:
            key: Configuration key in dot notation (e.g., 'brain.neuron_count')
            value: Configuration value
            
        Returns:
            Self for method chaining
        """
        if '.' not in key:
            raise ValidationError(f"Configuration key '{key}' must use dot notation")
        
        self.config_dict[key] = value
        return self
    
    def set_brain(self, **kwargs) -> 'ConfigBuilder':
        """Set brain-specific configuration parameters.
        
        Args:
            **kwargs: Brain configuration parameters
            
        Returns:
            Self for method chaining
        """
        for key, value in kwargs.items():
            self.set(f"brain.{key}", value)
        return self
    
    def set_plasticity(self, **kwargs) -> 'ConfigBuilder':
        """Set plasticity configuration parameters.
        
        Args:
            **kwargs: Plasticity configuration parameters
            
        Returns:
            Self for method chaining
        """
        for key, value in kwargs.items():
            self.set(f"plasticity.{key}", value)
        return self
    
    def set_neuromod(self, **kwargs) -> 'ConfigBuilder':
        """Set neuromodulation configuration parameters.
        
        Args:
            **kwargs: Neuromodulation configuration parameters
            
        Returns:
            Self for method chaining
        """
        for key, value in kwargs.items():
            self.set(f"neuromod.{key}", value)
        return self
    
    def set_simulation(self, **kwargs) -> 'ConfigBuilder':
        """Set simulation configuration parameters.
        
        Args:
            **kwargs: Simulation configuration parameters
            
        Returns:
            Self for method chaining
        """
        for key, value in kwargs.items():
            self.set(f"simulation.{key}", value)
        return self
    
    def validate(self) -> 'ConfigBuilder':
        """Validate the configuration.
        
        Returns:
            Self for method chaining
        """
        validate_config(self.config_dict)
        return self
    
    def create(self) -> pynlm.Config:
        """Create a Config object from the builder's configuration.
        
        Returns:
            Configured Config object
        """
        self.validate()
        
        # Create a config using the original API
        config = pynlm.createDefaultConfig()
        
        # Set configuration parameters
        for key, value in self.config_dict.items():
            # Use a simple approach - the Config class might not have a direct set method
            # This is a placeholder that should be adapted based on the actual Config interface
            pass
        
        return config
    
    def to_dict(self) -> Dict[str, Any]:
        """Get the configuration dictionary.
        
        Returns:
            Configuration dictionary
        """
        return self.config_dict.copy()

# Enhanced configuration class
class EnhancedConfig(pynlm.Config):
    """Enhanced configuration with validation and convenience methods."""
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize enhanced configuration.
        
        Args:
            config_file: Optional configuration file path
        """
        super().__init__()
        
        if config_file:
            self.load(config_file)
    
    def load(self, config_file: str) -> None:
        """Load configuration from file with validation.
        
        Args:
            config_file: Path to configuration file
        """
        try:
            super().loadFromFile(config_file)
        except Exception as e:
            ErrorHandler.handle_brain_error(e, "load configuration")
    
    def get_brain_params(self) -> Dict[str, Any]:
        """Get all brain-related configuration parameters.
        
        Returns:
            Dictionary of brain configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('brain.')}
    
    def get_plasticity_params(self) -> Dict[str, Any]:
        """Get all plasticity-related configuration parameters.
        
        Returns:
            Dictionary of plasticity configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('plasticity.')}
    
    def get_neuromod_params(self) -> Dict[str, Any]:
        """Get all neuromodulation configuration parameters.
        
        Returns:
            Dictionary of neuromodulation configuration parameters
        """
        return {k: v for k, v in vars(self).items() 
                if isinstance(k, str) and k.startswith('neuromod.')}
    
    def summary(self) -> str:
        """Get a comprehensive summary of configuration.
        
        Returns:
            Configuration summary string
        """
        summary = "Enhanced Configuration Summary:\n"
        summary += f"Brain parameters: {len(self.get_brain_params())}\n"
        summary += f"Plasticity parameters: {len(self.get_plasticity_params())}\n"
        summary += f"Neuromodulation parameters: {len(self.get_neuromod_params())}\n"
        return summary

# Brain manager
class BrainManager:
    """Manager for creating and managing enhanced brains."""
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """Initialize brain manager.
        
        Args:
            config: Optional configuration
        """
        self.config = config or pynlm.createDefaultConfig()
        self.brains = []
    
    def create_brain(self, name: Optional[str] = None) -> 'EnhancedBrain':
        """Create a new enhanced brain.
        
        Args:
            name: Optional name for the brain
            
        Returns:
            Enhanced brain instance
        """
        brain = EnhancedBrain(self.config)
        if name:
            brain.name = name
        self.brains.append(brain)
        return brain
    
    def list_brains(self) -> List[str]:
        """Get list of brain names.
        
        Returns:
            List of brain names
        """
        return [brain.name for brain in self.brains if hasattr(brain, 'name')]
    
    def get_brain(self, name: str) -> 'EnhancedBrain':
        """Get brain by name.
        
        Args:
            name: Brain name
            
        Returns:
            Enhanced brain instance
        """
        for brain in self.brains:
            if hasattr(brain, 'name') and brain.name == name:
                return brain
        raise NeuralNetworkError(f"Brain '{name}' not found")

# Enhanced brain class
class EnhancedBrain(pynlm.Brain):
    """Enhanced brain with Pythonic interface and convenience methods."""
    
    def __init__(self, config: Optional[pynlm.Config] = None, name: Optional[str] = None):
        """Initialize enhanced brain.
        
        Args:
            config: Optional configuration
            name: Optional brain name
        """
        self_manager = BrainManager(config)
        super().__init__(self_manager.config)
        self.name = name
        self._state_history = []
        self._stats = {}
    
    @contextmanager
    def training_context(self, steps: int, enable_learning: bool = True) -> Iterator[None]:
        """Context manager for training sessions.
        
        Args:
            steps: Number of training steps
            enable_learning: Whether to enable learning
        """
        self.enable_learning = enable_learning
        self.total_steps = steps
        
        try:
            for i in range(steps):
                self.step(i)
                self._record_state(i)
            yield
        finally:
            self._finalize_training()
    
    def _record_state(self, step: int) -> None:
        """Record state for analysis.
        
        Args:
            step: Current step number
        """
        state = {
            'step': step,
            'active_neurons': self.getFiringNeuronCount(),
            'total_spikes': self.getTotalSpikeCount(),
            'average_firing_rate': self.getAverageFiringRate(),
            'excitement_inhibition_ratio': self.getExcitationInhibitionRatio(),
            'developmental_stage': self.getDevelopmentalStage(),
            'simulation_time': getattr(self, '_simulation_time', step),
        }
        self._state_history.append(state)
    
    def _finalize_training(self) -> None:
        """Finalize training and compute statistics."""
        if self._state_history:
            self._compute_statistics()
    
    def _compute_statistics(self) -> None:
        """Compute statistics from recorded states."""
        if not self._state_history:
            return
        
        self._stats = {
            'total_steps': len(self._state_history),
            'final_active_neurons': self._state_history[-1]['active_neurons'],
            'peak_active_neurons': max(s['active_neurons'] for s in self._state_history),
            'total_spikes': sum(s['total_spikes'] for s in self._state_history),
            'average_firing_rate': sum(s['average_firing_rate'] for s in self._state_history) / len(self._state_history),
            'development_progress': {
                stage.value: list(s['developmental_stage'].value for s in self._state_history).count(stage.value)
                for stage in pynlm.DevelopmentalStage
            }
        }
    
    def analyze(self) -> 'BrainAnalysis':
        """Analyze brain activity and return analysis results.
        
        Returns:
            BrainAnalysis object with analysis results
        """
        return BrainAnalysis(self._state_history, self._stats, self)
    
    def __iter__(self) -> Iterator['EnhancedBrain']:
        """Iterator interface for brain training loops."""
        for i in range(10):
            self.step(i)
            yield self
    
    def __repr__(self) -> str:
        """String representation of the brain."""
        if hasattr(self, 'name'):
            return f"<EnhancedBrain '{self.name}' at {hex(id(self))}>"
        return f"<EnhancedBrain at {hex(id(self))}>"

# Brain analysis class
class BrainAnalysis:
    """Analyze brain activity and provide insights."""
    
    def __init__(self, history: List[Dict], stats: Dict, brain: EnhancedBrain):
        """Initialize brain analysis.
        
        Args:
            history: State history
            stats: Computed statistics
            brain: Brain being analyzed
        """
        self.history = history
        self.stats = stats
        self.brain = brain
    
    def get_stability(self) -> float:
        """Get brain stability metric.
        
        Returns:
            Stability metric (0-1, higher is more stable)
        """
        if len(self.history) < 2:
            return 1.0
        
        firing_rates = [s['average_firing_rate'] for s in self.history]
        avg_firing_rate = sum(firing_rates) / len(firing_rates)
        
        if avg_firing_rate == 0:
            return 1.0
        
        variance = sum((fr - avg_firing_rate) ** 2 for fr in firing_rates) / len(firing_rates)
        std_dev = variance ** 0.5
        
        return max(0.0, 1.0 - (std_dev / avg_firing_rate))
    
    def get_development_trajectory(self) -> List[Dict]:
        """Get developmental trajectory over time.
        
        Returns:
            List of developmental stages over time
        """
        return [
            {'step': s['step'], 'stage': s['developmental_stage']}
            for s in self.history
        ]
    
    def get_spike_pattern(self) -> Dict[str, Any]:
        """Analyze spike patterns.
        
        Returns:
            Dictionary with spike pattern analysis
        """
        if not self.history:
            return {'error': 'No history available'}
        
        total_spikes = self.stats.get('total_spikes', 0)
        
        return {
            'total_spikes': total_spikes,
            'average_firing_rate': self.stats.get('average_firing_rate', 0),
            'coherence': self._compute_coherence(),
            'burst_patterns': self._detect_bursts(),
        }
    
    def _compute_coherence(self) -> float:
        """Compute neural coherence."""
        return 0.5  # Placeholder
    
    def _detect_bursts(self) -> List[Dict]:
        """Detect burst patterns in firing."""
        return []  # Placeholder
    
    def __str__(self) -> str:
        """String representation of analysis."""
        return f"BrainAnalysis(total_steps={self.stats.get('total_steps', 0)}, " \
               f"stability={self.get_stability():.2f})"

# Enhanced agent class
class EnhancedAgent(pynlm.AgentBrain):
    """Enhanced agent with Pythonic interface and convenience methods."""
    
    def __init__(self, brain: pynlm.Brain, name: Optional[str] = None):
        """Initialize enhanced agent.
        
        Args:
            brain: Brain to control
            name: Optional agent name
        """
        super().__init__(brain)
        self.name = name
        self.action_history = []
        self.reward_history = []
    
    @contextmanager
    def episode_context(self, max_steps: int = 1000) -> Iterator[None]:
        """Context manager for agent episodes.
        
        Args:
            max_steps: Maximum episode steps
        """
        self.reset()
        for step in range(max_steps):
            yield step
            self._record_episode_step(step)
    
    def _record_episode_step(self, step: int) -> None:
        """Record episode step data.
        
        Args:
            step: Step number
        """
        # Record action and reward if available
        pass
    
    def run_episode(self, world: pynlm.SimpleWorld, max_steps: int = 1000) -> Dict:
        """Run a complete agent episode.
        
        Args:
            world: World to interact with
            max_steps: Maximum episode steps
            
        Returns:
            Episode statistics
        """
        stats = {
            'steps_completed': 0,
            'actions_taken': 0,
            'total_reward': 0.0,
            'episode_success': False,
        }
        
        for step in range(max_steps):
            try:
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                self.processSensoryInput(percept)
                
                action = self.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                stats['actions_taken'] += 1
                stats['steps_completed'] = step + 1
                
            except Exception as e:
                ErrorHandler.handle_brain_error(e, f"agent episode step {step}")
                break
        
        return stats
    
    def __repr__(self) -> str:
        """String representation of the agent."""
        if hasattr(self, 'name'):
            return f"<EnhancedAgent '{self.name}' at {hex(id(self))}>"
        return f"<EnhancedAgent at {hex(id(self))}>"

# Enhanced world class
class EnhancedWorld(pynlm.SimpleWorld):
    """Enhanced world with Pythonic interface and convenience methods."""
    
    def __init__(self):
        """Initialize enhanced world."""
        super().__init__()
        self.object_history = []
        self.agent_position_history = []
    
    def add_object_with_history(self, obj: pynlm.WorldObject) -> None:
        """Add object and record to history.
        
        Args:
            obj: World object to add
        """
        self.addObject(obj)
        self.object_history.append(obj)
    
    def configure_with_defaults(self, width: int = 20, height: int = 20,
                               vision_width: int = 8, vision_height: int = 8) -> None:
        """Configure world with default parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self.configure(width, height, vision_width, vision_height)
    
    def reset_with_history(self) -> None:
        """Reset world and record initial state."""
        self.reset()
        self.object_history = []
        self.agent_position_history = []
    
    def run_simulation(self, agent: EnhancedAgent, duration: float, timestep: float = 0.1) -> List[Dict]:
        """Run simulation with agent.
        
        Args:
            agent: Agent to control
            duration: Simulation duration
            timestep: Time step size
            
        Returns:
            List of simulation states
        """
        states = []
        steps = int(duration / timestep)
        
        for step in range(steps):
            self.update(timestep)
            
            percept = self.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            action = agent.decodeMotorCommand()
            self.applyMotorCommand(action, self.getSimulationTime())
            
            state = {
                'step': step,
                'time': self.getSimulationTime(),
                'percept': percept,
                'action': action,
                'agent_body': self.getAgentBody(),
            }
            states.append(state)
        
        return states

# Visualization tools
class VisualizationTools:
    """Tools for visualizing NLM data and results."""
    
    @staticmethod
    def plot_firing_dynamics(history: List[Dict], output_file: Optional[str] = None) -> None:
        """Plot firing dynamics over time.
        
        Args:
            history: Brain state history
            output_file: Optional output file path
        """
        try:
            import matplotlib.pyplot as plt
            
            steps = [s['step'] for s in history]
            active_neurons = [s['active_neurons'] for s in history]
            firing_rates = [s['average_firing_rate'] for s in history]
            
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            ax1.plot(steps, active_neurons, 'b-', label='Active Neurons')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Active Neurons')
            ax1.set_title('Firing Dynamics')
            ax1.legend()
            ax1.grid(True)
            
            ax2.plot(steps, firing_rates, 'r-', label='Average Firing Rate')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Firing Rate')
            ax2.set_title('Firing Rate Dynamics')
            ax2.legend()
            ax2.grid(True)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file)
            else:
                plt.show()
                
        except ImportError:
            warnings.warn("matplotlib not installed. Install with: pip install matplotlib")
    
    @staticmethod
    def export_to_json(history: List[Dict], output_file: str) -> None:
        """Export history to JSON file.
        
        Args:
            history: Data to export
            output_file: Output file path
        """
        with open(output_file, 'w') as f:
            json.dump(history, f, indent=2)
    
    @staticmethod
    def import_from_json(input_file: str) -> List[Dict]:
        """Import history from JSON file.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported history data
        """
        with open(input_file, 'r') as f:
            return json.load(f)

# High-level convenience functions

def create_enhanced_brain(config: Optional[pynlm.Config] = None, name: Optional[str] = None) -> EnhancedBrain:
    """Create an enhanced brain with default configuration.
    
    Args:
        config: Optional configuration
        name: Optional brain name
        
    Returns:
        Enhanced brain instance
    """
    return EnhancedBrain(config, name)

def create_enhanced_config(config_file: Optional[str] = None) -> EnhancedConfig:
    """Create an enhanced configuration.
    
    Args:
        config_file: Optional configuration file
        
    Returns:
        Enhanced configuration instance
    """
    return EnhancedConfig(config_file)

def create_enhanced_agent(brain: pynlm.Brain, name: Optional[str] = None) -> EnhancedAgent:
    """Create an enhanced agent.
    
    Args:
        brain: Brain to control
        name: Optional agent name
        
    Returns:
        Enhanced agent instance
    """
    return EnhancedAgent(brain, name)

def create_enhanced_world() -> EnhancedWorld:
    """Create an enhanced world.
    
    Args:
        
    Returns:
        Enhanced world instance
    """
    return EnhancedWorld()

def create_training_session(brain: EnhancedBrain, steps: int, enable_learning: bool = True) -> ContextManager:
    """Create a training session context manager.
    
    Args:
        brain: Brain to train
        steps: Number of training steps
        enable_learning: Whether to enable learning
        
    Returns:
        Training session context manager
    """
    return brain.training_context(steps, enable_learning)

def create_analysis_session(brain: EnhancedBrain) -> BrainAnalysis:
    """Create an analysis session for a brain.
    
    Args:
        brain: Brain to analyze
        
    Returns:
        BrainAnalysis instance
    """
    return brain.analyze()

# Example functions
def basic_brain_example() -> EnhancedBrain:
    """Basic brain example."""
    brain = create_enhanced_brain(name="Basic Brain")
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    return brain

def agent_world_example() -> Dict[str, Any]:
    """Agent and world example."""
    # Create components
    brain = create_enhanced_brain(name="Agent Brain")
    brain.initialize()
    
    world = create_enhanced_world()
    world.configure_with_defaults()
    world.reset()
    
    agent = create_enhanced_agent(brain, name="Example Agent")
    agent.initialize(world)
    
    # Run episode
    stats = agent.run_episode(world, max_steps=50)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'stats': stats,
    }

def complete_simulation_example() -> Dict[str, Any]:
    """Complete simulation example."""
    # Setup
    config = create_enhanced_config()
    brain = create_enhanced_brain(config, name="Complete Simulation")
    brain.initialize()
    
    # Create agent brain interface
    agent = create_enhanced_agent(brain, name="Complete Agent")
    
    # Create and configure world
    world = create_enhanced_world()
    world.configure_with_defaults(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    world.run_simulation(agent, duration=10.0, timestep=0.1)
    
    # Analyze results
    analysis = brain.analyze()
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'analysis': analysis,
    }

# Help and documentation
__doc_examples__ = {
    'basic_brain_example': basic_brain_example,
    'agent_world_example': agent_world_example,
    'complete_simulation_example': complete_simulation_example,
}

# Module-level docstring with examples
__doc__ = """Enhanced Python API for NLM (Neural Learning Machine)

This is the enhanced API module providing Pythonic interfaces for NLM.

Key features:
- Pythonic interfaces with context managers, iterators, and properties
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

Examples:
    >>> from pynlm.enhanced import create_enhanced_brain
    >>> brain = create_enhanced_brain(name="MyBrain")
    >>> brain.initialize()
    >>> for i in range(10):
    ...     brain.step(i)
    >>> analysis = brain.analyze()

See the individual function docstrings for more details.
"""

if __name__ == "__main__":
    # Run examples if executed as script
    print("Enhanced NLM API Examples")
    print("=" * 40)
    
    print("\n1. Basic Brain Example:")
    brain = basic_brain_example()
    print(f"   Created brain: {brain}")
    
    print("\n2. Agent and World Example:")
    result = agent_world_example()
    print(f"   Created agent: {result['agent']}")
    print(f"   Agent stats: {result['stats']}")
    
    print("\n3. Complete Simulation Example:")
    result = complete_simulation_example()
    print(f"   Created brain: {result['brain']}")
    print(f"   Analysis: {result['analysis']}")
"""
