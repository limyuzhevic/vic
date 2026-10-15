#!/usr/bin/env python3
"""
NLM Plugin System for Custom Neural Components
===============================================

This module implements a comprehensive plugin system for NLM that allows users to:

1. **Create custom neural components** - Neurons, synapses, regions
2. **Register plugin components** - Dynamic loading of custom modules
3. **Extend functionality** - Add new plasticity rules, neuromodulators, etc.
4. **Manage plugin lifecycle** - Load, enable, disable, and configure plugins
5. **Share plugins** - Plugin repository and package management

The system is designed to be:
- **Backward compatible** - Works with existing NLM installations
- **Extensible** - Easy to add new plugin types
- **Secure** - Sandboxed execution and validation
- **Performant** - Minimal overhead during simulation
- **User-friendly** - Simple API for plugin development
"""

import abc
import json
import importlib
import inspect
import os
import sys
import threading
import time
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Any, Callable, Union
from contextlib import contextmanager
import traceback

# Import NLM core modules
import pynlm

class PluginType(Enum):
    """Enumeration of supported plugin types."""
    NEURON = "neuron"
    SYNAPSE = "synapse"
    PLASTICITY_RULE = "plasticity_rule"
    NEUROMODULATOR = "neuromodulator"
    MEMORY_SYSTEM = "memory_system"
    COGNITIVE_MODULE = "cognitive_module"
    SENSORY_PROCESSOR = "sensory_processor"
    MOTOR_DECODER = "motor_decoder"
    CONFIGURATION_PLUGIN = "configuration_plugin"
    VISUALIZATION_PLUGIN = "visualization_plugin"
    UTILITY_PLUGIN = "utility_plugin"
class PluginStatus(Enum):
    """Enumeration of plugin lifecycle states."""
    DISABLED = "disabled"
    REGISTERED = "registered"
    INITIALIZED = "initialized"
    ENABLED = "enabled"
    RUNNING = "running"
    ERROR = "error"
    UNLOADED = "unloaded"
@dataclass
class PluginInfo:
    """Metadata about a plugin."""
    name: str
    version: str
    author: str
    description: str
    plugin_type: PluginType
    dependencies: List[str] = field(default_factory=list)
    tags: List[str] = field(default_factory=list)
    file_path: Optional[str] = None
    class_name: Optional[str] = None
    enabled: bool = True
    config: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)
@dataclass
class PluginExecutionContext:
    """Context information during plugin execution."""
    plugin_name: str
    plugin_type: PluginType
    timestamp: float
    step: int
    world_state: Dict[str, Any]
    brain_state: Dict[str, Any]
    sensory_input: Optional[Any] = None
    action_output: Optional[Any] = None
    reward_signal: Optional[float] = None
    custom_data: Dict[str, Any] = field(default_factory=dict)
@dataclass
class PluginValidationResult:
    """Result of plugin validation."""
    valid: bool
    errors: List[str] = field(default_factory=list)
    warnings: List[str] = field(default_factory=list)
    compatibility_score: float = 0.0
class PluginInterface(abc.ABC):
    """Abstract base class for all NLM plugins."""

    def __init__(self, info: PluginInfo):
        self.info = info
        self.status = PluginStatus.REGISTERED
        self.execution_count = 0
        self.last_execution_time = 0.0
        self._lock = threading.RLock()

    @abc.abstractmethod
    def validate(self) -> PluginValidationResult:
        """Validate plugin configuration and compatibility."""
        pass

    @abc.abstractmethod
    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize plugin with configuration."""
        pass

    @abc.abstractmethod
    def enable(self) -> bool:
        """Enable plugin for use."""
        pass

    @abc.abstractmethod
    def disable(self) -> bool:
        """Disable plugin."""
        pass

    @abc.abstractmethod
    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute plugin logic with given context."""
        pass

    @abc.abstractmethod
    def cleanup(self) -> bool:
        """Clean up plugin resources."""
        pass

    def get_info(self) -> PluginInfo:
        """Get plugin information."""
        return self.info

    def get_status(self) -> PluginStatus:
        """Get current plugin status."""
        return self.status

    def get_execution_stats(self) -> Dict[str, Any]:
        """Get execution statistics."""
        return {
            "name": self.info.name,
            "version": self.info.version,
            "status": self.status.value,
            "execution_count": self.execution_count,
            "last_execution_time": self.last_execution_time,
            "average_execution_time": (
                self.last_execution_time / self.execution_count 
                if self.execution_count > 0 else 0.0
            )
        }

    @contextmanager
    def execution_context(self):
        """Context manager for thread-safe execution."""
        with self._lock:
            yield self
