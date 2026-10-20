"""
Development NLM functionality.

This module contains development-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import (
    DevelopmentalSystem,
    Synaptogenesis,
    Pruning,
    Maturation,
    Brain,
)

def create_development_system(plasticity_enabled: bool = True,
                              critical_period: float = 100.0,
                              maturation_rate: float = 0.001) -> DevelopmentalSystem:
    """
    Create a development system with specified parameters.
    
    Args:
        plasticity_enabled: Whether plasticity is enabled during development.
        critical_period: Duration of critical period.
        maturation_rate: Rate of maturation.
        
    Returns:
        Development system instance.
    """
    development = DevelopmentalSystem()
    development.setPlasticityEnabled(plasticity_enabled)
    development.setCriticalPeriod(critical_period)
    development.setMaturationRate(maturation_rate)
    return development

def create_synaptogenesis(synaptogenesis_rate: float = 0.0001) -> Synaptogenesis:
    """
    Create a synaptogenesis system.
    
    Args:
        synaptogenesis_rate: Rate of new synapse formation.
        
    Returns:
        Synaptogenesis system instance.
    """
    synaptogenesis = Synaptogenesis()
    synaptogenesis.setSynaptogenesisRate(synaptogenesis_rate)
    return synaptogenesis

def create_pruning(pruning_rate: float = 0.00001,
                   stability_threshold: float = 0.1) -> Pruning:
    """
    Create a pruning system.
    
    Args:
        pruning_rate: Rate of synapse removal.
        stability_threshold: Threshold for synapse stability.
        
    Returns:
        Pruning system instance.
    """
    pruning = Pruning()
    pruning.setPruningRate(pruning_rate)
    pruning.setStabilityThreshold(stability_threshold)
    return pruning

def create_maturation(maturation_rate: float = 0.001,
                      adult_stability: float = 0.9) -> Maturation:
    """
    Create a maturation system.
    
    Args:
        maturation_rate: Rate of maturation.
        adult_stability: Stability threshold for adulthood.
        
    Returns:
        Maturation system instance.
    """
    maturation = Maturation()
    maturation.setMaturationRate(maturation_rate)
    maturation.setAdultStability(adult_stability)
    return maturation

def create_developmental_trajectory(config: Optional[Dict[str, Any]] = None,
                                    brain: Optional[Brain] = None) -> Dict[str, Any]:
    """
    Create and configure developmental trajectory.
    
    Args:
        config: Configuration dictionary.
        brain: Brain instance to attach development systems to.
        
    Returns:
        Dictionary containing developmental systems.
    """
    developmental_systems = {}
    
    if not config:
        config = {}
    
    # Create developmental system
    if config.get('enable_development', True):
        plasticity_enabled = config.get('plasticity_enabled', True)
        critical_period = config.get('critical_period', 100.0)
        maturation_rate = config.get('maturation_rate', 0.001)
        developmental_systems['development'] = create_development_system(plasticity_enabled, critical_period, maturation_rate)
    
    # Create synaptogenesis
    if config.get('enable_synaptogenesis', True):
        synaptogenesis_rate = config.get('synaptogenesis_rate', 0.0001)
        developmental_systems['synaptogenesis'] = create_synaptogenesis(synaptogenesis_rate)
    
    # Create pruning
    if config.get('enable_pruning', True):
        pruning_rate = config.get('pruning_rate', 0.00001)
        stability_threshold = config.get('pruning_stability_threshold', 0.1)
        developmental_systems['pruning'] = create_pruning(pruning_rate, stability_threshold)
    
    # Create maturation
    if config.get('enable_maturation', True):
        maturation_rate = config.get('maturation_rate', 0.001)
        adult_stability = config.get('adult_stability', 0.9)
        developmental_systems['maturation'] = create_maturation(maturation_rate, adult_stability)
    
    # Attach developmental systems to brain if provided
    if brain:
        for name, developmental_system in developmental_systems.items():
            if hasattr(brain, f'set{name.capitalize()}'):
                getattr(brain, f'set{name.capitalize()}')(developmental_system)
    
    return developmental_systems

def analyze_development_stage(brain: Brain) -> Dict[str, Any]:
    """
    Analyze the development stage of a brain.
    
    Args:
        brain: Brain instance to analyze.
        
    Returns:
        Dictionary containing development stage analysis.
    """
    analysis = {}
    
    # Get developmental stage
    if hasattr(brain, 'getDevelopmentalStage'):
        stage = brain.getDevelopmentalStage()
        analysis['stage'] = {
            'current_stage': stage,
            'plasticity_rate': get_plasticity_rate_for_stage(stage),
            'stability': get_stability_for_stage(stage),
        }
    
    # Check developmental systems
    if hasattr(brain, 'getDevelopmentSystem'):
        development = brain.getDevelopmentSystem()
        if development:
            analysis['development_system'] = {
                'enabled': development.isPlasticityEnabled(),
                'critical_period': development.getCriticalPeriod(),
                'maturation_rate': development.getMaturationRate(),
            }
    
    return analysis

def get_plasticity_rate_for_stage(stage) -> float:
    """
    Get plasticity rate for a specific developmental stage.
    
    Args:
        stage: Developmental stage.
        
    Returns:
        Plasticity rate for the stage.
    """
    plasticity_rates = {
        'Initial': 1.0,
        'CriticalPeriod': 0.8,
        'Maturation': 0.5,
        'Adult': 0.2,
        'Aging': 0.1,
    }
    return plasticity_rates.get(str(stage), 0.5)

def get_stability_for_stage(stage) -> float:
    """
    Get stability for a specific developmental stage.
    
    Args:
        stage: Developmental stage.
        
    Returns:
        Stability for the stage.
    """
    stability = {
        'Initial': 0.1,
        'CriticalPeriod': 0.3,
        'Maturation': 0.6,
        'Adult': 0.8,
        'Aging': 0.5,
    }
    return stability.get(str(stage), 0.5)

def create_ageing_simulation(brain: Brain, ageing_rate: float = 0.01) -> Brain:
    """
    Create an ageing simulation for a brain.
    
    Args:
        brain: Brain instance to age.
        ageing_rate: Rate of ageing (0.0 to 1.0).
        
    Returns:
        Aged brain instance.
    """
    # Set ageing configuration
    config = {
        'ageing_rate': ageing_rate,
        'plasticity_reduction': 0.9,
        'memory_decay': 0.8,
        'learning_rate_reduction': 0.7,
    }
    
    # Apply ageing effects
    for key, value in config.items():
        if hasattr(brain, f'set{key.title()}'):
            getattr(brain, f'set{key.title()}')(value)
    
    return brain