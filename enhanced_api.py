#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Enhanced Python API

This module provides enhanced Python bindings for the NLM framework with:
- Advanced error handling and validation
- Configuration templates and presets
- Simulation control and monitoring
- Performance optimization utilities
- Visualization and analysis tools
"""

import warnings
import logging
from typing import Dict, Any, Optional, List, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
import time

# Configure logging for NLM operations
logging.basicConfig(level=logging.INFO, format='%(asctime)s - NLM - %(levelname)s - %(message)s')
logger = logging.getLogger("nlm")

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigValidationError(NLMError):
    """Configuration validation error."""
    pass

class SimulationError(NLMError):
    """Simulation execution error."""
    pass

class PerformanceError(NLMError):
    """Performance-related error."""
    pass

@dataclass
class NMLErrorDetails:
    """Details for NLM errors."""
    code: str
    message: str
    details: Dict[str, Any] = field(default_factory=dict)
    suggestions: List[str] = field(default_factory=list)

class ValidationLevel(Enum):
    """Configuration validation levels."""
    OFF = "off"
    WARNING = "warning"
    STRICT = "strict"
    AUTO_FIX = "auto_fix"

class SimulationMode(Enum):
    """Simulation modes."""
    REAL_TIME = "real_time"
    STEP_BY_STEP = "step_by_step"
    BATCH = "batch"
    CONTINUOUS = "continuous"

@dataclass
class NMLEnvironmentConfig:
    """Enhanced configuration for NLM simulation environment."""
    # Basic simulation parameters
    neuron_count: int = 1000
    region_count: int = 2
    simulation_timestep: float = 0.001
    max_simulation_steps: int = 10000
    
    # Connectivity parameters
    connection_probability: float = 0.1
    initial_weight_mean: float = 0.5
    initial_weight_std: float = 0.1
    
    # Neuron dynamics
    v_thresh: float = -50.0
    v_rest: float = -70.0
    v_reset: float = -75.0
    tau_mem: float = 20.0
    tau_ref: float = 2.0
    
    # Plasticity parameters
    stdp_enable: bool = True
    stdp_learning_rate: float = 0.001
    stdp_tau_plus: float = 20.0
    stdp_tau_minus: float = 20.0
    hebbian_enable: bool = True
    hebbian_learning_rate: float = 0.01
    structural_enable: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Development parameters
    development_synaptogenesis_rate: float = 0.001
    development_pruning_rate: float = 0.0001
    developmental_stage_duration: float = 60.0
    
    # Neuromodulation parameters
    dopamine_baseline: float = 0.0
    curiosity_enable: bool = True
    curiosity_threshold: float = 0.5
    novelty_decay_rate: float = 0.99
    
    # Reward parameters
    reward_discount_factor: float = 0.99
    reward_scaling: float = 1.0
    
    # World parameters
    environment_name: str = "GridWorld"
    environment_width: int = 10
    environment_height: int = 10
    vision_width: int = 8
    vision_height: int = 8
    vision_channels: int = 3
    
    # Logging parameters
    log_level: str = "INFO"
    log_to_file: bool = False
    log_filename: str = "nlm.log"
    log_file_append: bool = True
    
    # Visualization parameters
    visualization_enabled: bool = False
    visualization_update_rate: float = 30.0
    visualization_backend: str = "ascii"
    
    # Performance parameters
    enable_performance_monitoring: bool = False
    performance_sampling_rate: float = 1.0
    enable_memory_profiling: bool = False
    
    # Safety parameters
    enable_safety_checks: bool = True
    max_energy: float = 100.0
    energy_decay_rate: float = 0.99
    health_threshold: float = 0.1
    
    # Callback functions
    pre_step_callback: Optional[Callable] = None
    post_step_callback: Optional[Callable] = None
    reward_callback: Optional[Callable] = None
    novelty_callback: Optional[Callable] = None
    
    # Advanced features
    enable_curiosity_driven_exploration: bool = True
    curiosity_exploration_rate: float = 0.1
    enable_hierarchical_planning: bool = False
    enable_metacognition: bool = False
    enable_transfer_learning: bool = False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {k: v for k, v in self.__dict__.items() 
                if not k.startswith('_') and not callable(v)}
    
    @classmethod
    def from_preset(cls, preset_name: str) -> 'NMLEnvironmentConfig':
        """Create configuration from preset."""
        presets = {
            'minimal': cls(
                neuron_count=100,
                region_count=1,
                simulation_timestep=0.01,
                max_simulation_steps=1000,
                connection_probability=0.05,
                visualization_enabled=False
            ),
            'balanced': cls(
                neuron_count=1000,
                region_count=2,
                simulation_timestep=0.001,
                max_simulation_steps=10000,
                connection_probability=0.1,
                visualization_enabled=False
            ),
            'complex': cls(
                neuron_count=5000,
                region_count=5,
                simulation_timestep=0.0001,
                max_simulation_steps=100000,
                connection_probability=0.2,
                visualization_enabled=True,
                enable_curiosity_driven_exploration=True,
                enable_hierarchical_planning=True
            ),
            'research': cls(
                neuron_count=10000,
                region_count=10,
                simulation_timestep=0.00001,
                max_simulation_steps=1000000,
                connection_probability=0.3,
                visualization_enabled=True,
                enable_curiosity_driven_exploration=True,
                enable_hierarchical_planning=True,
                enable_metacognition=True,
                enable_transfer_learning=True,
                enable_memory_profiling=True
            ),
            'production': cls(
                neuron_count=2000,
                region_count=3,
                simulation_timestep=0.001,
                max_simulation_steps=50000,
                connection_probability=0.15,
                visualization_enabled=True,
                enable_performance_monitoring=True,
                log_to_file=True,
                log_filename="nlm_production.log"
            )
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available presets: {list(presets.keys())}")
        
        config = presets[preset_name]
        
        # Apply preset-specific modifications
        if preset_name == 'minimal':
            config.enable_performance_monitoring = False
            config.enable_memory_profiling = False
        elif preset_name == 'balanced':
            config.enable_curiosity_driven_exploration = True
            config.enable_hierarchical_planning = False
        elif preset_name == 'complex':
            config.enable_curiosity_driven_exploration = True
            config.enable_hierarchical_planning = True
            config.enable_metacognition = True
        elif preset_name == 'research':
            config.enable_curiosity_driven_exploration = True
            config.enable_hierarchical_planning = True
            config.enable_metacognition = True
            config.enable_transfer_learning = True
        elif preset_name == 'production':
            config.enable_performance_monitoring = True
            config.enable_safety_checks = True
            config.log_to_file = True
        
        return config

class NLMException(Exception):
    """Custom exception class for NLM errors with error codes."""
    
    def __init__(self, message: str, error_code: Optional[str] = None, details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
    
    def __repr__(self):
        return f"NLMException(code={self.error_code}, message={self.message})"

class ConfigurationValidator:
    """Validates NML configuration parameters."""
    
    # Valid ranges for parameters
    VALID_RANGES = {
        'neuron_count': (10, 1000000),
        'region_count': (1, 100),
        'simulation_timestep': (0.00001, 1.0),
        'max_simulation_steps': (1, 10000000),
        'connection_probability': (0.0, 1.0),
        'initial_weight_mean': (-10.0, 10.0),
        'initial_weight_std': (0.0, 10.0),
        'v_thresh': (-100.0, 0.0),
        'v_rest': (-100.0, 0.0),
        'v_reset': (-100.0, 0.0),
        'tau_mem': (0.1, 1000.0),
        'tau_ref': (0.1, 1000.0),
        'stdp_learning_rate': (0.0, 1.0),
        'stdp_tau_plus': (0.1, 1000.0),
        'stdp_tau_minus': (0.1, 1000.0),
        'hebbian_learning_rate': (0.0, 1.0),
        'synaptogenesis_rate': (0.0, 1.0),
        'pruning_rate': (0.0, 1.0),
        'development_synaptogenesis_rate': (0.0, 1.0),
        'development_pruning_rate': (0.0, 1.0),
        'dopamine_baseline': (-10.0, 10.0),
        'curiosity_threshold': (0.0, 1.0),
        'novelty_decay_rate': (0.0, 1.0),
        'reward_discount_factor': (0.0, 1.0),
        'reward_scaling': (0.0, 100.0),
        'environment_width': (1, 1000),
        'environment_height': (1, 1000),
        'vision_width': (1, 100),
        'vision_height': (1, 100),
        'vision_channels': (1, 10),
        'visualization_update_rate': (1.0, 1000.0),
        'performance_sampling_rate': (0.1, 1000.0),
        'energy_decay_rate': (0.0, 1.0),
        'health_threshold': (0.0, 1.0),
        'curiosity_exploration_rate': (0.0, 1.0),
    }
    
    # Required parameters for different preset modes
    REQUIRED_PARAMS = {
        'minimal': ['neuron_count', 'region_count'],
        'balanced': ['neuron_count', 'region_count', 'simulation_timestep'],
        'complex': ['neuron_count', 'region_count', 'connection_probability'],
        'research': ['neuron_count', 'region_count', 'connection_probability', 'visualization_enabled'],
        'production': ['neuron_count', 'region_count', 'max_simulation_steps'],
    }
    
    # Valid values for enum parameters
    VALID_VALUES = {
        'log_level': ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
        'visualization_backend': ['ascii', 'console', 'file', 'web'],
        'environment_name': ['GridWorld', 'CageWorld', 'MazeWorld', 'CustomWorld'],
    }
    
    @staticmethod
    def validate_parameter(param_name: str, value: Any, level: ValidationLevel = ValidationLevel.STRICT) -> List[str]:
        """Validate a single parameter."""
        errors = []
        warnings_list = []
        
        # Check if parameter is required
        if level == ValidationLevel.STRICT:
            for preset, required in ConfigurationValidator.REQUIRED_PARAMS.items():
                if param_name in required:
                    errors.append(f"Parameter '{param_name}' is required for '{preset}' preset")
                    break
        
        # Check valid ranges
        if param_name in ConfigurationValidator.VALID_RANGES:
            min_val, max_val = ConfigurationValidator.VALID_RANGES[param_name]
            if not (min_val <= value <= max_val):
                error_msg = f"Parameter '{param_name}' ({value}) is outside valid range [{min_val}, {max_val}]"
                if level == ValidationLevel.STRICT:
                    errors.append(error_msg)
                else:
                    warnings_list.append(error_msg)
        
        # Check valid values for enums
        if param_name in ConfigurationValidator.VALID_VALUES:
            valid_values = ConfigurationValidator.VALID_VALUES[param_name]
            if value not in valid_values:
                error_msg = f"Parameter '{param_name}' value '{value}' is not valid. Valid values: {valid_values}"
                if level == ValidationLevel.STRICT:
                    errors.append(error_msg)
                else:
                    warnings_list.append(error_msg)
        
        # Type checking
        param_type = type(value)
        expected_types = {
            'neuron_count': int,
            'region_count': int,
            'simulation_timestep': float,
            'max_simulation_steps': int,
            'connection_probability': float,
            'initial_weight_mean': float,
            'initial_weight_std': float,
            'v_thresh': float,
            'v_rest': float,
            'v_reset': float,
            'tau_mem': float,
            'tau_ref': float,
            'stdp_learning_rate': float,
            'stdp_tau_plus': float,
            'stdp_tau_minus': float,
            'hebbian_learning_rate': float,
            'synaptogenesis_rate': float,
            'pruning_rate': float,
            'development_synaptogenesis_rate': float,
            'development_pruning_rate': float,
            'dopamine_baseline': float,
            'curiosity_threshold': float,
            'novelty_decay_rate': float,
            'reward_discount_factor': float,
            'reward_scaling': float,
            'environment_width': int,
            'environment_height': int,
            'vision_width': int,
            'vision_height': int,
            'vision_channels': int,
            'visualization_update_rate': float,
            'performance_sampling_rate': float,
            'energy_decay_rate': float,
            'health_threshold': float,
            'curiosity_exploration_rate': float,
        }
        
        if param_name in expected_types:
            expected_type = expected_types[param_name]
            if not isinstance(value, expected_type):
                type_error = f"Parameter '{param_name}' should be {expected_type.__name__}, got {param_type.__name__}"
                if level == ValidationLevel.STRICT:
                    errors.append(type_error)
                else:
                    warnings_list.append(type_error)
        
        # Special validation for boolean parameters
        bool_params = ['stdp_enable', 'hebbian_enable', 'structural_enable',
                       'curiosity_enable', 'enable_curiosity_driven_exploration',
                       'enable_hierarchical_planning', 'enable_metacognition',
                       'enable_transfer_learning', 'visualization_enabled',
                       'enable_performance_monitoring', 'enable_memory_profiling',
                       'enable_safety_checks', 'log_to_file', 'log_file_append']
        
        if param_name in bool_params and not isinstance(value, bool):
            bool_error = f"Parameter '{param_name}' should be boolean"
            if level == ValidationLevel.STRICT:
                errors.append(bool_error)
            else:
                warnings_list.append(bool_error)
        
        # Special validation for string parameters
        string_params = ['environment_name', 'log_level', 'log_filename',
                        'visualization_backend', 'environment_name']
        
        if param_name in string_params and not isinstance(value, str):
            str_error = f"Parameter '{param_name}' should be string"
            if level == ValidationLevel.STRICT:
                errors.append(str_error)
            else:
                warnings_list.append(str_error)
        
        # Correlation checks
        if param_name == 'curiosity_enable' and value and 'curiosity_threshold' not in ConfigurationValidator.REQUIRED_PARAMS:
            warnings_list.append("Curiosity enabled - ensure curiosity_threshold is set appropriately")
        
        if param_name == 'visualization_enabled' and value and 'visualization_backend' not in ConfigurationValidator.VALID_VALUES:
            warnings_list.append("Visualization enabled - ensure visualization_backend is set")
        
        return errors + warnings_list
    
    @classmethod
    def validate_configuration(cls, config: NMLEnvironmentConfig, level: ValidationLevel = ValidationLevel.STRICT) -> Dict[str, List[str]]:
        """Validate entire configuration."""
        results = {
            'errors': [],
            'warnings': [],
            'info': []
        }
        
        # Get all parameters from the config object
        for param_name in config.to_dict().keys():
            if hasattr(config, param_name):
                value = getattr(config, param_name)
                param_errors = cls.validate_parameter(param_name, value, level)
                
                # Categorize errors/warnings
                for error in param_errors:
                    if "outside valid range" in error or "should be" in error:
                        results['warnings'].append(error)
                    elif "not valid" in error or "required" in error:
                        results['errors'].append(error)
                    else:
                        results['info'].append(error)
        
        return results

class NMLEnhancedAPI:
    """Enhanced API for NLM with advanced features."""
    
    def __init__(self):
        self._config = None
        self._brain = None
        self._world = None
        self._agent = None
        self._simulation_mode = SimulationMode.CONTINUOUS
        self._current_step = 0
        self._performance_data = []
        self._simulation_active = False
        
    def create_enhanced_config(self, config_dict: Optional[Dict[str, Any]] = None, 
                              preset: str = "balanced",
                              validation_level: ValidationLevel = ValidationLevel.STRICT) -> NMLEnvironmentConfig:
        """Create enhanced configuration."""
        # Start with preset
        config = NMLEnvironmentConfig.from_preset(preset)
        
        # Override with user-provided values
        if config_dict:
            for key, value in config_dict.items():
                if hasattr(config, key):
                    setattr(config, key, value)
        
        # Validate configuration
        validation_results = ConfigurationValidator.validate_configuration(config, validation_level)
        
        if validation_results['errors']:
            raise ConfigValidationError(
                f"Configuration validation failed: {'; '.join(validation_results['errors'])}",
                error_code="CONFIG_VALIDATION_ERROR",
                details={'errors': validation_results['errors'], 'warnings': validation_results['warnings']}
            )
        
        if validation_results['warnings'] and validation_level == ValidationLevel.STRICT:
            warnings_messages = '; '.join(validation_results['warnings'])
            warnings.warn(f"Configuration warnings: {warnings_messages}", UserWarning)
        
        logger.info(f"Enhanced configuration created with preset: {preset}")
        return config
    
    def create_simulation_session(self, config: NMLEnvironmentConfig) -> Dict[str, Any]:
        """Create enhanced simulation session."""
        self._config = config
        
        # Import here to avoid circular imports
        try:
            import pynlm
        except ImportError:
            raise NLMError("pynlm module not found. Please install with: pip install pynlm")
        
        # Create components
        pynlm_config = pynlm.createDefaultConfig()
        
        # Map enhanced config to pynlm config
        if hasattr(config, 'neuron_count'):
            pynlm_config.set('brain.neuron_count', config.neuron_count)
        if hasattr(config, 'region_count'):
            pynlm_config.set('brain.region_count', config.region_count)
        if hasattr(config, 'simulation_timestep'):
            pynlm_config.set('simulation.timestep', config.simulation_timestep)
        if hasattr(config, 'connection_probability'):
            pynlm_config.set('brain.connection_probability', config.connection_probability)
        
        # Create brain
        self._brain = pynlm.createBrain(pynlm_config)
        self._brain.initialize()
        
        # Create world
        self._world = pynlm.createSimpleWorld()
        self._world.configure(
            config.environment_width, config.environment_height,
            config.vision_width, config.vision_height
        )
        self._world.reset()
        
        # Create agent
        self._agent = pynlm.createAgentBrain(self._brain)
        self._agent.initialize(self._world)
        
        # Enable features based on configuration
        if hasattr(config, 'enable_curiosity_driven_exploration'):
            self._agent.enableCuriosity(config.enable_curiosity_driven_exploration)
        
        if hasattr(config, 'enable_hierarchical_planning'):
            # Note: This is a placeholder - actual implementation would depend on NLM features
            pass
        
        if hasattr(config, 'enable_metacognition'):
            # Note: This is a placeholder - actual implementation would depend on NLM features
            pass
        
        logger.info(f"Enhanced simulation session created with {config.neuron_count} neurons")
        
        return {
            'brain': self._brain,
            'world': self._world,
            'agent': self._agent,
            'config': config,
            'step': 0,
            'simulation_time': 0.0,
            'performance_data': []
        }
    
    def run_enhanced_simulation(self, num_steps: int, config: Optional[NMLEnvironmentConfig] = None) -> Dict[str, Any]:
        """Run enhanced simulation."""
        if not self._brain or not self._world or not self._agent:
            raise SimulationError("No active simulation session. Create one first.")
        
        if config is None:
            config = self._config
        
        logger.info(f"Starting enhanced simulation for {num_steps} steps")
        
        # Initialize performance tracking
        start_time = time.time()
        self._performance_data = []
        
        for step in range(num_steps):
            self._current_step = step
            
            # Pre-step callback
            if hasattr(config, 'pre_step_callback') and config.pre_step_callback:
                try:
                    config.pre_step_callback(step, self._world, self._brain, self._agent)
                except Exception as e:
                    logger.warning(f"Pre-step callback failed: {e}")
            
            # Update world
            world_timestep = config.simulation_timestep if hasattr(config, 'simulation_timestep') else 0.1
            self._world.update(world_timestep)
            
            # Get sensory input
            percept = self._world.getSensoryPercept()
            
            # Process sensory input
            self._agent.processSensoryInput(percept)
            
            # Run brain step
            self._brain.step(step)
            
            # Decode motor command
            motor_cmd = self._agent.decodeMotorCommand()
            
            # Apply motor command
            self._world.applyMotorCommand(motor_cmd, self._world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if hasattr(config, 'reward_callback') and config.reward_callback:
                try:
                    reward = config.reward_callback(step, self._world, self._brain, self._agent)
                except Exception as e:
                    logger.warning(f"Reward callback failed: {e}")
            else:
                # Default reward from world state
                internal_signals = percept.getInternal() if hasattr(percept, 'getInternal') else []
                reward = sum(internal_signals) * 0.01 if internal_signals else 0.0
            
            predicted_reward = 0.0  # Placeholder for prediction
            self._agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            if hasattr(config, 'update_development') and callable(getattr(self._agent, 'updateDevelopment', None)):
                try:
                    self._agent.updateDevelopment(world_timestep)
                except Exception as e:
                    logger.warning(f"Development update failed: {e}")
            
            # Post-step callback
            if hasattr(config, 'post_step_callback') and config.post_step_callback:
                try:
                    config.post_step_callback(step, self._world, self._brain, self._agent, reward)
                except Exception as e:
                    logger.warning(f"Post-step callback failed: {e}")
            
            # Novelty callback
            if hasattr(config, 'novelty_callback') and config.novelty_callback:
                try:
                    novelty = config.novelty_callback(step, self._world, self._brain, self._agent)
                except Exception as e:
                    logger.warning(f"Novelty callback failed: {e}")
            
            # Collect performance data
            if hasattr(config, 'enable_performance_monitoring') and config.enable_performance_monitoring:
                performance_metrics = self._collect_performance_metrics(step)
                self._performance_data.append(performance_metrics)
            
            # Log progress periodically
            if step % 100 == 0:
                self._log_simulation_progress(step, num_steps, reward)
        
        # Calculate final statistics
        end_time = time.time()
        simulation_time = end_time - start_time
        
        session_stats = self._calculate_session_statistics()
        
        logger.info(f"Enhanced simulation completed in {simulation_time:.2f} seconds")
        
        return {
            'final_step': self._current_step,
            'simulation_time': simulation_time,
            'final_brain_stats': self._get_brain_statistics(),
            'session_stats': session_stats,
            'performance_data': self._performance_data,
            'total_neurons': self._brain.getTotalNeuronCount() if self._brain else 0,
            'total_synapses': self._brain.getTotalSynapseCount() if self._brain else 0,
            'total_spikes': self._brain.getTotalSpikeCount() if self._brain else 0,
            'firing_neurons': self._brain.getFiringNeuronCount() if self._brain else 0,
        }
    
    def _collect_performance_metrics(self, step: int) -> Dict[str, Any]:
        """Collect performance metrics."""
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'brain_stats': {}
        }
        
        if self._brain:
            metrics['brain_stats'] = {
                'total_neurons': self._brain.getTotalNeuronCount(),
                'total_synapses': self._brain.getTotalSynapseCount(),
                'total_spikes': self._brain.getTotalSpikeCount(),
                'firing_neurons': self._brain.getFiringNeuronCount(),
                'average_firing_rate': self._brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            }
        
        if self._world:
            metrics['world_stats'] = {
                'simulation_time': self._world.getSimulationTime(),
                'agent_position': (self._world.getAgentBody().x, self._world.getAgentBody().y),
                'agent_energy': self._world.getAgentBody().energy,
                'agent_health': self._world.getAgentBody().health,
            }
        
        if self._agent:
            metrics['agent_stats'] = {
                'curiosity_level': self._agent.getCuriosityLevel(),
                'novelty_level': self._agent.getNoveltyLevel(),
                'prediction_error': self._agent.getPredictionError(),
                'neuromodulation_level': self._agent.getNeuromodulationLevel(),
                'developmental_stage': self._agent.getDevelopmentalStage(),
            }
        
        return metrics
    
    def _calculate_session_statistics(self) -> Dict[str, Any]:
        """Calculate session-level statistics."""
        if not self._performance_data:
            return {}
        
        stats = {
            'total_steps': len(self._performance_data),
            'avg_firing_rate': 0.0,
            'max_firing_rate': 0.0,
            'avg_spikes': 0.0,
            'max_spikes': 0.0,
            'avg_curiosity': 0.0,
            'max_curiosity': 0.0,
            'final_energy': 0.0,
            'health_trajectory': []
        }
        
        # Calculate averages
        total_firing = sum(m['brain_stats'].get('average_firing_rate', 0.0) 
                          for m in self._performance_data)
        total_spikes = sum(m['brain_stats'].get('total_spikes', 0) 
                          for m in self._performance_data)
        total_curiosity = sum(m.get('agent_stats', {}).get('curiosity_level', 0.0) 
                             for m in self._performance_data)
        
        if self._performance_data:
            stats['avg_firing_rate'] = total_firing / len(self._performance_data)
            stats['max_firing_rate'] = max(m['brain_stats'].get('average_firing_rate', 0.0) 
                                          for m in self._performance_data)
            stats['avg_spikes'] = total_spikes / len(self._performance_data)
            stats['max_spikes'] = max(m['brain_stats'].get('total_spikes', 0) 
                                     for m in self._performance_data)
            stats['avg_curiosity'] = total_curiosity / len(self._performance_data)
            stats['max_curiosity'] = max(m.get('agent_stats', {}).get('curiosity_level', 0.0) 
                                        for m in self._performance_data)
            stats['final_energy'] = self._performance_data[-1].get('world_stats', {}).get('agent_energy', 0.0)
            stats['health_trajectory'] = [m.get('world_stats', {}).get('agent_health', 0.0) 
                                         for m in self._performance_data]
        
        return stats
    
    def _log_simulation_progress(self, step: int, total_steps: int, reward: float) -> None:
        """Log simulation progress."""
        progress = (step / total_steps) * 100
        
        if self._brain and self._agent:
            logger.info(
                f"Step {step}/{total_steps} ({progress:.1f}%): "
                f"Neurons firing: {self._brain.getFiringNeuronCount()}, "
                f"Total spikes: {self._brain.getTotalSpikeCount()}, "
                f"Curiosity: {self._agent.getCuriosityLevel():.3f}, "
                f"Reward: {reward:.3f}"
            )
        else:
            logger.info(f"Step {step}/{total_steps} ({progress:.1f}%): Reward: {reward:.3f}")
    
    def _get_brain_statistics(self) -> Dict[str, Any]:
        """Get current brain statistics."""
        if not self._brain or not self._agent:
            return {}
        
        return {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'developmental_stage': self._agent.getDevelopmentalStage(),
            'curiosity_level': self._agent.getCuriosityLevel(),
            'novelty_level': self._agent.getNoveltyLevel(),
            'prediction_error': self._agent.getPredictionError(),
            'neuromodulation_level': self._agent.getNeuromodulationLevel(),
        }
    
    def save_session(self, filepath: str) -> bool:
        """Save simulation session."""
        try:
            # Create a simple JSON file with session data
            session_data = {
                'config': self._config.to_dict() if self._config else {},
                'brain_stats': self._get_brain_statistics(),
                'performance_data': self._performance_data,
                'final_step': self._current_step,
                'timestamp': time.time(),
            }
            
            import json
            with open(filepath, 'w') as f:
                json.dump(session_data, f, indent=2)
            
            logger.info(f"Session saved to: {filepath}")
            return True
        except Exception as e:
            logger.error(f"Failed to save session: {e}")
            return False
    
    def load_session(self, filepath: str) -> bool:
        """Load simulation session."""
        try:
            import json
            with open(filepath, 'r') as f:
                session_data = json.load(f)
            
            # Restore configuration
            if 'config' in session_data:
                self._config = NMLEnvironmentConfig()
                for key, value in session_data['config'].items():
                    if hasattr(self._config, key):
                        setattr(self._config, key, value)
            
            # Restore performance data
            self._performance_data = session_data.get('performance_data', [])
            self._current_step = session_data.get('final_step', 0)
            
            logger.info(f"Session loaded from: {filepath}")
            return True
        except Exception as e:
            logger.error(f"Failed to load session: {e}")
            return False

# Global enhanced API instance
_enhanced_api = NMLEnhancedAPI()

def create_config(**kwargs) -> NMLEnvironmentConfig:
    """Create enhanced configuration."""
    preset = kwargs.pop('preset', 'balanced')
    validation_level = kwargs.pop('validation_level', ValidationLevel.STRICT)
    
    return _enhanced_api.create_enhanced_config(kwargs, preset, validation_level)

def create_session(config: Optional[NMLEnvironmentConfig] = None) -> Dict[str, Any]:
    """Create enhanced simulation session."""
    if config is None:
        config = create_config()
    
    return _enhanced_api.create_simulation_session(config)

def run_simulation(num_steps: int, config: Optional[NMLEnvironmentConfig] = None) -> Dict[str, Any]:
    """Run enhanced simulation."""
    if config is not None:
        _enhanced_api.create_simulation_session(config)
    
    return _enhanced_api.run_enhanced_simulation(num_steps, config)

def save_session(filepath: str) -> bool:
    """Save current simulation session."""
    return _enhanced_api.save_session(filepath)

def load_session(filepath: str) -> bool:
    """Load simulation session."""
    return _enhanced_api.load_session(filepath)

def get_current_session() -> Optional[Dict[str, Any]]:
    """Get current simulation session state."""
    if not _enhanced_api._brain:
        return None
    
    return {
        'brain': _enhanced_api._brain,
        'world': _enhanced_api._world,
        'agent': _enhanced_api._agent,
        'config': _enhanced_api._config,
        'step': _enhanced_api._current_step,
        'performance_data': _enhanced_api._performance_data,
    }

# Export main classes and functions
__all__ = [
    'NMLEnvironmentConfig',
    'ValidationLevel',
    'SimulationMode',
    'NLMError',
    'ConfigValidationError',
    'SimulationError',
    'PerformanceError',
    'ConfigurationValidator',
    'NMLEnhancedAPI',
    'create_config',
    'create_session',
    'run_simulation',
    'save_session',
    'load_session',
    'get_current_session',
    'NMLErrorDetails',
]

# Example usage
if __name__ == "__main__":
    # Example 1: Create and run minimal simulation
    print("=== Example 1: Minimal Simulation ===")
    config = create_config(preset='minimal')
    session = create_session(config)
    result = run_simulation(100, config)
    print(f"Simulation completed: {result['total_steps']} steps")
    
    # Example 2: Create and run balanced simulation
    print("\n=== Example 2: Balanced Simulation ===")
    config = create_config(preset='balanced')
    session = create_session(config)
    result = run_simulation(500, config)
    print(f"Simulation completed: {result['total_steps']} steps")
    print(f"Final firing rate: {result['final_brain_stats']['average_firing_rate']:.3f}")
    
    # Example 3: Save and load session
    print("\n=== Example 3: Session Persistence ===")
    save_session("nlm_session.json")
    print("Session saved")
    
    # Clear and reload
    # result = run_simulation(100)
    # load_session("nlm_session.json")
    # print("Session reloaded")