class NeuronPlugin(PluginInterface):
    """Plugin for custom neuron types."""

    def validate(self) -> PluginValidationResult:
        """Validate neuron plugin."""
        result = PluginValidationResult()
        
        # Check required attributes
        if not hasattr(self, 'neuron_type'):
            result.errors.append("Neuron plugin must define 'neuron_type' attribute")
        
        if not hasattr(self, 'step_neuron'):
            result.errors.append("Neuron plugin must define 'step_neuron' method")
        
        if not hasattr(self, 'get_neuron_properties'):
            result.errors.append("Neuron plugin must define 'get_neuron_properties' method")
        
        # Check compatibility with NLM
        if self.info.config.get('LIF_compatible', True):
            # Check if this plugin follows LIF principles
            required_attrs = ['threshold', 'rest_potential', 'time_constant']
            for attr in required_attrs:
                if not hasattr(self, attr):
                    result.warnings.append(f"Neuron plugin may not be compatible with LIF: missing '{attr}'")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 10.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize neuron plugin with configuration."""
        try:
            self.config = config
            
            # Set default neuron properties if not defined
            if not hasattr(self, 'threshold'):
                self.threshold = config.get('threshold', -50.0)
            
            if not hasattr(self, 'rest_potential'):
                self.rest_potential = config.get('rest_potential', -70.0)
            
            if not hasattr(self, 'time_constant'):
                self.time_constant = config.get('time_constant', 20.0)
            
            if not hasattr(self, 'refractory_period'):
                self.refractory_period = config.get('refractory_period', 2.0)
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing neuron plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable neuron plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling neuron plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable neuron plugin."""
        try:
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling neuron plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute neuron plugin logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Call the custom neuron step function
            result = self.step_neuron(
                membrane_potential=context.brain_state.get('membrane_potential', 0.0),
                synaptic_current=context.brain_state.get('synaptic_current', 0.0),
                step=context.step,
                time=context.timestamp
            )
            
            self.last_execution_time = time.time() - start_time
            
            return {
                'neuron_output': result,
                'plugin_name': self.info.name,
                'execution_time': self.last_execution_time
            }
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing neuron plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up neuron plugin resources."""
        try:
            # Clean up any allocated resources
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up neuron plugin {self.info.name}: {e}")
            return False

    def step_neuron(self, membrane_potential, synaptic_current, step, time) -> float:
        """Override this method in subclasses to implement custom neuron dynamics."""
        # Default implementation: Standard LIF neuron
        return membrane_potential + synaptic_current / self.time_constant

    def get_neuron_properties(self) -> Dict[str, Any]:
        """Get neuron properties."""
        return {
            'type': self.info.name,
            'neuron_type': getattr(self, 'neuron_type', 'generic'),
            'threshold': self.threshold,
            'rest_potential': self.rest_potential,
            'time_constant': self.time_constant,
            'refractory_period': self.refractory_period
        }
class SynapsePlugin(PluginInterface):
    """Plugin for custom synapse types and learning rules."""

    def validate(self) -> PluginValidationResult:
        """Validate synapse plugin."""
        result = PluginValidationResult()
        
        # Check required methods
        required_methods = ['transmit_signal', 'update_weight', 'get_synapse_properties']
        for method in required_methods:
            if not hasattr(self, method):
                result.errors.append(f"Synapse plugin must define '{method}' method")
        
        # Check compatibility
        if self.info.config.get('STDP_compatible', True):
            # Check for STDP-related methods
            stdp_methods = ['pre_post_pairing', 'weight_change_rule']
            has_stdp = any(hasattr(self, method) for method in stdp_methods)
            if not has_stdp:
                result.warnings.append("Synapse plugin may not support STDP learning")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 8.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize synapse plugin."""
        try:
            self.config = config
            
            # Initialize synapse properties
            if not hasattr(self, 'max_weight'):
                self.max_weight = config.get('max_weight', 1.0)
            
            if not hasattr(self, 'min_weight'):
                self.min_weight = config.get('min_weight', 0.0)
            
            if not hasattr(self, 'delay'):
                self.delay = config.get('delay', 1)
            
            if not hasattr(self, 'plasticity_enabled'):
                self.plasticity_enabled = config.get('plasticity_enabled', True)
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing synapse plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable synapse plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling synapse plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable synapse plugin."""
        try:
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling synapse plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute synapse plugin logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Transmit signal through synapse
            transmission_result = self.transmit_signal(
                pre_synaptic_neuron=context.brain_state.get('pre_synaptic_neuron'),
                post_synaptic_neuron=context.brain_state.get('post_synaptic_neuron'),
                weight=context.brain_state.get('synapse_weight', 1.0),
                time=context.timestamp
            )
            
            # Update synapse weight if plasticity is enabled
            if self.plasticity_enabled and context.reward_signal is not None:
                weight_update_result = self.update_weight(
                    pre_synaptic_neuron=context.brain_state.get('pre_synaptic_neuron'),
                    post_synaptic_neuron=context.brain_state.get('post_synaptic_neuron'),
                    reward=context.reward_signal,
                    pre_post_pairing=context.custom_data.get('pre_post_pairing')
                )
            
            self.last_execution_time = time.time() - start_time
            
            return {
                'transmission': transmission_result,
                'weight_update': weight_update_result if self.plasticity_enabled else None,
                'plugin_name': self.info.name,
                'execution_time': self.last_execution_time
            }
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing synapse plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up synapse plugin."""
        try:
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up synapse plugin {self.info.name}: {e}")
            return False

    def transmit_signal(self, pre_synaptic_neuron, post_synaptic_neuron, weight, time) -> Dict[str, Any]:
        """Override this method to implement custom synaptic transmission."""
        # Default implementation: Standard synaptic transmission
        transmission_strength = weight * 0.8  # Default weight factor
        
        return {
            'transmission_strength': transmission_strength,
            'delay': self.delay,
            'weight': weight,
            'time': time
        }

    def update_weight(self, pre_synaptic_neuron, post_synaptic_neuron, reward, pre_post_pairing) -> Dict[str, Any]:
        """Override this method to implement custom plasticity rules."""
        # Default implementation: Simple weight update based on reward
        weight_change = reward * 0.01
        new_weight = max(self.min_weight, min(self.max_weight, weight + weight_change))
        
        return {
            'old_weight': weight,
            'new_weight': new_weight,
            'weight_change': weight_change,
            'learning_rule': 'default_reward'
        }

    def get_synapse_properties(self) -> Dict[str, Any]:
        """Get synapse properties."""
        return {
            'type': self.info.name,
            'max_weight': self.max_weight,
            'min_weight': self.min_weight,
            'delay': self.delay,
            'plasticity_enabled': self.plasticity_enabled,
            'plasticity_rule': self.info.config.get('plasticity_rule', 'default')
        }
