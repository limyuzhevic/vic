"""Neuromodulation and reward systems for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import time
from enum import Enum


class NeuromodulatorType(Enum):
    DOPAMINE = "dopamine"
    SEROTONIN = "serotonin"
    NORADRENALIN = "noradrenaline"
    ACETYLCHOLINE = "acetylcholine"
    OXYTOCIN = "oxytocin"
    ENDORPHIN = "endorphin"


class DopamineSystem:
    """
    High-level dopamine modeling for reward prediction and learning.
    
    This class implements a biologically plausible dopamine system that
    encodes prediction errors and modulates learning based on reward
    expectations.
    """
    
    def __init__(self, learning_rate: float = 0.1, prediction_horizon: int = 10):
        """
        Initialize dopamine system.
        
        Args:
            learning_rate: Dopamine learning rate
            prediction_horizon: Number of time steps for prediction
        """
        self.learning_rate = learning_rate
        self.prediction_horizon = prediction_horizon
        
        # Dopamine state
        self.dopamine_level = 0.0
        self.prediction_error = 0.0
        self.expected_value = 0.0
        
        # History for learning
        self.reward_history = deque(maxlen=100)
        self.prediction_history = deque(maxlen=100)
        self.dopamine_history = deque(maxlen=100)
        
        # Modulation parameters
        self.baseline_dopamine = 0.05
        self.dopamine_sensitivity = 1.0
        self.decay_rate = 0.01
        
        # Learning signals
        self.lsm_phases = {
            "phasic": 0.0,
            "tonic": 0.0,
            "burst": 0.0
        }
        
    def update(self, reward: float, prediction: float) -> Dict[str, Any]:
        """
        Update dopamine system with new reward and prediction.
        
        Args:
            reward: Actual reward received
            prediction: Predicted reward
            
        Returns:
            Dict[str, Any]: Dopamine system update results
        """
        # Calculate prediction error
        self.prediction_error = reward - prediction
        
        # Update expected value
        self.expected_value = self.expected_value * (1 - self.learning_rate) + \
                             prediction * self.learning_rate
        
        # Calculate dopamine signals
        phasic_dopamine = self.prediction_error * self.dopamine_sensitivity
        
        # Tonic dopamine depends on prediction reliability
        prediction_variance = self._calculate_prediction_variance()
        tonic_dopamine = self.baseline_dopamine * np.exp(-prediction_variance)
        
        # Burst dopamine (phasic + tonic)
        burst_dopamine = phasic_dopamine + tonic_dopamine
        
        # Update total dopamine level with decay
        self.dopamine_level = (
            self.dopamine_level * (1 - self.decay_rate) + 
            burst_dopamine * self.learning_rate
        )
        
        # Store history
        self.reward_history.append(reward)
        self.prediction_history.append(prediction)
        self.dopamine_history.append(self.dopamine_level)
        
        # Determine LSM phases
        self._update_lsm_phases()
        
        return {
            "dopamine_level": self.dopamine_level,
            "prediction_error": self.prediction_error,
            "expected_value": self.expected_value,
            "phasic_dopamine": phasic_dopamine,
            "tonic_dopamine": tonic_dopamine,
            "burst_dopamine": burst_dopamine,
            "lsm_phases": self.lsm_phases,
            "learning_signal": self._calculate_learning_signal()
        }
        
    def _calculate_prediction_variance(self) -> float:
        """Calculate prediction variance."""
        if len(self.reward_history) < 2:
            return 0.0
            
        rewards = list(self.reward_history)
        predictions = list(self.prediction_history)
        
        errors = [r - p for r, p in zip(rewards, predictions)]
        return np.var(errors) if errors else 0.0
        
    def _update_lsm_phases(self) -> None:
        """Update limbic system modulation phases."""
        # Simplified LSM phase calculation
        if abs(self.prediction_error) > 0.1:
            self.lsm_phases["phasic"] = 0.8 + 0.2 * np.sign(self.prediction_error)
            self.lsm_phases["tonic"] = 0.3
            self.lsm_phases["burst"] = 1.0
        elif abs(self.prediction_error) > 0.01:
            self.lsm_phases["phasic"] = 0.4 + 0.1 * np.sign(self.prediction_error)
            self.lsm_phases["tonic"] = 0.5
            self.lsm_phases["burst"] = 0.7
        else:
            self.lsm_phases["phasic"] = 0.1
            self.lsm_phases["tonic"] = 0.7
            self.lsm_phases["burst"] = 0.5
            
    def _calculate_learning_signal(self) -> float:
        """Calculate learning signal from dopamine."""
        # Learning signal is proportional to prediction error
        learning_signal = self.prediction_error * self.dopamine_sensitivity
        
        # Scale by overall dopamine level
        if self.dopamine_level > 0:
            learning_signal *= self.dopamine_level / 0.5
            
        return learning_signal
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get dopamine system statistics."""
        if not self.dopamine_history:
            return {"initialized": False}
            
        return {
            "initialized": True,
            "current_dopamine_level": self.dopamine_level,
            "current_prediction_error": self.prediction_error,
            "current_expected_value": self.expected_value,
            "average_dopamine": np.mean(self.dopamine_history),
            "max_dopamine": np.max(self.dopamine_history),
            "min_dopamine": np.min(self.dopamine_history),
            "total_rewards": len(self.reward_history),
            "prediction_accuracy": self._calculate_prediction_accuracy(),
            "lsm_phases": self.lsm_phases,
            "learning_signal_strength": self._calculate_learning_signal()
        }
        
    def _calculate_prediction_accuracy(self) -> float:
        """Calculate prediction accuracy."""
        if len(self.reward_history) < 2:
            return 1.0
            
        rewards = list(self.reward_history)
        predictions = list(self.prediction_history)
        
        # Calculate mean squared error
        errors = [r - p for r, p in zip(rewards, predictions)]
        mse = np.mean([e**2 for e in errors])
        
        # Convert to accuracy score (higher is better)
        return max(0.0, 1.0 - mse)
        
    def reset(self) -> None:
        """Reset dopamine system to initial state."""
        self.dopamine_level = 0.0
        self.prediction_error = 0.0
        self.expected_value = 0.0
        
        self.reward_history.clear()
        self.prediction_history.clear()
        self.dopamine_history.clear()
        
        self.lsm_phases = {"phasic": 0.0, "tonic": 0.0, "burst": 0.0}


