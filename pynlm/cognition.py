"""
Cognitive systems and intelligent processing.

This module contains cognitive systems and advanced processing
features for expert users.
"""

from .core import *

class CognitiveProcessor:
    """
    Advanced cognitive processor for expert users.
    
    Provides concept formation, planning, attention, and other
    cognitive processing capabilities.
    """
    
    def __init__(self):
        self._concepts = {}
        self._plans = []
        self._attention_weights = {}
        self._cognitive_load = 0.0
        self._processing_history = []
        self._reasoning_rules = {}
        
    def form_concept(self, pattern, features, strength=1.0, context=None):
        """
        Form a new concept from patterns and features.
        
        Args:
            pattern: Pattern or input to form concept from
            features: Features of the pattern
            strength: Concept strength (0.0 to 1.0)
            context: Context information
            
        Returns:
            str: Concept identifier
        """
        concept_id = f"concept_{len(self._concepts)}"
        
        self._concepts[concept_id] = {
            'pattern': pattern,
            'features': features,
            'strength': strength,
            'context': context or {},
            'created_at': time.time() if 'time' in globals() else 0.0,
            'usage_count': 0,
            'similarity_scores': {}
        }
        
        # Update similarities with existing concepts
        for existing_id, existing_concept in self._concepts.items():
            if existing_id != concept_id:
                similarity = self._calculate_concept_similarity(
                    features, existing_concept['features']
                )
                self._concepts[concept_id]['similarity_scores'][existing_id] = similarity
                self._concepts[existing_id]['similarity_scores'][concept_id] = similarity
        
        print(f"[INFO] Formed concept '{concept_id}' with strength {strength}")
        return concept_id
        
    def retrieve_concept(self, concept_id, threshold=0.0):
        """
        Retrieve a concept by ID.
        
        Args:
            concept_id: Concept identifier
            threshold: Minimum strength threshold
            
        Returns:
            Concept data or None if not found or below threshold
        """
        if concept_id in self._concepts:
            concept = self._concepts[concept_id]
            if concept['strength'] >= threshold:
                concept['usage_count'] = concept.get('usage_count', 0) + 1
                return concept
        return None
        
    def search_concepts(self, query_features, threshold=0.0):
        """
        Search for concepts matching query features.
        
        Args:
            query_features: Features to search for
            threshold: Minimum similarity threshold
            
        Returns:
            list: Matching concepts
        """
        matches = []
        
        for concept_id, concept in self._concepts.items():
            similarity = self._calculate_concept_similarity(
                query_features, concept['features']
            )
            
            if similarity >= threshold:
                matches.append({
                    'concept_id': concept_id,
                    'concept': concept,
                    'similarity': similarity
                })
        
        # Sort by similarity
        matches.sort(key=lambda x: x['similarity'], reverse=True)
        return matches
        
    def plan_action_sequence(self, goal_state, current_state, constraints=None):
        """
        Plan an action sequence to reach a goal state.
        
        Args:
            goal_state: Target state to reach
            current_state: Current state
            constraints: Planning constraints
            
        Returns:
            list: Planned action sequence
        """
        plan_id = f"plan_{len(self._plans)}"
        
        # Simple planning algorithm
        plan = self._simple_planner(goal_state, current_state, constraints)
        
        self._plans.append({
            'plan_id': plan_id,
            'goal_state': goal_state,
            'current_state': current_state,
            'actions': plan,
            'constraints': constraints or {},
            'created_at': time.time() if 'time' in globals() else 0.0,
            'executed_steps': 0
        })
        
        print(f"[INFO] Created plan '{plan_id}' with {len(plan)} actions")
        return plan_id
        
    def get_plan(self, plan_id):
        """
        Get a specific plan.
        
        Args:
            plan_id: Plan identifier
            
        Returns:
            Plan data or None if not found
        """
        for plan in self._plans:
            if plan['plan_id'] == plan_id:
                return plan
        return None
        
    def execute_plan(self, plan_id, execution_context=None):
        """
        Execute a plan.
        
        Args:
            plan_id: Plan identifier
            execution_context: Execution context
            
        Returns:
            dict: Execution results
        """
        plan = self.get_plan(plan_id)
        if not plan:
            print(f"[ERROR] Plan not found: {plan_id}")
            return {'success': False, 'error': 'Plan not found'}
        
        execution_result = {
            'plan_id': plan_id,
            'success': True,
            'actions_executed': [],
            'errors': [],
            'final_state': plan['current_state'].copy()
        }
        
        for action in plan['actions']:
            try:
                # Execute action
                action_result = self._execute_action(action, execution_context)
                
                execution_result['actions_executed'].append({
                    'action': action,
                    'result': action_result,
                    'step': len(execution_result['actions_executed'])
                })
                
                # Update plan state
                if 'new_state' in action_result:
                    execution_result['final_state'] = action_result['new_state']
                
                # Update plan execution count
                for plan in self._plans:
                    if plan['plan_id'] == plan_id:
                        plan['executed_steps'] += 1
                        break
                
            except Exception as e:
                execution_result['success'] = False
                execution_result['errors'].append({
                    'action': action,
                    'error': str(e),
                    'step': len(execution_result['actions_executed'])
                })
                break
        
        print(f"[INFO] Plan '{plan_id}' executed with "
              f"{len(execution_result['actions_executed'])} actions, "
              f"success: {execution_result['success']}")
        
        return execution_result
        
    def set_attention_weights(self, weights):
        """
        Set attention weights for cognitive processing.
        
        Args:
            weights: Attention weights dictionary
        """
        self._attention_weights.update(weights)
        self._cognitive_load = sum(weights.values()) / len(weights) if weights else 0.0
        
    def get_attention_weights(self):
        """
        Get current attention weights.
        
        Returns:
            dict: Attention weights
        """
        return self._attention_weights.copy()
        
    def get_cognitive_load(self):
        """
        Get current cognitive load.
        
        Returns:
            float: Cognitive load
        """
        return self._cognitive_load
        
    def add_reasoning_rule(self, name, rule_func):
        """
        Add a reasoning rule.
        
        Args:
            name: Rule name
            rule_func: Rule function
        """
        self._reasoning_rules[name] = rule_func
        
    def apply_reasoning_rules(self, input_data):
        """
        Apply reasoning rules to input data.
        
        Args:
            input_data: Input data for reasoning
            
        Returns:
            dict: Reasoning results
        """
        results = {}
        
        for rule_name, rule in self._reasoning_rules.items():
            try:
                result = rule(input_data)
                results[rule_name] = result
            except Exception as e:
                print(f"[WARNING] Reasoning rule '{rule_name}' failed: {e}")
        
        return results
        
    def get_cognitive_statistics(self):
        """
        Get cognitive processing statistics.
        
        Returns:
            dict: Cognitive statistics
        """
        return {
            'concepts': len(self._concepts),
            'plans': len(self._plans),
            'attention_weights': len(self._attention_weights),
            'cognitive_load': self._cognitive_load,
            'processing_history': len(self._processing_history),
            'reasoning_rules': len(self._reasoning_rules),
            'statistics': {
                'avg_concept_strength': sum(c['strength'] for c in self._concepts.values()) / len(self._concepts) if self._concepts else 0,
                'avg_plan_complexity': sum(len(p['actions']) for p in self._plans) / len(self._plans) if self._plans else 0,
                'attention_efficiency': sum(self._attention_weights.values()) / max(1, len(self._attention_weights))
            }
        }
        
    def reset(self):
        """Reset cognitive processor."""
        self._concepts.clear()
        self._plans.clear()
        self._attention_weights.clear()
        self._cognitive_load = 0.0
        self._processing_history.clear()
        print("[INFO] Cognitive processor reset")
        
    def export_cognitive_data(self, filepath):
        """
        Export cognitive data to file.
        
        Args:
            filepath: Path to export file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            data = {
                'concepts': self._concepts,
                'plans': self._plans,
                'attention_weights': self._attention_weights,
                'cognitive_load': self._cognitive_load,
                'processing_history': self._processing_history,
                'reasoning_rules': self._reasoning_rules
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, default=str, indent=2)
                
            print(f"[INFO] Cognitive data exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to export cognitive data: {e}")
            return False
            
    def import_cognitive_data(self, filepath):
        """
        Import cognitive data from file.
        
        Args:
            filepath: Path to import file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'r') as f:
                data = json.load(f)
                
            self._concepts = data.get('concepts', {})
            self._plans = data.get('plans', [])
            self._attention_weights = data.get('attention_weights', {})
            self._cognitive_load = data.get('cognitive_load', 0.0)
            self._processing_history = data.get('processing_history', [])
            self._reasoning_rules = data.get('reasoning_rules', {})
            
            print(f"[INFO] Cognitive data imported from {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to import cognitive data: {e}")
            return False
            
    def _calculate_concept_similarity(self, features1, features2):
        """Calculate similarity between two concept feature sets."""
        if isinstance(features1, (list, tuple)) and isinstance(features2, (list, tuple)):
            if len(features1) == len(features2):
                # Cosine similarity for vectors
                dot_product = sum(f1 * f2 for f1, f2 in zip(features1, features2))
                norm1 = (sum(f1 ** 2 for f1 in features1)) ** 0.5
                norm2 = (sum(f2 ** 2 for f2 in features2)) ** 0.5
                if norm1 * norm2 > 0:
                    return dot_product / (norm1 * norm2)
        elif isinstance(features1, dict) and isinstance(features2, dict):
            # Key-based similarity for dicts
            common_keys = set(features1.keys()) & set(features2.keys())
            if common_keys:
                similarity = len(common_keys) / max(len(features1), len(features2))
                return similarity
        
        # Default similarity
        return 0.0
        
    def _simple_planner(self, goal_state, current_state, constraints):
        """Simple planning algorithm."""
        # Generate a simple action sequence
        actions = []
        
        # Simple example plan
        from .core import ActionType
        
        # Check if we need to move
        current_x = current_state.get('x', 0)
        goal_x = goal_state.get('x', 0)
        if abs(current_x - goal_x) > 0.1:
            if goal_x > current_x:
                actions.append(Action(ActionType.MoveForward))
            else:
                actions.append(Action(ActionType.MoveBackward))
        
        # Check if we need to turn
        current_orientation = current_state.get('orientation', 0)
        goal_orientation = goal_state.get('orientation', 0)
        angle_diff = (goal_orientation - current_orientation) % 360
        if angle_diff > 10 and angle_diff < 350:
            if angle_diff > 180:
                actions.append(Action(ActionType.TurnLeft))
            else:
                actions.append(Action(ActionType.TurnRight))
        
        # Check if we need to interact
        if goal_state.get('interact', False):
            actions.append(Action(ActionType.Interact))
        
        return actions
        
    def _execute_action(self, action, context):
        """Execute a cognitive action."""
        # Default action execution
        return {
            'action': action,
            'success': True,
            'new_state': context.copy() if context else {}
        }
        
    def __str__(self):
        """String representation of cognitive processor."""
        return f"CognitiveProcessor(concepts={len(self._concepts)}, " \
               f"plans={len(self._plans)}, cognitive_load={self._cognitive_load:.2f})"
        
    def __repr__(self):
        """Detailed representation of cognitive processor."""
        return f"CognitiveProcessor(concepts={len(self._concepts)}, " \
               f"plans={len(self._plans)}, attention_weights={len(self._attention_weights)}, " \
               f"cognitive_load={self._cognitive_load:.2f})"
# Export advanced cognition classes and functions
from .cognition import CognitiveProcessor