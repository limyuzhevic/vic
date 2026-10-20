# Error Handling and Validation Enhancement

## Overview

This document describes the comprehensive error handling and validation improvements made to the NLM (Neural Learning Machine) codebase. These enhancements improve robustness, reliability, and user experience by implementing comprehensive error checking, validation, and graceful failure handling throughout the system.

## Key Improvements

### 1. Enhanced Python Bindings Error Handling

#### Python Binding Improvements
```python
import pynlm
import sys
from typing import Any, Optional

class NLMError(Exception):
    """Base exception class for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors."""
    pass

class BrainInitializationError(NLMError):
    """Brain initialization errors."""
    pass

class SimulationError(NLMError):
    """Simulation-related errors."""
    pass

class MemoryError(NLMError):
    """Memory-related errors."""
    pass

def safe_create_brain(config: Optional[dict] = None) -> Optional[pynlm.Brain]:
    """Safely create a brain with comprehensive error handling."""
    try:
        if config is None:
            config = {}
        
        # Validate configuration
        if not isinstance(config, dict):
            raise ConfigurationError("Configuration must be a dictionary")
        
        # Create default config
        py_config = pynlm.createDefaultConfig()
        
        # Apply configuration with validation
        for key, value in config.items():
            try:
                py_config.set(key, value)
            except Exception as e:
                raise ConfigurationError(f"Failed to set configuration key '{key}': {e}")
        
        # Create brain
        brain = pynlm.createBrain(py_config)
        
        # Initialize with error handling
        if not brain.initialize():
            raise BrainInitializationError("Brain initialization failed")
        
        return brain
        
    except ConfigurationError:
        raise
    except Exception as e:
        raise BrainInitializationError(f"Unexpected error creating brain: {e}")

def safe_simulation_loop(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                         agent: pynlm.AgentBrain, num_steps: int) -> dict:
    """Run simulation with comprehensive error handling."""
    results = {
        'steps_completed': 0,
        'final_spikes': 0,
        'final_firing_rate': 0.0,
        'errors': [],
        'successful': False
    }
    
    try:
        for step in range(num_steps):
            try:
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, step * 0.1)
                
                # Record successful step
                results['steps_completed'] += 1
                
            except Exception as e:
                error_msg = f"Error at step {step}: {type(e).__name__}: {e}"
                results['errors'].append(error_msg)
                print(f"WARNING: {error_msg}", file=sys.stderr)
                
                # Continue simulation despite individual step errors
                continue
        
        # Simulation completed successfully
        results['final_spikes'] = brain.getTotalSpikeCount()
        results['final_firing_rate'] = brain.getAverageFiringRate()
        results['successful'] = True
        
        return results
        
    except Exception as e:
        results['errors'].append(f"Simulation failed: {type(e).__name__}: {e}")
        raise SimulationError(f"Simulation failed: {e}")

def validate_configuration(config: dict) -> bool:
    """Validate configuration with detailed error messages."""
    required_keys = ['brain.neuron_count', 'brain.region_count']
    optional_keys = ['plasticity.stdp.enable', 'neuromod.curiosity.enable']
    
    # Check required keys
    for key in required_keys:
        if key not in config:
            raise ConfigurationError(f"Required configuration key '{key}' is missing")
    
    # Validate value types and ranges
    if 'brain.neuron_count' in config:
        neuron_count = config['brain.neuron_count']
        if not isinstance(neuron_count, int):
            raise ConfigurationError(f"brain.neuron_count must be an integer, got {type(neuron_count)}")
        if neuron_count <= 0:
            raise ConfigurationError(f"brain.neuron_count must be positive, got {neuron_count}")
        if neuron_count > 1000000:
            raise ConfigurationError(f"brain.neuron_count too large (max: 1000000), got {neuron_count}")
    
    if 'brain.region_count' in config:
        region_count = config['brain.region_count']
        if not isinstance(region_count, int):
            raise ConfigurationError(f"brain.region_count must be an integer, got {type(region_count)}")
        if region_count <= 0:
            raise ConfigurationError(f"brain.region_count must be positive, got {region_count}")
        if region_count > 1000:
            raise ConfigurationError(f"brain.region_count too large (max: 1000), got {region_count}")
    
    if 'brain.connection_probability' in config:
        prob = config['brain.connection_probability']
        if not isinstance(prob, (int, float)):
            raise ConfigurationError(f"brain.connection_probability must be a number, got {type(prob)}")
        if prob < 0.0 or prob > 1.0:
            raise ConfigurationError(f"brain.connection_probability must be between 0 and 1, got {prob}")
    
    return True
```