class PlasticityRulePlugin(PluginInterface):
    """Plugin for custom plasticity rules (STDP, Hebbian, etc.)."""

    def validate(self) -> PluginValidationResult:
        """Validate plasticity rule plugin."""
        result = PluginValidationResult()
        
        # Check required methods
        required_methods = ['calculate_weight_change', 'get_plasticity_properties']
        for method in required_methods:
            if not hasattr(self, method):
                result.errors.append(f"Plasticity plugin must define '{method}' method")
        
        # Check for STDP-specific methods if STDP compatible
        if self.info.config.get('STDP_compatible', True):
            stdp_methods = ['pre_post_timing', 'trace_updates', ' LTP_rule', ' LTD_rule']
            has_stdp = any(hasattr(self, method) for method in stdp_methods)
            if not has_stdp:
                result.warnings.append("Plasticity plugin may not support STDP timing dependencies")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 6.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize plasticity rule plugin."""
        try:
            self.config = config
            
            # Initialize plasticity parameters
            if not hasattr(self, 'learning_rate'):
                self.learning_rate = config.get('learning_rate', 0.01)
            
            if not hasattr(self, 'time_constant'):
                self.time_constant = config.get('time_constant', 20.0)
            
            if not hasattr(self, 'max_weight_change'):
                self.max_weight_change = config.get('max_weight_change', 0.1)
            
            # Initialize STDP traces if STDP compatible
            if self.info.config.get('STDP_compatible', True):
                self.pre_trace = 0.0
                self.post_trace = 0.0
                self.trace_decay_rate = config.get('trace_decay_rate', 0.001)
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing plasticity plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable plasticity rule plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling plasticity plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable plasticity rule plugin."""
        try:
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling plasticity plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute plasticity rule logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Calculate weight change based on pre and post synaptic activity
            weight_change = self.calculate_weight_change(
                pre_synaptic_activity=context.brain_state.get('pre_synaptic_activity', 0.0),
                post_synaptic_activity=context.brain_state.get('post_synaptic_activity', 0.0),
                reward=context.reward_signal,
                time=context.timestamp
            )
            
            # Update STDP traces if applicable
            if self.info.config.get('STDP_compatible', True):
                self.trace_updates(context.step, context.timestamp)
            
            self.last_execution_time = time.time() - start_time
            
            return {
                'weight_change': weight_change,
                'learning_rate': self.learning_rate,
                'plugin_name': self.info.name,
                'execution_time': self.last_execution_time
            }
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing plasticity plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up plasticity rule plugin."""
        try:
            # Clean up STDP traces
            if hasattr(self, 'pre_trace'):
                del self.pre_trace
            if hasattr(self, 'post_trace'):
                del self.post_trace
            
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up plasticity plugin {self.info.name}: {e}")
            return False

    def calculate_weight_change(self, pre_synaptic_activity, post_synaptic_activity, reward, time) -> float:
        """Override this method to implement custom weight change rules."""
        # Default implementation: Reward-modulated Hebbian learning
        if reward is not None:
            weight_change = reward * self.learning_rate * (post_synaptic_activity - 0.5)
        else:
            weight_change = self.learning_rate * pre_synaptic_activity * post_synaptic_activity
        
        # Apply max weight change constraint
        weight_change = max(-self.max_weight_change, min(self.max_weight_change, weight_change))
        
        return weight_change

    def trace_updates(self, step, time) -> None:
        """Update STDP traces if STDP compatible."""
        # Decay traces
        self.pre_trace *= (1.0 - self.trace_decay_rate)
        self.post_trace *= (1.0 - self.trace_decay_rate)
        
        # This would be called with spike events in a real implementation
        # For now, this is a placeholder

    def get_plasticity_properties(self) -> Dict[str, Any]:
        """Get plasticity rule properties."""
        return {
            'type': self.info.name,
            'learning_rate': self.learning_rate,
            'time_constant': self.time_constant,
            'max_weight_change': self.max_weight_change,
            'STDP_compatible': self.info.config.get('STDP_compatible', True),
            'rule_type': self.info.config.get('rule_type', 'default')
        }
class NeuromodulatorPlugin(PluginInterface):
    """Plugin for custom neuromodulators (dopamine, curiosity, novelty, etc.)."""

    def validate(self) -> PluginValidationResult:
        """Validate neuromodulator plugin."""
        result = PluginValidationResult()
        
        # Check required methods
        required_methods = ['calculate_modulation', 'get_modulator_properties']
        for method in required_methods:
            if not hasattr(self, method):
                result.errors.append(f"Neuromodulator plugin must define '{method}' method")
        
        # Check for signal properties
        if not hasattr(self, 'modulation_strength'):
            result.warnings.append("Neuromodulator plugin should define 'modulation_strength' attribute")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 6.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize neuromodulator plugin."""
        try:
            self.config = config
            
            # Initialize neuromodulator properties
            if not hasattr(self, 'modulation_strength'):
                self.modulation_strength = config.get('modulation_strength', 1.0)
            
            if not hasattr(self, 'baseline_level'):
                self.baseline_level = config.get('baseline_level', 0.0)
            
            if not hasattr(self, 'adaptation_rate'):
                self.adaptation_rate = config.get('adaptation_rate', 0.1)
            
            # Initialize current level
            self.current_level = self.baseline_level
            self.target_level = self.baseline_level
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing neuromodulator plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable neuromodulator plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling neuromodulator plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable neuromodulator plugin."""
        try:
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling neuromodulator plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute neuromodulator logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Calculate neuromodulation level based on context
            modulation_result = self.calculate_modulation(
                prediction_error=context.reward_signal if context.reward_signal is not None else 0.0,
                novelty_level=context.custom_data.get('novelty_level', 0.0),
                reward=context.reward_signal,
                time=context.timestamp,
                step=context.step
            )
            
            # Adapt current level towards target
            self.current_level = (
                self.current_level * (1.0 - self.adaptation_rate) +
                modulation_result['modulation_level'] * self.adaptation_rate
            )
            
            self.last_execution_time = time.time() - start_time
            
            return {
                'modulation_level': self.current_level,
                'modulation_strength': self.modulation_strength,
                'adaptation_rate': self.adaptation_rate,
                'plugin_name': self.info.name,
                'execution_time': self.last_execution_time
            }
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing neuromodulator plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up neuromodulator plugin."""
        try:
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up neuromodulator plugin {self.info.name}: {e}")
            return False

    def calculate_modulation(self, prediction_error, novelty_level, reward, time, step) -> Dict[str, Any]:
        """Override this method to implement custom neuromodulation rules."""
        # Default implementation: Dopamine-like neuromodulation based on prediction error
        if prediction_error is not None:
            error_magnitude = abs(prediction_error)
            modulation_level = error_magnitude * self.modulation_strength
        else:
            modulation_level = self.baseline_level
        
        # Add novelty contribution if applicable
        if novelty_level > 0:
            novelty_contribution = novelty_level * 0.5 * self.modulation_strength
            modulation_level += novelty_contribution
        
        # Apply reward modulation if reward is available
        if reward is not None:
            reward_modulation = reward * 0.1 * self.modulation_strength
            modulation_level += reward_modulation
        
        return {
            'modulation_level': modulation_level,
            'prediction_error_contribution': error_magnitude if prediction_error is not None else 0.0,
            'novelty_contribution': novelty_level * self.modulation_strength if novelty_level > 0 else 0.0,
            'reward_contribution': reward * self.modulation_strength if reward is not None else 0.0
        }

    def get_modulator_properties(self) -> Dict[str, Any]:
        """Get neuromodulator properties."""
        return {
            'type': self.info.name,
            'modulation_strength': self.modulation_strength,
            'baseline_level': self.baseline_level,
            'adaptation_rate': self.adaptation_rate,
            'current_level': self.current_level,
            'modulator_type': self.info.config.get('modulator_type', 'dopamine')
        }
