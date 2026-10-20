"""
Cognition NLM functionality.

This module contains cognition-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import (
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
    Brain,
)

def create_neural_planner(planning_depth: int = 5,
                          confidence_threshold: float = 0.5) -> NeuralPlanner:
    """
    Create a neural planner with specified parameters.
    
    Args:
        planning_depth: Depth of planning horizon.
        confidence_threshold: Confidence threshold for action selection.
        
    Returns:
        Neural planner instance.
    """
    planner = NeuralPlanner()
    planner.setPlanningDepth(planning_depth)
    planner.setConfidenceThreshold(confidence_threshold)
    return planner

def create_concept_formation(learning_rate: float = 0.01,
                             stability_threshold: float = 0.1) -> ConceptFormation:
    """
    Create a concept formation system.
    
    Args:
        learning_rate: Rate of concept learning.
        stability_threshold: Threshold for concept stability.
        
    Returns:
        Concept formation instance.
    """
    concept_formation = ConceptFormation()
    concept_formation.setLearningRate(learning_rate)
    concept_formation.setStabilityThreshold(stability_threshold)
    return concept_formation

def create_attentional_selection(inhibition_strength: float = 0.5,
                                excitation_strength: float = 1.5) -> AttentionalSelection:
    """
    Create an attentional selection system.
    
    Args:
        inhibition_strength: Strength of competitive inhibition.
        excitation_strength: Strength of excitatory connections.
        
    Returns:
        Attentional selection instance.
    """
    attention = AttentionalSelection()
    attention.setInhibitionStrength(inhibition_strength)
    attention.setExcitationStrength(excitation_strength)
    return attention

def create_cognitive_system(config: Optional[Dict[str, Any]] = None,
                           brain: Optional[Brain] = None) -> Dict[str, Any]:
    """
    Create and configure cognitive systems.
    
    Args:
        config: Configuration dictionary.
        brain: Brain instance to attach cognitive systems to.
        
    Returns:
        Dictionary containing cognitive systems.
    """
    cognitive_systems = {}
    
    if not config:
        config = {}
    
    # Create neural planner
    if config.get('enable_planning', True):
        planning_depth = config.get('planning_depth', 5)
        confidence_threshold = config.get('confidence_threshold', 0.5)
        cognitive_systems['planner'] = create_neural_planner(planning_depth, confidence_threshold)
    
    # Create concept formation
    if config.get('enable_concept_formation', True):
        learning_rate = config.get('concept_learning_rate', 0.01)
        stability_threshold = config.get('concept_stability_threshold', 0.1)
        cognitive_systems['concept_formation'] = create_concept_formation(learning_rate, stability_threshold)
    
    # Create attentional selection
    if config.get('enable_attention', True):
        inhibition_strength = config.get('attention_inhibition_strength', 0.5)
        excitation_strength = config.get('attention_excitation_strength', 1.5)
        cognitive_systems['attention'] = create_attentional_selection(inhibition_strength, excitation_strength)
    
    # Attach cognitive systems to brain if provided
    if brain:
        for name, cognitive_system in cognitive_systems.items():
            if hasattr(brain, f'set{name.capitalize()}'):
                getattr(brain, f'set{name.capitalize()}')(cognitive_system)
    
    return cognitive_systems

def analyze_cognitive_state(brain: Brain) -> Dict[str, Any]:
    """
    Analyze the cognitive state of a brain.
    
    Args:
        brain: Brain instance to analyze.
        
    Returns:
        Dictionary containing cognitive state analysis.
    """
    analysis = {}
    
    # Check each cognitive system
    if hasattr(brain, 'getPlanner'):
        planner = brain.getPlanner()
        if planner:
            analysis['planner'] = {
                'enabled': planner.getPlanningDepth() > 0,
                'planning_depth': planner.getPlanningDepth(),
                'confidence_threshold': planner.getConfidenceThreshold(),
            }
    
    if hasattr(brain, 'getConceptFormation'):
        concept_formation = brain.getConceptFormation()
        if concept_formation:
            analysis['concept_formation'] = {
                'enabled': concept_formation.getLearningRate() > 0,
                'learning_rate': concept_formation.getLearningRate(),
                'stability_threshold': concept_formation.getStabilityThreshold(),
            }
    
    if hasattr(brain, 'getAttention'):
        attention = brain.getAttention()
        if attention:
            analysis['attention'] = {
                'enabled': attention.getInhibitionStrength() > 0,
                'inhibition_strength': attention.getInhibitionStrength(),
                'excitation_strength': attention.getExcitationStrength(),
            }
    
    return analysis

def create_advanced_cognitive_agent(brain: Brain, config: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a brain with advanced cognitive capabilities.
    
    Args:
        brain: Brain instance to enhance.
        config: Optional configuration dictionary.
        
    Returns:
        Enhanced brain instance.
    """
    # Configure cognitive systems
    cognitive_config = {}
    
    if config:
        cognitive_config.update(config)
    
    # Enable all cognitive systems by default
    cognitive_config['enable_planning'] = True
    cognitive_config['enable_concept_formation'] = True
    cognitive_config['enable_attention'] = True
    
    # Create cognitive systems and attach to brain
    cognitive_systems = create_cognitive_system(cognitive_config, brain)
    
    return brain