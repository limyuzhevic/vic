"""
Enhanced Python interface for NLM cognition systems
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError

class NeuralPlanner:
    """Neural planner for action planning"""
    
    def __init__(self, planning_depth: int = 5):
        self.planning_depth = planning_depth
        self.plans = []
        self.current_plan = None
        self.evaluation_function = None
        
    def set_evaluation_function(self, func) -> None:
        """Set evaluation function for plan selection"""
        self.evaluation_function = func
        
    def plan_action(self, current_state: Dict[str, Any], 
                    available_actions: List[str]) -> Dict[str, Any]:
        """Plan optimal action sequence"""
        # Simple planning: select action with highest potential value
        if not available_actions:
            return {'action': 'wait', 'sequence': [], 'score': 0.0}
            
        best_action = available_actions[0]
        best_score = 0.0
        
        for action in available_actions:
            # Simple heuristic evaluation
            action_score = len(action) * 0.1  # Dummy scoring
            if self.evaluation_function:
                action_score = self.evaluation_function(action, current_state)
                
            if action_score > best_score:
                best_score = action_score
                best_action = action
                
        plan = {
            'action': best_action,
            'sequence': [best_action],
            'depth': 1,
            'score': best_score,
            'alternatives': []
        }
        
        self.plans.append(plan)
        return plan
        
    def plan_sequence(self, current_state: Dict[str, Any], 
                      action_sequences: List[List[str]]) -> Dict[str, Any]:
        """Plan optimal action sequence from multiple options"""
        best_sequence = []
        best_score = 0.0
        
        for sequence in action_sequences:
            sequence_score = sum(len(action) * 0.1 for action in sequence)
            if self.evaluation_function:
                sequence_score = self.evaluation_function(sequence, current_state)
                
            if sequence_score > best_score:
                best_score = sequence_score
                best_sequence = sequence
                
        plan = {
            'action': best_sequence[0] if best_sequence else 'wait',
            'sequence': best_sequence,
            'depth': len(best_sequence),
            'score': best_score,
            'alternatives': [s for s in action_sequences if s != best_sequence]
        }
        
        self.current_plan = plan
        self.plans.append(plan)
        return plan
        
    def get_current_plan(self) -> Optional[Dict[str, Any]]:
        """Get current action plan"""
        return self.current_plan
        
    def get_planning_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get planning history"""
        if limit is None:
            return self.plans.copy()
        return self.plans[-limit:]
        
    def set_planning_depth(self, depth: int) -> None:
        """Set planning depth"""
        self.planning_depth = depth
        
    def get_planning_depth(self) -> int:
        """Get planning depth"""
        return self.planning_depth
        
    def __repr__(self):
        return f"<NeuralPlanner: depth={self.planning_depth}, plans={len(self.plans)}>"