class MemorySystemPlugin(PluginInterface):
    """Plugin for custom memory systems (working memory, episodic memory, etc.)."""

    def validate(self) -> PluginValidationResult:
        """Validate memory system plugin."""
        result = PluginValidationResult()
        
        # Check required methods
        required_methods = ['store_item', 'retrieve_item', 'get_memory_stats']
        for method in required_methods:
            if not hasattr(self, method):
                result.errors.append(f"Memory system plugin must define '{method}' method")
        
        # Check capacity and performance parameters
        if not hasattr(self, 'capacity'):
            result.warnings.append("Memory system plugin should define 'capacity' attribute")
        
        if not hasattr(self, 'access_time'):
            result.warnings.append("Memory system plugin should define 'access_time' attribute")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 8.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize memory system plugin."""
        try:
            self.config = config
            
            # Initialize memory system properties
            if not hasattr(self, 'capacity'):
                self.capacity = config.get('capacity', 1000)
            
            if not hasattr(self, 'access_time'):
                self.access_time = config.get('access_time', 0.001)
            
            if not hasattr(self, 'decay_rate'):
                self.decay_rate = config.get('decay_rate', 0.99)
            
            # Initialize memory storage
            self.memory_storage = {}
            self.access_log = []
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing memory system plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable memory system plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling memory system plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable memory system plugin."""
        try:
            # Clear memory storage
            self.memory_storage.clear()
            self.access_log.clear()
            
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling memory system plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute memory system logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Store or retrieve memory items based on context
            if context.custom_data.get('store_item'):
                store_result = self.store_item(
                    item=context.custom_data.get('item'),
                    key=context.custom_data.get('item_key', f"step_{context.step}"),
                    priority=context.custom_data.get('priority', 0.5)
                )
            elif context.custom_data.get('retrieve_item'):
                retrieve_result = self.retrieve_item(
                    key=context.custom_data.get('item_key', f"step_{context.step}"),
                    confidence=context.custom_data.get('confidence', 1.0)
                )
            else:
                store_result = {'status': 'no_action'}
                retrieve_result = {'status': 'no_action'}
            
            # Decay memory items if decay rate is configured
            if self.decay_rate < 1.0:
                self._decay_memory()
            
            self.last_execution_time = time.time() - start_time
            
            return {
                'store_result': store_result if 'store_result' in locals() else None,
                'retrieve_result': retrieve_result if 'retrieve_result' in locals() else None,
                'memory_usage': len(self.memory_storage),
                'capacity': self.capacity,
                'plugin_name': self.info.name,
                'execution_time': self.last_execution_time
            }
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing memory system plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up memory system plugin."""
        try:
            self.memory_storage.clear()
            self.access_log.clear()
            
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up memory system plugin {self.info.name}: {e}")
            return False

    def store_item(self, item, key, priority) -> Dict[str, Any]:
        """Override this method to implement custom memory storage."""
        # Default implementation: Simple key-value storage with priority
        if len(self.memory_storage) >= self.capacity:
            # Remove lowest priority item
            sorted_items = sorted(self.memory_storage.items(), key=lambda x: x[1]['priority'])
            del self.memory_storage[sorted_items[0][0]]
        
        self.memory_storage[key] = {
            'item': item,
            'priority': priority,
            'timestamp': time.time(),
            'access_count': 0
        }
        
        self.access_log.append({
            'key': key,
            'action': 'store',
            'timestamp': time.time(),
            'priority': priority
        })
        
        return {
            'status': 'stored',
            'key': key,
            'memory_size': len(self.memory_storage)
        }

    def retrieve_item(self, key, confidence) -> Dict[str, Any]:
        """Override this method to implement custom memory retrieval."""
        # Default implementation: Standard retrieval with access tracking
        if key not in self.memory_storage:
            return {'status': 'not_found', 'key': key}
        
        # Update access count and log
        self.memory_storage[key]['access_count'] += 1
        self.access_log.append({
            'key': key,
            'action': 'retrieve',
            'timestamp': time.time(),
            'confidence': confidence,
            'access_count': self.memory_storage[key]['access_count']
        })
        
        # Decay based on access patterns
        if self.access_log and len(self.access_log) > 10:
            recent_accesses = [log for log in self.access_log[-10:] if log['key'] == key]
            if recent_accesses:
                access_frequency = len(recent_accesses) / 10.0
                decay_factor = self.decay_rate ** access_frequency
                self.memory_storage[key]['item'] = self._apply_decay(self.memory_storage[key]['item'], decay_factor)
        
        return {
            'status': 'retrieved',
            'key': key,
            'item': self.memory_storage[key]['item'],
            'priority': self.memory_storage[key]['priority'],
            'access_count': self.memory_storage[key]['access_count'],
            'age': time.time() - self.memory_storage[key]['timestamp']
        }

    def _decay_memory(self) -> None:
        """Apply decay to all memory items."""
        for key, item_data in self.memory_storage.items():
            age_factor = self.decay_rate ** (time.time() - item_data['timestamp'])
            item_data['item'] = self._apply_decay(item_data['item'], age_factor)

    def _apply_decay(self, item, decay_factor) -> Any:
        """Apply decay factor to memory item (override in subclasses)."""
        # Default implementation: Apply decay to numeric values
        if isinstance(item, (int, float)):
            return item * decay_factor
        elif isinstance(item, dict):
            return {k: self._apply_decay(v, decay_factor) for k, v in item.items()}
        elif isinstance(item, list):
            return [self._apply_decay(v, decay_factor) for v in item]
        else:
            return item

    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory system statistics."""
        return {
            'type': self.info.name,
            'capacity': self.capacity,
            'current_usage': len(self.memory_storage),
            'utilization_rate': len(self.memory_storage) / self.capacity,
            'access_time': self.access_time,
            'decay_rate': self.decay_rate,
            'total_accesses': sum(item['access_count'] for item in self.memory_storage.values()),
            'memory_items': list(self.memory_storage.keys())
        }