class RewardEngine:
    """
    Reward processing and prediction system.
    
    This class implements reward processing mechanisms that integrate
    immediate and delayed rewards, discount future rewards, and update
    value estimates.
    """
    
    def __init__(self, discount_factor: float = 0.9, exploration_bonus: float = 0.1):
        """
        Initialize reward engine.
        
        Args:
            discount_factor: Discount factor for future rewards
            exploration_bonus: Bonus for exploratory actions
        """
        self.discount_factor = discount_factor
        self.exploration_bonus = exploration_bonus
        
        # Reward history
        self.reward_history = deque(maxlen=100)
        self.value_estimates = deque(maxlen=100)
        self.action_values = {}
        
        # Reward processing
        self.max_observed_reward = -float('inf')
        self.min_observed_reward = float('inf')
        self.reward_scale = 1.0
        
        # Prediction error tracking
        self.prediction_errors = deque(maxlen=100)
        self.prediction_accuracies = deque(maxlen=100)
        
    def process_reward(self, reward: float, action: Any = None, 
                      state: Any = None) -> Dict[str, Any]:
        """
        Process a reward signal.
        
        Args:
            reward: Reward value
            action: Action that produced the reward
            state: State before the reward
            
        Returns:
            Dict[str, Any]: Processed reward results
        """
        # Update reward statistics
        self.max_observed_reward = max(self.max_observed_reward, reward)
        self.min_observed_reward = min(self.min_observed_reward, reward)
        
        # Apply reward scaling if needed
        if self.max_observed_reward > self.min_observed_reward:
            scaled_reward = (reward - self.min_observed_reward) / \
                          (self.max_observed_reward - self.min_observed_reward)
        else:
            scaled_reward = 0.5
            
        # Store reward history
        self.reward_history.append(reward)
        
        # Update value estimates
        self._update_value_estimates(scaled_reward, action, state)
        
        # Calculate prediction error
        prediction_error = self._calculate_prediction_error(action, state)
        self.prediction_errors.append(prediction_error)
        
        # Calculate processed reward
        processed_reward = self._calculate_processed_reward(
            scaled_reward, prediction_error, action
        )
        
        return {
            "raw_reward": reward,
            "scaled_reward": scaled_reward,
            "processed_reward": processed_reward,
            "prediction_error": prediction_error,
            "value_estimate": self._get_value_estimate(action),
            "exploration_bonus": self._calculate_exploration_bonus(action),
            "total_reward": processed_reward + self._calculate_exploration_bonus(action)
        }
        
    def _update_value_estimates(self, reward: float, action: Any, state: Any) -> None:
        """Update value estimates using temporal difference learning."""
        # Get current value estimate
        current_value = self._get_value_estimate(action)
        
        # Calculate target value (immediate reward + discounted future value)
        if self.value_estimates:
            future_value = self.value_estimates[-1]
        else:
            future_value = 0.0
            
        target_value = reward + self.discount_factor * future_value
        
        # Update value estimate (simple TD learning)
        if current_value is not None:
            learning_rate = 0.1
            new_value = current_value + learning_rate * (target_value - current_value)
            
            # Update value estimate
            if action is not None:
                self.action_values[action] = new_value
                
            self.value_estimates.append(new_value)
        else:
            self.value_estimates.append(reward)
            
    def _get_value_estimate(self, action: Any) -> Optional[float]:
        """Get value estimate for an action."""
        if action is not None and action in self.action_values:
            return self.action_values[action]
        elif self.value_estimates:
            return self.value_estimates[-1]
        else:
            return None
            
    def _calculate_prediction_error(self, action: Any, state: Any) -> float:
        """Calculate prediction error for reward prediction."""
        if self.value_estimates and self.reward_history:
            predicted_value = self._get_value_estimate(action)
            actual_reward = self.reward_history[-1]
            
            if predicted_value is not None:
                return actual_reward - predicted_value
                
        return 0.0
        
    def _calculate_processed_reward(self, scaled_reward: float, 
                                   prediction_error: float, action: Any) -> float:
        """Calculate final processed reward."""
        # Base reward
        processed = scaled_reward
        
        # Adjust for prediction error
        processed += prediction_error * 0.5
        
        # Adjust for exploration bonus
        if action is not None:
            processed += self._calculate_exploration_bonus(action)
            
        return processed
        
    def _calculate_exploration_bonus(self, action: Any) -> float:
        """Calculate exploration bonus for novel actions."""
        if action is not None:
            # Bonus for actions not seen recently
            action_history = [a for a in self.action_values.keys() if a == action]
            if not action_history:
                return self.exploration_bonus
                
        return 0.0
        
    def predict_future_reward(self, state: Any, steps: int = 10) -> List[float]:
        """
        Predict future rewards for a given state.
        
        Args:
            state: Current state
            steps: Number of steps to predict
            
        Returns:
            List[float]: Predicted rewards for each step
        """
        predictions = []
        current_value = self._get_value_estimate(state)
        
        for step in range(steps):
            if current_value is not None:
                # Simple prediction based on current value estimate
                predicted_reward = current_value * (self.discount_factor ** step)
                predictions.append(predicted_reward)
                
                # Update current value for next step
                current_value = predicted_reward
            else:
                predictions.append(0.0)
                
        return predictions
        
    def get_reward_statistics(self) -> Dict[str, Any]:
        """Get reward processing statistics."""
        if not self.reward_history:
            return {"initialized": False}
            
        return {
            "initialized": True,
            "total_rewards": len(self.reward_history),
            "average_reward": np.mean(self.reward_history),
            "max_reward": np.max(self.reward_history),
            "min_reward": np.min(self.reward_history),
            "reward_std": np.std(self.reward_history),
            "current_value_estimate": self._get_value_estimate(None),
            "number_of_action_values": len(self.action_values),
            "average_prediction_error": np.mean(self.prediction_errors) if self.prediction_errors else 0.0,
            "prediction_accuracy": np.mean(self.prediction_accuracies) if self.prediction_accuracies else 0.0,
            "discount_factor": self.discount_factor,
            "exploration_bonus": self.exploration_bonus
        }
        
    def reset(self) -> None:
        """Reset reward engine to initial state."""
        self.reward_history.clear()
        self.value_estimates.clear()
        self.action_values.clear()
        
        self.max_observed_reward = -float('inf')
        self.min_observed_reward = float('inf')
        self.prediction_errors.clear()
        self.prediction_accuracies.clear()


