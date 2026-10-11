#!/usr/bin/env python3
"""
Custom Plasticity Rule Hooks for Advanced NLM Users

This module provides advanced plasticity rule customization capabilities
for users who want to create custom learning rules beyond the built-in
STDP and Hebbian implementations.
"""

import pynlm
import numpy as np
from typing import Callable, Dict, Any, Optional, List
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
import json


@dataclass
class PlasticityRuleConfig:
    """Configuration for custom plasticity rules"""
    name: str
    rule_type: str  # "stdp", "hebbian", "custom", "reward", "triplet"
    enabled: bool = True
    parameters: Dict[str, Any] = field(default_factory=dict)
    
    # Rule-specific parameters
    # STDP parameters
    ltp_weight: float = 0.01
    ltd_weight: float = 0.012
    time_constant: float = 20.0
    
    # Hebbian parameters
    hebbian_learning_rate: float = 0.01
    
    # Custom rule parameters
    custom_function: Optional[Callable] = None
    weight_bounds: tuple = (-1.0, 1.0)
    
    # Reward modulation parameters
    reward_scale: float = 1.0
    prediction_error_weight: float = 1.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary for serialization"""
        return {
            'name': self.name,
            'rule_type': self.rule_type,
            'enabled': self.enabled,
            'parameters': self.parameters,
            'ltp_weight': self.ltp_weight,
            'ltd_weight': self.ltd_weight,
            'time_constant': self.time_constant,
            'hebbian_learning_rate': self.hebbian_learning_rate,
            'custom_function': None,  # Cannot serialize functions
            'weight_bounds': self.weight_bounds,
            'reward_scale': self.reward_scale,
            'prediction_error_weight': self.prediction_error_weight,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'PlasticityRuleConfig':
        """Create from dictionary"""
        # Remove non-serializable fields
        data_copy = data.copy()
        data_copy.pop('custom_function', None)
        return cls(**data_copy)


class CustomPlasticityRule(ABC):
    """
    Abstract base class for custom plasticity rules.
    
    Advanced users can implement their own plasticity rules by subclassing
    this class and implementing the update method. This enables the creation
    of novel learning mechanisms beyond the standard STDP and Hebbian rules.
    """
    
    def __init__(self, name: str, config: Optional[PlasticityRuleConfig] = None):
        self.name = name
        self.config = config or PlasticityRuleConfig(name, "custom")
        self.enabled = self.config.enabled
        self.rule_type = self.config.rule_type
        
        # Statistics and debugging
        self.update_count = 0
        self.total_weight_change = 0.0
        self.last_update_time = 0.0
        
    @abstractmethod
    def update(self, synapse_info: Dict[str, Any], 
               pre_synaptic_activities: List[float],
               post_synaptic_activities: List[float],
               time_delta: float) -> float:
        """
        Update synaptic weights based on pre and post synaptic activities.
        
        Args:
            synapse_info: Dictionary containing synapse information
                (pre-synaptic neuron ID, post-synaptic neuron ID, current weight, etc.)
            pre_synaptic_activities: List of pre-synaptic neuron activities
            post_synaptic_activities: List of post-synaptic neuron activities
            time_delta: Time since last update (in ms)
            
        Returns:
            Weight change to apply to the synapse
        """
        pass
    
    def apply_rule(self, synapse_info: Dict[str, Any],
                   pre_activities: List[float],
                   post_activities: List[float],
                   time_delta: float) -> float:
        """
        Apply the plasticity rule with validation and statistics tracking.
        
        Args:
            synapse_info: Synapse information
            pre_activities: Pre-synaptic activities
            post_activities: Post-synaptic activities
            time_delta: Time delta
            
        Returns:
            Weight change to apply
        """
        if not self.enabled:
            return 0.0
        
        try:
            weight_change = self.update(synapse_info, pre_activities, 
                                      post_activities, time_delta)
            
            # Apply bounds checking
            weight_change = self._apply_bounds(weight_change)
            
            # Update statistics
            self.update_count += 1
            self.total_weight_change += weight_change
            self.last_update_time = time_delta
            
            return weight_change
            
        except Exception as e:
            print(f"Error in custom plasticity rule {self.name}: {e}")
            return 0.0
    
    def _apply_bounds(self, weight_change: float) -> float:
        """Apply weight bounds based on configuration"""
        bounds = self.config.weight_bounds
        current_weight = self._get_synapse_weight()
        
        # Check if weight change would exceed bounds
        if current_weight + weight_change > bounds[1]:
            weight_change = bounds[1] - current_weight
        elif current_weight + weight_change < bounds[0]:
            weight_change = bounds[0] - current_weight
            
        return weight_change
    
    def _get_synapse_weight(self) -> float:
        """Get current synapse weight (to be implemented by subclass)"""
        return 0.0
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get rule statistics"""
        return {
            'name': self.name,
            'rule_type': self.rule_type,
            'enabled': self.enabled,
            'update_count': self.update_count,
            'total_weight_change': self.total_weight_change,
            'average_weight_change': (self.total_weight_change / 
                                    self.update_count if self.update_count > 0 else 0.0),
            'last_update_time': self.last_update_time,
            'parameters': self.config.parameters,
        }
    
    def export_config(self) -> Dict[str, Any]:
        """Export rule configuration"""
        return self.config.to_dict()