class ConceptFormation:
    """Concept formation system for pattern discovery"""
    
    def __init__(self, concept_threshold: float = 0.5):
        self.concepts = {}
        self.concept_threshold = concept_threshold
        self.pattern_history = []
        self.concept_clusters = []
        
    def learn_pattern(self, pattern: List[float], strength: float = 1.0) -> None:
        """Learn new pattern and discover concepts"""
        # Store pattern
        self.pattern_history.append(pattern.copy())
        
        # Simple concept formation: cluster similar patterns
        concept_id = self._find_or_create_concept(pattern)
        if concept_id:
            if concept_id not in self.concepts:
                self.concepts[concept_id] = {'patterns': [], 'strength': 0.0}
            self.concepts[concept_id]['patterns'].append(pattern.copy())
            self.concepts[concept_id]['strength'] += strength
            
    def _find_or_create_concept(self, pattern: List[float]) -> Optional[str]:
        """Find existing concept or create new one"""
        if not self.concepts:
            return f"concept_{len(self.concepts)}"
            
        # Simple similarity: find closest concept
        min_distance = float('inf')
        best_concept = None
        
        for concept_id, concept_data in self.concepts.items():
            # Simple Euclidean distance to concept centroid
            centroid = self._get_concept_centroid(concept_id)
            distance = self._pattern_distance(pattern, centroid)
            if distance < min_distance:
                min_distance = distance
                best_concept = concept_id
                
        # If pattern is similar enough to existing concept, use it
        if min_distance < self.concept_threshold and best_concept:
            return best_concept
        
        # Create new concept
        new_concept = f"concept_{len(self.concepts)}"
        return new_concept
        
    def _get_concept_centroid(self, concept_id: str) -> List[float]:
        """Get centroid of concept cluster"""
        if concept_id not in self.concepts:
            return []
            
        patterns = self.concepts[concept_id]['patterns']
        if not patterns:
            return []
            
        # Calculate mean vector
        dim = len(patterns[0])
        centroid = [0.0] * dim
        for pattern in patterns:
            for i, value in enumerate(pattern):
                centroid[i] += value
        for i in range(dim):
            centroid[i] /= len(patterns)
        return centroid
        
    def _pattern_distance(self, pattern1: List[float], pattern2: List[float]) -> float:
        """Calculate distance between two patterns"""
        if len(pattern1) != len(pattern2):
            return float('inf')
            
        distance = 0.0
        for a, b in zip(pattern1, pattern2):
            distance += (a - b) ** 2
        return distance ** 0.5
        
    def get_concepts(self) -> Dict[str, Dict[str, Any]]:
        """Get all concepts"""
        return self.concepts.copy()
        
    def get_concept_strength(self, concept_id: str) -> float:
        """Get strength of specific concept"""
        return self.concepts.get(concept_id, {}).get('strength', 0.0)
        
    def get_concept_patterns(self, concept_id: str) -> List[List[float]]:
        """Get patterns for specific concept"""
        return self.concepts.get(concept_id, {}).get('patterns', []).copy()
        
    def get_concept_count(self) -> int:
        """Get number of concepts"""
        return len(self.concepts)
        
    def get_pattern_count(self) -> int:
        """Get total number of patterns"""
        return len(self.pattern_history)
        
    def clear(self) -> None:
        """Clear all concepts and patterns"""
        self.concepts.clear()
        self.pattern_history.clear()
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get concept formation statistics"""
        if not self.concepts:
            return {
                'concept_count': 0,
                'pattern_count': 0,
                'avg_concept_strength': 0.0,
                'avg_patterns_per_concept': 0.0
            }
            
        total_strength = sum(c['strength'] for c in self.concepts.values())
        total_patterns = sum(len(c['patterns']) for c in self.concepts.values())
        
        return {
            'concept_count': len(self.concepts),
            'pattern_count': len(self.pattern_history),
            'avg_concept_strength': total_strength / len(self.concepts),
            'avg_patterns_per_concept': total_patterns / len(self.concepts)
        }
        
    def __repr__(self):
        stats = self.get_statistics()
        return f"<ConceptFormation: {stats['concept_count']} concepts, {stats['pattern_count']} patterns, avg_strength={stats['avg_concept_strength']:.3f}>"

class AttentionalSelection:
    """Attentional selection system for focus"""
    
    def __init__(self, inhibition_strength: float = 0.5, excitation_strength: float = 1.5):
        self.inhibition_strength = inhibition_strength
        self.excitation_strength = excitation_strength
        self.attention_weights = []
        self.attention_history = []
        self.current_focus = []
        
    def process_competition(self, competitors: List[int]) -> List[float]:
        """Process competition among neural units"""
        if not competitors:
            return []
            
        # Simple competition model: winner-take-all with inhibition
        weights = [1.0] * len(competitors)
        
        # Normalize weights
        max_weight = max(weights)
        if max_weight > 0:
            weights = [w / max_weight for w in weights]
            
        # Apply attention mechanism
        self.attention_weights = weights.copy()
        self.attention_history.append(weights.copy())
        
        # Select focus based on weights
        max_idx = max(range(len(weights)), key=lambda i: weights[i])
        self.current_focus = [competitors[max_idx]]
        
        # Apply inhibition to non-winners
        for i, weight in enumerate(weights):
            if i != max_idx:
                weights[i] *= self.inhibition_strength
                
        return weights
        
    def get_current_focus(self) -> List[int]:
        """Get current attentional focus"""
        return self.current_focus.copy()
        
    def set_inhibition_strength(self, strength: float) -> None:
        """Set inhibition strength"""
        self.inhibition_strength = max(0.0, min(1.0, strength))
        
    def set_excitation_strength(self, strength: float) -> None:
        """Set excitation strength"""
        self.excitation_strength = max(0.0, min(2.0, strength))
        
    def get_inhibition_strength(self) -> float:
        """Get inhibition strength"""
        return self.inhibition_strength
        
    def get_excitation_strength(self) -> float:
        """Get excitation strength"""
        return self.excitation_strength
        
    def get_attention_weights(self) -> List[float]:
        """Get current attention weights"""
        return self.attention_weights.copy()
        
    def get_attention_history(self, limit: Optional[int] = None) -> List[List[float]]:
        """Get attention history"""
        if limit is None:
            return self.attention_history.copy()
        return self.attention_history[-limit:]
        
    def clear(self) -> None:
        """Clear attention state"""
        self.attention_weights.clear()
        self.attention_history.clear()
        self.current_focus.clear()
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get attentional selection statistics"""
        if not self.attention_history:
            return {
                'attention_events': 0,
                'avg_focus_size': 0,
                'max_attention_weight': 0.0,
                'focus_change_rate': 0.0
            }
            
        total_focus_size = sum(len(focus) for focus in self.current_focus)
        all_weights = [w for event in self.attention_history for w in event]
        
        return {
            'attention_events': len(self.attention_history),
            'avg_focus_size': total_focus_size / len(self.current_focus) if self.current_focus else 0,
            'max_attention_weight': max(all_weights) if all_weights else 0.0,
            'focus_change_rate': len(set(tuple(focus) for focus in self.current_focus)) / max(1, len(self.current_focus))
        }
        
    def __repr__(self):
        stats = self.get_statistics()
        return f"<AttentionalSelection: events={stats['attention_events']}, focus_size={stats['avg_focus_size']:.1f}, max_weight={stats['max_attention_weight']:.3f}>"