class ConfigurationPlugin(PluginInterface):
    """Plugin for custom configuration management."""

    def validate(self) -> PluginValidationResult:
        """Validate configuration plugin."""
        result = PluginValidationResult()
        
        # Check required methods
        required_methods = ['get_config_value', 'set_config_value', 'validate_config']
        for method in required_methods:
            if not hasattr(self, method):
                result.errors.append(f"Configuration plugin must define '{method}' method")
        
        result.valid = len(result.errors) == 0
        result.compatibility_score = 1.0 - (len(result.errors) + len(result.warnings)) / 6.0
        
        return result

    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize configuration plugin."""
        try:
            self.config = config
            self.config_storage = config.copy()
            
            # Initialize validation rules if provided
            if 'validation_rules' in config:
                self.validation_rules = config['validation_rules']
            else:
                self.validation_rules = {}
            
            self.status = PluginStatus.INITIALIZED
            return True
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error initializing configuration plugin {self.info.name}: {e}")
            return False

    def enable(self) -> bool:
        """Enable configuration plugin."""
        try:
            self.status = PluginStatus.ENABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error enabling configuration plugin {self.info.name}: {e}")
            return False

    def disable(self) -> bool:
        """Disable configuration plugin."""
        try:
            self.status = PluginStatus.DISABLED
            return True
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error disabling configuration plugin {self.info.name}: {e}")
            return False

    def execute(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute configuration plugin logic."""
        start_time = time.time()
        
        try:
            self.execution_count += 1
            
            # Get or set configuration values based on context
            result = {'plugin_name': self.info.name, 'execution_time': self.last_execution_time}
            
            # Handle configuration queries from context
            if context.custom_data.get('get_config'):
                key = context.custom_data.get('config_key')
                default = context.custom_data.get('default_value')
                value = self.get_config_value(key, default)
                result['config_value'] = value
                result['config_key'] = key
            
            # Handle configuration updates from context
            elif context.custom_data.get('set_config'):
                key = context.custom_data.get('config_key')
                value = context.custom_data.get('config_value')
                success = self.set_config_value(key, value)
                result['success'] = success
                result['config_key'] = key
            
            # Validate current configuration if requested
            elif context.custom_data.get('validate_config'):
                validation_result = self.validate_config()
                result.update(validation_result)
            
            self.last_execution_time = time.time() - start_time
            
            return result
            
        except Exception as e:
            self.status = PluginStatus.ERROR
            print(f"Error executing configuration plugin {self.info.name}: {e}")
            traceback.print_exc()
            
            return {
                'error': str(e),
                'plugin_name': self.info.name,
                'execution_time': time.time() - start_time
            }

    def cleanup(self) -> bool:
        """Clean up configuration plugin."""
        try:
            self.config_storage.clear()
            if hasattr(self, 'validation_rules'):
                del self.validation_rules
            
            self.status = PluginStatus.UNLOADED
            return True
        except Exception as e:
            print(f"Error cleaning up configuration plugin {self.info.name}: {e}")
            return False

    def get_config_value(self, key: str, default=None) -> Any:
        """Override this method to implement custom configuration retrieval."""
        return self.config_storage.get(key, default)

    def set_config_value(self, key: str, value: Any) -> bool:
        """Override this method to implement custom configuration setting."""
        # Validate value if validation rules are defined
        if hasattr(self, 'validation_rules') and key in self.validation_rules:
            validator = self.validation_rules[key]
            if not validator(value):
                return False
        
        self.config_storage[key] = value
        return True

    def validate_config(self) -> Dict[str, Any]:
        """Override this method to implement custom configuration validation."""
        # Default validation: Check for required keys
        required_keys = self.info.config.get('required_keys', [])
        missing_keys = [key for key in required_keys if key not in self.config_storage]
        
        return {
            'valid': len(missing_keys) == 0,
            'missing_keys': missing_keys,
            'current_config': self.config_storage.copy()
        }

    def get_plugin_config(self) -> Dict[str, Any]:
        """Get plugin-specific configuration."""
        return self.info.config.copy()