class STDPPlus(CustomPlasticityRule):
    """
    Enhanced STDP rule with additional features:
    - Configurable learning rate scaling
    - Spike triplet interactions
    - Neuromodulation support
    - Eligibility traces
    """
    
    def __init__(self, name: str, config: Optional[PlasticityRuleConfig] = None):
        super().__init__(name, config)
        if self.rule_type == "custom":
            self.rule_type = "stdp_plus"
    
    def update(self, synapse_info: Dict[str, Any],
               pre_synaptic_activities: List[float],
               post_synaptic_activities: List[float],
               time_delta: float) -> float:
        """
        Enhanced STDP with triplet interactions and eligibility traces.
        
        Mathematical formulation:
        - For pre-post spike pairs (Δt > 0): LTP = A+ * exp(-Δt/τ+) + B+ * trace_pre
        - For post-pre spike pairs (Δt < 0): LTD = A- * exp(Δt/τ-) + B- * trace_post
        
        Where traces represent eligibility for reward-modulated learning.
        """
        # Extract parameters from config
        ltp_weight = self.config.ltp_weight
        ltd_weight = self.config.ltd_weight
        time_constant = self.config.time_constant
        
        # Calculate spike timing differences
        weight_change = 0.0
        
        # Process pre-post spike pairs (Δt > 0) - LTP
        for pre_activity in pre_synaptic_activities:
            if pre_activity > 0.5:  # Threshold for considering a spike
                for post_activity in post_synaptic_activities:
                    if post_activity > 0.5 and time_delta > 0:
                        # Calculate LTP contribution
                        delta = ltp_weight * np.exp(-time_delta / time_constant)
                        # Add eligibility trace contribution if available
                        trace_factor = self._get_trace_factor('pre', time_delta)
                        weight_change += delta * trace_factor
        
        # Process post-pre spike pairs (Δt < 0) - LTD
        for post_activity in post_synaptic_activities:
            if post_activity > 0.5:
                for pre_activity in pre_synaptic_activities:
                    if pre_activity > 0.5 and time_delta < 0:
                        # Calculate LTD contribution
                        delta = -ltd_weight * np.exp(time_delta / time_constant)
                        # Add eligibility trace contribution if available
                        trace_factor = self._get_trace_factor('post', time_delta)
                        weight_change += delta * trace_factor
        
        return weight_change * self.config.reward_scale
    
    def _get_trace_factor(self, neuron_type: str, time_delta: float) -> float:
        """Get eligibility trace factor"""
        # Simple implementation - can be enhanced with more sophisticated traces
        if neuron_type == 'pre':
            # Pre-synaptic trace decays over time
            return np.exp(time_delta / 50.0)  # 50ms decay
        else:
            # Post-synaptic trace
            return np.exp(-time_delta / 100.0)  # 100ms decay


class RewardModulatedPlasticity(CustomPlasticityRule):
    """
    Plasticity rule modulated by reward prediction errors.
    
    Implements actor-critic style learning where synaptic changes are
    scaled by prediction error signals, similar to dopamine modulation.
    """
    
    def __init__(self, name: str, config: Optional[PlasticityRuleConfig] = None):
        super().__init__(name, config)
        if self.rule_type == "custom":
            self.rule_type = "reward_modulated"
    
    def update(self, synapse_info: Dict[str, Any],
               pre_synaptic_activities: List[float],
               post_synaptic_activities: List[float],
               time_delta: float) -> float:
        """
        Reward-modulated learning:
        
        Δw = learning_rate * prediction_error * correlation
        
        Where prediction_error = reward - predicted_reward
        and correlation measures pre-post spike pairing.
        """
        # Calculate prediction error from config
        prediction_error = self.config.get('prediction_error', 0.0)
        
        # Calculate spike correlation
        correlation = self._calculate_correlation(
            pre_synaptic_activities, post_synaptic_activities)
        
        # Apply reward modulation
        base_learning = self.config.hebbian_learning_rate * correlation
        modulated_learning = base_learning * prediction_error * self.config.reward_scale
        
        return modulated_learning
    
    def _calculate_correlation(self, pre_activities: List[float],
                             post_activities: List[float]) -> float:
        """Calculate spike-timing correlation between pre and post activities"""
        if not pre_activities or not post_activities:
            return 0.0
        
        # Simple correlation calculation
        pre_mean = np.mean(pre_activities)
        post_mean = np.mean(post_activities)
        
        covariance = np.mean([(p - pre_mean) * (q - post_mean)
                             for p, q in zip(pre_activities, post_activities)])
        pre_std = np.std(pre_activities)
        post_std = np.std(post_activities)
        
        if pre_std > 0 and post_std > 0:
            return covariance / (pre_std * post_std)
        return 0.0


