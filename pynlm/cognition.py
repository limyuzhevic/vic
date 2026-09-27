from dataclasses import dataclass, field
"""
Cognitive Systems - Planning, Concept Formation, and Attention
=============================================================

Implementation of cognitive systems for neural planning and intelligent behavior:
- Neural planning and action selection
- Concept formation from experience patterns
- Attentional selection with competitive dynamics
- Predictive planning and decision making
- Self-model and social learning
"""

from typing import List, Dict, Any, Optional
import numpy as np
@dataclass
class PlanningState:
    """Planning state for cognitive decision making."""
    
    # Planning parameters
    planning_depth: int = 5
    inhibition_strength: float = 0.5
    excitation_strength: float = 1.5
    
    # Current plan
    current_plan: List[Dict[str, Any]] = field(default_factory=list)
    
    # Cognitive load
    cognitive_load: float = 0.0
    
    # Concept activation
    concept_activations: Dict[int, float] = field(default_factory=dict)
class NeuralPlanner:
    """
    Neural planner for action planning and decision making.
    
    Implements planning using neural mechanisms:
    - Tree search with neural evaluation
    - Action sequence generation
    - Forward planning with model-based prediction
    - Value-based action selection
    
    Attributes:
        planning_depth (int): Planning depth
        inhibition_strength (float): Inhibition strength
        excitation_strength (float): Excitation strength
        cognitive_load (float): Current cognitive load
    """
    
    def __init__(self, planning_depth: int = 5, 
                 inhibition_strength: float = 0.5,
                 excitation_strength: float = 1.5):
        """
        Initialize neural planner.
        
        Args:
            planning_depth (int): Planning depth
            inhibition_strength (float): Inhibition strength
            excitation_strength (float): Excitation strength
        """
        self.planning_depth = planning_depth
        self.inhibition_strength = inhibition_strength
        self.excitation_strength = excitation_strength
        
        self.cognitive_load = 0.0
        self.current_plan = []
        
    def initialize(self):
        """Initialize neural planner."""
        self.current_plan = []
        self.cognitive_load = 0.0
        
    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update neural planner.
        
        Args:
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Planning statistics
        """
        # Update cognitive load based on plan complexity
        self.cognitive_load = min(1.0, len(self.current_plan) / self.planning_depth)
        
        return {
            "planning_depth": self.planning_depth,
            "inhibition_strength": self.inhibition_strength,
            "excitation_strength": self.excitation_strength,
            "cognitive_load": self.cognitive_load,
            "plan_length": len(self.current_plan)
        }
        
    def set_planning_depth(self, depth: int):
        """Set planning depth."""
        self.planning_depth = depth
        
    def set_inhibition_strength(self, strength: float):
        """Set inhibition strength."""
        self.inhibition_strength = max(0.0, min(1.0, strength))
        
    def set_excitation_strength(self, strength: float):
        """Set excitation strength."""
        self.excitation_strength = max(0.0, min(2.0, strength))
        
    def get_cognitive_load(self) -> float:
        """Get current cognitive load."""
        return self.cognitive_load
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get neural planner state.
        
        Returns:
            Dict[str, Any]: Neural planner state
        """
        return {
            "planning_depth": self.planning_depth,
            "inhibition_strength": self.inhibition_strength,
            "excitation_strength": self.excitation_strength,
            "cognitive_load": self.cognitive_load,
            "current_plan": self.current_plan
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set neural planner state.
        
        Args:
            state (Dict[str, Any]): Neural planner state
        """
        self.planning_depth = state.get("planning_depth", self.planning_depth)
        self.inhibition_strength = state.get("inhibition_strength", 
                                           self.inhibition_strength)
        self.excitation_strength = state.get("excitation_strength", 
                                           self.excitation_strength)
        self.cognitive_load = state.get("cognitive_load", self.cognitive_load)
        self.current_plan = state.get("current_plan", self.current_plan)
        
    def __repr__(self) -> str:
        return f"<NeuralPlanner depth={self.planning_depth} load={self.cognitive_load:.3f}>"
class Concept:
    """
    Concept representation from experience patterns.
    
    Represents abstract concepts formed from neural patterns:
    - Vector representation in concept space
    - Similarity metrics
    - Activation dynamics
    - Temporal persistence
    
    Attributes:
        concept_id (int): Concept identifier
        prototype (List[float]): Concept prototype
        activation (float): Current activation
        stability (float): Concept stability
        occurrence_count (int): Number of occurrences
    """
    
    def __init__(self, concept_id: int, prototype: List[float] = None):
        """
        Initialize concept.
        
        Args:
            concept_id (int): Concept identifier
            prototype (List[float], optional): Concept prototype
        """
        self.concept_id = concept_id
        self.prototype = prototype or []
        self.activation = 0.0
        self.stability = 1.0
        self.occurrence_count = 0
        self.last_occurrence_time = -1.0
        
    def update(self, input_pattern: List[float], dt: float):
        """
        Update concept with input pattern.
        
        Args:
            input_pattern (List[float]): Input pattern
            dt (float): Time step duration
        """
        if not self.prototype:
            # Initialize prototype with first input
            self.prototype = input_pattern.copy()
            return
            
        # Calculate similarity to prototype
        similarity = self._calculate_similarity(input_pattern, self.prototype)
        
        # Update activation based on similarity
        self.activation = max(0.0, self.activation + 0.1 * (similarity - self.activation))
        self.stability *= 0.99
        
        # Record occurrence
        self.occurrence_count += 1
        self.last_occurrence_time = 0.0  # Would need actual time
        
    def _calculate_similarity(self, pattern1: List[float], pattern2: List[float]) -> float:
        """
        Calculate similarity between two patterns.
        
        Args:
            pattern1 (List[float]): First pattern
            pattern2 (List[float]): Second pattern
            
        Returns:
            float: Similarity (0-1)
        """
        if len(pattern1) != len(pattern2):
            return 0.0
            
        # Cosine similarity
        dot_product = sum(a * b for a, b in zip(pattern1, pattern2))
        norm1 = math.sqrt(sum(a * a for a in pattern1))
        norm2 = math.sqrt(sum(a * a for a in pattern2))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
            
        return dot_product / (norm1 * norm2)
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get concept state.
        
        Returns:
            Dict[str, Any]: Concept state
        """
        return {
            "concept_id": self.concept_id,
            "prototype": self.prototype,
            "activation": self.activation,
            "stability": self.stability,
            "occurrence_count": self.occurrence_count,
            "last_occurrence_time": self.last_occurrence_time
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set concept state.
        
        Args:
            state (Dict[str, Any]): Concept state
        """
        self.concept_id = state.get("concept_id", self.concept_id)
        self.prototype = state.get("prototype", self.prototype)
        self.activation = state.get("activation", self.activation)
        self.stability = state.get("stability", self.stability)
        self.occurrence_count = state.get("occurrence_count", self.occurrence_count)
        self.last_occurrence_time = state.get("last_occurrence_time", 
                                            self.last_occurrence_time)
class ConceptFormation:
    """
    Concept formation system for pattern discovery.
    
    Discovers and forms abstract concepts from neural patterns:
    - Pattern clustering
    - Concept instantiation
    - Concept generalization
    - Concept maintenance and updating
    
    Attributes:
        concept_space_dim (int): Concept space dimensionality
        similarity_threshold (float): Similarity threshold
        concept_decay_rate (float): Concept decay rate
    """
    
    def __init__(self, concept_space_dim: int = 50,
                 similarity_threshold: float = 0.7,
                 concept_decay_rate: float = 0.01):
        """
        Initialize concept formation system.
        
        Args:
            concept_space_dim (int): Concept space dimensionality
            similarity_threshold (float): Similarity threshold
            concept_decay_rate (float): Concept decay rate
        """
        self.concept_space_dim = concept_space_dim
        self.similarity_threshold = similarity_threshold
        self.concept_decay_rate = concept_decay_rate
        
        # Concepts
        self.concepts: Dict[int, Concept] = {}
        self.active_concepts: Dict[int, float] = {}
        
    def initialize(self):
        """Initialize concept formation system."""
        self.concepts.clear()
        self.active_concepts.clear()
        
    def update(self, input_pattern: List[float], dt: float) -> Dict[str, Any]:
        """
        Update concept formation with input pattern.
        
        Args:
            input_pattern (List[float]): Input pattern
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Concept formation statistics
        """
        # Find existing concepts similar to input
        similar_concepts = []
        for concept_id, concept in self.concepts.items():
            similarity = concept._calculate_similarity(input_pattern, concept.prototype)
            if similarity >= self.similarity_threshold:
                similar_concepts.append((concept_id, similarity))
                
        # Update similar concepts
        for concept_id, similarity in similar_concepts:
            concept = self.concepts[concept_id]
            concept.update(input_pattern, dt)
            self.active_concepts[concept_id] = similarity
            
        # Decay inactive concepts
        for concept_id in list(self.concepts.keys()):
            if concept_id not in self.active_concepts:
                concept = self.concepts[concept_id]
                concept.stability *= (1.0 - self.concept_decay_rate)
                if concept.stability < 0.1:
                    del self.concepts[concept_id]
                    
        return {
            "concept_count": len(self.concepts),
            "active_concept_count": len(self.active_concepts),
            "concept_space_dim": self.concept_space_dim,
            "similarity_threshold": self.similarity_threshold,
            "concept_decay_rate": self.concept_decay_rate
        }
        
    def process(self, activity_pattern: List[float]):
        """
        Process neural activity for concept formation.
        
        Args:
            activity_pattern (List[float]): Neural activity pattern
        """
        # Find or create concept for pattern
        new_concept_id = len(self.concepts)
        new_concept = Concept(new_concept_id, activity_pattern)
        self.concepts[new_concept_id] = new_concept
        
    def get_active_concept_count(self) -> int:
        """Get number of active concepts."""
        return len(self.active_concepts)
        
    def get_concept_activations(self) -> Dict[int, float]:
        """Get concept activations."""
        return self.active_concepts.copy()
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get concept formation state.
        
        Returns:
            Dict[str, Any]: Concept formation state
        """
        return {
            "concepts": {
                cid: concept.get_state()
                for cid, concept in self.concepts.items()
            },
            "active_concepts": self.active_concepts,
            "concept_space_dim": self.concept_space_dim,
            "similarity_threshold": self.similarity_threshold,
            "concept_decay_rate": self.concept_decay_rate,
            "concept_count": len(self.concepts),
            "active_concept_count": len(self.active_concepts)
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set concept formation state.
        
        Args:
            state (Dict[str, Any]): Concept formation state
        """
        self.concepts.clear()
        for concept_data in state["concepts"].values():
            concept = Concept(concept_data["concept_id"])
            concept.set_state(concept_data)
            self.concepts[concept.concept_id] = concept
            
        self.active_concepts = state.get("active_concepts", {})
        self.concept_space_dim = state.get("concept_space_dim", 
                                         self.concept_space_dim)
        self.similarity_threshold = state.get("similarity_threshold", 
                                            self.similarity_threshold)
        self.concept_decay_rate = state.get("concept_decay_rate", 
                                          self.concept_decay_rate)
        
    def __repr__(self) -> str:
        return f"<ConceptFormation concepts={len(self.concepts)} active={len(self.active_concepts)}>"
class AttentionalSelection:
    """
    Attentional selection system for competitive dynamics.
    
    Implements attentional selection with winner-take-all dynamics:
    - Competitive inhibition between neurons
    - Priority-based attentional focus
    - Gating of neural activity
    - Attentional resource allocation
    
    Attributes:
        inhibition_strength (float): Inhibition strength
        excitation_strength (float): Excitation strength
        attentional_focus (float): Current attentional focus
    """
    
    def __init__(self, inhibition_strength: float = 0.5,
                 excitation_strength: float = 1.5):
        """
        Initialize attentional selection system.
        
        Args:
            inhibition_strength (float): Inhibition strength
            excitation_strength (float): Excitation strength
        """
        self.inhibition_strength = inhibition_strength
        self.excitation_strength = excitation_strength
        
        self.attentional_focus = 0.0
        self.competition_matrix: Dict[int, Dict[int, float]] = {}
        
    def initialize(self):
        """Initialize attentional selection system."""
        self.attentional_focus = 0.0
        self.competition_matrix.clear()
        
    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update attentional selection.
        
        Args:
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Attentional selection statistics
        """
        # Update attentional focus
        self.attentional_focus = min(1.0, self.attentional_focus + 0.01)
        
        return {
            "inhibition_strength": self.inhibition_strength,
            "excitation_strength": self.excitation_strength,
            "attentional_focus": self.attentional_focus,
            "competition_matrix_size": len(self.competition_matrix)
        }
        
    def set_inhibition_strength(self, strength: float):
        """Set inhibition strength."""
        self.inhibition_strength = max(0.0, min(1.0, strength))
        
    def set_excitation_strength(self, strength: float):
        """Set excitation strength."""
        self.excitation_strength = max(0.0, min(2.0, strength))
        
    def process_competition(self, competitors: List[int]):
        """
        Process competition among neurons.
        
        Args:
            competitors (List[int]): List of competing neuron IDs
        """
        if len(competitors) < 2:
            return
            
        # Find strongest competitor
        max_strength = -1.0
        strongest_id = None
        
        for neuron_id in competitors:
            # This would get actual neural strength
            strength = 1.0  # Placeholder
            if strength > max_strength:
                max_strength = strength
                strongest_id = neuron_id
                
        # Apply inhibition to all but strongest
        for neuron_id in competitors:
            if neuron_id != strongest_id:
                # This would apply inhibition
                pass
                
    def get_cognitive_load(self) -> float:
        """Get current cognitive load."""
        return self.attentional_focus
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get attentional selection state.
        
        Returns:
            Dict[str, Any]: Attentional selection state
        """
        return {
            "inhibition_strength": self.inhibition_strength,
            "excitation_strength": self.excitation_strength,
            "attentional_focus": self.attentional_focus,
            "competition_matrix": self.competition_matrix
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set attentional selection state.
        
        Args:
            state (Dict[str, Any]): Attentional selection state
        """
        self.inhibition_strength = state.get("inhibition_strength", 
                                           self.inhibition_strength)
        self.excitation_strength = state.get("excitation_strength", 
                                           self.excitation_strength)
        self.attentional_focus = state.get("attentional_focus", 
                                          self.attentional_focus)
        self.competition_matrix = state.get("competition_matrix", 
                                          self.competition_matrix)
        
    def __repr__(self) -> str:
        return f"<AttentionalSelection inhibition={self.inhibition_strength:.3f} excitation={self.excitation_strength:.3f}>"
class CognitiveSystem:
    """
    Cognitive system integrating planning, concept formation, and attention.
    
    Coordinates cognitive processes for intelligent behavior:
    - Neural planning and action selection
    - Concept formation from experience
    - Attentional selection with competitive dynamics
    - Cognitive load management
    - Executive control
    
    Attributes:
        planner (NeuralPlanner): Neural planner
        concept_formation (ConceptFormation): Concept formation system
        attention (AttentionalSelection): Attentional selection system
    """
    
    def __init__(self, planning_depth: int = 5):
        """
        Initialize cognitive system.
        
        Args:
            planning_depth (int): Planning depth
        """
        self.planner = NeuralPlanner(planning_depth=planning_depth)
        self.concept_formation = ConceptFormation()
        self.attention = AttentionalSelection()
        
    def initialize(self):
        """Initialize cognitive system."""
        self.planner.initialize()
        self.concept_formation.initialize()
        self.attention.initialize()
        
    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update cognitive system.
        
        Args:
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Cognitive system statistics
        """
        # Update subsystems
        planner_stats = self.planner.update(dt)
        concept_stats = self.concept_formation.update([], dt)  # No input pattern for now
        attention_stats = self.attention.update(dt)
        
        # Calculate overall cognitive load
        cognitive_load = (
            planner_stats["cognitive_load"] * 0.4 +
            concept_stats["active_concept_count"] / 50 * 0.3 +
            attention_stats["attentional_focus"] * 0.3
        )
        
        return {
            "cognitive_load": cognitive_load,
            "planning_stats": planner_stats,
            "concept_stats": concept_stats,
            "attention_stats": attention_stats
        }
        
    def set_planning_depth(self, depth: int):
        """Set planning depth."""
        self.planner.set_planning_depth(depth)
        
    def set_attention_inhibition(self, strength: float):
        """Set attention inhibition strength."""
        self.attention.set_inhibition_strength(strength)
        
    def set_attention_excitation(self, strength: float):
        """Set attention excitation strength."""
        self.attention.set_excitation_strength(strength)
        
    def process_competition(self, competitors: List[int]):
        """
        Process competition among neural populations.
        
        Args:
            competitors (List[int]): List of competing neuron IDs
        """
        self.attention.process_competition(competitors)
        
    def get_planner(self) -> NeuralPlanner:
        """Get neural planner."""
        return self.planner
        
    def get_concept_formation(self) -> ConceptFormation:
        """Get concept formation system."""
        return self.concept_formation
        
    def get_attention(self) -> AttentionalSelection:
        """Get attentional selection system."""
        return self.attention
        
    def get_cognitive_load(self) -> float:
        """Get current cognitive load."""
        return self.planner.get_cognitive_load()
        
    def get_active_concept_count(self) -> int:
        """Get number of active concepts."""
        return self.concept_formation.get_active_concept_count()
        
    def reset(self):
        """Reset cognitive system."""
        self.planner.initialize()
        self.concept_formation.initialize()
        self.attention.initialize()
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get cognitive system state.
        
        Returns:
            Dict[str, Any]: Cognitive system state
        """
        return {
            "planner": self.planner.get_state(),
            "concept_formation": self.concept_formation.get_state(),
            "attention": self.attention.get_state()
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set cognitive system state.
        
        Args:
            state (Dict[str, Any]): Cognitive system state
        """
        self.planner.set_state(state["planner"])
        self.concept_formation.set_state(state["concept_formation"])
        self.attention.set_state(state["attention"])
        
    def __repr__(self) -> str:
        return f"<CognitiveSystem concepts={len(self.concept_formation.concepts)} planner_depth={self.planner.planning_depth}>"