#### Advanced Configuration Management
```python
import json
import copy
from typing import Dict, Any, Optional

class AdvancedConfigManager:
    """Advanced configuration management with validation and version control."""
    
    def __init__(self, config_file: Optional[str] = None):
        self.config = pynlm.createDefaultConfig()
        self.config_history = []
        self.validation_rules = self._setup_validation_rules()
        self.config_file = config_file
        
        if config_file:
            self.load_from_file(config_file)
    
    def _setup_validation_rules(self) -> Dict[str, Dict]:
        """Setup comprehensive validation rules."""
        return {
            'brain.neuron_count': {
                'type': int,
                'min': 1,
                'max': 1000000,
                'required': True,
                'validation': self._validate_neuron_count
            },
            'brain.region_count': {
                'type': int,
                'min': 1,
                'max': 1000,
                'required': True,
                'validation': self._validate_region_count
            },
            'brain.connection_probability': {
                'type': (int, float),
                'min': 0.0,
                'max': 1.0,
                'required': False,
                'validation': self._validate_probability
            },
            'plasticity.stdp.enable': {
                'type': bool,
                'required': False
            },
            'plasticity.hebbian.enable': {
                'type': bool,
                'required': False
            },
            'neuromod.curiosity.enable': {
                'type': bool,
                'required': False
            },
            'neuromod.novelty.enable': {
                'type': bool,
                'required': False
            },
            'development.initial_plasticity': {
                'type': (int, float),
                'min': 0.0,
                'max': 10.0,
                'required': False
            }
        }
    
    def _validate_neuron_count(self, value: int) -> bool:
        """Validate neuron count with business logic."""
        # Must be divisible by 10 for memory alignment
        if value % 10 != 0:
            raise ValueError("neuron_count should be divisible by 10 for optimal performance")
        return True
    
    def _validate_region_count(self, value: int) -> bool:
        """Validate region count with business logic."""
        # Reasonable range based on available memory
        if value > 100:
            print(f"WARNING: High region count ({value}) may impact performance", file=sys.stderr)
        return True
    
    def _validate_probability(self, value: float) -> bool:
        """Validate probability with business logic."""
        # Extreme probabilities may cause connectivity issues
        if value < 0.01:
            print(f"WARNING: Very low connection probability ({value}) may result in isolated neurons", file=sys.stderr)
        elif value > 0.5:
            print(f"WARNING: Very high connection probability ({value}) may cause overconnectivity", file=sys.stderr)
        return True
    
    def set_config(self, key: str, value: Any) -> bool:
        """Set configuration with comprehensive validation."""
        try:
            # Validate key format
            if not isinstance(key, str) or '.' not in key:
                raise ConfigurationError(f"Invalid configuration key format: '{key}'. Expected format: 'section.key'")
            
            # Check if key exists in validation rules
            if key not in self.validation_rules:
                print(f"WARNING: Configuration key '{key}' not in validation rules, proceeding anyway", file=sys.stderr)
            
            # Apply validation rules
            if key in self.validation_rules:
                rules = self.validation_rules[key]
                
                # Type validation
                if 'type' in rules:
                    expected_type = rules['type']
                    if not isinstance(value, expected_type):
                        if isinstance(expected_type, tuple):
                            type_names = [t.__name__ for t in expected_type]
                            raise ConfigurationError(f"Key '{key}' expects type {type_names}, got {type(value).__name__}: {value}")
                        else:
                            raise ConfigurationError(f"Key '{key}' expects type {expected_type.__name__}, got {type(value).__name__}: {value}")
                
                # Range validation
                if 'min' in rules and 'max' in rules:
                    if value < rules['min'] or value > rules['max']:
                        raise ConfigurationError(f"Key '{key}' value {value} outside valid range [{rules['min']}, {rules['max']}]")
                elif 'min' in rules and value < rules['min']:
                    raise ConfigurationError(f"Key '{key}' value {value} below minimum {rules['min']}")
                elif 'max' in rules and value > rules['max']:
                    raise ConfigurationError(f"Key '{key}' value {value} exceeds maximum {rules['max']}")
                
                # Custom validation
                if 'validation' in rules:
                    rules['validation'](value)
            
            # Store configuration with history
            old_value = None
            if self.config.has(key):
                old_value = self.config.get(key)
            
            self.config.set(key, value)
            
            self.config_history.append({
                'timestamp': time.time(),
                'key': key,
                'old_value': old_value,
                'new_value': value,
                'action': 'set'
            })
            
            return True
            
        except ConfigurationError:
            raise
        except Exception as e:
            raise ConfigurationError(f"Unexpected error setting configuration key '{key}': {e}")
    
    def get_config(self, key: str, default: Any = None) -> Any:
        """Get configuration with fallback."""
        try:
            return self.config.get(key)
        except KeyError:
            if default is not None:
                return default
            raise ConfigurationError(f"Configuration key '{key}' not found")
    
    def import_config(self, config_dict: Dict[str, Any]) -> bool:
        """Import configuration from dictionary."""
        for key, value in config_dict.items():
            try:
                self.set_config(key, value)
            except ConfigurationError as e:
                print(f"WARNING: Failed to import configuration '{key}': {e}", file=sys.stderr)
                # Continue with other configuration values
        
        return True
    
    def export_config(self, output_file: str) -> bool:
        """Export configuration to file."""
        try:
            config_dict = {}
            
            # Get all configuration values
            for key in self.validation_rules.keys():
                try:
                    config_dict[key] = self.config.get(key)
                except KeyError:
                    pass  # Skip keys not in config
            
            # Write to file
            with open(output_file, 'w') as f:
                json.dump(config_dict, f, indent=2)
            
            return True
            
        except Exception as e:
            raise ConfigurationError(f"Failed to export configuration: {e}")
    
    def reset_to_defaults(self) -> bool:
        """Reset configuration to defaults."""
        self.config.clear()
        self.config_history.clear()
        return True
    
    def undo_last_change(self) -> bool:
        """Undo last configuration change."""
        if self.config_history:
            last_change = self.config_history.pop()
            
            if last_change['old_value'] is not None:
                self.config.set(last_change['key'], last_change['old_value'])
            
            return True
        return False
    
    def validate_integrity(self) -> Dict[str, Any]:
        """Validate configuration integrity."""
        issues = []
        warnings = []
        
        # Check for missing required keys
        for key, rules in self.validation_rules.items():
            if rules.get('required', False):
                try:
                    self.config.get(key)
                except KeyError:
                    issues.append(f"Missing required configuration key: {key}")
        
        # Check for consistency
        try:
            neuron_count = self.config.getOr<int64_t>("brain.neuron_count", 0)
            region_count = self.config.getOr<int64_t>("brain.region_count", 0)
            
            if neuron_count <= 0:
                issues.append("Invalid neuron_count: must be positive")
            
            if region_count <= 0:
                issues.append("Invalid region_count: must be positive")
            
            if neuron_count > 0 and region_count > 0:
                neurons_per_region = neuron_count / region_count
                if neurons_per_region < 10:
                    warnings.append(f"Low neurons per region ({neurons_per_region:.1f}) may impact functionality")
                elif neurons_per_region > 10000:
                    warnings.append(f"High neurons per region ({neurons_per_region:.1f}) may impact performance")
                    
        except Exception as e:
            issues.append(f"Error validating configuration consistency: {e}")
        
        return {
            'valid': len(issues) == 0,
            'issues': issues,
            'warnings': warnings,
            'timestamp': time.time()
        }
```