class NLMServer:
    """Central server for managing NLM plugins and their execution."""

    def __init__(self):
        self.plugins: Dict[str, PluginInterface] = {}
        self.plugin_infos: Dict[str, PluginInfo] = {}
        self.plugin_status: Dict[str, PluginStatus] = {}
        self.plugin_locks = threading.RLock()
        self.execution_history: List[Dict[str, Any]] = []
        self.performance_metrics = {
            'total_plugins': 0,
            'enabled_plugins': 0,
            'average_execution_time': 0.0,
            'total_execution_count': 0
        }

    def register_plugin(self, plugin: PluginInterface) -> bool:
        """Register a new plugin with the server."""
        try:
            with self.plugin_locks:
                plugin_name = plugin.info.name
                
                if plugin_name in self.plugins:
                    print(f"Plugin {plugin_name} is already registered")
                    return False
                
                # Validate plugin
                validation_result = plugin.validate()
                if not validation_result.valid:
                    print(f"Plugin {plugin_name} validation failed:")
                    for error in validation_result.errors:
                        print(f"  - {error}")
                    return False
                
                # Store plugin
                self.plugins[plugin_name] = plugin
                self.plugin_infos[plugin_name] = plugin.info
                self.plugin_status[plugin_name] = PluginStatus.REGISTERED
                
                # Update performance metrics
                self.performance_metrics['total_plugins'] += 1
                
                print(f"Plugin {plugin_name} registered successfully")
                return True
                
        except Exception as e:
            print(f"Error registering plugin: {e}")
            return False

    def load_plugin(self, plugin_info: PluginInfo, config: Dict[str, Any] = None) -> bool:
        """Load a plugin from its information and configuration."""
        try:
            with self.plugin_locks:
                plugin_name = plugin_info.name
                
                if plugin_name in self.plugins:
                    print(f"Plugin {plugin_name} is already loaded")
                    return False
                
                # Create plugin instance using its class
                if plugin_info.class_name:
                    module = importlib.import_module(plugin_info.file_path)
                    plugin_class = getattr(module, plugin_info.class_name)
                    plugin = plugin_class(plugin_info)
                else:
                    print(f"Cannot load plugin {plugin_name}: No class specified")
                    return False
                
                # Initialize plugin
                if config is None:
                    config = {}
                
                if not plugin.initialize(config):
                    print(f"Plugin {plugin_name} initialization failed")
                    return False
                
                # Enable plugin
                if not plugin.enable():
                    print(f"Plugin {plugin_name} enable failed")
                    return False
                
                # Register plugin
                if not self.register_plugin(plugin):
                    return False
                
                print(f"Plugin {plugin_name} loaded successfully")
                return True
                
        except Exception as e:
            print(f"Error loading plugin: {e}")
            return False

    def enable_plugin(self, plugin_name: str) -> bool:
        """Enable a loaded plugin."""
        try:
            with self.plugin_locks:
                if plugin_name not in self.plugins:
                    print(f"Plugin {plugin_name} not found")
                    return False
                
                plugin = self.plugins[plugin_name]
                if plugin.get_status() == PluginStatus.ENABLED:
                    print(f"Plugin {plugin_name} is already enabled")
                    return True
                
                if plugin.enable():
                    self.plugin_status[plugin_name] = PluginStatus.ENABLED
                    self.performance_metrics['enabled_plugins'] += 1
                    print(f"Plugin {plugin_name} enabled successfully")
                    return True
                else:
                    print(f"Failed to enable plugin {plugin_name}")
                    return False
                    
        except Exception as e:
            print(f"Error enabling plugin {plugin_name}: {e}")
            return False

    def disable_plugin(self, plugin_name: str) -> bool:
        """Disable a loaded plugin."""
        try:
            with self.plugin_locks:
                if plugin_name not in self.plugins:
                    print(f"Plugin {plugin_name} not found")
                    return False
                
                plugin = self.plugins[plugin_name]
                if plugin.get_status() == PluginStatus.DISABLED:
                    print(f"Plugin {plugin_name} is already disabled")
                    return True
                
                if plugin.disable():
                    self.plugin_status[plugin_name] = PluginStatus.DISABLED
                    self.performance_metrics['enabled_plugins'] -= 1
                    print(f"Plugin {plugin_name} disabled successfully")
                    return True
                else:
                    print(f"Failed to disable plugin {plugin_name}")
                    return False
                    
        except Exception as e:
            print(f"Error disabling plugin {plugin_name}: {e}")
            return False

    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a plugin from the server."""
        try:
            with self.plugin_locks:
                if plugin_name not in self.plugins:
                    print(f"Plugin {plugin_name} not found")
                    return False
                
                plugin = self.plugins[plugin_name]
                
                if plugin.get_status() in [PluginStatus.ENABLED, PluginStatus.RUNNING]:
                    print(f"Plugin {plugin_name} must be disabled before unloading")
                    return False
                
                # Clean up plugin
                if plugin.cleanup():
                    # Remove plugin from server
                    del self.plugins[plugin_name]
                    del self.plugin_infos[plugin_name]
                    del self.plugin_status[plugin_name]
                    
                    self.performance_metrics['total_plugins'] -= 1
                    if plugin.get_status() == PluginStatus.ENABLED:
                        self.performance_metrics['enabled_plugins'] -= 1
                    
                    print(f"Plugin {plugin_name} unloaded successfully")
                    return True
                else:
                    print(f"Failed to clean up plugin {plugin_name}")
                    return False
                    
        except Exception as e:
            print(f"Error unloading plugin {plugin_name}: {e}")
            return False

    def execute_plugins(self, context: PluginExecutionContext) -> Dict[str, Any]:
        """Execute all enabled plugins with the given context."""
        try:
            with self.plugin_locks:
                results = {}
                
                for plugin_name, plugin in self.plugins.items():
                    if plugin.get_status() in [PluginStatus.ENABLED, PluginStatus.RUNNING]:
                        plugin_start_time = time.time()
                        
                        # Execute plugin
                        plugin_result = plugin.execute(context)
                        
                        plugin_end_time = time.time()
                        execution_time = plugin_end_time - plugin_start_time
                        
                        # Store result
                        results[plugin_name] = {
                            **plugin_result,
                            'execution_time': execution_time,
                            'timestamp': context.timestamp
                        }
                        
                        # Update performance metrics
                        self.performance_metrics['total_execution_count'] += 1
                        self.performance_metrics['average_execution_time'] = (
                            (self.performance_metrics['average_execution_time'] * (self.performance_metrics['total_execution_count'] - 1) +
                             execution_time) / self.performance_metrics['total_execution_count']
                        )
                
                # Store execution history
                execution_record = {
                    'context': {
                        'step': context.step,
                        'timestamp': context.timestamp,
                        'plugin_names': list(results.keys())
                    },
                    'results': results,
                    'total_execution_time': sum(r['execution_time'] for r in results.values())
                }
                
                self.execution_history.append(execution_record)
                
                return results
                
        except Exception as e:
            print(f"Error executing plugins: {e}")
            return {}

    def get_plugin_status(self, plugin_name: str = None) -> Union[PluginStatus, Dict[str, PluginStatus]]:
        """Get status of a plugin or all plugins."""
        if plugin_name:
            return self.plugin_status.get(plugin_name, PluginStatus.DISABLED)
        else:
            return self.plugin_status.copy()

    def get_plugin_info(self, plugin_name: str = None) -> Union[PluginInfo, Dict[str, PluginInfo]]:
        """Get information about a plugin or all plugins."""
        if plugin_name:
            return self.plugin_infos.get(plugin_name)
        else:
            return self.plugin_infos.copy()

    def get_plugin_stats(self, plugin_name: str = None) -> Union[Dict[str, Any], Dict[str, Dict[str, Any]]]:
        """Get statistics for a plugin or all plugins."""
        if plugin_name:
            if plugin_name in self.plugins:
                return self.plugins[plugin_name].get_execution_stats()
            else:
                return None
        else:
            stats = {}
            for name, plugin in self.plugins.items():
                stats[name] = plugin.get_execution_stats()
            return stats

    def get_performance_metrics(self) -> Dict[str, Any]:
        """Get server performance metrics."""
        return self.performance_metrics.copy()

    def get_execution_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get execution history."""
        return self.execution_history[-limit:]

    def shutdown(self) -> bool:
        """Shutdown the plugin server and unload all plugins."""
        try:
            with self.plugin_locks:
                # Disable all enabled plugins
                for plugin_name in list(self.plugins.keys()):
                    self.disable_plugin(plugin_name)
                
                # Unload all plugins
                for plugin_name in list(self.plugins.keys()):
                    self.unload_plugin(plugin_name)
                
                print("Plugin server shutdown successfully")
                return True
                
        except Exception as e:
            print(f"Error shutting down plugin server: {e}")
            return False
