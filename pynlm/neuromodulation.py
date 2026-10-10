"""
Neuromodulation and learning systems.

This module contains neuromodulation components and advanced learning
systems for expert users.
"""

from .core import *

class NeuromodulationSystem:
    """
    Advanced neuromodulation system for expert users.
    
    Provides dopamine, curiosity, novelty, and prediction error
    neuromodulation with advanced learning algorithms.
    """
    
    def __init__(self):
        self._neuromodulators = {}
        self._learning_rules = {}
        self._prediction_errors = []
        self._curiosity_levels = []
        self._novelty_levels = []
        self._dopamine_levels = []
        self._modulation_history = []
        
    def add_neuromodulator(self, name, neuromodulator):
        """Add a neuromodulator to the system."""
        self._neuromodulators[name] = neuromodulator
        
    def add_learning_rule(self, name, learning_rule):
        """Add a learning rule to the system."""
        self._learning_rules[name] = learning_rule
        
    def process_prediction_error(self, prediction, outcome, error_type="reward"):
        """
        Process prediction error for learning.
        
        Args:
            prediction: Predicted outcome
            outcome: Actual outcome
            error_type: Type of error (reward, novelty, curiosity, etc.)
            
        Returns:
            float: Prediction error signal
        """
        error = outcome - prediction
        self._prediction_errors.append({
            'error': error,
            'prediction': prediction,
            'outcome': outcome,
            'type': error_type,
            'timestamp': time.time() if 'time' in globals() else 0.0
        })
        
        # Apply learning rules
        for rule_name, rule in self._learning_rules.items():
            try:
                rule(error, prediction, outcome, error_type)
            except Exception as e:
                print(f"[WARNING] Learning rule '{rule_name}' failed: {e}")
        
        return error
        
    def update_curiosity(self, environment_state, action, reward):
        """
        Update curiosity level based on environmental exploration.
        
        Args:
            environment_state: Current environment state
            action: Action taken
            reward: Reward received
            
        Returns:
            float: New curiosity level
        """
        # Calculate curiosity based on information gain
        novelty = self._calculate_novelty(environment_state, action, reward)
        
        # Update curiosity level
        curiosity_change = novelty * 0.1  # Simple curiosity update
        new_curiosity = min(1.0, curiosity_change)
        
        self._curiosity_levels.append(new_curiosity)
        self._novelty_levels.append(novelty)
        
        return new_curiosity
        
    def update_novelty(self, stimulus, context=None):
        """
        Update novelty detection for new stimuli.
        
        Args:
            stimulus: Stimulus to evaluate for novelty
            context: Contextual information for novelty detection
            
        Returns:
            float: Novelty level
        """
        # Simple novelty calculation based on stimulus statistics
        if not hasattr(self, '_novelty_history'):
            self._novelty_history = []
        
        if isinstance(stimulus, (list, tuple, dict)):
            # Vector-based novelty
            stimulus_str = str(stimulus)
        else:
            stimulus_str = str(stimulus)
        
        # Calculate novelty as deviation from recent stimuli
        recent_stimuli = self._novelty_history[-10:] if len(self._novelty_history) >= 10 else self._novelty_history
        
        if recent_stimuli:
            # Simple distance calculation
            import random
            novelty = random.uniform(0.1, 0.9)  # Simplified novelty
        else:
            novelty = 1.0  # First stimulus is always novel
        
        self._novelty_levels.append(novelty)
        self._novelty_history.append(stimulus_str)
        
        # Limit history
        if len(self._novelty_history) > 1000:
            self._novelty_history.pop(0)
        
        return novelty
        
    def update_dopamine(self, reward_prediction_error, action_utility, learning_rate=0.01):
        """
        Update dopamine level based on reward prediction error.
        
        Args:
            reward_prediction_error: Reward prediction error signal
            action_utility: Current action utility
            learning_rate: Learning rate for dopamine update
            
        Returns:
            float: New dopamine level
        """
        # Dopamine update based on prediction error
        dopamine_change = learning_rate * reward_prediction_error
        new_dopamine = min(1.0, max(0.0, dopamine_change))
        
        self._dopamine_levels.append(new_dopamine)
        
        return new_dopamine
        
    def get_modulation_summary(self):
        """
        Get comprehensive neuromodulation summary.
        
        Returns:
            dict: Neuromodulation summary
        """
        return {
            'neuromodulators': list(self._neuromodulators.keys()),
            'learning_rules': list(self._learning_rules.keys()),
            'prediction_errors': len(self._prediction_errors),
            'curiosity_levels': len(self._curiosity_levels),
            'novelty_levels': len(self._novelty_levels),
            'dopamine_levels': len(self._dopamine_levels),
            'modulation_history': len(self._modulation_history),
            'statistics': {
                'avg_curiosity': sum(self._curiosity_levels) / len(self._curiosity_levels) if self._curiosity_levels else 0,
                'avg_novelty': sum(self._novelty_levels) / len(self._novelty_levels) if self._novelty_levels else 0,
                'avg_dopamine': sum(self._dopamine_levels) / len(self._dopamine_levels) if self._dopamine_levels else 0,
                'avg_prediction_error': sum(e['error'] for e in self._prediction_errors) / len(self._prediction_errors) if self._prediction_errors else 0
            }
        }
        
    def get_current_modulation_state(self):
        """
        Get current neuromodulation state.
        
        Returns:
            dict: Current modulation state
        """
        return {
            'curiosity': self._curiosity_levels[-1] if self._curiosity_levels else 0.0,
            'novelty': self._novelty_levels[-1] if self._novelty_levels else 0.0,
            'dopamine': self._dopamine_levels[-1] if self._dopamine_levels else 0.0,
            'prediction_error': self._prediction_errors[-1]['error'] if self._prediction_errors else 0.0,
            'learning_rules_active': len(self._learning_rules)
        }
        
    def reset(self):
        """Reset neuromodulation system."""
        self._neuromodulators.clear()
        self._learning_rules.clear()
        self._prediction_errors.clear()
        self._curiosity_levels.clear()
        self._novelty_levels.clear()
        self._dopamine_levels.clear()
        self._modulation_history.clear()
        print("[INFO] Neuromodulation system reset")
        
    def export_modulation_data(self, filepath):
        """
        Export modulation data to file.
        
        Args:
            filepath: Path to export file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            data = {
                'neuromodulators': self._neuromodulators,
                'learning_rules': self._learning_rules,
                'prediction_errors': self._prediction_errors,
                'curiosity_levels': self._curiosity_levels,
                'novelty_levels': self._novelty_levels,
                'dopamine_levels': self._dopamine_levels,
                'modulation_history': self._modulation_history
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, default=str)
                
            print(f"[INFO] Neuromodulation data exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to export modulation data: {e}")
            return False
            
    def import_modulation_data(self, filepath):
        """
        Import modulation data from file.
        
        Args:
            filepath: Path to import file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'r') as f:
                data = json.load(f)
                
            self._neuromodulators = data.get('neuromodulators', {})
            self._learning_rules = data.get('learning_rules', {})
            self._prediction_errors = data.get('prediction_errors', [])
            self._curiosity_levels = data.get('curiosity_levels', [])
            self._novelty_levels = data.get('novelty_levels', [])
            self._dopamine_levels = data.get('dopamine_levels', [])
            self._modulation_history = data.get('modulation_history', [])
            
            print(f"[INFO] Neuromodulation data imported from {filepath}")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to import modulation data: {e}")
            return False
            
    def _calculate_novelty(self, stimulus, action, reward):
        """Calculate novelty score for stimulus."""
        # Simple novelty calculation
        import random
        return random.uniform(0.1, 0.9)
        
    def __str__(self):
        """String representation of neuromodulation system."""
        return f"NeuromodulationSystem(neuromodulators={len(self._neuromodulators)}, " \
               f"learning_rules={len(self._learning_rules)})"
        
    def __repr__(self):
        """Detailed representation of neuromodulation system."""
        return f"NeuromodulationSystem(neuromodulators={len(self._neuromodulators)}, " \
               f"learning_rules={len(self._learning_rules)}, " \
               f"prediction_errors={len(self._prediction_errors)})"
# Export advanced neuromodulation classes and functions
from .neuromodulation import NeuromodulationSystem