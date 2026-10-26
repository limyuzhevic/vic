import sys
import time
import json
from pathlib import Path
from typing import List, Dict, Any, Optional, Callable
from dataclasses import dataclass, field
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class InitializationError(NLMError):
    """Raised when brain initialization fails."""
    pass

class ConfigurationError(NLMError):
    """Raised when configuration is invalid."""
    pass

class SimulationError(NLMError):
    """Raised when simulation fails."""
    pass

@dataclass
class SimulationMetrics:
    """Collection of simulation metrics for analysis."""
    step: int = 0
    time: float = 0.0
    total_neurons: int = 0
    active_neurons: int = 0
    firing_rate: float = 0.0
    total_spikes: int = 0
    e_i_ratio: float = 0.0
    development_stage: str = "Unknown"
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0

    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary for analysis."""
        return {
            "step": self.step,
            "time": self.time,
            "total_neurons": self.total_neurons,
            "active_neurons": self.active_neurons,
            "firing_rate": self.firing_rate,
            "total_spikes": self.total_spikes,
            "e_i_ratio": self.e_i_ratio,
            "development_stage": self.development_stage,
            "curiosity_level": self.curiosity_level,
            "novelty_level": self.novelty_level,
            "prediction_error": self.prediction_error
        }

    @classmethod
    def from_brain(cls, brain, step: int, time: float) -> 'SimulationMetrics':
        """Create metrics from brain state."""
        try:
            return cls(
                step=step,
                time=time,
                total_neurons=brain.getTotalNeuronCount(),
                active_neurons=brain.getFiringNeuronCount(),
                firing_rate=brain.getAverageFiringRate(),
                total_spikes=brain.getTotalSpikeCount(),
                e_i_ratio=brain.getExcitationInhibitionRatio(),
                development_stage=brain.getDevelopmentalStage(),
                curiosity_level=getattr(brain, 'getCuriosityLevel', lambda: 0.0)(),
                novelty_level=getattr(brain, 'getNoveltyLevel', lambda: 0.0)(),
                prediction_error=getattr(brain, 'getPredictionError', lambda: 0.0)()
            )
        except Exception as e:
            logger.warning(f"Failed to get some metrics: {e}")
            # Return minimal metrics
            return cls(
                step=step,
                time=time,
                total_neurons=brain.getTotalNeuronCount(),
                active_neurons=brain.getFiringNeuronCount(),
                firing_rate=brain.getAverageFiringRate(),
                total_spikes=brain.getTotalSpikeCount(),
                e_i_ratio=brain.getExcitationInhibitionRatio(),
                development_stage="Unknown"
            )


class BrainSimulator:
    """
    High-level Python interface for NLM brain simulation.
    
    Provides a more user-friendly API with better error handling,
    metrics collection, and simulation control.
    """
    
    def __init__(self, config=None):
        """
        Initialize the brain simulator.
        
        Args:
            config: NLM configuration object. If None, creates default config.
        """
        try:
            # Import pynlm here to handle import errors gracefully
            import pynlm
            self._pynlm = pynlm
            self.logger = logger
            
            # Create brain with configuration
            if config is not None:
                self.brain = pynlm.createBrain(config)
            else:
                self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
                
            self.initialized = False
            self.metrics_history = []
            
            logger.info("BrainSimulator initialized successfully")
            
        except ImportError as e:
            raise NLMError(f"Failed to import pynlm: {e}")
        except Exception as e:
            raise InitializationError(f"Failed to initialize BrainSimulator: {e}")
    
    def initialize(self, **kwargs):
        """
        Initialize the brain with enhanced error handling.
        
        Args:
            **kwargs: Additional configuration options
            
        Raises:
            InitializationError: If initialization fails
        """
        try:
            if self.initialized:
                logger.warning("Brain already initialized. Resetting first.")
                self.reset()
            
            # Apply any additional configuration
            for key, value in kwargs.items():
                if hasattr(self.brain, f'set{key.title()}'):
                    getattr(self.brain, f'set{key.title()}')(value)
                else:
                    self.brain.set(key, value)
            
            # Initialize the brain
            success = self.brain.initialize()
            if not success:
                raise InitializationError("Brain initialization failed")
                
            self.initialized = True
            logger.info("Brain initialized successfully")
            
        except Exception as e:
            self.initialized = False
            raise InitializationError(f"Brain initialization failed: {e}")
    
    def step(self, step_number: int, timestep: float = None) -> SimulationMetrics:
        """
        Run a single simulation step with enhanced metrics collection.
        
        Args:
            step_number: Current step number
            timestep: Optional timestep override
            
        Returns:
            SimulationMetrics: Collected metrics for this step
            
        Raises:
            SimulationError: If step fails
        """
        if not self.initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
        
        try:
            # Determine timestep
            if timestep is None:
                # Use brain config timestep if available
                try:
                    timestep = self.brain.getTimestep() if hasattr(self.brain, 'getTimestep') else 0.001
                except:
                    timestep = 0.001
            
            # Run the step
            if timestep > 0:
                self.brain.step(step_number, step_number * timestep)
            else:
                self.brain.step(step_number)
            
            # Collect metrics
            metrics = SimulationMetrics.from_brain(self.brain, step_number, step_number * timestep)
            self.metrics_history.append(metrics)
            
            self.logger.debug(f"Step {step_number} completed: {metrics.active_neurons} firing neurons")
            
            return metrics
            
        except Exception as e:
            raise SimulationError(f"Simulation step failed: {e}")
    
    def run(self, num_steps: int, timestep: float = None, 
            callback: Optional[Callable] = None, 
            collect_metrics: bool = True) -> List[SimulationMetrics]:
        """
        Run a complete simulation sequence.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Optional timestep for all steps
            callback: Optional function called at each step with metrics
            collect_metrics: Whether to collect and return metrics
            
        Returns:
            List[SimulationMetrics]: History of simulation metrics
            
        Raises:
            SimulationError: If simulation fails
        """
        if not self.initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
        
        if collect_metrics:
            self.metrics_history = []
        
        logger.info(f"Starting simulation: {num_steps} steps")
        
        try:
            for step in range(num_steps):
                # Run step
                metrics = self.step(step, timestep)
                
                # Call callback if provided
                if callback is not None:
                    callback(metrics)
                
                # Log progress periodically
                if step % 100 == 0:
                    logger.info(f"Step {step}/{num_steps}: {metrics.active_neurons} firing neurons")
            
            logger.info(f"Simulation completed: {num_steps} steps")
            
            if collect_metrics:
                return self.metrics_history
            else:
                return []
                
        except Exception as e:
            raise SimulationError(f"Simulation failed at step {step}: {e}")
    
    def reset(self):
        """Reset the brain to initial state."""
        try:
            self.brain.reset()
            self.initialized = False
            self.metrics_history = []
            logger.info("Brain reset successfully")
        except Exception as e:
            raise SimulationError(f"Brain reset failed: {e}")
    
    def save(self, filepath: str):
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save brain state
            
        Raises:
            SimulationError: If save fails
        """
        try:
            self.brain.save(filepath)
            logger.info(f"Brain state saved to {filepath}")
        except Exception as e:
            raise SimulationError(f"Failed to save brain state: {e}")
    
    def load(self, filepath: str):
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
            
        Raises:
            SimulationError: If load fails
        """
        try:
            success = self.brain.load(filepath)
            if success:
                self.initialized = True
                logger.info(f"Brain state loaded from {filepath}")
            else:
                raise SimulationError(f"Failed to load brain state from {filepath}")
        except Exception as e:
            self.initialized = False
            raise SimulationError(f"Failed to load brain state: {e}")
    
    def get_metrics_summary(self) -> Dict[str, Any]:
        """
        Get summary statistics of simulation metrics.
        
        Returns:
            Dictionary containing summary statistics
        """
        if not self.metrics_history:
            return {}
        
        steps = [m.step for m in self.metrics_history]
        
        summary = {
            "total_steps": len(self.metrics_history),
            "step_range": (min(steps), max(steps)),
            "total_neurons": self.metrics_history[0].total_neurons if self.metrics_history else 0,
            "max_firing_neurons": max(m.active_neurons for m in self.metrics_history),
            "min_firing_neurons": min(m.active_neurons for m in self.metrics_history),
            "avg_firing_rate": sum(m.firing_rate for m in self.metrics_history) / len(self.metrics_history),
            "total_spikes": sum(m.total_spikes for m in self.metrics_history),
            "development_stages": list(set(m.development_stage for m in self.metrics_history))
        }
        
        return summary

    def export_metrics(self, filepath: str, format: str = "json"):
        """
        Export simulation metrics to file.
        
        Args:
            filepath: Output file path
            format: Export format (json, csv, or pickle)
            
        Raises:
            SimulationError: If export fails
        """
        try:
            if format.lower() == "json":
                data = {
                    "metrics": [m.to_dict() for m in self.metrics_history],
                    "summary": self.get_metrics_summary()
                }
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
            
            elif format.lower() == "csv":
                import pandas as pd
                df = pd.DataFrame([m.to_dict() for m in self.metrics_history])
                df.to_csv(filepath, index=False)
            
            else:
                raise SimulationError(f"Unsupported format: {format}")
            
            logger.info(f"Metrics exported to {filepath} ({format})")
            
        except Exception as e:
            raise SimulationError(f"Failed to export metrics: {e}")


class NLMEnvironment:
    """
    Python wrapper for world/environment simulation.
    
    Provides enhanced interface for world simulation and agent interaction.
    """
    
    def __init__(self):
        """Initialize environment."""
        try:
            import pynlm
            self._pynlm = pynlm
            self.world = pynlm.createSimpleWorld()
            self.logger = logger
            self.configured = False
            
            logger.info("NLMEnvironment initialized")
            
        except ImportError as e:
            raise NLMError(f"Failed to import pynlm: {e}")
    
    def configure(self, width: int = 20, height: int = 20, 
                  vision_width: int = 8, vision_height: int = 8,
                  max_energy: float = 100.0, energy_decay: float = 0.95):
        """
        Configure the world environment.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
            max_energy: Maximum agent energy
            energy_decay: Energy decay rate per timestep
        """
        try:
            self.world.configure(width, height, vision_width, vision_height)
            self.world.setMaxEnergy(max_energy)
            self.world.setEnergyDecayRate(energy_decay)
            self.configured = True
            logger.info(f"World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
            
        except Exception as e:
            raise SimulationError(f"Failed to configure world: {e}")
    
    def reset(self):
        """Reset environment to initial state."""
        if not self.configured:
            raise SimulationError("World not configured. Call configure() first.")
        
        self.world.reset()
        logger.info("World reset")
    
    def update(self, timestep: float = 0.1):
        """
        Update world state.
        
        Args:
            timestep: Time step duration
            
        Returns:
            Dictionary with sensor readings and agent state
        """
        if not self.configured:
            raise SimulationError("World not configured. Call configure() first.")
        
        try:
            self.world.update(timestep)
            
            # Get percept and agent body
            percept = self.world.getSensoryPercept()
            body = self.world.getAgentBody()
            
            return {
                "percept": percept,
                "agent_body": body,
                "timestamp": self.world.getSimulationTime()
            }
            
        except Exception as e:
            raise SimulationError(f"World update failed: {e}")
    
    def apply_action(self, action, current_time: float = None):
        """
        Apply agent action to world.
        
        Args:
            action: Action to apply
            current_time: Optional timestamp
            
        Returns:
            Action result
        """
        if not self.configured:
            raise SimulationError("World not configured. Call configure() first.")
        
        if current_time is None:
            current_time = self.world.getSimulationTime()
        
        try:
            result = self.world.applyMotorCommand(action, current_time)
            logger.debug(f"Applied action: {action}")
            return result
            
        except Exception as e:
            raise SimulationError(f"Failed to apply action: {e}")
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        if not self.configured:
            raise SimulationError("World not configured. Call configure() first.")
        
        self.world.setAgentStart(x, y)
        logger.debug(f"Agent start position set to ({x}, {y})")


class NLMConfigurator:
    """
    Enhanced Python interface for NLM configuration management.
    """
    
    def __init__(self):
        """Initialize configurator."""
        try:
            import pynlm
            self._pynlm = pynlm
            self.config = pynlm.createDefaultConfig()
            self.logger = logger
            
            logger.info("NLMConfigurator initialized")
            
        except ImportError as e:
            raise NLMError(f"Failed to import pynlm: {e}")
    
    def set(self, key: str, value: Any, source: str = "runtime"):
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            source: Source of configuration (file, command_line, runtime)
        """
        try:
            # Map string source to enum if needed
            source_enum = getattr(self._pynlm, f"ConfigSource_{source.title()}", 
                                 getattr(self._pynlm, "ConfigSource_Runtime", None))
            
            if source_enum:
                self.config.set(key, value, source_enum)
            else:
                self.config.set(key, value)
                
            logger.debug(f"Configuration set: {key} = {value} (source: {source})")
            
        except Exception as e:
            raise ConfigurationError(f"Failed to set configuration: {e}")
    
    def get(self, key: str, default=None):
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        try:
            if self.config.has(key):
                # Try to get as different types
                for type_name in ['int', 'float', 'string', 'bool']:
                    try:
                        val = self.config.get(type_name)(key)
                        if val is not None:
                            return val
                    except:
                        pass
                # Fallback to string
                return self.config.get("string")(key)
            else:
                return default
        except Exception as e:
            logger.warning(f"Failed to get configuration {key}: {e}")
            return default
    
    def load(self, filepath: str):
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            success = self.config.loadFromFile(filepath)
            if success:
                logger.info(f"Configuration loaded from {filepath}")
            else:
                logger.warning(f"Failed to load configuration from {filepath}")
            return success
        except Exception as e:
            raise ConfigurationError(f"Failed to load configuration: {e}")
    
    def save(self, filepath: str):
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
            
        Raises:
            ConfigurationError: If save fails
        """
        try:
            success = self.config.saveToFile(filepath)
            if success:
                logger.info(f"Configuration saved to {filepath}")
            else:
                raise ConfigurationError(f"Failed to save configuration to {filepath}")
        except Exception as e:
            raise ConfigurationError(f"Failed to save configuration: {e}")


# Global convenience functions
def create_brain(config=None):
    """Create a brain with optional configuration."""
    simulator = BrainSimulator(config)
    return simulator


def create_agent(config=None, world=None):
    """Create an agent with optional configuration and world."""
    import pynlm
    brain = pynlm.createBrain(config) if config else pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    if world:
        agent.initialize(world)
    
    return agent, brain


def create_world(**kwargs):
    """Create and configure a world."""
    env = NLMEnvironment()
    env.configure(**kwargs)
    return env


def run_experiment(brain_config=None, world_config=None, num_steps=1000):
    """
    Run a complete experiment with brain and world.
    
    Args:
        brain_config: Brain configuration
        world_config: World configuration
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with results
    """
    # Create brain and agent
    brain_sim = BrainSimulator(brain_config)
    brain_sim.initialize()
    
    # Create and configure world
    env = NLMEnvironment()
    env.configure(**world_config if world_config else {})
    env.reset()
    
    # Create agent
    import pynlm
    agent = pynlm.createAgentBrain(brain_sim.brain)
    agent.initialize(env.world)
    
    # Run simulation
    def callback(metrics):
        if metrics.step % 100 == 0:
            logger.info(f"Step {metrics.step}: {metrics.active_neurons} firing neurons")
    
    results = brain_sim.run(num_steps, callback=callback)
    
    return {
        "brain_simulator": brain_sim,
        "environment": env,
        "agent": agent,
        "metrics": results
    }


# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python interface for NLM neural simulation framework"

# Export key classes and functions
__all__ = [
    'NLMError',
    'InitializationError', 
    'ConfigurationError',
    'SimulationError',
    'BrainSimulator',
    'NLMEnvironment',
    'NLMConfigurator',
    'SimulationMetrics',
    'create_brain',
    'create_agent',
    'create_world',
    'run_experiment',
    '__version__',
    '__author__',
    '__description__'
]

# Initialize logging
logging.getLogger(__name__).info(f"NLM Enhanced Python module v{__version__} loaded")