def create_default_neuron_plugin(neuron_type: str = "generic") -> NeuronPlugin:
    """Create a default neuron plugin for testing and development."""
    info = PluginInfo(
        name=f"default_{neuron_type}_neuron",
        version="1.0.0",
        author="NLM Team",
        description=f"Default {neuron_type} neuron plugin for NLM",
        plugin_type=PluginType.NEURON,
        config={
            'LIF_compatible': True,
            'neuron_type': neuron_type,
            'threshold': -50.0,
            'rest_potential': -70.0,
            'time_constant': 20.0,
            'refractory_period': 2.0
        }
    )
    
    plugin = NeuronPlugin(info)
    return plugin
def create_default_synapse_plugin(synapse_type: str = "excitatory") -> SynapsePlugin:
    """Create a default synapse plugin for testing and development."""
    info = PluginInfo(
        name=f"default_{synapse_type}_synapse",
        version="1.0.0",
        author="NLM Team",
        description=f"Default {synapse_type} synapse plugin for NLM",
        plugin_type=PluginType.SYNAPSE,
        config={
            'max_weight': 1.0,
            'min_weight': 0.0,
            'delay': 1,
            'plasticity_enabled': True,
            'plasticity_rule': 'default'
        }
    )
    
    plugin = SynapsePlugin(info)
    return plugin
