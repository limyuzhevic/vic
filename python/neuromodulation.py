"""
Neuromodulation NLM functionality.

This module contains neuromodulation-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import (
    Neuromodulator,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    Brain,
    SimpleWorld,
    AgentBrain,
)

def create_dopamine_neuromodulator(scale: float = 1.0, 
                                   learning_rate: float = 0.001) -> Dopamine:
    """
    Create a dopamine neuromodulator.
    
    Args:
        scale: Neuromodulation scale factor.
        learning_rate: Learning rate for reward prediction.
        
    Returns:
        Dopamine neuromodulator instance.
    """
    neuromodulator = Dopamine(scale)
    neuromodulator.setLearningRate(learning_rate)
    return neuromodulator

def create_curiosity_neuromodulator(enable: bool = True,
                                    exploration_rate: float = 0.5) -> Curiosity:
    """
    Create a curiosity neuromodulator.
    
    Args:
        enable: Whether curiosity-driven exploration is enabled.
        exploration_rate: Rate of exploration behavior.
        
    Returns:
        Curiosity neuromodulator instance.
    """
    neuromodulator = Curiosity(enable, exploration_rate)
    return neuromodulator

def create_novelty_neuromodulator(enable: bool = True,
                                   sensitivity: float = 1.0) -> Novelty:
    """
    Create a novelty neuromodulator.
    
    Args:
        enable: Whether novelty detection is enabled.
        sensitivity: Sensitivity to novel stimuli.
        
    Returns:
        Novelty neuromodulator instance.
    """
    neuromodulator = Novelty(enable, sensitivity)
    return neuromodulator

def create_prediction_error_neuromodulator() -> PredictionError:
    """
    Create a prediction error neuromodulator.
    
    Returns:
        Prediction error neuromodulator instance.
    """
    return PredictionError()

def create_enhanced_neuromodulation(config: Optional[Dict[str, Any]] = None,
                                    brain: Optional[Brain] = None,
                                    world: Optional[SimpleWorld] = None) -> Dict[str, Any]:
    """
    Create and configure enhanced neuromodulation systems.
    
    Args:
        config: Configuration dictionary.
        brain: Brain instance to attach neuromodulators to.
        world: World instance for neuromodulation context.
        
    Returns:
        Dictionary containing neuromodulation systems.
    """
    neuromodulators = {}
    
    if not config:
        config = {}
    
    # Create dopamine system
    if config.get('enable_dopamine', True):
        scale = config.get('dopamine_scale', 1.0)
        learning_rate = config.get('dopamine_learning_rate', 0.001)
        neuromodulators['dopamine'] = create_dopamine_neuromodulator(scale, learning_rate)
    
    # Create curiosity system
    if config.get('enable_curiosity', True):
        enable = config.get('curiosity_enable', True)
        exploration_rate = config.get('curiosity_exploration_rate', 0.5)
        neuromodulators['curiosity'] = create_curiosity_neuromodulator(enable, exploration_rate)
    
    # Create novelty system
    if config.get('enable_novelty', True):
        enable = config.get('novelty_enable', True)
        sensitivity = config.get('novelty_sensitivity', 1.0)
        neuromodulators['novelty'] = create_novelty_neuromodulator(enable, sensitivity)
    
    # Create prediction error system
    if config.get('enable_prediction_error', True):
        neuromodulators['prediction_error'] = create_prediction_error_neuromodulator()
    
    # Attach neuromodulators to brain if provided
    if brain:
        for name, neuromodulator in neuromodulators.items():
            if hasattr(brain, f'set{name.capitalize()}'):
                getattr(brain, f'set{name.capitalize()}')(neuromodulator)
    
    return neuromodulators

def analyze_neuromodulation_state(brain: Brain) -> Dict[str, Any]:
    """
    Analyze the state of neuromodulation systems in a brain.
    
    Args:
        brain: Brain instance to analyze.
        
    Returns:
        Dictionary containing neuromodulation state analysis.
    """
    analysis = {}
    
    # Check each neuromodulation system
    if hasattr(brain, 'getDopamine'):
        dopamine = brain.getDopamine()
        if dopamine:
            analysis['dopamine'] = {
                'enabled': dopamine.getScale() > 0,
                'level': dopamine.getScale(),
                'learning_rate': dopamine.getLearningRate(),
            }
    
    if hasattr(brain, 'getCuriosity'):
        curiosity = brain.getCuriosity()
        if curiosity:
            analysis['curiosity'] = {
                'enabled': curiosity.getLevel() > 0,
                'level': curiosity.getLevel(),
                'exploration_rate': curiosity.getExplorationRate(),
            }
    
    if hasattr(brain, 'getNovelty'):
        novelty = brain.getNovelty()
        if novelty:
            analysis['novelty'] = {
                'enabled': novelty.getLevel() > 0,
                'level': novelty.getLevel(),
                'sensitivity': novelty.getSensitivity(),
            }
    
    if hasattr(brain, 'getPredictionErrorSignal'):
        prediction_error = brain.getPredictionErrorSignal()
        if prediction_error:
            analysis['prediction_error'] = {
                'enabled': prediction_error.getLevel() > 0,
                'level': prediction_error.getLevel(),
                'error_strength': prediction_error.getErrorStrength(),
            }
    
    return analysis

def create_learning_neuromodulation(agent: AgentBrain) -> Dict[str, Any]:
    """
    Create neuromodulation systems for learning agents.
    
    Args:
        agent: Agent brain instance.
        
    Returns:
        Dictionary containing learning neuromodulation systems.
    """
    neuromodulators = {}
    
    # Enable reward-based learning
    neuromodulators['reward'] = agent.enableRewardModulation(True)
    
    # Enable curiosity-driven exploration
    neuromodulators['curiosity'] = agent.enableCuriosity(True)
    
    # Enable structural plasticity
    neuromodulators['structural_plasticity'] = agent.enableStructuralPlasticity(True)
    
    # Enable development
    neuromodulators['development'] = agent.enableDevelopment(True)
    
    return neuromodulators