class CustomFunctionPlasticity(CustomPlasticityRule):
    """
    Plasticity rule based on a custom user-provided function.
    
    This allows users to implement completely custom plasticity rules
    by providing their own function that takes synaptic information
    and spike activities and returns weight changes.
    """
    
    def __init__(self, name: str, update_function: Callable,
                 config: Optional[PlasticityRuleConfig] = None):
        super().__init__(name, config)
        if self.rule_type == "custom":
            self.rule_type = "custom_function"
        self.update_function = update_function
    
    def update(self, synapse_info: Dict[str, Any],
               pre_synaptic_activities: List[float],
               post_synaptic_activities: List[float],
               time_delta: float) -> float:
        """
        Apply custom user function.
        
        The update function should have signature:
        (synapse_info, pre_activities, post_activities, time_delta) -> float
        """
        return self.update_function(synapse_info, pre_synaptic_activities,
                                   post_synaptic_activities, time_delta)


class PlasticityRuleManager:
    """
    Manages multiple plasticity rules for a brain.
    
    Allows switching between different plasticity rules,
    combining multiple rules, and managing rule configurations.
    """
    
    def __init__(self):
        self.rules: Dict[str, CustomPlasticityRule] = {}
        self.default_rule: Optional[str] = None
    
    def add_rule(self, rule: CustomPlasticityRule, is_default: bool = False):
        """Add a plasticity rule to the manager"""
        self.rules[rule.name] = rule
        if is_default or self.default_rule is None:
            self.default_rule = rule.name
    
    def remove_rule(self, name: str) -> bool:
        """Remove a plasticity rule"""
        if name in self.rules:
            del self.rules[name]
            if self.default_rule == name:
                self.default_rule = None
            return True
        return False
    
    def get_rule(self, name: str) -> Optional[CustomPlasticityRule]:
        """Get a specific plasticity rule"""
        return self.rules.get(name)
    
    def apply_rule(self, rule_name: str, synapse_info: Dict[str, Any],
                   pre_activities: List[float], post_activities: List[float],
                   time_delta: float) -> float:
        """Apply a specific rule to synapse"""
        rule = self.get_rule(rule_name)
        if rule:
            return rule.apply_rule(synapse_info, pre_activities,
                                 post_activities, time_delta)
        return 0.0
    
    def apply_default_rule(self, synapse_info: Dict[str, Any],
                          pre_activities: List[float], post_activities: List[float],
                          time_delta: float) -> float:
        """Apply the default rule to synapse"""
        if self.default_rule:
            return self.apply_rule(self.default_rule, synapse_info,
                                 pre_activities, post_activities, time_delta)
        return 0.0
    
    def enable_rule(self, name: str, enabled: bool = True) -> bool:
        """Enable or disable a rule"""
        rule = self.get_rule(name)
        if rule:
            rule.enabled = enabled
            return True
        return False
    
    def get_rule_statistics(self) -> Dict[str, Any]:
        """Get statistics for all rules"""
        return {
            name: rule.get_statistics()
            for name, rule in self.rules.items()
        }
    
    def export_config(self) -> Dict[str, Any]:
        """Export all rule configurations"""
        return {
            name: rule.export_config()
            for name, rule in self.rules.items()
        }
    
    def import_config(self, config_dict: Dict[str, Any]) -> bool:
        """Import rule configurations"""
        try:
            # Note: This is a simplified implementation
            # In practice, you'd need to handle rule serialization/deserialization
            for name, rule_config in config_dict.items():
                if name in self.rules:
                    self.rules[name].config = PlasticityRuleConfig.from_dict(rule_config)
            return True
        except Exception as e:
            print(f"Error importing plasticity rule config: {e}")
            return False