def create_default_plasticity_plugin(rule_type: str = "hebbian") -> PlasticityRulePlugin:
    """Create a default plasticity rule plugin for testing and development."""
    info = PluginInfo(
        name=f"default_{rule_type}_plasticity",
        version="1.0.0",
        author="NLM Team",
        description=f"Default {rule_type} plasticity rule plugin for NLM",
        plugin_type=PluginType.PLASTICITY_RULE,
        config={
            'learning_rate': 0.01,
            'time_constant': 20.0,
            'max_weight_change': 0.1,
            'STDP_compatible': False,
            'rule_type': rule_type
        }
    )
    
    plugin = PlasticityRulePlugin(info)
    return plugin
def create_default_neuromodulator_plugin(modulator_type: str = "dopamine") -> NeuromodulatorPlugin:
    """Create a default neuromodulator plugin for testing and development."""
    info = PluginInfo(
        name=f"default_{modulator_type}_neuromodulator",
        version="1.0.0",
        author="NLM Team",
        description=f"Default {modulator_type} neuromodulator plugin for NLM",
        plugin_type=PluginType.NEUROMODULATOR,
        config={
            'modulation_strength': 1.0,
            'baseline_level': 0.0,
            'adaptation_rate': 0.1,
            'modulator_type': modulator_type
        }
    )
    
    plugin = NeuromodulatorPlugin(info)
    return plugin
def create_default_configuration_plugin() -> ConfigurationPlugin:
    """Create a default configuration plugin for testing and development."""
    info = PluginInfo(
        name="default_configuration",
        version="1.0.0",
        author="NLM Team",
        description="Default configuration plugin for NLM",
        plugin_type=PluginType.CONFIGURATION_PLUGIN,
        config={
            'validation_rules': {},
            'required_keys': ['brain.neuron_count', 'random_seed']
        }
    )
    
    plugin = ConfigurationPlugin(info)
    return plugin
def main():
    """Demonstrate NLM plugin system functionality."""
    print("NLM Plugin System Demonstration")
    print("=" * 50)
    
    # Create plugin server
    server = NLMServer()
    
    # Create default plugins
    neuron_plugin = create_default_neuron_plugin("adaptive")
    synapse_plugin = create_default_synapse_plugin("excitatory")
    plasticity_plugin = create_default_plasticity_plugin("hebbian")
    neuromodulator_plugin = create_default_neuromodulator_plugin("dopamine")
    config_plugin = create_default_configuration_plugin()
    
    # Register plugins
    print("\n1. Registering default plugins...")
    server.register_plugin(neuron_plugin)
    server.register_plugin(synapse_plugin)
    server.register_plugin(plasticity_plugin)
    server.register_plugin(neuromodulator_plugin)
    server.register_plugin(config_plugin)
    
    # Enable some plugins
    print("\n2. Enabling plugins...")
    server.enable_plugin("default_adaptive_neuron")
    server.enable_plugin("default_excitatory_synapse")
    server.enable_plugin("default_hebbian_plasticity")
    server.enable_plugin("default_dopamine_neuromodulator")
    
    # Create example execution context
    print("\n3. Creating execution context...")
    context = PluginExecutionContext(
        plugin_name="demonstration",
        plugin_type=PluginType.NEURON,
        timestamp=time.time(),
        step=1,
        world_state={
            'membrane_potential': -65.0,
            'synaptic_current': 0.0
        },
        brain_state={
            'pre_synaptic_neuron': 'neuron_1',
            'post_synaptic_neuron': 'neuron_2',
            'synapse_weight': 0.8
        },
        custom_data={
            'novelty_level': 0.3,
            'store_item': True,
            'item_key': 'test_memory',
            'item': {'value': 42, 'priority': 0.7}
        }
    )
    
    # Execute plugins
    print("\n4. Executing plugins...")
    results = server.execute_plugins(context)
    
    for plugin_name, result in results.items():
        print(f"\n{plugin_name} results:")
        for key, value in result.items():
            if key != 'execution_time':
                print(f"  {key}: {value}")
        print(f"  execution_time: {result['execution_time']:.6f}s")
    
    # Display plugin statistics
    print("\n5. Plugin statistics...")
    for plugin_name in server.get_plugin_status().keys():
        stats = server.get_plugin_stats(plugin_name)
        print(f"\n{plugin_name}:")
        print(f"  Status: {stats['status']}")
        print(f"  Execution count: {stats['execution_count']}")
        print(f"  Average execution time: {stats['average_execution_time']:.6f}s")
    
    # Display server performance metrics
    print("\n6. Server performance metrics...")
    metrics = server.get_performance_metrics()
    for key, value in metrics.items():
        print(f"  {key}: {value}")
    
    # Shutdown server
    print("\n7. Shutting down plugin server...")
    server.shutdown()
    
    print("\nPlugin system demonstration completed successfully!")

if __name__ == "__main__":
    main()