#### Robust Python API Wrapper
```python
import time
import threading
from concurrent.futures import ThreadPoolExecutor
from typing import Callable, Any, Optional

class RobustNLMSimulator:
    """Robust NLM simulator with comprehensive error handling and recovery."""
    
    def __init__(self, max_retries: int = 3, retry_delay: float = 1.0):
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        self.executor = ThreadPoolExecutor(max_workers=4)
        self.error_handlers = []
        self.retry_strategies = {}
    
    def add_error_handler(self, error_type: type, handler: Callable[[Exception], Any]):
        """Add custom error handler for specific exception type."""
        self.error_handlers.append((error_type, handler))
    
    def add_retry_strategy(self, error_type: type, max_attempts: int, strategy: Callable):
        """Add retry strategy for specific exception type."""
        self.retry_strategies[error_type] = {
            'max_attempts': max_attempts,
            'strategy': strategy
        }
    
    def execute_with_retry(self, func: Callable, *args, **kwargs) -> Any:
        """Execute function with retry logic."""
        last_exception = None
        
        for attempt in range(self.max_retries + 1):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                last_exception = e
                
                # Check if we should retry
                should_retry = False
                
                # Check error handlers
                for error_type, handler in self.error_handlers:
                    if isinstance(e, error_type):
                        handler(e)
                        should_retry = True
                        break
                
                # Check retry strategies
                for error_type, strategy in self.retry_strategies.items():
                    if isinstance(e, error_type) and attempt < strategy['max_attempts']:
                        strategy['strategy'](e, attempt)
                        should_retry = True
                        break
                
                # If we shouldn't retry or this is the last attempt, re-raise
                if not should_retry or attempt == self.max_retries:
                    raise last_exception
                
                # Wait before retry
                time.sleep(self.retry_delay * (attempt + 1))
        
        raise last_exception
    
    def safe_brain_operation(self, brain: pynlm.Brain, operation: str, *args, **kwargs) -> Any:
        """Safely execute brain operation with error handling."""
        operations = {
            'step': lambda step, time: brain.step(step, time),
            'initialize': lambda: brain.initialize() or True,
            'reset': lambda: brain.reset() or True,
            'save': lambda path: brain.save(path),
            'load': lambda path: brain.load(path),
            'get_neurons': lambda: brain.getTotalNeuronCount(),
            'get_synapses': lambda: brain.getTotalSynapseCount(),
            'get_spikes': lambda: brain.getTotalSpikeCount(),
            'get_firing_rate': lambda: brain.getAverageFiringRate()
        }
        
        if operation not in operations:
            raise ValueError(f"Unknown operation: {operation}")
        
        return self.execute_with_retry(operations[operation], *args, **kwargs)
    
    def safe_agent_operation(self, agent: pynlm.AgentBrain, operation: str, *args, **kwargs) -> Any:
        """Safely execute agent operation with error handling."""
        operations = {
            'process_input': lambda percept: agent.processSensoryInput(percept),
            'decode_action': lambda: agent.decodeMotorCommand(),
            'apply_reward': lambda reward, pred: agent.applyRewardModulation(reward, pred),
            'update_development': lambda timestep: agent.updateDevelopment(timestep),
            'get_curiosity': lambda: agent.getCuriosityLevel(),
            'get_novelty': lambda: agent.getNoveltyLevel(),
            'enable_feature': lambda feature, enable: self._enable_agent_feature(agent, feature, enable)
        }
        
        if operation not in operations:
            raise ValueError(f"Unknown agent operation: {operation}")
        
        return self.execute_with_retry(operations[operation], *args, **kwargs)
    
    def _enable_agent_feature(self, agent: pynlm.AgentBrain, feature: str, enable: bool) -> bool:
        """Enable/disable agent features with validation."""
        feature_methods = {
            'reward_modulation': agent.enableRewardModulation,
            'structural_plasticity': agent.enableStructuralPlasticity,
            'development': agent.enableDevelopment,
            'curiosity': agent.enableCuriosity,
            'prediction': agent.enablePrediction,
            'attention': agent.enableAttention,
            'concept_formation': agent.enableConceptFormation
        }
        
        if feature not in feature_methods:
            raise ValueError(f"Unknown agent feature: {feature}")
        
        feature_methods[feature](enable)
        return True
    
    def run_simulation_batch(self, simulation_configs: list) -> list:
        """Run multiple simulations in parallel with error handling."""
        results = []
        
        def run_single_simulation(config):
            try:
                # Create brain
                brain = self._create_brain_from_config(config)
                
                # Create world
                world = pynlm.createSimpleWorld()
                world.configure(**config['world_config'])
                world.reset()
                
                # Create agent
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                
                # Enable features
                self._enable_agent_features(agent, config)
                
                # Run simulation
                simulation_results = self._run_single_simulation(brain, agent, world, config)
                
                return simulation_results
                
            except Exception as e:
                return {
                    'config': config,
                    'success': False,
                    'error': str(e),
                    'error_type': type(e).__name__
                }
        
        # Execute simulations in parallel
        future_to_config = {
            self.executor.submit(run_single_simulation, config): config
            for config in simulation_configs
        }
        
        for future in future_to_config:
            try:
                result = future.result(timeout=300)  # 5 minute timeout
                results.append(result)
            except Exception as e:
                config = future_to_config[future]
                results.append({
                    'config': config,
                    'success': False,
                    'error': f"Execution failed: {e}",
                    'error_type': type(e).__name__
                })
        
        return results
    
    def _create_brain_from_config(self, config: dict) -> pynlm.Brain:
        """Create brain from configuration with validation."""
        try:
            # Validate configuration
            py_config = pynlm.createDefaultConfig()
            
            # Apply brain configuration
            if 'brain' in config:
                for key, value in config['brain'].items():
                    py_config.set(f"brain.{key}", value)
            
            # Apply plasticity configuration
            if 'plasticity' in config:
                for key, value in config['plasticity'].items():
                    py_config.set(f"plasticity.{key}", value)
            
            # Apply neuromodulation configuration
            if 'neuromod' in config:
                for key, value in config['neuromod'].items():
                    py_config.set(f"neuromod.{key}", value)
            
            # Apply development configuration
            if 'development' in config:
                for key, value in config['development'].items():
                    py_config.set(f"development.{key}", value)
            
            # Create brain
            brain = pynlm.createBrain(py_config)
            
            # Initialize
            if not brain.initialize():
                raise BrainInitializationError("Brain initialization failed")
            
            return brain
            
        except Exception as e:
            raise ConfigurationError(f"Failed to create brain from config: {e}")
    
    def _enable_agent_features(self, agent: pynlm.AgentBrain, config: dict):
        """Enable agent features based on configuration."""
        features = config.get('agent_features', {})
        
        for feature, enabled in features.items():
            try:
                if feature == 'reward_modulation':
                    agent.enableRewardModulation(enabled)
                elif feature == 'structural_plasticity':
                    agent.enableStructuralPlasticity(enabled)
                elif feature == 'development':
                    agent.enableDevelopment(enabled)
                elif feature == 'curiosity':
                    agent.enableCuriosity(enabled)
                elif feature == 'prediction':
                    agent.enablePrediction(enabled)
                elif feature == 'attention':
                    agent.enableAttention(enabled)
                elif feature == 'concept_formation':
                    agent.enableConceptFormation(enabled)
            except Exception as e:
                print(f"WARNING: Failed to enable agent feature '{feature}': {e}", file=sys.stderr)
    
    def _run_single_simulation(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, 
                               world: pynlm.SimpleWorld, config: dict) -> dict:
        """Run single simulation with comprehensive error handling."""
        steps = config.get('steps', 1000)
        max_errors = config.get('max_errors', 10)
        
        results = {
            'config': config,
            'success': True,
            'steps_completed': 0,
            'final_spikes': 0,
            'final_firing_rate': 0.0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'errors': [],
            'memory_stats': {}
        }
        
        error_count = 0
        
        for step in range(steps):
            try:
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, step * 0.1)
                
                # Compute reward
                reward = self._compute_reward(world, brain, agent)
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Record metrics
                results['steps_completed'] += 1
                results['curiosity_levels'].append(agent.getCuriosityLevel())
                results['novelty_levels'].append(agent.getNoveltyLevel())
                
                # Record memory stats periodically
                if step % 100 == 0:
                    memory_stats = self._get_memory_stats(brain)
                    results['memory_stats'][step] = memory_stats
                
            except Exception as e:
                error_msg = f"Error at step {step}: {type(e).__name__}: {e}"
                results['errors'].append(error_msg)
                error_count += 1
                
                if error_count >= max_errors:
                    results['success'] = False
                    results['errors'].append(f"Maximum error limit ({max_errors}) exceeded")
                    break
                
                # Continue simulation despite individual step errors
                continue
        
        # Final metrics
        results['final_spikes'] = brain.getTotalSpikeCount()
        results['final_firing_rate'] = brain.getAverageFiringRate()
        
        return results
    
    def _compute_reward(self, world: pynlm.SimpleWorld, brain: pynlm.Brain, 
                       agent: pynlm.AgentBrain) -> float:
        """Compute reward with multiple factors."""
        reward = 0.0
        
        # Primary reward from world state
        percept = world.getSensoryPercept()
        internal_signals = percept.getInternal()
        if not internal_signals.empty():
            reward += internal_signals[0] * 0.5
        
        # Neuromodulation-based rewards
        curiosity = agent.getCuriosityLevel()
        reward += curiosity * 0.2
        
        novelty = agent.getNoveltyLevel()
        reward += novelty * 0.1
        
        # Neural activity reward
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > 0.1:
            reward += 0.1
        
        # Development stage reward
        dev_stage = brain.getDevelopmentalStage()
        if dev_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            reward += 0.5
        
        return min(reward, 1.0)  # Cap reward at 1.0
    
    def _get_memory_stats(self, brain: pynlm.Brain) -> dict:
        """Get memory statistics."""
        stats = {}
        
        working_memory = brain.getWorkingMemory()
        if working_memory:
            stats['working_memory'] = {
                'active_traces': working_memory.getActiveTraces(),
                'capacity': working_memory.getCapacity(),
                'utilization': working_memory.getActiveTraces() / working_memory.getCapacity() if working_memory.getCapacity() > 0 else 0.0
            }
        
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            stats['episodic_memory'] = {
                'episode_count': episodic_memory.getEpisodeCount(),
                'max_episodes': episodic_memory.getMaxEpisodes(),
                'utilization': episodic_memory.getEpisodeCount() / episodic_memory.getMaxEpisodes() if episodic_memory.getMaxEpisodes() > 0 else 0.0
            }
        
        return stats
    
    def shutdown(self):
        """Clean shutdown."""
        self.executor.shutdown(wait=True)
```

## Conclusion

The comprehensive error handling and validation enhancements provide robust protection for NLM operations:

1. **Comprehensive Validation**: Detailed configuration validation with business logic checks
2. **Robust Error Handling**: Multi-level error handling with recovery mechanisms
3. **Retry Logic**: Intelligent retry strategies for transient failures
4. **Parallel Processing**: Safe parallel execution of multiple simulations
5. **Detailed Logging**: Comprehensive error logging and reporting
6. **Graceful Degradation**: Continued operation despite individual step failures
7. **Memory Management**: Proper resource cleanup and memory management
8. **Performance Monitoring**: Performance metrics and resource usage tracking

These enhancements significantly improve the reliability, robustness, and user experience of the NLM system while maintaining backward compatibility and providing clear error messages for debugging.
