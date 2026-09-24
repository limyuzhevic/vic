"""
Cognitive systems for NLM - planning, concept formation, and attention.

This module provides the cognitive systems that enable higher-level
information processing in the NLM artificial brain system.
"""

import sys
from typing import List, Dict, Any

__all__ = [
    'NeuralPlanner',
    'ConceptFormation',
    'AttentionalSelection',
]


class NeuralPlanner:
    """Neural planner for action planning and decision making.
    
    The neural planner evaluates action sequences and selects optimal
    actions based on neural activity patterns and environmental context.
    It supports hierarchical planning and value-based decision making.
    
    Key features:
    - Action sequence evaluation
    - Value-based planning
    - Hierarchical planning capabilities
    - Integration with working memory and attention
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> planner = brain.getPlanner()
        >>> if planner:
        ...     planner.initialize(brain_ptr)
        ...     # Plan actions based on current state
        ...     plan = planner.planActions(current_state)
    """
    
    def __init__(self, planner_ptr=None):
        self._planner = planner_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize neural planner.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._planner:
            return self._planner.initialize(brain_ptr)
        return False
    
    def setPlanningDepth(self, depth: int) -> bool:
        """Set planning depth.
        
        Args:
            depth: Planning depth (number of steps ahead).
            
        Returns:
            bool: True if depth set successfully.
        """
        if self._planner:
            return self._planner.setPlanningDepth(depth)
        return False
    
    def planActions(self, current_state: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Plan actions for current state.
        
        Args:
            current_state: Current brain/environment state.
            
        Returns:
            List[Dict[str, Any]]: List of planned actions with values.
        """
        if self._planner:
            return self._planner.planActions(current_state)
        return []
    
    def __repr__(self) -> str:
        if self._planner:
            return f"<NeuralPlanner: initialized>"
        return "<NeuralPlanner: uninitialized>"


class ConceptFormation:
    """Concept formation for pattern discovery and abstraction.
    
    Concept formation discovers patterns and forms abstractions from
    neural activity patterns. It supports concept stability, generalization,
    and merging of similar concepts.
    
    Key features:
    - Pattern discovery and concept creation
    - Concept stability tracking
    - Generalization ability measurement
    - Concept merging and refinement
    - Similarity-based classification
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> concept_formation = brain.getConceptFormation()
        >>> if concept_formation:
        ...     concept_formation.initialize(brain_ptr)
        ...     
        ...     # Present experience to form concepts
        ...     concept_id = concept_formation.presentExperience(pattern, features, reward, time)
        ...     
        ...     # Check concept stability
        ...     if concept_formation.isConceptStable(concept_id):
        ...         prototype = concept_formation.getConceptPrototype(concept_id)
    """
    
    def __init__(self, concept_formation_ptr=None):
        self._concept_formation = concept_formation_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize concept formation.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._concept_formation:
            return self._concept_formation.initialize(brain_ptr)
        return False
    
    def presentExperience(self, pattern: List[float], features: List[float], 
                         reward: float, current_time: int) -> int:
        """Present experience to form concepts.
        
        Args:
            pattern: Neural activation pattern.
            features: Extracted features from pattern.
            reward: Reward value associated with experience.
            current_time: Current simulation time.
            
        Returns:
            int: Concept ID if new concept formed, 0 otherwise.
        """
        if self._concept_formation:
            return self._concept_formation.presentExperience(pattern, features, reward, current_time)
        return 0
    
    def isNovel(self, pattern: List[float], similarity_threshold: float = 0.75) -> bool:
        """Check if pattern is novel.
        
        Args:
            pattern: Pattern to check.
            similarity_threshold: Similarity threshold for novelty detection.
            
        Returns:
            bool: True if pattern is novel.
        """
        if self._concept_formation:
            return self._concept_formation.isNovel(pattern, similarity_threshold)
        return True
    
    def isConceptStable(self, concept_id: int) -> bool:
        """Check if concept is stable.
        
        Args:
            concept_id: Concept ID to check.
            
        Returns:
            bool: True if concept is stable.
        """
        if self._concept_formation:
            return self._concept_formation.isConceptStable(concept_id)
        return False
    
    def getConceptPrototype(self, concept_id: int) -> List[float]:
        """Get concept prototype.
        
        Args:
            concept_id: Concept ID.
            
        Returns:
            List[float]: Concept prototype pattern.
        """
        if self._concept_formation:
            return self._concept_formation.getConceptPrototype(concept_id)
        return []
    
    def getConceptStability(self, concept_id: int) -> float:
        """Get concept stability.
        
        Args:
            concept_id: Concept ID.
            
        Returns:
            float: Concept stability (0.0 to 1.0).
        """
        if self._concept_formation:
            return self._concept_formation.getConceptStability(concept_id)
        return 0.0
    
    def getGeneralizationAbility(self, concept_id: int) -> float:
        """Get concept generalization ability.
        
        Args:
            concept_id: Concept ID.
            
        Returns:
            float: Generalization ability (0.0 to 1.0).
        """
        if self._concept_formation:
            return self._concept_formation.getGeneralizationAbility(concept_id)
        return 0.0
    
    def clear(self) -> bool:
        """Clear all concepts.
        
        Returns:
            bool: True if cleared successfully.
        """
        if self._concept_formation:
            return self._concept_formation.clear()
        return False
    
    def __repr__(self) -> str:
        if self._concept_formation:
            return f"<ConceptFormation: initialized>"
        return "<ConceptFormation: uninitialized>"


class AttentionalSelection:
    """Attentional selection for focus and information filtering.
    
    Attentional selection controls which neural representations receive
    processing priority. It implements competitive dynamics between
    neural populations to select the most relevant information for
    current goals.
    
    Key features:
    - Competitive selection among neural populations
    - Bottom-up salience and top-down bias
    - Inhibition of irrelevant representations
    - Dynamic allocation of attentional resources
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> attention = brain.getAttention()
        >>> if attention:
        ...     attention.initialize(brain_ptr)
        ...     
        ...     # Process competition among neurons
        ...     competitors = [neuron1, neuron2, neuron3]
        ...     winners = attention.processCompetition(competitors)
        ...     
        ...     # Apply top-down bias
        ...     attention.applyTopDownBias(neuron1, bias_strength)
        ...     attention.applyBottomUpSalience(neuron2, salience_strength)
    """
    
    def __init__(self, attention_ptr=None):
        self._attention = attention_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize attentional selection.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._attention:
            return self._attention.initialize(brain_ptr)
        return False
    
    def setInhibitionStrength(self, strength: float) -> bool:
        """Set inhibition strength.
        
        Args:
            strength: Inhibition strength (0.0 to 1.0).
            
        Returns:
            bool: True if strength set successfully.
        """
        if self._attention:
            return self._attention.setInhibitionStrength(strength)
        return False
    
    def setExcitationStrength(self, strength: float) -> bool:
        """Set excitation strength.
        
        Args:
            strength: Excitation strength (0.0 to 1.0).
            
        Returns:
            bool: True if strength set successfully.
        """
        if self._attention:
            return self._attention.setExcitationStrength(strength)
        return False
    
    def setCompetitionThreshold(self, threshold: float) -> bool:
        """Set competition threshold.
        
        Args:
            threshold: Competition threshold (0.0 to 1.0).
            
        Returns:
            bool: True if threshold set successfully.
        """
        if self._attention:
            return self._attention.setCompetitionThreshold(threshold)
        return False
    
    def processCompetition(self, competitors: List[int], 
                          global_inhibition: float = 1.0) -> List[int]:
        """Process competition among neural representations.
        
        Args:
            competitors: List of competing neuron IDs.
            global_inhibition: Global inhibition level.
            
        Returns:
            List[int]: List of winner neuron IDs.
        """
        if self._attention:
            return self._attention.processCompetition(competitors, global_inhibition)
        return []
    
    def applyTopDownBias(self, neuron_id: int, bias_strength: float) -> bool:
        """Apply top-down bias to neuron.
        
        Args:
            neuron_id: Target neuron ID.
            bias_strength: Bias strength (positive or negative).
            
        Returns:
            bool: True if bias applied successfully.
        """
        if self._attention:
            return self._attention.applyTopDownBias(neuron_id, bias_strength)
        return False
    
    def applyBottomUpSalience(self, neuron_id: int, salience_strength: float) -> bool:
        """Apply bottom-up salience to neuron.
        
        Args:
            neuron_id: Target neuron ID.
            salience_strength: Salience strength (positive).
            
        Returns:
            bool: True if salience applied successfully.
        """
        if self._attention:
            return self._attention.applyBottomUpSalience(neuron_id, salience_strength)
        return False
    
    def isAttended(self, neuron_id: int) -> bool:
        """Check if neuron is attended.
        
        Args:
            neuron_id: Target neuron ID.
            
        Returns:
            bool: True if neuron is attended.
        """
        if self._attention:
            return self._attention.isAttended(neuron_id)
        return False
    
    def focusOnRegion(self, region_id: int) -> bool:
        """Focus attention on brain region.
        
        Args:
            region_id: Target region ID.
            
        Returns:
            bool: True if focus set successfully.
        """
        if self._attention:
            return self._attention.focusOnRegion(region_id)
        return False
    
    def releaseAttention(self) -> bool:
        """Release attentional focus.
        
        Returns:
            bool: True if attention released successfully.
        """
        if self._attention:
            return self._attention.releaseAttention()
        return False
    
    def update(self, dt: float) -> bool:
        """Update attentional dynamics.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        if self._attention:
            return self._attention.update(dt)
        return False
    
    def reset(self) -> bool:
        """Reset attentional state.
        
        Returns:
            bool: True if reset successfully.
        """
        if self._attention:
            return self._attention.reset()
        return False
    
    def __repr__(self) -> str:
        if self._attention:
            return f"<AttentionalSelection: initialized>"
        return "<AttentionalSelection: uninitialized>"