class PredictionErrorTracker:
    """
    Track prediction errors for learning optimization.
    
    This class monitors prediction errors across different time scales
    and provides insights into learning performance and uncertainty.
    """
    
    def __init__(self, time_window: int = 100):
        """
        Initialize prediction error tracker.
        
        Args:
            time_window: Window size for error analysis
        """
        self.time_window = time_window
        
        # Prediction error history
        self.prediction_errors = deque(maxlen=time_window)
        self.prediction_times = deque(maxlen=time_window)
        
        # Error statistics
        self.error_means = deque(maxlen=10)
        self.error_vars = deque(maxlen=10)
        self.error_slopes = deque(maxlen=10)
        
        # Learning metrics
        self.learning_rates = deque(maxlen=100)
        self.convergence_indicators = deque(maxlen=100)
        
    def record_error(self, error: float, timestamp: float = None) -> None:
        """
        Record a prediction error.
        
        Args:
            error: Prediction error value
            timestamp: Time of error (if None, uses current time)
        """
        if timestamp is None:
            timestamp = time.time()
            
        self.prediction_errors.append(error)
        self.prediction_times.append(timestamp)
        
        # Update statistics
        self._update_statistics()
        
    def _update_statistics(self) -> None:
        """Update error statistics."""
        if len(self.prediction_errors) < 10:
            return
            
        errors = list(self.prediction_errors)
        
        # Calculate mean and variance
        self.error_means.append(np.mean(errors))
        self.error_vars.append(np.var(errors))
        
        # Calculate trend (slope)
        if len(self.prediction_errors) >= 2:
            x = np.arange(len(errors))
            slope = np.polyfit(x, errors, 1)[0]
            self.error_slopes.append(slope)
            
        # Calculate learning rate (change in error)
        if len(self.error_means) >= 2:
            current_mean = self.error_means[-1]
            previous_mean = self.error_means[-2]
            learning_rate = max(0.0, previous_mean - current_mean) / max(current_mean, 0.001)
            self.learning_rates.append(learning_rate)
            
        # Calculate convergence indicator
        if len(self.error_means) >= 5:
            recent_errors = list(self.error_means)[-5:]
            variation = np.std(recent_errors)
            convergence = 1.0 - min(1.0, variation / (np.mean(recent_errors) + 0.001))
            self.convergence_indicators.append(convergence)
            
    def get_error_analysis(self) -> Dict[str, Any]:
        """Get prediction error analysis."""
        if not self.prediction_errors:
            return {"initialized": False}
            
        errors = list(self.prediction_errors)
        
        return {
            "initialized": True,
            "total_errors": len(errors),
            "current_error": errors[-1] if errors else 0.0,
            "mean_error": np.mean(errors),
            "error_std": np.std(errors),
            "error_range": np.max(errors) - np.min(errors) if errors else 0.0,
            "error_skewness": self._calculate_skewness(errors),
            "error_kurtosis": self._calculate_kurtosis(errors),
            "error_trend": self._calculate_error_trend(),
            "learning_rate": np.mean(self.learning_rates) if self.learning_rates else 0.0,
            "convergence": np.mean(self.convergence_indicators) if self.convergence_indicators else 0.0,
            "recent_errors": errors[-10:] if len(errors) >= 10 else errors,
            "error_buckets": self._bucket_errors()
        }
        
    def _calculate_skewness(self, errors: List[float]) -> float:
        """Calculate error skewness."""
        if len(errors) < 3:
            return 0.0
            
        return np.mean([(e - np.mean(errors))**3 for e in errors]) / (np.std(errors)**3 + 1e-6)
        
    def _calculate_kurtosis(self, errors: List[float]) -> float:
        """Calculate error kurtosis."""
        if len(errors) < 4:
            return 0.0
            
        return np.mean([(e - np.mean(errors))**4 for e in errors]) / (np.std(errors)**4 + 1e-6)
        
    def _calculate_error_trend(self) -> Dict[str, Any]:
        """Calculate error trend."""
        if len(self.error_slopes) < 2:
            return {"trend": "stable", "slope": 0.0, "confidence": 0.0}
            
        # Calculate average slope
        avg_slope = np.mean(self.error_slopes)
        
        # Determine trend
        if avg_slope > 0.01:
            trend = "increasing"
        elif avg_slope < -0.01:
            trend = "decreasing"
        else:
            trend = "stable"
            
        # Calculate confidence
        if len(self.error_slopes) >= 5:
            slope_std = np.std(self.error_slopes)
            confidence = max(0.0, 1.0 - slope_std / (abs(avg_slope) + 0.001))
        else:
            confidence = 0.5
            
        return {
            "trend": trend,
            "slope": avg_slope,
            "confidence": confidence
        }
        
    def _bucket_errors(self) -> Dict[str, int]:
        """Bucket errors by magnitude."""
        buckets = {
            "very_small": 0,    # |error| < 0.01
            "small": 0,         # 0.01 <= |error| < 0.1
            "medium": 0,        # 0.1 <= |error| < 0.5
            "large": 0,         # 0.5 <= |error| < 1.0
            "very_large": 0     # |error| >= 1.0
        }
        
        for error in self.prediction_errors:
            abs_error = abs(error)
            if abs_error < 0.01:
                buckets["very_small"] += 1
            elif abs_error < 0.1:
                buckets["small"] += 1
            elif abs_error < 0.5:
                buckets["medium"] += 1
            elif abs_error < 1.0:
                buckets["large"] += 1
            else:
                buckets["very_large"] += 1
                
        return buckets
        
    def reset(self) -> None:
        """Reset prediction error tracker."""
        self.prediction_errors.clear()
        self.prediction_times.clear()
        
        self.error_means.clear()
        self.error_vars.clear()
        self.error_slopes.clear()
        
        self.learning_rates.clear()
        self.convergence_indicators.clear()


