"""
Brain NLM functionality.

This module contains brain-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import Brain, createBrain, Config
from .core import ConfigurationError, InitializationError

def create_enhanced_brain(config: Optional[Dict[str, Any]] = None, 
                         enable_features: Optional[List[str]] = None) -> Brain:
    """
    Create an enhanced brain with additional features.
    
    Args:
        config: Configuration dictionary.
        enable_features: List of features to enable (e.g., 'reward', 'curiosity').
        
    Returns:
        Enhanced Brain instance.
    """
    nlm_config = Config()
    
    if config:
        for key, value in config.items():
            nlm_config.set(key, value)
    
    # Add enhanced default configuration
    nlm_config.set("brain.neuron_count", nlm_config.getOr("brain.neuron_count", 1000))
    nlm_config.set("plasticity.stdp.enable", True)
    nlm_config.set("plasticity.hebbian.enable", True)
    nlm_config.set("neuromod.dopamine.scale", 1.0)
    nlm_config.set("neuromod.curiosity.enable", True)
    nlm_config.set("neuromod.novelty.enable", True)
    
    try:
        brain = createBrain(nlm_config)
        brain.initialize()
        return brain
    except Exception as e:
        raise InitializationError(f"Failed to create enhanced brain: {e}") from e

def create_development_brain(config: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a brain with developmental capabilities.
    
    Args:
        config: Configuration dictionary.
        
    Returns:
        Development brain instance.
    """
    nlm_config = Config()
    
    if config:
        for key, value in config.items():
            nlm_config.set(key, value)
    
    # Enable developmental features
    nlm_config.set("development.enabled", True)
    nlm_config.set("development.critical_period", 100)
    nlm_config.set("development.maturation_rate", 0.001)
    
    brain = createBrain(nlm_config)
    brain.initialize()
    return brain

def create_curiosity_brain(config: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a brain with curiosity-driven exploration.
    
    Args:
        config: Configuration dictionary.
        
    Returns:
        Curiosity-driven brain instance.
    """
    nlm_config = Config()
    
    if config:
        for key, value in config.items():
            nlm_config.set(key, value)
    
    # Enable curiosity features
    nlm_config.set("neuromod.curiosity.enable", True)
    nlm_config.set("neuromod.novelty.enable", True)
    nlm_config.set("neuromod.curiosity.exploration_rate", 0.5)
    
    brain = createBrain(nlm_config)
    brain.initialize()
    return brain

def export_brain_state(brain: Brain) -> Dict[str, Any]:
    """
    Export brain state to a Python dictionary.
    
    Args:
        brain: Brain instance to export.
        
    Returns:
        Dictionary containing brain state.
    """
    return {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'active_neuron_count': brain.getActiveNeuronCount(),
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'total_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'config_keys': brain.getConfig().getKeys(),
    }

def import_brain_state(brain: Brain, state: Dict[str, Any]) -> bool:
    """
    Import brain state from a Python dictionary.
    
    Args:
        brain: Brain instance to import state into.
        state: Dictionary containing brain state.
        
    Returns:
        True if import successful, False otherwise.
    """
    try:
        # Note: This is a placeholder implementation
        # Actual implementation would need to save/load brain state
        print("Import brain state: Not implemented yet")
        return False
    except Exception:
        return False