class AdvancedBrainInterface:
    """
    Advanced interface for controlling brain plasticity rules.
    
    Provides a unified interface for managing plasticity rules
    and integrating them with the brain simulation.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.plasticity_manager = PlasticityRuleManager()
        
        # Set up event handlers for spike activity
        self.pre_spike_handlers: List[Callable] = []
        self.post_spike_handlers: List[Callable] = []
    
    def add_plasticity_rule(self, rule: CustomPlasticityRule,
                           is_default: bool = False):
        """Add a custom plasticity rule"""
        self.plasticity_manager.add_rule(rule, is_default)
    
    def set_current_plasticity_rule(self, rule_name: str) -> bool:
        """Set the current plasticity rule (for simple systems)"""
        # This would require integration with the brain's internal plasticity system
        return False
    
    def record_spike_event(self, neuron_type: str, neuron_id: int,
                          activity: float, timestamp: float):
        """
        Record a spike event that can be used by plasticity rules.
        
        This is called internally by the brain when neurons fire.
        """
        # This would need to integrate with the brain's spike recording system
        pass
    
    def apply_advanced_plasticity(self, synapse_info: Dict[str, Any],
                                  pre_activities: List[float],
                                  post_activities: List[float],
                                  time_delta: float) -> float:
        """
        Apply advanced plasticity rules to a synapse.
        
        This would be integrated with the brain's existing plasticity system.
        """
        return self.plasticity_manager.apply_default_rule(
            synapse_info, pre_activities, post_activities, time_delta)
    
    def get_rule_based_statistics(self) -> Dict[str, Any]:
        """Get statistics for all plasticity rules"""
        return self.plasticity_manager.get_rule_statistics()


# Factory functions for creating common plasticity rules
def create_stdp_plus_rule(name: str, ltp_weight: float = 0.01,
                         ltd_weight: float = 0.012,
                         time_constant: float = 20.0) -> STDPPlus:
    """Create an enhanced STDP rule"""
    config = PlasticityRuleConfig(
        name=name,
        rule_type="stdp_plus",
        ltp_weight=ltp_weight,
        ltd_weight=ltd_weight,
        time_constant=time_constant,
        reward_scale=1.0
    )
    return STDPPlus(name, config)


def create_reward_modulated_rule(name: str, prediction_error_weight: float = 1.0,
                                 reward_scale: float = 1.0) -> RewardModulatedPlasticity:
    """Create a reward-modulated plasticity rule"""
    config = PlasticityRuleConfig(
        name=name,
        rule_type="reward_modulated",
        prediction_error_weight=prediction_error_weight,
        reward_scale=reward_scale
    )
    return RewardModulatedPlasticity(name, config)


def create_custom_function_rule(name: str, update_function: Callable,
                                **config_params) -> CustomFunctionPlasticity:
    """Create a custom function-based plasticity rule"""
    config = PlasticityRuleConfig(
        name=name,
        rule_type="custom_function",
        **config_params
    )
    return CustomFunctionPlasticity(name, update_function, config)


def create_compound_rule(base_rule: CustomPlasticityRule,
                         modifier_rules: List[CustomPlasticityRule]) -> CustomPlasticityRule:
    """
    Create a compound plasticity rule that combines multiple rules.
    
    The compound rule applies multiple rules in sequence, with each
    rule's contribution weighted by a coefficient.
    """
    class CompoundRule(CustomPlasticityRule):
        def __init__(self, name: str, base_rule: CustomPlasticityRule,
                     modifier_rules: List[CustomPlasticityRule]):
            config = PlasticityRuleConfig(name, "compound")
            super().__init__(name, config)
            self.base_rule = base_rule
            self.modifier_rules = modifier_rules
            
            # Set coefficients from base rule config
            self.coefficients = {
                rule.name: rule.config.parameters.get('coefficient', 1.0)
                for rule in modifier_rules
            }
        
        def update(self, synapse_info: Dict[str, Any],
                   pre_synaptic_activities: List[float],
                   post_synaptic_activities: List[float],
                   time_delta: float) -> float:
            # Calculate base contribution
            base_contribution = self.base_rule.update(
                synapse_info, pre_synaptic_activities, post_synaptic_activities, time_delta)
            
            # Add modifier contributions
            total_contribution = base_contribution
            for modifier in self.modifier_rules:
                modifier_contribution = modifier.update(
                    synapse_info, pre_synaptic_activities, post_synaptic_activities, time_delta)
                coefficient = self.coefficients.get(modifier.name, 1.0)
                total_contribution += coefficient * modifier_contribution
            
            return total_contribution
    
    return CompoundRule(f"compound_{name}", base_rule, modifier_rules)