class MotivationSystem:
    """
    Integrate motivation signals from multiple sources.
    
    This class integrates motivation signals from dopamine, curiosity,
    reward prediction, and other sources to create a unified motivation
    system for guiding behavior.
    """
    
    def __init__(self, motivation_weights: Dict[str, float] = None):
        """
        Initialize motivation system.
        
        Args:
            motivation_weights: Weights for different motivation sources
        """
        if motivation_weights is None:
            motivation_weights = {
                "dopamine": 0.4,
                "curiosity": 0.3,
                "reward": 0.2,
                "novelty": 0.1
            }
            
        self.motivation_weights = motivation_weights
        
        # Motivation sources
        self.dopamine_source = DopamineSource()
        self.curiosity_source = CuriositySource()
        self.reward_source = RewardSource()
        self.novelty_source = NoveltySource()
        
        # Integrated motivation
        self.motivation_level = 0.0
        self.motivation_history = deque(maxlen=100)
        
        # Goal system
        self.goals = {}
        self.goal_progress = {}
        
    def update_motivation(self, context: Dict[str, Any] = None) -> Dict[str, Any]:
        """
        Update motivation based on current context.
        
        Args:
            context: Current context information
            
        Returns:
            Dict[str, Any]: Updated motivation information
        """
        if context is None:
            context = {}
            
        # Get motivation from different sources
        dopamine_motivation = self.dopamine_source.get_motivation(context)
        curiosity_motivation = self.curiosity_source.get_motivation(context)
        reward_motivation = self.reward_source.get_motivation(context)
        novelty_motivation = self.novelty_source.get_motivation(context)
        
        # Integrate motivations
        motivations = {
            "dopamine": dopamine_motivation,
            "curiosity": curiosity_motivation,
            "reward": reward_motivation,
            "novelty": novelty_motivation
        }
        
        # Calculate weighted motivation
        weighted_motivation = 0.0
        for source, weight in self.motivation_weights.items():
            weighted_motivation += motivations[source] * weight
            
        # Apply goal modulation
        goal_motivation = self._calculate_goal_motivation()
        
        # Final motivation
        self.motivation_level = (weighted_motivation + goal_motivation) / 2.0
        
        # Store in history
        self.motivation_history.append({
            "timestamp": time.time(),
            "weighted_motivation": weighted_motivation,
            "goal_motivation": goal_motivation,
            "final_motivation": self.motivation_level,
            "sources": motivations
        })
        
        return {
            "motivation_level": self.motivation_level,
            "motivation_sources": motivations,
            "weighted_motivation": weighted_motivation,
            "goal_motivation": goal_motivation,
            "motivation_change": self._calculate_motivation_change(),
            "exploration_drive": self._calculate_exploration_drive()
        }
        
    def add_goal(self, goal_id: str, description: str, target_value: float,
                deadline: float = None) -> None:
        """
        Add a goal to the motivation system.
        
        Args:
            goal_id: Goal identifier
            description: Goal description
            target_value: Target value for the goal
            deadline: Deadline for goal completion (timestamp)
        """
        self.goals[goal_id] = {
            "description": description,
            "target_value": target_value,
            "deadline": deadline,
            "current_value": 0.0,
            "achieved": False
        }
        self.goal_progress[goal_id] = 0.0
        
    def update_goal_progress(self, goal_id: str, progress: float) -> None:
        """
        Update progress for a goal.
        
        Args:
            goal_id: Goal identifier
            progress: Progress increment
        """
        if goal_id in self.goal_progress:
            self.goal_progress[goal_id] += progress
            
            # Check if goal is achieved
            if self.goal_progress[goal_id] >= self.goals[goal_id]["target_value"]:
                self.goals[goal_id]["achieved"] = True
                self.goals[goal_id]["current_value"] = self.goals[goal_id]["target_value"]
                
    def get_motivation_statistics(self) -> Dict[str, Any]:
        """Get motivation system statistics."""
        if not self.motivation_history:
            return {"initialized": False}
            
        recent_history = list(self.motivation_history)[-10:]
        
        return {
            "initialized": True,
            "current_motivation": self.motivation_level,
            "average_motivation": np.mean([h["final_motivation"] for h in self.motivation_history]),
            "max_motivation": np.max([h["final_motivation"] for h in self.motivation_history]),
            "min_motivation": np.min([h["final_motivation"] for h in self.motivation_history]),
            "motivation_variance": np.var([h["final_motivation"] for h in self.motivation_history]),
            "motivation_trend": self._calculate_motivation_trend(),
            "active_goals": len([g for g in self.goals.values() if not g["achieved"]]),
            "achieved_goals": len([g for g in self.goals.values() if g["achieved"]]),
            "recent_motivation": [h["final_motivation"] for h in recent_history],
            "source_contributions": self._get_source_contributions(recent_history)
        }
        
    def _calculate_goal_motivation(self) -> float:
        """Calculate motivation from goals."""
        if not self.goal_progress:
            return 0.0
            
        goal_motivations = []
        for goal_id, progress in self.goal_progress.items():
            if not self.goals[goal_id]["achieved"]:
                goal = self.goals[goal_id]
                target = goal["target_value"]
                if target > 0:
                    progress_ratio = min(1.0, progress / target)
                    goal_motivations.append(progress_ratio * (1.0 if goal["deadline"] is None or goal["deadline"] > time.time() else 0.5))
                    
        return np.mean(goal_motivations) if goal_motivations else 0.0
        
    def _calculate_motivation_change(self) -> float:
        """Calculate recent change in motivation."""
        if len(self.motivation_history) < 2:
            return 0.0
            
        recent = list(self.motivation_history)[-5:]
        if len(recent) < 2:
            return 0.0
            
        current = recent[-1]["final_motivation"]
        previous = recent[-2]["final_motivation"]
        
        return current - previous
        
    def _calculate_exploration_drive(self) -> float:
        """Calculate exploration drive from motivation."""
        if len(self.motivation_history) < 5:
            return 0.0
            
        # Calculate exploration drive as motivation variance
        recent_motivations = [h["final_motivation"] for h in self.motivation_history[-5:]]
        exploration_drive = np.var(recent_motivations) * 2.0
        
        return min(1.0, exploration_drive)
        
    def _calculate_motivation_trend(self) -> Dict[str, Any]:
        """Calculate motivation trend."""
        if len(self.motivation_history) < 2:
            return {"trend": "stable", "slope": 0.0}
            
        x = np.arange(len(self.motivation_history))
        y = [h["final_motivation"] for h in self.motivation_history]
        
        slope = np.polyfit(x, y, 1)[0]
        
        if slope > 0.01:
            trend = "increasing"
        elif slope < -0.01:
            trend = "decreasing"
        else:
            trend = "stable"
            
        return {
            "trend": trend,
            "slope": slope
        }
        
    def _get_source_contributions(self, recent_history: List[Dict]) -> Dict[str, float]:
        """Get recent contributions from motivation sources."""
        if not recent_history:
            return {}
            
        sources = {}
        for source in ["dopamine", "curiosity", "reward", "novelty"]:
            if source in recent_history[0]["motivation_sources"]:
                contributions = [h["motivation_sources"][source] for h in recent_history]
                sources[f"{source}_contribution"] = np.mean(contributions)
                
        return sources
        
    def reset(self) -> None:
        """Reset motivation system."""
        self.motivation_level = 0.0
        self.motivation_history.clear()
        
        self.goals.clear()
        self.goal_progress.clear()


