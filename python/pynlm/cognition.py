"""
Cognition module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Union, Callable
from enum import Enum
import numpy as np

try:
    from pynlm.bindings import createBrain, createDefaultConfig, createSimpleWorld
    from pynlm.bindings import createAgentBrain, ActionType, Action
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Cognition module limited.")


class AttentionState:
    """Represents current attention state."""
    
    def __init__(self, total_focus: float = 1.0):
        """
        Initialize attention state.
        
        Args:
            total_focus: Total attentional resources
        """
        self.total_focus = total_focus
        self.focus_distribution = {}
        self.attention_focus = {}
        self.distractors = []
        self.focus_target = None
        self.focus_strength = 0.0
        self.attention_span = 0.0
        self.selectivity = 0.0
        self.timestamp = time.time()
    
    def allocate_focus(self, target: str, amount: float, selectivity: float = 1.0):
        """
        Allocate attentional resources to target.
        
        Args:
            target: Target to focus on
            amount: Amount of focus to allocate
            selectivity: Focus selectivity (0.0 to 1.0)
        """
        # Ensure we don't exceed total focus
        available = self.total_focus - sum(self.focus_distribution.values())
        if available <= 0:
            # Redistribute focus
            total_current = sum(self.focus_distribution.values())
            if total_current > 0:
                for t in list(self.focus_distribution.keys()):
                    self.focus_distribution[t] *= 0.5
                    self.attention_focus[t] *= 0.5
        
        # Allocate new focus
        alloc_amount = min(amount, available)
        self.focus_distribution[target] = self.focus_distribution.get(target, 0) + alloc_amount
        self.attention_focus[target] = selectivity
        
        # Update overall focus
        self.focus_strength = min(1.0, sum(self.focus_distribution.values()) / self.total_focus)
        self.attention_span = len([f for f in self.focus_distribution.values() if f > 0.1])
        
        self.timestamp = time.time()
    
    def shift_focus(self, new_target: str, transition_speed: float = 0.1):
        """
        Shift focus from current target to new target.
        
        Args:
            new_target: New focus target
            transition_speed: Speed of focus transition
        """
        if self.focus_target:
            # Gradually reduce focus on current target
            current_focus = self.focus_distribution.get(self.focus_target, 0)
            self.focus_distribution[self.focus_target] = max(0, 
                current_focus * (1 - transition_speed))
        
        # Increase focus on new target
        self.focus_target = new_target
        self.allocate_focus(new_target, 0.5 + self.selectivity * 0.5)
        
        self.timestamp = time.time()
    
    def add_distractor(self, distractor: str, salience: float = 1.0):
        """
        Add distractor that competes for attention.
        
        Args:
            distractor: Distractor identifier
            salience: Distractor salience (0.0 to 1.0)
        """
        self.distractors.append({
            'name': distractor,
            'salience': salience,
            'timestamp': time.time(),
        })
        
        # Decay distractor salience over time
        for d in self.distractors:
            d['salience'] *= 0.95
        
        # Remove very weak distractors
        self.distractors = [d for d in self.distractors if d['salience'] > 0.01]
        
        self.timestamp = time.time()
    
    def get_focused_target(self) -> str:
        """Get currently focused target."""
        return self.focus_target
    
    def get_focus_strength(self) -> float:
        """Get current focus strength."""
        return self.focus_strength
    
    def get_attention_span(self) -> float:
        """Get current attention span."""
        return self.attention_span
    
    def get_focus_distribution(self) -> Dict[str, float]:
        """Get focus distribution."""
        return self.focus_distribution.copy()
    
    def clear_distractors(self):
        """Clear all distractors."""
        self.distractors.clear()
    
    def reset(self):
        """Reset attention state."""
        self.focus_distribution.clear()
        self.attention_focus.clear()
        self.distractors.clear()
        self.focus_target = None
        self.focus_strength = 0.0
        self.attention_span = 0.0
        self.timestamp = time.time()
    
    def __repr__(self) -> str:
        return f"AttentionState(focus={self.focus_target}, strength={self.focus_strength:.3f}, span={self.attention_span:.1f})"


class Concept:
    """Cognitive concept."""
    
    def __init__(self, name: str, definition: str = "", embeddings: List[float] = None):
        """
        Initialize concept.
        
        Args:
            name: Concept name
            definition: Concept definition
            embeddings: Feature embeddings
        """
        self.name = name
        self.definition = definition
        self.embeddings = embeddings or []
        self.associations = {}
        self.activation_level = 0.0
        self.confidence = 0.0
        self.frequency = 0
        self.last_accessed = time.time()
        self.strength = 0.0
    
    def add_association(self, related_concept: str, strength: float = 0.5):
        """
        Add associated concept.
        
        Args:
            related_concept: Related concept name
            strength: Association strength (0.0 to 1.0)
        """
        self.associations[related_concept] = max(
            self.associations.get(related_concept, 0),
            strength
        )
    
    def activate(self, activation_strength: float = 0.1):
        """
        Activate concept.
        
        Args:
            activation_strength: Activation strength
        """
        self.activation_level = min(1.0, self.activation_level + activation_strength)
        self.confidence = min(1.0, self.confidence + activation_strength * 0.5)
        self.last_accessed = time.time()
        self.frequency += 1
        self.strength = min(1.0, self.strength + activation_strength)
    
    def decay(self, decay_rate: float = 0.05):
        """
        Decay concept activation.
        
        Args:
            decay_rate: Decay rate
        """
        self.activation_level *= (1 - decay_rate)
        if self.activation_level < 0.01:
            self.activation_level = 0.0
    
    def get_related_concepts(self, max_results: int = 5) -> List[str]:
        """
        Get related concepts sorted by association strength.
        
        Args:
            max_results: Maximum results
        
        Returns:
            List of related concept names
        """
        sorted_associations = sorted(
            self.associations.items(),
            key=lambda x: x[1],
            reverse=True
        )
        return [concept for concept, _ in sorted_associations[:max_results]]
    
    def similarity_to(self, other_concept: 'Concept') -> float:
        """
        Calculate similarity to another concept.
        
        Args:
            other_concept: Other concept
        
        Returns:
            Similarity score (0.0 to 1.0)
        """
        if not self.embeddings or not other_concept.embeddings:
            return 0.0
        
        # Cosine similarity
        dot_product = sum(f1 * f2 for f1, f2 in zip(self.embeddings, other_concept.embeddings))
        norm1 = math.sqrt(sum(f ** 2 for f in self.embeddings))
        norm2 = math.sqrt(sum(f ** 2 for f in other_concept.embeddings))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
        
        return dot_product / (norm1 * norm2)
    
    def __repr__(self) -> str:
        return f"Concept({self.name}, activation={self.activation_level:.3f}, strength={self.strength:.3f})"


class Conceptualizer:
    """Concept formation system."""
    
    def __init__(self):
        """Initialize conceptualizer."""
        self.concepts = {}
        self.concept_patterns = {}
        self.association_strength = 0.5
        self.separation_threshold = 0.3
        self.integration_threshold = 0.7
        self.concept_detector = None
    
    def add_pattern(self, pattern_id: str, pattern_features: List[float], category: str = ""):
        """
        Add pattern for concept formation.
        
        Args:
            pattern_id: Pattern identifier
            pattern_features: Pattern features
            category: Pattern category
        """
        self.concept_patterns[pattern_id] = {
            'features': pattern_features,
            'category': category,
            'timestamp': time.time(),
            'activation_count': 0,
        }
    
    def extract_features(self, data: Dict[str, Any]) -> List[float]:
        """
        Extract features from data for concept formation.
        
        Args:
            data: Data to extract features from
        
        Returns:
            Feature vector
        """
        features = []
        
        # Frequency-like feature
        if 'events' in data:
            event_types = [e.get('type', '') for e in data['events']]
            unique_types = len(set(event_types))
            features.append(unique_types / 10.0)  # Normalized
        
        # Intensity feature
        if 'rewards' in data:
            rewards = [r.get('value', 0) for r in data['rewards']]
            features.append(sum(rewards) / max(len(rewards), 1))
        
        # Temporal pattern feature
        if 'timestamps' in data:
            timestamps = data['timestamps']
            if len(timestamps) > 1:
                intervals = [timestamps[i] - timestamps[i-1] for i in range(1, len(timestamps))]
                features.append(sum(intervals) / len(intervals) if intervals else 0)
        
        # Action diversity feature
        if 'actions' in data:
            actions = [a.get('type', '') for a in data['actions']]
            features.append(len(set(actions)) / 10.0)  # Normalized
        
        # Sensory input feature
        if 'sensory' in data:
            sensory_data = data['sensory']
            if sensory_data:
                # Average magnitude of sensory inputs
                magnitudes = [math.sqrt(sum(v**2 for v in s.values())) for s in sensory_data]
                features.append(sum(magnitudes) / len(magnitudes))
        
        return features
    
    def form_concept(self, data: Dict[str, Any], concept_name: str = None) -> Concept:
        """
        Form concept from data.
        
        Args:
            data: Data to form concept from
            concept_name: Name for concept (generated if None)
        
        Returns:
            Formed concept
        """
        # Extract features
        features = self.extract_features(data)
        
        # Check for existing similar concepts
        best_match = None
        best_similarity = 0.0
        
        for existing_concept in self.concepts.values():
            similarity = existing_concept.similarity_to(
                Concept("temp", "", features)
            )
            if similarity > best_similarity:
                best_similarity = similarity
                best_match = existing_concept
        
        if best_match and best_similarity > self.integration_threshold:
            # Integrate with existing concept
            concept = best_match
            concept.activate(0.2)
            # Update features
            for i, feature in enumerate(features):
                if i < len(concept.embeddings):
                    concept.embeddings[i] = (concept.embeddings[i] + feature) / 2
        else:
            # Create new concept
            if concept_name is None:
                concept_name = f"concept_{len(self.concepts)}"
            
            definition = self._generate_definition(data)
            concept = Concept(concept_name, definition, features)
            concept.activate(0.5)
            self.concepts[concept_name] = concept
            
            # Add associations to related concepts
            self._establish_associations(concept, data)
        
        return concept
    
    def _generate_definition(self, data: Dict[str, Any]) -> str:
        """Generate concept definition from data."""
        parts = []
        
        if 'events' in data:
            event_types = [e.get('type', '') for e in data['events'] if e.get('type')]
            if event_types:
                parts.append(f"characterized by {', '.join(set(event_types))}")
        
        if 'rewards' in data:
            rewards = [r.get('value', 0) for r in data['rewards']]
            avg_reward = sum(rewards) / len(rewards) if rewards else 0
            parts.append(f"with average reward {avg_reward:.2f}")
        
        if 'actions' in data:
            actions = [a.get('type', '') for a in data['actions'] if a.get('type')]
            if actions:
                parts.append(f"involving {', '.join(set(actions))}")
        
        if not parts:
            return "Abstract concept"
        
        return "Concept " + " and ".join(parts)
    
    def _establish_associations(self, concept: Concept, data: Dict[str, Any]):
        """Establish associations for new concept."""
        # Find related concepts based on data
        related_concepts = []
        
        if 'events' in data:
            event_types = set(e.get('type', '') for e in data['events'] if e.get('type'))
            for event_type in event_types:
                # Find concept matching event type
                for existing_name, existing_concept in self.concepts.items():
                    if event_type in existing_concept.definition:
                        related_concepts.append(existing_name)
        
        # Add associations
        for related_name in related_concepts:
            if related_name != concept.name:
                concept.add_association(related_name, 0.3)
    
    def activate_concept(self, concept_name: str) -> Concept:
        """
        Activate existing concept.
        
        Args:
            concept_name: Concept name
        
        Returns:
            Activated concept or None
        """
        if concept_name in self.concepts:
            concept = self.concepts[concept_name]
            concept.activate(0.1)
            return concept
        return None
    
    def get_concept(self, concept_name: str) -> Concept:
        """Get concept by name."""
        return self.concepts.get(concept_name)
    
    def get_all_concepts(self) -> List[Concept]:
        """Get all concepts."""
        return list(self.concepts.values())
    
    def get_concept_network(self) -> Dict[str, Any]:
        """Get concept association network."""
        network = {}
        
        for name, concept in self.concepts.items():
            network[name] = {
                'associations': concept.associations,
                'activation': concept.activation_level,
                'strength': concept.strength,
                'definition': concept.definition,
                'frequency': concept.frequency,
            }
        
        return network
    
    def get_concept_statistics(self) -> Dict[str, Any]:
        """Get conceptualizer statistics."""
        if not self.concepts:
            return {'concepts': 0, 'avg_activation': 0.0, 'avg_strength': 0.0}
        
        total_activation = sum(c.activation_level for c in self.concepts.values())
        total_strength = sum(c.strength for c in self.concepts.values())
        
        return {
            'concepts': len(self.concepts),
            'avg_activation': total_activation / len(self.concepts),
            'avg_strength': total_strength / len(self.concepts),
            'most_active': max(
                self.concepts.items(),
                key=lambda x: x[1].activation_level
            )[0] if self.concepts else "",
            'most_frequent': max(
                self.concepts.items(),
                key=lambda x: x[1].frequency
            )[0] if self.concepts else "",
        }
    
    def decay_all_concepts(self, decay_rate: float = 0.05):
        """Decay all concept activations."""
        for concept in self.concepts.values():
            concept.decay(decay_rate)
    
    def consolidate_concepts(self, threshold: float = 0.3):
        """
        Consolidate similar concepts.
        
        Args:
            threshold: Similarity threshold for consolidation
        """
        concept_names = list(self.concepts.keys())
        
        for i, name1 in enumerate(concept_names):
            for name2 in concept_names[i+1:]:
                concept1 = self.concepts[name1]
                concept2 = self.concepts[name2]
                
                similarity = concept1.similarity_to(concept2)
                
                if similarity > threshold:
                    # Merge concepts
                    merged_name = f"merged_{name1}_{name2}"
                    
                    # Average features
                    combined_features = []
                    if concept1.embeddings and concept2.embeddings:
                        min_len = min(len(concept1.embeddings), len(concept2.embeddings))
                        combined_features = [
                            (concept1.embeddings[i] + concept2.embeddings[i]) / 2
                            for i in range(min_len)
                        ]
                    
                    # Create merged concept
                    merged_concept = Concept(
                        merged_name,
                        f"Merged concept of {concept1.name} and {concept2.name}",
                        combined_features
                    )
                    
                    # Combine associations
                    for assoc_name, strength in concept2.associations.items():
                        if assoc_name != name1:
                            merged_concept.add_association(assoc_name, strength * 0.5)
                    
                    # Replace old concepts
                    self.concepts[merged_name] = merged_concept
                    del self.concepts[name1]
                    del self.concepts[name2]
    
    def __repr__(self) -> str:
        return f"Conceptualizer(concepts={len(self.concepts)})"


class NeuralPlanner:
    """Neural planning and action selection system."""
    
    def __init__(self, action_space_size: int = 20):
        """
        Initialize neural planner.
        
        Args:
            action_space_size: Size of action space
        """
        self.action_space_size = action_space_size
        self.action_preferences = np.zeros(action_space_size)
        self.action_values = np.zeros(action_space_size)
        self.planning_horizon = 10
        self.exploration_bonus = 0.1
        self.exploitation_strength = 0.9
        self.action_history = []
        self.value_history = []
    
    def add_action_preference(self, action_index: int, preference: float):
        """
        Add preference for action.
        
        Args:
            action_index: Action index
            preference: Preference strength
        """
        if 0 <= action_index < self.action_space_size:
            self.action_preferences[action_index] += preference
    
    def update_action_value(self, action_index: int, value: float):
        """
        Update action value using temporal difference learning.
        
        Args:
            action_index: Action index
            value: Expected value
        """
        if 0 <= action_index < self.action_space_size:
            old_value = self.action_values[action_index]
            td_error = value - old_value
            self.action_values[action_index] += td_error * self.exploitation_strength
    
    def select_action(self, method: str = "softmax", temperature: float = 1.0) -> int:
        """
        Select action based on preferences and values.
        
        Args:
            method: Selection method ('softmax', 'epsilon_greedy', 'argmax')
            temperature: Temperature for softmax selection
        
        Returns:
            Selected action index
        """
        if method == "softmax":
            # Softmax selection
            combined_scores = self.action_preferences * self.exploitation_strength + \
                            self.action_values * (1 - self.exploitation_strength)
            
            # Apply temperature
            scaled_scores = combined_scores / temperature
            
            # Convert to probabilities
            exp_scores = np.exp(scaled_scores - np.max(scaled_scores))  # For numerical stability
            probabilities = exp_scores / np.sum(exp_scores)
            
            return np.random.choice(self.action_space_size, p=probabilities)
        
        elif method == "epsilon_greedy":
            # Epsilon-greedy selection
            if np.random.random() < self.exploration_bonus:
                return np.random.randint(self.action_space_size)
            else:
                return np.argmax(self.action_values)
        
        else:  # argmax
            return np.argmax(self.action_values)
    
    def evaluate_action(self, action_index: int, world_state: Dict[str, Any] = None) -> float:
        """
        Evaluate action value.
        
        Args:
            action_index: Action index
            world_state: Current world state
        
        Returns:
            Action value
        """
        if not (0 <= action_index < self.action_space_size):
            return 0.0
        
        base_value = self.action_values[action_index]
        preference_bonus = self.action_preferences[action_index] * 0.1
        
        # Add world state evaluation if provided
        if world_state:
            state_value = self._evaluate_world_state(world_state, action_index)
            return base_value + preference_bonus + state_value
        
        return base_value + preference_bonus
    
    def _evaluate_world_state(self, world_state: Dict[str, Any], action_index: int) -> float:
        """Evaluate world state for given action."""
        # Simple heuristic evaluation
        value = 0.0
        
        # Check if action is valid
        if world_state.get('action_valid', {}).get(action_index, False):
            value += 1.0
        
        # Reward prediction
        if 'expected_reward' in world_state:
            value += world_state['expected_reward'] * 0.5
        
        # Novelty bonus
        if 'novelty_level' in world_state:
            value += world_state['novelty_level'] * 0.3
        
        return value
    
    def plan_action_sequence(self, current_state: Dict[str, Any], horizon: int = None) -> List[int]:
        """
        Plan action sequence for given horizon.
        
        Args:
            current_state: Current state
            horizon: Planning horizon (uses default if None)
        
        Returns:
            List of action indices
        """
        if horizon is None:
            horizon = self.planning_horizon
        
        sequence = []
        
        for step in range(horizon):
            # Evaluate all actions
            action_values = []
            for action_idx in range(self.action_space_size):
                value = self.evaluate_action(action_idx, current_state)
                action_values.append(value)
            
            # Select best action
            action_idx = np.argmax(action_values)
            sequence.append(action_idx)
            
            # Update current state (simplified)
            current_state = self._simulate_action(current_state, action_idx)
        
        return sequence
    
    def _simulate_action(self, state: Dict[str, Any], action_idx: int) -> Dict[str, Any]:
        """Simulate effect of action on world state."""
        new_state = state.copy()
        
        # Simple state transition
        if 'action_history' in new_state:
            new_state['action_history'].append(action_idx)
        else:
            new_state['action_history'] = [action_idx]
        
        # Update position based on action
        if 'position' in new_state:
            x, y = new_state['position']
            if action_idx == 0:  # Move forward
                new_state['position'] = (x + 1, y)
            elif action_idx == 1:  # Move backward
                new_state['position'] = (x - 1, y)
            elif action_idx == 2:  # Turn left
                new_state['orientation'] = (new_state.get('orientation', 0) + math.pi/4) % (2 * math.pi)
            elif action_idx == 3:  # Turn right
                new_state['orientation'] = (new_state.get('orientation', 0) - math.pi/4) % (2 * math.pi)
        
        return new_state
    
    def get_planning_summary(self) -> Dict[str, Any]:
        """Get planning system summary."""
        return {
            'action_space_size': self.action_space_size,
            'planning_horizon': self.planning_horizon,
            'exploration_bonus': self.exploration_bonus,
            'current_preferences': self.action_preferences.copy(),
            'current_values': self.action_values.copy(),
            'action_history_length': len(self.action_history),
            'value_history_length': len(self.value_history),
            'recommended_action': np.argmax(self.action_values) if self.action_space_size > 0 else -1,
        }
    
    def reset(self):
        """Reset planner."""
        self.action_preferences.fill(0)
        self.action_values.fill(0)
        self.action_history.clear()
        self.value_history.clear()
    
    def __repr__(self) -> str:
        return f"NeuralPlanner(actions={self.action_space_size}, horizon={self.planning_horizon})"


class CognitiveSystem:
    """Integrated cognitive processing system."""
    
    def __init__(self, action_space_size: int = 20):
        """
        Initialize cognitive system.
        
        Args:
            action_space_size: Size of action space
        """
        self.attention = AttentionState()
        self.conceptualizer = Conceptualizer()
        self.planner = NeuralPlanner(action_space_size)
        
        # Cognitive control
        self.goal_manager = {}
        self.task_scheduler = []
        self.working_memory = []
        self.long_term_memory = []
        
        # Processing parameters
        self.processing_speed = 1.0
        self.attention_span = 0.5
        self.selectivity = 0.8
        self.memory_capacity = 5
        
        # State
        self.current_goals = []
        self.current_task = None
        self.cognitive_load = 0.0
        self.mental_effort = 0.0
    
    def process_input(self, sensory_input: Dict[str, Any], world_state: Dict[str, Any] = None):
        """
        Process sensory input through cognitive system.
        
        Args:
            sensory_input: Sensory input data
            world_state: Current world state
        """
        # Extract features from sensory input
        features = self._extract_cognitive_features(sensory_input)
        
        # Update attention
        self._update_attention(features)
        
        # Update concepts
        self._update_concepts(features)
        
        # Plan actions
        self._plan_actions(features, world_state)
        
        # Update cognitive load
        self.cognitive_load = min(1.0, self.cognitive_load + 0.1 * self.processing_speed)
        self.mental_effort = self.cognitive_load * 0.5
    
    def _extract_cognitive_features(self, sensory_input: Dict[str, Any]) -> List[float]:
        """Extract features for cognitive processing."""
        features = []
        
        # Temporal features
        if 'timestamp' in sensory_input:
            features.append(sensory_input['timestamp'] % 1000 / 1000.0)
        
        # Intensity features
        for key in ['vision', 'audio', 'internal']:
            if key in sensory_input:
                data = sensory_input[key]
                if data:
                    features.append(sum(abs(x) for x in data) / len(data))
        
        # Pattern features
        if 'events' in sensory_input:
            event_types = [e.get('type', '') for e in sensory_input['events']]
            features.append(len(set(event_types)) / 10.0)
        
        return features
    
    def _update_attention(self, features: List[float]):
        """Update attention system."""
        # Simple attention update based on features
        attention_target = None
        
        if features:
            # Determine focus target based on features
            max_feature_idx = features.index(max(features))
            
            # Map to meaningful targets
            targets = ['vision', 'audio', 'internal', 'events', 'actions']
            if max_feature_idx < len(targets):
                attention_target = targets[max_feature_idx]
                self.attention.allocate_focus(attention_target, 0.3 + features[max_feature_idx] * 0.7)
        
        # Add distractor if not focused
        if not attention_target and features:
            self.attention.add_distractor('novelty', max(features) * 0.5)
    
    def _update_concepts(self, features: List[float]):
        """Update concept system."""
        # Create concept from features if significant
        if features and max(features) > 0.5:
            data = {
                'features': features,
                'timestamp': time.time(),
                'events': [{'type': f'event_{i}'} for i, f in enumerate(features) if f > 0.5],
                'rewards': [{'value': f} for f in features if f > 0.3],
            }
            
            concept = self.conceptualizer.form_concept(data)
            
            # Activate if relevant
            if concept and concept.activation_level > 0.5:
                self.conceptualizer.activate_concept(concept.name)
    
    def _plan_actions(self, features: List[float], world_state: Dict[str, Any] = None):
        """Plan actions based on features."""
        # Extract action preferences from features
        action_preferences = {}
        
        for i, feature in enumerate(features):
            if feature > 0.5:  # Significant feature
                action_idx = i % self.planner.action_space_size
                action_preferences[action_idx] = feature
        
        # Update planner preferences
        for action_idx, preference in action_preferences.items():
            self.planner.add_action_preference(action_idx, preference)
        
        # Select action
        action_idx = self.planner.select_action()
        
        # Record action
        self.action_history.append(action_idx)
    
    def set_goals(self, goals: List[str]):
        """Set cognitive goals."""
        self.current_goals = goals
        for goal in goals:
            self.goal_manager[goal] = {
                'progress': 0.0,
                'priority': 1.0,
                'satisfaction': 0.0,
                'timestamp': time.time(),
            }
    
    def get_cognitive_state(self) -> Dict[str, Any]:
        """Get current cognitive state."""
        return {
            'attention': {
                'focus_target': self.attention.get_focused_target(),
                'focus_strength': self.attention.get_focus_strength(),
                'attention_span': self.attention.get_attention_span(),
                'focus_distribution': self.attention.get_focus_distribution(),
            },
            'concepts': self.conceptualizer.get_concept_network(),
            'planner': self.planner.get_planning_summary(),
            'cognitive_load': self.cognitive_load,
            'mental_effort': self.mental_effort,
            'current_goals': self.current_goals,
            'working_memory': self.working_memory,
            'long_term_memory': self.long_term_memory,
        }
    
    def reset(self):
        """Reset cognitive system."""
        self.attention.reset()
        self.conceptualizer = Conceptualizer()
        self.planner = NeuralPlanner(self.planner.action_space_size)
        
        self.goal_manager.clear()
        self.task_scheduler.clear()
        self.working_memory.clear()
        self.long_term_memory.clear()
        
        self.current_goals = []
        self.current_task = None
        self.cognitive_load = 0.0
        self.mental_effort = 0.0
    
    def __repr__(self) -> str:
        return f"CognitiveSystem(goals={len(self.current_goals)}, concepts={len(self.conceptualizer.concepts)})"


def create_cognitive_system(action_space_size: int = 20) -> CognitiveSystem:
    """Create cognitive system."""
    return CognitiveSystem(action_space_size)


def create_attentional_system(total_focus: float = 1.0) -> AttentionState:
    """Create attentional system."""
    return AttentionState(total_focus)


def create_conceptualizer() -> Conceptualizer:
    """Create conceptualizer."""
    return Conceptualizer()


def create_neural_planner(action_space_size: int = 20) -> NeuralPlanner:
    """Create neural planner."""
    return NeuralPlanner(action_space_size)


def run_cognitive_simulation(brain, world, agent, steps: int = 100):
    """
    Run simulation with cognitive processing.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of steps
    """
    # Create cognitive system
    cognitive_system = create_cognitive_system()
    
    # Set goals for cognitive system
    cognitive_system.set_goals(['survival', 'exploration', 'learning'])
    
    for step in range(steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through cognitive system
        world_state = {
            'position': (agent.getBrain().getPosition() if hasattr(agent.getBrain(), 'getPosition') else (0, 0)),
            'orientation': (agent.getBrain().getOrientation() if hasattr(agent.getBrain(), 'getOrientation') else 0),
            'action_valid': {i: True for i in range(cognitive_system.planner.action_space_size)},
            'expected_reward': 0.5,  # Default expected reward
            'novelty_level': 0.2,   # Default novelty level
        }
        
        cognitive_system.process_input(percept, world_state)
        
        # Execute planned action
        action_idx = cognitive_system.planner.select_action()
        
        # Map action index to action type
        action_types = list(ActionType)
        if action_idx < len(action_types):
            action = Action(action_types[action_idx])
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Standard brain step
        brain.step(step)
        
        # Process reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return cognitive_system


def analyze_cognitive_performance(cognitive_system: CognitiveSystem) -> Dict[str, Any]:
    """Analyze cognitive performance from cognitive system."""
    state = cognitive_system.get_cognitive_state()
    
    analysis = {
        'attention_analysis': {},
        'concept_analysis': {},
        'planning_analysis': {},
        'cognitive_load': state['cognitive_load'],
        'mental_effort': state['mental_effort'],
    }
    
    # Analyze attention
    attention = state['attention']
    analysis['attention_analysis'] = {
        'focus_target': attention['focus_target'],
        'focus_strength': attention['focus_strength'],
        'attention_span': attention['attention_span'],
        'focus_distribution': attention['focus_distribution'],
    }
    
    # Analyze concepts
    concepts = state['concepts']
    analysis['concept_analysis'] = {
        'concept_count': len(concepts),
        'most_active_concept': max(concepts.items(), key=lambda x: x[1]['activation'], default=(None, {'activation': 0}))[0],
        'avg_concept_strength': sum(c['strength'] for c in concepts.values()) / len(concepts) if concepts else 0,
        'concept_network_density': len([k for k, v in concepts.items() for assoc in v['associations'] if v['associations']]) / max(len(concepts), 1),
    }
    
    # Analyze planning
    planner = state['planner']
    analysis['planning_analysis'] = {
        'action_space_size': planner['action_space_size'],
        'planning_horizon': planner['planning_horizon'],
        'current_preferences': planner['current_preferences'].copy(),
        'current_values': planner['current_values'].copy(),
        'recommended_action': planner['recommended_action'],
        'action_history_length': planner['action_history_length'],
    }
    
    return analysis