class DopamineSource:
    """Dopamine motivation source."""
    
    def get_motivation(self, context: Dict[str, Any]) -> float:
        """Get motivation from dopamine signals."""
        # Simple implementation - would be more complex in real system
        reward = context.get("reward", 0.0)
        prediction_error = context.get("prediction_error", 0.0)
        
        # Combine reward and prediction error
        motivation = min(1.0, max(0.0, (reward + abs(prediction_error)) / 2.0))
        
        return motivation


class CuriositySource:
    """Curiosity motivation source."""
    
    def get_motivation(self, context: Dict[str, Any]) -> float:
        """Get motivation from curiosity signals."""
        # Simple implementation - would be more complex in real system
        novelty = context.get("novelty", 0.0)
        uncertainty = context.get("uncertainty", 0.0)
        
        motivation = min(1.0, novelty * uncertainty * 0.5)
        
        return motivation


class RewardSource:
    """Reward motivation source."""
    
    def get_motivation(self, context: Dict[str, Any]) -> float:
        """Get motivation from reward signals."""
        # Simple implementation - would be more complex in real system
        reward = context.get("reward", 0.0)
        discount = context.get("discount_factor", 0.9)
        
        # Adjust for discounting
        motivation = min(1.0, max(0.0, reward * discount))
        
        return motivation


class NoveltySource:
    """Novelty motivation source."""
    
    def get_motivation(self, context: Dict[str, Any]) -> float:
        """Get motivation from novelty signals."""
        # Simple implementation - would be more complex in real system
        novelty_score = context.get("novelty_score", 0.0)
        
        motivation = min(1.0, novelty_score * 0.8)
        
        return motivation


# Re-export commonly used classes and functions
__all__ = [
    "NeuromodulatorType",
    "DopamineSystem", "RewardEngine", "PredictionErrorTracker", "MotivationSystem"
]